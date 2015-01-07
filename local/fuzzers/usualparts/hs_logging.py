import statemachine

def enable_logging():
    import common
    import globs
    options = globs.state.options

    log = open("./log-%s-%s-%s" % (options.fuzzbox_name, common.timestamp2(), options.origin), "a")
    common.report("Starting fuzzer")
    print("[%s] Stateful fuzzer" % common.timestamp())
    options.log = log

