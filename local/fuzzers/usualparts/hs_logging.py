import statemachine
import globs
import common

def enable_logging():
    options = globs.state.options

    log = open(options.settings.log_dir + "./log-%s-%s-%s" % (options.fuzzbox_name, common.timestamp2(), options.origin), "a")
    common.report("Starting fuzzer")
    print("[%s] Stateful fuzzer" % common.timestamp())
    options.log = log

def disable_logging():
    options = globs.state.options

    common.write_socket(options.s, "logStop")
