import time
from statemachine import MachineError
import common
import globs
from glob import glob
import os
import usualparts.disk_fs_parts as df
from script import rs, rss, runscriptq, write_monitor, write_monitor_2
from os import system

report = common.report
write_socket = common.write_socket
read_socket = common.read_socket

def create_temu_samples_disk():
    options = globs.state.options
    state = globs.state
    
    state.samples_list = glob(options.glob_pattern)

    disk_size = 0
    for sample in state.samples_list:
        disk_size += os.stat(sample).st_size

    globs.state.options.tmp_disk_img = df.create_temp_drive(options, size=disk_size, format_cmd = 'copy_secondary')
    time.sleep(1)

    df.create_mountpoint(options)
    df.mount_drive_host(options, 'loop,offset=32256')
    df.create_layout(options)

    for sample in state.samples_list:
        print(options.tmp_mountpoint + '/' + sample)
        os.spawnv(os.P_WAIT, '/usr/bin/sudo', ['sudo', 'cp', sample, options.tmp_mountpoint])

    df.umount_drive_host(options)
    df.del_mountpoint(options)
    options.log.write("[%s]\n" % options.tmp_disk_img)
    options.log.flush()

    options.qemu_args += ['-drive', 'file='+ options.tmp_disk_img]

def find_pid():
    options = globs.state.options
    state = globs.state
    status = globs.state.status

    write_socket(s, "ps")
    glob.state.pid = 0 #change

def temu_configure_tracecap():
    options = globs.state.options
    state = globs.state
    status = globs.state.status

    write_monitor(options.m, "load_plugin ./tracecap.so")
    time.sleep(1)
    write_monitor(options.m, "load_config ./main.ini")
    time.sleep(1)

def temu_taint_start():
    options = globs.state.options
    state = globs.state
    status = globs.state.status

    write_monitor(options.m, "enable_emulation")
    time.sleep(1)
    write_monitor(options.m, "taint_file %s 1 0" % 'plik')
    time.sleep(1)
    write_monitor(options.m, "trace %s %s" % ('pid', 'out'))

    # spawn sample

def temu_taint_conclude():
    options = globs.state.options
    state = globs.state
    status = globs.state.status

    write_monitor(options.m, "trace_stop")
    time.sleep(1)
    write_monitor(options.m, "disable_emulation")
    time.sleep(1)

def open_sample():
    options = globs.state.options
    state = globs.state
    status = globs.state.status

    for s in globs.state.samples_list:
        print s
    #this should be on run end
#    print("Current stats (SA/MA/TO): %d/%d/%d" % (stats.sample_count, stats.ma_count, stats.to_count))

    try:
        sample_path = globs.state.samples_list.pop()
    except Exception, e:
        print e
        globs.state.samples_exhausted = True
        return

    sample_file = os.path.basename(sample_path)
    test_path = options.settings.prepare_sample(sample_path)
    test_file = os.path.basename(test_path)
    write_socket(options.s, "testFile e:\\samples\\shared\\" + test_file)

    options.settings.runner_0(options, [test_file])
    options.log.write("%s: " % test_file)
    options.log.flush()
    #test is under way
    options.log.write("[%s] \n" % status)
    options.log.flush()


def perform_pre_test():
    options = globs.state.options

def perform_after_test():
    options = globs.state.options

    common.proceed5(options)
    write_socket(options.s, "")

