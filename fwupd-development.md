## Installation

It's recomended to build fwupd with freebsd port system. It provides
dependencies and installs package files.

1. Go to `/usr/` and clone our freebsd-ports fork

```
# git clone git@github.com:3mdeb/freebsd-ports.git -b fwupd ports
```

2. Install dependencies

```
# pkg install glib meson pkgconf gobject-introspection vala gtk-doc json-glib gpgme gnutls sqlite3 curl gcab libarchive
```

3. Go to fwupd package directory

```
# cd /usr/ports/sysutils/fwupd
```

4.1 Update `pkg-plist` (it will take some time)

```
# make makeplist > pkg-plist
```

4.2 Delete first line of the `pkg-plist` file

4.3 Install package (it will take some time)

```
# make install
```

5. To test fwupd run dbus

```
# service dbus onestart
```

6. In a first window run fwupd daemon

```
# cd /usr/local/libexec/fwupd
# ./fwupd -v
```

7. In the second window run `fwupdmgr`

```
$ fwupdmgr --version
```

---

## Development

1. Push your changes to the fwupd project fork

2. Go to the package directory

```
# cd /usr/ports/sysutils/fwupd/
```

3. Open Makefile in your favorite text editor

```
# vim Makefile
```

4. Add `GH_ACCOUNT` variable and set fork account name

```
GH_ACCOUNT=   3mdeb
```

5. Replace `GH_TAGNAME` value with the latest commit hash e.g

```
GH_TAGNAME=   cf63cc7e
```

6. Save and close Makefile


7. Update checksums source checksums

```
# make makesum
```

8. Clean workdir and uninstall old package

```
# make clean
# make deinstall
```

9. Compile and install the project with the changes

```
# make install
```

10. Run daemon and test fwupd
