name = "Test config"

args = ['-hda', '/home/ish/machines/qemu/xpsp2-1.qcow2']
args += (['-monitor', 'stdio'])
#args += (['-m', '5G'])
args += (['-enable-kvm'])

snap = "test"
