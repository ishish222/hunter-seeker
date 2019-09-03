import usualparts.internal_parts as internal_parts
import usualparts.tracing_parts as tracing_parts
import usualparts.tracer_parts as tracer_parts
import usualparts.taint_parts as taint_parts
import usualparts.test_parts as test_parts
import usualparts.other_parts as other_parts
import usualparts.hs_logging as hs_logging
import usualparts.diagnostic as diagnostic
import usualparts.qemu_parts as qemu_parts
import usualparts.disk_fs_parts as disk_fs_parts
import usualparts.responder_parts as responder_parts
import usualparts.mutator_parts as mutator_parts
import usualparts.binner_parts as binner_parts
import usualparts.ml_parts as ml_parts

global prototypes
prototypes = {}

prototypes['Test']                  = test_parts.test1
prototypes['TestAgain']             = test_parts.test2
prototypes['PrintLogo']             = other_parts.print_logo
prototypes['EnableLogging']         = hs_logging.enable_logging
prototypes['RegisterSignals']       = other_parts.register_signals
prototypes['PrepareStats']          = diagnostic.prepare_stats
prototypes['PreparePipes']          = qemu_parts.qemu_prepare_pipes
prototypes['ChooseSavedMethod']     = other_parts.choosing_saved_disk_procedure
prototypes['GlobMethod']            = disk_fs_parts.create_saved_disk_glob
prototypes['GlobPattern']           = tracing_parts.set_glob_pattern
prototypes['DiskGlob']              = disk_fs_parts.create_shared_disk_glob
prototypes['AutogeneratedMethod']   = disk_fs_parts.create_saved_disk_autogenerated
prototypes['AutogeneratedDisk']     = disk_fs_parts.create_shared_disk_autogenerated
prototypes['QemuStart']             = qemu_parts.qemu_start
prototypes['QemuArgsResearchDir']   = qemu_parts.qemu_args_research_dir
prototypes['QemuArgsKVM']           = qemu_parts.qemu_args_kvm
prototypes['StartQemuFull']         = qemu_parts.qemu_start
prototypes['StartQemuLoad']         = qemu_parts.qemu_start_revert
prototypes['QemuLoad']              = qemu_parts.qemu_load
prototypes['QemuSave']              = qemu_parts.qemu_save
prototypes['QemuDel']               = qemu_parts.qemu_delete
prototypes['QemuMountDisks']        = qemu_parts.qemu_mount_disks
prototypes['QemuMountDisksNoVirtio'] = qemu_parts.qemu_mount_disks_wo_virtio
prototypes['SpawnInternalController'] = tracing_parts.spawn_internal_controller
prototypes['SpawnInternalControllerFileLogSmb'] = tracing_parts.spawn_internal_controller_file_log_smb
prototypes['SpawnInternalControllerSmb'] = tracing_parts.spawn_internal_controller_smb
prototypes['SpawnInternalControllerWoThread'] = tracing_parts.spawn_internal_controller_no_thread
prototypes['QemuConnectDevSocket']  = qemu_parts.qemu_connect_dev_socket_infinite
prototypes['IsSocketConnected']     = qemu_parts.is_socket_connected
prototypes['Wait10']                = other_parts.wait_10_seconds
prototypes['Wait']                  = other_parts.wait
prototypes['KillExplorer']          = internal_parts.internal_kill_explorer
prototypes['HostResetTracers']          = tracing_parts.reset_tracer_controller_status
prototypes['SpawnTracerController'] = tracing_parts.spawn_tracer_controller
prototypes['SpawnTracerScrLog']     = tracing_parts.start_tracer
prototypes['SpawnTracerFileLog']    = tracing_parts.start_tracer_log_local
prototypes['SpawnTracerRemoteLog']  = tracing_parts.start_tracer_log_remote
prototypes['SpawnTracerNoLog']      = tracing_parts.start_tracer_no_log
# for backward compatibility
prototypes['SpawnTracer']           = tracing_parts.start_tracer
prototypes['SpawnTracerLog']        = tracing_parts.start_tracer_log_local
# ends
prototypes['GetSampleOptions']      = tracing_parts.get_sample_options
prototypes['OfflineRevert']           = qemu_parts.offline_revert
prototypes['OfflineRevertClean']           = qemu_parts.offline_revert_clean
prototypes['RevertReady']           = other_parts.noop
prototypes['TracerReopenIO'] = tracer_parts.tracer_reopen_io
prototypes['TracerGetCurrentConfig'] = tracer_parts.tracer_get_current_config
prototypes['GetCurrentConfig'] = tracer_parts.tracer_get_current_config
prototypes['TracerConfigureSample'] = tracer_parts.tracer_configure_sample
prototypes['TracerConfigureSamplePID'] = tracer_parts.tracer_configure_sample_pid
prototypes['TracerConfigureOutDir'] = tracer_parts.tracer_configure_out_dir
prototypes['TracerConfigureOutPrefix'] = tracer_parts.tracer_configure_out_prefix
prototypes['TracerConfigurePIDPrefix'] = tracer_parts.tracer_configure_pid_prefix
prototypes['TracerAppendOutPrefix'] = tracer_parts.tracer_append_out_prefix
prototypes['TracerAppendPIDPrefix'] = tracer_parts.tracer_append_pid_prefix
prototypes['TracerConfigureInDir']  = tracer_parts.tracer_configure_in_dir
prototypes['TracerPrepareTrace']    = tracer_parts.tracer_prepare_trace
prototypes['TracerRegisterRegions'] = tracer_parts.tracer_register_regions
prototypes['RegisterRegions'] = tracer_parts.tracer_register_regions
prototypes['TracerCheckRegion'] = tracer_parts.tracer_check_region
prototypes['CheckRegion'] = tracer_parts.tracer_check_region
prototypes['TracerAddScannedLocation'] = tracer_parts.tracer_add_scanned_location
prototypes['AddScannedLocation'] = tracer_parts.tracer_add_scanned_location
prototypes['TracerRegisterReactions'] = tracer_parts.tracer_register_reactions
prototypes['TracerRegisterReaction'] = tracer_parts.tracer_register_reactions
prototypes['RegisterReactions'] = tracer_parts.tracer_register_reactions
prototypes['RegisterReaction'] = tracer_parts.tracer_register_reactions
prototypes['TracerRegisterReactionsAt'] = tracer_parts.tracer_register_reactions_at
prototypes['TracerRegisterReactionAt'] = tracer_parts.tracer_register_reactions_at
prototypes['RegisterReactionsAt'] = tracer_parts.tracer_register_reactions_at
prototypes['RegisterReactionAt'] = tracer_parts.tracer_register_reactions_at
prototypes['TracerRegisterBuiltin'] = tracer_parts.tracer_register_builtin
prototypes['RegisterBuiltin'] = tracer_parts.tracer_register_builtin
prototypes['TracerGetExceptionCode'] = tracer_parts.tracer_get_exception_code
prototypes['TracerGetExceptionAddress'] = tracer_parts.tracer_get_exception_address
prototypes['TracerGetExceptionAddressStr'] = tracer_parts.tracer_get_exception_address_str
prototypes['GetExceptionCode'] = tracer_parts.tracer_get_exception_code
prototypes['GetExceptionAddress'] = tracer_parts.tracer_get_exception_address
prototypes['TracerDisableAllReactions']      = tracer_parts.tracer_disable_all_reactions
prototypes['TracerDisableReactions']      = tracer_parts.tracer_disable_all_reactions
prototypes['DisableAllReactions']      = tracer_parts.tracer_disable_all_reactions
prototypes['DisableReactions']      = tracer_parts.tracer_disable_all_reactions
prototypes['TracerDebugSample']     = tracer_parts.tracer_debug_sample
prototypes['DebugSample']     = tracer_parts.tracer_debug_sample
prototypes['TracerAttachSample']    = tracer_parts.tracer_attach_sample
prototypes['AttachSample']    = tracer_parts.tracer_attach_sample
prototypes['TracerDebugContinue'] = tracer_parts.tracer_debug_continue
prototypes['TracerDebugContinueInf'] = tracer_parts.tracer_debug_continue
prototypes['DebugContinue'] = tracer_parts.tracer_debug_continue
prototypes['TracerDebugContinueTime'] = tracer_parts.tracer_debug_continue_time
prototypes['DebugContinueTime'] = tracer_parts.tracer_debug_continue_time
prototypes['TracerSetDebugTimeout'] = tracer_parts.tracer_set_debug_timeout
prototypes['SetDebugTimeout'] = tracer_parts.tracer_set_debug_timeout
prototypes['Decision']              = tracing_parts.decision
prototypes['Continue']              = tracer_parts.tracer_debug_continue_decision
prototypes['EnableReaction']        = tracer_parts.tracer_enable_reaction
prototypes['RaiseReaction']         = tracer_parts.tracer_raise_reaction
prototypes['LowerReaction']         = tracer_parts.tracer_lower_reaction
prototypes['ExclusiveReaction']     = tracer_parts.tracer_exclusive_reaction
prototypes['AutorepeatReaction']    = tracer_parts.tracer_autorepeat_reaction
prototypes['DeautorepeatReaction']  = tracer_parts.tracer_deautorepeat_reaction
prototypes['DisableReaction']       = tracer_parts.tracer_disable_reaction
prototypes['EnableBuiltin']         = tracer_parts.tracer_enable_anchors
prototypes['DisableBuiltin']        = tracer_parts.tracer_disable_anchors
prototypes['ExclusiveBuiltin']      = tracer_parts.tracer_exclusive_anchors
prototypes['LowerBuiltin']          = tracer_parts.tracer_lower_anchors
prototypes['DumpMemory']            = tracer_parts.tracer_dump_memory
prototypes['TracerStartTrace']      = tracer_parts.tracer_start_trace
prototypes['StartTrace']            = tracer_parts.tracer_start_trace
prototypes['TracerStartStrace']     = tracer_parts.tracer_start_strace
prototypes['TracerStartTraceDebug'] = tracer_parts.tracer_start_trace_debug
prototypes['TracerStartTraceLight'] = tracer_parts.tracer_start_trace_light
prototypes['ReadRegister']          = tracer_parts.tracer_read_register
prototypes['WriteRegister']         = tracer_parts.tracer_write_register
prototypes['ResetTime']             = other_parts.reset_time
prototypes['ReadTime']              = other_parts.read_time
prototypes['Adjust']                = other_parts.adjust
prototypes['ReadDword']             = tracer_parts.tracer_read_dword
prototypes['WriteDword']            = tracer_parts.tracer_write_dword
prototypes['ReadPID']               = tracer_parts.tracer_read_pid
prototypes['SetPID']               = tracer_parts.tracer_set_pid
prototypes['ReadTID']               = tracer_parts.tracer_read_tid
prototypes['CurrentTID']            = tracer_parts.tracer_current_tid
prototypes['ReadEP']                = tracer_parts.tracer_read_ep
prototypes['TracerPrev']            = tracer_parts.trace_controller_activate_prev_tracer
prototypes['TracerNext']            = tracer_parts.trace_controller_activate_next_tracer
prototypes['TracerCurrent']         = tracer_parts.trace_controller_current_tracer
prototypes['CloseTracer']           = tracing_parts.stop_tracer
prototypes['CheckEqual']            = other_parts.check_equal
prototypes['CheckStrStr']           = other_parts.check_strstr
prototypes['CheckStrStrWQueue']     = other_parts.check_strstr_w_queue
prototypes['CheckStrStrWQueue2']     = other_parts.check_strstr_w_queue2
prototypes['AutoST']                = tracer_parts.tracer_auto_st
prototypes['ReleaseCurrentThread']          = tracer_parts.tracer_release_current_thread
prototypes['ReleaseThread']          = tracer_parts.tracer_release_thread
prototypes['ReleaseAll']             = tracer_parts.tracer_release_all
prototypes['ReleaseAllExcept']       = tracer_parts.tracer_release_all_except
prototypes['ResumeThread']          = tracer_parts.tracer_release_thread
prototypes['ResumeAll']             = tracer_parts.tracer_release_all
prototypes['ResumeAllExcept']       = tracer_parts.tracer_release_all_except
prototypes['SuspendCurrentThread']         = tracer_parts.tracer_suspend_current_thread
prototypes['SuspendThread']         = tracer_parts.tracer_suspend_thread
prototypes['SuspendAll']            = tracer_parts.tracer_suspend_all
prototypes['SuspendAllExcept']      = tracer_parts.tracer_suspend_all_except
prototypes['WriteLastSuspension']   = tracing_parts.write_last_suspension
prototypes['ReadLastSuspension']    = tracing_parts.read_last_suspension
prototypes['WhichTracer']           = tracing_parts.which_tracer
prototypes['MoreTracers']           = tracing_parts.more_tracers
prototypes['ReadPrefix']            = tracing_parts.read_prefix
prototypes['ListProcesses']         = tracer_parts.tracer_ps
prototypes['GetPIDByMatch']         = tracing_parts.get_pid_by_match
prototypes['ListTebs']              = tracer_parts.tracer_list_tebs
prototypes['ListAllTebs']           = tracer_parts.tracer_list_all_tebs
prototypes['LoadEP']                = tracing_parts.load_ep
prototypes['SaveFirstEP']           = tracing_parts.save_first_ep
prototypes['SaveEP']                = tracing_parts.save_ep
prototypes['SaveOffset']            = tracing_parts.save_offset
prototypes['SaveSize']              = tracing_parts.save_size
prototypes['ReadRegion']            = tracer_parts.tracer_read_region
prototypes['OutRegion']             = tracer_parts.tracer_out_region
prototypes['TaintRegions']             = tracer_parts.tracer_taint_regions
prototypes['ManualST']              = tracer_parts.tracer_manual_st
prototypes['ManualSTArg']           = tracer_parts.tracer_manual_st_from_arg
prototypes['ManualSTwSelf']         = tracer_parts.tracer_manual_st_w_self
prototypes['SetSampleFile']         = tracing_parts.set_sample_file
prototypes['SetResearchDir']        = tracing_parts.set_research_dir
prototypes['SetInDir']              = tracing_parts.set_research_dir
prototypes['GetInDir']              = tracing_parts.get_research_dir
prototypes['SetOutDir']             = tracing_parts.set_out_dir
prototypes['CheckHostDir']          = tracing_parts.check_host_dir
prototypes['ReadArgUni']            = tracer_parts.tracer_read_arg_uni
prototypes['ReadArgAnsi']           = tracer_parts.tracer_read_arg_ansi
prototypes['ReadArgAscii']           = tracer_parts.tracer_read_arg_ansi
prototypes['ReadArg']               = tracer_parts.tracer_read_arg
prototypes['SecureAllSections']     = tracing_parts.secure_all_sections
prototypes['SecureSection']         = tracing_parts.secure_section
prototypes['SecureSectionManually'] = tracing_parts.secure_section
prototypes['ExtractEP']             = tracing_parts.extract_ep
prototypes['RunRoutine']            = tracer_parts.tracer_run_routine
prototypes['Spawn']                 = tracer_parts.tracer_spawn
prototypes['Cooldown']              = tracer_parts.tracer_cooldown
prototypes['ReadStack']             = tracer_parts.tracer_read_stack
prototypes['SpawnResponder80']      = responder_parts.spawn_responder_80
prototypes['SpawnResponder']        = responder_parts.spawn_responder
prototypes['NextResponse']          = responder_parts.next_response
prototypes['SpawnMailslot']         = responder_parts.spawn_mailslot_client
prototypes['MailslotNextResponse']  = responder_parts.mailslot_next_response
prototypes['ResolveDNS']            = responder_parts.get_dns
prototypes['GetHTTP']               = responder_parts.get_http
prototypes['StartLog']              = responder_parts.start_log
prototypes['ChangeIP']              = responder_parts.change_ip
prototypes['RunCmd']                = responder_parts.run_cmd
prototypes['RunCommand']                = responder_parts.run_cmd
prototypes['RunAdminCmd']                = responder_parts.run_admin_cmd
prototypes['RunAdminCommand']                = responder_parts.run_admin_cmd
prototypes['RunCmdHost']            = other_parts.run_cmd_host
prototypes['RunCommandHost']            = other_parts.run_cmd_host
prototypes['ReadAscii']             = tracer_parts.read_ansi
prototypes['WriteAscii']             = tracer_parts.write_ansi
prototypes['ReadAnsi']             = tracer_parts.read_ansi
prototypes['WriteAnsi']             = tracer_parts.write_ansi
prototypes['ReadUnicode']          = tracer_parts.read_unicode
prototypes['WriteUnicode']          = tracer_parts.write_unicode
prototypes['Execute']               = other_parts.noop
prototypes['Push']                  = other_parts.push
prototypes['Pop']                   = other_parts.pop
prototypes['Push2']                 = other_parts.push2
prototypes['Pop2']                  = other_parts.pop2
prototypes['PrintEStack']           = other_parts.print_stack
prototypes['PrintEStack2']          = other_parts.print_stack_2
prototypes['AuxPush']               = other_parts.push2
prototypes['AuxPop']                = other_parts.pop2
prototypes['Enqueue']               = other_parts.enqueue
prototypes['Dequeue']               = other_parts.dequeue
prototypes['PrintQueue']            = other_parts.print_queue
prototypes['PrintQueue2']            = other_parts.print_queue2
prototypes['Enqueue2']               = other_parts.enqueue2
prototypes['Dequeue2']               = other_parts.dequeue2
prototypes['AuxEnqueue']               = other_parts.enqueue2
prototypes['AuxDequeue']               = other_parts.dequeue2
prototypes['ClearStack']            = other_parts.clear_stack
prototypes['Interrupt']             = tracing_parts.interrupt
prototypes['QemuQuit']              = qemu_parts.quit
prototypes['TracerResizeOutBuffer'] = tracer_parts.tracer_resize_out_buffer
prototypes['ResizeOutBuffer']       = tracer_parts.tracer_resize_out_buffer
prototypes['TracerResizeModBuffer'] = tracer_parts.tracer_resize_mod_buffer
prototypes['ResizeModBuffer']       = tracer_parts.tracer_resize_mod_buffer
prototypes['TracerChangeInterval']  = tracer_parts.tracer_change_interval
prototypes['ChangeInterval']        = tracer_parts.tracer_change_interval
prototypes['TracerFlushFiles']      = tracer_parts.flush_files
prototypes['FlushFiles']            = tracer_parts.flush_files
prototypes['TracerSetParameters']   = tracing_parts.set_parameters
prototypes['Beep']                  = other_parts.beep
prototypes['TracerPrintResult']     = tracer_parts.tracer_print_result
prototypes['HostPrintResult']       = other_parts.host_print_result
prototypes['HostPrint']             = other_parts.host_print
prototypes['DumpFile']              = tracing_parts.dump_file
prototypes['Pause']                 = other_parts.pause
prototypes['ResolveLocation']       = tracer_parts.tracer_resolve_location
prototypes['SetBase']       = tracer_parts.set_base
prototypes['Int10']                 = other_parts.int10
prototypes['Int16']                 = other_parts.int16
prototypes['StrCat']                = other_parts.str_cat
prototypes['StrCatQueue']           = other_parts.str_cat_queue
prototypes['StrExtractExtension']   = other_parts.str_extract_extension
prototypes['Str']                   = other_parts.strr
prototypes['CompareCounter']        = other_parts.compare_counter
prototypes['SetCounter']            = other_parts.set_counter
prototypes['GetCounter']            = other_parts.get_counter
prototypes['CheckCounter']          = other_parts.check_counter
prototypes['DecreaseCounter']       = other_parts.decrease_counter
prototypes['IncreaseCounter']       = other_parts.increase_counter
prototypes['SetPriorityHigh']          = tracer_parts.tracer_set_priority_high
prototypes['CreateFolderGlob']      = disk_fs_parts.create_shared_folder_glob
prototypes['HostDeployInputGlob']   = tracing_parts.host_deploy_input_glob
prototypes['GetCaller']   = tracer_parts.tracer_get_caller
#Mutator
prototypes['MutatorInit']               = mutator_parts.init_mutator
prototypes['MutatorSelectChanger']      = mutator_parts.select_changer
prototypes['MutatorMutationCount']       = mutator_parts.mutation_count
prototypes['MutatorBatchSize']          = mutator_parts.batch_size
prototypes['MutatorOriginal']           = mutator_parts.original
prototypes['MutatorDeployDir']          = mutator_parts.deploy_dir
prototypes['MutatorBatchExhausted']     = mutator_parts.batch_exhausted
prototypes['MutatorGetNextSample']      = mutator_parts.get_next_sample
prototypes['MutatorGetCurrentSample']   = mutator_parts.get_current_sample
prototypes['MutatorGenerateBatch']      = mutator_parts.generate_batch
prototypes['MutatorCurrentSampleName']  = mutator_parts.current_sample_name
prototypes['MutatorCurrentSamplePath']  = mutator_parts.current_sample_path
prototypes['MutatorCurrentSampleDrop']  = mutator_parts.current_sample_drop
#deprecated
#prototypes['MutatorSaveSample']         = mutator_parts.save_sample
#prototypes['MutatorSaveCrashData']      = mutator_parts.save_crash_data
prototypes['MutatorConfirmSample']      = mutator_parts.confirm_sample
prototypes['MutatorExtension']         = mutator_parts.extension
prototypes['MutatorReport']         = mutator_parts.report
prototypes['MutatorReportTimeout']         = mutator_parts.report_timeout
#Mutator
prototypes['BinnerInit']                = binner_parts.init_binner
prototypes['BinnerLoadSamples']         = binner_parts.load_samples
prototypes['BinnerBatchExhausted']      = binner_parts.batch_exhausted
prototypes['BinnerGetNextSample']       = binner_parts.get_next_sample
prototypes['BinnerGetCurrentSample']    = binner_parts.get_current_sample
prototypes['BinnerCurrentSampleName']   = binner_parts.current_sample_name
prototypes['BinnerCurrentSamplePath']   = binner_parts.current_sample_path
prototypes['BinnerCurrentSampleDrop']   = binner_parts.current_sample_drop
prototypes['BinnerConfirmSample']       = binner_parts.confirm_sample
prototypes['BinnerUnconfirmSample']     = binner_parts.unconfirm_sample
prototypes['BinnerReport']              = binner_parts.report
prototypes['BinnerReportTimeout']       = binner_parts.report_timeout
#ML
prototypes['MLInit']                    = ml_parts.init_ml
prototypes['MLLoadModel']               = ml_parts.load_ml
prototypes['MLSaveModel']               = ml_parts.save_ml
prototypes['MLRemoveModel']             = ml_parts.delete_ml
prototypes['MLSetEpochs']               = ml_parts.set_epochs
prototypes['MLSetInputFilename']        = ml_parts.set_input_filename
prototypes['MLSetInputGlob']            = ml_parts.set_input_glob
prototypes['MLSetInputDir']             = ml_parts.set_input_dir
prototypes['MLSetModelDir']            = ml_parts.set_model_dir
prototypes['MLTrainModelStep']          = ml_parts.train_model_step
prototypes['MLTrainModel']              = ml_parts.train_model
prototypes['MLSaveSamples']             = ml_parts.save_samples
prototypes['MLCheckMore']               = ml_parts.check_more
# new stuff
prototypes['DumpPhysicalMemory']        = qemu_parts.dump_physical_memory

#keywords

prototypes['Call']                  = other_parts.noop
prototypes['Return']                = other_parts.noop
prototypes['Version']               =  other_parts.noop

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
        
        if(ret_tab == None):
            self.ret_tab = None

        positions = ret_tab.split(",")
        for position in positions:
            key, val = position.split(":")
            self.ret_tab[key] = val

    def execute(self):
        ret = self.routine(self.args)
        if(self.ret_tab == None):
            return ret
        elif(ret in list(self.ret_tab.keys())):
            print('Decided on: %s:%s' % (ret, self.ret_tab[ret]))
            return self.ret_tab[ret]
        elif('default' in list(self.ret_tab.keys())):
            print('Decided on: default:%s' % (self.ret_tab['default']))
            return self.ret_tab['default']
