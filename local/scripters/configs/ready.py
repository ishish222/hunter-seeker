#from config import Config

#config = Config()

config = {}

config["name"] = "Installer config"
config["shared_folder"] = "/home/ish/projects/2012-08-02-korrino/hs2-current-deploy"
config["m"] = "4G"
#config["snap"] = "clean"
config["snap"] = None
config["port"] = "12345"

other_args = ['-enable-kvm']
other_args += (['-net', 'nic,model=rtl8139'])
other_args += (['-net', 'user,restrict=y,smb=' + config["shared_folder"]])
other_args += (['-net', 'nic,model=rtl8139'])
#other_args += (['-net', 'tap,ifname=tap1,script=no,downscript=no])
other_args += (['-monitor', 'stdio'])

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
