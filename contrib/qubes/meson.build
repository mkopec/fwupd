dom0_scr = [
  'src/__init__.py',
]

dom0_scr_exec = [
  'src/fwupd-dom0-update',
  'src/fwupd_receive_updates.py',
  'src/qubes_fwupd_heads.py',
  'src/qubes_fwupdmgr.py',
]

dom0_test = [
  'test/__init__.py',
]

dom0_test_exec = [
  'test/fwupd_logs.py',
  'test/test_qubes_fwupd_heads.py',
  'test/test_qubes_fwupdmgr.py',
]

dom0_test_logs = [
  'test/logs/firmware.metainfo.xml',
  'test/logs/get_devices.log',
  'test/logs/get_updates.log',
  'test/logs/help.log',
]

dom0_test_logs_meta = [
  'test/logs/metainfo_name/firmware.metainfo.xml',
  'test/logs/metainfo_version/firmware.metainfo.xml',
]

vm_scripts = [
  'src/updatevm/fwupd-download-updates.sh',
  'src/usbvm/fwupd_usbvm_validate.py',
]

install_data(dom0_scr,
             install_dir : 'share/qubes-fwupd/src')

install_data(dom0_scr_exec,
             install_dir : 'share/qubes-fwupd/src',
             install_mode : 'rwxrwxr-x')

install_data(dom0_test,
             install_dir : 'share/qubes-fwupd/test')

install_data(dom0_test_exec,
             install_dir : 'share/qubes-fwupd/test',
             install_mode : 'rwxrwxr-x')

install_data(dom0_test_logs,
             install_dir : 'share/qubes-fwupd/test/logs')

install_data('test/logs/metainfo_name/firmware.metainfo.xml',
             install_dir : 'share/qubes-fwupd/test/logs/metainfo_name')

install_data('test/logs/metainfo_version/firmware.metainfo.xml',
             install_dir : 'share/qubes-fwupd/test/logs/metainfo_version')

install_data(vm_scripts,
             install_dir : 'share/qubes-fwupd',
             install_mode : 'rwxrwxr-x')
