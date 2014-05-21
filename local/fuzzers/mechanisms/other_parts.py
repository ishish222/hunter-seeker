def always_true(options, state):
    return True

def check_successful(options, state):
    return not state.failed

def fail_check(options, state):
    if(not state.failed):
        return True
    else: 
        print "Failed"
        state.failed = False
        state.failure_reason = ""
        return False


def fuzzing_loop(options, state):
    stats = state.stats
    print("Current stats (SA/MA/TO): %d/%d/%d" % (stats.sample_count, stats.ma_count, stats.to_count))

def nop(options, state):
    pass

def exit_failure(options, state):
    print(state.failure_info)
    state.failure_count +=1
    if(state.failure_count > 3): 
        print "Too many failures, exiting"
        exit()

def next_test(options, state):
    if(state.status == "CR"):
        return "save"
    elif(state.samples_exhausted):
        return "poweroff_regenerate"
    else:
        return "test"

