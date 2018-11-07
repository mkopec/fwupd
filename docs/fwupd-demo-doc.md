FWUPD Documentation
===================

Build project manually on Ubuntu 18.04 on usb-stick with fwupd v1.0.6 version
---------------------------------------------------

Fwupd version v1.0.6 has `fwupdmgr` commands such as `hwids`, which helps with
finding required hwID number for firmware files validation and e.g. BIOS version
validation after firmware update.

1. Boot LT1000 with Ubuntu 18.04 (actually Kingston G3).
2. Send fwupd files from repo (branch 1.0.6)! e.g. via scp: `scp -r PATH_TO_FWUPD_DIR ubuntu@192.168.3.188:/home/ubuntu/`
3. Update package list: `sudo apt-get update`
4. Install required dependencies: `sudo apt-get install python3 python3-pip python3-gi-cairo python3-pil ninja-build flashrom pkg-config libglib2.0-dev gudev-1.0 libappstream-glib-dev libgusb-dev libsqlite3-dev libjson-glib-dev libpolkit-gobject-1-dev udev libgnutls28-dev libgpgme-dev gcab libefivar-dev libelf-dev libcairo2-dev libefiboot-dev libpango1.0-dev libsmbios-dev systemd gnutls-bin gtk-doc-tools libgirepository1.0-dev valac help2man gnu-efi libcolorhug-dev, libfwup-dev`
5. Install meson: `sudo pip3 install meson`
6. Go to `cd /home/ubuntu/fwupd`
7. Build project: `meson build`
8. Install daemon: `sudo ninja -C build install`
9. Check installed version: `fwupdmgr --version`
10. If you are getting error such as:

```
fwupdmgr: error while loading shared libraries: libfwupd.so.2: cannot open
shared object file: No such file or directory.
```

Run: `sudo /sbin/ldconfig -v`

Build with provided Dockerfile
-----------------

#### Prepare fwupd repository (original not forked!)

```
git clone git@github.com:hughsie/fwupd.git
cd fwupd
git checkout 1_1_X
```

#### Generate Docker image

```
cd contrib/ci
OS=debian-x86_64 ./generate_docker.py
```

#### Generate DEB package

In fwupd root directory:

```
docker run --privileged -t -v `pwd`:/build fwupd-debian-x86_64
```

If there is error with missing dependencies, try to update
`contrib/ci/dependencies.xml` file.


#### Docker package build results

From `1_1_X` branch:
* Debian-x86_64 - Dockerfile build succesfully, DEB package build (libxmlb not
  required / Richard said, that newest branch is exactly the same from the
  plugin point of view)

Built packages:

```
fwupd_1.1.3+r9+gbffdc91e_amd64.build
fwupd_1.1.3+r9+gbffdc91e_amd64.buildinfo
fwupd_1.1.3+r9+gbffdc91e_amd64.changes
fwupd_1.1.3+r9+gbffdc91e_amd64.deb
fwupd_1.1.3+r9+gbffdc91e.dsc
fwupd_1.1.3+r9+gbffdc91e.tar.xz
fwupd-amd64-signed-template_1.1.3+r9+gbffdc91e_amd64.deb
fwupd-dbgsym_1.1.3+r9+gbffdc91e_amd64.deb
fwupd-doc_1.1.3+r9+gbffdc91e_all.deb
fwupd-tests_1.1.3+r9+gbffdc91e_amd64.deb
fwupd-tests-dbgsym_1.1.3+r9+gbffdc91e_amd64.deb
gir1.2-fwupd-2.0_1.1.3+r9+gbffdc91e_amd64.deb
libfwupd2_1.1.3+r9+gbffdc91e_amd64.deb
libfwupd2-dbgsym_1.1.3+r9+gbffdc91e_amd64.deb
libfwupd-dev_1.1.3+r9+gbffdc91e_amd64.deb
```

LT1000: Installation on Debian 4.14 netboot / USB 3.0 stick:

```
# apt install ./fwupd_1.1.3+r9+gbffdc91e_amd64.deb  
Reading package lists... Done
Building dependency tree       
Reading state information... Done
Note, selecting 'fwupd' instead of './fwupd_1.1.3+r9+gbffdc91e_amd64.deb'
Some packages could not be installed. This may mean that you have
requested an impossible situation or if you are using the unstable
distribution that some required packages have not yet been created
or been moved out of Incoming.
The following information may help to resolve the situation:

The following packages have unmet dependencies:
 fwupd : Depends: libappstream-glib8 (>= 0.7.10) but it is not going to be installed
         Depends: libefiboot1 (>= 34) but it is not going to be installed
         Depends: libefivar1 (>= 34) but it is not going to be installed
         Depends: libfwupd2 but it is not installable
         Depends: libgcab-1.0-0 (>= 1.0) but it is not going to be installed
         Depends: libglib2.0-0 (>= 2.53.2) but 2.50.3-2 is to be installed
         Depends: libgusb2 (>= 0.2.10) but it is not going to be installed
         Depends: libsmbios-c2 but it is not installable
         Recommends: bolt but it is not installable
         Recommends: fwupd-signed but it is not installable
E: Unable to correct problems, you have held broken packages.
```

From `master` branch:
* Debian-x86_64 - Dockerfile build succesfully, DEB package failed (missing xmlb
  dependency -> required changes in `dependencies.xml` for libxmlb-dev section,
  TODO: check packages: libxmlb, libxmlbird-dev, libxmlbird1)

* Ubuntu-x86_64 - Dockerfile build succesfully, DEB package build succesfully (
  required commenting libxmlb-dev section in `dependencies.xml`) but there is no
  built DEB packages in directory -> it is possible to create snap from DEB
  packages, but running: `cd snap && snapcraft` leads to error:

  ```
  Downloading parts list|
  Failed to get part information: Cannot find the definition for part 'libefivar-fixpkgconfig'. If it is a remote part, run `snapcraft update` to refresh the remote parts cache. If it is a local part, make sure that it is defined in the `snapcraft.yaml`.
  ```

Build Docker image manually (Ubuntu)
-------------------------------------

1. Boot OS from any drive with installed Docker-CE.
    * (TBD if required) `apt-get install dbus-user-session`
    * `apt-get install python3 python3-pip python3-gi-cairo python3-pil ninja-build flashrom`
2. Send fwupd files from repo (branch 1_1_X) e.g. via scp
3. Type: `docker run --privileged --rm -it -v /PATH_TO_FWUPD_DIR:/home/fwupd ubuntu /bin/bash`
4. Update package list: `apt-get update`
5. Install required dependencies: `apt-get install pkg-config libglib2.0-dev gudev-1.0 libappstream-glib-dev libgusb-dev libsqlite3-dev libjson-glib-dev libpolkit-gobject-1-dev udev libgnutls28-dev libgpgme-dev gcab libefivar-dev libelf-dev libcairo2-dev libefiboot-dev libpango1.0-dev libsmbios-dev systemd gnutls-bin gtk-doc-tools libgirepository1.0-dev valac help2man gnu-efi`
6. Install meson: `pip3 install meson`
8. Go to `cd /home/fwupd`
9. Build project: `meson build`
10. Install daemon: `ninja -C build install`
11. If you are getting error such as:

```
fwupdmgr: error while loading shared libraries: libfwupd.so.2: cannot open
shared object file: No such file or directory.
```

Run: `/sbin/ldconfig -v`

Build docker image with custom Dockerfile / uploaded `docker-image.tar`
-----------------------------------------
1. Go to directory with custom `Dockerfile`: `cd contrib/ci`
2. Build Docker image: `docker build -t ubuntu-fwupd .` and make `.tar` archive by
   running `docker save ubuntu-fwupd > ubuntu-fwupd.tar`
    * or download Docker image `ubuntu-fwupd.tar` from 3mdeb cloud (TBD)
3. Upload `ubuntu-fwupd.tar` to LT1000 OS with preinstalled Docker (e.g via scp)
3. Login to LT1000 OS.
4. Load docker image: `docker load -i ubuntu_fwupd.tar`
5. Send required fwupd files from git repo (branch 1_1_X)
6. `cd fwupd`
7. Start docker: `docker run --privileged --rm -it -v $PWD:/home/fwupd ubuntu-fwupd:latest /bin/bash`
8. Build fwupd project: `meson build`
9. Instal fwupd daemon: `ninja -C build install`
10*. If you are getting error such as:

```
fwupdmgr: error while loading shared libraries: libfwupd.so.2: cannot open
shared object file: No such file or directory.
```

Run: `/sbin/ldconfig -v`

When trying to run any `fwupdmgr` commands, there is an error with D-Bus
connection, while operating in Docker container.

> TODO: try Ubuntu Docker image built on [solita/ubuntu-systemd](https://hub.docker.com/r/solita/ubuntu-systemd/)

LVFS
====

The metadata URLs can be used in /etc/fwupd/remotes.d to perform end-to-end
tests. It is important to not share the embargo URL with external users if you
want the firmware to remain hidden from the public. You also may need to do
`fwupdmgr` refresh on each client to show new updates.

|Description    |Public|URL                    |Custom Remote     |
|:-------------:|:----:|:---------------------:|:----------------:|
|Stable         |	Yes  |firmware.xml.gz        |not required      |
|Testing        | Yes  |firmware-testing.xml.gz|not required      |
|Embargo ‘3mdeb’| No   |firmware-3c81bfd       |3mdeb-embargo.conf|

> End-to-end tests using the metadata can only be used with a custom /etc/fwupd/remotes.d/3mdeb-embargo.conf file as this user account does not yet
have permission to push to testing or stable.

#### Uploading firmware

All firmware is uploaded as a cabinet archive, which matches the Microsoft
Update requirements. Along with the firmware binary, the LVFS expects the
archive to contain at least one `.metainfo.xml` file that describes the target
device and firmware. You can create a cabinet archives using `makecab.exe` on
Windows and `gcab` on Linux.

It is recommended you name the archive with the vendor, device and version
number, e.g. `3mdeb-LT1000-1.2.3.cab` and is suggested that the files inside the
cab file have the same basename, for example:

```
   3mdeb-LT1000-1.2.3.cab
    |- flashrom.LT1000.firmware.rom
    \- flashrom.LT1000.firmware.metainfo.xml
```

For demo purposes, there will be uploaded two binaries:
* LT1000-v4.8.0.2.rom
* LT1000-v4.8.0.3.rom
