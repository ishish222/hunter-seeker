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
import generators.generatorCorrected as generatorCorrected
import generators.jsMutator as jsMutator

read_socket = common.read_socket
write_socket = common.write_socket
report = common.report

def mechanism_prepare(options, state):
    options.generate = my_generate

def my_generate(options):
    options.real_target = options.args

    if(options.real_target == None):
        print("Point to real target (e.g. htm file)")
        raise Exception

    generator = generatorCorrected.DirGenerator
    mutator = jsMutator.nodeSwitcher
    corrector = "None"
    samples_list = []
    common.create_layout(options)
    os.spawnv(os.P_WAIT, "/bin/cp", ["cp", "-r", options.origin, options.tmp_mountpoint+"/samples/shared"])


    my_generator = generator(options.origin, options.tmp_mountpoint+"/samples/shared", mutator_=mutator, fext="*.js", corrector = None, real_target = options.real_target)
    my_generator.mutations=int(options.mutations)

    samples_list += my_generator.generate(options.samples_count)

    return samples_list

#TODO: need to translate this to another statful mechanism
def test(options, state):
    while True:
        (lastResponse, status, reqScript) = read_socket(options.s)
        state.status = status
        state.lastResponse = lastResponse
        state.reqScript = reqScript
        stats = state.stats
    
        #TODO: need to replace proceed5 with custom operations
    
        if(status == "CR"):
            state.status = status
            state.got_cr = True
            report("Got CR!!1")
            return status
        if(status == "PTO"):
            common.proceed5(options)
            return status
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
    #        sample_path = prepare_sample(sample_path)
    #        sample_file = os.path.basename(sample_path)
    #        test_path = options.settings.prepare_sample(sample_path)
            test_file = sample_path.replace(options.real_target, "")
            
            write_socket(options.s, "testDir \"e:\\samples\\shared\\%s\" \"e:\\samples\\shared\\%s%s\"" % (common.windows_escape(test_file), common.windows_escape(test_file), common.windows_escape(options.real_target)))
    
            #TODO: replace runner with custom operations
            options.log.write("%s: " % test_file)
            options.log.flush()
#            return status
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
            state.status = status
            return status

def my_window_wait(options, state):
    pass

def test_my_window(options, state):
    write_socket(options.s, "windowExists Firefox")
    (lastResponse, status, reqScript) = read_socket(options.s)
    if(lastResponse.find("NOT FOUND") > -1):
        return False
    elif(lastResponse.find("FOUND") > -1):
        return True
    return False
    
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


qemu_firefox_fuzzer = {
    "name" : "Firefox Fuzzer",
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
#        next_step = "stop"
        next_step = "qemu_start_full"
        ),
    "qemu_start_full" : m.step(
        qemu_parts.qemu_start_full, 
        next_step = "qemu_mount_disks"
        ),
    "qemu_mount_disks" : m.step(
        qemu_parts.qemu_mount_disks,
        next_step = "binner_spawn_python_server"
        ),
    "binner_spawn_python_server" : m.step(
        binner_parts.binner_spawn_python_server, 
        next_step = "qemu_connect_dev_socket"
        ),
    "qemu_connect_dev_socket" : m.step(
        qemu_parts.qemu_connect_dev_socket, 
        next_step = other_parts.init_test
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
        next_step = "test_my_window"
        ),
    "test_my_window" : m.step(
        my_window_wait,
        test = test_my_window,
        retries = -1,
        timeout = 3,
        should_retry = True,
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
        test = other_parts.always_true,
        next_step = other_parts.next_test2,
        ),
    "close" : m.step(
        binner_parts.binner_close_sample,
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
        next_step = "check_cr",
        ),
    "check_cr" : m.step(
        qemu_parts.check_cr,
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

