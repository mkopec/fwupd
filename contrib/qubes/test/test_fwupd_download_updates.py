#!/usr/bin/python3
#
# The Qubes OS Project, http://www.qubes-os.org
#
# Copyright (C) 2021  Norbert Kamiński  <norbert.kaminski@3mdeb.com>
#
# SPDX-License-Identifier: LGPL-2.1+
#

import unittest
import platform
import os
import fwupd_download_updates

FWUPD_VM_DIR = "/home/user/.cache/fwupd"
FWUPD_VM_UPDATES_DIR = os.path.join(FWUPD_VM_DIR, "updates")
FWUPD_VM_METADATA_DIR = os.path.join(FWUPD_VM_DIR, "metadata")
FWUPD_VM_METADATA_SIGNATURE = os.path.join(
    FWUPD_VM_METADATA_DIR,
    "firmware.xml.gz.asc"
)
FWUPD_VM_METADATA_FILE = os.path.join(
    FWUPD_VM_METADATA_DIR,
    "firmware.xml.gz"
)
FWUPD_VM_METADATA_JCAT = os.path.join(
    FWUPD_VM_METADATA_DIR,
    "firmware.xml.gz.jcat"
)


class TestFwupdDownloadUpdates(unittest.TestCase):
    @unittest.skipUnless('qubes' in platform.release(), "Requires Qubes OS")
    def test_download_metadata(self):
        dn = fwupd_download_updates.DownloadData()
        dn.download_metadata()
        self.assertTrue(os.path.isfile(FWUPD_VM_METADATA_SIGNATURE))
        self.assertTrue(os.path.isfile(FWUPD_VM_METADATA_FILE))
        self.assertTrue(os.path.isfile(FWUPD_VM_METADATA_JCAT))

    @unittest.skipUnless('qubes' in platform.release(), "Requires Qubes OS")
    def test_download_updates(self):
        dn = fwupd_download_updates.DownloadData()
        dn.download_updates(
            "https://fwupd.org/downloads/0a29848de74d26348bc5a6e24fc9f03778eddf0e-hughski-colorhug2-2.0.7.cab",
            "80bddeb898cda5b87d9837e13a9ace19846053bf"
        )
        update_path = os.path.join(
            FWUPD_VM_UPDATES_DIR,
            "0a29848de74d26348bc5a6e24fc9f03778eddf0e-hughski-colorhug2-2.0.7"
        )
        self.assertTrue(os.path.exists(update_path))

    @unittest.skipUnless('qubes' in platform.release(), "Requires Qubes OS")
    def test_check_shasum(self):
        dn = fwupd_download_updates.DownloadData()
        dn.download_updates(
            "https://fwupd.org/downloads/0a29848de74d26348bc5a6e24fc9f03778eddf0e-hughski-colorhug2-2.0.7.cab",
            "80bddeb898cda5b87d9837e13a9ace19846053bf"
        )
        update_path = os.path.join(
            FWUPD_VM_UPDATES_DIR,
            "0a29848de74d26348bc5a6e24fc9f03778eddf0e-hughski-colorhug2-2.0.7"
        )
        try:
            dn._check_shasum(
                update_path,
                "80bddeb898cda5b87d9837e13a9ace19846053bf"
            )
        except ValueError:
            return
        self.fail("Computed checksum should not match.")


if __name__ == '__main__':
    unittest.main()
