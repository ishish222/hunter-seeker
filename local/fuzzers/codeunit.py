import usualparts.binner_parts as binner_parts
import usualparts.tracing_parts as tracing_parts
import usualparts.tracer_parts as tracer_parts
import usualparts.taint_parts as taint_parts
import usualparts.test_parts as test_parts
import usualparts.other_parts as other_parts
import usualparts.hs_logging as hs_logging
import usualparts.diagnostic as diagnostic
import usualparts.qemu_parts as qemu_parts

global prototypes
prototypes = {}

prototypes['Test'] = test_parts.test1
prototypes['TestAgain'] = test_parts.test2

prototypes['PrintLogo'] = other_parts.print_logo
prototypes['GetOptions'] = other_parts.get_options
prototypes['EnableLogging'] = hs_logging.enable_logging
prototypes['RegisterSignals'] = other_parts.register_signals
prototypes['PrepareStats'] = diagnostic.prepare_stats
prototypes['PreparePipes'] = qemu_parts.qemu_prepare_pipes
prototypes['ChooseSavedMethod'] = other_parts.choosing_saved_disk_procedure
#prototypes[''] = hs_logging.enable_logging

class CodeUnit:
    def __init__(self, name = "Unknown"):
        self.name = name

    def execute(self):
        pass

class Instruction(CodeUnit):
    def __init__(self, name, args):
        self.name = name
        self.args = args
        self.routine = prototypes[name]

    def execute(self):
        ret = self.routine(self.args)
        return ret

class GoTo(CodeUnit):
    def __init__(self, jmp_ip):
        self.name = "goto(%d)" % jmp_ip
        self.jmp_ip = jmp_ip

    def execute(self):
        return self.jmp_ip
        
class Decision(CodeUnit):
    def __init__(self, name, args, ret_tab):
        self.name = name
        self.args = args
        self.ret_tab = {}
        self.routine = prototypes[name]
        
        positions = ret_tab.split(",")
        for position in positions:
            key, val = position.split(":")
            self.ret_tab[key] = int(val, 10)

    def execute(self):
        ret = self.routine(self.args)
        if ret in self.ret_tab.keys():
            print 'Decided on: %s:%d' % (ret, self.ret_tab[ret])
            return self.ret_tab[ret]

