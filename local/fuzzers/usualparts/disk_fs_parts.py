import os
import sys
sys.path += ["../../common"]
import common
from glob import glob
from subprocess import Popen, PIPE
import time
import tempfile
import globs

report = common.report

def calc_disk_size(options):
    origin_size = 0

    if(os.path.isfile(options.origin)):
        origin_size = os.stat(options.origin).st_size
    else:
        for dirpath, dirnames, filenames in os.walk(options.origin):
            for f in filenames:
                fp = os.path.join(dirpath, f)
                origin_size += os.path.getsize(fp)
    
    print("Origin size: %d" % origin_size)

    size_margin  = origin_size * options.samples_size_margin
    origin_size += size_margin
    disk_size = origin_size * (options.samples_count + 1) + options.settings.SERVER_SIZE
    disk_size = max(int(disk_size), options.settings.MIN_DISK_SIZE)
    return disk_size

def create_saved_disk_autogenerated():
    options = globs.state.options

    disk_size = calc_disk_size(options) 
    saved_size = disk_size * 10
    options.disk_size = disk_size
    options.saved_disk_img = create_drive(options, size=saved_size, name="%s-saved.raw" % common.timestamp2(), label="saved")
    
def create_saved_disk_glob():
    options = globs.state.options

    state.samples_list = glob(options.glob_pattern)
    disk_size = 0
    for sample in state.samples_list:
        disk_size += os.stat(sample).st_size
    size_margin  = disk_size * options.samples_size_margin
    disk_size += size_margin
    disk_size += options.settings.SERVER_SIZE
    disk_size = max(int(disk_size), options.settings.MIN_DISK_SIZE)
    saved_size = disk_size * 2 #only one run

    options.disk_size = disk_size
    options.saved_disk_img = create_drive(options, size=saved_size, name="%s-saved.raw" % common.timestamp2(), label="saved")

def prepare_disk_autogenerated():
    state = globs.state
    options = globs.state.options

    options.tmp_disk_img = create_drive(options, size=options.disk_size)
    mount_drive(options)
    create_layout(options)
    state.samples_list = common.generate(options)
    state.samples_exhausted = False
    umount_drive(options)

def prepare_disk_glob():
    options = globs.state.options

    options.tmp_disk_img = create_drive(options, size=options.disk_size)
    mount_drive(options)
    create_layout(options)
    for sample in state.samples_list:
        print(options.tmp_mountpoint + "/samples/shared/" + sample)
        os.spawnv(os.P_WAIT, "/bin/cp", ["cp", sample, options.tmp_mountpoint + "/samples/shared/"])
    umount_drive(options)

def create_layout(options):
    os.spawnv(os.P_WAIT, "/bin/mkdir", ["mkdir", "-p", options.tmp_mountpoint+"/samples/shared"])
    os.spawnv(os.P_WAIT, "/bin/mkdir", ["mkdir", "-p", options.tmp_mountpoint+"/samples/saved"])
    os.spawnv(os.P_WAIT, "/bin/mkdir", ["mkdir", "-p", options.tmp_mountpoint+"/samples/binned"])
    os.spawnv(os.P_WAIT, "/bin/mkdir", ["mkdir", "-p", options.tmp_mountpoint+"/samples/other"])
    os.spawnv(os.P_WAIT, "/bin/mkdir", ["mkdir", "-p", options.tmp_mountpoint+"/logs"])
    os.spawnv(os.P_WAIT, "/bin/mkdir", ["mkdir", "-p", options.tmp_mountpoint+"/server"])
    # copy server files
    os.spawnv(os.P_WAIT, "/bin/cp", ["cp", "-r", "../server", options.tmp_mountpoint])
    os.spawnv(os.P_WAIT, "/bin/cp", ["cp", "-r", "../common", options.tmp_mountpoint])

def mount_drive(options):
    options.tmp_mountpoint = tempfile.mktemp()
    os.spawnv(os.P_WAIT, "/bin/mkdir", ["mkdir", options.tmp_mountpoint])
    print("Mounting %s" % options.tmp_mountpoint)
    os.spawnv(os.P_WAIT, "/usr/bin/sudo", ["sudo", "mount", "-o", "loop,umask=0000", options.tmp_disk_img, options.tmp_mountpoint])

def umount_drive(options):
    print("Umounting %s" % options.tmp_mountpoint)
    os.spawnv(os.P_WAIT, "/usr/bin/sudo", ["sudo", "umount", options.tmp_mountpoint])
    os.spawnv(os.P_WAIT, "/bin/rm", ["rm", "-rf", options.tmp_mountpoint])

def simple_exec(args):
    ans = Popen(args, stdout=PIPE)
    return ans.stdout.readline()[:-1]

def long_exec(args):
    ans = Popen(args, stdout=PIPE)
    return ans.stdout,read()

def create_image(path, size, label):
    simple_exec(["qemu-img", "create", "-f", "raw", path, str(size)])
    loop_dev = simple_exec(["sudo", "losetup", "-f", "--show", path])
    simple_exec(["sudo", "mkfs.ntfs", "-f", "-L", label, "-H", "0", "-S", "0", "-z", "0", "-p", "0", loop_dev])
    time.sleep(2)
    simple_exec(["sudo", "losetup", "-d", loop_dev])
    return path

def create_drive(options, size=None, name=None, label=None):
    if(size == None):
        size = calc_disk_size(options)
    if(size > options.settings.MAX_DISK_SIZE): raise FuzzingException
    if(name == None):
        name = tempfile.mktemp(suffix = "-samples.raw", dir=".")
    if(label == None):
        label = "secondary"
    create_image(name, size, label)
    return name

