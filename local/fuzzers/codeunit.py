import usualparts.binner_parts as binner_parts
import usualparts.tracing_parts as tracing_parts
import usualparts.tracer_parts as tracer_parts
import usualparts.taint_parts as taint_parts
import usualparts.test_parts as test_parts
import usualparts.other_parts as other_parts
import usualparts.hs_logging as hs_logging
import usualparts.diagnostic as diagnostic
import usualparts.qemu_parts as qemu_parts
import usualparts.disk_fs_parts as disk_fs_parts

global prototypes
prototypes = {}

prototypes['Test']                  = test_parts.test1
prototypes['TestAgain']             = test_parts.test2
prototypes['PrintLogo']             = other_parts.print_logo
prototypes['GetOptions']            = other_parts.get_options
prototypes['EnableLogging']         = hs_logging.enable_logging
prototypes['RegisterSignals']       = other_parts.register_signals
prototypes['PrepareStats']          = diagnostic.prepare_stats
prototypes['PreparePipes']          = qemu_parts.qemu_prepare_pipes
prototypes['ChooseSavedMethod']     = other_parts.choosing_saved_disk_procedure
prototypes['GlobMethod']            = disk_fs_parts.create_saved_disk_glob
prototypes['DiskGlob']              = disk_fs_parts.create_shared_disk_glob
prototypes['AutogeneratedMethod']   = disk_fs_parts.create_saved_disk_autogenerated
prototypes['AutogeneratedDisk']     = disk_fs_parts.create_shared_disk_autogenerated
prototypes['StartQemuFull']         = qemu_parts.qemu_start_full
prototypes['StartQemuLoad']         = qemu_parts.qemu_start_revert
prototypes['QemuMountDisks']        = qemu_parts.qemu_mount_disks
prototypes['SpawnInternalController'] = tracing_parts.spawn_internal_controller
prototypes['QemuConnectDevSocket']  = qemu_parts.qemu_connect_dev_socket_infinite
prototypes['IsSocketConnected']     = qemu_parts.is_socket_connected
prototypes['Wait10']                = other_parts.wait_10_seconds
prototypes['KillExplorer']          = binner_parts.binner_kill_explorer
prototypes['ResetTracers']          = tracing_parts.reset_tracer_controller_status
prototypes['SpawnTracerController'] = tracing_parts.spawn_tracer_controller
prototypes['SpawnTracer']           = tracing_parts.start_tracer
prototypes['SpawnTracerLog']        = tracing_parts.start_tracer_log
prototypes['GetSampleOptions']      = tracing_parts.get_sample_options
prototypes['RevertClean']           = qemu_parts.offline_revert
prototypes['TracerConfigureSample'] = tracer_parts.tracer_configure_sample
prototypes['TracerConfigureSamplePID'] = tracer_parts.tracer_configure_sample_pid
prototypes['TracerConfigureOutDir'] = tracer_parts.tracer_configure_out_dir
prototypes['TracerConfigureOutPrefix'] = tracer_parts.tracer_configure_out_prefix
prototypes['TracerConfigureInDir']  = tracer_parts.tracer_configure_in_dir
prototypes['TracerPrepareTrace']    = tracer_parts.tracer_prepare_trace
prototypes['TracerRegisterRegions'] = tracer_parts.tracer_register_regions
prototypes['TracerRegisterReactions'] = tracer_parts.tracer_register_reactions
prototypes['DisableReactions']      = tracer_parts.tracer_disable_all_reactions
prototypes['TracerDebugSample']     = tracer_parts.tracer_debug_sample
prototypes['TracerDebugContinueInf'] = tracer_parts.tracer_debug_continue
prototypes['Decision']              = tracing_parts.decision
prototypes['EnableReaction']        = tracer_parts.tracer_enable_reaction
prototypes['DumpMemory']            = tracer_parts.tracer_dump_memory
prototypes['TracerStartTrace']      = tracer_parts.tracer_start_trace
prototypes['ReadRegister']          = tracer_parts.tracer_read_register
prototypes['Adjust']                = other_parts.adjust
prototypes['ReadDword']             = tracer_parts.tracer_read_dword
prototypes['WriteDword']            = tracer_parts.tracer_write_dword
prototypes['ReadPID']               = tracer_parts.tracer_read_pid
prototypes['ReadTID']               = tracer_parts.tracer_read_tid
prototypes['ReadEP']                = tracer_parts.tracer_read_ep
prototypes['TracerPrev']            = tracer_parts.trace_controller_activate_prev_tracer
prototypes['TracerNext']            = tracer_parts.trace_controller_activate_next_tracer
prototypes['CloseTracer']           = tracing_parts.stop_tracer
prototypes['CheckEqual']            = other_parts.check_equal
prototypes['AutoST']                = tracer_parts.tracer_auto_st
#prototypes['ResumeThread']          = tracer_parts.


class CodeUnit:
    def __init__(self, name = "Unknown"):
        self.name = name

    def execute(self):
        pass

class Instruction(CodeUnit):
    def __init__(self, name, args):
        self.name = name
        self.args = args
        if(self.args == ''): self.args = None
        self.routine = prototypes[name]

    def execute(self):
        ret = self.routine(self.args)
        return ret

class GoTo(CodeUnit):
    def __init__(self, jmp_ip):
        self.name = "goto(%s)" % jmp_ip
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
            self.ret_tab[key] = val

    def execute(self):
        ret = self.routine(self.args)
        if ret in self.ret_tab.keys():
            print 'Decided on: %s:%s' % (ret, self.ret_tab[ret])
            return self.ret_tab[ret]

