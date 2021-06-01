/*
 * Copyright (C) 2021 Richard Hughes <richard@hughsie.com>
 * Copyright (C) 2021 Norbert Kamiński <norbert.kaminski@3mdeb.com>
 *
 * SPDX-License-Identifier: LGPL-2.1+
 */

#include "config.h"

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <efivar.h>

#include "fu-common.h"
#include "fu-efivar.h"

#include "fwupd-error.h"


/**
 * fu_efivar_supported:
 * @error: #GError
 *
 * Determines if the kernel supports EFI variables
 *
 * Returns: %TRUE on success
 *
 * Since: 1.6.1
 **/
gboolean
fu_efivar_supported (GError **error)
{
	if (efi_variables_supported () == 0)
		return FALSE;
	return TRUE;
}

/**
 * fu_efivar_space_used:
 * @error: (nullable): optional return location for an error
 *
 * Gets the total size used by all EFI variables. This may be less than the size reported by the
 * kernel as some (hopefully small) variables are hidden from userspace.
 *
 * Returns: total allocated size of all visible variables, or %G_MAXUINT64 on error
 *
 * Since: 1.5.1
 **/
guint64
fu_efivar_space_used (GError **error)
{
	guint64 total = 0;

	g_return_val_if_fail (error == NULL || *error == NULL, G_MAXUINT64);

	/* stat each variable */


	/* success */
	return total;
}


/**
 * fu_efivar_delete:
 * @guid: Globally unique identifier
 * @name: Variable name
 * @error: #GError
 *
 * Removes a variable from NVRAM
 *
 * Returns: %TRUE on success
 *
 * Since: 1.4.0
 **/
gboolean
fu_efivar_delete (const gchar *guid, const gchar *name, GError **error)
{
	g_autofree gchar *fn = NULL;
	g_autoptr(GFile) file = NULL;

	g_return_val_if_fail (guid != NULL, FALSE);
	g_return_val_if_fail (name != NULL, FALSE);
	g_return_val_if_fail (error == NULL || *error == NULL, FALSE);

	fn = fu_efivar_get_filename (guid, name);
	file = g_file_new_for_path (fn);
	if (!g_file_query_exists (file, NULL))
		return TRUE;
	if (!fu_efivar_set_immutable (fn, FALSE, NULL, error)) {
		g_prefix_error (error, "failed to set %s as mutable: ", fn);
		return FALSE;
	}
	return g_file_delete (file, NULL, error);
}

/**
 * fu_efivar_get_data:
 * @guid: Globally unique identifier
 * @name: Variable name
 * @data: Data to set
 * @data_sz: size of data
 * @attr: Attributes
 * @error: (nullable): optional return location for an error
 *
 * Gets the data from a UEFI variable in NVRAM
 *
 * Returns: %TRUE on success
 *
 * Since: 1.4.0
 **/
gboolean
fu_efivar_get_data (const gchar *guid, const gchar *name, guint8 **data,
		       gsize *data_sz, guint32 *attr, GError **error)
{
#ifndef _WIN32
	gssize attr_sz;
	gssize data_sz_tmp;
	guint32 attr_tmp;
	guint64 sz;
	g_autofree gchar *fn = NULL;
	g_autoptr(GFile) file = NULL;
	g_autoptr(GFileInfo) info = NULL;
	g_autoptr(GInputStream) istr = NULL;

	g_return_val_if_fail (guid != NULL, FALSE);
	g_return_val_if_fail (name != NULL, FALSE);
	g_return_val_if_fail (error == NULL || *error == NULL, FALSE);

	/* open file as stream */
	fn = fu_efivar_get_filename (guid, name);
	file = g_file_new_for_path (fn);
	istr = G_INPUT_STREAM (g_file_read (file, NULL, error));
	if (istr == NULL)
		return FALSE;
	info = g_file_input_stream_query_info (G_FILE_INPUT_STREAM (istr),
					       G_FILE_ATTRIBUTE_STANDARD_SIZE,
					       NULL, error);
	if (info == NULL) {
		g_prefix_error (error, "failed to get stream info: ");
		return FALSE;
	}

	/* get total stream size */
	sz = g_file_info_get_attribute_uint64 (info, G_FILE_ATTRIBUTE_STANDARD_SIZE);
	if (sz < 4) {
		g_set_error (error,
			     G_IO_ERROR,
			     G_IO_ERROR_INVALID_DATA,
			     "efivars file too small: %" G_GUINT64_FORMAT, sz);
		return FALSE;
	}

	/* read out the attributes */
	attr_sz = g_input_stream_read (istr, &attr_tmp, sizeof(attr_tmp), NULL, error);
	if (attr_sz == -1) {
		g_prefix_error (error, "failed to read attr: ");
		return FALSE;
	}
	if (attr != NULL)
		*attr = attr_tmp;

	/* read out the data */
	data_sz_tmp = sz - sizeof(attr_tmp);
	if (data_sz != NULL)
		*data_sz = data_sz_tmp;
	if (data != NULL) {
		g_autofree guint8 *data_tmp = g_malloc0 (data_sz_tmp);
		if (!g_input_stream_read_all (istr, data_tmp, data_sz_tmp,
					      NULL, NULL, error)) {
			g_prefix_error (error, "failed to read data: ");
			return FALSE;
		}
		*data = g_steal_pointer (&data_tmp);
	}
	return TRUE;
#else
	g_set_error_literal (error,
			     FWUPD_ERROR,
			     FWUPD_ERROR_NOT_SUPPORTED,
			     "efivarfs not currently supported on Windows");
	return FALSE;
#endif
}

/**
 * fu_efivar_secure_boot_enabled_full:
 * @error: (nullable): optional return location for an error
 *
 * Determines if secure boot was enabled
 *
 * Returns: %TRUE on success
 *
 * Since: 1.5.0
 **/
gboolean
fu_efivar_secure_boot_enabled_full (GError **error)
{
	gsize data_size = 0;
	g_autofree guint8 *data = NULL;

	g_return_val_if_fail (error == NULL || *error == NULL, FALSE);

	if (!fu_efivar_get_data (FU_EFIVAR_GUID_EFI_GLOBAL, "SecureBoot",
				 &data, &data_size, NULL, NULL)) {
		g_set_error_literal (error,
				     FWUPD_ERROR,
				     FWUPD_ERROR_NOT_SUPPORTED,
				     "SecureBoot is not available");
		return FALSE;
	}
	if (data_size >= 1 && data[0] & 1)
		return TRUE;

	/* available, but not enabled */
	g_set_error_literal (error,
			     FWUPD_ERROR,
			     FWUPD_ERROR_NOT_FOUND,
			     "SecureBoot is not enabled");
	return FALSE;
}
