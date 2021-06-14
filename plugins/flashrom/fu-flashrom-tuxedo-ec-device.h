/*
 * Copyright (C) 2021 Daniel Campello <campello@chromium.org>
 * Copyright (C) 2021 Michał Kopeć <michal.kopec@3mdeb.com>
 *
 * SPDX-License-Identifier: LGPL-2.1+
 */

#pragma once

#include "fu-flashrom-device.h"

#define FU_TYPE_FLASHROM_TUXEDO_EC_DEVICE (fu_flashrom_tuxedo_ec_device_get_type ())
G_DECLARE_FINAL_TYPE (FuFlashromTuxedoEcDevice, fu_flashrom_tuxedo_ec_device, FU,
		      FLASHROM_TUXEDO_EC_DEVICE, FuFlashromDevice)

FuDevice	*fu_flashrom_internal_device_new			(void);