contrib/qubes: Add Qubes wrapper source and create packages

This patch is adding the fwupd wrapper for Qubes.
The wrapper provides fwupd functionalities for Qubes R4.1.
It creates three packages (two RPMs and one Debian package):
fwupd-qubes-dom0 (RPM)
fwupd-qubes-vm (RPM)
fwupd-qubes-vm-whonix (deb)
More information about the wrapper could be found in the
contrib/qubes/README.md

Firmware resigning solved the problem. I'm using the `jcat-tool verify` command.
Indeed, this information is available in the new metadata, but there is one
more problem. fwupdagent provides a checksum list that contains only one
element -  SHA1. I'm using the fwupdagent to parse information about possible
updates and available devices.
