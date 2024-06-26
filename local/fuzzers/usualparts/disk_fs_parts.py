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

def calc_shared_disk_size(options):
    origin_size = 0

    if(os.path.isfile(options.origin)):
        origin_size = os.stat(options.origin).st_size
    else:
        for dirpath, dirnames, filenames in os.walk(options.origin):
            for f in filenames:
                fp = os.path.join(dirpath, f)
                origin_size += os.path.getsize(fp)
    
    print(("Origin size: %d" % origin_size))

    size_margin  = origin_size * options.samples_size_margin
    origin_size += size_margin
    disk_size = origin_size * (options.samples_count + 1) + options.settings.SERVER_SIZE
    disk_size = max(int(disk_size), options.settings.MIN_DISK_SIZE)
    return disk_size

def create_saved_disk_autogenerated(args=None):
    options = globs.state.options

    disk_size = calc_shared_disk_size(options) 
    saved_size = disk_size * 10 #maybe movi to options/settings?
    options.disk_size = disk_size
    options.saved_disk_img = create_temp_drive(options, size=saved_size, name=options.settings.saved_dir+"%s-saved.raw" % common.timestamp2(), label="saved", format_cmd=options.settings.qemu_drive_format, offset = options.settings.template_offset)
    print('created: ' + options.saved_disk_img)
    os.spawnv(os.P_WAIT, "/usr/bin/sudo", ["sudo", "chown", options.settings.hs_user, options.saved_disk_img])
    
def create_saved_disk_glob(args=None):
    options = globs.state.options
    state = globs.state

    disk_size = 0
    state.samples_list = glob(options.glob_pattern)
    for sample in state.samples_list:
        disk_size += os.stat(sample).st_size

    size_margin  = disk_size * options.samples_size_margin
    disk_size += size_margin
    disk_size += options.settings.SERVER_SIZE
    disk_size = max(int(disk_size), options.settings.MIN_DISK_SIZE)
    saved_size = disk_size * 2 #only one run

    options.disk_size = disk_size
    options.saved_disk_img = create_temp_drive(options, size=saved_size, name=options.settings.saved_dir+"/%s-saved.raw" % common.timestamp2(), label="saved", format_cmd=options.settings.qemu_drive_format, offset = options.settings.template_offset)

    print('created: ' + options.saved_disk_img)
    os.spawnv(os.P_WAIT, "/usr/bin/sudo", ["sudo", "chown", options.settings.hs_user, options.saved_disk_img])

def create_mountpoint(options):
    options.tmp_mountpoint = tempfile.mktemp()
    os.spawnv(os.P_WAIT, "/bin/mkdir", ["mkdir", options.tmp_mountpoint])
    print(("Created mountpoint: %s" % options.tmp_mountpoint))

def del_mountpoint(options):
    os.spawnv(os.P_WAIT, "/bin/rm", ["rm", "-rf", options.tmp_mountpoint])
    print(("Removed mountpoint: %s" % options.tmp_mountpoint))

def create_disk_empty(args=None):
    state = globs.state
    options = globs.state.options

    options.tmp_disk_img = create_temp_drive(options, size=10000000, offset = options.settings.template_offset)

    create_mountpoint(options)
    mount_drive_host(options)
    create_layout(options)

    globs.state.samples_exhausted = True

    umount_drive_host(options)
    del_mountpoint(options)

    options.log.write("[%s]\n" % options.tmp_disk_img)
    options.log.flush()
    os.spawnv(os.P_WAIT, "/usr/bin/sudo", ["sudo", "chown", options.settings.hs_user, globs.state.options.tmp_disk_img])

def create_shared_disk_autogenerated(args=None):
    state = globs.state
    options = globs.state.options

    globs.state.options.tmp_disk_img = create_temp_drive(options, size=options.disk_size, format_cmd=options.settings.qemu_drive_format, offset = options.settings.template_offset)

    create_mountpoint(options)
    mount_drive_host(options)
    create_layout(options)

    globs.state.samples_list = common.generate(options)

#    for s in globs.state.samples_list:
#        print s
    globs.state.samples_exhausted = False

    umount_drive_host(options)
    del_mountpoint(options)

    options.log.write("[%s]\n" % options.tmp_disk_img)
    options.log.flush()
    os.spawnv(os.P_WAIT, "/usr/bin/sudo", ["sudo", "chown", options.settings.hs_user, globs.state.options.tmp_disk_img])

def create_shared_disk_glob(args=None):
    options = globs.state.options
    state = globs.state

    globs.state.options.tmp_disk_img = create_temp_drive(options, size=options.disk_size, format_cmd=options.settings.qemu_drive_format, offset = options.settings.template_offset)
    create_mountpoint(options)
    mount_drive_host(options)
    create_layout(options)
    sl_corrected = list()
    for sample in state.samples_list:
        new_sample = os.path.basename(sample)
        print((options.tmp_mountpoint + "/samples/shared/" + new_sample))
        os.spawnv(os.P_WAIT, "/bin/cp", ["cp", sample, options.tmp_mountpoint + "/samples/shared/"])
        sl_corrected.append(new_sample)
        sl_corrected.sort() # in order to control order :)
    state.samples_list = sl_corrected
    globs.state.samples_exhausted = False
    umount_drive_host(options)
    del_mountpoint(options)
    options.log.write("[%s]\n" % options.tmp_disk_img)
    options.log.flush()
    os.spawnv(os.P_WAIT, "/usr/bin/sudo", ["sudo", "chown", options.settings.hs_user, globs.state.options.tmp_disk_img])

def create_shared_disk_glob_temu(args=None):
    options = globs.state.options

    globs.state.options.tmp_disk_img = create_temp_drive(options, size=options.disk_size, format_cmd=options.settings.qemu_drive_format, offset = options.settings.template_offset)
    create_mountpoint(options)
    mount_drive_host(options, 32256)
    create_layout(options)
    for sample in state.samples_list:
        print((options.tmp_mountpoint + "/samples/shared/" + sample))
        os.spawnv(os.P_WAIT, "/bin/cp", ["cp", sample, options.tmp_mountpoint + "/samples/shared/"])
    umount_drive_host(options)
    del_mountpoint(options)
    options.log.write("[%s]\n" % options.tmp_disk_img)
    options.log.flush()
    os.spawnv(os.P_WAIT, "/usr/bin/sudo", ["sudo", "chown", options.settings.hs_user, globs.state.options.tmp_disk_img])

def create_layout(options):
    os.spawnv(os.P_WAIT, "/usr/bin/sudo", ["sudo", "mkdir", "-p", options.tmp_mountpoint+"/samples/shared"])
    os.spawnv(os.P_WAIT, "/usr/bin/sudo", ["sudo", "mkdir", "-p", options.tmp_mountpoint+"/samples/saved"])
    os.spawnv(os.P_WAIT, "/usr/bin/sudo", ["sudo", "mkdir", "-p", options.tmp_mountpoint+"/samples/binned"])
    os.spawnv(os.P_WAIT, "/usr/bin/sudo", ["sudo", "mkdir", "-p", options.tmp_mountpoint+"/samples/other"])
    os.spawnv(os.P_WAIT, "/usr/bin/sudo", ["sudo", "mkdir", "-p", options.tmp_mountpoint+"/logs"])
    os.spawnv(os.P_WAIT, "/usr/bin/sudo", ["sudo", "mkdir", "-p", options.tmp_mountpoint+"/server"])
    # copy server files
    os.spawnv(os.P_WAIT, "/usr/bin/sudo", ["sudo", "cp", "-r", "../server", options.tmp_mountpoint])
    os.spawnv(os.P_WAIT, "/usr/bin/sudo", ["sudo", "cp", "-r", "../common", options.tmp_mountpoint])

def mount_drive_host(options, additional='loop,umask=0000'):
    additional = options.settings.host_mount_options
    print(("Mounting %s" % options.tmp_mountpoint))
    if(hasattr(options.settings, 'template_offset')):
        additional = 'loop,umask=0000'
        print(("Executing: %s" % " ".join(["sudo", "mount", "-o", "%s,offset=%s" % (additional, options.settings.template_offset), options.tmp_disk_img, options.tmp_mountpoint])))
        os.spawnv(os.P_WAIT, "/usr/bin/sudo", ["sudo", "mount", "-o", "%s,offset=%s" % (additional, options.settings.template_offset), options.tmp_disk_img, options.tmp_mountpoint])
    else:
        os.spawnv(os.P_WAIT, "/usr/bin/sudo", ["sudo", "mount", "-o", additional, options.tmp_disk_img, options.tmp_mountpoint])

def umount_drive_host(options):
    print(("Umounting %s" % options.tmp_mountpoint))
    os.spawnv(os.P_WAIT, "/usr/bin/sudo", ["sudo", "umount", options.tmp_mountpoint])

def mount_drive_host_offset(options, offset):
    print(("Mounting %s" % options.tmp_mountpoint))
    os.spawnv(os.P_WAIT, "/usr/bin/sudo", ["sudo", "mount", "-o", "loop,umask=0000,offset=%s" % offset, options.tmp_disk_img, options.tmp_mountpoint])

def simple_exec(args):
    ans = Popen(args, stdout=PIPE)
    return ans.stdout.readline()[:-1]

def long_exec(args):
    ans = Popen(args, stdout=PIPE)
    return ans.stdout,read()

def create_image(path, size, label, offset=0, format_cmd='mkfs.ntfs'):
    simple_exec(["qemu-img", "create", "-f", "raw", path, str(size)])
    loop_dev = simple_exec(["sudo", "losetup", "-o", str(offset), "-f", "--show", path])
    simple_exec(["sudo", format_cmd, "-f", "-L", label, "-H", "0", "-S", "0", "-z", "0", "-p", "0", loop_dev])
    time.sleep(2)
    simple_exec(["sudo", "losetup", "-d", loop_dev])
    return path

def copy_image(path1, path2):
    simple_exec(["sudo", "cp", path1, path2])
    return path2

def create_temp_drive(options, size=None, name=None, label=None, offset=0, format_cmd='mkfs.ntfs'):

    if(size == None):
        size = calc_shared_disk_size(options)
    if(size > options.settings.MAX_DISK_SIZE): raise FuzzingException
    if(name == None):
        name = tempfile.mktemp(suffix = "-samples.raw", dir=".")
        name = options.settings.saved_dir + "/" + name
    if(label == None):
        label = "secondary"
    if(format_cmd == 'copy_secondary'):
        print('Copying %s' % options.settings.qemu_drives_dir + '/' + options.settings.qemu_secondary)
        copy_image(options.settings.qemu_drives_dir + '/' + options.settings.qemu_secondary, name)
    else:
        create_image(name, size, label, offset)
    return name

def create_shared_folder_glob(args = None):
    options = globs.state.options
    state = globs.state

    # make temp name
    name = tempfile.mktemp(suffix = "-input", dir=options.settings.host_data_output_path)
    print(name)
    os.spawnv(os.P_WAIT, "/bin/mkdir", ["mkdir", name])
    globs.state.options.tmp_mountpoint = name
    create_layout(options)
    return 
