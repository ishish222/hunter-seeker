name = "Installer config"

shared_folder = "/home/ish/vbox_share_ro/"

args = ['-hda', '/home/ish/machines/qemu/xpsp2-1.qcow2']
args += (['-monitor', 'stdio'])
args += (['-m', '4G'])
args += (['-enable-kvm'])
args += (['-net', 'nic,model=rtl8139'])
args += (['-net', 'user,restrict=y,smb=' + shared_folder])
args += (['-net', 'nic,model=rtl8139'])
args += (['-net', 'tap,ifname=tap1,script=no,downscript=no'])
snap = "4Gclean"

