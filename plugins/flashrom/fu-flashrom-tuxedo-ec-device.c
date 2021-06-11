/*
 * Copyright (C) 2021 Daniel Campello <campello@chromium.org>
 * Copyright (C) 2021 Michał Kopeć <michal.kopec@3mdeb.com>
 *
 * SPDX-License-Identifier: LGPL-2.1+
 */

#include "config.h"

#include "fu-plugin-vfuncs.h"
#include "fu-flashrom-device.h"
#include "fu-flashrom-tuxedo-ec-device.h"

#include <libflashrom.h>

struct _FuFlashromTuxedoEcDevice {
	FuFlashromDevice		 parent_instance;
};

G_DEFINE_TYPE (FuFlashromTuxedoEcDevice, fu_flashrom_tuxedo_ec_device,
	       FU_TYPE_FLASHROM_DEVICE)

static void
fu_flashrom_tuxedo_ec_device_init (FuFlashromTuxedoEcDevice *self)
{
	fu_device_add_flag (FU_DEVICE (self), FWUPD_DEVICE_FLAG_INTERNAL);
	fu_device_add_flag (FU_DEVICE (self), FWUPD_DEVICE_FLAG_UPDATABLE);
	fu_device_add_flag (FU_DEVICE (self), FWUPD_DEVICE_FLAG_NEEDS_REBOOT);
	fu_device_add_flag (FU_DEVICE (self), FWUPD_DEVICE_FLAG_REQUIRE_AC);
	fu_device_add_instance_id (FU_DEVICE (self), "embedded-controller-firmware");
	fu_device_add_internal_flag (FU_DEVICE (self), FU_DEVICE_INTERNAL_FLAG_ENSURE_SEMVER);
	fu_device_set_physical_id (FU_DEVICE (self), "flashrom");
	fu_device_set_logical_id (FU_DEVICE (self), "tuxec");
	fu_device_set_version_format (FU_DEVICE (self), FWUPD_VERSION_FORMAT_TRIPLET);
	fu_device_add_icon (FU_DEVICE (self), "computer");
}

fu_flashrom_lspcon_i2c_spi_device_probe (FuDevice *device, GError **error)
{
	FuFlashromTuxedoEcDevice *self = FU_FLASHROM_TUXEDO_EC_DEVICE (device);
	FuFlashromDevice *flashrom_device = FU_FLASHROM_DEVICE (device);
	FuDeviceClass *klass =
		FU_DEVICE_CLASS (fu_flashrom_tuxedo_ec_device_parent_class);

	/* FuFlashromDevice->probe */
	if (!klass->probe (device, error))
		return FALSE;

	if (g_strcmp0 (fu_flashrom_device_get_programmer_name (flashrom_device),
		       "tuxedo") != 0) {
		g_set_error_literal (error,
				     FWUPD_ERROR,
				     FWUPD_ERROR_NOT_SUPPORTED,
				     "invalid programmer");
		return FALSE;
	}
}

static gboolean
fu_flashrom_tuxedo_ec_device_set_version (FuDevice *device, GError **error)
{
	FuContext *ctx = fu_device_get_context (device);
	const gchar *version_major;
	const gchar *version_minor;

	/* The Tuxedo SMBIOS only gives us component EC FW versions */
	version_major = fu_context_get_hwid_value (ctx, FU_HWIDS_KEY_FIRMWARE_MAJOR_RELEASE);
	version_minor = fu_context_get_hwid_value (ctx, FU_HWIDS_KEY_FIRMWARE_MINOR_RELEASE);
	if (version_major != NULL && version_minor != NULL) {
		g_autofree gchar *tmp = g_strdup_printf ("1.%s.%s", /* Verify if correct */
							 version_major,
							 version_minor);
		fu_device_set_version (device, tmp);
		return TRUE;
	}

	g_set_error (error, FWUPD_ERROR, FWUPD_ERROR_BROKEN_SYSTEM,
				"Could not determine EC firmware verion");
	return FALSE;
}

static gboolean
fu_flashrom_tuxedo_ec_device_prepare (FuDevice *device,
				     FwupdInstallFlags flags,
				     GError **error)
{
	g_autofree gchar *firmware_orig = NULL;
	g_autofree gchar *localstatedir = NULL;
	g_autofree gchar *basename = NULL;

	/* if the original firmware doesn't exist, grab it now */
	basename = g_strdup_printf ("flashrom-%s.bin", fu_device_get_id (device));
	localstatedir = fu_common_get_path (FU_PATH_KIND_LOCALSTATEDIR_PKG);
	firmware_orig = g_build_filename (localstatedir, "builder", basename, NULL);
	if (!fu_common_mkdir_parent (firmware_orig, error))
		return FALSE;
	if (!g_file_test (firmware_orig, G_FILE_TEST_EXISTS)) {
		struct flashrom_flashctx *flashctx = fu_flashrom_device_get_flashctx (parent);
		gsize flash_size = fu_flashrom_device_get_flash_size (parent);
		g_autofree guint8 *newcontents = g_malloc0 (flash_size);
		g_autoptr(GBytes) buf = NULL;

		fu_device_set_status (device, FWUPD_STATUS_DEVICE_READ);
		if (flashrom_image_read (flashctx, newcontents, flash_size)) {
			g_set_error_literal (error,
					     FWUPD_ERROR,
					     FWUPD_ERROR_READ,
					     "failed to back up original firmware");
			return FALSE;
		}
		buf = g_bytes_new_static (newcontents, flash_size);
		if (!fu_common_set_contents_bytes (firmware_orig, buf, error))
			return FALSE;
	}

	return TRUE;
}

static gboolean
fu_flashrom_tuxedo_ec_device_write_firmware (FuDevice *device,
					    FuFirmware *firmware,
					    FwupdInstallFlags flags,
					    GError **error)
{
	FuFlashromDevice *parent = FU_FLASHROM_DEVICE (device);
	struct flashrom_flashctx *flashctx = fu_flashrom_device_get_flashctx (parent);
	gsize flash_size = fu_flashrom_device_get_flash_size (parent);
	struct flashrom_layout *layout;
	gsize sz = 0;
	gint rc;
	const guint8 *buf;
	g_autoptr(GBytes) blob_fw = fu_firmware_get_bytes (firmware, error);
	if (blob_fw == NULL)
		return FALSE;

	buf = g_bytes_get_data (blob_fw, &sz);

	if (flashrom_layout_read_from_ifd (&layout, flashctx, NULL, 0)) {
		g_set_error_literal (error,
				     FWUPD_ERROR,
				     FWUPD_ERROR_READ,
				     "failed to read layout from Intel ICH descriptor");
		return FALSE;
	}

	/* include bios region for safety reasons */
	if (flashrom_layout_include_region (layout, "bios")) {
		g_set_error_literal (error,
				     FWUPD_ERROR,
				     FWUPD_ERROR_NOT_SUPPORTED,
				     "invalid region name");
		return FALSE;
	}

	/* write region */
	flashrom_layout_set (flashctx, layout);
	if (sz != flash_size) {
		g_set_error (error,
			     FWUPD_ERROR,
			     FWUPD_ERROR_NOT_SUPPORTED,
			     "invalid image size 0x%x, expected 0x%x",
			     (guint) sz, (guint) flash_size);
		return FALSE;
	}

	fu_device_set_status (device, FWUPD_STATUS_DEVICE_WRITE);
	fu_device_set_progress (device, 0); /* urgh */
	rc = flashrom_image_write (flashctx, (void *) buf, sz, NULL /* refbuffer */);
	if (rc != 0) {
		g_set_error (error,
			     FWUPD_ERROR,
			     FWUPD_ERROR_WRITE,
			     "image write failed, err=%i", rc);
		return FALSE;
	}

	fu_device_set_status (device, FWUPD_STATUS_DEVICE_VERIFY);
	if (flashrom_image_verify (flashctx, (void *) buf, sz)) {
		g_set_error (error,
			     FWUPD_ERROR,
			     FWUPD_ERROR_WRITE,
			     "image verify failed");
		return FALSE;
	}
	flashrom_layout_release (layout);

	/* success */
	return TRUE;
}

static void
fu_flashrom_tuxedo_ec_device_class_init (FuFlashromTuxedoEcDeviceClass *klass)
{
	FuDeviceClass *klass_device = FU_DEVICE_CLASS (klass);
	klass_device->prepare = fu_flashrom_tuxedo_ec_device_prepare;
	klass_device->write_firmware = fu_flashrom_tuxedo_ec_device_write_firmware;
	klass_device->reload = fu_flashrom_tuxedo_ec_device_set_version;
}
