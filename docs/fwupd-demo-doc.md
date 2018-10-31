FWUPD Documentation
===================

Build with Docker
-----------------

#### Prepare fwupd repository (original not forked!)

```
git clone git@github.com:hughsie/fwupd.git
cd fwupd
git checkout 1_1_X
```

#### Generate Dockerfile

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

LT1000: Installation on Debian (usb-stick):

```
root@debian:/home/debian# apt install ./fwupd_1.1.3+r9+gbffdc91e_amd64.deb
Reading package lists... Error!
E: Sub-process Popen returned an error code (2)
E: Encountered a section with no Package: header
E: Problem with MergeList /home/debian/fwupd_1.1.3+r9+gbffdc91e_amd64.deb
E: The package lists or status file could not be parsed or opened.
```

LT1000: Installation on Debian 4.14 netboot:

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
  built DEB packages in directory?

Local build without Docker
==========================

> Boot to Wilk usb hard-drive on LT1000
  login: root
  password: debian


Go to fwupd directory: `cd /home/debian/fwupd`
Building fwupd project locally: `meson build`
Install fwupd after succesfull compilation:

```
cd build
meson install
```

Error output

```
Installing /home/debian/fwupd/contrib/firmware-packager/firmware-packager to /usr/local/share/fwupd
Running custom install script '/root/.local/bin/meson --internal gtkdoc --sourcedir=/home/debian/fwu'
Failed to run install script '/root/.local/bin/meson --internal gtkdoc --sourcedir=/home/debian/fwup'
```

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
