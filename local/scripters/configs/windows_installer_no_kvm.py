#from config import Config

#config = Config()

config = {}

config["name"] = "Windows installer config"
config["shared_folder"] = "/home/ish/vbox_share_ro/"
config["m"] = "4G"
#config["snap"] = "4Gclean"

other_args = []
other_args += ['-no-kvm']
other_args += (['-net', 'nic,model=rtl8139'])
other_args += (['-net', 'user,restrict=y,smb=' + config["shared_folder"]])
#other_args += (['-net', 'nic,model=rtl8139'])
#other_args += (['-net', 'tap,ifname=tap1,script=no,downscript=no'])
other_args += (['-monitor', 'stdio'])
other_args += (['-boot', 'order=dc'])

config["other_args"] = other_args

#args = ['-hda', '/home/ish/machines/qemu/xpsp2-1.qcow2']
#args += (['-monitor', 'stdio'])
#args += (['-m', '4G'])
#args += (['-enable-kvm'])
#args += (['-net', 'nic,model=rtl8139'])
#args += (['-net', 'user,restrict=y,smb=' + config["shared_folder"]])
#args += (['-net', 'nic,model=rtl8139'])
#args += (['-net', 'tap,ifname=tap1,script=no,downscript=no'])
#snap = "4Gclean"
#
