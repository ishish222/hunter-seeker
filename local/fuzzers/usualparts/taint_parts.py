import time
from statemachine import MachineError
import common
import globs
from glob import glob
import os
import usualparts.disk_fs_parts as df
from script import rs, rss, runscriptq, write_monitor, write_monitor_2, read_monitor
from os import system, path
import re

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
        print((options.tmp_mountpoint + '/' + sample))
        os.spawnv(os.P_WAIT, '/usr/bin/sudo', ['sudo', 'cp', sample, '%s/%s' % (options.tmp_mountpoint, options.settings.samples_shared_subdir)])

    df.umount_drive_host(options)
    df.del_mountpoint(options)
    options.log.write("[%s]\n" % options.tmp_disk_img)
    options.log.flush()

    options.qemu_args += ['-drive', 'file='+ options.tmp_disk_img]

def find_pid():
    options = globs.state.options
    state = globs.state
    status = globs.state.status

    write_socket(options.s, "ps")
    (lastResponse, status, reqScript) = read_socket(options.s)

    pid_re = '0x[0-9A-Fa-f]+ (0x[0-9A-Fa-f]+) (\S+)'
    pids = re.findall(pid_re, lastResponse)

    print(pids)

    globs.state.pid = 0

    for pid, name in pids:
        if(name == options.settings.app_module):
            globs.state.pid = int(pid, 16)

    print('Found: 0x%x' % globs.state.pid)

def temu_configure_tracecap():
    options = globs.state.options
    state = globs.state
    status = globs.state.status

    globs.state.options.qemu_dir = path.dirname(options.settings.qemu_command)
    qemu_dir = globs.state.options.qemu_dir

    write_monitor_2(options.m, 'load_plugin %s/tracecap.so\n' % qemu_dir)
    print(read_monitor(options.m))
    print('plgin loaded')
    time.sleep(1)
    write_monitor_2(options.m, "load_config %s/main.ini\n" % qemu_dir)
    print(read_monitor(options.m))
    print('config loaded')
    time.sleep(1)

def temu_taint_start():
    options = globs.state.options
    state = globs.state
    status = globs.state.status

    sample_path = state.samples_list.pop()
    sample_file = path.basename(sample_path)
    
    print(sample_path)
    print(sample_file)
    
    write_monitor_2(options.m, "enable_emulation\n")
    read_monitor(options.m)
    time.sleep(1)


    wo_drive = '/'.join(options.settings.samples_shared_path.split('\\')[1:])
    print("taint_file %s/%s 1 0\n" % (wo_drive, sample_path))
    write_monitor_2(options.m, "taint_file %s/%s 1 0\n" % (wo_drive, sample_path))
    print((read_monitor(options.m)))
    time.sleep(1)

#    state.pid=1

    print("trace %s %s\n" % (state.pid, '%s/%s-%s.out' % (options.settings.large_results_dir, common.timestamp2(), sample_file)))
    write_monitor_2(options.m, "trace %s \"%s\"\n" % (state.pid, '%s/%s-%s.out' % (options.settings.large_results_dir, common.timestamp2(), sample_file)))
    print((read_monitor(options.m)))
    time.sleep(1)

    # prepare sample
    test_path = options.settings.prepare_sample(sample_path)
    test_file = path.basename(test_path)

    time.sleep(2)
    write_socket(options.s, "runFile %s\\%s" % (options.settings.samples_shared_path, test_file))

    options.settings.runner_0(options, [test_file])
    options.log.write("%s: " % test_file)

    print((read_monitor(options.m)))
    print((read_monitor(options.m)))
    print((read_monitor(options.m)))

def temu_taint_conclude():
    options = globs.state.options
    state = globs.state
    status = globs.state.status

    write_monitor_2(options.m, "trace_stop\n")
    print((read_monitor(options.m)))
    time.sleep(1)
    write_monitor_2(options.m, "disable_emulation\n")
    print((read_monitor(options.m)))
    time.sleep(1)

def open_sample():
    options = globs.state.options
    state = globs.state
    status = globs.state.status

    for s in globs.state.samples_list:
        print(s)
    #this should be on run end
#    print("Current stats (SA/MA/TO): %d/%d/%d" % (stats.sample_count, stats.ma_count, stats.to_count))

    try:
        sample_path = globs.state.samples_list.pop()
    except Exception as e:
        print(e)
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

def cooldown_temu():
    options = globs.state.options

    print("Cooling down for Temu")
    #write_socket(options.s, "cooldown2 5 30")
    write_socket(options.s, "cooldown2 5 101")
    read_socket(options.s)

