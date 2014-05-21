import qemu_parts
import disk_fs_parts
import binner_parts
import other_parts
import mechanisms as m
import sys
sys.path += ["../../common"]
import common
import os
import time

read_socket = common.read_socket
write_socket = common.write_socket
report = common.report

def mechanism_prepare(options, state):
    options.generate = common.generate

#TODO: adjust this
def specific_preperations_1(options, state):
    from sys import path
    path.append("../client")
    from script import rs
    rs("lclick", options.m)
    rs("python_server_spawn_args", options.m)

#TODO: need to translate this to another statful mechanism
def test(options, state):
    (lastResponse, status, reqScript) = read_socket(options.s)
    state.status = status
    state.lastResponse = lastResponse
    state.reqScript = reqScript
    stats = state.stats

    #TODO: need to replace proceed5 with custom operations

    if(status == "PTO"):
        common.proceed5(options)
        return status
    if(status == "STTO"):
        common.proceed5(options)
        return status
    if(status == "RDTO"):
        common.proceed5(options)
    if(status == "SR"):
        time.sleep(0.2)
        common.execute_script(options, reqScript)
        write_socket(options.s, "")
        return status
    if(status == "RD"):
        try:
            sample_path = state.samples_list.pop()
        except Exception, e:
            state.samples_exhausted = True
            return
        sample_file = os.path.basename(sample_path)
        test_path = options.settings.prepare_sample(sample_path)
        test_file = os.path.basename(test_path)
        write_socket(options.s, "testFile e:\\samples\\shared\\" + test_file)

        #TODO: replace runner with custom operations
        options.settings.runner_0(options, [test_file])
        options.log.write("%s: " % test_file)
        options.log.flush()
        return status
    if(status == "MA" or status == "TO"):
        # react to test end
        common.proceed5(options)
        write_socket(options.s, "")
        options.log.write("[%s] \n" % status)
        options.log.flush()

        if(status == "MA"):
            stats.ma_count += 1
            stats.to_count = 0
        if(status == "TO"):
            stats.to_count += 1
            if(stats.to_count % 3 == 0):
                report("3x TO, settling")
                stats.to_count = 0
                common.settle(options)
        stats.sample_count += 1
    return state.status

def check_stats(options, state):
    stats = state.stats

    if True:
        # MA or TO, keep track on sample count
        if(stats.sample_count % stats.metric_res == 0):
            current_time = time.localtime()
            elapsed = time.mktime(current_time) - time.mktime(stats.last_time_check)
            report("Tested: " + str(stats.sample_count))
            report(str(stats.metric_res) + " tested in " + str(elapsed) + " seconds")
            report("Last speed: " + str(stats.metric_res/elapsed) + " tps")
            report("MA count: " + str(state.stats.ma_count))
            stats.to_count = 0
            stats.ma_count = 0
            stats.last_time_check = current_time
            if(options.profiling):
                write_socket(options.s, "dump_stats")


qemu_xara_fuzzer = {
    "name" : "Xara Fuzzer",
    "start" : m.step(
        mechanism_prepare,
        next_step = "qemu_prepare"
        ),
    "qemu_prepare" : m.step(
        qemu_parts.qemu_prepare, 
        next_step = "create_saved_disk_autogenerated"
        ),
    "create_saved_disk_autogenerated" : m.step(
        disk_fs_parts.create_saved_disk_autogenerated, 
        next_step = "prepare_disk_autogenerated"
        ), 
    "prepare_disk_autogenerated" : m.step(
        disk_fs_parts.prepare_disk_autogenerated,
        next_step = "qemu_start_full"
        ),
    "qemu_start_full" : m.step(
        qemu_parts.qemu_start_full, 
        next_step = "qemu_mount_disks"
        ),
    "qemu_mount_disks" : m.step(
        qemu_parts.qemu_mount_disks,
        next_step = "xara_script_1"
        ),
    "xara_script_1" : m.step(
        specific_preperations_1,
        next_step = "qemu_connect_dev_socket",
        ),
    "qemu_connect_dev_socket" : m.step(
        qemu_parts.qemu_connect_dev_socket, 
        next_step = "fuzzing_loop"
        ),
    "fuzzing_loop" : m.step(
        other_parts.fuzzing_loop,
        next_step = "binner_kill_explorer"
        ),
    "binner_kill_explorer" : m.step(
        binner_parts.binner_kill_explorer,
        next_step = "binner_spawn"
        ),
    "binner_spawn" : m.step(
        binner_parts.binner_spawn,
        next_step = "binner_spawn_app"
        ),
    "binner_spawn_app" : m.step(
        binner_parts.binner_spawn_app,
        next_step = "binner_configure"
        ),
    "binner_configure" : m.step(
        binner_parts.binner_configure,
        next_step = "binner_start_logs",
        ),
    "binner_start_logs" : m.step(
        binner_parts.binner_start_logs,
        next_step = "binner_check_ready",
        ),
    "binner_check_ready" : m.step(
        binner_parts.binner_check_ready,
        next_step = "binner_key_wait",
        ),
    "binner_key_wait" : m.step(
        binner_parts.binner_key_wait,
        next_step = "test",
        ),
    "test" : m.step(
        test,
        next_step = other_parts.next_test,
        ),
    "save" : m.step(
        binner_parts.binner_save,
        next_step = "poweroff_no_revert",
        ),
    "poweroff_no_revert" : m.step(
        qemu_parts.poweroff_no_revert,
        next_step = "qemu_start_full",
        ),
    "poweroff_regenerate" : m.step(
        qemu_parts.poweroff_no_revert,
        next_step = "create_saved_disk_autogenerated",
        ),
    "shutdown" : m.step(
        qemu_parts.shutdown,
        next_step = "stop",
        ),
    "exit_failure" : m.step(
        other_parts.exit_failure,
        next_step = "stop",
        ),
    "stop" : m.step(
        is_final = True
        )
}

