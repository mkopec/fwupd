# qubes-fwupd

fwupd wrapper for QubesOS

## Table of Contents

* [Requirements](#Requirements)
* [Usage](#Usage)
* [Installation](#Installation)
* [Testing](#Testing)
* [Whonix support](doc/whonix.md)
* [UEFI capsule update](doc/uefi_capsule_update.md)
* [Heads update](doc/heads_update.md)

## OS Requirements

**Operating System:** Qubes OS R4.1 or Qubes OS R4.0

**Admin VM (dom0):** Fedora 25 or higher

**Template VM:** Fedora 30

**Whonix VM:** whonix-gw-15

**fwupd version - dom0:** 0.9.5 or higher

**fwupd version - VMs:** 1.2.6 or higher

qubes-fwupd does not support dom0 updates and downgrades for fwupd 0.9.5 and
older. Use sys-usb to update external devices.

## Usage

```
==========================================================================================
Usage:
==========================================================================================
        Command:                        qubes-fwupdmgr [OPTION…][FLAG..]
        Example:                        qubes-fwupdmgr refresh --whonix --url=<url>

Options:
==========================================================================================
        get-devices:                    Get all devices that support firmware updates
        get-updates:                    Get the list of updates for connected hardware
        refresh:                        Refresh metadata from remote server
        update:                         Update chosen device to latest firmware version
        update-heads:                   Updates heads firmware to the latest version
        downgrade:                      Downgrade chosen device to chosen firmware version
        clean:                          Delete all cached update files

Flags:
==========================================================================================
        --whonix:                       Download firmware updates via Tor
        --device:                       Specify device for heads update (default - x230)
        --url:                          Address of the custom metadata remote server

Help:
==========================================================================================
        -h --help:                      Show help options
```

## Installation

For development purpose:

1. Build the package for fedora and debian as it is shown in the contrib
[README](../README.md).
2. The build artifacts are placed in `dist` directory:
  -- dom0 package - `dist/fwupd-qubes-dom0-<ver>-0.1alpha.fc32.x86_64.rpm`
  -- vm package - `dist/fwupd-qubes-vm-<ver>-0.1alpha.fc32.x86_64.rpm`
  -- whonix package - `dist/fwupd-qubes-vm-whonix-<ver>_amd64.deb`

3. Copy packages to the Qubes OS.
4. Move the `fwupd-qubes-vm-<ver>-0.1alpha.fc32.x86_64.rpm` to the Fedora 32
template VM (replace `<ver>` with the current version)

```
$ qvm-copy fwupd-qubes-vm-<ver>-0.1alpha.fc32.x86_64.rpm
```

5. Install package dependencies

```
# dnf install cabextract fwudp
```

6. Run terminal in the template VM and go to
`~/QubesIncoming/<qubes-builderVM>`. Compare sha sums of the package in
TemplateVM and qubes-builder VM. If they match, install the package:

```
# rpm -U fwupd-qubes-vm-<ver>-0.1alpha.fc32.x86_64.rpm
```

7. Shutdown TemplateVM

8. Run whonix-gw-15 and copy whonix a package from qubes builder VM

```
$ qvm-copy fwupd-qubes-vm-whonix-<ver>_amd64.deb
```

9. Install dependencies

```
# apt install cabextract fwudp
```

10. Run terminal in the whonix-gw-15 and go to `~/QubesIncoming/qubes-builder`.
Compare sha sums of the package in TemplateVM and qubes-builder VM. If they
match, install the package:

```
# dpkg -i fwupd-qubes-vm-whonix-<ver>_amd64.deb
```

11. Shutdown whonix-gw-15

12. Run dom0 terminal in the dom0 and copy package

```
$ qvm-run --pass-io <qubes-builder-vm-name> \
'cat <qubes-builder-repo-path>/qubes-src/fwupd/pkgs/dom0-fc32/x86_64/fwupd-qubes-dom0-<ver>-0.1alpha.fc32.x86_64.rpm' > \
fwupd-qubes-dom0-<ver>-0.1alpha.fc32.x86_64.rpm
```

13. Install package dependencies

```
# qubes-dom0-update cabextract fwudp python36
```

14. Make sure that sys-firewall, sys-whonix, and sys-usb (if exists) are running.

15. Compare the sha sums of the package in dom0 and qubes-builder VM.
If they match, install the package:

```
# rpm -U qubes-fwupd-dom0-0.2.0-1.fc32.x86_64.rpm
```

16. Reboot system (or reboot sys-firewall, sys-whonix, and sys-usb)

17. Run the tests to verify the installation process

## Testing

### Outside the Qubes OS

A test case covers the whole qubes_fwupdmgr script. It could be run outside the
Qubes OS. If the requirements of a single test are not met, it will be omitted.
To run the tests, move to the repo directory and type the following:

```
$ python3 -m unittest -v test.test_qubes_fwupdmgr

test_clean_cache (test.test_qubes_fwupdmgr.TestQubesFwupdmgr) ... ok
test_downgrade_firmware (test.test_qubes_fwupdmgr.TestQubesFwupdmgr) ... skipped 'Required device not connected'
test_download_firmware_updates (test.test_qubes_fwupdmgr.TestQubesFwupdmgr) ... skipped 'requires Qubes OS'
test_download_metadata (test.test_qubes_fwupdmgr.TestQubesFwupdmgr) ... skipped 'requires Qubes OS'
test_get_devices (test.test_qubes_fwupdmgr.TestQubesFwupdmgr) ... skipped 'requires Qubes OS'
test_get_devices_qubes (test.test_qubes_fwupdmgr.TestQubesFwupdmgr) ... skipped 'requires Qubes OS'
test_get_updates (test.test_qubes_fwupdmgr.TestQubesFwupdmgr) ... skipped 'requires Qubes OS'
test_get_updates_qubes (test.test_qubes_fwupdmgr.TestQubesFwupdmgr) ... skipped 'requires Qubes OS'
test_help (test.test_qubes_fwupdmgr.TestQubesFwupdmgr) ... ok
test_output_crawler (test.test_qubes_fwupdmgr.TestQubesFwupdmgr) ... ok
test_parse_downgrades (test.test_qubes_fwupdmgr.TestQubesFwupdmgr) ... ok
test_parse_parameters (test.test_qubes_fwupdmgr.TestQubesFwupdmgr) ... ok
test_parse_updates_info (test.test_qubes_fwupdmgr.TestQubesFwupdmgr) ... ok
test_refresh_metadata (test.test_qubes_fwupdmgr.TestQubesFwupdmgr) ... skipped 'requires Qubes OS'
test_user_input_choice (test.test_qubes_fwupdmgr.TestQubesFwupdmgr) ... ok
test_user_input_downgrade (test.test_qubes_fwupdmgr.TestQubesFwupdmgr) ... ok
test_user_input_empty_list (test.test_qubes_fwupdmgr.TestQubesFwupdmgr) ... ok
test_user_input_n (test.test_qubes_fwupdmgr.TestQubesFwupdmgr) ... ok
test_verify_dmi (test.test_qubes_fwupdmgr.TestQubesFwupdmgr) ... ok
test_verify_dmi_argument_version (test.test_qubes_fwupdmgr.TestQubesFwupdmgr) ... ok
test_verify_dmi_version (test.test_qubes_fwupdmgr.TestQubesFwupdmgr) ... ok
test_verify_dmi_wrong_vendor (test.test_qubes_fwupdmgr.TestQubesFwupdmgr) ... ok

----------------------------------------------------------------------
Ran 22 tests in 0.003s

OK (skipped=8)
```

### In the Qubes OS

In the dom0, move to:

```
$ cd /usr/share/qubes-fwupd/
```

#### Qubes OS 4.1

Run the tests with sudo privileges:

```
# python3 -m unittest -v test.test_qubes_fwupdmgr
```

Note: If the whonix tests failed, make sure that you are connected to the Tor

[![asciicast](https://asciinema.org/a/TgHOkLnD2YICxB0U80PVcQGqX.svg)](https://asciinema.org/a/TgHOkLnD2YICxB0U80PVcQGqX)

#### Qubes OS 4.0

**Make sure that you are using Python 3.6!**

```
# python36 -m unittest -v test.test_qubes_fwupdmgr
```

Note: If the whonix tests failed, make sure that you are connected to the Tor

[![asciicast](https://asciinema.org/a/HRf27PXEHnLsiQtlwhb55l9Ni.svg)](https://asciinema.org/a/HRf27PXEHnLsiQtlwhb55l9Ni)

## Whonix support

```
# qubes-fwupdmgr [refresh/update/downgrade] --whonix [FLAG]
```

More specified information you will find in the
[whonix documentation](doc/whonix.md).

## UEFI capsule update

```
# qubes-fwupdmgr [update/downgrade]
```

Requirements and more specified information you will find in the
[UEFI capsule update documentation](doc/uefi_capsule_update.md).

## Heads update

```
# qubes-fwupdmgr update-heads --device=x230 --url=<custom-metadata-url>
```

Requirements and more specified information you will find in the
[heads update documentation](doc/heads_udpate.md).
