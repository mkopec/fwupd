FROM solita/ubuntu-systemd:latest

MAINTAINER Artur Raglis <artur.raglis@3mdeb.com>

RUN apt-get update && apt-get upgrade -y && \
    apt-get install -y python3 python3-pip python3-cairo python3-pil \
    ninja-build flashrom pkg-config libglib2.0-dev gudev-1.0 \
    libappstream-glib-dev libgusb-dev libsqlite3-dev libjson-glib-dev \
    libpolkit-gobject-1-dev udev libgnutls28-dev libgpgme-dev gcab \
    libefivar-dev libelf-dev libcairo2-dev libefiboot-dev libpango1.0-dev \
    libsmbios-dev systemd gnutls-bin gtk-doc-tools libgirepository1.0-dev \
    valac help2man gnu-efi python3-gi-cairo

RUN pip3 install meson

WORKDIR /home/fwupd
