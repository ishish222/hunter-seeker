	.file	"trace6.c"
	.globl	_my_trace
	.bss
	.align 4
_my_trace:
	.space 4
	.globl	_spawned
_spawned:
	.space 1
	.globl	_log
	.align 4
_log:
	.space 4
	.globl	_started
_started:
	.space 1
	.globl	_instr_count_s
	.align 32
_instr_count_s:
	.space 32
	.globl	_full_log
	.align 4
_full_log:
	.space 4
	.globl	_scan_on
	.align 4
_scan_on:
	.space 4
	.globl	_scan_count
	.align 4
_scan_count:
	.space 4
	.globl	_last_read_record
	.align 4
_last_read_record:
	.space 20
	.globl	_watched
	.align 64
_watched:
	.space 6144
	.globl	_file_handle
	.align 4
_file_handle:
	.space 4
	.globl	_sysenter_esp
	.align 4
_sysenter_esp:
	.space 4
	.globl	_sysenter_no
	.align 4
_sysenter_no:
	.space 4
	.globl	_file
	.align 4
_file:
	.space 4
	.globl	_buffer
	.align 64
_buffer:
	.space 1048576
	.globl	_index
	.align 4
_index:
	.space 4
	.globl	_instr
	.align 64
_instr:
	.space 96
	.globl	_target_lib
	.section .rdata,"dr"
	.align 2
LC0:
	.ascii "n\0t\0d\0l\0l\0.\0d\0l\0l\0\0\0"
	.data
	.align 4
_target_lib:
	.long	LC0
	.globl	_target_lib_off
	.bss
	.align 4
_target_lib_off:
	.space 4
	.globl	_target_call
	.section .rdata,"dr"
LC1:
	.ascii "NtCreateFile\0"
	.data
	.align 4
_target_call:
	.long	LC1
	.globl	_target_call_off
	.align 4
_target_call_off:
	.long	281104
	.globl	_target_off
	.bss
	.align 4
_target_off:
	.space 4
	.globl	_cpdi
	.align 32
_cpdi:
	.space 40
	.globl	_myPID
	.align 4
_myPID:
	.space 4
	.globl	_myTID
	.align 4
_myTID:
	.space 4
	.globl	_img_base
	.align 4
_img_base:
	.space 4
	.globl	_nt1_off
	.align 4
_nt1_off:
	.space 4
	.globl	_nt2_off
	.align 4
_nt2_off:
	.space 4
	.globl	_nt3_off
	.align 4
_nt3_off:
	.space 4
	.globl	_nt4_off
	.align 4
_nt4_off:
	.space 4
	.globl	_sysenter_off
	.align 4
_sysenter_off:
	.space 4
	.globl	_sysret_off
	.align 4
_sysret_off:
	.space 4
	.globl	_last_arg
	.data
	.align 4
_last_arg:
	.long	0
	.long	0
	.byte	6
	.byte	6
	.space 2
	.long	0
	.globl	_last_location
	.bss
	.align 4
_last_location:
	.space 8
	.globl	_syscall_out_args
	.align 64
_syscall_out_args:
	.space 16777216
	.globl	_syscall_out_args_dump_list
	.align 64
_syscall_out_args_dump_list:
	.space 128
	.globl	_buffer_addr
	.align 4
_buffer_addr:
	.space 4
	.globl	_size_addr
	.align 4
_size_addr:
	.space 4
	.globl	_threads
	.align 4
_threads:
	.space 1
	.globl	_blacklist_lib
	.section .rdata,"dr"
LC2:
	.ascii "kernel32.dll\0"
LC3:
	.ascii "ntdll.dll\0"
LC4:
	.ascii "user32.dll\0"
	.data
	.align 4
_blacklist_lib:
	.long	LC2
	.long	LC3
	.long	LC4
	.globl	_blacklist_addr
	.bss
	.align 4
_blacklist_addr:
	.space 1
	.globl	_line2
	.align 64
_line2:
	.space 768
	.text
	.globl	__Z7d_printPKcz
	.def	__Z7d_printPKcz;	.scl	2;	.type	32;	.endef
__Z7d_printPKcz:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$808, %esp
	leal	12(%ebp), %eax
	movl	%eax, -12(%ebp)
	movl	-12(%ebp), %eax
	movl	__imp___iob, %edx
	addl	$32, %edx
	movl	%eax, 8(%esp)
	movl	8(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	%edx, (%esp)
	call	_vfprintf
	movl	__imp___iob, %eax
	addl	$32, %eax
	movl	%eax, (%esp)
	call	_fflush
	movl	$0, %eax
	leave
	ret
	.section .rdata,"dr"
LC5:
	.ascii "Sample routine 1\12\0"
	.text
	.globl	__Z22react_sample_routine_1Pv
	.def	__Z22react_sample_routine_1Pv;	.scl	2;	.type	32;	.endef
__Z22react_sample_routine_1Pv:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$24, %esp
	movl	$LC5, (%esp)
	call	__Z7d_printPKcz
	nop
	leave
	ret
	.section .rdata,"dr"
LC6:
	.ascii "Setting ZF\12\0"
	.align 4
LC7:
	.ascii "Failed to get context, error: 0x%08x\12\0"
LC8:
	.ascii "before flipping: 0x%08x\12\0"
LC9:
	.ascii "after flipping: 0x%08x\12\0"
	.text
	.globl	__Z13react_flip_ZFPv
	.def	__Z13react_flip_ZFPv;	.scl	2;	.type	32;	.endef
__Z13react_flip_ZFPv:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$760, %esp
	movl	$LC6, (%esp)
	call	__Z7d_printPKcz
	movl	$-1, -12(%ebp)
	movl	_my_trace, %eax
	movl	68964100(%eax), %eax
	movl	%eax, -16(%ebp)
	movl	_my_trace, %eax
	movl	-16(%ebp), %edx
	addl	$32, %edx
	movl	8(%eax,%edx,4), %eax
	movl	%eax, -20(%ebp)
	movl	_my_trace, %ecx
	movl	-20(%ebp), %edx
	movl	%edx, %eax
	sall	$2, %eax
	addl	%edx, %eax
	sall	$2, %eax
	addl	%ecx, %eax
	addl	$67108992, %eax
	movl	16(%eax), %eax
	movl	%eax, -12(%ebp)
	movl	$65537, -740(%ebp)
	leal	-740(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	-12(%ebp), %eax
	movl	%eax, (%esp)
	movl	__imp__GetThreadContext@8, %eax
	call	*%eax
	subl	$8, %esp
	testl	%eax, %eax
	sete	%al
	testb	%al, %al
	je	L6
	movl	__imp__GetLastError@0, %eax
	call	*%eax
	movl	%eax, 4(%esp)
	movl	$LC7, (%esp)
	call	__Z7d_printPKcz
L6:
	movl	-548(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC8, (%esp)
	call	__Z7d_printPKcz
	leal	-740(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z13print_contextP8_CONTEXT
	movl	-548(%ebp), %eax
	andl	$64, %eax
	movb	%al, -21(%ebp)
	cmpb	$0, -21(%ebp)
	je	L7
	movl	-548(%ebp), %eax
	andl	$-65, %eax
	movl	%eax, -548(%ebp)
	jmp	L8
L7:
	movl	-548(%ebp), %eax
	orl	$64, %eax
	movl	%eax, -548(%ebp)
L8:
	movl	-548(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC9, (%esp)
	call	__Z7d_printPKcz
	leal	-740(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z13print_contextP8_CONTEXT
	leal	-740(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	-12(%ebp), %eax
	movl	%eax, (%esp)
	movl	__imp__SetThreadContext@8, %eax
	call	*%eax
	subl	$8, %esp
	nop
	leave
	ret
	.section .rdata,"dr"
LC10:
	.ascii "Setting CF\12\0"
LC11:
	.ascii "before setting: 0x%08x\12\0"
LC12:
	.ascii "after setting: 0x%08x\12\0"
	.text
	.globl	__Z12react_set_CFPv
	.def	__Z12react_set_CFPv;	.scl	2;	.type	32;	.endef
__Z12react_set_CFPv:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$760, %esp
	movl	$LC10, (%esp)
	call	__Z7d_printPKcz
	movl	$-1, -12(%ebp)
	movl	_my_trace, %eax
	movl	68964100(%eax), %eax
	movl	%eax, -16(%ebp)
	movl	_my_trace, %eax
	movl	-16(%ebp), %edx
	addl	$32, %edx
	movl	8(%eax,%edx,4), %eax
	movl	%eax, -20(%ebp)
	movl	_my_trace, %ecx
	movl	-20(%ebp), %edx
	movl	%edx, %eax
	sall	$2, %eax
	addl	%edx, %eax
	sall	$2, %eax
	addl	%ecx, %eax
	addl	$67108992, %eax
	movl	16(%eax), %eax
	movl	%eax, -12(%ebp)
	movl	$65537, -736(%ebp)
	leal	-736(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	-12(%ebp), %eax
	movl	%eax, (%esp)
	movl	__imp__GetThreadContext@8, %eax
	call	*%eax
	subl	$8, %esp
	testl	%eax, %eax
	sete	%al
	testb	%al, %al
	je	L11
	movl	__imp__GetLastError@0, %eax
	call	*%eax
	movl	%eax, 4(%esp)
	movl	$LC7, (%esp)
	call	__Z7d_printPKcz
L11:
	movl	-544(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC11, (%esp)
	call	__Z7d_printPKcz
	leal	-736(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z13print_contextP8_CONTEXT
	movl	-544(%ebp), %eax
	orl	$1, %eax
	movl	%eax, -544(%ebp)
	movl	-544(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC12, (%esp)
	call	__Z7d_printPKcz
	leal	-736(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z13print_contextP8_CONTEXT
	leal	-736(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	-12(%ebp), %eax
	movl	%eax, (%esp)
	movl	__imp__SetThreadContext@8, %eax
	call	*%eax
	subl	$8, %esp
	nop
	leave
	ret
	.globl	__Z12react_set_ZFPv
	.def	__Z12react_set_ZFPv;	.scl	2;	.type	32;	.endef
__Z12react_set_ZFPv:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$760, %esp
	movl	$LC6, (%esp)
	call	__Z7d_printPKcz
	movl	$-1, -12(%ebp)
	movl	_my_trace, %eax
	movl	68964100(%eax), %eax
	movl	%eax, -16(%ebp)
	movl	_my_trace, %eax
	movl	-16(%ebp), %edx
	addl	$32, %edx
	movl	8(%eax,%edx,4), %eax
	movl	%eax, -20(%ebp)
	movl	_my_trace, %ecx
	movl	-20(%ebp), %edx
	movl	%edx, %eax
	sall	$2, %eax
	addl	%edx, %eax
	sall	$2, %eax
	addl	%ecx, %eax
	addl	$67108992, %eax
	movl	16(%eax), %eax
	movl	%eax, -12(%ebp)
	movl	$65537, -736(%ebp)
	leal	-736(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	-12(%ebp), %eax
	movl	%eax, (%esp)
	movl	__imp__GetThreadContext@8, %eax
	call	*%eax
	subl	$8, %esp
	testl	%eax, %eax
	sete	%al
	testb	%al, %al
	je	L14
	movl	__imp__GetLastError@0, %eax
	call	*%eax
	movl	%eax, 4(%esp)
	movl	$LC7, (%esp)
	call	__Z7d_printPKcz
L14:
	movl	-544(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC11, (%esp)
	call	__Z7d_printPKcz
	leal	-736(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z13print_contextP8_CONTEXT
	movl	-544(%ebp), %eax
	orl	$64, %eax
	movl	%eax, -544(%ebp)
	movl	-544(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC12, (%esp)
	call	__Z7d_printPKcz
	leal	-736(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z13print_contextP8_CONTEXT
	leal	-736(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	-12(%ebp), %eax
	movl	%eax, (%esp)
	movl	__imp__SetThreadContext@8, %eax
	call	*%eax
	subl	$8, %esp
	nop
	leave
	ret
	.section .rdata,"dr"
LC13:
	.ascii "Zeroing ZF\12\0"
LC14:
	.ascii "before zeroing: 0x%08x\12\0"
LC15:
	.ascii "after zeroing: 0x%08x\12\0"
	.text
	.globl	__Z13react_zero_ZFPv
	.def	__Z13react_zero_ZFPv;	.scl	2;	.type	32;	.endef
__Z13react_zero_ZFPv:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$744, %esp
	movl	$LC13, (%esp)
	call	__Z7d_printPKcz
	movl	_my_trace, %eax
	movl	68964100(%eax), %eax
	movl	%eax, -12(%ebp)
	leal	-728(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	-12(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z12read_contextmP8_CONTEXT
	movl	-536(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC14, (%esp)
	call	__Z7d_printPKcz
	leal	-728(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z13print_contextP8_CONTEXT
	movl	-536(%ebp), %eax
	andl	$-65, %eax
	movl	%eax, -536(%ebp)
	movl	-536(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC15, (%esp)
	call	__Z7d_printPKcz
	leal	-728(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z13print_contextP8_CONTEXT
	leal	-728(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	-12(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z13write_contextmP8_CONTEXT
	nop
	leave
	ret
	.section .rdata,"dr"
LC16:
	.ascii "Flipping CF\12\0"
	.text
	.globl	__Z13react_flip_CFPv
	.def	__Z13react_flip_CFPv;	.scl	2;	.type	32;	.endef
__Z13react_flip_CFPv:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$760, %esp
	movl	$LC16, (%esp)
	call	__Z7d_printPKcz
	movl	$-1, -12(%ebp)
	movl	_my_trace, %eax
	movl	68964100(%eax), %eax
	movl	%eax, -16(%ebp)
	movl	_my_trace, %eax
	movl	-16(%ebp), %edx
	addl	$32, %edx
	movl	8(%eax,%edx,4), %eax
	movl	%eax, -20(%ebp)
	movl	_my_trace, %ecx
	movl	-20(%ebp), %edx
	movl	%edx, %eax
	sall	$2, %eax
	addl	%edx, %eax
	sall	$2, %eax
	addl	%ecx, %eax
	addl	$67108992, %eax
	movl	16(%eax), %eax
	movl	%eax, -12(%ebp)
	movl	$65537, -740(%ebp)
	leal	-740(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	-12(%ebp), %eax
	movl	%eax, (%esp)
	movl	__imp__GetThreadContext@8, %eax
	call	*%eax
	subl	$8, %esp
	testl	%eax, %eax
	sete	%al
	testb	%al, %al
	je	L19
	movl	__imp__GetLastError@0, %eax
	call	*%eax
	movl	%eax, 4(%esp)
	movl	$LC7, (%esp)
	call	__Z7d_printPKcz
L19:
	movl	-548(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC8, (%esp)
	call	__Z7d_printPKcz
	leal	-740(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z13print_contextP8_CONTEXT
	movl	-548(%ebp), %eax
	andl	$1, %eax
	movb	%al, -21(%ebp)
	cmpb	$0, -21(%ebp)
	je	L20
	movl	-548(%ebp), %eax
	andl	$-2, %eax
	movl	%eax, -548(%ebp)
	jmp	L21
L20:
	movl	-548(%ebp), %eax
	orl	$1, %eax
	movl	%eax, -548(%ebp)
L21:
	movl	-548(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC9, (%esp)
	call	__Z7d_printPKcz
	leal	-740(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z13print_contextP8_CONTEXT
	leal	-740(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	-12(%ebp), %eax
	movl	%eax, (%esp)
	movl	__imp__SetThreadContext@8, %eax
	call	*%eax
	subl	$8, %esp
	nop
	leave
	ret
	.section .rdata,"dr"
LC17:
	.ascii "Flipping SF\12\0"
	.text
	.globl	__Z13react_flip_SFPv
	.def	__Z13react_flip_SFPv;	.scl	2;	.type	32;	.endef
__Z13react_flip_SFPv:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$760, %esp
	movl	$LC17, (%esp)
	call	__Z7d_printPKcz
	movl	$-1, -12(%ebp)
	movl	_my_trace, %eax
	movl	68964100(%eax), %eax
	movl	%eax, -16(%ebp)
	movl	_my_trace, %eax
	movl	-16(%ebp), %edx
	addl	$32, %edx
	movl	8(%eax,%edx,4), %eax
	movl	%eax, -20(%ebp)
	movl	_my_trace, %ecx
	movl	-20(%ebp), %edx
	movl	%edx, %eax
	sall	$2, %eax
	addl	%edx, %eax
	sall	$2, %eax
	addl	%ecx, %eax
	addl	$67108992, %eax
	movl	16(%eax), %eax
	movl	%eax, -12(%ebp)
	movl	$65537, -740(%ebp)
	leal	-740(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	-12(%ebp), %eax
	movl	%eax, (%esp)
	movl	__imp__GetThreadContext@8, %eax
	call	*%eax
	subl	$8, %esp
	testl	%eax, %eax
	sete	%al
	testb	%al, %al
	je	L24
	movl	__imp__GetLastError@0, %eax
	call	*%eax
	movl	%eax, 4(%esp)
	movl	$LC7, (%esp)
	call	__Z7d_printPKcz
L24:
	movl	-548(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC8, (%esp)
	call	__Z7d_printPKcz
	leal	-740(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z13print_contextP8_CONTEXT
	movl	-548(%ebp), %eax
	andl	$-128, %eax
	movb	%al, -21(%ebp)
	cmpb	$0, -21(%ebp)
	je	L25
	movl	-548(%ebp), %eax
	andb	$127, %al
	movl	%eax, -548(%ebp)
	jmp	L26
L25:
	movl	-548(%ebp), %eax
	orb	$-128, %al
	movl	%eax, -548(%ebp)
L26:
	movl	-548(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC9, (%esp)
	call	__Z7d_printPKcz
	leal	-740(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z13print_contextP8_CONTEXT
	leal	-740(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	-12(%ebp), %eax
	movl	%eax, (%esp)
	movl	__imp__SetThreadContext@8, %eax
	call	*%eax
	subl	$8, %esp
	nop
	leave
	ret
	.section .rdata,"dr"
LC18:
	.ascii "Zeroing CF\12\0"
	.text
	.globl	__Z13react_zero_CFPv
	.def	__Z13react_zero_CFPv;	.scl	2;	.type	32;	.endef
__Z13react_zero_CFPv:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$744, %esp
	movl	$LC18, (%esp)
	call	__Z7d_printPKcz
	movl	_my_trace, %eax
	movl	68964100(%eax), %eax
	movl	%eax, -12(%ebp)
	leal	-728(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	-12(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z12read_contextmP8_CONTEXT
	movl	-536(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC14, (%esp)
	call	__Z7d_printPKcz
	leal	-728(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z13print_contextP8_CONTEXT
	movl	-536(%ebp), %eax
	andl	$-2, %eax
	movl	%eax, -536(%ebp)
	movl	-536(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC15, (%esp)
	call	__Z7d_printPKcz
	leal	-728(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z13print_contextP8_CONTEXT
	leal	-728(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	-12(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z13write_contextmP8_CONTEXT
	nop
	leave
	ret
	.section .rdata,"dr"
LC19:
	.ascii "Zeroing EAX\12\0"
	.text
	.globl	__Z14react_zero_EAXPv
	.def	__Z14react_zero_EAXPv;	.scl	2;	.type	32;	.endef
__Z14react_zero_EAXPv:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$744, %esp
	movl	$LC19, (%esp)
	call	__Z7d_printPKcz
	movl	_my_trace, %eax
	movl	68964100(%eax), %eax
	movl	%eax, -12(%ebp)
	leal	-728(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	-12(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z12read_contextmP8_CONTEXT
	movl	-536(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC14, (%esp)
	call	__Z7d_printPKcz
	leal	-728(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z13print_contextP8_CONTEXT
	movl	$0, -552(%ebp)
	movl	-536(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC15, (%esp)
	call	__Z7d_printPKcz
	leal	-728(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z13print_contextP8_CONTEXT
	leal	-728(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	-12(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z13write_contextmP8_CONTEXT
	nop
	leave
	ret
	.section .rdata,"dr"
LC20:
	.ascii "Zeroing SF\12\0"
	.text
	.globl	__Z13react_zero_SFPv
	.def	__Z13react_zero_SFPv;	.scl	2;	.type	32;	.endef
__Z13react_zero_SFPv:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$744, %esp
	movl	$LC20, (%esp)
	call	__Z7d_printPKcz
	movl	_my_trace, %eax
	movl	68964100(%eax), %eax
	movl	%eax, -12(%ebp)
	leal	-728(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	-12(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z12read_contextmP8_CONTEXT
	movl	-536(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC14, (%esp)
	call	__Z7d_printPKcz
	leal	-728(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z13print_contextP8_CONTEXT
	movl	-536(%ebp), %eax
	andb	$127, %al
	movl	%eax, -536(%ebp)
	movl	-536(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC15, (%esp)
	call	__Z7d_printPKcz
	leal	-728(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z13print_contextP8_CONTEXT
	leal	-728(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	-12(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z13write_contextmP8_CONTEXT
	nop
	leave
	ret
	.section .rdata,"dr"
	.align 4
LC21:
	.ascii "# Current mod position: 0x%08x\12\0"
	.align 4
LC22:
	.ascii "[Updated location: 0x%08x, size: 0x%08x]\12\0"
LC23:
	.ascii "UP,0x%08x,0x%08x\12\0"
	.text
	.globl	__Z17update_region_oldP9LOCATION_
	.def	__Z17update_region_oldP9LOCATION_;	.scl	2;	.type	32;	.endef
__Z17update_region_oldP9LOCATION_:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$808, %esp
	movl	_my_trace, %eax
	movl	70012880(%eax), %eax
	movl	%eax, (%esp)
	call	_ftell
	movl	%eax, 8(%esp)
	movl	$LC21, 4(%esp)
	leal	-780(%ebp), %eax
	movl	%eax, (%esp)
	call	_sprintf
	leal	-780(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z13add_to_bufferPc
	movl	8(%ebp), %eax
	movl	4(%eax), %edx
	movl	8(%ebp), %eax
	movl	(%eax), %eax
	movl	%eax, %ecx
	movl	_my_trace, %eax
	movl	70012880(%eax), %eax
	movl	%edx, 8(%esp)
	movl	%ecx, 4(%esp)
	movl	%eax, (%esp)
	call	__Z8dump_memP6_iobufPvm
	movl	%eax, -12(%ebp)
	movl	8(%ebp), %eax
	movl	4(%eax), %eax
	cmpl	-12(%ebp), %eax
	jne	L35
	movl	8(%ebp), %eax
	movl	4(%eax), %edx
	movl	8(%ebp), %eax
	movl	(%eax), %eax
	movl	%edx, 8(%esp)
	movl	%eax, 4(%esp)
	movl	$LC22, (%esp)
	call	__Z7d_printPKcz
	movl	8(%ebp), %eax
	movl	4(%eax), %edx
	movl	8(%ebp), %eax
	movl	(%eax), %eax
	movl	%edx, 12(%esp)
	movl	%eax, 8(%esp)
	movl	$LC23, 4(%esp)
	leal	-780(%ebp), %eax
	movl	%eax, (%esp)
	call	_sprintf
	leal	-780(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z13add_to_bufferPc
	movl	_my_trace, %eax
	movl	70012880(%eax), %eax
	movl	%eax, (%esp)
	call	_ftell
	movl	%eax, 8(%esp)
	movl	$LC21, 4(%esp)
	leal	-780(%ebp), %eax
	movl	%eax, (%esp)
	call	_sprintf
	leal	-780(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z13add_to_bufferPc
L35:
	nop
	leave
	ret
	.globl	__Z13update_regionj
	.def	__Z13update_regionj;	.scl	2;	.type	32;	.endef
__Z13update_regionj:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$808, %esp
	movl	_my_trace, %eax
	movl	8(%ebp), %edx
	addl	$8753186, %edx
	sall	$3, %edx
	addl	%edx, %eax
	addl	$4, %eax
	movl	%eax, -12(%ebp)
	movl	-12(%ebp), %eax
	movl	(%eax), %eax
	movl	%eax, (%esp)
	call	__Z16resolve_loc_descP24LOCATION_DESCRIPTOR_NEW_
	movl	%eax, -16(%ebp)
	movl	-12(%ebp), %eax
	movl	4(%eax), %eax
	movl	%eax, (%esp)
	call	__Z16resolve_loc_descP24LOCATION_DESCRIPTOR_NEW_
	movl	%eax, -20(%ebp)
	movl	_my_trace, %eax
	movl	70012880(%eax), %eax
	movl	%eax, (%esp)
	call	_ftell
	movl	%eax, 8(%esp)
	movl	$LC21, 4(%esp)
	leal	-792(%ebp), %eax
	movl	%eax, (%esp)
	call	_sprintf
	leal	-792(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z13add_to_bufferPc
	movl	-16(%ebp), %edx
	movl	_my_trace, %eax
	movl	70012880(%eax), %eax
	movl	-20(%ebp), %ecx
	movl	%ecx, 8(%esp)
	movl	%edx, 4(%esp)
	movl	%eax, (%esp)
	call	__Z8dump_memP6_iobufPvm
	movl	%eax, -24(%ebp)
	movl	-24(%ebp), %eax
	cmpl	-20(%ebp), %eax
	jne	L38
	movl	-20(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	-16(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC22, (%esp)
	call	__Z7d_printPKcz
	movl	-20(%ebp), %eax
	movl	%eax, 12(%esp)
	movl	-16(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	$LC23, 4(%esp)
	leal	-792(%ebp), %eax
	movl	%eax, (%esp)
	call	_sprintf
	leal	-792(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z13add_to_bufferPc
	movl	_my_trace, %eax
	movl	70012880(%eax), %eax
	movl	%eax, (%esp)
	call	_ftell
	movl	%eax, 8(%esp)
	movl	$LC21, 4(%esp)
	leal	-792(%ebp), %eax
	movl	%eax, (%esp)
	call	_sprintf
	leal	-792(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z13add_to_bufferPc
L38:
	nop
	leave
	ret
	.section .rdata,"dr"
LC24:
	.ascii "Updating region 1\12\0"
	.text
	.globl	__Z21react_update_region_1Pv
	.def	__Z21react_update_region_1Pv;	.scl	2;	.type	32;	.endef
__Z21react_update_region_1Pv:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$24, %esp
	movl	$LC24, (%esp)
	call	__Z7d_printPKcz
	movl	$0, (%esp)
	call	__Z13update_regionj
	nop
	leave
	ret
	.section .rdata,"dr"
LC25:
	.ascii "Updating region 2\12\0"
	.text
	.globl	__Z21react_update_region_2Pv
	.def	__Z21react_update_region_2Pv;	.scl	2;	.type	32;	.endef
__Z21react_update_region_2Pv:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$24, %esp
	movl	$LC25, (%esp)
	call	__Z7d_printPKcz
	movl	$1, (%esp)
	call	__Z13update_regionj
	nop
	leave
	ret
	.section .rdata,"dr"
LC26:
	.ascii "Outputting register %s\12\0"
LC27:
	.ascii "OU,0x%x,%s: 0x%08x\12\0"
	.text
	.globl	__Z15output_registerPc
	.def	__Z15output_registerPc;	.scl	2;	.type	32;	.endef
__Z15output_registerPc:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$824, %esp
	movl	8(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC26, (%esp)
	call	__Z7d_printPKcz
	movl	_my_trace, %eax
	movl	68964276(%eax), %eax
	movl	8(%ebp), %edx
	movl	%edx, 4(%esp)
	movl	%eax, (%esp)
	call	__Z13read_registermPc
	movl	%eax, -12(%ebp)
	movl	_my_trace, %eax
	movl	68964276(%eax), %eax
	movl	-12(%ebp), %edx
	movl	%edx, 16(%esp)
	movl	8(%ebp), %edx
	movl	%edx, 12(%esp)
	movl	%eax, 8(%esp)
	movl	$LC27, 4(%esp)
	leal	-780(%ebp), %eax
	movl	%eax, (%esp)
	call	_sprintf
	leal	-780(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z13add_to_bufferPc
	nop
	leave
	ret
	.section .rdata,"dr"
LC28:
	.ascii "EAX\0"
	.text
	.globl	__Z16react_output_eaxPv
	.def	__Z16react_output_eaxPv;	.scl	2;	.type	32;	.endef
__Z16react_output_eaxPv:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$24, %esp
	movl	$LC28, (%esp)
	call	__Z15output_registerPc
	leave
	ret
	.section .rdata,"dr"
LC29:
	.ascii "EBX\0"
	.text
	.globl	__Z16react_output_ebxPv
	.def	__Z16react_output_ebxPv;	.scl	2;	.type	32;	.endef
__Z16react_output_ebxPv:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$24, %esp
	movl	$LC29, (%esp)
	call	__Z15output_registerPc
	leave
	ret
	.section .rdata,"dr"
LC30:
	.ascii "ECX\0"
	.text
	.globl	__Z16react_output_ecxPv
	.def	__Z16react_output_ecxPv;	.scl	2;	.type	32;	.endef
__Z16react_output_ecxPv:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$24, %esp
	movl	$LC30, (%esp)
	call	__Z15output_registerPc
	leave
	ret
	.section .rdata,"dr"
LC31:
	.ascii "EDX\0"
	.text
	.globl	__Z16react_output_edxPv
	.def	__Z16react_output_edxPv;	.scl	2;	.type	32;	.endef
__Z16react_output_edxPv:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$24, %esp
	movl	$LC31, (%esp)
	call	__Z15output_registerPc
	leave
	ret
	.section .rdata,"dr"
LC32:
	.ascii "EDI\0"
	.text
	.globl	__Z16react_output_ediPv
	.def	__Z16react_output_ediPv;	.scl	2;	.type	32;	.endef
__Z16react_output_ediPv:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$24, %esp
	movl	$LC32, (%esp)
	call	__Z15output_registerPc
	leave
	ret
	.section .rdata,"dr"
LC33:
	.ascii "ESI\0"
	.text
	.globl	__Z16react_output_esiPv
	.def	__Z16react_output_esiPv;	.scl	2;	.type	32;	.endef
__Z16react_output_esiPv:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$24, %esp
	movl	$LC33, (%esp)
	call	__Z15output_registerPc
	leave
	ret
	.section .rdata,"dr"
LC34:
	.ascii "ESP\0"
	.text
	.globl	__Z16react_output_espPv
	.def	__Z16react_output_espPv;	.scl	2;	.type	32;	.endef
__Z16react_output_espPv:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$24, %esp
	movl	$LC34, (%esp)
	call	__Z15output_registerPc
	leave
	ret
	.section .rdata,"dr"
LC35:
	.ascii "EIP\0"
	.text
	.globl	__Z16react_output_eipPv
	.def	__Z16react_output_eipPv;	.scl	2;	.type	32;	.endef
__Z16react_output_eipPv:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$24, %esp
	movl	$LC35, (%esp)
	call	__Z15output_registerPc
	leave
	ret
	.section .rdata,"dr"
LC36:
	.ascii "Outputting arg %d\12\0"
LC37:
	.ascii "%ls\0"
	.align 4
LC38:
	.ascii "# Failed to read UNICODE string @ 0x%08x\12\0"
	.text
	.globl	__Z27report_arg_unicode_string_xj
	.def	__Z27report_arg_unicode_string_xj;	.scl	2;	.type	32;	.endef
__Z27report_arg_unicode_string_xj:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$2056, %esp
	movl	8(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC36, (%esp)
	call	__Z7d_printPKcz
	leal	-1504(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$0, (%esp)
	call	__Z12read_contextmP8_CONTEXT
	movl	-1308(%ebp), %eax
	movl	%eax, -12(%ebp)
	movl	8(%ebp), %eax
	sall	$2, %eax
	addl	%eax, -12(%ebp)
	movl	-12(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z10read_dwordm
	movl	%eax, -16(%ebp)
	movl	-16(%ebp), %edx
	movl	_my_trace, %eax
	movl	88(%eax), %eax
	leal	-20(%ebp), %ecx
	movl	%ecx, 16(%esp)
	movl	$512, 12(%esp)
	leal	-2016(%ebp), %ecx
	movl	%ecx, 8(%esp)
	movl	%edx, 4(%esp)
	movl	%eax, (%esp)
	call	__Z11read_memoryPvS_S_mPm
	movl	-20(%ebp), %eax
	testl	%eax, %eax
	je	L55
	movl	_my_trace, %eax
	movl	$7, 67915420(%eax)
	leal	-2016(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	$LC37, 4(%esp)
	leal	-788(%ebp), %eax
	movl	%eax, (%esp)
	call	_sprintf
	movl	_my_trace, %eax
	leal	67915424(%eax), %edx
	leal	-788(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	%edx, (%esp)
	call	_strcpy
	jmp	L56
L55:
	movl	_my_trace, %eax
	movl	$7, 67915420(%eax)
	movl	-12(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	$LC38, 4(%esp)
	leal	-788(%ebp), %eax
	movl	%eax, (%esp)
	call	_sprintf
	movl	_my_trace, %eax
	leal	67915424(%eax), %edx
	leal	-788(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	%edx, (%esp)
	call	_strcpy
L56:
	nop
	leave
	ret
	.section .rdata,"dr"
LC39:
	.ascii "OU,0x%x,Arg%d: %ls\12\0"
	.text
	.globl	__Z27output_arg_unicode_string_xj
	.def	__Z27output_arg_unicode_string_xj;	.scl	2;	.type	32;	.endef
__Z27output_arg_unicode_string_xj:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$2056, %esp
	movl	8(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC36, (%esp)
	call	__Z7d_printPKcz
	leal	-1504(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$0, (%esp)
	call	__Z12read_contextmP8_CONTEXT
	movl	-1308(%ebp), %eax
	movl	%eax, -12(%ebp)
	movl	8(%ebp), %eax
	sall	$2, %eax
	addl	%eax, -12(%ebp)
	movl	-12(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z10read_dwordm
	movl	%eax, -16(%ebp)
	movl	-16(%ebp), %edx
	movl	_my_trace, %eax
	movl	88(%eax), %eax
	leal	-20(%ebp), %ecx
	movl	%ecx, 16(%esp)
	movl	$512, 12(%esp)
	leal	-2016(%ebp), %ecx
	movl	%ecx, 8(%esp)
	movl	%edx, 4(%esp)
	movl	%eax, (%esp)
	call	__Z11read_memoryPvS_S_mPm
	movl	-20(%ebp), %eax
	testl	%eax, %eax
	je	L59
	movl	_my_trace, %eax
	movl	68964276(%eax), %eax
	leal	-2016(%ebp), %edx
	movl	%edx, 16(%esp)
	movl	8(%ebp), %edx
	movl	%edx, 12(%esp)
	movl	%eax, 8(%esp)
	movl	$LC39, 4(%esp)
	leal	-788(%ebp), %eax
	movl	%eax, (%esp)
	call	_sprintf
	leal	-788(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z13add_to_bufferPc
	jmp	L60
L59:
	movl	-12(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	$LC38, 4(%esp)
	leal	-788(%ebp), %eax
	movl	%eax, (%esp)
	call	_sprintf
	leal	-788(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z13add_to_bufferPc
L60:
	nop
	leave
	ret
	.globl	__Z30react_output_arg_unicode_str_0Pv
	.def	__Z30react_output_arg_unicode_str_0Pv;	.scl	2;	.type	32;	.endef
__Z30react_output_arg_unicode_str_0Pv:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$24, %esp
	movl	$0, (%esp)
	call	__Z27output_arg_unicode_string_xj
	leave
	ret
	.globl	__Z30react_output_arg_unicode_str_1Pv
	.def	__Z30react_output_arg_unicode_str_1Pv;	.scl	2;	.type	32;	.endef
__Z30react_output_arg_unicode_str_1Pv:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$24, %esp
	movl	$1, (%esp)
	call	__Z27output_arg_unicode_string_xj
	leave
	ret
	.globl	__Z30react_output_arg_unicode_str_2Pv
	.def	__Z30react_output_arg_unicode_str_2Pv;	.scl	2;	.type	32;	.endef
__Z30react_output_arg_unicode_str_2Pv:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$24, %esp
	movl	$2, (%esp)
	call	__Z27output_arg_unicode_string_xj
	leave
	ret
	.globl	__Z30react_output_arg_unicode_str_3Pv
	.def	__Z30react_output_arg_unicode_str_3Pv;	.scl	2;	.type	32;	.endef
__Z30react_output_arg_unicode_str_3Pv:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$24, %esp
	movl	$3, (%esp)
	call	__Z27output_arg_unicode_string_xj
	leave
	ret
	.globl	__Z30react_output_arg_unicode_str_4Pv
	.def	__Z30react_output_arg_unicode_str_4Pv;	.scl	2;	.type	32;	.endef
__Z30react_output_arg_unicode_str_4Pv:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$24, %esp
	movl	$4, (%esp)
	call	__Z27output_arg_unicode_string_xj
	leave
	ret
	.globl	__Z30react_output_arg_unicode_str_5Pv
	.def	__Z30react_output_arg_unicode_str_5Pv;	.scl	2;	.type	32;	.endef
__Z30react_output_arg_unicode_str_5Pv:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$24, %esp
	movl	$5, (%esp)
	call	__Z27output_arg_unicode_string_xj
	leave
	ret
	.globl	__Z30react_output_arg_unicode_str_6Pv
	.def	__Z30react_output_arg_unicode_str_6Pv;	.scl	2;	.type	32;	.endef
__Z30react_output_arg_unicode_str_6Pv:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$24, %esp
	movl	$6, (%esp)
	call	__Z27output_arg_unicode_string_xj
	leave
	ret
	.globl	__Z30react_output_arg_unicode_str_7Pv
	.def	__Z30react_output_arg_unicode_str_7Pv;	.scl	2;	.type	32;	.endef
__Z30react_output_arg_unicode_str_7Pv:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$24, %esp
	movl	$7, (%esp)
	call	__Z27output_arg_unicode_string_xj
	leave
	ret
	.globl	__Z30react_output_arg_unicode_str_8Pv
	.def	__Z30react_output_arg_unicode_str_8Pv;	.scl	2;	.type	32;	.endef
__Z30react_output_arg_unicode_str_8Pv:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$24, %esp
	movl	$8, (%esp)
	call	__Z27output_arg_unicode_string_xj
	leave
	ret
	.section .rdata,"dr"
	.align 4
LC40:
	.ascii "Outputting arg %d as ANSI string\12\0"
LC41:
	.ascii "ESP: 0x%08x\12\0"
LC42:
	.ascii "arg addr: 0x%08x\12\0"
LC43:
	.ascii "str addr: 0x%08x\12\0"
	.align 4
LC44:
	.ascii "# Failed to read ANSI string @ 0x%08x\12\0"
	.text
	.globl	__Z19report_arg_string_xj
	.def	__Z19report_arg_string_xj;	.scl	2;	.type	32;	.endef
__Z19report_arg_string_xj:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$1800, %esp
	movl	8(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC40, (%esp)
	call	__Z7d_printPKcz
	leal	-1504(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$0, (%esp)
	call	__Z12read_contextmP8_CONTEXT
	movl	-12(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC41, (%esp)
	call	__Z7d_printPKcz
	movl	-1308(%ebp), %eax
	movl	%eax, -12(%ebp)
	movl	8(%ebp), %eax
	sall	$2, %eax
	addl	%eax, -12(%ebp)
	movl	-12(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC42, (%esp)
	call	__Z7d_printPKcz
	movl	-12(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z10read_dwordm
	movl	%eax, -16(%ebp)
	movl	-16(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC43, (%esp)
	call	__Z7d_printPKcz
	movl	-16(%ebp), %edx
	movl	_my_trace, %eax
	movl	88(%eax), %eax
	leal	-20(%ebp), %ecx
	movl	%ecx, 16(%esp)
	movl	$256, 12(%esp)
	leal	-1760(%ebp), %ecx
	movl	%ecx, 8(%esp)
	movl	%edx, 4(%esp)
	movl	%eax, (%esp)
	call	__Z11read_memoryPvS_S_mPm
	movl	-20(%ebp), %eax
	testl	%eax, %eax
	je	L72
	movl	_my_trace, %eax
	movl	$7, 67915420(%eax)
	leal	-1760(%ebp), %eax
	movl	%eax, 4(%esp)
	leal	-788(%ebp), %eax
	movl	%eax, (%esp)
	call	_strcpy
	movl	_my_trace, %eax
	leal	67915424(%eax), %edx
	leal	-788(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	%edx, (%esp)
	call	_strcpy
	jmp	L73
L72:
	movl	_my_trace, %eax
	movl	$7, 67915420(%eax)
	movl	-12(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	$LC44, 4(%esp)
	leal	-788(%ebp), %eax
	movl	%eax, (%esp)
	call	_sprintf
	movl	_my_trace, %eax
	leal	67915424(%eax), %edx
	leal	-788(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	%edx, (%esp)
	call	_strcpy
L73:
	nop
	leave
	ret
	.section .rdata,"dr"
LC45:
	.ascii "OU,0x%x,Arg%d: %s\12\0"
	.text
	.globl	__Z19output_arg_string_xj
	.def	__Z19output_arg_string_xj;	.scl	2;	.type	32;	.endef
__Z19output_arg_string_xj:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$1800, %esp
	movl	8(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC40, (%esp)
	call	__Z7d_printPKcz
	leal	-1504(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$0, (%esp)
	call	__Z12read_contextmP8_CONTEXT
	movl	-12(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC41, (%esp)
	call	__Z7d_printPKcz
	movl	-1308(%ebp), %eax
	movl	%eax, -12(%ebp)
	movl	8(%ebp), %eax
	sall	$2, %eax
	addl	%eax, -12(%ebp)
	movl	-12(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC42, (%esp)
	call	__Z7d_printPKcz
	movl	-12(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z10read_dwordm
	movl	%eax, -16(%ebp)
	movl	-16(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC43, (%esp)
	call	__Z7d_printPKcz
	movl	-16(%ebp), %edx
	movl	_my_trace, %eax
	movl	88(%eax), %eax
	leal	-20(%ebp), %ecx
	movl	%ecx, 16(%esp)
	movl	$256, 12(%esp)
	leal	-1760(%ebp), %ecx
	movl	%ecx, 8(%esp)
	movl	%edx, 4(%esp)
	movl	%eax, (%esp)
	call	__Z11read_memoryPvS_S_mPm
	movl	-20(%ebp), %eax
	testl	%eax, %eax
	je	L76
	movl	_my_trace, %eax
	movl	68964276(%eax), %eax
	leal	-1760(%ebp), %edx
	movl	%edx, 16(%esp)
	movl	8(%ebp), %edx
	movl	%edx, 12(%esp)
	movl	%eax, 8(%esp)
	movl	$LC45, 4(%esp)
	leal	-788(%ebp), %eax
	movl	%eax, (%esp)
	call	_sprintf
	leal	-788(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z13add_to_bufferPc
	jmp	L77
L76:
	movl	-12(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	$LC44, 4(%esp)
	leal	-788(%ebp), %eax
	movl	%eax, (%esp)
	call	_sprintf
	leal	-788(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z13add_to_bufferPc
L77:
	nop
	leave
	ret
	.globl	__Z22react_output_arg_str_0Pv
	.def	__Z22react_output_arg_str_0Pv;	.scl	2;	.type	32;	.endef
__Z22react_output_arg_str_0Pv:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$24, %esp
	movl	$0, (%esp)
	call	__Z19output_arg_string_xj
	leave
	ret
	.globl	__Z22react_output_arg_str_1Pv
	.def	__Z22react_output_arg_str_1Pv;	.scl	2;	.type	32;	.endef
__Z22react_output_arg_str_1Pv:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$24, %esp
	movl	$1, (%esp)
	call	__Z19output_arg_string_xj
	leave
	ret
	.globl	__Z22react_output_arg_str_2Pv
	.def	__Z22react_output_arg_str_2Pv;	.scl	2;	.type	32;	.endef
__Z22react_output_arg_str_2Pv:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$24, %esp
	movl	$2, (%esp)
	call	__Z19output_arg_string_xj
	leave
	ret
	.globl	__Z22react_output_arg_str_3Pv
	.def	__Z22react_output_arg_str_3Pv;	.scl	2;	.type	32;	.endef
__Z22react_output_arg_str_3Pv:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$24, %esp
	movl	$3, (%esp)
	call	__Z19output_arg_string_xj
	leave
	ret
	.globl	__Z22react_output_arg_str_4Pv
	.def	__Z22react_output_arg_str_4Pv;	.scl	2;	.type	32;	.endef
__Z22react_output_arg_str_4Pv:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$24, %esp
	movl	$4, (%esp)
	call	__Z19output_arg_string_xj
	leave
	ret
	.globl	__Z22react_output_arg_str_5Pv
	.def	__Z22react_output_arg_str_5Pv;	.scl	2;	.type	32;	.endef
__Z22react_output_arg_str_5Pv:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$24, %esp
	movl	$5, (%esp)
	call	__Z19output_arg_string_xj
	leave
	ret
	.globl	__Z22react_output_arg_str_6Pv
	.def	__Z22react_output_arg_str_6Pv;	.scl	2;	.type	32;	.endef
__Z22react_output_arg_str_6Pv:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$24, %esp
	movl	$6, (%esp)
	call	__Z19output_arg_string_xj
	leave
	ret
	.globl	__Z22react_output_arg_str_7Pv
	.def	__Z22react_output_arg_str_7Pv;	.scl	2;	.type	32;	.endef
__Z22react_output_arg_str_7Pv:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$24, %esp
	movl	$7, (%esp)
	call	__Z19output_arg_string_xj
	leave
	ret
	.globl	__Z22react_output_arg_str_8Pv
	.def	__Z22react_output_arg_str_8Pv;	.scl	2;	.type	32;	.endef
__Z22react_output_arg_str_8Pv:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$24, %esp
	movl	$8, (%esp)
	call	__Z19output_arg_string_xj
	leave
	ret
	.section .rdata,"dr"
LC46:
	.ascii "0x%08x\0"
	.text
	.globl	__Z12report_arg_xj
	.def	__Z12report_arg_xj;	.scl	2;	.type	32;	.endef
__Z12report_arg_xj:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$1528, %esp
	movl	8(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC36, (%esp)
	call	__Z7d_printPKcz
	leal	-1500(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$0, (%esp)
	call	__Z12read_contextmP8_CONTEXT
	movl	-1304(%ebp), %eax
	movl	%eax, -12(%ebp)
	movl	8(%ebp), %eax
	sall	$2, %eax
	addl	%eax, -12(%ebp)
	movl	-12(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z10read_dwordm
	movl	%eax, -16(%ebp)
	movl	_my_trace, %eax
	movl	$7, 67915420(%eax)
	movl	-16(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	$LC46, 4(%esp)
	leal	-784(%ebp), %eax
	movl	%eax, (%esp)
	call	_sprintf
	movl	_my_trace, %eax
	leal	67915424(%eax), %edx
	leal	-784(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	%edx, (%esp)
	call	_strcpy
	nop
	leave
	ret
	.section .rdata,"dr"
LC47:
	.ascii "OU,0x%x,Arg%d: %08x\12\0"
	.text
	.globl	__Z12output_arg_xj
	.def	__Z12output_arg_xj;	.scl	2;	.type	32;	.endef
__Z12output_arg_xj:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$1544, %esp
	movl	8(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC36, (%esp)
	call	__Z7d_printPKcz
	leal	-1500(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$0, (%esp)
	call	__Z12read_contextmP8_CONTEXT
	movl	-1304(%ebp), %eax
	movl	%eax, -12(%ebp)
	movl	8(%ebp), %eax
	sall	$2, %eax
	addl	%eax, -12(%ebp)
	movl	-12(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z10read_dwordm
	movl	%eax, -16(%ebp)
	movl	_my_trace, %eax
	movl	68964276(%eax), %eax
	movl	-16(%ebp), %edx
	movl	%edx, 16(%esp)
	movl	8(%ebp), %edx
	movl	%edx, 12(%esp)
	movl	%eax, 8(%esp)
	movl	$LC47, 4(%esp)
	leal	-784(%ebp), %eax
	movl	%eax, (%esp)
	call	_sprintf
	leal	-784(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z13add_to_bufferPc
	nop
	leave
	ret
	.globl	__Z18react_output_arg_0Pv
	.def	__Z18react_output_arg_0Pv;	.scl	2;	.type	32;	.endef
__Z18react_output_arg_0Pv:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$24, %esp
	movl	$0, (%esp)
	call	__Z12output_arg_xj
	leave
	ret
	.globl	__Z18react_output_arg_1Pv
	.def	__Z18react_output_arg_1Pv;	.scl	2;	.type	32;	.endef
__Z18react_output_arg_1Pv:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$24, %esp
	movl	$1, (%esp)
	call	__Z12output_arg_xj
	leave
	ret
	.globl	__Z18react_output_arg_2Pv
	.def	__Z18react_output_arg_2Pv;	.scl	2;	.type	32;	.endef
__Z18react_output_arg_2Pv:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$24, %esp
	movl	$2, (%esp)
	call	__Z12output_arg_xj
	leave
	ret
	.globl	__Z18react_output_arg_3Pv
	.def	__Z18react_output_arg_3Pv;	.scl	2;	.type	32;	.endef
__Z18react_output_arg_3Pv:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$24, %esp
	movl	$3, (%esp)
	call	__Z12output_arg_xj
	leave
	ret
	.globl	__Z18react_output_arg_4Pv
	.def	__Z18react_output_arg_4Pv;	.scl	2;	.type	32;	.endef
__Z18react_output_arg_4Pv:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$24, %esp
	movl	$4, (%esp)
	call	__Z12output_arg_xj
	leave
	ret
	.globl	__Z18react_output_arg_5Pv
	.def	__Z18react_output_arg_5Pv;	.scl	2;	.type	32;	.endef
__Z18react_output_arg_5Pv:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$24, %esp
	movl	$5, (%esp)
	call	__Z12output_arg_xj
	leave
	ret
	.globl	__Z18react_output_arg_6Pv
	.def	__Z18react_output_arg_6Pv;	.scl	2;	.type	32;	.endef
__Z18react_output_arg_6Pv:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$24, %esp
	movl	$6, (%esp)
	call	__Z12output_arg_xj
	leave
	ret
	.globl	__Z18react_output_arg_7Pv
	.def	__Z18react_output_arg_7Pv;	.scl	2;	.type	32;	.endef
__Z18react_output_arg_7Pv:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$24, %esp
	movl	$7, (%esp)
	call	__Z12output_arg_xj
	leave
	ret
	.globl	__Z18react_output_arg_8Pv
	.def	__Z18react_output_arg_8Pv;	.scl	2;	.type	32;	.endef
__Z18react_output_arg_8Pv:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$24, %esp
	movl	$8, (%esp)
	call	__Z12output_arg_xj
	leave
	ret
	.section .rdata,"dr"
LC48:
	.ascii "OU,0x%x,Arg_p%d: %08x\12\0"
	.text
	.globl	__Z14output_p_arg_xj
	.def	__Z14output_p_arg_xj;	.scl	2;	.type	32;	.endef
__Z14output_p_arg_xj:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$1544, %esp
	movl	8(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC36, (%esp)
	call	__Z7d_printPKcz
	leal	-1500(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$0, (%esp)
	call	__Z12read_contextmP8_CONTEXT
	movl	-1304(%ebp), %eax
	movl	%eax, -12(%ebp)
	movl	8(%ebp), %eax
	sall	$2, %eax
	addl	%eax, -12(%ebp)
	movl	-12(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z10read_dwordm
	movl	%eax, -16(%ebp)
	movl	-16(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z10read_dwordm
	movl	%eax, -16(%ebp)
	movl	_my_trace, %eax
	movl	68964276(%eax), %eax
	movl	-16(%ebp), %edx
	movl	%edx, 16(%esp)
	movl	8(%ebp), %edx
	movl	%edx, 12(%esp)
	movl	%eax, 8(%esp)
	movl	$LC48, 4(%esp)
	leal	-784(%ebp), %eax
	movl	%eax, (%esp)
	call	_sprintf
	leal	-784(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z13add_to_bufferPc
	nop
	leave
	ret
	.globl	__Z20react_output_p_arg_0Pv
	.def	__Z20react_output_p_arg_0Pv;	.scl	2;	.type	32;	.endef
__Z20react_output_p_arg_0Pv:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$24, %esp
	movl	$0, (%esp)
	call	__Z14output_p_arg_xj
	leave
	ret
	.globl	__Z20react_output_p_arg_1Pv
	.def	__Z20react_output_p_arg_1Pv;	.scl	2;	.type	32;	.endef
__Z20react_output_p_arg_1Pv:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$24, %esp
	movl	$1, (%esp)
	call	__Z14output_p_arg_xj
	leave
	ret
	.globl	__Z20react_output_p_arg_2Pv
	.def	__Z20react_output_p_arg_2Pv;	.scl	2;	.type	32;	.endef
__Z20react_output_p_arg_2Pv:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$24, %esp
	movl	$2, (%esp)
	call	__Z14output_p_arg_xj
	leave
	ret
	.globl	__Z20react_output_p_arg_3Pv
	.def	__Z20react_output_p_arg_3Pv;	.scl	2;	.type	32;	.endef
__Z20react_output_p_arg_3Pv:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$24, %esp
	movl	$3, (%esp)
	call	__Z14output_p_arg_xj
	leave
	ret
	.globl	__Z20react_output_p_arg_4Pv
	.def	__Z20react_output_p_arg_4Pv;	.scl	2;	.type	32;	.endef
__Z20react_output_p_arg_4Pv:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$24, %esp
	movl	$4, (%esp)
	call	__Z14output_p_arg_xj
	leave
	ret
	.globl	__Z20react_output_p_arg_5Pv
	.def	__Z20react_output_p_arg_5Pv;	.scl	2;	.type	32;	.endef
__Z20react_output_p_arg_5Pv:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$24, %esp
	movl	$5, (%esp)
	call	__Z14output_p_arg_xj
	leave
	ret
	.globl	__Z20react_output_p_arg_6Pv
	.def	__Z20react_output_p_arg_6Pv;	.scl	2;	.type	32;	.endef
__Z20react_output_p_arg_6Pv:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$24, %esp
	movl	$6, (%esp)
	call	__Z14output_p_arg_xj
	leave
	ret
	.globl	__Z20react_output_p_arg_7Pv
	.def	__Z20react_output_p_arg_7Pv;	.scl	2;	.type	32;	.endef
__Z20react_output_p_arg_7Pv:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$24, %esp
	movl	$7, (%esp)
	call	__Z14output_p_arg_xj
	leave
	ret
	.globl	__Z20react_output_p_arg_8Pv
	.def	__Z20react_output_p_arg_8Pv;	.scl	2;	.type	32;	.endef
__Z20react_output_p_arg_8Pv:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$24, %esp
	movl	$8, (%esp)
	call	__Z14output_p_arg_xj
	leave
	ret
	.section .rdata,"dr"
LC49:
	.ascii "Modifying address: 0x%08x\12\0"
	.text
	.globl	__Z21react_cry_antidebug_1Pv
	.def	__Z21react_cry_antidebug_1Pv;	.scl	2;	.type	32;	.endef
__Z21react_cry_antidebug_1Pv:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$56, %esp
	movl	$4260836, -12(%ebp)
	movb	$-7, -17(%ebp)
	movb	$1, -18(%ebp)
	movl	-12(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC49, (%esp)
	call	__Z7d_printPKcz
	movl	-12(%ebp), %edx
	movl	_my_trace, %eax
	movl	128(%eax), %eax
	leal	-16(%ebp), %ecx
	movl	%ecx, 16(%esp)
	movl	$1, 12(%esp)
	leal	-17(%ebp), %ecx
	movl	%ecx, 8(%esp)
	movl	%edx, 4(%esp)
	movl	%eax, (%esp)
	call	__Z12write_memoryPvS_S_mPm
	addl	$1, -12(%ebp)
	movl	-12(%ebp), %edx
	movl	_my_trace, %eax
	movl	128(%eax), %eax
	leal	-16(%ebp), %ecx
	movl	%ecx, 16(%esp)
	movl	$1, 12(%esp)
	leal	-18(%ebp), %ecx
	movl	%ecx, 8(%esp)
	movl	%edx, 4(%esp)
	movl	%eax, (%esp)
	call	__Z12write_memoryPvS_S_mPm
	nop
	leave
	ret
	.section .rdata,"dr"
LC50:
	.ascii "Executing routine 0x%02x\12\0"
	.text
	.globl	__Z11run_routinej
	.def	__Z11run_routinej;	.scl	2;	.type	32;	.endef
__Z11run_routinej:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$24, %esp
	movl	8(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC50, (%esp)
	call	__Z7d_printPKcz
	movl	_my_trace, %eax
	movl	8(%ebp), %edx
	addl	$17505476, %edx
	movl	(%eax,%edx,4), %eax
	movl	$0, (%esp)
	call	*%eax
	leave
	ret
	.section .rdata,"dr"
	.align 4
LC51:
	.ascii "Writing ANSI string to 0x%08x\12\0"
LC52:
	.ascii "# Wrote ANSI string @ 0x%08x\12\0"
	.align 4
LC53:
	.ascii "# Failed to write ANSI string @ 0x%08x\12\0"
	.text
	.globl	__Z18write_string_asciijPc
	.def	__Z18write_string_asciijPc;	.scl	2;	.type	32;	.endef
__Z18write_string_asciijPc:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%ebx
	subl	$820, %esp
	movl	8(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC51, (%esp)
	call	__Z7d_printPKcz
	movl	12(%ebp), %eax
	movl	%eax, (%esp)
	call	_strlen
	movl	%eax, %ebx
	movl	8(%ebp), %edx
	movl	_my_trace, %eax
	movl	88(%eax), %eax
	leal	-12(%ebp), %ecx
	movl	%ecx, 16(%esp)
	movl	%ebx, 12(%esp)
	movl	12(%ebp), %ecx
	movl	%ecx, 8(%esp)
	movl	%edx, 4(%esp)
	movl	%eax, (%esp)
	call	__Z12write_memoryPvS_S_mPm
	movl	-12(%ebp), %eax
	testl	%eax, %eax
	je	L116
	movl	8(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	$LC52, 4(%esp)
	leal	-780(%ebp), %eax
	movl	%eax, (%esp)
	call	_sprintf
	leal	-780(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z13add_to_bufferPc
	jmp	L117
L116:
	movl	8(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	$LC53, 4(%esp)
	leal	-780(%ebp), %eax
	movl	%eax, (%esp)
	call	_sprintf
	leal	-780(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z13add_to_bufferPc
L117:
	nop
	addl	$820, %esp
	popl	%ebx
	popl	%ebp
	ret
	.section .rdata,"dr"
	.align 4
LC54:
	.ascii "Writing UNICODE string to 0x%08x\12\0"
	.align 4
LC55:
	.ascii "# Wrote UNICODE string @ 0x%08x\12\0"
LC56:
	.ascii "Success\12\0"
	.align 4
LC57:
	.ascii "# Failed to write UNICODE string @ 0x%08x\12\0"
LC58:
	.ascii "Failed\12\0"
	.text
	.globl	__Z20write_string_unicodejPc
	.def	__Z20write_string_unicodejPc;	.scl	2;	.type	32;	.endef
__Z20write_string_unicodejPc:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$824, %esp
	movl	8(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC54, (%esp)
	call	__Z7d_printPKcz
	movl	12(%ebp), %eax
	movl	%eax, (%esp)
	call	_strlen
	addl	$1, %eax
	addl	%eax, %eax
	movl	%eax, -12(%ebp)
	movl	-12(%ebp), %eax
	movl	%eax, (%esp)
	call	_malloc
	movl	%eax, -16(%ebp)
	movl	12(%ebp), %eax
	movl	%eax, (%esp)
	call	_strlen
	addl	$1, %eax
	movl	%eax, 20(%esp)
	movl	-16(%ebp), %eax
	movl	%eax, 16(%esp)
	movl	$-1, 12(%esp)
	movl	12(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	$0, 4(%esp)
	movl	$1, (%esp)
	movl	__imp__MultiByteToWideChar@24, %eax
	call	*%eax
	subl	$24, %esp
	movl	%eax, -20(%ebp)
	movl	8(%ebp), %edx
	movl	_my_trace, %eax
	movl	88(%eax), %eax
	leal	-24(%ebp), %ecx
	movl	%ecx, 16(%esp)
	movl	-12(%ebp), %ecx
	movl	%ecx, 12(%esp)
	movl	-16(%ebp), %ecx
	movl	%ecx, 8(%esp)
	movl	%edx, 4(%esp)
	movl	%eax, (%esp)
	call	__Z12write_memoryPvS_S_mPm
	movl	-24(%ebp), %eax
	cmpl	-12(%ebp), %eax
	jne	L120
	movl	8(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	$LC55, 4(%esp)
	leal	-792(%ebp), %eax
	movl	%eax, (%esp)
	call	_sprintf
	movl	$LC56, (%esp)
	call	__Z7d_printPKcz
	leal	-792(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z13add_to_bufferPc
	jmp	L121
L120:
	movl	8(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	$LC57, 4(%esp)
	leal	-792(%ebp), %eax
	movl	%eax, (%esp)
	call	_sprintf
	movl	$LC58, (%esp)
	call	__Z7d_printPKcz
	leal	-792(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z13add_to_bufferPc
L121:
	movl	-16(%ebp), %eax
	movl	%eax, (%esp)
	call	_free
	nop
	leave
	ret
	.section .rdata,"dr"
	.align 4
LC59:
	.ascii "# Increasing skipping in TID: 0x%08x @ %p\12\0"
LC60:
	.ascii "Skipping in TID: 0x%08x...\12\0"
	.align 4
LC61:
	.ascii "# Started skipping in TID: 0x%08x\12\0"
	.text
	.globl	__Z13react_skip_onPv
	.def	__Z13react_skip_onPv;	.scl	2;	.type	32;	.endef
__Z13react_skip_onPv:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%esi
	pushl	%ebx
	subl	$800, %esp
	movl	8(%ebp), %eax
	movl	%eax, -12(%ebp)
	movl	-12(%ebp), %eax
	movl	8(%eax), %eax
	movl	%eax, -16(%ebp)
	movl	_my_trace, %ebx
	movl	_my_trace, %eax
	movl	-16(%ebp), %edx
	addl	$32, %edx
	movl	8(%eax,%edx,4), %edx
	movl	_my_trace, %esi
	movl	_my_trace, %eax
	movl	-16(%ebp), %ecx
	addl	$32, %ecx
	movl	8(%eax,%ecx,4), %ecx
	movl	%ecx, %eax
	sall	$2, %eax
	addl	%ecx, %eax
	sall	$2, %eax
	addl	%esi, %eax
	addl	$67108992, %eax
	movzbl	20(%eax), %eax
	addl	$1, %eax
	movl	%eax, %ecx
	movl	%edx, %eax
	sall	$2, %eax
	addl	%edx, %eax
	sall	$2, %eax
	addl	%ebx, %eax
	addl	$67108992, %eax
	movb	%cl, 20(%eax)
	movl	_my_trace, %eax
	movl	68964200(%eax), %eax
	movl	%eax, 12(%esp)
	movl	-16(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	$LC59, 4(%esp)
	leal	-784(%ebp), %eax
	movl	%eax, (%esp)
	call	_sprintf
	leal	-784(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z13add_to_bufferPc
	movl	_my_trace, %ecx
	movl	_my_trace, %eax
	movl	-16(%ebp), %edx
	addl	$32, %edx
	movl	8(%eax,%edx,4), %edx
	movl	%edx, %eax
	sall	$2, %eax
	addl	%edx, %eax
	sall	$2, %eax
	addl	%ecx, %eax
	addl	$67108992, %eax
	movzbl	20(%eax), %eax
	cmpb	$1, %al
	jne	L124
	movl	-16(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z8unset_ssm
	movl	-16(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC60, (%esp)
	call	__Z7d_printPKcz
	movl	-16(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	$LC61, 4(%esp)
	leal	-784(%ebp), %eax
	movl	%eax, (%esp)
	call	_sprintf
	leal	-784(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z13add_to_bufferPc
L124:
	nop
	addl	$800, %esp
	popl	%ebx
	popl	%esi
	popl	%ebp
	ret
	.section .rdata,"dr"
	.align 4
LC62:
	.ascii "# Decreasing skipping in TID: 0x%08x @ %p\12\0"
	.align 4
LC63:
	.ascii "Finished skippingin TID: 0x%08x\12\0"
	.align 4
LC64:
	.ascii "# Stopped skipping in TID: 0x%08x\12\0"
	.text
	.globl	__Z14react_skip_offPv
	.def	__Z14react_skip_offPv;	.scl	2;	.type	32;	.endef
__Z14react_skip_offPv:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%esi
	pushl	%ebx
	subl	$800, %esp
	movl	8(%ebp), %eax
	movl	%eax, -12(%ebp)
	movl	-12(%ebp), %eax
	movl	8(%eax), %eax
	movl	%eax, -16(%ebp)
	movl	_my_trace, %ebx
	movl	_my_trace, %eax
	movl	-16(%ebp), %edx
	addl	$32, %edx
	movl	8(%eax,%edx,4), %edx
	movl	_my_trace, %esi
	movl	_my_trace, %eax
	movl	-16(%ebp), %ecx
	addl	$32, %ecx
	movl	8(%eax,%ecx,4), %ecx
	movl	%ecx, %eax
	sall	$2, %eax
	addl	%ecx, %eax
	sall	$2, %eax
	addl	%esi, %eax
	addl	$67108992, %eax
	movzbl	20(%eax), %eax
	subl	$1, %eax
	movl	%eax, %ecx
	movl	%edx, %eax
	sall	$2, %eax
	addl	%edx, %eax
	sall	$2, %eax
	addl	%ebx, %eax
	addl	$67108992, %eax
	movb	%cl, 20(%eax)
	movl	_my_trace, %eax
	movl	68964200(%eax), %eax
	movl	%eax, 12(%esp)
	movl	-16(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	$LC62, 4(%esp)
	leal	-784(%ebp), %eax
	movl	%eax, (%esp)
	call	_sprintf
	leal	-784(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z13add_to_bufferPc
	movl	_my_trace, %ecx
	movl	_my_trace, %eax
	movl	-16(%ebp), %edx
	addl	$32, %edx
	movl	8(%eax,%edx,4), %edx
	movl	%edx, %eax
	sall	$2, %eax
	addl	%edx, %eax
	sall	$2, %eax
	addl	%ecx, %eax
	addl	$67108992, %eax
	movzbl	20(%eax), %eax
	testb	%al, %al
	jg	L127
	movl	-16(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC63, (%esp)
	call	__Z7d_printPKcz
	movl	_my_trace, %ecx
	movl	_my_trace, %eax
	movl	-16(%ebp), %edx
	addl	$32, %edx
	movl	8(%eax,%edx,4), %edx
	movl	%edx, %eax
	sall	$2, %eax
	addl	%edx, %eax
	sall	$2, %eax
	addl	%ecx, %eax
	addl	$67108992, %eax
	movb	$0, 20(%eax)
	movl	-16(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z6set_ssm
	movl	-16(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	$LC64, 4(%esp)
	leal	-784(%ebp), %eax
	movl	%eax, (%esp)
	call	_sprintf
	leal	-784(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z13add_to_bufferPc
L127:
	nop
	addl	$800, %esp
	popl	%ebx
	popl	%esi
	popl	%ebp
	ret
	.globl	__Z20enable_all_reactionsv
	.def	__Z20enable_all_reactionsv;	.scl	2;	.type	32;	.endef
__Z20enable_all_reactionsv:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$40, %esp
	movl	$0, -12(%ebp)
	jmp	L130
L131:
	movl	_my_trace, %eax
	movl	70021896(%eax), %edx
	movl	-12(%ebp), %eax
	imull	$188, %eax, %eax
	addl	%edx, %eax
	movl	%eax, (%esp)
	call	__Z15enable_reactionPc
	addl	$1, -12(%ebp)
L130:
	movl	_my_trace, %eax
	movl	70021900(%eax), %eax
	cmpl	-12(%ebp), %eax
	ja	L131
	movl	$0, %eax
	leave
	ret
	.globl	__Z21disable_all_reactionsv
	.def	__Z21disable_all_reactionsv;	.scl	2;	.type	32;	.endef
__Z21disable_all_reactionsv:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$40, %esp
	movl	$0, -12(%ebp)
	jmp	L134
L135:
	movl	_my_trace, %eax
	movl	70021896(%eax), %edx
	movl	-12(%ebp), %eax
	imull	$188, %eax, %eax
	addl	%edx, %eax
	movl	%eax, (%esp)
	call	__Z16disable_reactionPc
	addl	$1, -12(%ebp)
L134:
	movl	_my_trace, %eax
	movl	70021900(%eax), %eax
	cmpl	-12(%ebp), %eax
	ja	L135
	movl	$0, %eax
	leave
	ret
	.section .rdata,"dr"
LC65:
	.ascii "[raise_reaction]\12\0"
LC66:
	.ascii "React string: %s\12\0"
LC67:
	.ascii ":\0"
LC68:
	.ascii "Found another reaction: %s\12\0"
	.align 4
LC69:
	.ascii "Setting exclusive reaction %s\12\0"
LC70:
	.ascii "[raise_reaction ends]\12\0"
	.text
	.globl	__Z14raise_reactionPc
	.def	__Z14raise_reactionPc;	.scl	2;	.type	32;	.endef
__Z14raise_reactionPc:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$824, %esp
	movl	$LC65, (%esp)
	call	__Z7d_printPKcz
	movl	8(%ebp), %eax
	movl	%eax, 4(%esp)
	leal	-796(%ebp), %eax
	movl	%eax, (%esp)
	call	_strcpy
	leal	-796(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC66, (%esp)
	call	__Z7d_printPKcz
	movl	$58, 4(%esp)
	leal	-796(%ebp), %eax
	movl	%eax, (%esp)
	call	_strchr
	testl	%eax, %eax
	je	L138
	movl	$LC67, 4(%esp)
	leal	-796(%ebp), %eax
	movl	%eax, (%esp)
	call	_strtok
	movl	%eax, -16(%ebp)
	jmp	L139
L140:
	movl	-16(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC68, (%esp)
	call	__Z7d_printPKcz
	movl	-16(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z18exclusive_reactionPc
	movl	$LC67, 4(%esp)
	movl	$0, (%esp)
	call	_strtok
	movl	%eax, -16(%ebp)
L139:
	cmpl	$0, -16(%ebp)
	jne	L140
	movl	$0, %eax
	jmp	L148
L138:
	movl	$0, -12(%ebp)
	jmp	L142
L147:
	movl	_my_trace, %eax
	movl	70021896(%eax), %edx
	movl	-12(%ebp), %eax
	imull	$188, %eax, %eax
	addl	%edx, %eax
	movl	%eax, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_strcmp
	testl	%eax, %eax
	jne	L143
	movl	8(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC69, (%esp)
	call	__Z7d_printPKcz
	movl	_my_trace, %eax
	movl	70021896(%eax), %edx
	movl	-12(%ebp), %eax
	imull	$188, %eax, %eax
	addl	%edx, %eax
	movl	176(%eax), %edx
	addl	$1, %edx
	movl	%edx, 176(%eax)
	movl	_my_trace, %eax
	movl	70021896(%eax), %edx
	movl	-12(%ebp), %eax
	imull	$188, %eax, %eax
	addl	%edx, %eax
	movl	%eax, -24(%ebp)
	movl	$0, -20(%ebp)
	jmp	L144
L146:
	movl	-24(%ebp), %ecx
	movl	-20(%ebp), %edx
	movl	%edx, %eax
	addl	%eax, %eax
	addl	%edx, %eax
	addl	%ecx, %eax
	movzbl	3(%eax), %eax
	testb	%al, %al
	je	L145
	movl	-20(%ebp), %edx
	movl	%edx, %eax
	addl	%eax, %eax
	addl	%edx, %eax
	movl	-24(%ebp), %edx
	addl	%edx, %eax
	addl	$3, %eax
	movl	%eax, (%esp)
	call	__Z13find_reactionPc
	movl	%eax, -28(%ebp)
	movl	-28(%ebp), %eax
	movl	176(%eax), %eax
	leal	1(%eax), %edx
	movl	-28(%ebp), %eax
	movl	%edx, 176(%eax)
L145:
	addl	$1, -20(%ebp)
L144:
	cmpl	$31, -20(%ebp)
	jbe	L146
L143:
	addl	$1, -12(%ebp)
L142:
	movl	_my_trace, %eax
	movl	70021900(%eax), %eax
	cmpl	-12(%ebp), %eax
	ja	L147
	movl	$LC70, (%esp)
	call	__Z7d_printPKcz
	movl	$0, %eax
L148:
	leave
	ret
	.section .rdata,"dr"
LC71:
	.ascii "[lower_reaction]\12\0"
LC72:
	.ascii "Lowering reaction %s\12\0"
LC73:
	.ascii "Lowering coupled reaction %s\12\0"
LC74:
	.ascii "[lower_reaction ends]\12\0"
	.text
	.globl	__Z14lower_reactionPc
	.def	__Z14lower_reactionPc;	.scl	2;	.type	32;	.endef
__Z14lower_reactionPc:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$824, %esp
	movl	$LC71, (%esp)
	call	__Z7d_printPKcz
	movl	8(%ebp), %eax
	movl	%eax, 4(%esp)
	leal	-796(%ebp), %eax
	movl	%eax, (%esp)
	call	_strcpy
	leal	-796(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC66, (%esp)
	call	__Z7d_printPKcz
	movl	$58, 4(%esp)
	leal	-796(%ebp), %eax
	movl	%eax, (%esp)
	call	_strchr
	testl	%eax, %eax
	je	L150
	movl	$LC67, 4(%esp)
	leal	-796(%ebp), %eax
	movl	%eax, (%esp)
	call	_strtok
	movl	%eax, -16(%ebp)
	jmp	L151
L152:
	movl	-16(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC68, (%esp)
	call	__Z7d_printPKcz
	movl	-16(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z14lower_reactionPc
	movl	$LC67, 4(%esp)
	movl	$0, (%esp)
	call	_strtok
	movl	%eax, -16(%ebp)
L151:
	cmpl	$0, -16(%ebp)
	jne	L152
	movl	$0, %eax
	jmp	L160
L150:
	movl	$0, -12(%ebp)
	jmp	L154
L159:
	movl	_my_trace, %eax
	movl	70021896(%eax), %edx
	movl	-12(%ebp), %eax
	imull	$188, %eax, %eax
	addl	%edx, %eax
	movl	%eax, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_strcmp
	testl	%eax, %eax
	jne	L155
	movl	8(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC72, (%esp)
	call	__Z7d_printPKcz
	movl	_my_trace, %eax
	movl	70021896(%eax), %edx
	movl	-12(%ebp), %eax
	imull	$188, %eax, %eax
	addl	%edx, %eax
	movl	176(%eax), %edx
	subl	$1, %edx
	movl	%edx, 176(%eax)
	movl	_my_trace, %eax
	movl	70021896(%eax), %edx
	movl	-12(%ebp), %eax
	imull	$188, %eax, %eax
	addl	%edx, %eax
	movl	%eax, -24(%ebp)
	movl	$0, -20(%ebp)
	jmp	L156
L158:
	movl	-24(%ebp), %ecx
	movl	-20(%ebp), %edx
	movl	%edx, %eax
	addl	%eax, %eax
	addl	%edx, %eax
	addl	%ecx, %eax
	movzbl	3(%eax), %eax
	testb	%al, %al
	je	L157
	movl	-20(%ebp), %edx
	movl	%edx, %eax
	addl	%eax, %eax
	addl	%edx, %eax
	movl	-24(%ebp), %edx
	addl	%edx, %eax
	addl	$3, %eax
	movl	%eax, (%esp)
	call	__Z13find_reactionPc
	movl	%eax, -28(%ebp)
	movl	-28(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC73, (%esp)
	call	__Z7d_printPKcz
	movl	-28(%ebp), %eax
	movl	176(%eax), %eax
	leal	-1(%eax), %edx
	movl	-28(%ebp), %eax
	movl	%edx, 176(%eax)
L157:
	addl	$1, -20(%ebp)
L156:
	cmpl	$31, -20(%ebp)
	jbe	L158
L155:
	addl	$1, -12(%ebp)
L154:
	movl	_my_trace, %eax
	movl	70021900(%eax), %eax
	cmpl	-12(%ebp), %eax
	ja	L159
	movl	$LC74, (%esp)
	call	__Z7d_printPKcz
	movl	$0, %eax
L160:
	leave
	ret
	.section .rdata,"dr"
LC75:
	.ascii "[exclusive_reaction]\12\0"
LC76:
	.ascii "[exclusive_reaction ends]\12\0"
	.text
	.globl	__Z18exclusive_reactionPc
	.def	__Z18exclusive_reactionPc;	.scl	2;	.type	32;	.endef
__Z18exclusive_reactionPc:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$808, %esp
	movl	$LC75, (%esp)
	call	__Z7d_printPKcz
	movl	8(%ebp), %eax
	movl	%eax, 4(%esp)
	leal	-784(%ebp), %eax
	movl	%eax, (%esp)
	call	_strcpy
	leal	-784(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC66, (%esp)
	call	__Z7d_printPKcz
	movl	$58, 4(%esp)
	leal	-784(%ebp), %eax
	movl	%eax, (%esp)
	call	_strchr
	testl	%eax, %eax
	je	L162
	movl	$LC67, 4(%esp)
	leal	-784(%ebp), %eax
	movl	%eax, (%esp)
	call	_strtok
	movl	%eax, -16(%ebp)
	jmp	L163
L164:
	movl	-16(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC68, (%esp)
	call	__Z7d_printPKcz
	movl	-16(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z18exclusive_reactionPc
	movl	$LC67, 4(%esp)
	movl	$0, (%esp)
	call	_strtok
	movl	%eax, -16(%ebp)
L163:
	cmpl	$0, -16(%ebp)
	jne	L164
	movl	$0, %eax
	jmp	L169
L162:
	movl	$0, -12(%ebp)
	jmp	L166
L168:
	movl	_my_trace, %eax
	movl	70021896(%eax), %edx
	movl	-12(%ebp), %eax
	imull	$188, %eax, %eax
	addl	%edx, %eax
	movl	%eax, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_strcmp
	testl	%eax, %eax
	jne	L167
	movl	8(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC69, (%esp)
	call	__Z7d_printPKcz
	movl	_my_trace, %eax
	movl	70021896(%eax), %edx
	movl	-12(%ebp), %eax
	imull	$188, %eax, %eax
	addl	%edx, %eax
	movb	$1, 173(%eax)
L167:
	addl	$1, -12(%ebp)
L166:
	movl	_my_trace, %eax
	movl	70021900(%eax), %eax
	cmpl	-12(%ebp), %eax
	ja	L168
	movl	$LC76, (%esp)
	call	__Z7d_printPKcz
	movl	$0, %eax
L169:
	leave
	ret
	.section .rdata,"dr"
LC77:
	.ascii "[enable_reaction]\12\0"
LC78:
	.ascii "Enabling reaction %s\12\0"
LC79:
	.ascii "[enable_reaction ends]\12\0"
	.text
	.globl	__Z15enable_reactionPc
	.def	__Z15enable_reactionPc;	.scl	2;	.type	32;	.endef
__Z15enable_reactionPc:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$808, %esp
	movl	$LC77, (%esp)
	call	__Z7d_printPKcz
	movl	8(%ebp), %eax
	movl	%eax, 4(%esp)
	leal	-788(%ebp), %eax
	movl	%eax, (%esp)
	call	_strcpy
	leal	-788(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC66, (%esp)
	call	__Z7d_printPKcz
	movl	$58, 4(%esp)
	leal	-788(%ebp), %eax
	movl	%eax, (%esp)
	call	_strchr
	testl	%eax, %eax
	je	L171
	movl	$LC67, 4(%esp)
	leal	-788(%ebp), %eax
	movl	%eax, (%esp)
	call	_strtok
	movl	%eax, -16(%ebp)
	jmp	L172
L173:
	movl	-16(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC68, (%esp)
	call	__Z7d_printPKcz
	movl	-16(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z15enable_reactionPc
	movl	$LC67, 4(%esp)
	movl	$0, (%esp)
	call	_strtok
	movl	%eax, -16(%ebp)
L172:
	cmpl	$0, -16(%ebp)
	jne	L173
	movl	$0, %eax
	jmp	L178
L171:
	movl	$0, -12(%ebp)
	jmp	L175
L177:
	movl	_my_trace, %eax
	movl	70021896(%eax), %edx
	movl	-12(%ebp), %eax
	imull	$188, %eax, %eax
	addl	%edx, %eax
	movl	%eax, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_strcmp
	testl	%eax, %eax
	jne	L176
	movl	8(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC78, (%esp)
	call	__Z7d_printPKcz
	movl	_my_trace, %eax
	movl	70021896(%eax), %edx
	movl	-12(%ebp), %eax
	imull	$188, %eax, %eax
	addl	%edx, %eax
	movb	$1, 172(%eax)
	movl	_my_trace, %eax
	movl	70021896(%eax), %edx
	movl	-12(%ebp), %eax
	imull	$188, %eax, %eax
	addl	%edx, %eax
	movl	180(%eax), %eax
	movl	%eax, -20(%ebp)
	movl	-20(%ebp), %eax
	movb	$1, 521(%eax)
	movl	-20(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z17update_breakpointP11BREAKPOINT_
L176:
	addl	$1, -12(%ebp)
L175:
	movl	_my_trace, %eax
	movl	70021900(%eax), %eax
	cmpl	-12(%ebp), %eax
	ja	L177
	movl	$LC79, (%esp)
	call	__Z7d_printPKcz
	movl	$0, %eax
L178:
	leave
	ret
	.section .rdata,"dr"
LC80:
	.ascii "[disable_reaction]\12\0"
LC81:
	.ascii "Disabling reaction %s\12\0"
	.align 4
LC82:
	.ascii "Verifying if BP is still enabled\12\0"
LC83:
	.ascii "Got BP @ %p\12\0"
LC84:
	.ascii "Current reaction is broken!\12\0"
LC85:
	.ascii "[disable_reaction ends]\12\0"
	.text
	.globl	__Z16disable_reactionPc
	.def	__Z16disable_reactionPc;	.scl	2;	.type	32;	.endef
__Z16disable_reactionPc:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$56, %esp
	movl	$LC80, (%esp)
	call	__Z7d_printPKcz
	movl	$0, -16(%ebp)
	movl	$0, -12(%ebp)
	jmp	L180
L183:
	movl	8(%ebp), %eax
	movzbl	(%eax), %edx
	movl	_my_trace, %eax
	movl	70021896(%eax), %ecx
	movl	-12(%ebp), %eax
	imull	$188, %eax, %eax
	addl	%ecx, %eax
	movzbl	(%eax), %eax
	cmpb	%al, %dl
	jne	L181
	movl	8(%ebp), %eax
	addl	$1, %eax
	movzbl	(%eax), %edx
	movl	_my_trace, %eax
	movl	70021896(%eax), %ecx
	movl	-12(%ebp), %eax
	imull	$188, %eax, %eax
	addl	%ecx, %eax
	movzbl	1(%eax), %eax
	cmpb	%al, %dl
	jne	L181
	movl	8(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC81, (%esp)
	call	__Z7d_printPKcz
	movl	_my_trace, %eax
	movl	70021896(%eax), %edx
	movl	-12(%ebp), %eax
	imull	$188, %eax, %eax
	addl	%edx, %eax
	movb	$0, 172(%eax)
	movl	$1, -16(%ebp)
	jmp	L182
L181:
	addl	$1, -12(%ebp)
L180:
	movl	_my_trace, %eax
	movl	70021900(%eax), %eax
	cmpl	-12(%ebp), %eax
	ja	L183
L182:
	cmpl	$0, -16(%ebp)
	jne	L184
	movl	$0, %eax
	jmp	L185
L184:
	movl	$LC82, (%esp)
	call	__Z7d_printPKcz
	movl	_my_trace, %eax
	movl	70021896(%eax), %edx
	movl	-12(%ebp), %eax
	imull	$188, %eax, %eax
	addl	%edx, %eax
	movl	180(%eax), %eax
	movl	%eax, -24(%ebp)
	movl	-24(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC83, (%esp)
	call	__Z7d_printPKcz
	movb	$0, -17(%ebp)
	movl	$0, -12(%ebp)
	jmp	L186
L190:
	movl	-24(%ebp), %eax
	movl	524(%eax), %eax
	movl	-12(%ebp), %edx
	sall	$2, %edx
	addl	%edx, %eax
	movl	(%eax), %eax
	movl	%eax, -28(%ebp)
	cmpl	$0, -28(%ebp)
	jne	L187
	movl	$LC84, (%esp)
	call	__Z7d_printPKcz
L187:
	movl	-28(%ebp), %eax
	movzbl	172(%eax), %eax
	testb	%al, %al
	je	L188
	movb	$1, -17(%ebp)
	jmp	L189
L188:
	addl	$1, -12(%ebp)
L186:
	movl	-24(%ebp), %eax
	movl	528(%eax), %eax
	cmpl	-12(%ebp), %eax
	ja	L190
L189:
	cmpb	$0, -17(%ebp)
	jne	L191
	movl	-24(%ebp), %eax
	movb	$0, 521(%eax)
L191:
	movl	-24(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z17update_breakpointP11BREAKPOINT_
	movl	$LC85, (%esp)
	call	__Z7d_printPKcz
	movl	$0, %eax
L185:
	leave
	ret
	.globl	__Z13add_to_bufferPc
	.def	__Z13add_to_bufferPc;	.scl	2;	.type	32;	.endef
__Z13add_to_bufferPc:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%ebx
	subl	$36, %esp
	movl	$0, -12(%ebp)
	movl	_my_trace, %eax
	movl	70012876(%eax), %ebx
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_strlen
	movl	%ebx, 12(%esp)
	movl	$1, 8(%esp)
	movl	%eax, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_fwrite
	movl	%eax, -12(%ebp)
	movl	_my_trace, %eax
	movl	70012876(%eax), %eax
	movl	%eax, (%esp)
	call	_fflush
	movl	-12(%ebp), %eax
	addl	$36, %esp
	popl	%ebx
	popl	%ebp
	ret
	.section .rdata,"dr"
LC86:
	.ascii "Context:\12\0"
LC87:
	.ascii "EAX:\11"
	.ascii "0x%08x\12\0"
LC88:
	.ascii "EBX:\11"
	.ascii "0x%08x\12\0"
LC89:
	.ascii "ECX:\11"
	.ascii "0x%08x\12\0"
LC90:
	.ascii "EDX:\11"
	.ascii "0x%08x\12\0"
LC91:
	.ascii "ESI:\11"
	.ascii "0x%08x\12\0"
LC92:
	.ascii "EDI:\11"
	.ascii "0x%08x\12\0"
LC93:
	.ascii "EBP:\11"
	.ascii "0x%08x\12\0"
LC94:
	.ascii "ESP:\11"
	.ascii "0x%08x\12\0"
LC95:
	.ascii "EFLAGS:\11"
	.ascii "0x%08x\12\0"
LC96:
	.ascii "EIP:\11"
	.ascii "0x%08x\12\0"
	.text
	.globl	__Z13print_contextP8_CONTEXT
	.def	__Z13print_contextP8_CONTEXT;	.scl	2;	.type	32;	.endef
__Z13print_contextP8_CONTEXT:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$24, %esp
	movl	$LC86, (%esp)
	call	__Z7d_printPKcz
	movl	8(%ebp), %eax
	movl	176(%eax), %eax
	movl	%eax, 4(%esp)
	movl	$LC87, (%esp)
	call	__Z7d_printPKcz
	movl	8(%ebp), %eax
	movl	164(%eax), %eax
	movl	%eax, 4(%esp)
	movl	$LC88, (%esp)
	call	__Z7d_printPKcz
	movl	8(%ebp), %eax
	movl	172(%eax), %eax
	movl	%eax, 4(%esp)
	movl	$LC89, (%esp)
	call	__Z7d_printPKcz
	movl	8(%ebp), %eax
	movl	168(%eax), %eax
	movl	%eax, 4(%esp)
	movl	$LC90, (%esp)
	call	__Z7d_printPKcz
	movl	8(%ebp), %eax
	movl	160(%eax), %eax
	movl	%eax, 4(%esp)
	movl	$LC91, (%esp)
	call	__Z7d_printPKcz
	movl	8(%ebp), %eax
	movl	156(%eax), %eax
	movl	%eax, 4(%esp)
	movl	$LC92, (%esp)
	call	__Z7d_printPKcz
	movl	8(%ebp), %eax
	movl	180(%eax), %eax
	movl	%eax, 4(%esp)
	movl	$LC93, (%esp)
	call	__Z7d_printPKcz
	movl	8(%ebp), %eax
	movl	196(%eax), %eax
	movl	%eax, 4(%esp)
	movl	$LC94, (%esp)
	call	__Z7d_printPKcz
	movl	8(%ebp), %eax
	movl	192(%eax), %eax
	movl	%eax, 4(%esp)
	movl	$LC95, (%esp)
	call	__Z7d_printPKcz
	movl	8(%ebp), %eax
	movl	184(%eax), %eax
	movl	%eax, 4(%esp)
	movl	$LC96, (%esp)
	call	__Z7d_printPKcz
	leave
	ret
	.section .rdata,"dr"
LC97:
	.ascii "Refreshing\12\0"
LC98:
	.ascii "Unsetting SS for 0x%08x\12\0"
	.text
	.globl	__Z20react_sysret_refreshPv
	.def	__Z20react_sysret_refreshPv;	.scl	2;	.type	32;	.endef
__Z20react_sysret_refreshPv:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$24, %esp
	movl	$LC97, (%esp)
	call	__Z7d_printPKcz
	movl	_my_trace, %eax
	movl	68964276(%eax), %eax
	movl	%eax, 4(%esp)
	movl	$LC98, (%esp)
	call	__Z7d_printPKcz
	movl	_my_trace, %eax
	movl	68964276(%eax), %eax
	movl	%eax, (%esp)
	call	__Z8unset_ssm
	movl	$0, (%esp)
	call	__Z6set_ssm
	leave
	ret
	.section .rdata,"dr"
LC99:
	.ascii "Error: 0x%08x\12\0"
	.text
	.globl	__Z11read_memoryPvS_S_mPm
	.def	__Z11read_memoryPvS_S_mPm;	.scl	2;	.type	32;	.endef
__Z11read_memoryPvS_S_mPm:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$56, %esp
	leal	-16(%ebp), %eax
	movl	%eax, 16(%esp)
	movl	$64, 12(%esp)
	movl	20(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	16(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	movl	__imp__VirtualProtectEx@20, %eax
	call	*%eax
	subl	$20, %esp
	movl	24(%ebp), %eax
	movl	%eax, 16(%esp)
	movl	20(%ebp), %eax
	movl	%eax, 12(%esp)
	movl	16(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	12(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	movl	__imp__ReadProcessMemory@20, %eax
	call	*%eax
	subl	$20, %esp
	movl	%eax, -12(%ebp)
	movl	-16(%ebp), %eax
	leal	-16(%ebp), %edx
	movl	%edx, 16(%esp)
	movl	%eax, 12(%esp)
	movl	20(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	16(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	movl	__imp__VirtualProtectEx@20, %eax
	call	*%eax
	subl	$20, %esp
	cmpl	$0, -12(%ebp)
	jne	L197
	movl	__imp__GetLastError@0, %eax
	call	*%eax
	movl	%eax, 4(%esp)
	movl	$LC99, (%esp)
	call	__Z7d_printPKcz
L197:
	nop
	leave
	ret
	.section .rdata,"dr"
LC100:
	.ascii "write_memory, handle: 0x%08x\12\0"
	.text
	.globl	__Z12write_memoryPvS_S_mPm
	.def	__Z12write_memoryPvS_S_mPm;	.scl	2;	.type	32;	.endef
__Z12write_memoryPvS_S_mPm:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$56, %esp
	movl	8(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC100, (%esp)
	call	__Z7d_printPKcz
	leal	-16(%ebp), %eax
	movl	%eax, 16(%esp)
	movl	$64, 12(%esp)
	movl	20(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	12(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	movl	__imp__VirtualProtectEx@20, %eax
	call	*%eax
	subl	$20, %esp
	movl	24(%ebp), %eax
	movl	%eax, 16(%esp)
	movl	20(%ebp), %eax
	movl	%eax, 12(%esp)
	movl	16(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	12(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	movl	__imp__WriteProcessMemory@20, %eax
	call	*%eax
	subl	$20, %esp
	movl	%eax, -12(%ebp)
	cmpl	$0, -12(%ebp)
	jne	L200
	movl	__imp__GetLastError@0, %eax
	call	*%eax
	movl	%eax, 4(%esp)
	movl	$LC99, (%esp)
	call	__Z7d_printPKcz
L200:
	movl	-16(%ebp), %eax
	leal	-16(%ebp), %edx
	movl	%edx, 16(%esp)
	movl	%eax, 12(%esp)
	movl	20(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	12(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	movl	__imp__VirtualProtectEx@20, %eax
	call	*%eax
	subl	$20, %esp
	leave
	ret
	.section .rdata,"dr"
LC101:
	.ascii "before: 0x%08x\12\0"
LC102:
	.ascii "after: 0x%08x\12\0"
	.text
	.globl	__Z7dec_eipm
	.def	__Z7dec_eipm;	.scl	2;	.type	32;	.endef
__Z7dec_eipm:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$760, %esp
	movl	$-1, -12(%ebp)
	movl	_my_trace, %eax
	movl	8(%ebp), %edx
	addl	$32, %edx
	movl	8(%eax,%edx,4), %eax
	movl	%eax, -16(%ebp)
	movl	_my_trace, %ecx
	movl	-16(%ebp), %edx
	movl	%edx, %eax
	sall	$2, %eax
	addl	%edx, %eax
	sall	$2, %eax
	addl	%ecx, %eax
	addl	$67108992, %eax
	movl	16(%eax), %eax
	movl	%eax, -12(%ebp)
	movl	$65537, -732(%ebp)
	leal	-732(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	-12(%ebp), %eax
	movl	%eax, (%esp)
	movl	__imp__GetThreadContext@8, %eax
	call	*%eax
	subl	$8, %esp
	testl	%eax, %eax
	sete	%al
	testb	%al, %al
	je	L202
	movl	__imp__GetLastError@0, %eax
	call	*%eax
	movl	%eax, 4(%esp)
	movl	$LC7, (%esp)
	call	__Z7d_printPKcz
L202:
	movl	-548(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC101, (%esp)
	call	__Z7d_printPKcz
	movl	-548(%ebp), %eax
	subl	$1, %eax
	movl	%eax, -548(%ebp)
	movl	-548(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC102, (%esp)
	call	__Z7d_printPKcz
	leal	-732(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	-12(%ebp), %eax
	movl	%eax, (%esp)
	movl	__imp__SetThreadContext@8, %eax
	call	*%eax
	subl	$8, %esp
	movl	$0, %eax
	leave
	ret
	.section .rdata,"dr"
LC103:
	.ascii "0x%08x,0x%08x,0x%08x\0"
	.text
	.globl	__Z19serialize_exception17_EXCEPTION_RECORDPc
	.def	__Z19serialize_exception17_EXCEPTION_RECORDPc;	.scl	2;	.type	32;	.endef
__Z19serialize_exception17_EXCEPTION_RECORDPc:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$40, %esp
	movl	20(%ebp), %ecx
	movl	12(%ebp), %edx
	movl	8(%ebp), %eax
	movl	%ecx, 16(%esp)
	movl	%edx, 12(%esp)
	movl	%eax, 8(%esp)
	movl	$LC103, 4(%esp)
	movl	88(%ebp), %eax
	movl	%eax, (%esp)
	call	_sprintf
	nop
	leave
	ret
	.globl	__Z21deserialize_exceptionP17_EXCEPTION_RECORDPc
	.def	__Z21deserialize_exceptionP17_EXCEPTION_RECORDPc;	.scl	2;	.type	32;	.endef
__Z21deserialize_exceptionP17_EXCEPTION_RECORDPc:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$40, %esp
	movl	8(%ebp), %eax
	movl	12(%eax), %ecx
	movl	8(%ebp), %eax
	movl	4(%eax), %edx
	movl	8(%ebp), %eax
	movl	(%eax), %eax
	movl	%ecx, 16(%esp)
	movl	%edx, 12(%esp)
	movl	%eax, 8(%esp)
	movl	$LC103, 4(%esp)
	movl	12(%ebp), %eax
	movl	%eax, (%esp)
	call	_sscanf
	nop
	leave
	ret
	.globl	__Z16serialize_threadP13_THREAD_ENTRYPc
	.def	__Z16serialize_threadP13_THREAD_ENTRYPc;	.scl	2;	.type	32;	.endef
__Z16serialize_threadP13_THREAD_ENTRYPc:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$40, %esp
	movl	8(%ebp), %eax
	movzbl	4(%eax), %eax
	movsbl	%al, %ecx
	movl	8(%ebp), %eax
	movl	8(%eax), %edx
	movl	8(%ebp), %eax
	movl	(%eax), %eax
	movl	%ecx, 16(%esp)
	movl	%edx, 12(%esp)
	movl	%eax, 8(%esp)
	movl	$LC103, 4(%esp)
	movl	12(%ebp), %eax
	movl	%eax, (%esp)
	call	_sprintf
	leave
	ret
	.section .rdata,"dr"
LC104:
	.ascii "0x%08x,%s\0"
	.text
	.globl	__Z13serialize_libP10_LIB_ENTRYPc
	.def	__Z13serialize_libP10_LIB_ENTRYPc;	.scl	2;	.type	32;	.endef
__Z13serialize_libP10_LIB_ENTRYPc:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$24, %esp
	movl	8(%ebp), %eax
	leal	8(%eax), %edx
	movl	8(%ebp), %eax
	movl	4(%eax), %eax
	movl	%edx, 12(%esp)
	movl	%eax, 8(%esp)
	movl	$LC104, 4(%esp)
	movl	12(%ebp), %eax
	movl	%eax, (%esp)
	call	_sprintf
	leave
	ret
	.section .rdata,"dr"
	.align 4
LC105:
	.ascii "0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x\0"
	.text
	.globl	__Z17serialize_context8_CONTEXTP10_LDT_ENTRYPc
	.def	__Z17serialize_context8_CONTEXTP10_LDT_ENTRYPc;	.scl	2;	.type	32;	.endef
__Z17serialize_context8_CONTEXTP10_LDT_ENTRYPc:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%edi
	pushl	%esi
	pushl	%ebx
	subl	$268, %esp
	movl	724(%ebp), %eax
	addl	$40, %eax
	movl	%eax, -28(%ebp)
	movl	724(%ebp), %eax
	addl	$40, %eax
	movzwl	2(%eax), %eax
	movzwl	%ax, %edx
	movl	%edx, -32(%ebp)
	movl	724(%ebp), %eax
	addl	$40, %eax
	movzwl	(%eax), %eax
	movzwl	%ax, %ecx
	movl	%ecx, -36(%ebp)
	movl	724(%ebp), %eax
	leal	32(%eax), %ebx
	movl	%ebx, -40(%ebp)
	movl	724(%ebp), %eax
	addl	$32, %eax
	movzwl	2(%eax), %eax
	movzwl	%ax, %esi
	movl	%esi, -44(%ebp)
	movl	724(%ebp), %eax
	addl	$32, %eax
	movzwl	(%eax), %eax
	movzwl	%ax, %edi
	movl	%edi, -48(%ebp)
	movl	724(%ebp), %eax
	leal	24(%eax), %edx
	movl	%edx, -52(%ebp)
	movl	724(%ebp), %eax
	addl	$24, %eax
	movzwl	2(%eax), %eax
	movzwl	%ax, %ecx
	movl	%ecx, -56(%ebp)
	movl	724(%ebp), %eax
	addl	$24, %eax
	movzwl	(%eax), %eax
	movzwl	%ax, %ebx
	movl	%ebx, -60(%ebp)
	movl	724(%ebp), %eax
	leal	16(%eax), %esi
	movl	%esi, -64(%ebp)
	movl	724(%ebp), %eax
	addl	$16, %eax
	movzwl	2(%eax), %eax
	movzwl	%ax, %edi
	movl	%edi, -68(%ebp)
	movl	724(%ebp), %eax
	addl	$16, %eax
	movzwl	(%eax), %eax
	movzwl	%ax, %edx
	movl	%edx, -72(%ebp)
	movl	724(%ebp), %eax
	leal	8(%eax), %ecx
	movl	%ecx, -76(%ebp)
	movl	724(%ebp), %eax
	addl	$8, %eax
	movzwl	2(%eax), %eax
	movzwl	%ax, %ebx
	movl	%ebx, -80(%ebp)
	movl	724(%ebp), %eax
	addl	$8, %eax
	movzwl	(%eax), %eax
	movzwl	%ax, %esi
	movl	%esi, -84(%ebp)
	movl	724(%ebp), %eax
	movzwl	2(%eax), %eax
	movzwl	%ax, %edi
	movl	%edi, -88(%ebp)
	movl	724(%ebp), %eax
	movzwl	(%eax), %eax
	movzwl	%ax, %edx
	movl	%edx, -92(%ebp)
	movl	208(%ebp), %ecx
	movl	%ecx, -96(%ebp)
	movl	196(%ebp), %ebx
	movl	%ebx, -100(%ebp)
	movl	160(%ebp), %esi
	movl	%esi, -104(%ebp)
	movl	156(%ebp), %edi
	movl	%edi, -108(%ebp)
	movl	152(%ebp), %edx
	movl	%edx, -112(%ebp)
	movl	148(%ebp), %ecx
	movl	%ecx, -116(%ebp)
	movl	192(%ebp), %ebx
	movl	%ebx, -120(%ebp)
	movl	200(%ebp), %esi
	movl	%esi, -124(%ebp)
	movl	204(%ebp), %edi
	movl	%edi, -128(%ebp)
	movl	188(%ebp), %edx
	movl	%edx, -132(%ebp)
	movl	164(%ebp), %ecx
	movl	%ecx, -136(%ebp)
	movl	168(%ebp), %edi
	movl	172(%ebp), %esi
	movl	176(%ebp), %ebx
	movl	180(%ebp), %ecx
	movl	184(%ebp), %edx
	movl	-28(%ebp), %eax
	movl	4(%eax), %eax
	movl	%eax, 140(%esp)
	movl	-32(%ebp), %eax
	movl	%eax, 136(%esp)
	movl	-36(%ebp), %eax
	movl	%eax, 132(%esp)
	movl	-40(%ebp), %eax
	movl	4(%eax), %eax
	movl	%eax, 128(%esp)
	movl	-44(%ebp), %eax
	movl	%eax, 124(%esp)
	movl	-48(%ebp), %eax
	movl	%eax, 120(%esp)
	movl	-52(%ebp), %eax
	movl	4(%eax), %eax
	movl	%eax, 116(%esp)
	movl	-56(%ebp), %eax
	movl	%eax, 112(%esp)
	movl	-60(%ebp), %eax
	movl	%eax, 108(%esp)
	movl	-64(%ebp), %eax
	movl	4(%eax), %eax
	movl	%eax, 104(%esp)
	movl	-68(%ebp), %eax
	movl	%eax, 100(%esp)
	movl	-72(%ebp), %eax
	movl	%eax, 96(%esp)
	movl	-76(%ebp), %eax
	movl	4(%eax), %eax
	movl	%eax, 92(%esp)
	movl	-80(%ebp), %eax
	movl	%eax, 88(%esp)
	movl	-84(%ebp), %eax
	movl	%eax, 84(%esp)
	movl	724(%ebp), %eax
	movl	4(%eax), %eax
	movl	%eax, 80(%esp)
	movl	-88(%ebp), %eax
	movl	%eax, 76(%esp)
	movl	-92(%ebp), %eax
	movl	%eax, 72(%esp)
	movl	-96(%ebp), %eax
	movl	%eax, 68(%esp)
	movl	-100(%ebp), %eax
	movl	%eax, 64(%esp)
	movl	-104(%ebp), %eax
	movl	%eax, 60(%esp)
	movl	-108(%ebp), %eax
	movl	%eax, 56(%esp)
	movl	-112(%ebp), %eax
	movl	%eax, 52(%esp)
	movl	-116(%ebp), %eax
	movl	%eax, 48(%esp)
	movl	-120(%ebp), %eax
	movl	%eax, 44(%esp)
	movl	-124(%ebp), %eax
	movl	%eax, 40(%esp)
	movl	-128(%ebp), %eax
	movl	%eax, 36(%esp)
	movl	-132(%ebp), %eax
	movl	%eax, 32(%esp)
	movl	-136(%ebp), %eax
	movl	%eax, 28(%esp)
	movl	%edi, 24(%esp)
	movl	%esi, 20(%esp)
	movl	%ebx, 16(%esp)
	movl	%ecx, 12(%esp)
	movl	%edx, 8(%esp)
	movl	$LC105, 4(%esp)
	movl	728(%ebp), %eax
	movl	%eax, (%esp)
	call	_sprintf
	nop
	addl	$268, %esp
	popl	%ebx
	popl	%esi
	popl	%edi
	popl	%ebp
	ret
	.section .rdata,"dr"
	.align 4
LC106:
	.ascii "0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x\0"
	.text
	.globl	__Z19deserialize_contextP8_CONTEXTPc
	.def	__Z19deserialize_contextP8_CONTEXTPc;	.scl	2;	.type	32;	.endef
__Z19deserialize_contextP8_CONTEXTPc:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%edi
	pushl	%esi
	pushl	%ebx
	subl	$76, %esp
	movl	8(%ebp), %eax
	movl	184(%eax), %ebx
	movl	8(%ebp), %eax
	movl	192(%eax), %esi
	movl	8(%ebp), %eax
	movl	196(%eax), %eax
	movl	%eax, -28(%ebp)
	movl	8(%ebp), %eax
	movl	180(%eax), %edx
	movl	%edx, -32(%ebp)
	movl	8(%ebp), %eax
	movl	156(%eax), %ecx
	movl	%ecx, -36(%ebp)
	movl	8(%ebp), %eax
	movl	160(%eax), %edi
	movl	%edi, -40(%ebp)
	movl	8(%ebp), %eax
	movl	164(%eax), %edi
	movl	8(%ebp), %eax
	movl	168(%eax), %ecx
	movl	8(%ebp), %eax
	movl	172(%eax), %edx
	movl	8(%ebp), %eax
	movl	176(%eax), %eax
	movl	%ebx, 44(%esp)
	movl	%esi, 40(%esp)
	movl	-28(%ebp), %ebx
	movl	%ebx, 36(%esp)
	movl	-32(%ebp), %esi
	movl	%esi, 32(%esp)
	movl	-36(%ebp), %ebx
	movl	%ebx, 28(%esp)
	movl	-40(%ebp), %esi
	movl	%esi, 24(%esp)
	movl	%edi, 20(%esp)
	movl	%ecx, 16(%esp)
	movl	%edx, 12(%esp)
	movl	%eax, 8(%esp)
	movl	$LC106, 4(%esp)
	movl	12(%ebp), %eax
	movl	%eax, (%esp)
	call	_sscanf
	nop
	addl	$76, %esp
	popl	%ebx
	popl	%esi
	popl	%edi
	popl	%ebp
	ret
	.section .rdata,"dr"
	.align 4
LC107:
	.ascii "Registering Exception: code 0x%08x at: 0x%08x\12\0"
LC108:
	.ascii "EX,0x%08x,%s\12\0"
	.text
	.globl	__Z18register_exceptionm17_EXCEPTION_RECORD
	.def	__Z18register_exceptionm17_EXCEPTION_RECORD;	.scl	2;	.type	32;	.endef
__Z18register_exceptionm17_EXCEPTION_RECORD:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%edi
	pushl	%esi
	pushl	%ebx
	subl	$1132, %esp
	movl	24(%ebp), %edx
	movl	12(%ebp), %eax
	movl	%edx, 8(%esp)
	movl	%eax, 4(%esp)
	movl	$LC107, (%esp)
	call	__Z7d_printPKcz
	leal	-1048(%ebp), %eax
	movl	%eax, 80(%esp)
	movl	%esp, %ebx
	leal	12(%ebp), %eax
	movl	$20, %edx
	movl	%ebx, %edi
	movl	%eax, %esi
	movl	%edx, %ecx
	rep movsl
	call	__Z19serialize_exception17_EXCEPTION_RECORDPc
	leal	-1048(%ebp), %eax
	movl	%eax, 12(%esp)
	movl	8(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	$LC108, 4(%esp)
	leal	-792(%ebp), %eax
	movl	%eax, (%esp)
	call	_sprintf
	leal	-792(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z13add_to_bufferPc
	nop
	addl	$1132, %esp
	popl	%ebx
	popl	%esi
	popl	%edi
	popl	%ebp
	ret
	.section .rdata,"dr"
LC109:
	.ascii "Failed: 0x%08x\12\0"
	.text
	.globl	__Z18getSelectorEntriesPv8_CONTEXTP10_LDT_ENTRY
	.def	__Z18getSelectorEntriesPv8_CONTEXTP10_LDT_ENTRY;	.scl	2;	.type	32;	.endef
__Z18getSelectorEntriesPv8_CONTEXTP10_LDT_ENTRY:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$24, %esp
	movl	152(%ebp), %eax
	movl	728(%ebp), %edx
	movl	%edx, 8(%esp)
	movl	%eax, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	movl	__imp__GetThreadSelectorEntry@12, %eax
	call	*%eax
	subl	$12, %esp
	testl	%eax, %eax
	sete	%al
	testb	%al, %al
	je	L217
	movl	__imp__GetLastError@0, %eax
	call	*%eax
	movl	%eax, 4(%esp)
	movl	$LC109, (%esp)
	call	__Z7d_printPKcz
L217:
	movl	728(%ebp), %eax
	leal	8(%eax), %edx
	movl	156(%ebp), %eax
	movl	%edx, 8(%esp)
	movl	%eax, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	movl	__imp__GetThreadSelectorEntry@12, %eax
	call	*%eax
	subl	$12, %esp
	testl	%eax, %eax
	sete	%al
	testb	%al, %al
	je	L218
	movl	$LC58, (%esp)
	call	__Z7d_printPKcz
L218:
	movl	728(%ebp), %eax
	leal	16(%eax), %edx
	movl	160(%ebp), %eax
	movl	%edx, 8(%esp)
	movl	%eax, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	movl	__imp__GetThreadSelectorEntry@12, %eax
	call	*%eax
	subl	$12, %esp
	testl	%eax, %eax
	sete	%al
	testb	%al, %al
	je	L219
	movl	$LC58, (%esp)
	call	__Z7d_printPKcz
L219:
	movl	728(%ebp), %eax
	leal	24(%eax), %edx
	movl	164(%ebp), %eax
	movl	%edx, 8(%esp)
	movl	%eax, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	movl	__imp__GetThreadSelectorEntry@12, %eax
	call	*%eax
	subl	$12, %esp
	testl	%eax, %eax
	sete	%al
	testb	%al, %al
	je	L220
	movl	$LC58, (%esp)
	call	__Z7d_printPKcz
L220:
	movl	728(%ebp), %eax
	leal	32(%eax), %edx
	movl	200(%ebp), %eax
	movl	%edx, 8(%esp)
	movl	%eax, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	movl	__imp__GetThreadSelectorEntry@12, %eax
	call	*%eax
	subl	$12, %esp
	testl	%eax, %eax
	sete	%al
	testb	%al, %al
	je	L221
	movl	$LC58, (%esp)
	call	__Z7d_printPKcz
L221:
	movl	728(%ebp), %eax
	leal	40(%eax), %edx
	movl	212(%ebp), %eax
	movl	%edx, 8(%esp)
	movl	%eax, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	movl	__imp__GetThreadSelectorEntry@12, %eax
	call	*%eax
	subl	$12, %esp
	testl	%eax, %eax
	sete	%al
	testb	%al, %al
	je	L222
	movl	$LC58, (%esp)
	call	__Z7d_printPKcz
L222:
	nop
	leave
	ret
	.section .rdata,"dr"
	.align 4
LC110:
	.ascii "Registering: TID 0x%08x, handle 0x%08x\12\0"
LC111:
	.ascii "# Thread count: 0x%08x\12\0"
	.align 4
LC112:
	.ascii "Updating: TID 0x%08x, handle 0x%08x\12\0"
LC113:
	.ascii "RT,0x%08x,%s\12\0"
	.text
	.globl	__Z15register_threadmPv
	.def	__Z15register_threadmPv;	.scl	2;	.type	32;	.endef
__Z15register_threadmPv:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%edi
	pushl	%esi
	pushl	%ebx
	subl	$2796, %esp
	cmpl	$0, 12(%ebp)
	jne	L225
	movl	8(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	$0, 4(%esp)
	movl	$2097151, (%esp)
	movl	__imp__OpenThread@12, %eax
	call	*%eax
	subl	$12, %esp
	movl	%eax, 12(%ebp)
L225:
	movl	_my_trace, %eax
	movl	8(%ebp), %edx
	addl	$32, %edx
	movl	8(%eax,%edx,4), %eax
	cmpl	$-1, %eax
	jne	L226
	movl	_my_trace, %eax
	movl	68964052(%eax), %edx
	movl	68964048(%eax), %eax
	movl	%eax, -28(%ebp)
	movl	_my_trace, %ecx
	movl	-28(%ebp), %edx
	movl	%edx, %eax
	sall	$2, %eax
	addl	%edx, %eax
	sall	$2, %eax
	addl	%ecx, %eax
	addl	$67108992, %eax
	movb	$1, 12(%eax)
	movl	_my_trace, %ecx
	movl	-28(%ebp), %edx
	movl	%edx, %eax
	sall	$2, %eax
	addl	%edx, %eax
	sall	$2, %eax
	addl	%ecx, %eax
	leal	67108992(%eax), %edx
	movl	12(%ebp), %eax
	movl	%eax, 16(%edx)
	movl	_my_trace, %ecx
	movl	-28(%ebp), %edx
	movl	%edx, %eax
	sall	$2, %eax
	addl	%edx, %eax
	sall	$2, %eax
	addl	%ecx, %eax
	addl	$67108992, %eax
	movb	$1, 13(%eax)
	movl	_my_trace, %ecx
	movl	-28(%ebp), %edx
	movl	%edx, %eax
	sall	$2, %eax
	addl	%edx, %eax
	sall	$2, %eax
	addl	%ecx, %eax
	addl	$67108992, %eax
	movb	$1, 14(%eax)
	movl	_my_trace, %ecx
	movl	-28(%ebp), %edx
	movl	%edx, %eax
	sall	$2, %eax
	addl	%edx, %eax
	sall	$2, %eax
	addl	%ecx, %eax
	leal	67108992(%eax), %edx
	movl	8(%ebp), %eax
	movl	%eax, 8(%edx)
	movl	_my_trace, %ecx
	movl	-28(%ebp), %edx
	movl	%edx, %eax
	sall	$2, %eax
	addl	%edx, %eax
	sall	$2, %eax
	addl	%ecx, %eax
	addl	$67109008, %eax
	movl	$0, 8(%eax)
	movl	12(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	8(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC110, (%esp)
	call	__Z7d_printPKcz
	movl	_my_trace, %eax
	movl	8(%ebp), %edx
	leal	32(%edx), %ecx
	movl	-28(%ebp), %edx
	movl	%edx, 8(%eax,%ecx,4)
	movl	_my_trace, %ecx
	movl	68964048(%ecx), %eax
	movl	68964052(%ecx), %edx
	addl	$1, %eax
	adcl	$0, %edx
	movl	%eax, 68964048(%ecx)
	movl	%edx, 68964052(%ecx)
	movl	_my_trace, %eax
	movl	68964052(%eax), %edx
	movl	68964048(%eax), %eax
	movl	%eax, 8(%esp)
	movl	%edx, 12(%esp)
	movl	$LC111, 4(%esp)
	leal	-796(%ebp), %eax
	movl	%eax, (%esp)
	call	_sprintf
	leal	-796(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z13add_to_bufferPc
	jmp	L227
L226:
	movl	_my_trace, %eax
	movl	8(%ebp), %edx
	addl	$32, %edx
	movl	8(%eax,%edx,4), %eax
	movl	%eax, -28(%ebp)
	movl	12(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	8(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC112, (%esp)
	call	__Z7d_printPKcz
	movl	_my_trace, %ecx
	movl	-28(%ebp), %edx
	movl	%edx, %eax
	sall	$2, %eax
	addl	%edx, %eax
	sall	$2, %eax
	addl	%ecx, %eax
	addl	$67108992, %eax
	movb	$1, 12(%eax)
	movl	_my_trace, %ecx
	movl	-28(%ebp), %edx
	movl	%edx, %eax
	sall	$2, %eax
	addl	%edx, %eax
	sall	$2, %eax
	addl	%ecx, %eax
	leal	67108992(%eax), %edx
	movl	12(%ebp), %eax
	movl	%eax, 16(%edx)
L227:
	movl	$65543, -1512(%ebp)
	movl	_my_trace, %ecx
	movl	-28(%ebp), %edx
	movl	%edx, %eax
	sall	$2, %eax
	addl	%edx, %eax
	sall	$2, %eax
	addl	%ecx, %eax
	addl	$67108992, %eax
	movl	16(%eax), %eax
	leal	-1512(%ebp), %edx
	movl	%edx, 4(%esp)
	movl	%eax, (%esp)
	movl	__imp__GetThreadContext@8, %eax
	call	*%eax
	subl	$8, %esp
	leal	-2072(%ebp), %eax
	movl	%eax, 720(%esp)
	leal	4(%esp), %eax
	leal	-1512(%ebp), %edx
	movl	$179, %ecx
	movl	%eax, %edi
	movl	%edx, %esi
	rep movsl
	movl	12(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z18getSelectorEntriesPv8_CONTEXTP10_LDT_ENTRY
	leal	-2024(%ebp), %eax
	movl	%eax, 720(%esp)
	leal	-2072(%ebp), %eax
	movl	%eax, 716(%esp)
	movl	%esp, %ebx
	leal	-1512(%ebp), %eax
	movl	$179, %edx
	movl	%ebx, %edi
	movl	%eax, %esi
	movl	%edx, %ecx
	rep movsl
	call	__Z17serialize_context8_CONTEXTP10_LDT_ENTRYPc
	leal	-2024(%ebp), %eax
	movl	%eax, 12(%esp)
	movl	8(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	$LC113, 4(%esp)
	leal	-796(%ebp), %eax
	movl	%eax, (%esp)
	call	_sprintf
	leal	-796(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z13add_to_bufferPc
	nop
	leal	-12(%ebp), %esp
	popl	%ebx
	popl	%esi
	popl	%edi
	popl	%ebp
	ret
	.section .rdata,"dr"
LC114:
	.ascii "CT,0x%08x,%s\12\0"
	.text
	.globl	__Z21register_thread_debugmPv
	.def	__Z21register_thread_debugmPv;	.scl	2;	.type	32;	.endef
__Z21register_thread_debugmPv:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%edi
	pushl	%esi
	pushl	%ebx
	subl	$2812, %esp
	movl	$0, -28(%ebp)
	cmpl	$0, 12(%ebp)
	jne	L230
	movl	8(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	$0, 4(%esp)
	movl	$2097151, (%esp)
	movl	__imp__OpenThread@12, %eax
	call	*%eax
	subl	$12, %esp
	movl	%eax, 12(%ebp)
L230:
	movl	$65543, -1516(%ebp)
	movl	_my_trace, %eax
	movl	8(%ebp), %edx
	addl	$32, %edx
	movl	8(%eax,%edx,4), %eax
	movl	%eax, -32(%ebp)
	movl	_my_trace, %ecx
	movl	-32(%ebp), %edx
	movl	%edx, %eax
	sall	$2, %eax
	addl	%edx, %eax
	sall	$2, %eax
	addl	%ecx, %eax
	addl	$67108992, %eax
	movl	16(%eax), %eax
	leal	-1516(%ebp), %edx
	movl	%edx, 4(%esp)
	movl	%eax, (%esp)
	movl	__imp__GetThreadContext@8, %eax
	call	*%eax
	subl	$8, %esp
	leal	-2076(%ebp), %eax
	movl	%eax, 720(%esp)
	leal	4(%esp), %eax
	leal	-1516(%ebp), %edx
	movl	$179, %ecx
	movl	%eax, %edi
	movl	%edx, %esi
	rep movsl
	movl	12(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z18getSelectorEntriesPv8_CONTEXTP10_LDT_ENTRY
	leal	-2028(%ebp), %eax
	movl	%eax, 720(%esp)
	leal	-2076(%ebp), %eax
	movl	%eax, 716(%esp)
	movl	%esp, %ebx
	leal	-1516(%ebp), %eax
	movl	$179, %edx
	movl	%ebx, %edi
	movl	%eax, %esi
	movl	%edx, %ecx
	rep movsl
	call	__Z17serialize_context8_CONTEXTP10_LDT_ENTRYPc
	leal	-2028(%ebp), %eax
	movl	%eax, 12(%esp)
	movl	8(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	$LC114, 4(%esp)
	leal	-800(%ebp), %eax
	movl	%eax, (%esp)
	call	_sprintf
	leal	-800(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z13add_to_bufferPc
	movl	%eax, -28(%ebp)
	movl	-28(%ebp), %eax
	leal	-12(%ebp), %esp
	popl	%ebx
	popl	%esi
	popl	%edi
	popl	%ebp
	ret
	.section .rdata,"dr"
LC115:
	.ascii "DT,0x%08x,%s\12\0"
	.text
	.globl	__Z17deregister_threadmPv
	.def	__Z17deregister_threadmPv;	.scl	2;	.type	32;	.endef
__Z17deregister_threadmPv:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%edi
	pushl	%esi
	pushl	%ebx
	subl	$2796, %esp
	cmpl	$0, 12(%ebp)
	jne	L233
	movl	8(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	$0, 4(%esp)
	movl	$2097151, (%esp)
	movl	__imp__OpenThread@12, %eax
	call	*%eax
	subl	$12, %esp
	movl	%eax, 12(%ebp)
L233:
	movl	_my_trace, %eax
	movl	8(%ebp), %edx
	addl	$32, %edx
	movl	8(%eax,%edx,4), %eax
	movl	%eax, -28(%ebp)
	movl	_my_trace, %ecx
	movl	-28(%ebp), %edx
	movl	%edx, %eax
	sall	$2, %eax
	addl	%edx, %eax
	sall	$2, %eax
	addl	%ecx, %eax
	addl	$67108992, %eax
	movb	$0, 12(%eax)
	movl	_my_trace, %ecx
	movl	-28(%ebp), %edx
	movl	%edx, %eax
	sall	$2, %eax
	addl	%edx, %eax
	sall	$2, %eax
	addl	%ecx, %eax
	leal	67108992(%eax), %edx
	movl	12(%ebp), %eax
	movl	%eax, 16(%edx)
	movl	_my_trace, %ecx
	movl	-28(%ebp), %edx
	movl	%edx, %eax
	sall	$2, %eax
	addl	%edx, %eax
	sall	$2, %eax
	addl	%ecx, %eax
	addl	$67108992, %eax
	movb	$1, 13(%eax)
	movl	$65543, -1512(%ebp)
	movl	_my_trace, %ecx
	movl	-28(%ebp), %edx
	movl	%edx, %eax
	sall	$2, %eax
	addl	%edx, %eax
	sall	$2, %eax
	addl	%ecx, %eax
	addl	$67108992, %eax
	movl	16(%eax), %eax
	leal	-1512(%ebp), %edx
	movl	%edx, 4(%esp)
	movl	%eax, (%esp)
	movl	__imp__GetThreadContext@8, %eax
	call	*%eax
	subl	$8, %esp
	leal	-2072(%ebp), %eax
	movl	%eax, 720(%esp)
	leal	4(%esp), %eax
	leal	-1512(%ebp), %edx
	movl	$179, %ecx
	movl	%eax, %edi
	movl	%edx, %esi
	rep movsl
	movl	12(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z18getSelectorEntriesPv8_CONTEXTP10_LDT_ENTRY
	leal	-2024(%ebp), %eax
	movl	%eax, 720(%esp)
	leal	-2072(%ebp), %eax
	movl	%eax, 716(%esp)
	movl	%esp, %ebx
	leal	-1512(%ebp), %eax
	movl	$179, %edx
	movl	%ebx, %edi
	movl	%eax, %esi
	movl	%edx, %ecx
	rep movsl
	call	__Z17serialize_context8_CONTEXTP10_LDT_ENTRYPc
	leal	-2024(%ebp), %eax
	movl	%eax, 12(%esp)
	movl	8(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	$LC115, 4(%esp)
	leal	-796(%ebp), %eax
	movl	%eax, (%esp)
	call	_sprintf
	leal	-796(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z13add_to_bufferPc
	nop
	leal	-12(%ebp), %esp
	popl	%ebx
	popl	%esi
	popl	%edi
	popl	%ebp
	ret
	.section .rdata,"dr"
LC116:
	.ascii "DT,0x%08x\12\0"
	.text
	.globl	__Z18deregister_thread2m
	.def	__Z18deregister_thread2m;	.scl	2;	.type	32;	.endef
__Z18deregister_thread2m:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$808, %esp
	movl	_my_trace, %eax
	movl	8(%ebp), %edx
	addl	$32, %edx
	movl	8(%eax,%edx,4), %eax
	movl	%eax, -12(%ebp)
	movl	_my_trace, %ecx
	movl	-12(%ebp), %edx
	movl	%edx, %eax
	sall	$2, %eax
	addl	%edx, %eax
	sall	$2, %eax
	addl	%ecx, %eax
	addl	$67108992, %eax
	movl	16(%eax), %eax
	testl	%eax, %eax
	je	L236
	movl	_my_trace, %ecx
	movl	-12(%ebp), %edx
	movl	%edx, %eax
	sall	$2, %eax
	addl	%edx, %eax
	sall	$2, %eax
	addl	%ecx, %eax
	addl	$67108992, %eax
	movl	16(%eax), %eax
	movl	%eax, (%esp)
	movl	__imp__CloseHandle@4, %eax
	call	*%eax
	subl	$4, %esp
L236:
	movl	_my_trace, %ecx
	movl	-12(%ebp), %edx
	movl	%edx, %eax
	sall	$2, %eax
	addl	%edx, %eax
	sall	$2, %eax
	addl	%ecx, %eax
	addl	$67108992, %eax
	movl	$0, 16(%eax)
	movl	_my_trace, %ecx
	movl	-12(%ebp), %edx
	movl	%edx, %eax
	sall	$2, %eax
	addl	%edx, %eax
	sall	$2, %eax
	addl	%ecx, %eax
	addl	$67108992, %eax
	movb	$0, 13(%eax)
	movl	8(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	$LC116, 4(%esp)
	leal	-780(%ebp), %eax
	movl	%eax, (%esp)
	call	_sprintf
	leal	-780(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z13add_to_bufferPc
	nop
	leave
	ret
	.globl	__Z9find_filePc
	.def	__Z9find_filePc;	.scl	2;	.type	32;	.endef
__Z9find_filePc:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$40, %esp
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_strlen
	movl	%eax, -20(%ebp)
	movl	8(%ebp), %eax
	movl	%eax, -24(%ebp)
	movl	$0, -16(%ebp)
	jmp	L239
L241:
	movl	8(%ebp), %edx
	movl	-16(%ebp), %eax
	addl	%edx, %eax
	movzbl	(%eax), %eax
	cmpb	$92, %al
	jne	L240
	movl	8(%ebp), %edx
	movl	-16(%ebp), %eax
	addl	%edx, %eax
	movl	%eax, -12(%ebp)
L240:
	addl	$1, -16(%ebp)
L239:
	movl	-16(%ebp), %eax
	cmpl	-20(%ebp), %eax
	jb	L241
	movl	-12(%ebp), %eax
	addl	$1, %eax
	leave
	ret
	.section .rdata,"dr"
LC117:
	.ascii "self\0"
LC118:
	.ascii "self is always loaded dummy\12\0"
LC119:
	.ascii "Lib %s is loaded\12\0"
	.text
	.globl	__Z16check_lib_loadedPc
	.def	__Z16check_lib_loadedPc;	.scl	2;	.type	32;	.endef
__Z16check_lib_loadedPc:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%ebx
	subl	$36, %esp
	movl	$LC117, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_strcmpi
	testl	%eax, %eax
	sete	%al
	testb	%al, %al
	je	L244
	movl	$LC118, (%esp)
	call	__Z7d_printPKcz
	movl	$1, %eax
	jmp	L245
L244:
	movl	$0, -12(%ebp)
	jmp	L246
L248:
	movl	_my_trace, %edx
	movl	-12(%ebp), %eax
	sall	$3, %eax
	movl	%eax, %ecx
	sall	$7, %ecx
	addl	%ecx, %eax
	addl	$67114112, %eax
	addl	%edx, %eax
	leal	16(%eax), %edx
	movl	8(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	%edx, (%esp)
	call	_strcmpi
	testl	%eax, %eax
	sete	%al
	testb	%al, %al
	je	L247
	movl	_my_trace, %edx
	movl	-12(%ebp), %eax
	sall	$3, %eax
	movl	%eax, %ecx
	sall	$7, %ecx
	addl	%ecx, %eax
	addl	%edx, %eax
	addl	$67114112, %eax
	movzbl	8(%eax), %eax
	testb	%al, %al
	je	L247
	movl	8(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC119, (%esp)
	call	__Z7d_printPKcz
	movl	-12(%ebp), %eax
	jmp	L245
L247:
	addl	$1, -12(%ebp)
L246:
	movl	-12(%ebp), %ecx
	movl	$0, %ebx
	movl	_my_trace, %eax
	movl	68964060(%eax), %edx
	movl	68964056(%eax), %eax
	cmpl	%edx, %ebx
	jb	L248
	cmpl	%edx, %ebx
	ja	L250
	cmpl	%eax, %ecx
	jb	L248
L250:
	movl	$-1, %eax
L245:
	addl	$36, %esp
	popl	%ebx
	popl	%ebp
	ret
	.section .rdata,"dr"
LC120:
	.ascii "RL,0x%08x,%s\12\0"
	.text
	.globl	__Z12register_lib20_LOAD_DLL_DEBUG_INFO
	.def	__Z12register_lib20_LOAD_DLL_DEBUG_INFO;	.scl	2;	.type	32;	.endef
__Z12register_lib20_LOAD_DLL_DEBUG_INFO:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%ebx
	subl	$804, %esp
	movl	$0, -12(%ebp)
	movl	_my_trace, %ecx
	movl	_my_trace, %eax
	movl	68964060(%eax), %edx
	movl	68964056(%eax), %eax
	sall	$3, %eax
	movl	%eax, %edx
	sall	$7, %edx
	addl	%edx, %eax
	addl	$67114624, %eax
	addl	%ecx, %eax
	leal	16(%eax), %edx
	movl	_my_trace, %eax
	movl	68964104(%eax), %eax
	movl	$4, 12(%esp)
	movl	$768, 8(%esp)
	movl	%edx, 4(%esp)
	movl	%eax, (%esp)
	movl	__imp__GetFinalPathNameByHandleA@16, %eax
	call	*%eax
	subl	$16, %esp
	movl	_my_trace, %ecx
	movl	_my_trace, %eax
	movl	68964060(%eax), %edx
	movl	68964056(%eax), %eax
	sall	$3, %eax
	movl	%eax, %edx
	sall	$7, %edx
	addl	%edx, %eax
	addl	$67114624, %eax
	addl	%ecx, %eax
	addl	$16, %eax
	movl	%eax, (%esp)
	call	__Z9find_filePc
	movl	%eax, %ebx
	movl	_my_trace, %ecx
	movl	_my_trace, %eax
	movl	68964060(%eax), %edx
	movl	68964056(%eax), %eax
	sall	$3, %eax
	movl	%eax, %edx
	sall	$7, %edx
	addl	%edx, %eax
	addl	$67114112, %eax
	addl	%ecx, %eax
	addl	$16, %eax
	movl	%ebx, 4(%esp)
	movl	%eax, (%esp)
	call	_strcpy
	movl	_my_trace, %ecx
	movl	_my_trace, %eax
	movl	68964060(%eax), %edx
	movl	68964056(%eax), %eax
	movl	12(%ebp), %ebx
	sall	$3, %eax
	movl	%eax, %edx
	sall	$7, %edx
	addl	%edx, %eax
	addl	%ecx, %eax
	addl	$67114112, %eax
	movl	%ebx, 12(%eax)
	movl	_my_trace, %ecx
	movl	_my_trace, %eax
	movl	68964060(%eax), %edx
	movl	68964056(%eax), %eax
	sall	$3, %eax
	movl	%eax, %edx
	sall	$7, %edx
	addl	%edx, %eax
	addl	$67114112, %eax
	addl	%ecx, %eax
	leal	16(%eax), %ecx
	movl	_my_trace, %ebx
	movl	_my_trace, %eax
	movl	68964060(%eax), %edx
	movl	68964056(%eax), %eax
	sall	$3, %eax
	movl	%eax, %edx
	sall	$7, %edx
	addl	%edx, %eax
	addl	%ebx, %eax
	addl	$67114112, %eax
	movl	12(%eax), %eax
	movl	%ecx, 8(%esp)
	movl	%eax, 4(%esp)
	movl	$LC120, (%esp)
	call	__Z7d_printPKcz
	movl	_my_trace, %ecx
	movl	_my_trace, %eax
	movl	68964060(%eax), %edx
	movl	68964056(%eax), %eax
	sall	$3, %eax
	movl	%eax, %edx
	sall	$7, %edx
	addl	%edx, %eax
	addl	$67114112, %eax
	addl	%ecx, %eax
	leal	16(%eax), %ecx
	movl	_my_trace, %ebx
	movl	_my_trace, %eax
	movl	68964060(%eax), %edx
	movl	68964056(%eax), %eax
	sall	$3, %eax
	movl	%eax, %edx
	sall	$7, %edx
	addl	%edx, %eax
	addl	%ebx, %eax
	addl	$67114112, %eax
	movl	12(%eax), %eax
	movl	%ecx, 12(%esp)
	movl	%eax, 8(%esp)
	movl	$LC120, 4(%esp)
	leal	-780(%ebp), %eax
	movl	%eax, (%esp)
	call	_sprintf
	movl	_my_trace, %ecx
	movl	_my_trace, %eax
	movl	68964060(%eax), %edx
	movl	68964056(%eax), %eax
	sall	$3, %eax
	movl	%eax, %edx
	sall	$7, %edx
	addl	%edx, %eax
	addl	%ecx, %eax
	addl	$67114112, %eax
	movb	$1, 8(%eax)
	movl	_my_trace, %ecx
	movl	68964056(%ecx), %eax
	movl	68964060(%ecx), %edx
	addl	$1, %eax
	adcl	$0, %edx
	movl	%eax, 68964056(%ecx)
	movl	%edx, 68964060(%ecx)
	leal	-780(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z13add_to_bufferPc
	nop
	movl	-4(%ebp), %ebx
	leave
	ret
	.section .rdata,"dr"
LC121:
	.ascii "DL,0x%08x,%s\12\0"
	.text
	.globl	__Z14deregister_libm
	.def	__Z14deregister_libm;	.scl	2;	.type	32;	.endef
__Z14deregister_libm:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%ebx
	subl	$788, %esp
	movl	_my_trace, %edx
	movl	8(%ebp), %eax
	sall	$3, %eax
	movl	%eax, %ecx
	sall	$7, %ecx
	addl	%ecx, %eax
	addl	$67114112, %eax
	addl	%edx, %eax
	leal	16(%eax), %edx
	movl	_my_trace, %ecx
	movl	8(%ebp), %eax
	sall	$3, %eax
	movl	%eax, %ebx
	sall	$7, %ebx
	addl	%ebx, %eax
	addl	%ecx, %eax
	addl	$67114112, %eax
	movl	12(%eax), %eax
	movl	%edx, 12(%esp)
	movl	%eax, 8(%esp)
	movl	$LC121, 4(%esp)
	leal	-776(%ebp), %eax
	movl	%eax, (%esp)
	call	_sprintf
	movl	_my_trace, %edx
	movl	8(%ebp), %eax
	sall	$3, %eax
	movl	%eax, %ecx
	sall	$7, %ecx
	addl	%ecx, %eax
	addl	%edx, %eax
	addl	$67114112, %eax
	movb	$0, 8(%eax)
	leal	-776(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z13add_to_bufferPc
	addl	$788, %esp
	popl	%ebx
	popl	%ebp
	ret
	.globl	__Z14deregister_lib22_UNLOAD_DLL_DEBUG_INFO
	.def	__Z14deregister_lib22_UNLOAD_DLL_DEBUG_INFO;	.scl	2;	.type	32;	.endef
__Z14deregister_lib22_UNLOAD_DLL_DEBUG_INFO:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%ebx
	subl	$804, %esp
	movl	$0, -12(%ebp)
	jmp	L255
L258:
	movl	_my_trace, %edx
	movl	-12(%ebp), %eax
	sall	$3, %eax
	movl	%eax, %ecx
	sall	$7, %ecx
	addl	%ecx, %eax
	addl	%edx, %eax
	addl	$67114112, %eax
	movl	12(%eax), %eax
	movl	8(%ebp), %edx
	cmpl	%edx, %eax
	jne	L256
	jmp	L257
L256:
	addl	$1, -12(%ebp)
L255:
	movl	-12(%ebp), %ecx
	movl	$0, %ebx
	movl	_my_trace, %eax
	movl	68964060(%eax), %edx
	movl	68964056(%eax), %eax
	cmpl	%edx, %ebx
	jb	L258
	cmpl	%edx, %ebx
	ja	L257
	cmpl	%eax, %ecx
	jb	L258
L257:
	movl	_my_trace, %edx
	movl	-12(%ebp), %eax
	sall	$3, %eax
	movl	%eax, %ecx
	sall	$7, %ecx
	addl	%ecx, %eax
	addl	$67114112, %eax
	addl	%edx, %eax
	leal	16(%eax), %edx
	movl	_my_trace, %ecx
	movl	-12(%ebp), %eax
	sall	$3, %eax
	movl	%eax, %ebx
	sall	$7, %ebx
	addl	%ebx, %eax
	addl	%ecx, %eax
	addl	$67114112, %eax
	movl	12(%eax), %eax
	movl	%edx, 12(%esp)
	movl	%eax, 8(%esp)
	movl	$LC121, 4(%esp)
	leal	-780(%ebp), %eax
	movl	%eax, (%esp)
	call	_sprintf
	movl	_my_trace, %edx
	movl	-12(%ebp), %eax
	sall	$3, %eax
	movl	%eax, %ecx
	sall	$7, %ecx
	addl	%ecx, %eax
	addl	%edx, %eax
	addl	$67114112, %eax
	movb	$0, 8(%eax)
	leal	-780(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z13add_to_bufferPc
	nop
	addl	$804, %esp
	popl	%ebx
	popl	%ebp
	ret
	.section .rdata,"dr"
LC122:
	.ascii "sysenter\12\0"
LC123:
	.ascii "ESP: %p\12\0"
	.align 4
LC124:
	.ascii "# Syscall in TID: 0x%08x no: 0x%08x, stack@ 0x%08x\12\0"
	.text
	.globl	__Z23react_sysenter_callbackPv
	.def	__Z23react_sysenter_callbackPv;	.scl	2;	.type	32;	.endef
__Z23react_sysenter_callbackPv:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$1544, %esp
	movl	$LC122, (%esp)
	call	__Z7d_printPKcz
	movl	8(%ebp), %eax
	movl	%eax, -12(%ebp)
	movl	-12(%ebp), %eax
	movl	8(%eax), %eax
	movl	%eax, -16(%ebp)
	movl	_my_trace, %eax
	movl	-16(%ebp), %edx
	addl	$32, %edx
	movl	8(%eax,%edx,4), %eax
	movl	%eax, -20(%ebp)
	movl	$65543, -1504(%ebp)
	movl	_my_trace, %ecx
	movl	-20(%ebp), %edx
	movl	%edx, %eax
	sall	$2, %eax
	addl	%edx, %eax
	sall	$2, %eax
	addl	%ecx, %eax
	addl	$67108992, %eax
	movl	16(%eax), %eax
	leal	-1504(%ebp), %edx
	movl	%edx, 4(%esp)
	movl	%eax, (%esp)
	movl	__imp__GetThreadContext@8, %eax
	call	*%eax
	subl	$8, %esp
	testl	%eax, %eax
	sete	%al
	testb	%al, %al
	je	L262
	movl	__imp__GetLastError@0, %eax
	call	*%eax
	movl	%eax, 4(%esp)
	movl	$LC7, (%esp)
	call	__Z7d_printPKcz
	jmp	L261
L262:
	movl	-1308(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC123, (%esp)
	call	__Z7d_printPKcz
	movl	-1328(%ebp), %eax
	movl	%eax, _sysenter_no
	movl	-1308(%ebp), %eax
	movl	%eax, _sysenter_esp
	movl	_sysenter_esp, %edx
	movl	_sysenter_no, %eax
	movl	%edx, 16(%esp)
	movl	%eax, 12(%esp)
	movl	-16(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	$LC124, 4(%esp)
	leal	-788(%ebp), %eax
	movl	%eax, (%esp)
	call	_sprintf
	leal	-788(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z13add_to_bufferPc
	movl	_my_trace, %ecx
	movl	-20(%ebp), %edx
	movl	%edx, %eax
	sall	$2, %eax
	addl	%edx, %eax
	sall	$2, %eax
	addl	%ecx, %eax
	addl	$67108992, %eax
	movl	16(%eax), %eax
	movl	%eax, 4(%esp)
	movl	-16(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z17deregister_threadmPv
	movl	$0, (%esp)
	call	__Z6set_ssm
L261:
	leave
	ret
	.section .rdata,"dr"
LC125:
	.ascii "sysret\12\0"
LC126:
	.ascii "EAX: 0x%08x\12\0"
LC127:
	.ascii "[[Syscall: 0x%08x @ 0x%08x]]\12\0"
	.align 4
LC128:
	.ascii "Hunting for reads from: 0x%08x\12\0"
LC129:
	.ascii "RN,0x%08x,0x%08x\12\0"
LC130:
	.ascii "Marked taint: 0x%08x,0x%08x\12\0"
	.text
	.globl	__Z21react_sysret_callbackPv
	.def	__Z21react_sysret_callbackPv;	.scl	2;	.type	32;	.endef
__Z21react_sysret_callbackPv:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$1576, %esp
	movl	$LC125, (%esp)
	call	__Z7d_printPKcz
	movl	8(%ebp), %eax
	movl	%eax, -16(%ebp)
	movl	-16(%ebp), %eax
	movl	8(%eax), %eax
	movl	%eax, -20(%ebp)
	movl	_my_trace, %eax
	movl	-20(%ebp), %edx
	addl	$32, %edx
	movl	8(%eax,%edx,4), %eax
	movl	%eax, -24(%ebp)
	movl	_my_trace, %ecx
	movl	-24(%ebp), %edx
	movl	%edx, %eax
	sall	$2, %eax
	addl	%edx, %eax
	sall	$2, %eax
	addl	%ecx, %eax
	addl	$67108992, %eax
	movl	16(%eax), %eax
	movl	%eax, 4(%esp)
	movl	-20(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z15register_threadmPv
	movl	$65543, -1536(%ebp)
	movl	_my_trace, %ecx
	movl	-24(%ebp), %edx
	movl	%edx, %eax
	sall	$2, %eax
	addl	%edx, %eax
	sall	$2, %eax
	addl	%ecx, %eax
	addl	$67108992, %eax
	movl	16(%eax), %eax
	leal	-1536(%ebp), %edx
	movl	%edx, 4(%esp)
	movl	%eax, (%esp)
	movl	__imp__GetThreadContext@8, %eax
	call	*%eax
	subl	$8, %esp
	testl	%eax, %eax
	sete	%al
	testb	%al, %al
	je	L265
	movl	__imp__GetLastError@0, %eax
	call	*%eax
	movl	%eax, 4(%esp)
	movl	$LC7, (%esp)
	call	__Z7d_printPKcz
	jmp	L264
L265:
	movl	-1360(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC126, (%esp)
	call	__Z7d_printPKcz
	movl	-1352(%ebp), %edx
	movl	_sysenter_no, %eax
	movl	%edx, 8(%esp)
	movl	%eax, 4(%esp)
	movl	$LC127, (%esp)
	call	__Z7d_printPKcz
	movl	$0, -12(%ebp)
	jmp	L267
L272:
	movl	_my_trace, %eax
	movl	_sysenter_no, %edx
	movl	%edx, %ecx
	sall	$4, %ecx
	movl	-12(%ebp), %edx
	addl	%ecx, %edx
	addl	$4376611, %edx
	sall	$4, %edx
	addl	%edx, %eax
	movzbl	16(%eax), %edx
	movzbl	_last_arg+8, %eax
	cmpb	%al, %dl
	je	L269
	movl	_my_trace, %eax
	movl	_sysenter_no, %edx
	movl	%edx, %ecx
	sall	$4, %ecx
	movl	-12(%ebp), %edx
	addl	%ecx, %edx
	addl	$4376611, %edx
	sall	$4, %edx
	addl	%edx, %eax
	movl	20(%eax), %eax
	cmpl	$-16711936, %eax
	je	L270
	movl	_my_trace, %eax
	movl	_sysenter_no, %edx
	movl	%edx, %ecx
	sall	$4, %ecx
	movl	-12(%ebp), %edx
	addl	%ecx, %edx
	addl	$4376611, %edx
	sall	$4, %edx
	addl	%edx, %eax
	movl	20(%eax), %edx
	movl	-1360(%ebp), %eax
	cmpl	%eax, %edx
	jne	L271
L270:
	movl	_my_trace, %eax
	movl	_sysenter_no, %edx
	movl	%edx, %ecx
	sall	$4, %ecx
	movl	-12(%ebp), %edx
	addl	%ecx, %edx
	addl	$4376611, %edx
	sall	$4, %edx
	addl	%edx, %eax
	leal	8(%eax), %edx
	leal	-52(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	%edx, (%esp)
	call	__Z14resolve_regionP20LOCATION_DESCRIPTOR_P9LOCATION_
	leal	-52(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z17update_region_oldP9LOCATION_
L271:
	addl	$1, -12(%ebp)
L267:
	cmpl	$15, -12(%ebp)
	jbe	L272
L269:
	movl	_sysenter_no, %eax
	cmpl	$66, %eax
	jne	L273
	movl	_file_handle, %eax
	testl	%eax, %eax
	jne	L273
	movl	_sysenter_esp, %eax
	addl	$8, %eax
	movl	%eax, -28(%ebp)
	movl	_my_trace, %eax
	movl	_sysenter_no, %edx
	movl	%edx, %ecx
	sall	$4, %ecx
	movl	-12(%ebp), %edx
	addl	%ecx, %edx
	addl	$4376611, %edx
	sall	$4, %edx
	addl	%edx, %eax
	movl	12(%eax), %eax
	sall	$2, %eax
	addl	%eax, -28(%ebp)
	movl	-28(%ebp), %edx
	movl	_my_trace, %eax
	movl	88(%eax), %eax
	leal	-44(%ebp), %ecx
	movl	%ecx, 16(%esp)
	movl	$4, 12(%esp)
	leal	-40(%ebp), %ecx
	movl	%ecx, 8(%esp)
	movl	%edx, 4(%esp)
	movl	%eax, (%esp)
	call	__Z11read_memoryPvS_S_mPm
	movl	-40(%ebp), %eax
	movl	%eax, -28(%ebp)
	movl	-28(%ebp), %edx
	movl	_my_trace, %eax
	movl	88(%eax), %eax
	leal	-44(%ebp), %ecx
	movl	%ecx, 16(%esp)
	movl	$4, 12(%esp)
	movl	$_file_handle, 8(%esp)
	movl	%edx, 4(%esp)
	movl	%eax, (%esp)
	call	__Z11read_memoryPvS_S_mPm
	movl	_file_handle, %eax
	movl	%eax, 4(%esp)
	movl	$LC128, (%esp)
	call	__Z7d_printPKcz
L273:
	movl	_sysenter_no, %eax
	cmpl	$273, %eax
	jne	L274
	movl	_my_trace, %eax
	movl	86803000(%eax), %eax
	movl	%eax, %edx
	movl	_file_handle, %eax
	cmpl	%eax, %edx
	jne	L274
	movl	_my_trace, %eax
	movl	86803028(%eax), %edx
	movl	_my_trace, %eax
	movl	86803024(%eax), %eax
	movl	%edx, 12(%esp)
	movl	%eax, 8(%esp)
	movl	$LC129, 4(%esp)
	leal	-820(%ebp), %eax
	movl	%eax, (%esp)
	call	_sprintf
	leal	-820(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z13add_to_bufferPc
	movl	-36(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	-32(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC130, (%esp)
	call	__Z7d_printPKcz
L274:
	movl	-20(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z6set_ssm
L264:
	leave
	ret
	.globl	__Z9get_stackPvj
	.def	__Z9get_stackPvj;	.scl	2;	.type	32;	.endef
__Z9get_stackPvj:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$776, %esp
	leal	-740(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	movl	__imp__GetThreadContext@8, %eax
	call	*%eax
	subl	$8, %esp
	movl	$0, -16(%ebp)
	jmp	L276
L277:
	addl	$4, -12(%ebp)
	addl	$1, -16(%ebp)
L276:
	movl	-16(%ebp), %eax
	cmpl	12(%ebp), %eax
	jb	L277
	movl	-12(%ebp), %edx
	movl	_my_trace, %eax
	movl	88(%eax), %eax
	leal	-24(%ebp), %ecx
	movl	%ecx, 16(%esp)
	movl	$4, 12(%esp)
	leal	-20(%ebp), %ecx
	movl	%ecx, 8(%esp)
	movl	%edx, 4(%esp)
	movl	%eax, (%esp)
	movl	__imp__ReadProcessMemory@20, %eax
	call	*%eax
	subl	$20, %esp
	movl	-20(%ebp), %eax
	leave
	ret
	.section .rdata,"dr"
LC131:
	.ascii "TS,0x%08x\12\0"
	.text
	.globl	__Z21createthread_callbackPv
	.def	__Z21createthread_callbackPv;	.scl	2;	.type	32;	.endef
__Z21createthread_callbackPv:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$808, %esp
	movl	8(%ebp), %eax
	movl	%eax, -12(%ebp)
	movl	-12(%ebp), %eax
	movl	8(%eax), %eax
	movl	%eax, -16(%ebp)
	movl	-16(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	$0, 4(%esp)
	movl	$2097151, (%esp)
	movl	__imp__OpenThread@12, %eax
	call	*%eax
	subl	$12, %esp
	movl	%eax, -20(%ebp)
	movl	$3, 4(%esp)
	movl	-20(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z9get_stackPvj
	movl	%eax, -24(%ebp)
	movl	-24(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	$LC131, 4(%esp)
	leal	-792(%ebp), %eax
	movl	%eax, (%esp)
	call	_sprintf
	movl	-24(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC131, (%esp)
	call	__Z7d_printPKcz
	leal	-792(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z13add_to_bufferPc
	leave
	ret
	.section .rdata,"dr"
LC132:
	.ascii "[antidebug] enter\12\0"
	.text
	.globl	__Z28isdebuggerpresent_callback_1Pv
	.def	__Z28isdebuggerpresent_callback_1Pv;	.scl	2;	.type	32;	.endef
__Z28isdebuggerpresent_callback_1Pv:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$24, %esp
	movl	$LC132, (%esp)
	call	__Z7d_printPKcz
	nop
	leave
	ret
	.section .rdata,"dr"
LC133:
	.ascii "[antidebug] EAX: 0x%08x\12\0"
	.align 4
LC134:
	.ascii "[antidebug] IsDebuggerPresent handled\12\0"
LC135:
	.ascii "[antidebug] exit\12\0"
	.text
	.globl	__Z28isdebuggerpresent_callback_2Pv
	.def	__Z28isdebuggerpresent_callback_2Pv;	.scl	2;	.type	32;	.endef
__Z28isdebuggerpresent_callback_2Pv:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$760, %esp
	movl	8(%ebp), %eax
	movl	%eax, -12(%ebp)
	movl	-12(%ebp), %eax
	movl	8(%eax), %eax
	movl	%eax, -16(%ebp)
	movl	$65543, -736(%ebp)
	movl	-16(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	$0, 4(%esp)
	movl	$2097151, (%esp)
	movl	__imp__OpenThread@12, %eax
	call	*%eax
	subl	$12, %esp
	movl	%eax, -20(%ebp)
	leal	-736(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	-20(%ebp), %eax
	movl	%eax, (%esp)
	movl	__imp__GetThreadContext@8, %eax
	call	*%eax
	subl	$8, %esp
	movl	-560(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC133, (%esp)
	call	__Z7d_printPKcz
	movl	$0, -560(%ebp)
	leal	-736(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	-20(%ebp), %eax
	movl	%eax, (%esp)
	movl	__imp__SetThreadContext@8, %eax
	call	*%eax
	subl	$8, %esp
	movl	-20(%ebp), %eax
	movl	%eax, (%esp)
	movl	__imp__CloseHandle@4, %eax
	call	*%eax
	subl	$4, %esp
	movl	-16(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	$0, 4(%esp)
	movl	$2097151, (%esp)
	movl	__imp__OpenThread@12, %eax
	call	*%eax
	subl	$12, %esp
	movl	%eax, -20(%ebp)
	leal	-736(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	-20(%ebp), %eax
	movl	%eax, (%esp)
	movl	__imp__GetThreadContext@8, %eax
	call	*%eax
	subl	$8, %esp
	movl	-20(%ebp), %eax
	movl	%eax, (%esp)
	movl	__imp__CloseHandle@4, %eax
	call	*%eax
	subl	$4, %esp
	movl	$LC134, (%esp)
	call	__Z7d_printPKcz
	movl	-560(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC133, (%esp)
	call	__Z7d_printPKcz
	movl	$LC135, (%esp)
	call	__Z7d_printPKcz
	nop
	leave
	ret
	.globl	__Z14empty_callbackPv
	.def	__Z14empty_callbackPv;	.scl	2;	.type	32;	.endef
__Z14empty_callbackPv:
	pushl	%ebp
	movl	%esp, %ebp
	nop
	popl	%ebp
	ret
	.section .rdata,"dr"
LC136:
	.ascii "Thread32First\12\0"
	.text
	.globl	__Z20register_all_threadsv
	.def	__Z20register_all_threadsv;	.scl	2;	.type	32;	.endef
__Z20register_all_threadsv:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%esi
	pushl	%ebx
	subl	$48, %esp
	movl	$-1, -12(%ebp)
	movl	$0, 4(%esp)
	movl	$4, (%esp)
	call	_CreateToolhelp32Snapshot@8
	subl	$8, %esp
	movl	%eax, -12(%ebp)
	cmpl	$-1, -12(%ebp)
	jne	L287
	movl	$0, %eax
	jmp	L291
L287:
	movl	$28, -40(%ebp)
	leal	-40(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	-12(%ebp), %eax
	movl	%eax, (%esp)
	call	_Thread32First@8
	subl	$8, %esp
	testl	%eax, %eax
	sete	%al
	testb	%al, %al
	je	L289
	movl	$LC136, (%esp)
	call	__Z7d_printPKcz
	movl	-12(%ebp), %eax
	movl	%eax, (%esp)
	movl	__imp__CloseHandle@4, %eax
	call	*%eax
	subl	$4, %esp
	movl	$-1, %eax
	jmp	L291
L289:
	movl	-28(%ebp), %eax
	movl	_myPID, %edx
	cmpl	%edx, %eax
	jne	L290
	movl	_my_trace, %ecx
	movl	-32(%ebp), %edx
	movl	%edx, %eax
	sall	$2, %eax
	addl	%edx, %eax
	sall	$2, %eax
	addl	%ecx, %eax
	addl	$67108992, %eax
	movb	$1, 12(%eax)
	movl	_my_trace, %esi
	movl	-32(%ebp), %ebx
	movl	-32(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	$0, 4(%esp)
	movl	$2097151, (%esp)
	movl	__imp__OpenThread@12, %eax
	call	*%eax
	subl	$12, %esp
	movl	%eax, %edx
	movl	%ebx, %eax
	sall	$2, %eax
	addl	%ebx, %eax
	sall	$2, %eax
	addl	%esi, %eax
	addl	$67108992, %eax
	movl	%edx, 16(%eax)
	movl	_my_trace, %ecx
	movl	-32(%ebp), %edx
	movl	%edx, %eax
	sall	$2, %eax
	addl	%edx, %eax
	sall	$2, %eax
	addl	%ecx, %eax
	addl	$67108992, %eax
	movb	$1, 13(%eax)
	movl	_my_trace, %ecx
	movl	_my_trace, %eax
	movl	68964052(%eax), %edx
	movl	68964048(%eax), %eax
	movl	-32(%ebp), %ebx
	addl	$32, %eax
	movl	%ebx, 8(%ecx,%eax,4)
	movl	_my_trace, %ecx
	movl	68964048(%ecx), %eax
	movl	68964052(%ecx), %edx
	addl	$1, %eax
	adcl	$0, %edx
	movl	%eax, 68964048(%ecx)
	movl	%edx, 68964052(%ecx)
L290:
	leal	-40(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	-12(%ebp), %eax
	movl	%eax, (%esp)
	call	_Thread32Next@8
	subl	$8, %esp
	testl	%eax, %eax
	setne	%al
	testb	%al, %al
	jne	L289
	movl	-12(%ebp), %eax
	movl	%eax, (%esp)
	movl	__imp__CloseHandle@4, %eax
	call	*%eax
	subl	$4, %esp
	movl	$0, %eax
L291:
	leal	-8(%ebp), %esp
	popl	%ebx
	popl	%esi
	popl	%ebp
	ret
	.section .rdata,"dr"
LC137:
	.ascii "# Trap is set for 0x%08x\12\0"
	.text
	.globl	__Z9verify_ssm
	.def	__Z9verify_ssm;	.scl	2;	.type	32;	.endef
__Z9verify_ssm:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%ebx
	subl	$1524, %esp
	cmpl	$0, 8(%ebp)
	jne	L293
	movl	$0, -12(%ebp)
	jmp	L294
L295:
	movl	_my_trace, %eax
	movl	-12(%ebp), %edx
	addl	$32, %edx
	movl	8(%eax,%edx,4), %eax
	movl	%eax, -16(%ebp)
	movl	-16(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z9verify_ssm
	addl	$1, -12(%ebp)
L294:
	movl	-12(%ebp), %eax
	movl	%eax, %ecx
	movl	%eax, %ebx
	sarl	$31, %ebx
	movl	_my_trace, %eax
	movl	68964052(%eax), %edx
	movl	68964048(%eax), %eax
	cmpl	%edx, %ebx
	jb	L295
	cmpl	%edx, %ebx
	ja	L297
	cmpl	%eax, %ecx
	jb	L295
	jmp	L297
L293:
	movl	$65537, -1504(%ebp)
	movl	8(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	$0, 4(%esp)
	movl	$2097151, (%esp)
	movl	__imp__OpenThread@12, %eax
	call	*%eax
	subl	$12, %esp
	movl	%eax, -20(%ebp)
	leal	-1504(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	-20(%ebp), %eax
	movl	%eax, (%esp)
	movl	__imp__GetThreadContext@8, %eax
	call	*%eax
	subl	$8, %esp
	movl	-1312(%ebp), %eax
	andb	$254, %ah
	testl	%eax, %eax
	je	L298
	movl	8(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	$LC137, 4(%esp)
	leal	-788(%ebp), %eax
	movl	%eax, (%esp)
	call	_sprintf
	leal	-788(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z13add_to_bufferPc
L298:
	leal	-1504(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	-20(%ebp), %eax
	movl	%eax, (%esp)
	movl	__imp__SetThreadContext@8, %eax
	call	*%eax
	subl	$8, %esp
	movl	-20(%ebp), %eax
	movl	%eax, (%esp)
	movl	__imp__CloseHandle@4, %eax
	call	*%eax
	subl	$4, %esp
L297:
	movl	$0, %eax
	movl	-4(%ebp), %ebx
	leave
	ret
	.globl	__Z8unset_ssm
	.def	__Z8unset_ssm;	.scl	2;	.type	32;	.endef
__Z8unset_ssm:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%ebx
	subl	$804, %esp
	cmpl	$0, 8(%ebp)
	jne	L301
	movl	$0, -12(%ebp)
	jmp	L302
L303:
	movl	_my_trace, %eax
	movl	-12(%ebp), %edx
	addl	$32, %edx
	movl	8(%eax,%edx,4), %eax
	movl	%eax, -20(%ebp)
	movl	-20(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z8unset_ssm
	addl	$1, -12(%ebp)
L302:
	movl	-12(%ebp), %eax
	movl	%eax, %ecx
	movl	%eax, %ebx
	sarl	$31, %ebx
	movl	_my_trace, %eax
	movl	68964052(%eax), %edx
	movl	68964048(%eax), %eax
	cmpl	%edx, %ebx
	jb	L303
	cmpl	%edx, %ebx
	ja	L305
	cmpl	%eax, %ecx
	jb	L303
	jmp	L305
L301:
	movl	$65537, -788(%ebp)
	movl	8(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	$0, 4(%esp)
	movl	$2097151, (%esp)
	movl	__imp__OpenThread@12, %eax
	call	*%eax
	subl	$12, %esp
	movl	%eax, -16(%ebp)
	cmpl	$0, -16(%ebp)
	jne	L306
	movl	$-1, %eax
	jmp	L308
L306:
	leal	-788(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	-16(%ebp), %eax
	movl	%eax, (%esp)
	movl	__imp__GetThreadContext@8, %eax
	call	*%eax
	subl	$8, %esp
	movl	-596(%ebp), %eax
	andb	$254, %ah
	movl	%eax, -596(%ebp)
	leal	-788(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	-16(%ebp), %eax
	movl	%eax, (%esp)
	movl	__imp__SetThreadContext@8, %eax
	call	*%eax
	subl	$8, %esp
	movl	-16(%ebp), %eax
	movl	%eax, (%esp)
	movl	__imp__CloseHandle@4, %eax
	call	*%eax
	subl	$4, %esp
L305:
	movl	$0, %eax
L308:
	movl	-4(%ebp), %ebx
	leave
	ret
	.section .rdata,"dr"
LC138:
	.ascii "Failed to open TID: 0x%08x\12\0"
	.text
	.globl	__Z6set_ssm
	.def	__Z6set_ssm;	.scl	2;	.type	32;	.endef
__Z6set_ssm:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%ebx
	subl	$804, %esp
	movl	_my_trace, %ecx
	movl	_my_trace, %eax
	movl	8(%ebp), %edx
	addl	$32, %edx
	movl	8(%eax,%edx,4), %edx
	movl	%edx, %eax
	sall	$2, %eax
	addl	%edx, %eax
	sall	$2, %eax
	addl	%ecx, %eax
	addl	$67108992, %eax
	movzbl	20(%eax), %eax
	cmpb	$1, %al
	jne	L310
	movl	$0, %eax
	jmp	L318
L310:
	cmpl	$0, 8(%ebp)
	jne	L312
	movl	$0, -12(%ebp)
	jmp	L313
L314:
	movl	_my_trace, %ecx
	movl	-12(%ebp), %edx
	movl	%edx, %eax
	sall	$2, %eax
	addl	%edx, %eax
	sall	$2, %eax
	addl	%ecx, %eax
	addl	$67108992, %eax
	movl	8(%eax), %eax
	movl	%eax, -20(%ebp)
	movl	-20(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z6set_ssm
	addl	$1, -12(%ebp)
L313:
	movl	-12(%ebp), %eax
	movl	%eax, %ecx
	movl	%eax, %ebx
	sarl	$31, %ebx
	movl	_my_trace, %eax
	movl	68964052(%eax), %edx
	movl	68964048(%eax), %eax
	cmpl	%edx, %ebx
	jb	L314
	cmpl	%edx, %ebx
	ja	L316
	cmpl	%eax, %ecx
	jb	L314
	jmp	L316
L312:
	movl	$65537, -788(%ebp)
	movl	8(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	$0, 4(%esp)
	movl	$2097151, (%esp)
	movl	__imp__OpenThread@12, %eax
	call	*%eax
	subl	$12, %esp
	movl	%eax, -16(%ebp)
	cmpl	$0, -16(%ebp)
	jne	L317
	movl	8(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC138, (%esp)
	call	__Z7d_printPKcz
	movl	$-1, %eax
	jmp	L318
L317:
	leal	-788(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	-16(%ebp), %eax
	movl	%eax, (%esp)
	movl	__imp__GetThreadContext@8, %eax
	call	*%eax
	subl	$8, %esp
	movl	-596(%ebp), %eax
	orb	$1, %ah
	movl	%eax, -596(%ebp)
	leal	-788(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	-16(%ebp), %eax
	movl	%eax, (%esp)
	movl	__imp__SetThreadContext@8, %eax
	call	*%eax
	subl	$8, %esp
	movl	-16(%ebp), %eax
	movl	%eax, (%esp)
	movl	__imp__CloseHandle@4, %eax
	call	*%eax
	subl	$4, %esp
L316:
	movl	$0, %eax
L318:
	movl	-4(%ebp), %ebx
	leave
	ret
	.section .rdata,"dr"
LC139:
	.ascii "Offhit: %08x\12\0"
LC140:
	.ascii "Instr_no: %lld\12\0"
LC141:
	.ascii "Print debug info @ 0x%08x\12\0"
	.align 4
LC142:
	.ascii "Scan start triggered by watchpoint %d @ instr no %lld\12\0"
LC143:
	.ascii "# Scan start\12\0"
LC144:
	.ascii "Previous not hit\12\0"
	.text
	.globl	__Z11check_debugmym
	.def	__Z11check_debugmym;	.scl	2;	.type	32;	.endef
__Z11check_debugmym:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$776, %esp
	movl	12(%ebp), %eax
	movl	%eax, -752(%ebp)
	movl	16(%ebp), %eax
	movl	%eax, -748(%ebp)
	movl	$-1, -16(%ebp)
	movl	_my_trace, %ecx
	movl	20(%ebp), %edx
	movl	%edx, %eax
	sall	$2, %eax
	addl	%edx, %eax
	sall	$2, %eax
	addl	%ecx, %eax
	addl	$67108992, %eax
	movl	16(%eax), %eax
	movl	%eax, -16(%ebp)
	movl	$0, -12(%ebp)
	jmp	L320
L327:
	movl	-12(%ebp), %edx
	movl	%edx, %eax
	addl	%eax, %eax
	addl	%edx, %eax
	sall	$3, %eax
	addl	$_watched, %eax
	movl	(%eax), %eax
	cmpl	8(%ebp), %eax
	je	L321
	movl	-12(%ebp), %edx
	movl	%edx, %eax
	addl	%eax, %eax
	addl	%edx, %eax
	sall	$3, %eax
	addl	$_watched+16, %eax
	movl	4(%eax), %edx
	movl	(%eax), %eax
	movl	%edx, %ecx
	xorl	-748(%ebp), %ecx
	xorl	-752(%ebp), %eax
	orl	%ecx, %eax
	testl	%eax, %eax
	jne	L322
L321:
	movl	8(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC139, (%esp)
	call	__Z7d_printPKcz
	movl	_my_trace, %eax
	movl	68964020(%eax), %edx
	movl	68964016(%eax), %eax
	movl	%eax, 4(%esp)
	movl	%edx, 8(%esp)
	movl	$LC140, (%esp)
	call	__Z7d_printPKcz
	movl	-12(%ebp), %edx
	movl	%edx, %eax
	addl	%eax, %eax
	addl	%edx, %eax
	sall	$3, %eax
	addl	$_watched, %eax
	movl	4(%eax), %ecx
	movl	-12(%ebp), %edx
	movl	%edx, %eax
	addl	%eax, %eax
	addl	%edx, %eax
	sall	$3, %eax
	addl	$_watched, %eax
	movl	12(%eax), %eax
	cmpl	%eax, %ecx
	jg	L323
	movl	$65543, -732(%ebp)
	leal	-732(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	-16(%ebp), %eax
	movl	%eax, (%esp)
	movl	__imp__GetThreadContext@8, %eax
	call	*%eax
	subl	$8, %esp
	testl	%eax, %eax
	sete	%al
	testb	%al, %al
	je	L324
	movl	__imp__GetLastError@0, %eax
	call	*%eax
	movl	%eax, 4(%esp)
	movl	$LC7, (%esp)
	call	__Z7d_printPKcz
L324:
	movl	8(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC141, (%esp)
	call	__Z7d_printPKcz
	leal	-732(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z13print_contextP8_CONTEXT
	movl	-12(%ebp), %edx
	movl	%edx, %eax
	addl	%eax, %eax
	addl	%edx, %eax
	sall	$3, %eax
	addl	$_watched, %eax
	movl	12(%eax), %eax
	leal	1(%eax), %ecx
	movl	-12(%ebp), %edx
	movl	%edx, %eax
	addl	%eax, %eax
	addl	%edx, %eax
	sall	$3, %eax
	addl	$_watched, %eax
	movl	%ecx, 12(%eax)
	movl	-12(%ebp), %edx
	movl	%edx, %eax
	addl	%eax, %eax
	addl	%edx, %eax
	sall	$3, %eax
	addl	$_watched, %eax
	movl	8(%eax), %eax
	cmpl	$1, %eax
	jne	L325
	movl	-752(%ebp), %eax
	movl	-748(%ebp), %edx
	movl	%eax, 8(%esp)
	movl	%edx, 12(%esp)
	movl	-12(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC142, (%esp)
	call	__Z7d_printPKcz
	movl	_file, %eax
	movl	%eax, 12(%esp)
	movl	$1, 8(%esp)
	movl	$13, 4(%esp)
	movl	$LC143, (%esp)
	call	_fwrite
	movl	$1, _scan_on
L325:
	jmp	L326
L323:
	movl	-12(%ebp), %edx
	movl	%edx, %eax
	addl	%eax, %eax
	addl	%edx, %eax
	sall	$3, %eax
	addl	$_watched, %eax
	movl	12(%eax), %eax
	leal	1(%eax), %ecx
	movl	-12(%ebp), %edx
	movl	%edx, %eax
	addl	%eax, %eax
	addl	%edx, %eax
	sall	$3, %eax
	addl	$_watched, %eax
	movl	%ecx, 12(%eax)
L326:
	movl	$LC144, (%esp)
	call	__Z7d_printPKcz
L322:
	addl	$1, -12(%ebp)
L320:
	cmpl	$255, -12(%ebp)
	jbe	L327
	nop
	leave
	ret
	.globl	__Z7is_callj
	.def	__Z7is_callj;	.scl	2;	.type	32;	.endef
__Z7is_callj:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$808, %esp
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z9read_bytem
	movb	%al, -9(%ebp)
	cmpb	$-24, -9(%ebp)
	je	L330
	cmpb	$-1, -9(%ebp)
	jne	L331
L330:
	movl	$1, %eax
	jmp	L333
L331:
	movl	$0, %eax
L333:
	leave
	ret
	.section .rdata,"dr"
LC145:
	.ascii "Error writing to file: 0x%x\12\0"
	.def	___umoddi3;	.scl	2;	.type	32;	.endef
LC146:
	.ascii "0x%x 0x%08x %lld\12\0"
	.text
	.globl	__Z11ss_callbackPv
	.def	__Z11ss_callbackPv;	.scl	2;	.type	32;	.endef
__Z11ss_callbackPv:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%ebx
	subl	$836, %esp
	movl	8(%ebp), %eax
	movl	%eax, -12(%ebp)
	movl	$0, -16(%ebp)
	movl	-12(%ebp), %eax
	movl	24(%eax), %eax
	movl	%eax, -20(%ebp)
	movl	_my_trace, %eax
	movzbl	68964008(%eax), %eax
	cmpb	$3, %al
	je	L335
	movl	_my_trace, %eax
	movzbl	68964008(%eax), %eax
	cmpb	$4, %al
	je	L335
	movl	_my_trace, %eax
	movzbl	68964008(%eax), %eax
	cmpb	$5, %al
	je	L335
	jmp	L334
L335:
	movl	-12(%ebp), %eax
	movl	8(%eax), %eax
	movl	%eax, -24(%ebp)
	movl	_my_trace, %eax
	movl	-24(%ebp), %edx
	addl	$32, %edx
	movl	8(%eax,%edx,4), %eax
	movl	%eax, -28(%ebp)
	movl	_my_trace, %eax
	movl	67642512(%eax), %eax
	movl	$-1, 4(%esp)
	movl	%eax, (%esp)
	movl	__imp__WaitForSingleObject@8, %eax
	call	*%eax
	subl	$8, %esp
	movl	-12(%ebp), %eax
	movl	24(%eax), %eax
	movl	%eax, -20(%ebp)
	movl	_my_trace, %eax
	movl	-20(%ebp), %edx
	movl	%edx, 68964272(%eax)
	movl	_my_trace, %eax
	movl	-24(%ebp), %edx
	movl	%edx, 68964276(%eax)
	movl	_my_trace, %eax
	movzbl	68964008(%eax), %eax
	cmpb	$4, %al
	jne	L337
	movl	_my_trace, %ecx
	movl	-28(%ebp), %edx
	movl	%edx, %eax
	sall	$2, %eax
	addl	%edx, %eax
	sall	$2, %eax
	addl	%ecx, %eax
	addl	$67108992, %eax
	movl	16(%eax), %eax
	movl	%eax, 4(%esp)
	movl	-24(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z21register_thread_debugmPv
	testl	%eax, %eax
	setle	%al
	testb	%al, %al
	je	L337
	movl	__imp__GetLastError@0, %eax
	call	*%eax
	movl	%eax, 4(%esp)
	movl	$LC145, (%esp)
	call	__Z7d_printPKcz
	movl	$1, (%esp)
	call	_exit
L337:
	movl	_my_trace, %eax
	movzbl	68964008(%eax), %eax
	cmpb	$5, %al
	jne	L338
	movl	-20(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z7is_callj
	testl	%eax, %eax
	setne	%al
	testb	%al, %al
	je	L338
	movl	_my_trace, %ecx
	movl	-28(%ebp), %edx
	movl	%edx, %eax
	sall	$2, %eax
	addl	%edx, %eax
	sall	$2, %eax
	addl	%ecx, %eax
	addl	$67108992, %eax
	movl	16(%eax), %eax
	movl	%eax, 4(%esp)
	movl	-24(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z21register_thread_debugmPv
	testl	%eax, %eax
	setle	%al
	testb	%al, %al
	je	L338
	movl	__imp__GetLastError@0, %eax
	call	*%eax
	movl	%eax, 4(%esp)
	movl	$LC145, (%esp)
	call	__Z7d_printPKcz
	movl	$1, (%esp)
	call	_exit
L338:
	movl	_my_trace, %eax
	movl	68964020(%eax), %edx
	movl	68964016(%eax), %eax
	movl	$10000000, 8(%esp)
	movl	$0, 12(%esp)
	movl	%eax, (%esp)
	movl	%edx, 4(%esp)
	call	___umoddi3
	orl	%edx, %eax
	testl	%eax, %eax
	jne	L339
	movl	_my_trace, %eax
	movl	68964020(%eax), %edx
	movl	68964016(%eax), %eax
	orl	%edx, %eax
	testl	%eax, %eax
	je	L339
	call	__Z15reload_out_filev
L339:
	movl	_my_trace, %ecx
	movl	68964016(%ecx), %eax
	movl	68964020(%ecx), %edx
	addl	$1, %eax
	adcl	$0, %edx
	movl	%eax, 68964016(%ecx)
	movl	%edx, 68964020(%ecx)
	movl	_my_trace, %eax
	movl	68964020(%eax), %edx
	movl	68964016(%eax), %eax
	movl	%eax, 16(%esp)
	movl	%edx, 20(%esp)
	movl	-20(%ebp), %eax
	movl	%eax, 12(%esp)
	movl	-24(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	$LC146, 4(%esp)
	leal	-798(%ebp), %eax
	movl	%eax, (%esp)
	call	_sprintf
	leal	-798(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z13add_to_bufferPc
	movl	_my_trace, %eax
	movl	68964036(%eax), %edx
	movl	68964032(%eax), %eax
	orl	%edx, %eax
	testl	%eax, %eax
	je	L340
	movl	_my_trace, %eax
	movl	68964016(%eax), %ecx
	movl	68964020(%eax), %ebx
	movl	_my_trace, %eax
	movl	68964036(%eax), %edx
	movl	68964032(%eax), %eax
	cmpl	%edx, %ebx
	jb	L340
	cmpl	%edx, %ebx
	ja	L343
	cmpl	%eax, %ecx
	jb	L340
L343:
	movl	$0, (%esp)
	call	__Z14HandlerRoutinem@4
	subl	$4, %esp
L340:
	movl	_my_trace, %eax
	movl	67642512(%eax), %eax
	movl	%eax, (%esp)
	movl	__imp__ReleaseMutex@4, %eax
	call	*%eax
	subl	$4, %esp
	nop
L334:
	movl	-4(%ebp), %ebx
	leave
	ret
	.globl	__Z15page_accessible25_MEMORY_BASIC_INFORMATION
	.def	__Z15page_accessible25_MEMORY_BASIC_INFORMATION;	.scl	2;	.type	32;	.endef
__Z15page_accessible25_MEMORY_BASIC_INFORMATION:
	pushl	%ebp
	movl	%esp, %ebp
	movl	16(%ebp), %eax
	andl	$256, %eax
	testl	%eax, %eax
	je	L345
	movl	$0, %eax
	jmp	L346
L345:
	movl	28(%ebp), %eax
	andl	$256, %eax
	testl	%eax, %eax
	je	L347
	movl	$0, %eax
	jmp	L346
L347:
	movl	16(%ebp), %eax
	andl	$1, %eax
	testl	%eax, %eax
	je	L348
	movl	$0, %eax
	jmp	L346
L348:
	movl	28(%ebp), %eax
	andl	$1, %eax
	testl	%eax, %eax
	je	L349
	movl	$0, %eax
	jmp	L346
L349:
	movl	$1, %eax
L346:
	popl	%ebp
	ret
	.section .rdata,"dr"
LC147:
	.ascii "Position before: 0x%08x\12\0"
LC148:
	.ascii "Read: 0x%08x\12\0"
	.align 4
LC149:
	.ascii "Failed to read from %p to %p\12Error: 0x%08x\12\0"
LC150:
	.ascii "0x%02x \0"
LC151:
	.ascii "\12\0"
LC152:
	.ascii "Position after: 0x%08x\12\0"
	.text
	.globl	__Z8dump_memP6_iobufPvm
	.def	__Z8dump_memP6_iobufPvm;	.scl	2;	.type	32;	.endef
__Z8dump_memP6_iobufPvm:
	pushl	%ebp
	movl	%esp, %ebp
	movl	$1048648, %eax
	call	___chkstk_ms
	subl	%eax, %esp
	movl	$0, -16(%ebp)
	movl	16(%ebp), %eax
	andl	$1048575, %eax
	movl	%eax, -24(%ebp)
	movl	16(%ebp), %eax
	subl	-24(%ebp), %eax
	movl	%eax, -28(%ebp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_ftell
	movl	%eax, 4(%esp)
	movl	$LC147, (%esp)
	call	__Z7d_printPKcz
	movl	$0, -12(%ebp)
	jmp	L351
L354:
	movl	12(%ebp), %edx
	movl	-12(%ebp), %eax
	leal	(%edx,%eax), %ecx
	movl	_my_trace, %eax
	movl	128(%eax), %eax
	leal	-32(%ebp), %edx
	movl	%edx, 16(%esp)
	movl	$1048576, 12(%esp)
	leal	-1048608(%ebp), %edx
	movl	%edx, 8(%esp)
	movl	%ecx, 4(%esp)
	movl	%eax, (%esp)
	movl	__imp__ReadProcessMemory@20, %eax
	call	*%eax
	subl	$20, %esp
	movl	-32(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC148, (%esp)
	call	__Z7d_printPKcz
	movl	-32(%ebp), %eax
	testl	%eax, %eax
	jne	L352
	movl	__imp__GetLastError@0, %eax
	call	*%eax
	movl	%eax, %ecx
	movl	12(%ebp), %edx
	movl	-24(%ebp), %eax
	addl	%edx, %eax
	movl	%ecx, 12(%esp)
	movl	%eax, 8(%esp)
	movl	12(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC149, (%esp)
	call	__Z7d_printPKcz
	jmp	L353
L352:
	movl	-32(%ebp), %eax
	movl	8(%ebp), %edx
	movl	%edx, 12(%esp)
	movl	$1, 8(%esp)
	movl	%eax, 4(%esp)
	leal	-1048608(%ebp), %eax
	movl	%eax, (%esp)
	call	_fwrite
	movl	-32(%ebp), %eax
	addl	%eax, -16(%ebp)
	addl	$1048576, -12(%ebp)
L351:
	movl	-12(%ebp), %eax
	cmpl	-28(%ebp), %eax
	jb	L354
L353:
	movl	$0, -20(%ebp)
	jmp	L355
L356:
	leal	-1048608(%ebp), %edx
	movl	-20(%ebp), %eax
	addl	%edx, %eax
	movzbl	(%eax), %eax
	movsbl	%al, %eax
	movl	%eax, 4(%esp)
	movl	$LC150, (%esp)
	call	__Z7d_printPKcz
	addl	$1, -20(%ebp)
L355:
	cmpl	$4, -20(%ebp)
	jbe	L356
	movl	$LC151, (%esp)
	call	__Z7d_printPKcz
	cmpl	$0, -24(%ebp)
	je	L357
	movl	12(%ebp), %edx
	movl	-12(%ebp), %eax
	leal	(%edx,%eax), %ecx
	movl	_my_trace, %eax
	movl	128(%eax), %eax
	leal	-32(%ebp), %edx
	movl	%edx, 16(%esp)
	movl	-24(%ebp), %edx
	movl	%edx, 12(%esp)
	leal	-1048608(%ebp), %edx
	movl	%edx, 8(%esp)
	movl	%ecx, 4(%esp)
	movl	%eax, (%esp)
	movl	__imp__ReadProcessMemory@20, %eax
	call	*%eax
	subl	$20, %esp
	movl	-32(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC148, (%esp)
	call	__Z7d_printPKcz
	movl	-32(%ebp), %eax
	testl	%eax, %eax
	jne	L358
	movl	__imp__GetLastError@0, %eax
	call	*%eax
	movl	%eax, %ecx
	movl	12(%ebp), %edx
	movl	-24(%ebp), %eax
	addl	%edx, %eax
	movl	%ecx, 12(%esp)
	movl	%eax, 8(%esp)
	movl	12(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC149, (%esp)
	call	__Z7d_printPKcz
L358:
	movl	-32(%ebp), %eax
	movl	8(%ebp), %edx
	movl	%edx, 12(%esp)
	movl	$1, 8(%esp)
	movl	%eax, 4(%esp)
	leal	-1048608(%ebp), %eax
	movl	%eax, (%esp)
	call	_fwrite
	movl	-32(%ebp), %eax
	addl	%eax, -16(%ebp)
L357:
	movl	$0, -20(%ebp)
	jmp	L359
L360:
	leal	-1048608(%ebp), %edx
	movl	-20(%ebp), %eax
	addl	%edx, %eax
	movzbl	(%eax), %eax
	movsbl	%al, %eax
	movl	%eax, 4(%esp)
	movl	$LC150, (%esp)
	call	__Z7d_printPKcz
	addl	$1, -20(%ebp)
L359:
	cmpl	$4, -20(%ebp)
	jbe	L360
	movl	$LC151, (%esp)
	call	__Z7d_printPKcz
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_ftell
	movl	%eax, 4(%esp)
	movl	$LC152, (%esp)
	call	__Z7d_printPKcz
	movl	-16(%ebp), %eax
	leave
	ret
	.globl	__Z10dump_zerosP6_iobufm
	.def	__Z10dump_zerosP6_iobufm;	.scl	2;	.type	32;	.endef
__Z10dump_zerosP6_iobufm:
	pushl	%ebp
	movl	%esp, %ebp
	movl	$1048616, %eax
	call	___chkstk_ms
	subl	%eax, %esp
	movl	$0, -16(%ebp)
	movl	12(%ebp), %eax
	andl	$1048575, %eax
	movl	%eax, -20(%ebp)
	movl	12(%ebp), %eax
	subl	-20(%ebp), %eax
	movl	%eax, -24(%ebp)
	movl	$1048576, 8(%esp)
	movl	$0, 4(%esp)
	leal	-1048600(%ebp), %eax
	movl	%eax, (%esp)
	call	_memset
	movl	$0, -12(%ebp)
	jmp	L363
L364:
	movl	8(%ebp), %eax
	movl	%eax, 12(%esp)
	movl	$1, 8(%esp)
	movl	$1048576, 4(%esp)
	leal	-1048600(%ebp), %eax
	movl	%eax, (%esp)
	call	_fwrite
	addl	$1048576, -16(%ebp)
	addl	$1048576, -12(%ebp)
L363:
	movl	-12(%ebp), %eax
	cmpl	-24(%ebp), %eax
	jb	L364
	cmpl	$0, -20(%ebp)
	je	L365
	movl	8(%ebp), %eax
	movl	%eax, 12(%esp)
	movl	$1, 8(%esp)
	movl	-20(%ebp), %eax
	movl	%eax, 4(%esp)
	leal	-1048600(%ebp), %eax
	movl	%eax, (%esp)
	call	_fwrite
	movl	-20(%ebp), %eax
	addl	%eax, -16(%ebp)
L365:
	movl	-16(%ebp), %eax
	leave
	ret
	.section .rdata,"dr"
LC153:
	.ascii "Dumping mem start\12\0"
LC154:
	.ascii "wb\0"
	.align 4
LC155:
	.ascii "Block 0x%08x - 0x%08x, 0x%08x bytes\12\0"
LC156:
	.ascii "dumping mem end\12\0"
LC157:
	.ascii "LM,%s.dump\12\0"
	.text
	.globl	__Z11dump_memoryv
	.def	__Z11dump_memoryv;	.scl	2;	.type	32;	.endef
__Z11dump_memoryv:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%ebx
	subl	$852, %esp
	movl	$LC153, (%esp)
	call	__Z7d_printPKcz
	movl	_my_trace, %ebx
	movl	_my_trace, %eax
	addl	$70018284, %eax
	movl	$LC154, 4(%esp)
	movl	%eax, (%esp)
	call	_fopen
	movl	%eax, 70012884(%ebx)
	movl	$0, -12(%ebp)
	jmp	L368
L375:
	movl	-12(%ebp), %edx
	movl	_my_trace, %eax
	movl	128(%eax), %eax
	movl	$28, 12(%esp)
	leal	-44(%ebp), %ecx
	movl	%ecx, 8(%esp)
	movl	%edx, 4(%esp)
	movl	%eax, (%esp)
	movl	__imp__VirtualQueryEx@16, %eax
	call	*%eax
	subl	$16, %esp
	movl	__imp__GetLastError@0, %eax
	call	*%eax
	cmpl	$87, %eax
	sete	%al
	testb	%al, %al
	je	L369
	jmp	L370
L369:
	movl	-32(%ebp), %edx
	movl	-44(%ebp), %ecx
	movl	-32(%ebp), %eax
	addl	%eax, %ecx
	movl	-44(%ebp), %eax
	movl	%edx, 12(%esp)
	movl	%ecx, 8(%esp)
	movl	%eax, 4(%esp)
	movl	$LC155, (%esp)
	call	__Z7d_printPKcz
	movl	-28(%ebp), %eax
	cmpl	$4096, %eax
	jne	L371
	movl	-44(%ebp), %eax
	movl	%eax, (%esp)
	movl	-40(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	-36(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	-32(%ebp), %eax
	movl	%eax, 12(%esp)
	movl	-28(%ebp), %eax
	movl	%eax, 16(%esp)
	movl	-24(%ebp), %eax
	movl	%eax, 20(%esp)
	movl	-20(%ebp), %eax
	movl	%eax, 24(%esp)
	call	__Z15page_accessible25_MEMORY_BASIC_INFORMATION
	testl	%eax, %eax
	je	L371
	movl	$1, %eax
	jmp	L372
L371:
	movl	$0, %eax
L372:
	testb	%al, %al
	je	L373
	movl	-32(%ebp), %ecx
	movl	-44(%ebp), %edx
	movl	_my_trace, %eax
	movl	70012884(%eax), %eax
	movl	%ecx, 8(%esp)
	movl	%edx, 4(%esp)
	movl	%eax, (%esp)
	call	__Z8dump_memP6_iobufPvm
	movl	%eax, -16(%ebp)
	jmp	L374
L373:
	movl	-32(%ebp), %edx
	movl	_my_trace, %eax
	movl	70012884(%eax), %eax
	movl	%edx, 4(%esp)
	movl	%eax, (%esp)
	call	__Z10dump_zerosP6_iobufm
	movl	%eax, -16(%ebp)
L374:
	movl	-32(%ebp), %eax
	addl	%eax, -12(%ebp)
L368:
	cmpl	$-1, -12(%ebp)
	jne	L375
L370:
	movl	_my_trace, %eax
	movl	70012884(%eax), %eax
	movl	%eax, (%esp)
	call	_fclose
	movl	$LC156, (%esp)
	call	__Z7d_printPKcz
	movl	_my_trace, %eax
	addl	$70016744, %eax
	movl	%eax, 8(%esp)
	movl	$LC157, 4(%esp)
	leal	-812(%ebp), %eax
	movl	%eax, (%esp)
	call	_sprintf
	leal	-812(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z13add_to_bufferPc
	movl	-4(%ebp), %ebx
	leave
	ret
	.section .rdata,"dr"
LC158:
	.ascii "Dumping contexts\12\0"
	.text
	.globl	__Z13dump_contextsv
	.def	__Z13dump_contextsv;	.scl	2;	.type	32;	.endef
__Z13dump_contextsv:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$24, %esp
	movl	$LC158, (%esp)
	call	__Z7d_printPKcz
	leave
	ret
	.section .rdata,"dr"
	.align 4
LC159:
	.ascii "Finishing @ 0x%08x, detaching\12\0"
LC160:
	.ascii "FI,0x%08x\12\0"
	.text
	.globl	__Z12end_callbackPv
	.def	__Z12end_callbackPv;	.scl	2;	.type	32;	.endef
__Z12end_callbackPv:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$808, %esp
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z11ss_callbackPv
	movl	8(%ebp), %eax
	movl	%eax, -12(%ebp)
	movl	-12(%ebp), %eax
	movl	24(%eax), %eax
	movl	%eax, -16(%ebp)
	movl	-16(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC159, (%esp)
	call	__Z7d_printPKcz
	movl	-16(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	$LC160, 4(%esp)
	leal	-784(%ebp), %eax
	movl	%eax, (%esp)
	call	_sprintf
	leal	-784(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z13add_to_bufferPc
	movl	$0, (%esp)
	call	__Z14HandlerRoutinem@4
	subl	$4, %esp
	leave
	ret
	.section .rdata,"dr"
LC161:
	.ascii "Detected ctrl-c\12\0"
LC162:
	.ascii "Deregistering threads\12\0"
LC163:
	.ascii "Deregistering: 0x%08x\12\0"
LC164:
	.ascii "Deregistering libs\12\0"
LC165:
	.ascii "instruction count\0"
LC166:
	.ascii "general\0"
	.text
	.globl	__Z14HandlerRoutinem@4
	.def	__Z14HandlerRoutinem@4;	.scl	2;	.type	32;	.endef
__Z14HandlerRoutinem@4:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%ebx
	subl	$36, %esp
	movl	$LC161, (%esp)
	call	__Z7d_printPKcz
	movl	$LC162, (%esp)
	call	__Z7d_printPKcz
	movl	$0, -12(%ebp)
	jmp	L379
L381:
	movl	-12(%ebp), %eax
	movl	%eax, -16(%ebp)
	movl	_my_trace, %ecx
	movl	-16(%ebp), %edx
	movl	%edx, %eax
	sall	$2, %eax
	addl	%edx, %eax
	sall	$2, %eax
	addl	%ecx, %eax
	addl	$67108992, %eax
	movzbl	12(%eax), %eax
	cmpb	$1, %al
	jne	L380
	movl	_my_trace, %ecx
	movl	-16(%ebp), %edx
	movl	%edx, %eax
	sall	$2, %eax
	addl	%edx, %eax
	sall	$2, %eax
	addl	%ecx, %eax
	addl	$67108992, %eax
	movl	8(%eax), %eax
	movl	%eax, 4(%esp)
	movl	$LC163, (%esp)
	call	__Z7d_printPKcz
	movl	_my_trace, %ecx
	movl	-16(%ebp), %edx
	movl	%edx, %eax
	sall	$2, %eax
	addl	%edx, %eax
	sall	$2, %eax
	addl	%ecx, %eax
	addl	$67108992, %eax
	movl	16(%eax), %ecx
	movl	_my_trace, %ebx
	movl	-16(%ebp), %edx
	movl	%edx, %eax
	sall	$2, %eax
	addl	%edx, %eax
	sall	$2, %eax
	addl	%ebx, %eax
	addl	$67108992, %eax
	movl	8(%eax), %eax
	movl	%ecx, 4(%esp)
	movl	%eax, (%esp)
	call	__Z17deregister_threadmPv
L380:
	addl	$1, -12(%ebp)
L379:
	movl	-12(%ebp), %ecx
	movl	$0, %ebx
	movl	_my_trace, %eax
	movl	68964052(%eax), %edx
	movl	68964048(%eax), %eax
	cmpl	%edx, %ebx
	jb	L381
	cmpl	%edx, %ebx
	ja	L387
	cmpl	%eax, %ecx
	jb	L381
L387:
	movl	$LC164, (%esp)
	call	__Z7d_printPKcz
	movl	$0, -12(%ebp)
	jmp	L383
L385:
	movl	_my_trace, %edx
	movl	-12(%ebp), %eax
	sall	$3, %eax
	movl	%eax, %ecx
	sall	$7, %ecx
	addl	%ecx, %eax
	addl	%edx, %eax
	addl	$67114112, %eax
	movzbl	8(%eax), %eax
	cmpb	$1, %al
	jne	L384
	movl	-12(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z14deregister_libm
L384:
	addl	$1, -12(%ebp)
L383:
	movl	-12(%ebp), %ecx
	movl	$0, %ebx
	movl	_my_trace, %eax
	movl	68964060(%eax), %edx
	movl	68964056(%eax), %eax
	cmpl	%edx, %ebx
	jb	L385
	cmpl	%edx, %ebx
	ja	L388
	cmpl	%eax, %ecx
	jb	L385
L388:
	movl	_my_trace, %eax
	leal	70019052(%eax), %ebx
	movl	_my_trace, %eax
	movl	68964020(%eax), %edx
	movl	68964016(%eax), %eax
	movl	$10, 8(%esp)
	movl	$_instr_count_s, 4(%esp)
	movl	%eax, (%esp)
	call	_itoa
	movl	%ebx, 12(%esp)
	movl	%eax, 8(%esp)
	movl	$LC165, 4(%esp)
	movl	$LC166, (%esp)
	movl	__imp__WritePrivateProfileStringA@16, %eax
	call	*%eax
	subl	$16, %esp
	movl	_my_trace, %eax
	movl	67642504(%eax), %eax
	movl	%eax, (%esp)
	movl	__imp__SetEvent@4, %eax
	call	*%eax
	subl	$4, %esp
	movl	_my_trace, %eax
	movl	67642508(%eax), %eax
	movl	$-1, 4(%esp)
	movl	%eax, (%esp)
	movl	__imp__WaitForSingleObject@8, %eax
	call	*%eax
	subl	$8, %esp
	movl	$1, (%esp)
	call	_exit
	.globl	__Z6writerPv@4
	.def	__Z6writerPv@4;	.scl	2;	.type	32;	.endef
__Z6writerPv@4:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$24, %esp
L390:
	movl	_my_trace, %eax
	movl	67642504(%eax), %eax
	movl	$-1, 4(%esp)
	movl	%eax, (%esp)
	movl	__imp__WaitForSingleObject@8, %eax
	call	*%eax
	subl	$8, %esp
	movl	_my_trace, %eax
	movl	67642504(%eax), %eax
	movl	%eax, (%esp)
	movl	__imp__ResetEvent@4, %eax
	call	*%eax
	subl	$4, %esp
	movl	_my_trace, %eax
	movl	70012876(%eax), %eax
	movl	_index, %edx
	movl	%eax, 12(%esp)
	movl	$1, 8(%esp)
	movl	%edx, 4(%esp)
	movl	$_buffer, (%esp)
	call	_fwrite
	movl	_my_trace, %eax
	movl	70012876(%eax), %eax
	movl	%eax, (%esp)
	call	_fflush
	movl	_my_trace, %eax
	movl	67642508(%eax), %eax
	movl	%eax, (%esp)
	movl	__imp__SetEvent@4, %eax
	call	*%eax
	subl	$4, %esp
	jmp	L390
	.section .rdata,"dr"
LC167:
	.ascii "Default handler!\12\0"
	.text
	.globl	__Z15default_handlerPv
	.def	__Z15default_handlerPv;	.scl	2;	.type	32;	.endef
__Z15default_handlerPv:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$24, %esp
	movl	$LC167, (%esp)
	call	__Z7d_printPKcz
	nop
	leave
	ret
	.globl	__Z18del_breakpoint_idxj
	.def	__Z18del_breakpoint_idxj;	.scl	2;	.type	32;	.endef
__Z18del_breakpoint_idxj:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$24, %esp
	movl	_my_trace, %eax
	movl	8(%ebp), %edx
	imull	$532, %edx, %edx
	addl	$67642512, %edx
	addl	%edx, %eax
	addl	$4, %eax
	movl	%eax, (%esp)
	call	__Z18unwrite_breakpointP11BREAKPOINT_
	movl	_my_trace, %edx
	movl	8(%ebp), %eax
	imull	$532, %eax, %eax
	addl	%edx, %eax
	addl	$67643024, %eax
	movb	$0, 13(%eax)
	movl	$0, %eax
	leave
	ret
	.section .rdata,"dr"
	.align 4
LC168:
	.ascii "Unable to find breakpoint to delete\12\0"
	.text
	.globl	__Z14del_breakpointm
	.def	__Z14del_breakpointm;	.scl	2;	.type	32;	.endef
__Z14del_breakpointm:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%ebx
	subl	$36, %esp
	movb	$-52, -17(%ebp)
	movl	$-1, -16(%ebp)
	movl	$0, -12(%ebp)
	jmp	L396
L399:
	movl	_my_trace, %edx
	movl	-12(%ebp), %eax
	imull	$532, %eax, %eax
	addl	%edx, %eax
	addl	$67643024, %eax
	movl	8(%eax), %eax
	cmpl	8(%ebp), %eax
	jne	L397
	movl	-12(%ebp), %eax
	movl	%eax, -16(%ebp)
	jmp	L398
L397:
	addl	$1, -12(%ebp)
L396:
	movl	-12(%ebp), %eax
	movl	%eax, %ecx
	movl	%eax, %ebx
	sarl	$31, %ebx
	movl	_my_trace, %eax
	movl	68964068(%eax), %edx
	movl	68964064(%eax), %eax
	cmpl	%edx, %ebx
	jb	L399
	cmpl	%edx, %ebx
	ja	L398
	cmpl	%eax, %ecx
	jb	L399
L398:
	cmpl	$-1, -16(%ebp)
	jne	L401
	movl	$LC168, (%esp)
	call	__Z7d_printPKcz
	movl	$1, %eax
	jmp	L402
L401:
	movl	_my_trace, %eax
	movl	-12(%ebp), %edx
	imull	$532, %edx, %edx
	addl	$67642512, %edx
	addl	%edx, %eax
	addl	$4, %eax
	movl	%eax, (%esp)
	call	__Z18unwrite_breakpointP11BREAKPOINT_
	movl	_my_trace, %edx
	movl	-16(%ebp), %eax
	imull	$532, %eax, %eax
	addl	%edx, %eax
	addl	$67643024, %eax
	movb	$0, 13(%eax)
	movl	$0, %eax
L402:
	addl	$36, %esp
	popl	%ebx
	popl	%ebp
	ret
	.section .rdata,"dr"
LC169:
	.ascii "Current reaction: %s\12\0"
	.align 4
LC170:
	.ascii "Enabling coupled reaction: %s\12\0"
LC171:
	.ascii "Found coupled reaction: %s\12\0"
	.align 4
LC172:
	.ascii "ER3 Reaction lock is active, continuing, missing reaction %s due to lock by %s\12\0"
	.align 4
LC173:
	.ascii "ER3 Reaction lock %s overriden by %s\12\0"
	.align 4
LC174:
	.ascii "ER32 in handle_reaction: %p, %s\12\0"
	.align 4
LC175:
	.ascii "ER3 Locking reaction lock with: %s\12\0"
	.align 4
LC176:
	.ascii "Routine is null, reporting to controller\12\0"
	.align 4
LC177:
	.ascii "ER3 Executing routine 0x%08x @ %d\12\0"
	.align 4
LC178:
	.ascii "OU,0x%x,0x%08x Routine 0x%08x\12\0"
	.text
	.globl	__Z15handle_reactionP9REACTION_Pv
	.def	__Z15handle_reactionP9REACTION_Pv;	.scl	2;	.type	32;	.endef
__Z15handle_reactionP9REACTION_Pv:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%ebx
	subl	$836, %esp
	movl	12(%ebp), %eax
	movl	%eax, -16(%ebp)
	movl	-16(%ebp), %eax
	movl	8(%eax), %eax
	movl	%eax, -20(%ebp)
	movl	_my_trace, %eax
	movl	-20(%ebp), %edx
	addl	$32, %edx
	movl	8(%eax,%edx,4), %eax
	movl	%eax, -24(%ebp)
	movl	$0, -12(%ebp)
	jmp	L404
L406:
	movl	8(%ebp), %ecx
	movl	-12(%ebp), %edx
	movl	%edx, %eax
	addl	%eax, %eax
	addl	%edx, %eax
	addl	%ecx, %eax
	movzbl	3(%eax), %eax
	testb	%al, %al
	je	L405
	movl	8(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC169, (%esp)
	call	__Z7d_printPKcz
	movl	-12(%ebp), %edx
	movl	%edx, %eax
	addl	%eax, %eax
	addl	%edx, %eax
	movl	8(%ebp), %edx
	addl	%edx, %eax
	addl	$3, %eax
	movl	%eax, 4(%esp)
	movl	$LC170, (%esp)
	call	__Z7d_printPKcz
	movl	-12(%ebp), %edx
	movl	%edx, %eax
	addl	%eax, %eax
	addl	%edx, %eax
	movl	8(%ebp), %edx
	addl	%edx, %eax
	addl	$3, %eax
	movl	%eax, (%esp)
	call	__Z13find_reactionPc
	movl	%eax, -28(%ebp)
	movl	-28(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC171, (%esp)
	call	__Z7d_printPKcz
	movl	-28(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z15enable_reactionPc
L405:
	addl	$1, -12(%ebp)
L404:
	cmpl	$31, -12(%ebp)
	jbe	L406
	movl	_my_trace, %ecx
	movl	-24(%ebp), %edx
	movl	%edx, %eax
	sall	$2, %eax
	addl	%edx, %eax
	sall	$2, %eax
	addl	%ecx, %eax
	addl	$67109008, %eax
	movl	8(%eax), %eax
	movl	%eax, -32(%ebp)
	cmpl	$0, -32(%ebp)
	je	L407
	movl	8(%ebp), %eax
	movl	176(%eax), %edx
	movl	-32(%ebp), %eax
	movl	176(%eax), %eax
	cmpl	%eax, %edx
	jg	L408
	movl	-32(%ebp), %edx
	movl	8(%ebp), %eax
	movl	%edx, 8(%esp)
	movl	%eax, 4(%esp)
	movl	$LC172, (%esp)
	call	__Z7d_printPKcz
	movl	$0, %eax
	jmp	L416
L408:
	movl	8(%ebp), %edx
	movl	-32(%ebp), %eax
	movl	%edx, 8(%esp)
	movl	%eax, 4(%esp)
	movl	$LC173, (%esp)
	call	__Z7d_printPKcz
L407:
	movl	8(%ebp), %eax
	movzbl	173(%eax), %eax
	testb	%al, %al
	je	L410
	movl	8(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	8(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC174, (%esp)
	call	__Z7d_printPKcz
	movl	8(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC175, (%esp)
	call	__Z7d_printPKcz
	movl	_my_trace, %ecx
	movl	-24(%ebp), %edx
	movl	%edx, %eax
	sall	$2, %eax
	addl	%edx, %eax
	sall	$2, %eax
	addl	%ecx, %eax
	leal	67109008(%eax), %edx
	movl	8(%ebp), %eax
	movl	%eax, 8(%edx)
L410:
	movl	$0, -12(%ebp)
	jmp	L411
L415:
	movl	8(%ebp), %eax
	movl	-12(%ebp), %edx
	addl	$24, %edx
	movl	8(%eax,%edx,4), %eax
	testl	%eax, %eax
	jne	L412
	movl	$LC176, (%esp)
	call	__Z7d_printPKcz
	movl	_my_trace, %eax
	movl	8(%ebp), %edx
	movl	%edx, 68964280(%eax)
	movl	_my_trace, %eax
	movl	$5, 67915420(%eax)
	jmp	L413
L412:
	movl	8(%ebp), %eax
	movl	-12(%ebp), %edx
	addl	$24, %edx
	movl	8(%eax,%edx,4), %eax
	cmpl	$767, %eax
	ja	L414
	movl	8(%ebp), %eax
	movl	-12(%ebp), %edx
	addl	$24, %edx
	movl	8(%eax,%edx,4), %eax
	cmpl	$256, %eax
	jbe	L414
	movl	_my_trace, %eax
	movl	68964020(%eax), %edx
	movl	68964016(%eax), %eax
	movl	8(%ebp), %ecx
	movl	-12(%ebp), %ebx
	addl	$24, %ebx
	movl	8(%ecx,%ebx,4), %ecx
	movl	%eax, 8(%esp)
	movl	%edx, 12(%esp)
	movl	%ecx, 4(%esp)
	movl	$LC177, (%esp)
	call	__Z7d_printPKcz
	movl	8(%ebp), %eax
	movl	-12(%ebp), %edx
	addl	$24, %edx
	movl	8(%eax,%edx,4), %ecx
	movl	_my_trace, %eax
	movl	68964272(%eax), %edx
	movl	_my_trace, %eax
	movl	68964276(%eax), %eax
	movl	%ecx, 16(%esp)
	movl	%edx, 12(%esp)
	movl	%eax, 8(%esp)
	movl	$LC178, 4(%esp)
	leal	-800(%ebp), %eax
	movl	%eax, (%esp)
	call	_sprintf
	leal	-800(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z13add_to_bufferPc
L414:
	movl	_my_trace, %eax
	movl	8(%ebp), %edx
	movl	-12(%ebp), %ecx
	addl	$24, %ecx
	movl	8(%edx,%ecx,4), %edx
	addl	$17505476, %edx
	movl	(%eax,%edx,4), %eax
	movl	12(%ebp), %edx
	movl	%edx, (%esp)
	call	*%eax
L413:
	addl	$1, -12(%ebp)
L411:
	movl	8(%ebp), %eax
	movl	168(%eax), %eax
	cmpl	-12(%ebp), %eax
	ja	L415
	movl	$0, %eax
L416:
	addl	$836, %esp
	popl	%ebx
	popl	%ebp
	ret
	.section .rdata,"dr"
LC179:
	.ascii "[handle_breakpoint]\12\0"
LC180:
	.ascii "ER4 TID: 0x%08x\12\0"
	.align 4
LC181:
	.ascii "ER5 TID1: 0x%08x instr_count: %d\12\0"
	.align 4
LC182:
	.ascii "ER3 TID: 0x%08x, thread_no=0x%08x, locking_reaction=%p\12\0"
LC183:
	.ascii "ER3 Reaction no %d: %p, %s\12\0"
	.align 4
LC184:
	.ascii "Reaction not enabled, continuing\12\0"
	.align 4
LC185:
	.ascii "ER3 Unlocking reaction lock with: %s in TID: 0x%08x\12\0"
LC186:
	.ascii "ER6 TID: 0x%08x\12\0"
LC187:
	.ascii "[handle_breakpoint ends]\12\0"
	.text
	.globl	__Z17handle_breakpointmPv
	.def	__Z17handle_breakpointmPv;	.scl	2;	.type	32;	.endef
__Z17handle_breakpointmPv:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%ebx
	subl	$68, %esp
	movl	$LC179, (%esp)
	call	__Z7d_printPKcz
	movl	12(%ebp), %eax
	movl	%eax, -12(%ebp)
	movl	-12(%ebp), %eax
	movl	8(%eax), %eax
	movl	%eax, 4(%esp)
	movl	$LC180, (%esp)
	call	__Z7d_printPKcz
	movl	$-1, -28(%ebp)
	movl	$0, -32(%ebp)
	movl	$0, -16(%ebp)
	jmp	L418
L429:
	movl	_my_trace, %edx
	movl	-16(%ebp), %eax
	imull	$532, %eax, %eax
	addl	%edx, %eax
	addl	$67643024, %eax
	movl	8(%eax), %eax
	cmpl	8(%ebp), %eax
	jne	L419
	movl	_my_trace, %eax
	movl	-16(%ebp), %edx
	imull	$532, %edx, %edx
	addl	$67642512, %edx
	addl	%edx, %eax
	addl	$4, %eax
	movl	%eax, -36(%ebp)
	movl	$0, -40(%ebp)
	movl	-12(%ebp), %eax
	movl	8(%eax), %eax
	movl	%eax, -44(%ebp)
	movl	_my_trace, %eax
	movl	-44(%ebp), %edx
	addl	$32, %edx
	movl	8(%eax,%edx,4), %eax
	movl	%eax, -48(%ebp)
	cmpl	$-1, -48(%ebp)
	jne	L420
	jmp	L419
L420:
	movl	_my_trace, %eax
	movl	68964020(%eax), %edx
	movl	68964016(%eax), %eax
	movl	-12(%ebp), %ecx
	movl	8(%ecx), %ecx
	movl	%eax, 8(%esp)
	movl	%edx, 12(%esp)
	movl	%ecx, 4(%esp)
	movl	$LC181, (%esp)
	call	__Z7d_printPKcz
	movl	$0, -52(%ebp)
	movl	_my_trace, %ecx
	movl	-48(%ebp), %edx
	movl	%edx, %eax
	sall	$2, %eax
	addl	%edx, %eax
	sall	$2, %eax
	addl	%ecx, %eax
	addl	$67109008, %eax
	movl	8(%eax), %eax
	movl	%eax, -52(%ebp)
	movl	-52(%ebp), %eax
	movl	%eax, 12(%esp)
	movl	-48(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	-44(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC182, (%esp)
	call	__Z7d_printPKcz
	movl	$0, -20(%ebp)
	jmp	L421
L428:
	movl	-36(%ebp), %eax
	movl	524(%eax), %eax
	movl	-20(%ebp), %edx
	sall	$2, %edx
	addl	%edx, %eax
	movl	(%eax), %eax
	movl	%eax, -40(%ebp)
	movl	-40(%ebp), %eax
	movl	%eax, 12(%esp)
	movl	-40(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	-20(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC183, (%esp)
	call	__Z7d_printPKcz
	movl	-40(%ebp), %eax
	movzbl	172(%eax), %eax
	testb	%al, %al
	jne	L422
	movl	$LC184, (%esp)
	call	__Z7d_printPKcz
	jmp	L423
L422:
	cmpl	$0, -52(%ebp)
	je	L424
	movl	$0, -56(%ebp)
	movl	$0, -24(%ebp)
	jmp	L425
L427:
	movl	-24(%ebp), %edx
	movl	%edx, %eax
	addl	%eax, %eax
	addl	%edx, %eax
	movl	-52(%ebp), %edx
	addl	%edx, %eax
	addl	$3, %eax
	movl	%eax, (%esp)
	call	__Z13find_reactionPc
	movl	%eax, -56(%ebp)
	movl	-56(%ebp), %eax
	cmpl	-40(%ebp), %eax
	jne	L426
	movl	-40(%ebp), %eax
	movl	-44(%ebp), %edx
	movl	%edx, 8(%esp)
	movl	%eax, 4(%esp)
	movl	$LC185, (%esp)
	call	__Z7d_printPKcz
	movl	_my_trace, %ecx
	movl	-48(%ebp), %edx
	movl	%edx, %eax
	sall	$2, %eax
	addl	%edx, %eax
	sall	$2, %eax
	addl	%ecx, %eax
	addl	$67109008, %eax
	movl	$0, 8(%eax)
L426:
	addl	$1, -24(%ebp)
L425:
	movl	-24(%ebp), %edx
	movl	-52(%ebp), %eax
	movl	100(%eax), %eax
	cmpl	%eax, %edx
	jb	L427
L424:
	movl	12(%ebp), %eax
	movl	%eax, -12(%ebp)
	movl	-12(%ebp), %eax
	movl	8(%eax), %eax
	movl	%eax, 4(%esp)
	movl	$LC186, (%esp)
	call	__Z7d_printPKcz
	movl	-40(%ebp), %eax
	movl	%eax, 12(%esp)
	movl	-40(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	-20(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC183, (%esp)
	call	__Z7d_printPKcz
	movl	12(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	-40(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z15handle_reactionP9REACTION_Pv
L423:
	addl	$1, -20(%ebp)
L421:
	movl	-20(%ebp), %edx
	movl	-36(%ebp), %eax
	movl	528(%eax), %eax
	cmpl	%eax, %edx
	jb	L428
L419:
	addl	$1, -16(%ebp)
L418:
	movl	-16(%ebp), %eax
	movl	%eax, %ecx
	movl	%eax, %ebx
	sarl	$31, %ebx
	movl	_my_trace, %eax
	movl	68964068(%eax), %edx
	movl	68964064(%eax), %eax
	cmpl	%edx, %ebx
	jb	L429
	cmpl	%edx, %ebx
	ja	L432
	cmpl	%eax, %ecx
	jb	L429
L432:
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z14del_breakpointm
	movl	-12(%ebp), %eax
	movl	8(%eax), %eax
	movl	%eax, (%esp)
	call	__Z7dec_eipm
	movl	$LC187, (%esp)
	call	__Z7d_printPKcz
	movl	$0, %eax
	addl	$68, %esp
	popl	%ebx
	popl	%ebp
	ret
	.section .rdata,"dr"
LC188:
	.ascii "[write_breakpoint]\12\0"
LC189:
	.ascii "[write_breakpoint ends]\12\0"
	.text
	.globl	__Z16write_breakpointP11BREAKPOINT_
	.def	__Z16write_breakpointP11BREAKPOINT_;	.scl	2;	.type	32;	.endef
__Z16write_breakpointP11BREAKPOINT_:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%ebx
	subl	$52, %esp
	movl	$LC188, (%esp)
	call	__Z7d_printPKcz
	movb	$-52, -17(%ebp)
	movl	8(%ebp), %eax
	movl	(%eax), %eax
	movl	%eax, (%esp)
	call	__Z16resolve_loc_descP24LOCATION_DESCRIPTOR_NEW_
	movl	%eax, -12(%ebp)
	movl	8(%ebp), %eax
	leal	523(%eax), %ebx
	movl	-12(%ebp), %edx
	movl	_my_trace, %eax
	movl	128(%eax), %eax
	leal	-16(%ebp), %ecx
	movl	%ecx, 16(%esp)
	movl	$1, 12(%esp)
	movl	%ebx, 8(%esp)
	movl	%edx, 4(%esp)
	movl	%eax, (%esp)
	call	__Z11read_memoryPvS_S_mPm
	movl	-12(%ebp), %edx
	movl	_my_trace, %eax
	movl	128(%eax), %eax
	leal	-16(%ebp), %ecx
	movl	%ecx, 16(%esp)
	movl	$1, 12(%esp)
	leal	-17(%ebp), %ecx
	movl	%ecx, 8(%esp)
	movl	%edx, 4(%esp)
	movl	%eax, (%esp)
	call	__Z12write_memoryPvS_S_mPm
	movl	-12(%ebp), %edx
	movl	_my_trace, %eax
	movl	128(%eax), %eax
	leal	-16(%ebp), %ecx
	movl	%ecx, 16(%esp)
	movl	$1, 12(%esp)
	leal	-17(%ebp), %ecx
	movl	%ecx, 8(%esp)
	movl	%edx, 4(%esp)
	movl	%eax, (%esp)
	call	__Z11read_memoryPvS_S_mPm
	movl	8(%ebp), %eax
	movb	$1, 522(%eax)
	movl	$LC189, (%esp)
	call	__Z7d_printPKcz
	movl	$0, %eax
	addl	$52, %esp
	popl	%ebx
	popl	%ebp
	ret
	.section .rdata,"dr"
LC190:
	.ascii "[unwrite_breakpoint]\12\0"
LC191:
	.ascii "[unwrite_breakpoint ends]\12\0"
	.text
	.globl	__Z18unwrite_breakpointP11BREAKPOINT_
	.def	__Z18unwrite_breakpointP11BREAKPOINT_;	.scl	2;	.type	32;	.endef
__Z18unwrite_breakpointP11BREAKPOINT_:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%ebx
	subl	$52, %esp
	movl	$LC190, (%esp)
	call	__Z7d_printPKcz
	movb	$-52, -13(%ebp)
	movl	8(%ebp), %eax
	movl	516(%eax), %eax
	movl	%eax, -12(%ebp)
	movl	8(%ebp), %eax
	leal	523(%eax), %ebx
	movl	-12(%ebp), %edx
	movl	_my_trace, %eax
	movl	128(%eax), %eax
	leal	-20(%ebp), %ecx
	movl	%ecx, 16(%esp)
	movl	$1, 12(%esp)
	movl	%ebx, 8(%esp)
	movl	%edx, 4(%esp)
	movl	%eax, (%esp)
	call	__Z12write_memoryPvS_S_mPm
	movl	-12(%ebp), %edx
	movl	_my_trace, %eax
	movl	128(%eax), %eax
	leal	-20(%ebp), %ecx
	movl	%ecx, 16(%esp)
	movl	$1, 12(%esp)
	leal	-13(%ebp), %ecx
	movl	%ecx, 8(%esp)
	movl	%edx, 4(%esp)
	movl	%eax, (%esp)
	call	__Z11read_memoryPvS_S_mPm
	movl	8(%ebp), %eax
	movb	$0, 522(%eax)
	movl	$LC191, (%esp)
	call	__Z7d_printPKcz
	movl	$0, %eax
	addl	$52, %esp
	popl	%ebx
	popl	%ebp
	ret
	.section .rdata,"dr"
LC192:
	.ascii "[print_loc_desc]\12\0"
LC193:
	.ascii "%s \0"
LC194:
	.ascii "[print_loc_desc ends]\12\0"
	.text
	.globl	__Z14print_loc_descP24LOCATION_DESCRIPTOR_NEW_
	.def	__Z14print_loc_descP24LOCATION_DESCRIPTOR_NEW_;	.scl	2;	.type	32;	.endef
__Z14print_loc_descP24LOCATION_DESCRIPTOR_NEW_:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$24, %esp
	movl	$LC192, (%esp)
	call	__Z7d_printPKcz
	cmpl	$0, 8(%ebp)
	jne	L438
	movl	$0, %eax
	jmp	L439
L438:
	movl	8(%ebp), %eax
	addl	$8, %eax
	movl	%eax, 4(%esp)
	movl	$LC193, (%esp)
	call	__Z7d_printPKcz
	movl	8(%ebp), %eax
	movl	(%eax), %eax
	movl	%eax, (%esp)
	call	__Z14print_loc_descP24LOCATION_DESCRIPTOR_NEW_
	movl	8(%ebp), %eax
	movl	4(%eax), %eax
	movl	%eax, (%esp)
	call	__Z14print_loc_descP24LOCATION_DESCRIPTOR_NEW_
	movl	$LC194, (%esp)
	call	__Z7d_printPKcz
	movl	$0, %eax
L439:
	leave
	ret
	.section .rdata,"dr"
LC195:
	.ascii "[print_loc_desc_rev]\12\0"
LC196:
	.ascii "%s\12\0"
LC197:
	.ascii "[print_loc_desc_rev ends]\12\0"
	.text
	.globl	__Z18print_loc_desc_revP24LOCATION_DESCRIPTOR_NEW_
	.def	__Z18print_loc_desc_revP24LOCATION_DESCRIPTOR_NEW_;	.scl	2;	.type	32;	.endef
__Z18print_loc_desc_revP24LOCATION_DESCRIPTOR_NEW_:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$24, %esp
	movl	$LC195, (%esp)
	call	__Z7d_printPKcz
	cmpl	$0, 8(%ebp)
	jne	L441
	movl	$0, %eax
	jmp	L442
L441:
	movl	8(%ebp), %eax
	movl	(%eax), %eax
	movl	%eax, (%esp)
	call	__Z18print_loc_desc_revP24LOCATION_DESCRIPTOR_NEW_
	movl	8(%ebp), %eax
	movl	4(%eax), %eax
	movl	%eax, (%esp)
	call	__Z18print_loc_desc_revP24LOCATION_DESCRIPTOR_NEW_
	movl	8(%ebp), %eax
	addl	$8, %eax
	movl	%eax, 4(%esp)
	movl	$LC196, (%esp)
	call	__Z7d_printPKcz
	movl	$LC197, (%esp)
	call	__Z7d_printPKcz
	movl	$0, %eax
L442:
	leave
	ret
	.globl	__Z7findanyPcS_
	.def	__Z7findanyPcS_;	.scl	2;	.type	32;	.endef
__Z7findanyPcS_:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$40, %esp
	movl	12(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_strcspn
	movl	%eax, %edx
	movl	8(%ebp), %eax
	addl	%edx, %eax
	movl	%eax, -12(%ebp)
	movl	-12(%ebp), %eax
	movzbl	(%eax), %eax
	testb	%al, %al
	je	L444
	movl	-12(%ebp), %eax
	jmp	L445
L444:
	movl	$0, %eax
L445:
	leave
	ret
	.globl	__Z5paintPcj
	.def	__Z5paintPcj;	.scl	2;	.type	32;	.endef
__Z5paintPcj:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$40, %esp
	movl	$0, -16(%ebp)
	movb	$0, -17(%ebp)
	movl	$0, -12(%ebp)
	jmp	L448
L457:
	movl	8(%ebp), %edx
	movl	-12(%ebp), %eax
	addl	%edx, %eax
	movzbl	(%eax), %eax
	cmpb	$91, %al
	jne	L449
	addl	$1, -16(%ebp)
	cmpl	$0, -16(%ebp)
	je	L450
	movb	$1, -17(%ebp)
	jmp	L451
L450:
	movb	$0, -17(%ebp)
L451:
	cmpb	$0, -17(%ebp)
	je	L452
	movl	8(%ebp), %edx
	movl	-12(%ebp), %eax
	addl	%edx, %eax
	movl	8(%ebp), %ecx
	movl	-12(%ebp), %edx
	addl	%ecx, %edx
	movzbl	(%edx), %edx
	addl	$3, %edx
	movb	%dl, (%eax)
	jmp	L452
L449:
	movl	8(%ebp), %edx
	movl	-12(%ebp), %eax
	addl	%edx, %eax
	movzbl	(%eax), %eax
	cmpb	$93, %al
	jne	L453
	cmpb	$0, -17(%ebp)
	je	L454
	movl	8(%ebp), %edx
	movl	-12(%ebp), %eax
	addl	%edx, %eax
	movl	8(%ebp), %ecx
	movl	-12(%ebp), %edx
	addl	%ecx, %edx
	movzbl	(%edx), %edx
	addl	$3, %edx
	movb	%dl, (%eax)
L454:
	subl	$1, -16(%ebp)
	cmpl	$0, -16(%ebp)
	je	L455
	movb	$1, -17(%ebp)
	jmp	L452
L455:
	movb	$0, -17(%ebp)
	jmp	L452
L453:
	cmpb	$0, -17(%ebp)
	je	L452
	movl	8(%ebp), %edx
	movl	-12(%ebp), %eax
	addl	%edx, %eax
	movl	8(%ebp), %ecx
	movl	-12(%ebp), %edx
	addl	%ecx, %edx
	movzbl	(%edx), %edx
	addl	$3, %edx
	movb	%dl, (%eax)
L452:
	movl	8(%ebp), %edx
	movl	-12(%ebp), %eax
	addl	%edx, %eax
	movzbl	(%eax), %eax
	movsbl	%al, %eax
	movl	%eax, (%esp)
	call	_putchar
	addl	$1, -12(%ebp)
L448:
	movl	-12(%ebp), %eax
	cmpl	12(%ebp), %eax
	jb	L457
	movl	$10, (%esp)
	call	_putchar
	movl	$0, %eax
	leave
	ret
	.globl	__Z7unpaintPcj
	.def	__Z7unpaintPcj;	.scl	2;	.type	32;	.endef
__Z7unpaintPcj:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$40, %esp
	movl	$0, -16(%ebp)
	movb	$0, -17(%ebp)
	movl	$0, -12(%ebp)
	jmp	L460
L469:
	movl	8(%ebp), %edx
	movl	-12(%ebp), %eax
	addl	%edx, %eax
	movzbl	(%eax), %eax
	cmpb	$94, %al
	jne	L461
	addl	$1, -16(%ebp)
	cmpl	$0, -16(%ebp)
	je	L462
	movb	$1, -17(%ebp)
	jmp	L463
L462:
	movb	$0, -17(%ebp)
L463:
	cmpb	$0, -17(%ebp)
	je	L464
	movl	8(%ebp), %edx
	movl	-12(%ebp), %eax
	addl	%edx, %eax
	movl	8(%ebp), %ecx
	movl	-12(%ebp), %edx
	addl	%ecx, %edx
	movzbl	(%edx), %edx
	subl	$3, %edx
	movb	%dl, (%eax)
	jmp	L464
L461:
	movl	8(%ebp), %edx
	movl	-12(%ebp), %eax
	addl	%edx, %eax
	movzbl	(%eax), %eax
	cmpb	$96, %al
	jne	L465
	cmpb	$0, -17(%ebp)
	je	L466
	movl	8(%ebp), %edx
	movl	-12(%ebp), %eax
	addl	%edx, %eax
	movl	8(%ebp), %ecx
	movl	-12(%ebp), %edx
	addl	%ecx, %edx
	movzbl	(%edx), %edx
	subl	$3, %edx
	movb	%dl, (%eax)
L466:
	subl	$1, -16(%ebp)
	cmpl	$0, -16(%ebp)
	je	L467
	movb	$1, -17(%ebp)
	jmp	L464
L467:
	movb	$0, -17(%ebp)
	jmp	L464
L465:
	cmpb	$0, -17(%ebp)
	je	L464
	movl	8(%ebp), %edx
	movl	-12(%ebp), %eax
	addl	%edx, %eax
	movl	8(%ebp), %ecx
	movl	-12(%ebp), %edx
	addl	%ecx, %edx
	movzbl	(%edx), %edx
	subl	$3, %edx
	movb	%dl, (%eax)
L464:
	movl	8(%ebp), %edx
	movl	-12(%ebp), %eax
	addl	%edx, %eax
	movzbl	(%eax), %eax
	movsbl	%al, %eax
	movl	%eax, (%esp)
	call	_putchar
	addl	$1, -12(%ebp)
L460:
	movl	-12(%ebp), %eax
	cmpl	12(%ebp), %eax
	jb	L469
	movl	$10, (%esp)
	call	_putchar
	movl	$0, %eax
	leave
	ret
	.section .rdata,"dr"
LC198:
	.ascii "[+-\0"
LC199:
	.ascii "[\0"
LC200:
	.ascii "-\0"
LC201:
	.ascii "+\0"
LC202:
	.ascii "Reading register ESP: 0x%08x\12\0"
LC203:
	.ascii "Reading register EAX: 0x%08x\12\0"
LC204:
	.ascii "Reading register EBX: 0x%08x\12\0"
LC205:
	.ascii "Reading register ECX: 0x%08x\12\0"
LC206:
	.ascii "Reading register EDX: 0x%08x\12\0"
LC207:
	.ascii "Reading register ESI: 0x%08x\12\0"
LC208:
	.ascii "Reading register EDI: 0x%08x\12\0"
LC209:
	.ascii "EBP\0"
LC210:
	.ascii "Reading register EBP: 0x%08x\12\0"
LC211:
	.ascii "Reading register EIP: 0x%08x\12\0"
	.text
	.globl	__Z16resolve_loc_descP24LOCATION_DESCRIPTOR_NEW_
	.def	__Z16resolve_loc_descP24LOCATION_DESCRIPTOR_NEW_;	.scl	2;	.type	32;	.endef
__Z16resolve_loc_descP24LOCATION_DESCRIPTOR_NEW_:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$760, %esp
	cmpl	$0, 8(%ebp)
	jne	L472
	movl	$-1, %eax
	jmp	L473
L472:
	movl	8(%ebp), %eax
	addl	$8, %eax
	movl	$LC198, 4(%esp)
	movl	%eax, (%esp)
	call	__Z7findanyPcS_
	testl	%eax, %eax
	setne	%al
	testb	%al, %al
	je	L474
	movl	8(%ebp), %eax
	addl	$8, %eax
	movl	$LC199, 4(%esp)
	movl	%eax, (%esp)
	call	_strcmp
	testl	%eax, %eax
	jne	L475
	movl	8(%ebp), %eax
	movl	(%eax), %eax
	movl	%eax, (%esp)
	call	__Z16resolve_loc_descP24LOCATION_DESCRIPTOR_NEW_
	movl	%eax, -16(%ebp)
	cmpl	$-1, -16(%ebp)
	jne	L476
	movl	$-1, %eax
	jmp	L473
L476:
	movl	-16(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z10read_dwordm
	movl	%eax, -20(%ebp)
	movl	-20(%ebp), %eax
	jmp	L473
L475:
	movl	8(%ebp), %eax
	addl	$8, %eax
	movl	$LC200, 4(%esp)
	movl	%eax, (%esp)
	call	_strcmp
	testl	%eax, %eax
	jne	L477
	movl	8(%ebp), %eax
	movl	(%eax), %eax
	movl	%eax, (%esp)
	call	__Z16resolve_loc_descP24LOCATION_DESCRIPTOR_NEW_
	movl	%eax, -16(%ebp)
	movl	8(%ebp), %eax
	movl	4(%eax), %eax
	movl	%eax, (%esp)
	call	__Z16resolve_loc_descP24LOCATION_DESCRIPTOR_NEW_
	movl	%eax, -24(%ebp)
	cmpl	$-1, -16(%ebp)
	jne	L478
	movl	$-1, %eax
	jmp	L473
L478:
	cmpl	$-1, -24(%ebp)
	jne	L479
	movl	$-1, %eax
	jmp	L473
L479:
	movl	-16(%ebp), %eax
	subl	-24(%ebp), %eax
	movl	%eax, -12(%ebp)
	jmp	L480
L477:
	movl	8(%ebp), %eax
	addl	$8, %eax
	movl	$LC201, 4(%esp)
	movl	%eax, (%esp)
	call	_strcmp
	testl	%eax, %eax
	jne	L480
	movl	8(%ebp), %eax
	movl	(%eax), %eax
	movl	%eax, (%esp)
	call	__Z16resolve_loc_descP24LOCATION_DESCRIPTOR_NEW_
	movl	%eax, -16(%ebp)
	movl	8(%ebp), %eax
	movl	4(%eax), %eax
	movl	%eax, (%esp)
	call	__Z16resolve_loc_descP24LOCATION_DESCRIPTOR_NEW_
	movl	%eax, -24(%ebp)
	cmpl	$-1, -16(%ebp)
	jne	L481
	movl	$-1, %eax
	jmp	L473
L481:
	cmpl	$-1, -24(%ebp)
	jne	L482
	movl	$-1, %eax
	jmp	L473
L482:
	movl	-16(%ebp), %edx
	movl	-24(%ebp), %eax
	addl	%edx, %eax
	movl	%eax, -12(%ebp)
L480:
	jmp	L483
L474:
	movl	8(%ebp), %eax
	addl	$8, %eax
	movl	$LC117, 4(%esp)
	movl	%eax, (%esp)
	call	_strcmp
	testl	%eax, %eax
	jne	L484
	movl	_my_trace, %eax
	movl	96(%eax), %eax
	movl	%eax, -12(%ebp)
	cmpl	$0, -12(%ebp)
	jne	L483
	movl	$-1, %eax
	jmp	L473
L484:
	movl	8(%ebp), %eax
	movzbl	8(%eax), %eax
	cmpb	$48, %al
	jne	L485
	movl	8(%ebp), %eax
	movzbl	9(%eax), %eax
	cmpb	$120, %al
	jne	L485
	movl	8(%ebp), %eax
	addl	$8, %eax
	movl	$16, 8(%esp)
	movl	$0, 4(%esp)
	movl	%eax, (%esp)
	call	_strtoul
	movl	%eax, -12(%ebp)
	jmp	L483
L485:
	movl	8(%ebp), %eax
	addl	$8, %eax
	movl	$LC34, 4(%esp)
	movl	%eax, (%esp)
	call	_strcmp
	testl	%eax, %eax
	jne	L486
	leal	-740(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$0, (%esp)
	call	__Z12read_contextmP8_CONTEXT
	movl	-544(%ebp), %eax
	movl	%eax, -12(%ebp)
	movl	-12(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC202, (%esp)
	call	__Z7d_printPKcz
	jmp	L483
L486:
	movl	8(%ebp), %eax
	addl	$8, %eax
	movl	$LC28, 4(%esp)
	movl	%eax, (%esp)
	call	_strcmp
	testl	%eax, %eax
	jne	L487
	leal	-740(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$0, (%esp)
	call	__Z12read_contextmP8_CONTEXT
	movl	-564(%ebp), %eax
	movl	%eax, -12(%ebp)
	movl	-12(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC203, (%esp)
	call	__Z7d_printPKcz
	jmp	L483
L487:
	movl	8(%ebp), %eax
	addl	$8, %eax
	movl	$LC29, 4(%esp)
	movl	%eax, (%esp)
	call	_strcmp
	testl	%eax, %eax
	jne	L488
	leal	-740(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$0, (%esp)
	call	__Z12read_contextmP8_CONTEXT
	movl	-576(%ebp), %eax
	movl	%eax, -12(%ebp)
	movl	-12(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC204, (%esp)
	call	__Z7d_printPKcz
	jmp	L483
L488:
	movl	8(%ebp), %eax
	addl	$8, %eax
	movl	$LC30, 4(%esp)
	movl	%eax, (%esp)
	call	_strcmp
	testl	%eax, %eax
	jne	L489
	leal	-740(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$0, (%esp)
	call	__Z12read_contextmP8_CONTEXT
	movl	-568(%ebp), %eax
	movl	%eax, -12(%ebp)
	movl	-12(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC205, (%esp)
	call	__Z7d_printPKcz
	jmp	L483
L489:
	movl	8(%ebp), %eax
	addl	$8, %eax
	movl	$LC31, 4(%esp)
	movl	%eax, (%esp)
	call	_strcmp
	testl	%eax, %eax
	jne	L490
	leal	-740(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$0, (%esp)
	call	__Z12read_contextmP8_CONTEXT
	movl	-572(%ebp), %eax
	movl	%eax, -12(%ebp)
	movl	-12(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC206, (%esp)
	call	__Z7d_printPKcz
	jmp	L483
L490:
	movl	8(%ebp), %eax
	addl	$8, %eax
	movl	$LC33, 4(%esp)
	movl	%eax, (%esp)
	call	_strcmp
	testl	%eax, %eax
	jne	L491
	leal	-740(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$0, (%esp)
	call	__Z12read_contextmP8_CONTEXT
	movl	-580(%ebp), %eax
	movl	%eax, -12(%ebp)
	movl	-12(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC207, (%esp)
	call	__Z7d_printPKcz
	jmp	L483
L491:
	movl	8(%ebp), %eax
	addl	$8, %eax
	movl	$LC32, 4(%esp)
	movl	%eax, (%esp)
	call	_strcmp
	testl	%eax, %eax
	jne	L492
	leal	-740(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$0, (%esp)
	call	__Z12read_contextmP8_CONTEXT
	movl	-584(%ebp), %eax
	movl	%eax, -12(%ebp)
	movl	-12(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC208, (%esp)
	call	__Z7d_printPKcz
	jmp	L483
L492:
	movl	8(%ebp), %eax
	addl	$8, %eax
	movl	$LC209, 4(%esp)
	movl	%eax, (%esp)
	call	_strcmp
	testl	%eax, %eax
	jne	L493
	leal	-740(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$0, (%esp)
	call	__Z12read_contextmP8_CONTEXT
	movl	-560(%ebp), %eax
	movl	%eax, -12(%ebp)
	movl	-12(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC210, (%esp)
	call	__Z7d_printPKcz
	jmp	L483
L493:
	movl	8(%ebp), %eax
	addl	$8, %eax
	movl	$LC35, 4(%esp)
	movl	%eax, (%esp)
	call	_strcmp
	testl	%eax, %eax
	jne	L494
	leal	-740(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$0, (%esp)
	call	__Z12read_contextmP8_CONTEXT
	movl	-556(%ebp), %eax
	movl	%eax, -12(%ebp)
	movl	-12(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC211, (%esp)
	call	__Z7d_printPKcz
	jmp	L483
L494:
	movl	8(%ebp), %eax
	addl	$8, %eax
	movl	%eax, (%esp)
	call	__Z8find_libPc
	movl	%eax, -12(%ebp)
	cmpl	$0, -12(%ebp)
	jne	L483
	cmpl	$0, -12(%ebp)
	jne	L483
	movl	$-1, -12(%ebp)
L483:
	movl	-12(%ebp), %eax
L473:
	leave
	ret
	.section .rdata,"dr"
LC212:
	.ascii "Out of memory\12\0"
LC213:
	.ascii "Creating new descriptor node\12\0"
LC214:
	.ascii "+-\0"
	.align 4
LC215:
	.ascii "No operators found, assuming leaf: %s\12\0"
LC216:
	.ascii "Found operator: %s\12\0"
	.text
	.globl	__Z19parse_location_descPc
	.def	__Z19parse_location_descPc;	.scl	2;	.type	32;	.endef
__Z19parse_location_descPc:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$40, %esp
	movl	$520, (%esp)
	call	_malloc
	movl	%eax, -12(%ebp)
	movl	$520, 8(%esp)
	movl	$0, 4(%esp)
	movl	-12(%ebp), %eax
	movl	%eax, (%esp)
	call	_memset
	movl	-12(%ebp), %eax
	movl	$0, (%eax)
	movl	-12(%ebp), %eax
	movl	$0, 4(%eax)
	cmpl	$0, -12(%ebp)
	jne	L496
	movl	$LC212, (%esp)
	call	__Z7d_printPKcz
L496:
	movl	$LC213, (%esp)
	call	__Z7d_printPKcz
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_strlen
	movl	%eax, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z5paintPcj
	movl	$LC214, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z7findanyPcS_
	movl	%eax, -16(%ebp)
	cmpl	$0, -16(%ebp)
	jne	L497
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_strlen
	movl	%eax, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z7unpaintPcj
	movl	$LC199, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z7findanyPcS_
	movl	%eax, -16(%ebp)
	cmpl	$0, -16(%ebp)
	setne	%al
	testb	%al, %al
	je	L498
	movl	-12(%ebp), %eax
	addl	$8, %eax
	movw	$91, (%eax)
	movl	-16(%ebp), %eax
	movb	$0, (%eax)
	movl	-16(%ebp), %eax
	addl	$1, %eax
	movl	%eax, (%esp)
	call	__Z19parse_location_descPc
	movl	%eax, %edx
	movl	-12(%ebp), %eax
	movl	%edx, (%eax)
	movl	-12(%ebp), %eax
	movl	$0, 4(%eax)
	jmp	L500
L498:
	movl	8(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC215, (%esp)
	call	__Z7d_printPKcz
	movl	-12(%ebp), %eax
	leal	8(%eax), %edx
	movl	8(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	%edx, (%esp)
	call	_strcpy
	jmp	L500
L497:
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_strlen
	movl	%eax, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z7unpaintPcj
	movl	-16(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC216, (%esp)
	call	__Z7d_printPKcz
	movl	-16(%ebp), %eax
	movzbl	(%eax), %eax
	cmpb	$43, %al
	jne	L501
	movl	-12(%ebp), %eax
	addl	$8, %eax
	movw	$43, (%eax)
	movl	-16(%ebp), %eax
	movb	$0, (%eax)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z19parse_location_descPc
	movl	%eax, %edx
	movl	-12(%ebp), %eax
	movl	%edx, (%eax)
	movl	-16(%ebp), %eax
	addl	$1, %eax
	movl	%eax, (%esp)
	call	__Z19parse_location_descPc
	movl	%eax, %edx
	movl	-12(%ebp), %eax
	movl	%edx, 4(%eax)
	jmp	L500
L501:
	movl	-16(%ebp), %eax
	movzbl	(%eax), %eax
	cmpb	$45, %al
	jne	L500
	movl	-12(%ebp), %eax
	addl	$8, %eax
	movw	$45, (%eax)
	movl	-16(%ebp), %eax
	movb	$0, (%eax)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z19parse_location_descPc
	movl	%eax, %edx
	movl	-12(%ebp), %eax
	movl	%edx, (%eax)
	movl	-16(%ebp), %eax
	addl	$1, %eax
	movl	%eax, (%esp)
	call	__Z19parse_location_descPc
	movl	%eax, %edx
	movl	-12(%ebp), %eax
	movl	%edx, 4(%eax)
L500:
	movl	-12(%ebp), %eax
	leave
	ret
	.section .rdata,"dr"
LC217:
	.ascii "Trying to resolve BP addr\12\0"
	.align 4
LC218:
	.ascii "Unable to resolve BP address, will not be updated at this time\12\0"
LC219:
	.ascii "BP written @ 0x%08x\12\0"
LC220:
	.ascii "[update_breakpoint ends]\12\0"
	.text
	.globl	__Z17update_breakpointP11BREAKPOINT_
	.def	__Z17update_breakpointP11BREAKPOINT_;	.scl	2;	.type	32;	.endef
__Z17update_breakpointP11BREAKPOINT_:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$40, %esp
	movl	$LC217, (%esp)
	call	__Z7d_printPKcz
	movl	8(%ebp), %eax
	movl	(%eax), %eax
	movl	%eax, (%esp)
	call	__Z16resolve_loc_descP24LOCATION_DESCRIPTOR_NEW_
	movl	%eax, -16(%ebp)
	movl	8(%ebp), %eax
	movl	-16(%ebp), %edx
	movl	%edx, 516(%eax)
	cmpl	$-1, -16(%ebp)
	jne	L504
	movl	$LC218, (%esp)
	call	__Z7d_printPKcz
	movl	$0, %eax
	jmp	L505
L504:
	movl	8(%ebp), %eax
	movzbl	521(%eax), %eax
	testb	%al, %al
	je	L506
	movl	8(%ebp), %eax
	movzbl	522(%eax), %eax
	testb	%al, %al
	jne	L507
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z16write_breakpointP11BREAKPOINT_
	movl	8(%ebp), %eax
	movb	$1, 522(%eax)
L507:
	movl	-16(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC219, (%esp)
	call	__Z7d_printPKcz
	movl	$1, -12(%ebp)
	jmp	L508
L506:
	movl	8(%ebp), %eax
	movzbl	522(%eax), %eax
	testb	%al, %al
	je	L509
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z18unwrite_breakpointP11BREAKPOINT_
	movl	8(%ebp), %eax
	movb	$0, 522(%eax)
L509:
	movl	$0, -12(%ebp)
L508:
	movl	$LC220, (%esp)
	call	__Z7d_printPKcz
	movl	-12(%ebp), %eax
L505:
	leave
	ret
	.section .rdata,"dr"
LC221:
	.ascii "[add_breakpoint]\12\0"
LC222:
	.ascii "Found!\12\0"
LC223:
	.ascii "Creating new bp\12\0"
	.align 4
LC224:
	.ascii "Failed creating new bp- out of memory\12\0"
LC225:
	.ascii "Created new bp\12\0"
	.align 4
LC226:
	.ascii "Attempt to parse location string: %s\12\0"
	.align 4
LC227:
	.ascii "Bp created. Current number of breakpoints: %d\12\0"
	.align 4
LC228:
	.ascii "Current reaction count for this BP: %d\12\0"
LC229:
	.ascii "Assigned new reaction\12\0"
LC230:
	.ascii "[add_breakpoint ends]\12\0"
	.text
	.globl	__Z14add_breakpointPcP9REACTION_
	.def	__Z14add_breakpointPcP9REACTION_;	.scl	2;	.type	32;	.endef
__Z14add_breakpointPcP9REACTION_:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%ebx
	subl	$36, %esp
	movl	$LC221, (%esp)
	call	__Z7d_printPKcz
	movl	$-1, -16(%ebp)
	movl	$0, -12(%ebp)
	jmp	L511
L514:
	movl	_my_trace, %eax
	movl	-12(%ebp), %edx
	imull	$532, %edx, %edx
	addl	$67642512, %edx
	addl	%edx, %eax
	leal	8(%eax), %edx
	movl	8(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	%edx, (%esp)
	call	_strcmp
	testl	%eax, %eax
	jne	L512
	movl	$LC222, (%esp)
	call	__Z7d_printPKcz
	movl	-12(%ebp), %eax
	movl	%eax, -16(%ebp)
	jmp	L513
L512:
	addl	$1, -12(%ebp)
L511:
	movl	-12(%ebp), %eax
	movl	%eax, %ecx
	movl	%eax, %ebx
	sarl	$31, %ebx
	movl	_my_trace, %eax
	movl	68964068(%eax), %edx
	movl	68964064(%eax), %eax
	cmpl	%edx, %ebx
	jb	L514
	cmpl	%edx, %ebx
	ja	L513
	cmpl	%eax, %ecx
	jb	L514
L513:
	cmpl	$-1, -16(%ebp)
	jne	L516
	movl	_my_trace, %eax
	movl	68964068(%eax), %edx
	movl	68964064(%eax), %eax
	movl	%eax, -16(%ebp)
	movl	$LC223, (%esp)
	call	__Z7d_printPKcz
	movl	_my_trace, %ebx
	movl	$16384, %eax
	movl	%eax, (%esp)
	call	_malloc
	movl	%eax, %edx
	movl	-16(%ebp), %eax
	imull	$532, %eax, %eax
	addl	%ebx, %eax
	addl	$67643024, %eax
	movl	%edx, 16(%eax)
	movl	_my_trace, %edx
	movl	-16(%ebp), %eax
	imull	$532, %eax, %eax
	addl	%edx, %eax
	addl	$67643024, %eax
	movl	16(%eax), %eax
	testl	%eax, %eax
	jne	L517
	movl	$LC224, (%esp)
	call	__Z7d_printPKcz
L517:
	movl	_my_trace, %edx
	movl	-16(%ebp), %eax
	imull	$532, %eax, %eax
	addl	%edx, %eax
	addl	$67643024, %eax
	movb	$1, 13(%eax)
	movl	_my_trace, %edx
	movl	-16(%ebp), %eax
	imull	$532, %eax, %eax
	addl	%edx, %eax
	addl	$67643024, %eax
	movb	$0, 14(%eax)
	movl	$LC225, (%esp)
	call	__Z7d_printPKcz
	movl	_my_trace, %eax
	movl	-16(%ebp), %edx
	imull	$532, %edx, %edx
	addl	$67642512, %edx
	addl	%edx, %eax
	leal	8(%eax), %edx
	movl	8(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	%edx, (%esp)
	call	_strcpy
	movl	8(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC226, (%esp)
	call	__Z7d_printPKcz
	movl	_my_trace, %ebx
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z19parse_location_descPc
	movl	%eax, %edx
	movl	-16(%ebp), %eax
	imull	$532, %eax, %eax
	addl	%ebx, %eax
	addl	$67642512, %eax
	movl	%edx, 4(%eax)
	movl	_my_trace, %ecx
	movl	68964064(%ecx), %eax
	movl	68964068(%ecx), %edx
	addl	$1, %eax
	adcl	$0, %edx
	movl	%eax, 68964064(%ecx)
	movl	%edx, 68964068(%ecx)
L516:
	movl	_my_trace, %eax
	movl	68964068(%eax), %edx
	movl	68964064(%eax), %eax
	movl	%eax, 4(%esp)
	movl	%edx, 8(%esp)
	movl	$LC227, (%esp)
	call	__Z7d_printPKcz
	movl	_my_trace, %edx
	movl	-16(%ebp), %eax
	imull	$532, %eax, %eax
	addl	%edx, %eax
	addl	$67643040, %eax
	movl	4(%eax), %eax
	movl	%eax, -20(%ebp)
	movl	-20(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC228, (%esp)
	call	__Z7d_printPKcz
	movl	_my_trace, %edx
	movl	-16(%ebp), %eax
	imull	$532, %eax, %eax
	addl	%edx, %eax
	addl	$67643024, %eax
	movl	16(%eax), %eax
	movl	-20(%ebp), %edx
	sall	$2, %edx
	addl	%eax, %edx
	movl	12(%ebp), %eax
	movl	%eax, (%edx)
	movl	$LC229, (%esp)
	call	__Z7d_printPKcz
	movl	_my_trace, %edx
	movl	-16(%ebp), %eax
	imull	$532, %eax, %eax
	addl	%edx, %eax
	addl	$67643024, %eax
	movb	$1, 13(%eax)
	movl	_my_trace, %eax
	movl	-16(%ebp), %edx
	imull	$532, %edx, %edx
	addl	%eax, %edx
	addl	$67643040, %edx
	movl	4(%edx), %edx
	addl	$1, %edx
	movl	-16(%ebp), %ecx
	imull	$532, %ecx, %ecx
	addl	%ecx, %eax
	addl	$67643040, %eax
	movl	%edx, 4(%eax)
	movl	$LC230, (%esp)
	call	__Z7d_printPKcz
	movl	_my_trace, %eax
	movl	-16(%ebp), %edx
	imull	$532, %edx, %edx
	addl	$67642512, %edx
	addl	%edx, %eax
	addl	$4, %eax
	addl	$36, %esp
	popl	%ebx
	popl	%ebp
	ret
	.section .rdata,"dr"
LC231:
	.ascii "Creating process: %s\12\0"
	.align 4
LC232:
	.ascii "Error creating process: 0x%08x\12\0"
	.text
	.globl	__Z17start_trace_fnamev
	.def	__Z17start_trace_fnamev;	.scl	2;	.type	32;	.endef
__Z17start_trace_fnamev:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$72, %esp
	movl	_my_trace, %eax
	addl	$70021356, %eax
	movl	%eax, 4(%esp)
	movl	$LC231, (%esp)
	call	__Z7d_printPKcz
	movl	_my_trace, %eax
	movl	%eax, %ecx
	movl	_my_trace, %eax
	leal	16(%eax), %edx
	movl	_my_trace, %eax
	addl	$70021356, %eax
	movl	%ecx, 36(%esp)
	movl	%edx, 32(%esp)
	movl	$0, 28(%esp)
	movl	$0, 24(%esp)
	movl	$2, 20(%esp)
	movl	$0, 16(%esp)
	movl	$0, 12(%esp)
	movl	$0, 8(%esp)
	movl	$0, 4(%esp)
	movl	%eax, (%esp)
	movl	__imp__CreateProcessA@40, %eax
	call	*%eax
	subl	$40, %esp
	movl	%eax, -12(%ebp)
	cmpl	$0, -12(%ebp)
	jne	L520
	movl	__imp__GetLastError@0, %eax
	call	*%eax
	movl	%eax, 4(%esp)
	movl	$LC232, (%esp)
	call	__Z7d_printPKcz
L520:
	movl	_my_trace, %eax
	movl	_my_trace, %edx
	movl	8(%edx), %edx
	movl	%edx, 124(%eax)
	leave
	ret
	.section .rdata,"dr"
LC233:
	.ascii "Attaching debugger\12\0"
	.align 4
LC234:
	.ascii "Successfully attached to PID: 0x%x, handle: 0x%x\12\0"
LC235:
	.ascii "Attach failed\12\0"
	.text
	.globl	__Z15start_trace_pidv
	.def	__Z15start_trace_pidv;	.scl	2;	.type	32;	.endef
__Z15start_trace_pidv:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$24, %esp
	movl	$LC233, (%esp)
	call	__Z7d_printPKcz
	movl	_my_trace, %eax
	movl	70015972(%eax), %eax
	movl	%eax, (%esp)
	movl	__imp__DebugActiveProcess@4, %eax
	call	*%eax
	subl	$4, %esp
	testl	%eax, %eax
	setne	%al
	testb	%al, %al
	je	L522
	movl	_my_trace, %eax
	movl	70015972(%eax), %eax
	movl	%eax, 4(%esp)
	movl	$LC234, (%esp)
	call	__Z7d_printPKcz
	jmp	L521
L522:
	movl	$LC235, (%esp)
	call	__Z7d_printPKcz
L521:
	leave
	ret
	.globl	__Z8find_libPc
	.def	__Z8find_libPc;	.scl	2;	.type	32;	.endef
__Z8find_libPc:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%ebx
	subl	$1060, %esp
	movl	$0, -20(%ebp)
	movl	8(%ebp), %eax
	movl	%eax, 4(%esp)
	leal	-1044(%ebp), %eax
	movl	%eax, (%esp)
	call	_strcpy
	movl	$0, -16(%ebp)
	jmp	L525
L526:
	leal	-1044(%ebp), %edx
	movl	-16(%ebp), %eax
	addl	%edx, %eax
	movzbl	(%eax), %eax
	movsbl	%al, %eax
	movl	%eax, (%esp)
	movl	__imp__tolower, %eax
	call	*%eax
	movl	%eax, %ecx
	leal	-1044(%ebp), %edx
	movl	-16(%ebp), %eax
	addl	%edx, %eax
	movb	%cl, (%eax)
	addl	$1, -16(%ebp)
L525:
	leal	-1044(%ebp), %edx
	movl	-16(%ebp), %eax
	addl	%edx, %eax
	movzbl	(%eax), %eax
	testb	%al, %al
	jne	L526
	movl	$0, -12(%ebp)
	jmp	L527
L532:
	movl	_my_trace, %edx
	movl	-12(%ebp), %eax
	sall	$3, %eax
	movl	%eax, %ecx
	sall	$7, %ecx
	addl	%ecx, %eax
	addl	$67114112, %eax
	addl	%edx, %eax
	addl	$16, %eax
	movl	%eax, 4(%esp)
	leal	-532(%ebp), %eax
	movl	%eax, (%esp)
	call	_strcpy
	movl	$0, -16(%ebp)
	jmp	L528
L529:
	leal	-532(%ebp), %edx
	movl	-16(%ebp), %eax
	addl	%edx, %eax
	movzbl	(%eax), %eax
	movsbl	%al, %eax
	movl	%eax, (%esp)
	movl	__imp__tolower, %eax
	call	*%eax
	movl	%eax, %ecx
	leal	-532(%ebp), %edx
	movl	-16(%ebp), %eax
	addl	%edx, %eax
	movb	%cl, (%eax)
	addl	$1, -16(%ebp)
L528:
	leal	-532(%ebp), %edx
	movl	-16(%ebp), %eax
	addl	%edx, %eax
	movzbl	(%eax), %eax
	testb	%al, %al
	jne	L529
	leal	-1044(%ebp), %eax
	movl	%eax, 4(%esp)
	leal	-532(%ebp), %eax
	movl	%eax, (%esp)
	call	_strstr
	testl	%eax, %eax
	je	L530
	movl	_my_trace, %edx
	movl	-12(%ebp), %eax
	sall	$3, %eax
	movl	%eax, %ecx
	sall	$7, %ecx
	addl	%ecx, %eax
	addl	%edx, %eax
	addl	$67114112, %eax
	movl	12(%eax), %eax
	movl	%eax, -20(%ebp)
	jmp	L531
L530:
	addl	$1, -12(%ebp)
L527:
	movl	-12(%ebp), %ecx
	movl	$0, %ebx
	movl	_my_trace, %eax
	movl	68964060(%eax), %edx
	movl	68964056(%eax), %eax
	cmpl	%edx, %ebx
	jb	L532
	cmpl	%edx, %ebx
	ja	L531
	cmpl	%eax, %ecx
	jb	L532
L531:
	movl	-20(%ebp), %eax
	addl	$1060, %esp
	popl	%ebx
	popl	%ebp
	ret
	.section .rdata,"dr"
LC236:
	.ascii "rb\0"
LC237:
	.ascii "Line: %s\12\0"
LC238:
	.ascii "# desc: %s\12\0"
	.text
	.globl	__Z16parse_descriptorPc
	.def	__Z16parse_descriptorPc;	.scl	2;	.type	32;	.endef
__Z16parse_descriptorPc:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$808, %esp
	movl	$LC236, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_fopen
	movl	%eax, -12(%ebp)
	jmp	L536
L537:
	leal	-780(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC237, (%esp)
	call	__Z7d_printPKcz
	leal	-780(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	$LC238, 4(%esp)
	movl	$_line2, (%esp)
	call	_sprintf
	movl	$_line2, (%esp)
	call	__Z13add_to_bufferPc
L536:
	movl	-12(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	$80, 4(%esp)
	leal	-780(%ebp), %eax
	movl	%eax, (%esp)
	call	_fgets
	testl	%eax, %eax
	setne	%al
	testb	%al, %al
	jne	L537
	movl	$0, %eax
	leave
	ret
	.section .rdata,"dr"
LC239:
	.ascii "Sending report: %s\12\0"
	.text
	.globl	__Z11send_reportv
	.def	__Z11send_reportv;	.scl	2;	.type	32;	.endef
__Z11send_reportv:
	pushl	%ebp
	movl	%esp, %ebp
	movl	$1048616, %eax
	call	___chkstk_ms
	subl	%eax, %esp
	movl	$1263484205, -18(%ebp)
	movw	$11581, -14(%ebp)
	movb	$0, -12(%ebp)
	movl	_my_trace, %eax
	movl	67915420(%eax), %eax
	cmpl	$4, %eax
	je	L541
	cmpl	$4, %eax
	ja	L542
	cmpl	$1, %eax
	je	L543
	cmpl	$1, %eax
	jb	L544
	cmpl	$2, %eax
	je	L545
	cmpl	$3, %eax
	je	L546
	jmp	L540
L542:
	cmpl	$6, %eax
	je	L547
	cmpl	$6, %eax
	jb	L548
	cmpl	$7, %eax
	je	L549
	cmpl	$153, %eax
	je	L550
	jmp	L540
L544:
	movw	$17234, -11(%ebp)
	movb	$0, -9(%ebp)
	jmp	L540
L543:
	movw	$21074, -11(%ebp)
	movb	$0, -9(%ebp)
	jmp	L540
L545:
	movw	$22610, -11(%ebp)
	movb	$0, -9(%ebp)
	jmp	L540
L546:
	movw	$21330, -11(%ebp)
	movb	$0, -9(%ebp)
	jmp	L540
L541:
	movw	$17490, -11(%ebp)
	movb	$0, -9(%ebp)
	jmp	L540
L548:
	movw	$16978, -11(%ebp)
	movb	$0, -9(%ebp)
	jmp	L540
L547:
	movw	$17746, -11(%ebp)
	movb	$0, -9(%ebp)
	jmp	L540
L549:
	movw	$18770, -11(%ebp)
	movb	$0, -9(%ebp)
	jmp	L540
L550:
	movw	$20050, -11(%ebp)
	movb	$0, -9(%ebp)
	nop
L540:
	leal	-11(%ebp), %eax
	movl	%eax, 4(%esp)
	leal	-1048594(%ebp), %eax
	movl	%eax, (%esp)
	call	_strcpy
	movl	_my_trace, %eax
	addl	$67915424, %eax
	movl	%eax, 4(%esp)
	leal	-1048594(%ebp), %eax
	movl	%eax, (%esp)
	call	_strcat
	leal	-18(%ebp), %eax
	movl	%eax, 4(%esp)
	leal	-1048594(%ebp), %eax
	movl	%eax, (%esp)
	call	_strcat
	leal	-1048594(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC239, (%esp)
	call	__Z7d_printPKcz
	leal	-1048594(%ebp), %eax
	movl	%eax, (%esp)
	call	_strlen
	movl	%eax, %edx
	movl	_my_trace, %eax
	movl	70012892(%eax), %eax
	movl	$0, 12(%esp)
	movl	%edx, 8(%esp)
	leal	-1048594(%ebp), %edx
	movl	%edx, 4(%esp)
	movl	%eax, (%esp)
	movl	__imp__send@16, %eax
	call	*%eax
	subl	$16, %esp
	movl	$0, %eax
	leave
	ret
	.section .rdata,"dr"
LC240:
	.ascii "Listing reactions\12\0"
LC241:
	.ascii "%s:\12\0"
LC242:
	.ascii "%s:0x%08x\12\0"
	.text
	.globl	__Z14list_reactionsv
	.def	__Z14list_reactionsv;	.scl	2;	.type	32;	.endef
__Z14list_reactionsv:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%edi
	pushl	%ebx
	subl	$800, %esp
	movl	$LC240, (%esp)
	call	__Z7d_printPKcz
	movl	_my_trace, %eax
	movl	$7, 67915420(%eax)
	movl	_my_trace, %eax
	addl	$67915424, %eax
	movb	$0, (%eax)
	movl	$0, -12(%ebp)
	jmp	L553
L556:
	movl	_my_trace, %eax
	movl	70021896(%eax), %edx
	movl	-12(%ebp), %eax
	imull	$188, %eax, %eax
	addl	%edx, %eax
	movl	%eax, -20(%ebp)
	movl	-20(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	$LC241, 4(%esp)
	leal	-792(%ebp), %eax
	movl	%eax, (%esp)
	call	_sprintf
	movl	$0, -16(%ebp)
	jmp	L554
L555:
	movl	-20(%ebp), %eax
	movl	180(%eax), %eax
	movl	%eax, -24(%ebp)
	movl	-24(%ebp), %eax
	movl	516(%eax), %eax
	movl	-24(%ebp), %edx
	addl	$4, %edx
	movl	%eax, 12(%esp)
	movl	%edx, 8(%esp)
	movl	$LC242, 4(%esp)
	leal	-792(%ebp), %eax
	movl	%eax, (%esp)
	call	_sprintf
	addl	$1, -16(%ebp)
L554:
	movl	-20(%ebp), %eax
	movl	184(%eax), %eax
	cmpl	-16(%ebp), %eax
	ja	L555
	addl	$1, -12(%ebp)
L553:
	movl	_my_trace, %eax
	movl	70021900(%eax), %eax
	cmpl	-12(%ebp), %eax
	ja	L556
	movl	_my_trace, %eax
	leal	67915424(%eax), %edx
	leal	-792(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	%edx, (%esp)
	call	_strcat
	movl	_my_trace, %eax
	leal	67915424(%eax), %edx
	movl	%edx, %eax
	movl	$-1, %ecx
	movl	%eax, %ebx
	movl	$0, %eax
	movl	%ebx, %edi
	repnz scasb
	movl	%ecx, %eax
	notl	%eax
	subl	$1, %eax
	addl	%edx, %eax
	movw	$10, (%eax)
	movl	$0, %eax
	addl	$800, %esp
	popl	%ebx
	popl	%edi
	popl	%ebp
	ret
	.section .rdata,"dr"
LC243:
	.ascii "Listing BPTs\12\0"
LC244:
	.ascii "0x%08x\12\0"
	.text
	.globl	__Z9list_bptsv
	.def	__Z9list_bptsv;	.scl	2;	.type	32;	.endef
__Z9list_bptsv:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%edi
	pushl	%ebx
	subl	$800, %esp
	movl	$LC243, (%esp)
	call	__Z7d_printPKcz
	movl	_my_trace, %eax
	movl	$7, 67915420(%eax)
	movl	_my_trace, %eax
	addl	$67915424, %eax
	movb	$0, (%eax)
	movl	$0, -12(%ebp)
	jmp	L559
L560:
	movl	_my_trace, %edx
	movl	-12(%ebp), %eax
	imull	$532, %eax, %eax
	addl	%edx, %eax
	addl	$67643024, %eax
	movl	8(%eax), %eax
	movl	%eax, 8(%esp)
	movl	$LC244, 4(%esp)
	leal	-780(%ebp), %eax
	movl	%eax, (%esp)
	call	_sprintf
	movl	_my_trace, %eax
	leal	67915424(%eax), %edx
	leal	-780(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	%edx, (%esp)
	call	_strcat
	movl	_my_trace, %eax
	leal	67915424(%eax), %edx
	movl	%edx, %eax
	movl	$-1, %ecx
	movl	%eax, %ebx
	movl	$0, %eax
	movl	%ebx, %edi
	repnz scasb
	movl	%ecx, %eax
	notl	%eax
	subl	$1, %eax
	addl	%edx, %eax
	movw	$10, (%eax)
	addl	$1, -12(%ebp)
L559:
	movl	-12(%ebp), %ecx
	movl	$0, %ebx
	movl	_my_trace, %eax
	movl	68964068(%eax), %edx
	movl	68964064(%eax), %eax
	cmpl	%edx, %ebx
	jb	L560
	cmpl	%edx, %ebx
	ja	L563
	cmpl	%eax, %ecx
	jb	L560
L563:
	movl	$0, %eax
	addl	$800, %esp
	popl	%ebx
	popl	%edi
	popl	%ebp
	ret
	.section .rdata,"dr"
LC245:
	.ascii "Listing LIBs\12\0"
LC246:
	.ascii "Currently have %d libs\12\0"
LC247:
	.ascii "Reporting: %s\12\0"
	.text
	.globl	__Z9list_libsv
	.def	__Z9list_libsv;	.scl	2;	.type	32;	.endef
__Z9list_libsv:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%edi
	pushl	%ebx
	subl	$800, %esp
	movl	$LC245, (%esp)
	call	__Z7d_printPKcz
	movl	_my_trace, %eax
	movl	$7, 67915420(%eax)
	movl	_my_trace, %eax
	movl	68964060(%eax), %edx
	movl	68964056(%eax), %eax
	movl	%eax, 4(%esp)
	movl	%edx, 8(%esp)
	movl	$LC246, (%esp)
	call	__Z7d_printPKcz
	movl	_my_trace, %eax
	addl	$67915424, %eax
	movb	$0, (%eax)
	movl	$0, -12(%ebp)
	jmp	L565
L566:
	movl	_my_trace, %edx
	movl	-12(%ebp), %eax
	sall	$3, %eax
	movl	%eax, %ecx
	sall	$7, %ecx
	addl	%ecx, %eax
	addl	$67114112, %eax
	addl	%edx, %eax
	leal	8(%eax), %edx
	leal	-780(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	%edx, (%esp)
	call	__Z13serialize_libP10_LIB_ENTRYPc
	movl	_my_trace, %eax
	leal	67915424(%eax), %edx
	leal	-780(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	%edx, (%esp)
	call	_strcat
	movl	_my_trace, %eax
	leal	67915424(%eax), %edx
	movl	%edx, %eax
	movl	$-1, %ecx
	movl	%eax, %ebx
	movl	$0, %eax
	movl	%ebx, %edi
	repnz scasb
	movl	%ecx, %eax
	notl	%eax
	subl	$1, %eax
	addl	%edx, %eax
	movw	$10, (%eax)
	addl	$1, -12(%ebp)
L565:
	movl	-12(%ebp), %ecx
	movl	$0, %ebx
	movl	_my_trace, %eax
	movl	68964060(%eax), %edx
	movl	68964056(%eax), %eax
	cmpl	%edx, %ebx
	jb	L566
	cmpl	%edx, %ebx
	ja	L569
	cmpl	%eax, %ecx
	jb	L566
L569:
	movl	_my_trace, %eax
	addl	$67915424, %eax
	movl	%eax, 4(%esp)
	movl	$LC247, (%esp)
	call	__Z7d_printPKcz
	movl	$0, %eax
	addl	$800, %esp
	popl	%ebx
	popl	%edi
	popl	%ebp
	ret
	.section .rdata,"dr"
LC248:
	.ascii "Listings TEBs\12\0"
LC249:
	.ascii "Currently have %d threads\12\0"
	.text
	.globl	__Z9list_tebsv
	.def	__Z9list_tebsv;	.scl	2;	.type	32;	.endef
__Z9list_tebsv:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%edi
	pushl	%ebx
	subl	$800, %esp
	movl	$LC248, (%esp)
	call	__Z7d_printPKcz
	movl	_my_trace, %eax
	movl	$7, 67915420(%eax)
	movl	_my_trace, %eax
	movl	68964052(%eax), %edx
	movl	68964048(%eax), %eax
	movl	%eax, 4(%esp)
	movl	%edx, 8(%esp)
	movl	$LC249, (%esp)
	call	__Z7d_printPKcz
	movl	_my_trace, %eax
	addl	$67915424, %eax
	movb	$0, (%eax)
	movl	$0, -12(%ebp)
	jmp	L571
L572:
	movl	_my_trace, %eax
	leal	67915424(%eax), %edx
	movl	%edx, %eax
	movl	$-1, %ecx
	movl	%eax, %ebx
	movl	$0, %eax
	movl	%ebx, %edi
	repnz scasb
	movl	%ecx, %eax
	notl	%eax
	subl	$1, %eax
	addl	%edx, %eax
	movw	$120, (%eax)
	addl	$1, -12(%ebp)
L571:
	movl	-12(%ebp), %ecx
	movl	$0, %ebx
	movl	_my_trace, %eax
	movl	68964052(%eax), %edx
	movl	68964048(%eax), %eax
	cmpl	%edx, %ebx
	jb	L572
	cmpl	%edx, %ebx
	ja	L575
	cmpl	%eax, %ecx
	jb	L572
L575:
	movl	_my_trace, %eax
	addl	$67915424, %eax
	movl	%eax, 4(%esp)
	movl	$LC247, (%esp)
	call	__Z7d_printPKcz
	movl	$0, %eax
	addl	$800, %esp
	popl	%ebx
	popl	%edi
	popl	%ebp
	ret
	.section .rdata,"dr"
	.align 4
LC250:
	.ascii "Failed to set context, error: 0x%08x\12\0"
	.text
	.globl	__Z13write_contextmP8_CONTEXT
	.def	__Z13write_contextmP8_CONTEXT;	.scl	2;	.type	32;	.endef
__Z13write_contextmP8_CONTEXT:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$40, %esp
	movl	_my_trace, %eax
	movl	8(%ebp), %edx
	addl	$32, %edx
	movl	8(%eax,%edx,4), %eax
	movl	%eax, -12(%ebp)
	movl	_my_trace, %ecx
	movl	-12(%ebp), %edx
	movl	%edx, %eax
	sall	$2, %eax
	addl	%edx, %eax
	sall	$2, %eax
	addl	%ecx, %eax
	addl	$67108992, %eax
	movl	16(%eax), %eax
	movl	%eax, -16(%ebp)
	movl	12(%ebp), %eax
	movl	$65543, (%eax)
	movl	12(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	-16(%ebp), %eax
	movl	%eax, (%esp)
	movl	__imp__SetThreadContext@8, %eax
	call	*%eax
	subl	$8, %esp
	testl	%eax, %eax
	sete	%al
	testb	%al, %al
	je	L577
	movl	__imp__GetLastError@0, %eax
	call	*%eax
	movl	%eax, 4(%esp)
	movl	$LC250, (%esp)
	call	__Z7d_printPKcz
L577:
	movl	$0, %eax
	leave
	ret
	.section .rdata,"dr"
LC251:
	.ascii "Thread32First\0"
LC252:
	.ascii "0x%08x - %08x\12\0"
	.text
	.globl	__Z13list_all_tebsv
	.def	__Z13list_all_tebsv;	.scl	2;	.type	32;	.endef
__Z13list_all_tebsv:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$824, %esp
	movl	$-1, -12(%ebp)
	movl	$0, 4(%esp)
	movl	$4, (%esp)
	call	_CreateToolhelp32Snapshot@8
	subl	$8, %esp
	movl	%eax, -12(%ebp)
	cmpl	$-1, -12(%ebp)
	jne	L580
	movl	$0, %eax
	jmp	L583
L580:
	movl	$28, -40(%ebp)
	leal	-40(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	-12(%ebp), %eax
	movl	%eax, (%esp)
	call	_Thread32First@8
	subl	$8, %esp
	testl	%eax, %eax
	sete	%al
	testb	%al, %al
	je	L582
	movl	$LC251, (%esp)
	call	__Z7d_printPKcz
	movl	-12(%ebp), %eax
	movl	%eax, (%esp)
	movl	__imp__CloseHandle@4, %eax
	call	*%eax
	subl	$4, %esp
	movl	$0, %eax
	jmp	L583
L582:
	movl	-32(%ebp), %edx
	movl	-28(%ebp), %eax
	movl	%edx, 12(%esp)
	movl	%eax, 8(%esp)
	movl	$LC252, 4(%esp)
	leal	-808(%ebp), %eax
	movl	%eax, (%esp)
	call	_sprintf
	movl	_my_trace, %eax
	leal	67915424(%eax), %edx
	leal	-808(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	%edx, (%esp)
	call	_strcat
	leal	-40(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	-12(%ebp), %eax
	movl	%eax, (%esp)
	call	_Thread32Next@8
	subl	$8, %esp
	testl	%eax, %eax
	setne	%al
	testb	%al, %al
	jne	L582
	movl	-12(%ebp), %eax
	movl	%eax, (%esp)
	movl	__imp__CloseHandle@4, %eax
	call	*%eax
	subl	$4, %esp
	movl	$0, %eax
L583:
	leave
	ret
	.section .rdata,"dr"
LC253:
	.ascii "Trying relese TID: 0x%08x\12\0"
	.align 4
LC254:
	.ascii "Failed to open thread, error: 0x%08x\12\0"
	.align 4
LC255:
	.ascii "Failed to resume thread, error: 0x%08x\12\0"
	.text
	.globl	__Z14release_threadm
	.def	__Z14release_threadm;	.scl	2;	.type	32;	.endef
__Z14release_threadm:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$808, %esp
	movl	$-1, -12(%ebp)
	movl	8(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC253, (%esp)
	call	__Z7d_printPKcz
	movl	8(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	$0, 4(%esp)
	movl	$2, (%esp)
	movl	__imp__OpenThread@12, %eax
	call	*%eax
	subl	$12, %esp
	movl	%eax, -12(%ebp)
	cmpl	$0, -12(%ebp)
	sete	%al
	testb	%al, %al
	je	L585
	movl	__imp__GetLastError@0, %eax
	call	*%eax
	movl	%eax, 4(%esp)
	movl	$LC254, (%esp)
	call	__Z7d_printPKcz
	movl	__imp__GetLastError@0, %eax
	call	*%eax
	movl	%eax, 8(%esp)
	movl	$LC99, 4(%esp)
	leal	-780(%ebp), %eax
	movl	%eax, (%esp)
	call	_sprintf
	movl	_my_trace, %eax
	leal	67915424(%eax), %edx
	leal	-780(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	%edx, (%esp)
	call	_strcpy
L585:
	movl	-12(%ebp), %eax
	movl	%eax, (%esp)
	movl	__imp__ResumeThread@4, %eax
	call	*%eax
	subl	$4, %esp
	cmpl	$-1, %eax
	sete	%al
	testb	%al, %al
	je	L586
	movl	__imp__GetLastError@0, %eax
	call	*%eax
	movl	%eax, 4(%esp)
	movl	$LC255, (%esp)
	call	__Z7d_printPKcz
	movl	__imp__GetLastError@0, %eax
	call	*%eax
	movl	%eax, 8(%esp)
	movl	$LC99, 4(%esp)
	leal	-780(%ebp), %eax
	movl	%eax, (%esp)
	call	_sprintf
	movl	_my_trace, %eax
	leal	67915424(%eax), %edx
	leal	-780(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	%edx, (%esp)
	call	_strcpy
L586:
	movl	$0, %eax
	leave
	ret
	.section .rdata,"dr"
	.align 4
LC256:
	.ascii "Trying to get context of TID: 0x%08x, index: 0x%08x, handle: 0x%08x\12\0"
	.text
	.globl	__Z12read_contextmP8_CONTEXT
	.def	__Z12read_contextmP8_CONTEXT;	.scl	2;	.type	32;	.endef
__Z12read_contextmP8_CONTEXT:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$808, %esp
	movl	$-1, -12(%ebp)
	cmpl	$0, 8(%ebp)
	jne	L589
	movl	_my_trace, %eax
	movl	68964100(%eax), %eax
	movl	%eax, 8(%ebp)
L589:
	movl	_my_trace, %eax
	movl	8(%ebp), %edx
	addl	$32, %edx
	movl	8(%eax,%edx,4), %eax
	movl	%eax, -16(%ebp)
	movl	_my_trace, %ecx
	movl	-16(%ebp), %edx
	movl	%edx, %eax
	sall	$2, %eax
	addl	%edx, %eax
	sall	$2, %eax
	addl	%ecx, %eax
	addl	$67108992, %eax
	movl	16(%eax), %eax
	movl	%eax, -12(%ebp)
	movl	-12(%ebp), %eax
	movl	%eax, 12(%esp)
	movl	-16(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	8(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC256, (%esp)
	call	__Z7d_printPKcz
	movl	12(%ebp), %eax
	movl	$65543, (%eax)
	movl	12(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	-12(%ebp), %eax
	movl	%eax, (%esp)
	movl	__imp__GetThreadContext@8, %eax
	call	*%eax
	subl	$8, %esp
	testl	%eax, %eax
	sete	%al
	testb	%al, %al
	je	L590
	movl	__imp__GetLastError@0, %eax
	call	*%eax
	movl	%eax, 4(%esp)
	movl	$LC7, (%esp)
	call	__Z7d_printPKcz
	movl	__imp__GetLastError@0, %eax
	call	*%eax
	movl	%eax, 8(%esp)
	movl	$LC99, 4(%esp)
	leal	-784(%ebp), %eax
	movl	%eax, (%esp)
	call	_sprintf
	movl	_my_trace, %eax
	leal	67915424(%eax), %edx
	leal	-784(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	%edx, (%esp)
	call	_strcpy
L590:
	movl	$0, %eax
	leave
	ret
	.section .rdata,"dr"
	.align 4
LC257:
	.ascii "Trying to write: 0x%08x @ %p, handle: 0x%08x\12\0"
LC258:
	.ascii "New val @ %p: 0x%08x\12\0"
	.text
	.globl	__Z11write_dwordmm
	.def	__Z11write_dwordmm;	.scl	2;	.type	32;	.endef
__Z11write_dwordmm:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$824, %esp
	movl	_my_trace, %eax
	movl	128(%eax), %eax
	movl	%eax, -12(%ebp)
	movl	_my_trace, %eax
	movl	128(%eax), %edx
	movl	12(%ebp), %eax
	movl	%edx, 12(%esp)
	movl	8(%ebp), %edx
	movl	%edx, 8(%esp)
	movl	%eax, 4(%esp)
	movl	$LC257, (%esp)
	call	__Z7d_printPKcz
	movl	8(%ebp), %edx
	movl	_my_trace, %eax
	movl	128(%eax), %eax
	leal	-16(%ebp), %ecx
	movl	%ecx, 16(%esp)
	movl	$4, 12(%esp)
	leal	12(%ebp), %ecx
	movl	%ecx, 8(%esp)
	movl	%edx, 4(%esp)
	movl	%eax, (%esp)
	call	__Z12write_memoryPvS_S_mPm
	movl	-16(%ebp), %eax
	cmpl	$4, %eax
	jne	L593
	leal	-784(%ebp), %eax
	movl	$1380931396, (%eax)
	movl	$1920409668, 4(%eax)
	movl	$1702130793, 8(%eax)
	movw	$110, 12(%eax)
	movl	_my_trace, %eax
	leal	67915424(%eax), %edx
	leal	-784(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	%edx, (%esp)
	call	_strcpy
	jmp	L594
L593:
	movl	__imp__GetLastError@0, %eax
	call	*%eax
	movl	%eax, 8(%esp)
	movl	$LC99, 4(%esp)
	leal	-784(%ebp), %eax
	movl	%eax, (%esp)
	call	_sprintf
	movl	_my_trace, %eax
	leal	67915424(%eax), %edx
	leal	-784(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	%edx, (%esp)
	call	_strcpy
L594:
	movl	8(%ebp), %edx
	movl	_my_trace, %eax
	movl	128(%eax), %eax
	leal	-16(%ebp), %ecx
	movl	%ecx, 16(%esp)
	movl	$4, 12(%esp)
	leal	12(%ebp), %ecx
	movl	%ecx, 8(%esp)
	movl	%edx, 4(%esp)
	movl	%eax, (%esp)
	call	__Z11read_memoryPvS_S_mPm
	movl	12(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	8(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC258, (%esp)
	call	__Z7d_printPKcz
	movl	$0, %eax
	leave
	ret
	.globl	__Z9read_bytem
	.def	__Z9read_bytem;	.scl	2;	.type	32;	.endef
__Z9read_bytem:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$824, %esp
	movl	8(%ebp), %edx
	movl	_my_trace, %eax
	movl	128(%eax), %eax
	leal	-16(%ebp), %ecx
	movl	%ecx, 16(%esp)
	movl	$1, 12(%esp)
	leal	-9(%ebp), %ecx
	movl	%ecx, 8(%esp)
	movl	%edx, 4(%esp)
	movl	%eax, (%esp)
	call	__Z11read_memoryPvS_S_mPm
	movzbl	-9(%ebp), %eax
	leave
	ret
	.section .rdata,"dr"
LC259:
	.ascii "BYTE written\0"
	.text
	.globl	__Z10write_bytemPc
	.def	__Z10write_bytemPc;	.scl	2;	.type	32;	.endef
__Z10write_bytemPc:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$824, %esp
	movl	$16, 8(%esp)
	movl	$0, 4(%esp)
	movl	12(%ebp), %eax
	movl	%eax, (%esp)
	call	_strtoul
	movb	%al, -13(%ebp)
	movl	8(%ebp), %edx
	movl	_my_trace, %eax
	movl	128(%eax), %eax
	leal	-12(%ebp), %ecx
	movl	%ecx, 16(%esp)
	movl	$1, 12(%esp)
	leal	-13(%ebp), %ecx
	movl	%ecx, 8(%esp)
	movl	%edx, 4(%esp)
	movl	%eax, (%esp)
	call	__Z12write_memoryPvS_S_mPm
	movl	-12(%ebp), %eax
	cmpl	$1, %eax
	jne	L599
	movl	12(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	$LC259, 4(%esp)
	leal	-781(%ebp), %eax
	movl	%eax, (%esp)
	call	_sprintf
	movl	_my_trace, %eax
	leal	67915424(%eax), %edx
	leal	-781(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	%edx, (%esp)
	call	_strcpy
	jmp	L600
L599:
	leal	-781(%ebp), %eax
	movl	$1869771333, (%eax)
	movw	$114, 4(%eax)
	movl	_my_trace, %eax
	leal	67915424(%eax), %edx
	leal	-781(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	%edx, (%esp)
	call	_strcpy
L600:
	movl	$0, %eax
	leave
	ret
	.section .rdata,"dr"
LC260:
	.ascii "%04x\0"
	.text
	.globl	__Z9read_wordm
	.def	__Z9read_wordm;	.scl	2;	.type	32;	.endef
__Z9read_wordm:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$824, %esp
	movl	8(%ebp), %edx
	movl	_my_trace, %eax
	movl	128(%eax), %eax
	leal	-16(%ebp), %ecx
	movl	%ecx, 16(%esp)
	movl	$2, 12(%esp)
	leal	-10(%ebp), %ecx
	movl	%ecx, 8(%esp)
	movl	%edx, 4(%esp)
	movl	%eax, (%esp)
	call	__Z11read_memoryPvS_S_mPm
	movl	-16(%ebp), %eax
	cmpl	$2, %eax
	jne	L603
	movzwl	-10(%ebp), %eax
	movzwl	%ax, %eax
	movl	%eax, 8(%esp)
	movl	$LC260, 4(%esp)
	leal	-784(%ebp), %eax
	movl	%eax, (%esp)
	call	_sprintf
	movl	_my_trace, %eax
	leal	67915424(%eax), %edx
	leal	-784(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	%edx, (%esp)
	call	_strcpy
	jmp	L604
L603:
	leal	-784(%ebp), %eax
	movl	$1869771333, (%eax)
	movw	$114, 4(%eax)
	movl	_my_trace, %eax
	leal	67915424(%eax), %edx
	leal	-784(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	%edx, (%esp)
	call	_strcpy
L604:
	movzwl	-10(%ebp), %eax
	movzwl	%ax, %eax
	leave
	ret
	.section .rdata,"dr"
LC261:
	.ascii "WORD written\0"
	.text
	.globl	__Z10write_wordmPc
	.def	__Z10write_wordmPc;	.scl	2;	.type	32;	.endef
__Z10write_wordmPc:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$824, %esp
	movl	$16, 8(%esp)
	movl	$0, 4(%esp)
	movl	12(%ebp), %eax
	movl	%eax, (%esp)
	call	_strtoul
	movw	%ax, -14(%ebp)
	movl	8(%ebp), %edx
	movl	_my_trace, %eax
	movl	128(%eax), %eax
	leal	-12(%ebp), %ecx
	movl	%ecx, 16(%esp)
	movl	$2, 12(%esp)
	leal	-14(%ebp), %ecx
	movl	%ecx, 8(%esp)
	movl	%edx, 4(%esp)
	movl	%eax, (%esp)
	call	__Z12write_memoryPvS_S_mPm
	movl	-12(%ebp), %eax
	cmpl	$2, %eax
	jne	L607
	movl	12(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	$LC261, 4(%esp)
	leal	-782(%ebp), %eax
	movl	%eax, (%esp)
	call	_sprintf
	movl	_my_trace, %eax
	leal	67915424(%eax), %edx
	leal	-782(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	%edx, (%esp)
	call	_strcpy
	jmp	L608
L607:
	leal	-782(%ebp), %eax
	movl	$1869771333, (%eax)
	movw	$114, 4(%eax)
	movl	_my_trace, %eax
	leal	67915424(%eax), %edx
	leal	-782(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	%edx, (%esp)
	call	_strcpy
L608:
	movl	$0, %eax
	leave
	ret
	.globl	__Z10read_dwordm
	.def	__Z10read_dwordm;	.scl	2;	.type	32;	.endef
__Z10read_dwordm:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$824, %esp
	movl	8(%ebp), %edx
	movl	_my_trace, %eax
	movl	128(%eax), %eax
	leal	-16(%ebp), %ecx
	movl	%ecx, 16(%esp)
	movl	$4, 12(%esp)
	leal	-12(%ebp), %ecx
	movl	%ecx, 8(%esp)
	movl	%edx, 4(%esp)
	movl	%eax, (%esp)
	call	__Z11read_memoryPvS_S_mPm
	movl	-12(%ebp), %eax
	leave
	ret
	.section .rdata,"dr"
LC262:
	.ascii "%02x \0"
LC263:
	.ascii "%c\0"
	.text
	.globl	__Z6hexifyPcS_j
	.def	__Z6hexifyPcS_j;	.scl	2;	.type	32;	.endef
__Z6hexifyPcS_j:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%edi
	subl	$1604, %esp
	movl	16(%ebp), %eax
	shrl	$5, %eax
	movl	%eax, -20(%ebp)
	movl	16(%ebp), %eax
	andl	$31, %eax
	movl	%eax, -24(%ebp)
	movl	$0, -16(%ebp)
	jmp	L613
L622:
	movl	$768, 8(%esp)
	movl	$0, 4(%esp)
	leal	-1560(%ebp), %eax
	movl	%eax, (%esp)
	call	_memset
	movl	$32, 8(%esp)
	movl	$0, 4(%esp)
	leal	-1592(%ebp), %eax
	movl	%eax, (%esp)
	call	_memset
	movl	-16(%ebp), %eax
	sall	$5, %eax
	movl	%eax, %edx
	movl	8(%ebp), %eax
	addl	%edx, %eax
	movl	(%eax), %edx
	movl	%edx, -1592(%ebp)
	movl	4(%eax), %edx
	movl	%edx, -1588(%ebp)
	movl	8(%eax), %edx
	movl	%edx, -1584(%ebp)
	movl	12(%eax), %edx
	movl	%edx, -1580(%ebp)
	movl	16(%eax), %edx
	movl	%edx, -1576(%ebp)
	movl	20(%eax), %edx
	movl	%edx, -1572(%ebp)
	movl	24(%eax), %edx
	movl	%edx, -1568(%ebp)
	movl	28(%eax), %eax
	movl	%eax, -1564(%ebp)
	movl	$0, -12(%ebp)
	jmp	L614
L617:
	leal	-1592(%ebp), %edx
	movl	-12(%ebp), %eax
	addl	%edx, %eax
	movzbl	(%eax), %eax
	cmpb	$10, %al
	jne	L615
	leal	-1592(%ebp), %edx
	movl	-12(%ebp), %eax
	addl	%edx, %eax
	movb	$46, (%eax)
L615:
	leal	-1592(%ebp), %edx
	movl	-12(%ebp), %eax
	addl	%edx, %eax
	movzbl	(%eax), %eax
	cmpb	$13, %al
	jne	L616
	leal	-1592(%ebp), %edx
	movl	-12(%ebp), %eax
	addl	%edx, %eax
	movb	$46, (%eax)
L616:
	leal	-1592(%ebp), %edx
	movl	-12(%ebp), %eax
	addl	%edx, %eax
	movzbl	(%eax), %eax
	movsbl	%al, %eax
	movzbl	%al, %eax
	movl	%eax, 8(%esp)
	movl	$LC262, 4(%esp)
	leal	-792(%ebp), %eax
	movl	%eax, (%esp)
	call	_sprintf
	leal	-792(%ebp), %eax
	movl	%eax, 4(%esp)
	leal	-1560(%ebp), %eax
	movl	%eax, (%esp)
	call	_strcat
	addl	$1, -12(%ebp)
L614:
	cmpl	$31, -12(%ebp)
	jbe	L617
	leal	-1560(%ebp), %eax
	movl	$-1, %ecx
	movl	%eax, %edx
	movl	$0, %eax
	movl	%edx, %edi
	repnz scasb
	movl	%ecx, %eax
	notl	%eax
	leal	-1(%eax), %edx
	leal	-1560(%ebp), %eax
	addl	%edx, %eax
	movw	$8224, (%eax)
	movb	$0, 2(%eax)
	movl	$0, -12(%ebp)
	jmp	L618
L621:
	leal	-1592(%ebp), %edx
	movl	-12(%ebp), %eax
	addl	%edx, %eax
	movzbl	(%eax), %eax
	cmpb	$10, %al
	jne	L619
	leal	-1592(%ebp), %edx
	movl	-12(%ebp), %eax
	addl	%edx, %eax
	movb	$46, (%eax)
L619:
	leal	-1592(%ebp), %edx
	movl	-12(%ebp), %eax
	addl	%edx, %eax
	movzbl	(%eax), %eax
	cmpb	$13, %al
	jne	L620
	leal	-1592(%ebp), %edx
	movl	-12(%ebp), %eax
	addl	%edx, %eax
	movb	$46, (%eax)
L620:
	leal	-1592(%ebp), %edx
	movl	-12(%ebp), %eax
	addl	%edx, %eax
	movzbl	(%eax), %eax
	movsbl	%al, %eax
	movl	%eax, 8(%esp)
	movl	$LC263, 4(%esp)
	leal	-792(%ebp), %eax
	movl	%eax, (%esp)
	call	_sprintf
	leal	-792(%ebp), %eax
	movl	%eax, 4(%esp)
	leal	-1560(%ebp), %eax
	movl	%eax, (%esp)
	call	_strcat
	addl	$1, -12(%ebp)
L618:
	cmpl	$31, -12(%ebp)
	jbe	L621
	leal	-1560(%ebp), %eax
	movl	$-1, %ecx
	movl	%eax, %edx
	movl	$0, %eax
	movl	%edx, %edi
	repnz scasb
	movl	%ecx, %eax
	notl	%eax
	leal	-1(%eax), %edx
	leal	-1560(%ebp), %eax
	addl	%edx, %eax
	movw	$10, (%eax)
	leal	-1560(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	12(%ebp), %eax
	movl	%eax, (%esp)
	call	_strcat
	addl	$1, -16(%ebp)
L613:
	movl	-16(%ebp), %eax
	cmpl	-20(%ebp), %eax
	jb	L622
	movl	$768, 8(%esp)
	movl	$0, 4(%esp)
	leal	-1560(%ebp), %eax
	movl	%eax, (%esp)
	call	_memset
	movl	$32, 8(%esp)
	movl	$0, 4(%esp)
	leal	-1592(%ebp), %eax
	movl	%eax, (%esp)
	call	_memset
	movl	-16(%ebp), %eax
	sall	$5, %eax
	movl	%eax, %edx
	movl	8(%ebp), %eax
	addl	%eax, %edx
	movl	-24(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	%edx, 4(%esp)
	leal	-1592(%ebp), %eax
	movl	%eax, (%esp)
	call	_memcpy
	movl	$0, -12(%ebp)
	jmp	L623
L626:
	leal	-1592(%ebp), %edx
	movl	-12(%ebp), %eax
	addl	%edx, %eax
	movzbl	(%eax), %eax
	cmpb	$10, %al
	jne	L624
	leal	-1592(%ebp), %edx
	movl	-12(%ebp), %eax
	addl	%edx, %eax
	movb	$46, (%eax)
L624:
	leal	-1592(%ebp), %edx
	movl	-12(%ebp), %eax
	addl	%edx, %eax
	movzbl	(%eax), %eax
	cmpb	$13, %al
	jne	L625
	leal	-1592(%ebp), %edx
	movl	-12(%ebp), %eax
	addl	%edx, %eax
	movb	$46, (%eax)
L625:
	leal	-1592(%ebp), %edx
	movl	-12(%ebp), %eax
	addl	%edx, %eax
	movzbl	(%eax), %eax
	movsbl	%al, %eax
	movzbl	%al, %eax
	movl	%eax, 8(%esp)
	movl	$LC262, 4(%esp)
	leal	-792(%ebp), %eax
	movl	%eax, (%esp)
	call	_sprintf
	leal	-792(%ebp), %eax
	movl	%eax, 4(%esp)
	leal	-1560(%ebp), %eax
	movl	%eax, (%esp)
	call	_strcat
	addl	$1, -12(%ebp)
L623:
	movl	-12(%ebp), %eax
	cmpl	-24(%ebp), %eax
	jb	L626
	leal	-1560(%ebp), %eax
	movl	$-1, %ecx
	movl	%eax, %edx
	movl	$0, %eax
	movl	%edx, %edi
	repnz scasb
	movl	%ecx, %eax
	notl	%eax
	leal	-1(%eax), %edx
	leal	-1560(%ebp), %eax
	addl	%edx, %eax
	movw	$8224, (%eax)
	movb	$0, 2(%eax)
	movl	$0, -12(%ebp)
	jmp	L627
L630:
	leal	-1592(%ebp), %edx
	movl	-12(%ebp), %eax
	addl	%edx, %eax
	movzbl	(%eax), %eax
	cmpb	$10, %al
	jne	L628
	leal	-1592(%ebp), %edx
	movl	-12(%ebp), %eax
	addl	%edx, %eax
	movb	$46, (%eax)
L628:
	leal	-1592(%ebp), %edx
	movl	-12(%ebp), %eax
	addl	%edx, %eax
	movzbl	(%eax), %eax
	cmpb	$13, %al
	jne	L629
	leal	-1592(%ebp), %edx
	movl	-12(%ebp), %eax
	addl	%edx, %eax
	movb	$46, (%eax)
L629:
	leal	-1592(%ebp), %edx
	movl	-12(%ebp), %eax
	addl	%edx, %eax
	movzbl	(%eax), %eax
	movsbl	%al, %eax
	movl	%eax, 8(%esp)
	movl	$LC263, 4(%esp)
	leal	-792(%ebp), %eax
	movl	%eax, (%esp)
	call	_sprintf
	leal	-792(%ebp), %eax
	movl	%eax, 4(%esp)
	leal	-1560(%ebp), %eax
	movl	%eax, (%esp)
	call	_strcat
	addl	$1, -12(%ebp)
L627:
	movl	-12(%ebp), %eax
	cmpl	-24(%ebp), %eax
	jb	L630
	leal	-1560(%ebp), %eax
	movl	$-1, %ecx
	movl	%eax, %edx
	movl	$0, %eax
	movl	%edx, %edi
	repnz scasb
	movl	%ecx, %eax
	notl	%eax
	leal	-1(%eax), %edx
	leal	-1560(%ebp), %eax
	addl	%edx, %eax
	movw	$10, (%eax)
	leal	-1560(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	12(%ebp), %eax
	movl	%eax, (%esp)
	call	_strcat
	movl	$0, %eax
	addl	$1604, %esp
	popl	%edi
	popl	%ebp
	ret
	.section .rdata,"dr"
LC264:
	.ascii "# out_arg1 @ %d\12\0"
	.align 4
LC265:
	.ascii "Trying to read 0x%08x bytes: @ %p, handle: 0x%08x\12\0"
LC266:
	.ascii "# out_arg2 @ %d\12\0"
LC267:
	.ascii "OU,0x%x,%s\12\0"
LC268:
	.ascii "Error: 0x%08x\0"
LC269:
	.ascii "# OU,0x%x error\12\0"
	.text
	.globl	__Z10out_regionmm
	.def	__Z10out_regionmm;	.scl	2;	.type	32;	.endef
__Z10out_regionmm:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$1592, %esp
	cmpl	$32, 12(%ebp)
	jbe	L633
	movl	$32, 12(%ebp)
L633:
	movl	12(%ebp), %eax
	addl	$1, %eax
	movl	%eax, (%esp)
	call	_malloc
	movl	%eax, -12(%ebp)
	movl	12(%ebp), %eax
	sall	$5, %eax
	movl	%eax, (%esp)
	call	_malloc
	movl	%eax, -16(%ebp)
	movl	12(%ebp), %eax
	sall	$5, %eax
	movl	%eax, 8(%esp)
	movl	$0, 4(%esp)
	movl	-16(%ebp), %eax
	movl	%eax, (%esp)
	call	_memset
	movl	_my_trace, %eax
	movl	68964020(%eax), %edx
	movl	68964016(%eax), %eax
	movl	%eax, 8(%esp)
	movl	%edx, 12(%esp)
	movl	$LC264, 4(%esp)
	leal	-792(%ebp), %eax
	movl	%eax, (%esp)
	call	_sprintf
	cmpl	$0, -12(%ebp)
	jne	L634
	leal	-1560(%ebp), %eax
	movl	$1869771333, (%eax)
	movl	$539828338, 4(%eax)
	movl	$544501614, 8(%eax)
	movl	$1970237029, 12(%eax)
	movl	$1830840423, 16(%eax)
	movl	$1919905125, 20(%eax)
	movw	$121, 24(%eax)
	movl	_my_trace, %eax
	leal	67915424(%eax), %edx
	leal	-1560(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	%edx, (%esp)
	call	_strcpy
	movl	$0, %eax
	jmp	L640
L634:
	cmpl	$0, -16(%ebp)
	jne	L636
	leal	-1560(%ebp), %eax
	movl	$1869771333, (%eax)
	movl	$539828338, 4(%eax)
	movl	$544501614, 8(%eax)
	movl	$1970237029, 12(%eax)
	movl	$1830840423, 16(%eax)
	movl	$1919905125, 20(%eax)
	movw	$121, 24(%eax)
	movl	_my_trace, %eax
	leal	67915424(%eax), %edx
	leal	-1560(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	%edx, (%esp)
	call	_strcpy
	movl	$0, %eax
	jmp	L640
L636:
	movl	_my_trace, %eax
	movl	88(%eax), %eax
	movl	%eax, 12(%esp)
	movl	8(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	12(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC265, (%esp)
	call	__Z7d_printPKcz
	movl	8(%ebp), %edx
	movl	_my_trace, %eax
	movl	88(%eax), %eax
	leal	-24(%ebp), %ecx
	movl	%ecx, 16(%esp)
	movl	12(%ebp), %ecx
	movl	%ecx, 12(%esp)
	movl	-12(%ebp), %ecx
	movl	%ecx, 8(%esp)
	movl	%edx, 4(%esp)
	movl	%eax, (%esp)
	call	__Z11read_memoryPvS_S_mPm
	movl	_my_trace, %eax
	movl	68964020(%eax), %edx
	movl	68964016(%eax), %eax
	movl	%eax, 8(%esp)
	movl	%edx, 12(%esp)
	movl	$LC266, 4(%esp)
	leal	-792(%ebp), %eax
	movl	%eax, (%esp)
	call	_sprintf
	leal	-792(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z13add_to_bufferPc
	movl	-24(%ebp), %eax
	testl	%eax, %eax
	je	L637
	movl	12(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	-16(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	-12(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z6hexifyPcS_j
	movl	%eax, -20(%ebp)
	cmpl	$0, -20(%ebp)
	je	L638
	leal	-1560(%ebp), %eax
	movl	$1869771333, (%eax)
	movl	$1969496178, 4(%eax)
	movl	$1735289202, 8(%eax)
	movl	$2019911712, 12(%eax)
	movl	$7956073, 16(%eax)
	movl	_my_trace, %eax
	leal	67915424(%eax), %edx
	leal	-1560(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	%edx, (%esp)
	call	_strcpy
	movl	$0, %eax
	jmp	L640
L638:
	movl	_my_trace, %eax
	movl	68964276(%eax), %eax
	movl	-16(%ebp), %edx
	movl	%edx, 12(%esp)
	movl	%eax, 8(%esp)
	movl	$LC267, 4(%esp)
	leal	-792(%ebp), %eax
	movl	%eax, (%esp)
	call	_sprintf
	leal	-792(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z13add_to_bufferPc
	jmp	L639
L637:
	movl	__imp__GetLastError@0, %eax
	call	*%eax
	movl	%eax, 8(%esp)
	movl	$LC268, 4(%esp)
	leal	-1560(%ebp), %eax
	movl	%eax, (%esp)
	call	_sprintf
	movl	_my_trace, %eax
	leal	67915424(%eax), %edx
	leal	-1560(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	%edx, (%esp)
	call	_strcpy
	movl	_my_trace, %eax
	movl	68964276(%eax), %eax
	movl	-16(%ebp), %edx
	movl	%edx, 12(%esp)
	movl	%eax, 8(%esp)
	movl	$LC269, 4(%esp)
	leal	-792(%ebp), %eax
	movl	%eax, (%esp)
	call	_sprintf
	leal	-792(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z13add_to_bufferPc
L639:
	movl	-12(%ebp), %eax
	movl	%eax, (%esp)
	call	_free
	movl	-16(%ebp), %eax
	movl	%eax, (%esp)
	call	_free
	movl	$0, %eax
L640:
	leave
	ret
	.globl	__Z13report_regionmm
	.def	__Z13report_regionmm;	.scl	2;	.type	32;	.endef
__Z13report_regionmm:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$824, %esp
	cmpl	$256, 12(%ebp)
	jbe	L642
	movl	$256, 12(%ebp)
L642:
	movl	12(%ebp), %eax
	addl	$1, %eax
	movl	%eax, (%esp)
	call	_malloc
	movl	%eax, -12(%ebp)
	movl	12(%ebp), %eax
	sall	$5, %eax
	movl	%eax, (%esp)
	call	_malloc
	movl	%eax, -16(%ebp)
	cmpl	$0, -12(%ebp)
	jne	L643
	leal	-792(%ebp), %eax
	movl	$1869771333, (%eax)
	movl	$539828338, 4(%eax)
	movl	$544501614, 8(%eax)
	movl	$1970237029, 12(%eax)
	movl	$1830840423, 16(%eax)
	movl	$1919905125, 20(%eax)
	movw	$121, 24(%eax)
	movl	_my_trace, %eax
	leal	67915424(%eax), %edx
	leal	-792(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	%edx, (%esp)
	call	_strcpy
	movl	$0, %eax
	jmp	L649
L643:
	cmpl	$0, -16(%ebp)
	jne	L645
	leal	-792(%ebp), %eax
	movl	$1869771333, (%eax)
	movl	$539828338, 4(%eax)
	movl	$544501614, 8(%eax)
	movl	$1970237029, 12(%eax)
	movl	$1830840423, 16(%eax)
	movl	$1919905125, 20(%eax)
	movw	$121, 24(%eax)
	movl	_my_trace, %eax
	leal	67915424(%eax), %edx
	leal	-792(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	%edx, (%esp)
	call	_strcpy
	movl	$0, %eax
	jmp	L649
L645:
	movl	_my_trace, %eax
	movl	88(%eax), %eax
	movl	%eax, 12(%esp)
	movl	8(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	12(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC265, (%esp)
	call	__Z7d_printPKcz
	movl	8(%ebp), %edx
	movl	_my_trace, %eax
	movl	88(%eax), %eax
	leal	-24(%ebp), %ecx
	movl	%ecx, 16(%esp)
	movl	12(%ebp), %ecx
	movl	%ecx, 12(%esp)
	movl	-12(%ebp), %ecx
	movl	%ecx, 8(%esp)
	movl	%edx, 4(%esp)
	movl	%eax, (%esp)
	call	__Z11read_memoryPvS_S_mPm
	movl	-24(%ebp), %eax
	cmpl	12(%ebp), %eax
	jne	L646
	movl	12(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	-16(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	-12(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z6hexifyPcS_j
	movl	%eax, -20(%ebp)
	cmpl	$0, -20(%ebp)
	je	L647
	leal	-792(%ebp), %eax
	movl	$1869771333, (%eax)
	movl	$1969496178, 4(%eax)
	movl	$1735289202, 8(%eax)
	movl	$2019911712, 12(%eax)
	movl	$7956073, 16(%eax)
	movl	_my_trace, %eax
	leal	67915424(%eax), %edx
	leal	-792(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	%edx, (%esp)
	call	_strcpy
	movl	$0, %eax
	jmp	L649
L647:
	movl	_my_trace, %eax
	addl	$67915424, %eax
	movw	$10, (%eax)
	movl	_my_trace, %eax
	leal	67915424(%eax), %edx
	movl	-16(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	%edx, (%esp)
	call	_strcat
	jmp	L648
L646:
	movl	__imp__GetLastError@0, %eax
	call	*%eax
	movl	%eax, 8(%esp)
	movl	$LC268, 4(%esp)
	leal	-792(%ebp), %eax
	movl	%eax, (%esp)
	call	_sprintf
	movl	_my_trace, %eax
	leal	67915424(%eax), %edx
	leal	-792(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	%edx, (%esp)
	call	_strcpy
L648:
	movl	-12(%ebp), %eax
	movl	%eax, (%esp)
	call	_free
	movl	-16(%ebp), %eax
	movl	%eax, (%esp)
	call	_free
	movl	$0, %eax
L649:
	leave
	ret
	.section .rdata,"dr"
	.align 4
LC270:
	.ascii "Trying to read: @ %p, handle: 0x%08x\12\0"
LC271:
	.ascii "%08x\0"
	.text
	.globl	__Z12report_dwordm
	.def	__Z12report_dwordm;	.scl	2;	.type	32;	.endef
__Z12report_dwordm:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$824, %esp
	movl	_my_trace, %eax
	movl	88(%eax), %eax
	movl	%eax, 8(%esp)
	movl	8(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC270, (%esp)
	call	__Z7d_printPKcz
	movl	8(%ebp), %edx
	movl	_my_trace, %eax
	movl	88(%eax), %eax
	leal	-16(%ebp), %ecx
	movl	%ecx, 16(%esp)
	movl	$4, 12(%esp)
	leal	-12(%ebp), %ecx
	movl	%ecx, 8(%esp)
	movl	%edx, 4(%esp)
	movl	%eax, (%esp)
	call	__Z11read_memoryPvS_S_mPm
	movl	-16(%ebp), %eax
	cmpl	$4, %eax
	jne	L651
	movl	-12(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	$LC271, 4(%esp)
	leal	-784(%ebp), %eax
	movl	%eax, (%esp)
	call	_sprintf
	movl	_my_trace, %eax
	leal	67915424(%eax), %edx
	leal	-784(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	%edx, (%esp)
	call	_strcpy
	jmp	L652
L651:
	movl	__imp__GetLastError@0, %eax
	call	*%eax
	movl	%eax, 8(%esp)
	movl	$LC268, 4(%esp)
	leal	-784(%ebp), %eax
	movl	%eax, (%esp)
	call	_sprintf
	movl	_my_trace, %eax
	leal	67915424(%eax), %edx
	leal	-784(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	%edx, (%esp)
	call	_strcpy
L652:
	movl	-12(%ebp), %eax
	leave
	ret
	.section .rdata,"dr"
LC272:
	.ascii "%s written\12\0"
	.text
	.globl	__Z14write_registermPcS_
	.def	__Z14write_registermPcS_;	.scl	2;	.type	32;	.endef
__Z14write_registermPcS_:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$1512, %esp
	leal	-1496(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z12read_contextmP8_CONTEXT
	movl	$LC28, 4(%esp)
	movl	12(%ebp), %eax
	movl	%eax, (%esp)
	call	_strcmp
	testl	%eax, %eax
	jne	L655
	movl	$16, 8(%esp)
	movl	$0, 4(%esp)
	movl	16(%ebp), %eax
	movl	%eax, (%esp)
	call	_strtoul
	movl	%eax, -12(%ebp)
	movl	-12(%ebp), %eax
	movl	%eax, -1320(%ebp)
	leal	-1496(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z13write_contextmP8_CONTEXT
	movl	_my_trace, %eax
	leal	67915424(%eax), %edx
	leal	-780(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	%edx, (%esp)
	call	_strcpy
	jmp	L656
L655:
	movl	$LC29, 4(%esp)
	movl	12(%ebp), %eax
	movl	%eax, (%esp)
	call	_strcmp
	testl	%eax, %eax
	jne	L657
	movl	$16, 8(%esp)
	movl	$0, 4(%esp)
	movl	16(%ebp), %eax
	movl	%eax, (%esp)
	call	_strtoul
	movl	%eax, -12(%ebp)
	movl	-12(%ebp), %eax
	movl	%eax, -1332(%ebp)
	leal	-1496(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z13write_contextmP8_CONTEXT
	movl	_my_trace, %eax
	leal	67915424(%eax), %edx
	leal	-780(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	%edx, (%esp)
	call	_strcpy
	jmp	L656
L657:
	movl	$LC30, 4(%esp)
	movl	12(%ebp), %eax
	movl	%eax, (%esp)
	call	_strcmp
	testl	%eax, %eax
	jne	L658
	movl	$16, 8(%esp)
	movl	$0, 4(%esp)
	movl	16(%ebp), %eax
	movl	%eax, (%esp)
	call	_strtoul
	movl	%eax, -12(%ebp)
	movl	-12(%ebp), %eax
	movl	%eax, -1324(%ebp)
	leal	-1496(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z13write_contextmP8_CONTEXT
	movl	_my_trace, %eax
	leal	67915424(%eax), %edx
	leal	-780(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	%edx, (%esp)
	call	_strcpy
	jmp	L656
L658:
	movl	$LC31, 4(%esp)
	movl	12(%ebp), %eax
	movl	%eax, (%esp)
	call	_strcmp
	testl	%eax, %eax
	jne	L659
	movl	$16, 8(%esp)
	movl	$0, 4(%esp)
	movl	16(%ebp), %eax
	movl	%eax, (%esp)
	call	_strtoul
	movl	%eax, -12(%ebp)
	movl	-12(%ebp), %eax
	movl	%eax, -1328(%ebp)
	leal	-1496(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z13write_contextmP8_CONTEXT
	movl	_my_trace, %eax
	leal	67915424(%eax), %edx
	leal	-780(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	%edx, (%esp)
	call	_strcpy
	jmp	L656
L659:
	movl	$LC33, 4(%esp)
	movl	12(%ebp), %eax
	movl	%eax, (%esp)
	call	_strcmp
	testl	%eax, %eax
	jne	L660
	movl	$16, 8(%esp)
	movl	$0, 4(%esp)
	movl	16(%ebp), %eax
	movl	%eax, (%esp)
	call	_strtoul
	movl	%eax, -12(%ebp)
	movl	-12(%ebp), %eax
	movl	%eax, -1336(%ebp)
	leal	-1496(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z13write_contextmP8_CONTEXT
	movl	_my_trace, %eax
	leal	67915424(%eax), %edx
	leal	-780(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	%edx, (%esp)
	call	_strcpy
	jmp	L656
L660:
	movl	$LC32, 4(%esp)
	movl	12(%ebp), %eax
	movl	%eax, (%esp)
	call	_strcmp
	testl	%eax, %eax
	jne	L661
	movl	$16, 8(%esp)
	movl	$0, 4(%esp)
	movl	16(%ebp), %eax
	movl	%eax, (%esp)
	call	_strtoul
	movl	%eax, -12(%ebp)
	movl	-12(%ebp), %eax
	movl	%eax, -1340(%ebp)
	leal	-1496(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z13write_contextmP8_CONTEXT
	movl	_my_trace, %eax
	leal	67915424(%eax), %edx
	leal	-780(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	%edx, (%esp)
	call	_strcpy
	jmp	L656
L661:
	movl	$LC209, 4(%esp)
	movl	12(%ebp), %eax
	movl	%eax, (%esp)
	call	_strcmp
	testl	%eax, %eax
	jne	L662
	movl	$16, 8(%esp)
	movl	$0, 4(%esp)
	movl	16(%ebp), %eax
	movl	%eax, (%esp)
	call	_strtoul
	movl	%eax, -12(%ebp)
	movl	-12(%ebp), %eax
	movl	%eax, -1316(%ebp)
	leal	-1496(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z13write_contextmP8_CONTEXT
	movl	_my_trace, %eax
	leal	67915424(%eax), %edx
	leal	-780(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	%edx, (%esp)
	call	_strcpy
	jmp	L656
L662:
	movl	$LC34, 4(%esp)
	movl	12(%ebp), %eax
	movl	%eax, (%esp)
	call	_strcmp
	testl	%eax, %eax
	jne	L663
	movl	$16, 8(%esp)
	movl	$0, 4(%esp)
	movl	16(%ebp), %eax
	movl	%eax, (%esp)
	call	_strtoul
	movl	%eax, -12(%ebp)
	movl	-12(%ebp), %eax
	movl	%eax, -1300(%ebp)
	leal	-1496(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z13write_contextmP8_CONTEXT
	movl	_my_trace, %eax
	leal	67915424(%eax), %edx
	leal	-780(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	%edx, (%esp)
	call	_strcpy
	jmp	L656
L663:
	movl	$LC35, 4(%esp)
	movl	12(%ebp), %eax
	movl	%eax, (%esp)
	call	_strcmp
	testl	%eax, %eax
	jne	L656
	movl	$16, 8(%esp)
	movl	$0, 4(%esp)
	movl	16(%ebp), %eax
	movl	%eax, (%esp)
	call	_strtoul
	movl	%eax, -12(%ebp)
	movl	-12(%ebp), %eax
	movl	%eax, -1312(%ebp)
	leal	-1496(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z13write_contextmP8_CONTEXT
	movl	_my_trace, %eax
	leal	67915424(%eax), %edx
	leal	-780(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	%edx, (%esp)
	call	_strcpy
L656:
	movl	_my_trace, %eax
	movl	$7, 67915420(%eax)
	movl	12(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	$LC272, 4(%esp)
	leal	-780(%ebp), %eax
	movl	%eax, (%esp)
	call	_sprintf
	movl	_my_trace, %eax
	leal	67915424(%eax), %edx
	leal	-780(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	%edx, (%esp)
	call	_strcpy
	movl	$0, %eax
	leave
	ret
	.section .rdata,"dr"
LC273:
	.ascii "Eip\0"
LC274:
	.ascii "EFLAGS\0"
LC275:
	.ascii "Error reading register %s\12\0"
	.text
	.globl	__Z13read_registermPc
	.def	__Z13read_registermPc;	.scl	2;	.type	32;	.endef
__Z13read_registermPc:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$744, %esp
	cmpl	$0, 8(%ebp)
	jne	L666
	movl	_my_trace, %eax
	movl	68964100(%eax), %eax
	movl	%eax, 8(%ebp)
L666:
	leal	-724(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z12read_contextmP8_CONTEXT
	movl	$LC28, 4(%esp)
	movl	12(%ebp), %eax
	movl	%eax, (%esp)
	call	_strcmp
	testl	%eax, %eax
	jne	L667
	movl	-548(%ebp), %eax
	jmp	L677
L667:
	movl	$LC29, 4(%esp)
	movl	12(%ebp), %eax
	movl	%eax, (%esp)
	call	_strcmp
	testl	%eax, %eax
	jne	L669
	movl	-560(%ebp), %eax
	jmp	L677
L669:
	movl	$LC30, 4(%esp)
	movl	12(%ebp), %eax
	movl	%eax, (%esp)
	call	_strcmp
	testl	%eax, %eax
	jne	L670
	movl	-552(%ebp), %eax
	jmp	L677
L670:
	movl	$LC31, 4(%esp)
	movl	12(%ebp), %eax
	movl	%eax, (%esp)
	call	_strcmp
	testl	%eax, %eax
	jne	L671
	movl	-556(%ebp), %eax
	jmp	L677
L671:
	movl	$LC32, 4(%esp)
	movl	12(%ebp), %eax
	movl	%eax, (%esp)
	call	_strcmp
	testl	%eax, %eax
	jne	L672
	movl	-568(%ebp), %eax
	jmp	L677
L672:
	movl	$LC209, 4(%esp)
	movl	12(%ebp), %eax
	movl	%eax, (%esp)
	call	_strcmp
	testl	%eax, %eax
	jne	L673
	movl	-544(%ebp), %eax
	jmp	L677
L673:
	movl	$LC34, 4(%esp)
	movl	12(%ebp), %eax
	movl	%eax, (%esp)
	call	_strcmp
	testl	%eax, %eax
	jne	L674
	movl	-528(%ebp), %eax
	jmp	L677
L674:
	movl	$LC273, 4(%esp)
	movl	12(%ebp), %eax
	movl	%eax, (%esp)
	call	_strcmp
	testl	%eax, %eax
	jne	L675
	movl	-540(%ebp), %eax
	jmp	L677
L675:
	movl	$LC274, 4(%esp)
	movl	12(%ebp), %eax
	movl	%eax, (%esp)
	call	_strcmp
	testl	%eax, %eax
	jne	L676
	movl	-532(%ebp), %eax
	jmp	L677
L676:
	movl	12(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC275, (%esp)
	call	__Z7d_printPKcz
	movl	$-1, %eax
L677:
	leave
	ret
	.globl	__Z15report_registermPc
	.def	__Z15report_registermPc;	.scl	2;	.type	32;	.endef
__Z15report_registermPc:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$808, %esp
	movl	_my_trace, %eax
	movl	$7, 67915420(%eax)
	movl	12(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z13read_registermPc
	movl	%eax, -12(%ebp)
	movl	-12(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	$LC271, 4(%esp)
	leal	-780(%ebp), %eax
	movl	%eax, (%esp)
	call	_sprintf
	movl	_my_trace, %eax
	leal	67915424(%eax), %edx
	leal	-780(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	%edx, (%esp)
	call	_strcpy
	movl	$0, %eax
	leave
	ret
	.section .rdata,"dr"
LC276:
	.ascii "0x%08x:0x%08x\0"
LC277:
	.ascii "new buffer: %p - %s\12\0"
	.text
	.globl	__Z10read_stackmm
	.def	__Z10read_stackmm;	.scl	2;	.type	32;	.endef
__Z10read_stackmm:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%edi
	pushl	%ebx
	subl	$1520, %esp
	movl	_my_trace, %eax
	movl	$7, 67915420(%eax)
	leal	-1504(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z12read_contextmP8_CONTEXT
	movl	-1308(%ebp), %eax
	movl	%eax, -12(%ebp)
	movl	_my_trace, %eax
	addl	$67915424, %eax
	movl	$1048576, 8(%esp)
	movl	$0, 4(%esp)
	movl	%eax, (%esp)
	call	_memset
	movl	$0, -16(%ebp)
	jmp	L681
L682:
	movl	-12(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z10read_dwordm
	movl	%eax, -20(%ebp)
	movl	$768, 8(%esp)
	movl	$0, 4(%esp)
	leal	-788(%ebp), %eax
	movl	%eax, (%esp)
	call	_memset
	movl	-20(%ebp), %eax
	movl	%eax, 12(%esp)
	movl	-12(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	$LC276, 4(%esp)
	leal	-788(%ebp), %eax
	movl	%eax, (%esp)
	call	_sprintf
	movl	_my_trace, %eax
	leal	67915424(%eax), %edx
	leal	-788(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	%edx, (%esp)
	call	_strcat
	movl	_my_trace, %eax
	leal	67915424(%eax), %edx
	movl	%edx, %eax
	movl	$-1, %ecx
	movl	%eax, %ebx
	movl	$0, %eax
	movl	%ebx, %edi
	repnz scasb
	movl	%ecx, %eax
	notl	%eax
	subl	$1, %eax
	addl	%edx, %eax
	movw	$10, (%eax)
	addl	$1, -16(%ebp)
	addl	$4, -12(%ebp)
L681:
	movl	-16(%ebp), %eax
	cmpl	12(%ebp), %eax
	jb	L682
	movl	_my_trace, %eax
	leal	67915424(%eax), %edx
	movl	_my_trace, %eax
	addl	$67915424, %eax
	movl	%edx, 8(%esp)
	movl	%eax, 4(%esp)
	movl	$LC277, (%esp)
	call	__Z7d_printPKcz
	movl	$0, %eax
	addl	$1520, %esp
	popl	%ebx
	popl	%edi
	popl	%ebp
	ret
	.section .rdata,"dr"
LC278:
	.ascii "RL,0x%08x,self\12\0"
	.text
	.globl	__Z13register_selfj
	.def	__Z13register_selfj;	.scl	2;	.type	32;	.endef
__Z13register_selfj:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$792, %esp
	movl	8(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC278, (%esp)
	call	__Z7d_printPKcz
	movl	8(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	$LC278, 4(%esp)
	leal	-776(%ebp), %eax
	movl	%eax, (%esp)
	call	_sprintf
	leal	-776(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z13add_to_bufferPc
	movl	$0, %eax
	leave
	ret
	.section .rdata,"dr"
	.align 4
LC279:
	.ascii "Created process, checking breakpoints\12\0"
	.align 4
LC280:
	.ascii "bp: %s, resolved_location: 0x%08x\12\0"
	.align 4
LC281:
	.ascii "Breakpoint hit! @ 0x%08x, instr_count: %d\12\0"
LC282:
	.ascii "[BP handling]\12\0"
LC283:
	.ascii "Handling breakpoint @ 0x%08x\12\0"
	.align 4
LC284:
	.ascii "This BP is not our, we pass it to the debugee\12\0"
LC285:
	.ascii "[BP handling ends]\12\0"
LC286:
	.ascii "other\12\0"
	.align 4
LC287:
	.ascii "Loaded library, checking breakpoints\12\0"
LC288:
	.ascii "Exiting thread\12\0"
LC289:
	.ascii "Exiting process\12\0"
	.text
	.globl	__Z18process_last_eventv
	.def	__Z18process_last_eventv;	.scl	2;	.type	32;	.endef
__Z18process_last_eventv:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%edi
	pushl	%esi
	pushl	%ebx
	subl	$140, %esp
	movl	_my_trace, %eax
	movl	_my_trace, %edx
	movl	68964100(%edx), %edx
	movl	%edx, 68964276(%eax)
	movl	_my_trace, %eax
	movl	68964092(%eax), %eax
	cmpl	$7, %eax
	ja	L687
	movl	L689(,%eax,4), %eax
	jmp	*%eax
	.section .rdata,"dr"
	.align 4
L689:
	.long	L687
	.long	L688
	.long	L690
	.long	L691
	.long	L692
	.long	L693
	.long	L694
	.long	L695
	.text
L691:
	movl	_my_trace, %eax
	movl	_my_trace, %edx
	movl	68964104(%edx), %ecx
	movl	%ecx, 84(%eax)
	movl	68964108(%edx), %ecx
	movl	%ecx, 88(%eax)
	movl	68964112(%edx), %ecx
	movl	%ecx, 92(%eax)
	movl	68964116(%edx), %ecx
	movl	%ecx, 96(%eax)
	movl	68964120(%edx), %ecx
	movl	%ecx, 100(%eax)
	movl	68964124(%edx), %ecx
	movl	%ecx, 104(%eax)
	movl	68964128(%edx), %ecx
	movl	%ecx, 108(%eax)
	movl	68964132(%edx), %ecx
	movl	%ecx, 112(%eax)
	movl	68964136(%edx), %ecx
	movl	%ecx, 116(%eax)
	movl	68964140(%edx), %edx
	movl	%edx, 120(%eax)
	movl	_my_trace, %eax
	movl	_my_trace, %edx
	movl	88(%edx), %edx
	movl	%edx, 128(%eax)
	movl	_my_trace, %eax
	movl	68964112(%eax), %edx
	movl	_my_trace, %eax
	movl	68964100(%eax), %eax
	movl	%edx, 4(%esp)
	movl	%eax, (%esp)
	call	__Z15register_threadmPv
	movl	_my_trace, %eax
	movl	96(%eax), %eax
	movl	%eax, (%esp)
	call	__Z13register_selfj
	movl	$LC279, (%esp)
	call	__Z7d_printPKcz
	movl	$0, -28(%ebp)
	jmp	L696
L698:
	movl	_my_trace, %edx
	movl	-28(%ebp), %eax
	imull	$532, %eax, %eax
	addl	%edx, %eax
	addl	$67643024, %eax
	movl	8(%eax), %eax
	movl	_my_trace, %edx
	movl	-28(%ebp), %ecx
	imull	$532, %ecx, %ecx
	addl	$67642512, %ecx
	addl	%ecx, %edx
	addl	$8, %edx
	movl	%eax, 8(%esp)
	movl	%edx, 4(%esp)
	movl	$LC280, (%esp)
	call	__Z7d_printPKcz
	movl	_my_trace, %edx
	movl	-28(%ebp), %eax
	imull	$532, %eax, %eax
	addl	%edx, %eax
	addl	$67643024, %eax
	movl	8(%eax), %eax
	cmpl	$-1, %eax
	jne	L697
	movl	_my_trace, %eax
	movl	-28(%ebp), %edx
	imull	$532, %edx, %edx
	addl	$67642512, %edx
	addl	%edx, %eax
	addl	$4, %eax
	movl	%eax, (%esp)
	call	__Z17update_breakpointP11BREAKPOINT_
L697:
	addl	$1, -28(%ebp)
L696:
	movl	-28(%ebp), %ecx
	movl	$0, %ebx
	movl	_my_trace, %eax
	movl	68964068(%eax), %edx
	movl	68964064(%eax), %eax
	cmpl	%edx, %ebx
	jb	L698
	cmpl	%edx, %ebx
	ja	L714
	cmpl	%eax, %ecx
	jb	L698
L714:
	movl	$1, %eax
	jmp	L686
L688:
	movl	$-2147418111, -40(%ebp)
	movl	_my_trace, %eax
	movl	_my_trace, %edx
	addl	$68964188, %eax
	addl	$68964104, %edx
	movl	$20, %ecx
	movl	%eax, %edi
	movl	%edx, %esi
	rep movsl
	movl	_my_trace, %eax
	movl	68964188(%eax), %eax
	cmpl	$-2147483645, %eax
	je	L702
	cmpl	$-2147483644, %eax
	jne	L717
	movl	_my_trace, %eax
	addl	$68964092, %eax
	movl	%eax, (%esp)
	call	__Z11ss_callbackPv
	movl	_my_trace, %eax
	movl	68964100(%eax), %eax
	movl	%eax, (%esp)
	call	__Z6set_ssm
	movl	_my_trace, %eax
	movl	$65538, 68964268(%eax)
	movl	$0, %eax
	jmp	L686
L702:
	movl	_my_trace, %eax
	movl	68964200(%eax), %eax
	movl	%eax, -44(%ebp)
	movl	_my_trace, %eax
	movl	68964020(%eax), %edx
	movl	68964016(%eax), %eax
	movl	%eax, 8(%esp)
	movl	%edx, 12(%esp)
	movl	-44(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC281, (%esp)
	call	__Z7d_printPKcz
	movl	$LC282, (%esp)
	call	__Z7d_printPKcz
	movb	$0, -33(%ebp)
	movl	_my_trace, %eax
	movl	$0, 67915420(%eax)
	movl	$0, -32(%ebp)
	jmp	L704
L706:
	movl	_my_trace, %edx
	movl	-32(%ebp), %eax
	imull	$532, %eax, %eax
	addl	%edx, %eax
	addl	$67643024, %eax
	movl	8(%eax), %eax
	cmpl	-44(%ebp), %eax
	jne	L705
	movl	-44(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC283, (%esp)
	call	__Z7d_printPKcz
	movl	_my_trace, %eax
	leal	68964092(%eax), %edx
	movl	_my_trace, %eax
	movl	68964200(%eax), %eax
	movl	%edx, 4(%esp)
	movl	%eax, (%esp)
	call	__Z17handle_breakpointmPv
	movb	$1, -33(%ebp)
L705:
	addl	$1, -32(%ebp)
L704:
	movl	-32(%ebp), %ecx
	movl	$0, %ebx
	movl	_my_trace, %eax
	movl	68964068(%eax), %edx
	movl	68964064(%eax), %eax
	cmpl	%edx, %ebx
	jb	L706
	cmpl	%edx, %ebx
	ja	L715
	cmpl	%eax, %ecx
	jb	L706
L715:
	cmpb	$0, -33(%ebp)
	jne	L708
	movl	$LC284, (%esp)
	call	__Z7d_printPKcz
	movl	_my_trace, %eax
	movl	$8, 67915420(%eax)
L708:
	movl	$LC285, (%esp)
	call	__Z7d_printPKcz
	movl	_my_trace, %eax
	movl	67915420(%eax), %eax
	jmp	L686
L717:
	movl	$LC286, (%esp)
	call	__Z7d_printPKcz
	movl	_my_trace, %edx
	movl	_my_trace, %eax
	movl	68964100(%eax), %ebx
	leal	4(%esp), %eax
	addl	$68964188, %edx
	movl	$20, %ecx
	movl	%eax, %edi
	movl	%edx, %esi
	rep movsl
	movl	%ebx, (%esp)
	call	__Z18register_exceptionm17_EXCEPTION_RECORD
	movl	_my_trace, %eax
	addl	$68964092, %eax
	movl	%eax, (%esp)
	call	__Z11ss_callbackPv
	movl	$6, %eax
	jmp	L686
L690:
	movl	_my_trace, %eax
	movl	68964104(%eax), %edx
	movl	_my_trace, %eax
	movl	68964100(%eax), %eax
	movl	%edx, 4(%esp)
	movl	%eax, (%esp)
	call	__Z15register_threadmPv
	movl	_my_trace, %eax
	movl	$65538, 68964268(%eax)
	movl	$0, %eax
	jmp	L686
L694:
	movl	_my_trace, %eax
	movl	68964104(%eax), %edx
	movl	%edx, (%esp)
	movl	68964108(%eax), %edx
	movl	%edx, 4(%esp)
	movl	68964112(%eax), %edx
	movl	%edx, 8(%esp)
	movl	68964116(%eax), %edx
	movl	%edx, 12(%esp)
	movl	68964120(%eax), %edx
	movl	%edx, 16(%esp)
	movl	68964124(%eax), %eax
	movl	%eax, 20(%esp)
	call	__Z12register_lib20_LOAD_DLL_DEBUG_INFO
	movl	$LC287, (%esp)
	call	__Z7d_printPKcz
	movl	$0, -28(%ebp)
	jmp	L709
L711:
	movl	_my_trace, %edx
	movl	-28(%ebp), %eax
	imull	$532, %eax, %eax
	addl	%edx, %eax
	addl	$67643024, %eax
	movl	8(%eax), %eax
	movl	_my_trace, %edx
	movl	-28(%ebp), %ecx
	imull	$532, %ecx, %ecx
	addl	$67642512, %ecx
	addl	%ecx, %edx
	addl	$8, %edx
	movl	%eax, 8(%esp)
	movl	%edx, 4(%esp)
	movl	$LC280, (%esp)
	call	__Z7d_printPKcz
	movl	_my_trace, %edx
	movl	-28(%ebp), %eax
	imull	$532, %eax, %eax
	addl	%edx, %eax
	addl	$67643024, %eax
	movl	8(%eax), %eax
	cmpl	$-1, %eax
	jne	L710
	movl	_my_trace, %eax
	movl	-28(%ebp), %edx
	imull	$532, %edx, %edx
	addl	$67642512, %edx
	addl	%edx, %eax
	addl	$4, %eax
	movl	%eax, (%esp)
	call	__Z17update_breakpointP11BREAKPOINT_
L710:
	addl	$1, -28(%ebp)
L709:
	movl	-28(%ebp), %ecx
	movl	$0, %ebx
	movl	_my_trace, %eax
	movl	68964068(%eax), %edx
	movl	68964064(%eax), %eax
	cmpl	%edx, %ebx
	jb	L711
	cmpl	%edx, %ebx
	ja	L716
	cmpl	%eax, %ecx
	jb	L711
L716:
	movl	_my_trace, %eax
	movl	$65538, 68964268(%eax)
	movl	$0, %eax
	jmp	L686
L695:
	movl	_my_trace, %eax
	movl	68964104(%eax), %eax
	movl	%eax, (%esp)
	call	__Z14deregister_lib22_UNLOAD_DLL_DEBUG_INFO
	movl	$0, %eax
	jmp	L686
L692:
	movl	$LC288, (%esp)
	call	__Z7d_printPKcz
	movl	_my_trace, %eax
	movl	68964104(%eax), %edx
	movl	_my_trace, %eax
	movl	68964100(%eax), %eax
	movl	%edx, 4(%esp)
	movl	%eax, (%esp)
	call	__Z17deregister_threadmPv
	movl	_my_trace, %ecx
	movl	_my_trace, %eax
	movl	68964100(%eax), %edx
	movl	%edx, %eax
	sall	$2, %eax
	addl	%edx, %eax
	sall	$2, %eax
	addl	%ecx, %eax
	addl	$67108992, %eax
	movb	$0, 12(%eax)
	movl	_my_trace, %ecx
	movl	_my_trace, %eax
	movl	68964100(%eax), %edx
	movl	%edx, %eax
	sall	$2, %eax
	addl	%edx, %eax
	sall	$2, %eax
	addl	%ecx, %eax
	addl	$67108992, %eax
	movl	16(%eax), %eax
	testl	%eax, %eax
	je	L713
	movl	_my_trace, %ecx
	movl	_my_trace, %eax
	movl	68964100(%eax), %edx
	movl	%edx, %eax
	sall	$2, %eax
	addl	%edx, %eax
	sall	$2, %eax
	addl	%ecx, %eax
	addl	$67108992, %eax
	movl	16(%eax), %eax
	movl	%eax, (%esp)
	movl	__imp__CloseHandle@4, %eax
	call	*%eax
	subl	$4, %esp
L713:
	movl	_my_trace, %ecx
	movl	_my_trace, %eax
	movl	68964100(%eax), %edx
	movl	%edx, %eax
	sall	$2, %eax
	addl	%edx, %eax
	sall	$2, %eax
	addl	%ecx, %eax
	addl	$67108992, %eax
	movl	$0, 16(%eax)
	movl	_my_trace, %ecx
	movl	_my_trace, %eax
	movl	68964100(%eax), %edx
	movl	%edx, %eax
	sall	$2, %eax
	addl	%edx, %eax
	sall	$2, %eax
	addl	%ecx, %eax
	addl	$67108992, %eax
	movb	$0, 13(%eax)
	movl	_my_trace, %eax
	movl	$65538, 68964268(%eax)
	movl	$0, %eax
	jmp	L686
L693:
	movl	$LC289, (%esp)
	call	__Z7d_printPKcz
	movl	$2, %eax
	jmp	L686
L687:
L686:
	leal	-12(%ebp), %esp
	popl	%ebx
	popl	%esi
	popl	%edi
	popl	%ebp
	ret
	.section .rdata,"dr"
LC290:
	.ascii "Exception: 0x%08x\12\0"
LC291:
	.ascii "at: 0x%08x\12\0"
LC292:
	.ascii "First chance: 0x%08x\12\0"
LC293:
	.ascii "instr: %d\12\0"
	.text
	.globl	__Z13create_reporti
	.def	__Z13create_reporti;	.scl	2;	.type	32;	.endef
__Z13create_reporti:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%edi
	pushl	%esi
	subl	$1632, %esp
	movl	8(%ebp), %eax
	cmpl	$2, %eax
	je	L720
	cmpl	$2, %eax
	jg	L721
	cmpl	$1, %eax
	je	L722
	jmp	L719
L721:
	cmpl	$5, %eax
	je	L723
	cmpl	$6, %eax
	je	L724
	jmp	L719
L722:
	leal	-1544(%ebp), %eax
	movl	$1330660690, (%eax)
	movl	$1348424786, 4(%eax)
	movl	$1162039122, 8(%eax)
	movl	$1130320723, 12(%eax)
	movl	$1413563730, 16(%eax)
	movl	$672837, 20(%eax)
	jmp	L725
L723:
	movl	_my_trace, %eax
	movl	68964280(%eax), %eax
	movl	%eax, 8(%esp)
	movl	$LC196, 4(%esp)
	leal	-1544(%ebp), %eax
	movl	%eax, (%esp)
	call	_sprintf
	jmp	L725
L724:
	movl	_my_trace, %edx
	leal	-1624(%ebp), %eax
	addl	$68964104, %edx
	movl	$20, %ecx
	movl	%eax, %edi
	movl	%edx, %esi
	rep movsl
	leal	-1544(%ebp), %eax
	movl	$1330660690, (%eax)
	movl	$1163875410, 4(%eax)
	movl	$1346716504, 8(%eax)
	movl	$1313818964, 12(%eax)
	movw	$10, 16(%eax)
	movl	-1624(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	$LC290, 4(%esp)
	leal	-776(%ebp), %eax
	movl	%eax, (%esp)
	call	_sprintf
	leal	-776(%ebp), %eax
	movl	%eax, 4(%esp)
	leal	-1544(%ebp), %eax
	movl	%eax, (%esp)
	call	_strcat
	movl	-1612(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	$LC291, 4(%esp)
	leal	-776(%ebp), %eax
	movl	%eax, (%esp)
	call	_sprintf
	leal	-776(%ebp), %eax
	movl	%eax, 4(%esp)
	leal	-1544(%ebp), %eax
	movl	%eax, (%esp)
	call	_strcat
	movl	_my_trace, %eax
	movl	68964184(%eax), %eax
	movl	%eax, 8(%esp)
	movl	$LC292, 4(%esp)
	leal	-776(%ebp), %eax
	movl	%eax, (%esp)
	call	_sprintf
	leal	-776(%ebp), %eax
	movl	%eax, 4(%esp)
	leal	-1544(%ebp), %eax
	movl	%eax, (%esp)
	call	_strcat
	jmp	L725
L720:
	leal	-1544(%ebp), %eax
	movl	$1330660690, (%eax)
	movl	$1348424786, 4(%eax)
	movl	$1162039122, 8(%eax)
	movl	$1163875155, 12(%eax)
	movl	$173295960, 16(%eax)
	movb	$0, 20(%eax)
	jmp	L725
L719:
	leal	-1544(%ebp), %eax
	movl	$1330660690, (%eax)
	movl	$1331647570, 4(%eax)
	movl	$1380272212, 8(%eax)
	movw	$10, 12(%eax)
	movl	_my_trace, %eax
	movl	68964272(%eax), %eax
	movl	%eax, 8(%esp)
	movl	$LC291, 4(%esp)
	leal	-776(%ebp), %eax
	movl	%eax, (%esp)
	call	_sprintf
	leal	-776(%ebp), %eax
	movl	%eax, 4(%esp)
	leal	-1544(%ebp), %eax
	movl	%eax, (%esp)
	call	_strcat
	movl	_my_trace, %eax
	movl	68964020(%eax), %edx
	movl	68964016(%eax), %eax
	movl	%eax, 8(%esp)
	movl	%edx, 12(%esp)
	movl	$LC293, 4(%esp)
	leal	-776(%ebp), %eax
	movl	%eax, (%esp)
	call	_sprintf
	leal	-776(%ebp), %eax
	movl	%eax, 4(%esp)
	leal	-1544(%ebp), %eax
	movl	%eax, (%esp)
	call	_strcat
	nop
L725:
	movl	_my_trace, %eax
	leal	67915424(%eax), %edx
	leal	-1544(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	%edx, (%esp)
	call	_strcpy
	movl	$0, %eax
	addl	$1632, %esp
	popl	%esi
	popl	%edi
	popl	%ebp
	ret
	.globl	__Z15handle_continuemmj
	.def	__Z15handle_continuemmj;	.scl	2;	.type	32;	.endef
__Z15handle_continuemmj:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$24, %esp
	movl	16(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	12(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	movl	__imp__ContinueDebugEvent@12, %eax
	call	*%eax
	subl	$12, %esp
	movl	$0, %eax
	leave
	ret
	.section .rdata,"dr"
	.align 4
LC294:
	.ascii "Continuing: PID: 0x%08x, TID: 0x%08x\12\0"
LC295:
	.ascii "Continuing\12\0"
LC296:
	.ascii "Report buffer: %s\12\0"
	.text
	.globl	__Z18get_pending_eventsv
	.def	__Z18get_pending_eventsv;	.scl	2;	.type	32;	.endef
__Z18get_pending_eventsv:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$808, %esp
	movl	_my_trace, %eax
	movl	68964100(%eax), %edx
	movl	_my_trace, %eax
	movl	68964096(%eax), %eax
	movl	%edx, 8(%esp)
	movl	%eax, 4(%esp)
	movl	$LC294, (%esp)
	call	__Z7d_printPKcz
	movl	_my_trace, %eax
	movl	68964096(%eax), %eax
	testl	%eax, %eax
	jne	L730
	movl	_my_trace, %eax
	movl	12(%eax), %edx
	movl	_my_trace, %eax
	movl	8(%eax), %eax
	movl	$65538, 8(%esp)
	movl	%edx, 4(%esp)
	movl	%eax, (%esp)
	call	__Z15handle_continuemmj
	jmp	L731
L730:
	movl	_my_trace, %eax
	movl	68964100(%eax), %edx
	movl	_my_trace, %eax
	movl	68964096(%eax), %eax
	movl	$65538, 8(%esp)
	movl	%edx, 4(%esp)
	movl	%eax, (%esp)
	call	__Z15handle_continuemmj
L731:
	movl	$LC295, (%esp)
	call	__Z7d_printPKcz
L738:
	movl	_my_trace, %eax
	addl	$68964092, %eax
	movl	$0, 4(%esp)
	movl	%eax, (%esp)
	movl	__imp__WaitForDebugEvent@8, %eax
	call	*%eax
	subl	$8, %esp
	movl	_my_trace, %eax
	movl	68964092(%eax), %eax
	testl	%eax, %eax
	jne	L732
	movl	$153, %eax
	jmp	L739
L732:
	call	__Z18process_last_eventv
	movl	%eax, -16(%ebp)
	cmpl	$0, -16(%ebp)
	jne	L734
	movl	$65538, -12(%ebp)
	jmp	L735
L734:
	cmpl	$8, -16(%ebp)
	jne	L736
	movl	$-2147418111, -12(%ebp)
	jmp	L735
L736:
	nop
	movl	-16(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z13create_reporti
	movl	_my_trace, %eax
	addl	$67915424, %eax
	movl	%eax, 4(%esp)
	movl	$LC296, (%esp)
	call	__Z7d_printPKcz
	movl	_my_trace, %eax
	movl	-16(%ebp), %edx
	movl	%edx, 67915420(%eax)
	movl	-16(%ebp), %eax
	jmp	L739
L735:
	movl	_my_trace, %eax
	movl	68964100(%eax), %edx
	movl	_my_trace, %eax
	movl	68964096(%eax), %eax
	movl	-12(%ebp), %ecx
	movl	%ecx, 8(%esp)
	movl	%edx, 4(%esp)
	movl	%eax, (%esp)
	call	__Z15handle_continuemmj
	jmp	L738
L739:
	leave
	ret
	.globl	__Z16continue_routinemj
	.def	__Z16continue_routinemj;	.scl	2;	.type	32;	.endef
__Z16continue_routinemj:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$808, %esp
	movl	12(%ebp), %eax
	movl	%eax, -12(%ebp)
	movl	_my_trace, %eax
	movl	68964096(%eax), %eax
	testl	%eax, %eax
	jne	L741
	movl	_my_trace, %eax
	movl	12(%eax), %edx
	movl	_my_trace, %eax
	movl	8(%eax), %eax
	movl	-12(%ebp), %ecx
	movl	%ecx, 8(%esp)
	movl	%edx, 4(%esp)
	movl	%eax, (%esp)
	call	__Z15handle_continuemmj
	jmp	L742
L741:
	movl	_my_trace, %eax
	movl	68964100(%eax), %edx
	movl	_my_trace, %eax
	movl	68964096(%eax), %eax
	movl	-12(%ebp), %ecx
	movl	%ecx, 8(%esp)
	movl	%edx, 4(%esp)
	movl	%eax, (%esp)
	call	__Z15handle_continuemmj
L742:
	movl	$LC295, (%esp)
	call	__Z7d_printPKcz
L747:
	movl	_my_trace, %eax
	leal	68964092(%eax), %edx
	movl	8(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	%edx, (%esp)
	movl	__imp__WaitForDebugEvent@8, %eax
	call	*%eax
	subl	$8, %esp
	call	__Z18process_last_eventv
	movl	%eax, -16(%ebp)
	cmpl	$0, -16(%ebp)
	jne	L743
	movl	$65538, -12(%ebp)
	jmp	L744
L743:
	cmpl	$8, -16(%ebp)
	jne	L745
	movl	$-2147418111, -12(%ebp)
	jmp	L744
L745:
	jmp	L749
L744:
	movl	_my_trace, %eax
	movl	68964100(%eax), %edx
	movl	_my_trace, %eax
	movl	68964096(%eax), %eax
	movl	-12(%ebp), %ecx
	movl	%ecx, 8(%esp)
	movl	%edx, 4(%esp)
	movl	%eax, (%esp)
	call	__Z15handle_continuemmj
	jmp	L747
L749:
	movl	-16(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z13create_reporti
	movl	_my_trace, %eax
	addl	$67915424, %eax
	movl	%eax, 4(%esp)
	movl	$LC296, (%esp)
	call	__Z7d_printPKcz
	movl	_my_trace, %eax
	movl	-16(%ebp), %edx
	movl	%edx, 67915420(%eax)
	movl	-16(%ebp), %eax
	leave
	ret
	.section .rdata,"dr"
LC297:
	.ascii "Resolving region\12\0"
LC298:
	.ascii "Locating buffer\12\0"
LC299:
	.ascii "LOCATION_CONST\12\0"
LC300:
	.ascii "Arg off: 0x%08x\12\0"
LC301:
	.ascii "LOCATION_MEM\12\0"
LC302:
	.ascii "0x%08x: 0x%08x\12\0"
LC303:
	.ascii "LOCATION_STACK\12\0"
LC304:
	.ascii "LOCATION_ADDR_STACK\12\0"
LC305:
	.ascii "LOCATION_ADDR_ADDR_STACK\12\0"
LC306:
	.ascii "Locating size\12\0"
LC307:
	.ascii "Arg size: 0x%08x\12\0"
	.align 4
LC308:
	.ascii "[Resolved location: 0x%08x, size: 0x%08x]\12\0"
	.text
	.globl	__Z14resolve_regionP20LOCATION_DESCRIPTOR_P9LOCATION_
	.def	__Z14resolve_regionP20LOCATION_DESCRIPTOR_P9LOCATION_;	.scl	2;	.type	32;	.endef
__Z14resolve_regionP20LOCATION_DESCRIPTOR_P9LOCATION_:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$792, %esp
	movl	$LC297, (%esp)
	call	__Z7d_printPKcz
	movl	$LC298, (%esp)
	call	__Z7d_printPKcz
	movl	_my_trace, %eax
	movl	68964100(%eax), %eax
	leal	-748(%ebp), %edx
	movl	%edx, 4(%esp)
	movl	%eax, (%esp)
	call	__Z12read_contextmP8_CONTEXT
	movl	-552(%ebp), %eax
	movl	%eax, -12(%ebp)
	movl	$0, -28(%ebp)
	movl	$0, -16(%ebp)
	movl	$0, -20(%ebp)
	movl	$0, -24(%ebp)
	movl	-12(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC41, (%esp)
	call	__Z7d_printPKcz
	movl	8(%ebp), %eax
	movzbl	8(%eax), %eax
	movsbl	%al, %eax
	cmpl	$5, %eax
	ja	L751
	movl	L753(,%eax,4), %eax
	jmp	*%eax
	.section .rdata,"dr"
	.align 4
L753:
	.long	L752
	.long	L754
	.long	L755
	.long	L756
	.long	L751
	.long	L757
	.text
L752:
	movl	$LC299, (%esp)
	call	__Z7d_printPKcz
	movl	8(%ebp), %eax
	movl	(%eax), %edx
	movl	12(%ebp), %eax
	movl	%edx, (%eax)
	movl	12(%ebp), %eax
	movl	(%eax), %eax
	movl	%eax, 4(%esp)
	movl	$LC300, (%esp)
	call	__Z7d_printPKcz
	movl	12(%ebp), %eax
	movl	(%eax), %eax
	movl	%eax, -28(%ebp)
	jmp	L751
L757:
	movl	$LC301, (%esp)
	call	__Z7d_printPKcz
	movl	8(%ebp), %eax
	movl	(%eax), %eax
	movl	%eax, -16(%ebp)
	movl	-16(%ebp), %edx
	movl	_my_trace, %eax
	movl	88(%eax), %eax
	leal	-32(%ebp), %ecx
	movl	%ecx, 16(%esp)
	movl	$4, 12(%esp)
	leal	-28(%ebp), %ecx
	movl	%ecx, 8(%esp)
	movl	%edx, 4(%esp)
	movl	%eax, (%esp)
	call	__Z11read_memoryPvS_S_mPm
	movl	-28(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	-16(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC302, (%esp)
	call	__Z7d_printPKcz
	movl	-28(%ebp), %eax
	testl	%eax, %eax
	jne	L758
	jmp	L751
L758:
	movl	-28(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC300, (%esp)
	call	__Z7d_printPKcz
	movl	-28(%ebp), %edx
	movl	12(%ebp), %eax
	movl	%edx, (%eax)
	jmp	L751
L754:
	movl	$LC303, (%esp)
	call	__Z7d_printPKcz
	movl	-12(%ebp), %eax
	addl	$8, %eax
	movl	%eax, -16(%ebp)
	movl	8(%ebp), %eax
	movl	(%eax), %eax
	sall	$2, %eax
	addl	%eax, -16(%ebp)
	movl	-16(%ebp), %eax
	movl	%eax, -28(%ebp)
	movl	-28(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	-16(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC302, (%esp)
	call	__Z7d_printPKcz
	movl	-28(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC300, (%esp)
	call	__Z7d_printPKcz
	movl	-28(%ebp), %eax
	testl	%eax, %eax
	jne	L759
	jmp	L751
L759:
	movl	-28(%ebp), %edx
	movl	12(%ebp), %eax
	movl	%edx, (%eax)
	jmp	L751
L755:
	movl	$LC304, (%esp)
	call	__Z7d_printPKcz
	movl	-12(%ebp), %eax
	addl	$8, %eax
	movl	%eax, -16(%ebp)
	movl	8(%ebp), %eax
	movl	(%eax), %eax
	sall	$2, %eax
	addl	%eax, -16(%ebp)
	movl	-16(%ebp), %edx
	movl	_my_trace, %eax
	movl	88(%eax), %eax
	leal	-32(%ebp), %ecx
	movl	%ecx, 16(%esp)
	movl	$4, 12(%esp)
	leal	-28(%ebp), %ecx
	movl	%ecx, 8(%esp)
	movl	%edx, 4(%esp)
	movl	%eax, (%esp)
	call	__Z11read_memoryPvS_S_mPm
	movl	-28(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	-16(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC302, (%esp)
	call	__Z7d_printPKcz
	movl	-28(%ebp), %eax
	testl	%eax, %eax
	jne	L760
	jmp	L751
L760:
	movl	-28(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC300, (%esp)
	call	__Z7d_printPKcz
	movl	-28(%ebp), %edx
	movl	12(%ebp), %eax
	movl	%edx, (%eax)
	jmp	L751
L756:
	movl	$LC305, (%esp)
	call	__Z7d_printPKcz
	movl	-12(%ebp), %eax
	addl	$8, %eax
	movl	%eax, -16(%ebp)
	movl	8(%ebp), %eax
	movl	(%eax), %eax
	sall	$2, %eax
	addl	%eax, -16(%ebp)
	movl	-16(%ebp), %edx
	movl	_my_trace, %eax
	movl	88(%eax), %eax
	leal	-32(%ebp), %ecx
	movl	%ecx, 16(%esp)
	movl	$4, 12(%esp)
	leal	-28(%ebp), %ecx
	movl	%ecx, 8(%esp)
	movl	%edx, 4(%esp)
	movl	%eax, (%esp)
	call	__Z11read_memoryPvS_S_mPm
	movl	-28(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	-16(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC302, (%esp)
	call	__Z7d_printPKcz
	movl	-28(%ebp), %eax
	movl	%eax, -16(%ebp)
	movl	-16(%ebp), %edx
	movl	_my_trace, %eax
	movl	88(%eax), %eax
	leal	-32(%ebp), %ecx
	movl	%ecx, 16(%esp)
	movl	$4, 12(%esp)
	leal	-28(%ebp), %ecx
	movl	%ecx, 8(%esp)
	movl	%edx, 4(%esp)
	movl	%eax, (%esp)
	call	__Z11read_memoryPvS_S_mPm
	movl	-28(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	-16(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC302, (%esp)
	call	__Z7d_printPKcz
	movl	-28(%ebp), %eax
	testl	%eax, %eax
	jne	L761
	jmp	L751
L761:
	movl	-28(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC300, (%esp)
	call	__Z7d_printPKcz
	movl	-28(%ebp), %edx
	movl	12(%ebp), %eax
	movl	%edx, (%eax)
	nop
L751:
	movl	-28(%ebp), %eax
	testl	%eax, %eax
	jne	L762
	movl	$-1, %eax
	jmp	L770
L762:
	movl	-28(%ebp), %eax
	movl	%eax, -20(%ebp)
	movl	$0, -28(%ebp)
	movl	$0, -16(%ebp)
	movl	$LC306, (%esp)
	call	__Z7d_printPKcz
	movl	8(%ebp), %eax
	movzbl	9(%eax), %eax
	movsbl	%al, %eax
	cmpl	$1, %eax
	je	L765
	cmpl	$1, %eax
	jg	L766
	testl	%eax, %eax
	je	L767
	jmp	L764
L766:
	cmpl	$2, %eax
	je	L768
	cmpl	$5, %eax
	je	L769
	jmp	L764
L767:
	movl	$LC299, (%esp)
	call	__Z7d_printPKcz
	movl	8(%ebp), %eax
	movl	4(%eax), %edx
	movl	12(%ebp), %eax
	movl	%edx, 4(%eax)
	movl	8(%ebp), %eax
	movl	4(%eax), %eax
	movl	%eax, -28(%ebp)
	movl	-28(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC307, (%esp)
	call	__Z7d_printPKcz
	jmp	L764
L769:
	movl	$LC301, (%esp)
	call	__Z7d_printPKcz
	movl	8(%ebp), %eax
	movl	4(%eax), %eax
	movl	%eax, -16(%ebp)
	movl	-16(%ebp), %edx
	movl	_my_trace, %eax
	movl	88(%eax), %eax
	leal	-32(%ebp), %ecx
	movl	%ecx, 16(%esp)
	movl	$4, 12(%esp)
	leal	-28(%ebp), %ecx
	movl	%ecx, 8(%esp)
	movl	%edx, 4(%esp)
	movl	%eax, (%esp)
	call	__Z11read_memoryPvS_S_mPm
	movl	-28(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	-16(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC302, (%esp)
	call	__Z7d_printPKcz
	movl	-28(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC307, (%esp)
	call	__Z7d_printPKcz
	movl	-28(%ebp), %edx
	movl	12(%ebp), %eax
	movl	%edx, 4(%eax)
	jmp	L764
L765:
	movl	$LC303, (%esp)
	call	__Z7d_printPKcz
	movl	-12(%ebp), %eax
	addl	$8, %eax
	movl	%eax, -16(%ebp)
	movl	8(%ebp), %eax
	movl	4(%eax), %eax
	sall	$2, %eax
	addl	%eax, -16(%ebp)
	movl	-16(%ebp), %edx
	movl	_my_trace, %eax
	movl	88(%eax), %eax
	leal	-32(%ebp), %ecx
	movl	%ecx, 16(%esp)
	movl	$4, 12(%esp)
	leal	-28(%ebp), %ecx
	movl	%ecx, 8(%esp)
	movl	%edx, 4(%esp)
	movl	%eax, (%esp)
	call	__Z11read_memoryPvS_S_mPm
	movl	-28(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	-16(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC302, (%esp)
	call	__Z7d_printPKcz
	movl	-28(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC307, (%esp)
	call	__Z7d_printPKcz
	movl	-28(%ebp), %edx
	movl	12(%ebp), %eax
	movl	%edx, 4(%eax)
	jmp	L764
L768:
	movl	$LC304, (%esp)
	call	__Z7d_printPKcz
	movl	-12(%ebp), %eax
	addl	$8, %eax
	movl	%eax, -16(%ebp)
	movl	8(%ebp), %eax
	movl	4(%eax), %eax
	sall	$2, %eax
	addl	%eax, -16(%ebp)
	movl	-16(%ebp), %edx
	movl	_my_trace, %eax
	movl	88(%eax), %eax
	leal	-32(%ebp), %ecx
	movl	%ecx, 16(%esp)
	movl	$4, 12(%esp)
	leal	-28(%ebp), %ecx
	movl	%ecx, 8(%esp)
	movl	%edx, 4(%esp)
	movl	%eax, (%esp)
	call	__Z11read_memoryPvS_S_mPm
	movl	-28(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	-16(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC302, (%esp)
	call	__Z7d_printPKcz
	movl	-28(%ebp), %eax
	movl	%eax, -16(%ebp)
	movl	-16(%ebp), %edx
	movl	_my_trace, %eax
	movl	88(%eax), %eax
	leal	-32(%ebp), %ecx
	movl	%ecx, 16(%esp)
	movl	$4, 12(%esp)
	leal	-28(%ebp), %ecx
	movl	%ecx, 8(%esp)
	movl	%edx, 4(%esp)
	movl	%eax, (%esp)
	call	__Z11read_memoryPvS_S_mPm
	movl	-28(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	-16(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC302, (%esp)
	call	__Z7d_printPKcz
	movl	-28(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC307, (%esp)
	call	__Z7d_printPKcz
	movl	-28(%ebp), %edx
	movl	12(%ebp), %eax
	movl	%edx, 4(%eax)
	nop
L764:
	movl	-28(%ebp), %eax
	movl	%eax, -24(%ebp)
	movl	-24(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	-20(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC308, (%esp)
	call	__Z7d_printPKcz
	movl	$0, %eax
L770:
	leave
	ret
	.section .rdata,"dr"
LC309:
	.ascii "Registered region id 0x%08x\12\0"
	.text
	.globl	__Z14add_region_selmmcc
	.def	__Z14add_region_selmmcc;	.scl	2;	.type	32;	.endef
__Z14add_region_selmmcc:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$56, %esp
	movl	16(%ebp), %edx
	movl	20(%ebp), %eax
	movb	%dl, -28(%ebp)
	movb	%al, -32(%ebp)
	movl	_my_trace, %eax
	movl	70025488(%eax), %eax
	movl	%eax, -12(%ebp)
	movl	_my_trace, %eax
	movl	-12(%ebp), %edx
	addl	$4376561, %edx
	sall	$4, %edx
	addl	%eax, %edx
	movl	8(%ebp), %eax
	movl	%eax, (%edx)
	movl	_my_trace, %eax
	movl	-12(%ebp), %edx
	addl	$4376561, %edx
	sall	$4, %edx
	addl	%eax, %edx
	movl	12(%ebp), %eax
	movl	%eax, 4(%edx)
	movl	_my_trace, %eax
	movl	-12(%ebp), %edx
	addl	$4376561, %edx
	sall	$4, %edx
	addl	%eax, %edx
	movzbl	-28(%ebp), %eax
	movb	%al, 8(%edx)
	movl	_my_trace, %eax
	movl	-12(%ebp), %edx
	addl	$4376561, %edx
	sall	$4, %edx
	addl	%eax, %edx
	movzbl	-32(%ebp), %eax
	movb	%al, 9(%edx)
	movl	-12(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC309, (%esp)
	call	__Z7d_printPKcz
	addl	$1, -12(%ebp)
	movl	_my_trace, %eax
	movl	-12(%ebp), %edx
	movl	%edx, 70025488(%eax)
	movl	$0, %eax
	leave
	ret
	.section .rdata,"dr"
LC310:
	.ascii "xx\0"
	.align 4
LC311:
	.ascii "Found reaction dor id: %s - %s\12\0"
	.text
	.globl	__Z13find_reactionPc
	.def	__Z13find_reactionPc;	.scl	2;	.type	32;	.endef
__Z13find_reactionPc:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$40, %esp
	movl	8(%ebp), %eax
	addl	$2, %eax
	movb	$0, (%eax)
	movl	$0, -12(%ebp)
	jmp	L774
L777:
	movl	_my_trace, %eax
	movl	70021896(%eax), %edx
	movl	-12(%ebp), %eax
	imull	$188, %eax, %eax
	addl	%edx, %eax
	movl	%eax, %edx
	movl	8(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	%edx, (%esp)
	call	_strcmp
	testl	%eax, %eax
	jne	L775
	movl	_my_trace, %eax
	movl	70021896(%eax), %edx
	movl	-12(%ebp), %eax
	imull	$188, %eax, %eax
	addl	%edx, %eax
	movl	$LC310, 4(%esp)
	movl	%eax, (%esp)
	call	_strcmp
	testl	%eax, %eax
	je	L775
	movl	_my_trace, %eax
	movl	70021896(%eax), %edx
	movl	-12(%ebp), %eax
	imull	$188, %eax, %eax
	addl	%edx, %eax
	movl	%eax, %edx
	movl	8(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	%edx, 4(%esp)
	movl	$LC311, (%esp)
	call	__Z7d_printPKcz
	movl	_my_trace, %eax
	movl	70021896(%eax), %edx
	movl	-12(%ebp), %eax
	imull	$188, %eax, %eax
	addl	%edx, %eax
	jmp	L776
L775:
	addl	$1, -12(%ebp)
L774:
	movl	_my_trace, %eax
	movl	70021900(%eax), %eax
	cmpl	-12(%ebp), %eax
	ja	L777
	movl	$0, %eax
L776:
	leave
	ret
	.section .rdata,"dr"
LC312:
	.ascii "[add_couple]\12\0"
LC313:
	.ascii "Added couple: %s\12\0"
LC314:
	.ascii "[add_couple ends]\12\0"
	.text
	.globl	__Z10add_couplePcS_
	.def	__Z10add_couplePcS_;	.scl	2;	.type	32;	.endef
__Z10add_couplePcS_:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$40, %esp
	movl	$LC312, (%esp)
	call	__Z7d_printPKcz
	movl	12(%ebp), %eax
	movzbl	(%eax), %eax
	movb	%al, -15(%ebp)
	movl	12(%ebp), %eax
	movzbl	1(%eax), %eax
	movb	%al, -14(%ebp)
	movb	$0, -13(%ebp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z13find_reactionPc
	movl	%eax, -12(%ebp)
	movl	-12(%ebp), %eax
	movl	100(%eax), %edx
	movl	%edx, %eax
	addl	%eax, %eax
	addl	%edx, %eax
	movl	-12(%ebp), %edx
	addl	%edx, %eax
	leal	3(%eax), %edx
	movl	12(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	%edx, (%esp)
	call	_strcpy
	movl	-12(%ebp), %eax
	movl	100(%eax), %eax
	leal	1(%eax), %edx
	movl	-12(%ebp), %eax
	movl	%edx, 100(%eax)
	leal	-15(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC313, (%esp)
	call	__Z7d_printPKcz
	movl	$LC314, (%esp)
	call	__Z7d_printPKcz
	movl	$0, %eax
	leave
	ret
	.section .rdata,"dr"
LC315:
	.ascii "[add_rid]\12\0"
LC316:
	.ascii "Added rid: 0x%02x\12\0"
LC317:
	.ascii "[add_rid ends]\12\0"
	.text
	.globl	__Z7add_ridPcj
	.def	__Z7add_ridPcj;	.scl	2;	.type	32;	.endef
__Z7add_ridPcj:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$40, %esp
	movl	$LC315, (%esp)
	call	__Z7d_printPKcz
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z13find_reactionPc
	movl	%eax, -12(%ebp)
	movl	-12(%ebp), %eax
	movl	168(%eax), %edx
	movl	-12(%ebp), %eax
	leal	24(%edx), %ecx
	movl	12(%ebp), %edx
	movl	%edx, 8(%eax,%ecx,4)
	movl	-12(%ebp), %eax
	movl	168(%eax), %eax
	leal	1(%eax), %edx
	movl	-12(%ebp), %eax
	movl	%edx, 168(%eax)
	movl	12(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC316, (%esp)
	call	__Z7d_printPKcz
	movl	$LC317, (%esp)
	call	__Z7d_printPKcz
	movl	$0, %eax
	leave
	ret
	.section .rdata,"dr"
LC318:
	.ascii "[add_reaction]\12\0"
	.align 4
LC319:
	.ascii "Trying to add reaction at: %s with id: %s\12\0"
LC320:
	.ascii "New reaction: %s:%s\12\0"
LC321:
	.ascii "[add_reaction ends]\12\0"
	.text
	.globl	__Z12add_reactionPcS_
	.def	__Z12add_reactionPcS_;	.scl	2;	.type	32;	.endef
__Z12add_reactionPcS_:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%ebx
	subl	$36, %esp
	movl	$LC318, (%esp)
	call	__Z7d_printPKcz
	movl	_my_trace, %eax
	movl	70021900(%eax), %eax
	movl	%eax, -12(%ebp)
	movl	12(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	8(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC319, (%esp)
	call	__Z7d_printPKcz
	movl	_my_trace, %eax
	movl	70021896(%eax), %edx
	movl	-12(%ebp), %eax
	imull	$188, %eax, %eax
	addl	%edx, %eax
	movl	$188, 8(%esp)
	movl	$0, 4(%esp)
	movl	%eax, (%esp)
	call	_memset
	movl	_my_trace, %eax
	movl	70021896(%eax), %edx
	movl	_my_trace, %eax
	movl	70021900(%eax), %eax
	imull	$188, %eax, %eax
	addl	%edx, %eax
	movl	%eax, %edx
	movl	12(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	%edx, (%esp)
	call	_strcpy
	movl	_my_trace, %eax
	movl	70021896(%eax), %edx
	movl	-12(%ebp), %eax
	imull	$188, %eax, %eax
	addl	%edx, %eax
	movb	$0, 2(%eax)
	movl	_my_trace, %eax
	movl	70021896(%eax), %edx
	movl	-12(%ebp), %eax
	imull	$188, %eax, %eax
	leal	(%edx,%eax), %ebx
	movl	_my_trace, %eax
	movl	70021896(%eax), %edx
	movl	-12(%ebp), %eax
	imull	$188, %eax, %eax
	addl	%edx, %eax
	movl	%eax, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z14add_breakpointPcP9REACTION_
	movl	%eax, 180(%ebx)
	movl	_my_trace, %eax
	movl	70021896(%eax), %edx
	movl	-12(%ebp), %eax
	imull	$188, %eax, %eax
	addl	%edx, %eax
	movl	180(%eax), %eax
	movb	$1, 521(%eax)
	movl	_my_trace, %eax
	movl	70021896(%eax), %edx
	movl	-12(%ebp), %eax
	imull	$188, %eax, %eax
	addl	%edx, %eax
	movb	$1, 172(%eax)
	movl	_my_trace, %eax
	movl	70021896(%eax), %edx
	movl	-12(%ebp), %eax
	imull	$188, %eax, %eax
	addl	%edx, %eax
	movb	$0, 173(%eax)
	movl	_my_trace, %eax
	movl	70021896(%eax), %edx
	movl	-12(%ebp), %eax
	imull	$188, %eax, %eax
	addl	%edx, %eax
	movl	180(%eax), %eax
	movl	%eax, (%esp)
	call	__Z17update_breakpointP11BREAKPOINT_
	movl	_my_trace, %eax
	movl	70021896(%eax), %edx
	movl	-12(%ebp), %eax
	imull	$188, %eax, %eax
	addl	%edx, %eax
	movl	%eax, 8(%esp)
	movl	8(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC320, (%esp)
	call	__Z7d_printPKcz
	movl	_my_trace, %eax
	movl	70021900(%eax), %edx
	addl	$1, %edx
	movl	%edx, 70021900(%eax)
	movl	$LC321, (%esp)
	call	__Z7d_printPKcz
	movl	$0, %eax
	addl	$36, %esp
	popl	%ebx
	popl	%ebp
	ret
	.section .rdata,"dr"
LC322:
	.ascii "[parse_region]\12\0"
LC323:
	.ascii "loc_str: %s\12\0"
LC324:
	.ascii "[parse_region finishes]\12\0"
	.text
	.globl	__Z12parse_regionPc
	.def	__Z12parse_regionPc;	.scl	2;	.type	32;	.endef
__Z12parse_regionPc:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$40, %esp
	movl	$LC322, (%esp)
	call	__Z7d_printPKcz
	movl	$LC67, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_strtok
	movl	%eax, -12(%ebp)
	movl	-12(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC323, (%esp)
	call	__Z7d_printPKcz
	movl	$LC67, 4(%esp)
	movl	$0, (%esp)
	call	_strtok
	movl	%eax, -16(%ebp)
	movl	-12(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC323, (%esp)
	call	__Z7d_printPKcz
	movl	_my_trace, %edx
	movl	_my_trace, %eax
	movl	70025748(%eax), %eax
	addl	$8753186, %eax
	sall	$3, %eax
	addl	%edx, %eax
	addl	$4, %eax
	movl	%eax, -20(%ebp)
	movl	-12(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z19parse_location_descPc
	movl	%eax, %edx
	movl	-20(%ebp), %eax
	movl	%edx, (%eax)
	movl	-16(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z19parse_location_descPc
	movl	%eax, %edx
	movl	-20(%ebp), %eax
	movl	%edx, 4(%eax)
	movl	_my_trace, %eax
	movl	70025748(%eax), %edx
	addl	$1, %edx
	movl	%edx, 70025748(%eax)
	movl	$LC324, (%esp)
	call	__Z7d_printPKcz
	movl	$0, %eax
	leave
	ret
	.section .rdata,"dr"
LC325:
	.ascii "[parse_reaction]\12\0"
LC326:
	.ascii "Parsing until ;: %s\12\0"
LC327:
	.ascii ",\0"
LC328:
	.ascii "id_str: %s\12\0"
LC329:
	.ascii ";\0"
LC330:
	.ascii "rids_str: %s\12\0"
LC331:
	.ascii "Processing rids\12\0"
LC332:
	.ascii "id_str: %s, rid: %s\12\0"
LC333:
	.ascii "Processing couples\12\0"
LC334:
	.ascii "id_str: %s, coupled_id: %s\12\0"
LC335:
	.ascii "[parse_reaction ends]\12\0"
	.text
	.globl	__Z14parse_reactionPc
	.def	__Z14parse_reactionPc;	.scl	2;	.type	32;	.endef
__Z14parse_reactionPc:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$3128, %esp
	movl	$LC325, (%esp)
	call	__Z7d_printPKcz
	movl	8(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC326, (%esp)
	call	__Z7d_printPKcz
	movl	$LC327, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_strtok
	movl	%eax, -12(%ebp)
	movl	-12(%ebp), %eax
	movl	%eax, 4(%esp)
	leal	-3100(%ebp), %eax
	movl	%eax, (%esp)
	call	_strcpy
	leal	-3100(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC323, (%esp)
	call	__Z7d_printPKcz
	movl	$LC327, 4(%esp)
	movl	$0, (%esp)
	call	_strtok
	movl	%eax, -16(%ebp)
	movl	-16(%ebp), %eax
	movl	%eax, 4(%esp)
	leal	-2332(%ebp), %eax
	movl	%eax, (%esp)
	call	_strcpy
	leal	-2332(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC328, (%esp)
	call	__Z7d_printPKcz
	movl	$LC329, 4(%esp)
	movl	$0, (%esp)
	call	_strtok
	movl	%eax, -20(%ebp)
	movl	-20(%ebp), %eax
	movl	%eax, 4(%esp)
	leal	-796(%ebp), %eax
	movl	%eax, (%esp)
	call	_strcpy
	leal	-796(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC330, (%esp)
	call	__Z7d_printPKcz
	leal	-2332(%ebp), %eax
	movl	%eax, 4(%esp)
	leal	-1564(%ebp), %eax
	movl	%eax, (%esp)
	call	_strcpy
	leal	-2332(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	-12(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z12add_reactionPcS_
	movl	$LC331, (%esp)
	call	__Z7d_printPKcz
	movl	$LC67, 4(%esp)
	leal	-796(%ebp), %eax
	movl	%eax, (%esp)
	call	_strtok
	movl	%eax, -24(%ebp)
	movl	-24(%ebp), %eax
	movl	%eax, 8(%esp)
	leal	-2332(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC332, (%esp)
	call	__Z7d_printPKcz
	movl	$16, 8(%esp)
	movl	$0, 4(%esp)
	movl	-24(%ebp), %eax
	movl	%eax, (%esp)
	call	_strtol
	movl	%eax, 4(%esp)
	leal	-2332(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z7add_ridPcj
	jmp	L787
L788:
	movl	-24(%ebp), %eax
	movl	%eax, 8(%esp)
	leal	-2332(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC332, (%esp)
	call	__Z7d_printPKcz
	movl	$16, 8(%esp)
	movl	$0, 4(%esp)
	movl	-24(%ebp), %eax
	movl	%eax, (%esp)
	call	_strtol
	movl	%eax, 4(%esp)
	leal	-2332(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z7add_ridPcj
L787:
	movl	$LC67, 4(%esp)
	movl	$0, (%esp)
	call	_strtok
	movl	%eax, -24(%ebp)
	cmpl	$0, -24(%ebp)
	setne	%al
	testb	%al, %al
	jne	L788
	movl	$LC333, (%esp)
	call	__Z7d_printPKcz
	movl	$LC67, 4(%esp)
	leal	-1564(%ebp), %eax
	movl	%eax, (%esp)
	call	_strtok
	movl	%eax, -28(%ebp)
	jmp	L789
L790:
	movl	-28(%ebp), %eax
	movl	%eax, 8(%esp)
	leal	-2332(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC334, (%esp)
	call	__Z7d_printPKcz
	movl	-28(%ebp), %eax
	movl	%eax, 4(%esp)
	leal	-2332(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z10add_couplePcS_
L789:
	movl	$LC67, 4(%esp)
	movl	$0, (%esp)
	call	_strtok
	movl	%eax, -28(%ebp)
	cmpl	$0, -28(%ebp)
	setne	%al
	testb	%al, %al
	jne	L790
	movl	$LC335, (%esp)
	call	__Z7d_printPKcz
	leave
	ret
	.globl	__Z15parse_reactionsPc
	.def	__Z15parse_reactionsPc;	.scl	2;	.type	32;	.endef
__Z15parse_reactionsPc:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$808, %esp
	movl	8(%ebp), %eax
	movl	%eax, -12(%ebp)
	jmp	L792
L794:
	movl	-12(%ebp), %eax
	movl	%eax, 4(%esp)
	leal	-780(%ebp), %eax
	movl	%eax, (%esp)
	call	_strcpy
	leal	-780(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z14parse_reactionPc
	movl	$59, 4(%esp)
	movl	-12(%ebp), %eax
	movl	%eax, (%esp)
	call	_strchr
	movl	%eax, -12(%ebp)
	cmpl	$0, -12(%ebp)
	je	L792
	addl	$1, -12(%ebp)
L792:
	cmpl	$0, -12(%ebp)
	je	L793
	movl	-12(%ebp), %eax
	movzbl	(%eax), %eax
	testb	%al, %al
	jne	L794
L793:
	movl	$0, %eax
	leave
	ret
	.globl	__Z13parse_regionsPc
	.def	__Z13parse_regionsPc;	.scl	2;	.type	32;	.endef
__Z13parse_regionsPc:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$808, %esp
	movl	8(%ebp), %eax
	movl	%eax, -12(%ebp)
	jmp	L797
L799:
	movl	-12(%ebp), %eax
	movl	%eax, 4(%esp)
	leal	-780(%ebp), %eax
	movl	%eax, (%esp)
	call	_strcpy
	leal	-780(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z12parse_regionPc
	movl	$59, 4(%esp)
	movl	-12(%ebp), %eax
	movl	%eax, (%esp)
	call	_strchr
	movl	%eax, -12(%ebp)
	cmpl	$0, -12(%ebp)
	je	L797
	addl	$1, -12(%ebp)
L797:
	cmpl	$0, -12(%ebp)
	je	L798
	movl	-12(%ebp), %eax
	movzbl	(%eax), %eax
	testb	%al, %al
	jne	L799
L798:
	movl	$0, %eax
	leave
	ret
	.section .rdata,"dr"
LC336:
	.ascii "%s\\%s_%d.out\0"
LC337:
	.ascii "LF,%s_%d.out\12\0"
LC338:
	.ascii "w+\0"
LC339:
	.ascii "Setting out file to: %s\12\0"
	.text
	.globl	__Z15reload_out_filev
	.def	__Z15reload_out_filev;	.scl	2;	.type	32;	.endef
__Z15reload_out_filev:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%ebx
	subl	$1572, %esp
	movl	_my_trace, %eax
	movl	70017512(%eax), %edx
	addl	$1, %edx
	movl	%edx, 70017512(%eax)
	movl	$0, %eax
	movb	%al, -1544(%ebp)
	movl	_my_trace, %eax
	movl	70017512(%eax), %eax
	movl	_my_trace, %edx
	leal	70016744(%edx), %ecx
	movl	_my_trace, %edx
	addl	$70015976, %edx
	movl	%eax, 16(%esp)
	movl	%ecx, 12(%esp)
	movl	%edx, 8(%esp)
	movl	$LC336, 4(%esp)
	leal	-1544(%ebp), %eax
	movl	%eax, (%esp)
	call	_sprintf
	movl	_my_trace, %eax
	leal	70017516(%eax), %edx
	leal	-1544(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	%edx, (%esp)
	call	_strcpy
	movl	_my_trace, %eax
	movl	70017512(%eax), %eax
	movl	_my_trace, %edx
	addl	$70016744, %edx
	movl	%eax, 12(%esp)
	movl	%edx, 8(%esp)
	movl	$LC337, 4(%esp)
	leal	-776(%ebp), %eax
	movl	%eax, (%esp)
	call	_sprintf
	leal	-776(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z13add_to_bufferPc
	movl	_my_trace, %eax
	movl	70012876(%eax), %eax
	movl	%eax, (%esp)
	call	_fclose
	movl	_my_trace, %ebx
	movl	_my_trace, %eax
	addl	$70017516, %eax
	movl	$LC338, 4(%esp)
	movl	%eax, (%esp)
	call	_fopen
	movl	%eax, 70012876(%ebx)
	movl	_my_trace, %eax
	addl	$70017516, %eax
	movl	%eax, 4(%esp)
	movl	$LC339, (%esp)
	call	__Z7d_printPKcz
	movl	$0, %eax
	addl	$1572, %esp
	popl	%ebx
	popl	%ebp
	ret
	.section .rdata,"dr"
LC340:
	.ascii "SN\0"
LC341:
	.ascii "Sample path set to: %s\12\0"
LC342:
	.ascii "SI\0"
LC343:
	.ascii "Sample PID set to: 0x%08x\12\0"
LC344:
	.ascii "SD\0"
LC345:
	.ascii "Research dir set to: %s\12\0"
LC346:
	.ascii "Sd\0"
LC347:
	.ascii "Out dir set to: %s\12\0"
LC348:
	.ascii "Sp\0"
LC349:
	.ascii "Out prefix set to: %s\12\0"
LC350:
	.ascii "Rp\0"
LC351:
	.ascii "SL\0"
LC352:
	.ascii "Trace limit set to: %d\12\0"
LC353:
	.ascii "ST\0"
LC354:
	.ascii "Trace name set to: %s\12\0"
LC355:
	.ascii "sD\0"
LC356:
	.ascii "Dump name set to: %s\12\0"
LC357:
	.ascii "Info name set to: %s\12\0"
LC358:
	.ascii "PT\0"
LC359:
	.ascii "Error, out dir not set\0"
LC360:
	.ascii "%s\\%s.log\0"
LC361:
	.ascii "w\0"
LC362:
	.ascii "Log file: %s\12\0"
LC363:
	.ascii "%s\\%s.dump\0"
LC364:
	.ascii "Dump file: %s\12\0"
LC365:
	.ascii "%s\\%s.mod\0"
LC366:
	.ascii "Mods file: %s\12\0"
LC367:
	.ascii "OM,%s.mod\12\0"
LC368:
	.ascii "%s\\%s.ini\0"
LC369:
	.ascii "Ini file: %s\12\0"
LC370:
	.ascii "ET\0"
LC371:
	.ascii "Tracing enabled\12\0"
LC372:
	.ascii "Starting @ 0x%08x\12\0"
LC373:
	.ascii "ST,0x%08x\12\0"
LC374:
	.ascii "EL\0"
	.align 4
LC375:
	.ascii "Light tracing debugged enabled\12\0"
LC376:
	.ascii "ED\0"
LC377:
	.ascii "Tracing debugged enabled\12\0"
LC378:
	.ascii "DT\0"
LC379:
	.ascii "Tracing disabled\12\0"
LC380:
	.ascii "Ending @ 0x%08x\12\0"
LC381:
	.ascii "END,0x%08x\12\0"
LC382:
	.ascii "DM\0"
LC383:
	.ascii "ER\0"
LC384:
	.ascii " \0"
	.align 4
LC385:
	.ascii "Setting exclusion for reaction no %s\12\0"
LC386:
	.ascii "NR\0"
LC387:
	.ascii "eR\0"
LC388:
	.ascii "Enabling reaction no %s\12\0"
LC389:
	.ascii "dR\0"
LC390:
	.ascii "Disabling single reaction %s\12\0"
LC391:
	.ascii "eA\0"
LC392:
	.ascii "Enabling all reactions\12\0"
LC393:
	.ascii "dA\0"
LC394:
	.ascii "Disabling all reactions\12\0"
LC395:
	.ascii "rR\0"
LC396:
	.ascii "Raising reaction no %s\12\0"
LC397:
	.ascii "lR\0"
LC398:
	.ascii "Lowering reaction no %s\12\0"
LC399:
	.ascii "Rx\0"
LC400:
	.ascii "ad\0"
LC401:
	.ascii "Starting debugging: 0x%08x\12\0"
LC402:
	.ascii "sd\0"
LC403:
	.ascii "Starting debugging: %s\12\0"
LC404:
	.ascii "ra\0"
LC405:
	.ascii "rs\0"
LC406:
	.ascii "ru\0"
LC407:
	.ascii "lt\0"
LC408:
	.ascii "la\0"
LC409:
	.ascii "lm\0"
LC410:
	.ascii "lb\0"
LC411:
	.ascii "ll\0"
LC412:
	.ascii "or\0"
	.align 4
LC413:
	.ascii "Ouputting region: 0x%08x, 0x%08x @ %d\12\0"
LC414:
	.ascii "Rr\0"
	.align 4
LC415:
	.ascii "Reading region: 0x%08x, 0x%08x\12\0"
LC416:
	.ascii "RM\0"
LC417:
	.ascii "Reading addr: 0x%08x\12\0"
LC418:
	.ascii "WS\0"
LC419:
	.ascii "Writing str: %s @ 0x%08x\12\0"
LC420:
	.ascii "WU\0"
LC421:
	.ascii "WM\0"
	.align 4
LC422:
	.ascii "Writing addr: 0x%08x with 0x%08x\12\0"
LC423:
	.ascii "RE\0"
LC424:
	.ascii "RR\0"
LC425:
	.ascii "WR\0"
LC426:
	.ascii "RS\0"
LC427:
	.ascii "cn\0"
	.align 4
LC428:
	.ascii "Continuing with status: 0x%08x\12\0"
LC429:
	.ascii "cN\0"
LC430:
	.ascii "SS\0"
LC431:
	.ascii "AS\0"
LC432:
	.ascii "cR\0"
LC433:
	.ascii "Configuring reactions\12\0"
LC434:
	.ascii "cr\0"
LC435:
	.ascii "R1\0"
	.text
	.globl	__Z10handle_cmdPc
	.def	__Z10handle_cmdPc;	.scl	2;	.type	32;	.endef
__Z10handle_cmdPc:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%edi
	pushl	%ebx
	subl	$1744, %esp
	movl	8(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC196, (%esp)
	call	__Z7d_printPKcz
	movl	_my_trace, %eax
	movl	$153, 67915420(%eax)
	movl	_my_trace, %eax
	addl	$67915424, %eax
	movl	$1048576, 8(%esp)
	movl	$0, 4(%esp)
	movl	%eax, (%esp)
	call	_memset
	movl	_my_trace, %eax
	addl	$67915424, %eax
	movb	$0, (%eax)
	movl	$2, 8(%esp)
	movl	$LC340, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_strncmp
	testl	%eax, %eax
	jne	L804
	movl	8(%ebp), %eax
	leal	3(%eax), %edx
	movl	_my_trace, %eax
	addl	$70015204, %eax
	movl	%edx, 4(%esp)
	movl	%eax, (%esp)
	call	_strcpy
	movl	_my_trace, %eax
	addl	$70015204, %eax
	movl	%eax, 4(%esp)
	movl	$LC341, (%esp)
	call	__Z7d_printPKcz
	call	__Z11send_reportv
	jmp	L805
L804:
	movl	$2, 8(%esp)
	movl	$LC342, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_strncmp
	testl	%eax, %eax
	jne	L806
	movl	_my_trace, %ebx
	movl	8(%ebp), %eax
	addl	$3, %eax
	movl	$16, 8(%esp)
	movl	$0, 4(%esp)
	movl	%eax, (%esp)
	call	_strtol
	movl	%eax, 70015972(%ebx)
	movl	_my_trace, %eax
	movl	70015972(%eax), %eax
	movl	%eax, 4(%esp)
	movl	$LC343, (%esp)
	call	__Z7d_printPKcz
	call	__Z11send_reportv
	jmp	L805
L806:
	movl	$2, 8(%esp)
	movl	$LC344, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_strncmp
	testl	%eax, %eax
	jne	L807
	movl	8(%ebp), %eax
	leal	3(%eax), %edx
	movl	_my_trace, %eax
	addl	$70014436, %eax
	movl	%edx, 4(%esp)
	movl	%eax, (%esp)
	call	_strcpy
	movl	_my_trace, %eax
	addl	$70014436, %eax
	movl	%eax, 4(%esp)
	movl	$LC345, (%esp)
	call	__Z7d_printPKcz
	call	__Z11send_reportv
	jmp	L805
L807:
	movl	$2, 8(%esp)
	movl	$LC346, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_strncmp
	testl	%eax, %eax
	jne	L808
	movl	8(%ebp), %eax
	leal	3(%eax), %edx
	movl	_my_trace, %eax
	addl	$70015976, %eax
	movl	%edx, 4(%esp)
	movl	%eax, (%esp)
	call	_strcpy
	movl	_my_trace, %eax
	addl	$70015976, %eax
	movl	%eax, 4(%esp)
	movl	$LC347, (%esp)
	call	__Z7d_printPKcz
	call	__Z11send_reportv
	jmp	L805
L808:
	movl	$2, 8(%esp)
	movl	$LC348, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_strncmp
	testl	%eax, %eax
	jne	L809
	movl	8(%ebp), %eax
	leal	3(%eax), %edx
	movl	_my_trace, %eax
	addl	$70016744, %eax
	movl	%edx, 4(%esp)
	movl	%eax, (%esp)
	call	_strcpy
	movl	_my_trace, %eax
	addl	$70016744, %eax
	movl	%eax, 4(%esp)
	movl	$LC349, (%esp)
	call	__Z7d_printPKcz
	call	__Z11send_reportv
	jmp	L805
L809:
	movl	$2, 8(%esp)
	movl	$LC350, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_strncmp
	testl	%eax, %eax
	jne	L810
	movl	_my_trace, %eax
	movl	$7, 67915420(%eax)
	movl	_my_trace, %eax
	leal	70016744(%eax), %edx
	movl	_my_trace, %eax
	addl	$67915424, %eax
	movl	%edx, 4(%esp)
	movl	%eax, (%esp)
	call	_strcpy
	call	__Z11send_reportv
	jmp	L805
L810:
	movl	$2, 8(%esp)
	movl	$LC351, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_strncmp
	testl	%eax, %eax
	jne	L811
	movl	_my_trace, %ebx
	movl	8(%ebp), %eax
	addl	$3, %eax
	movl	$10, 8(%esp)
	movl	$0, 4(%esp)
	movl	%eax, (%esp)
	call	_strtoul
	movl	$0, %edx
	movl	%eax, 68964032(%ebx)
	movl	%edx, 68964036(%ebx)
	movl	_my_trace, %eax
	movl	68964036(%eax), %edx
	movl	68964032(%eax), %eax
	movl	%eax, 4(%esp)
	movl	%edx, 8(%esp)
	movl	$LC352, (%esp)
	call	__Z7d_printPKcz
	call	__Z11send_reportv
	jmp	L805
L811:
	movl	$2, 8(%esp)
	movl	$LC353, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_strncmp
	testl	%eax, %eax
	jne	L812
	movl	_my_trace, %eax
	addl	$70017516, %eax
	movl	%eax, 4(%esp)
	movl	$LC354, (%esp)
	call	__Z7d_printPKcz
	call	__Z11send_reportv
	jmp	L805
L812:
	movl	$2, 8(%esp)
	movl	$LC355, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_strncmp
	testl	%eax, %eax
	jne	L813
	movl	_my_trace, %eax
	addl	$70018284, %eax
	movl	%eax, 4(%esp)
	movl	$LC356, (%esp)
	call	__Z7d_printPKcz
	call	__Z11send_reportv
	jmp	L805
L813:
	movl	$2, 8(%esp)
	movl	$LC342, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_strncmp
	testl	%eax, %eax
	jne	L814
	movl	_my_trace, %eax
	addl	$70019052, %eax
	movl	%eax, 4(%esp)
	movl	$LC357, (%esp)
	call	__Z7d_printPKcz
	call	__Z11send_reportv
	jmp	L805
L814:
	movl	$2, 8(%esp)
	movl	$LC358, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_strncmp
	testl	%eax, %eax
	jne	L815
	movl	_my_trace, %eax
	addl	$70015976, %eax
	testl	%eax, %eax
	je	L816
	movl	_my_trace, %eax
	addl	$70016744, %eax
	testl	%eax, %eax
	jne	L817
L816:
	movl	$LC359, (%esp)
	call	__Z7d_printPKcz
	movl	$-1, (%esp)
	call	_exit
L817:
	movl	_my_trace, %eax
	movl	70015972(%eax), %eax
	testl	%eax, %eax
	je	L818
	movl	_my_trace, %eax
	movl	70015972(%eax), %eax
	movl	_my_trace, %edx
	addl	$70016744, %edx
	movl	%eax, 8(%esp)
	movl	$LC271, 4(%esp)
	movl	%edx, (%esp)
	call	_sprintf
L818:
	movl	_my_trace, %eax
	movl	$-1, 70017512(%eax)
	call	__Z15reload_out_filev
	movl	$0, %eax
	movb	%al, -1720(%ebp)
	movl	_my_trace, %eax
	leal	70016744(%eax), %edx
	movl	_my_trace, %eax
	addl	$70015976, %eax
	movl	%edx, 12(%esp)
	movl	%eax, 8(%esp)
	movl	$LC360, 4(%esp)
	leal	-1720(%ebp), %eax
	movl	%eax, (%esp)
	call	_sprintf
	movl	_my_trace, %eax
	leal	70020588(%eax), %edx
	leal	-1720(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	%edx, (%esp)
	call	_strcpy
	movl	_my_trace, %ebx
	movl	_my_trace, %eax
	addl	$70020588, %eax
	movl	$LC361, 4(%esp)
	movl	%eax, (%esp)
	call	_fopen
	movl	%eax, 70012868(%ebx)
	movl	_my_trace, %eax
	movl	70012868(%eax), %eax
	movl	$0, 12(%esp)
	movl	$4, 8(%esp)
	movl	$0, 4(%esp)
	movl	%eax, (%esp)
	call	_setvbuf
	movl	_my_trace, %eax
	addl	$70020588, %eax
	movl	%eax, 4(%esp)
	movl	$LC362, (%esp)
	call	__Z7d_printPKcz
	movl	$0, %eax
	movb	%al, -1720(%ebp)
	movl	_my_trace, %eax
	leal	70016744(%eax), %edx
	movl	_my_trace, %eax
	addl	$70015976, %eax
	movl	%edx, 12(%esp)
	movl	%eax, 8(%esp)
	movl	$LC363, 4(%esp)
	leal	-1720(%ebp), %eax
	movl	%eax, (%esp)
	call	_sprintf
	movl	_my_trace, %eax
	leal	70018284(%eax), %edx
	leal	-1720(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	%edx, (%esp)
	call	_strcpy
	movl	_my_trace, %eax
	addl	$70018284, %eax
	movl	%eax, 4(%esp)
	movl	$LC364, (%esp)
	call	__Z7d_printPKcz
	movl	$0, %eax
	movb	%al, -1720(%ebp)
	movl	_my_trace, %eax
	leal	70016744(%eax), %edx
	movl	_my_trace, %eax
	addl	$70015976, %eax
	movl	%edx, 12(%esp)
	movl	%eax, 8(%esp)
	movl	$LC365, 4(%esp)
	leal	-1720(%ebp), %eax
	movl	%eax, (%esp)
	call	_sprintf
	movl	_my_trace, %eax
	leal	70019820(%eax), %edx
	leal	-1720(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	%edx, (%esp)
	call	_strcpy
	movl	_my_trace, %ebx
	movl	_my_trace, %eax
	addl	$70019820, %eax
	movl	$LC154, 4(%esp)
	movl	%eax, (%esp)
	call	_fopen
	movl	%eax, 70012880(%ebx)
	movl	_my_trace, %eax
	addl	$70019820, %eax
	movl	%eax, 4(%esp)
	movl	$LC366, (%esp)
	call	__Z7d_printPKcz
	movl	_my_trace, %eax
	addl	$70016744, %eax
	movl	%eax, 8(%esp)
	movl	$LC367, 4(%esp)
	leal	-952(%ebp), %eax
	movl	%eax, (%esp)
	call	_sprintf
	leal	-952(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z13add_to_bufferPc
	movl	$0, %eax
	movb	%al, -1720(%ebp)
	movl	_my_trace, %eax
	leal	70016744(%eax), %edx
	movl	_my_trace, %eax
	addl	$70015976, %eax
	movl	%edx, 12(%esp)
	movl	%eax, 8(%esp)
	movl	$LC368, 4(%esp)
	leal	-1720(%ebp), %eax
	movl	%eax, (%esp)
	call	_sprintf
	movl	_my_trace, %eax
	leal	70019052(%eax), %edx
	leal	-1720(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	%edx, (%esp)
	call	_strcpy
	movl	_my_trace, %ebx
	movl	_my_trace, %eax
	addl	$70019052, %eax
	movl	$LC361, 4(%esp)
	movl	%eax, (%esp)
	call	_fopen
	movl	%eax, 70012888(%ebx)
	movl	_my_trace, %eax
	addl	$70019052, %eax
	movl	%eax, 4(%esp)
	movl	$LC369, (%esp)
	call	__Z7d_printPKcz
	call	__Z11send_reportv
	movl	_my_trace, %eax
	movb	$1, 68964008(%eax)
	jmp	L805
L815:
	movl	$2, 8(%esp)
	movl	$LC370, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_strncmp
	testl	%eax, %eax
	jne	L819
	movl	_my_trace, %eax
	movb	$3, 68964008(%eax)
	movl	_my_trace, %eax
	addl	$68964092, %eax
	movl	%eax, (%esp)
	call	__Z11ss_callbackPv
	movl	$0, (%esp)
	call	__Z6set_ssm
	movl	$LC371, (%esp)
	call	__Z7d_printPKcz
	movl	_my_trace, %eax
	movl	68964272(%eax), %eax
	movl	%eax, 4(%esp)
	movl	$LC372, (%esp)
	call	__Z7d_printPKcz
	movl	_my_trace, %eax
	movl	68964272(%eax), %eax
	movl	%eax, 8(%esp)
	movl	$LC373, 4(%esp)
	leal	-952(%ebp), %eax
	movl	%eax, (%esp)
	call	_sprintf
	leal	-952(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z13add_to_bufferPc
	call	__Z11send_reportv
	jmp	L805
L819:
	movl	$2, 8(%esp)
	movl	$LC374, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_strncmp
	testl	%eax, %eax
	jne	L820
	movl	_my_trace, %eax
	movb	$5, 68964008(%eax)
	movl	_my_trace, %eax
	addl	$68964092, %eax
	movl	%eax, (%esp)
	call	__Z11ss_callbackPv
	movl	$0, (%esp)
	call	__Z6set_ssm
	movl	$LC375, (%esp)
	call	__Z7d_printPKcz
	movl	_my_trace, %eax
	movl	68964272(%eax), %eax
	movl	%eax, 4(%esp)
	movl	$LC372, (%esp)
	call	__Z7d_printPKcz
	movl	_my_trace, %eax
	movl	68964272(%eax), %eax
	movl	%eax, 8(%esp)
	movl	$LC373, 4(%esp)
	leal	-952(%ebp), %eax
	movl	%eax, (%esp)
	call	_sprintf
	leal	-952(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z13add_to_bufferPc
	call	__Z11send_reportv
	jmp	L805
L820:
	movl	$2, 8(%esp)
	movl	$LC376, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_strncmp
	testl	%eax, %eax
	jne	L821
	movl	_my_trace, %eax
	movb	$4, 68964008(%eax)
	movl	_my_trace, %eax
	addl	$68964092, %eax
	movl	%eax, (%esp)
	call	__Z11ss_callbackPv
	movl	$0, (%esp)
	call	__Z6set_ssm
	movl	$LC377, (%esp)
	call	__Z7d_printPKcz
	movl	_my_trace, %eax
	movl	68964272(%eax), %eax
	movl	%eax, 4(%esp)
	movl	$LC372, (%esp)
	call	__Z7d_printPKcz
	movl	_my_trace, %eax
	movl	68964272(%eax), %eax
	movl	%eax, 8(%esp)
	movl	$LC373, 4(%esp)
	leal	-952(%ebp), %eax
	movl	%eax, (%esp)
	call	_sprintf
	leal	-952(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z13add_to_bufferPc
	call	__Z11send_reportv
	jmp	L805
L821:
	movl	$2, 8(%esp)
	movl	$LC378, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_strncmp
	testl	%eax, %eax
	jne	L822
	movl	_my_trace, %eax
	movb	$6, 68964008(%eax)
	movl	$LC379, (%esp)
	call	__Z7d_printPKcz
	movl	_my_trace, %eax
	movl	68964272(%eax), %eax
	movl	%eax, 4(%esp)
	movl	$LC380, (%esp)
	call	__Z7d_printPKcz
	movl	_my_trace, %eax
	movl	68964272(%eax), %eax
	movl	%eax, 8(%esp)
	movl	$LC381, 4(%esp)
	leal	-952(%ebp), %eax
	movl	%eax, (%esp)
	call	_sprintf
	leal	-952(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z13add_to_bufferPc
	call	__Z11send_reportv
	jmp	L805
L822:
	movl	$2, 8(%esp)
	movl	$LC382, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_strncmp
	testl	%eax, %eax
	jne	L823
	call	__Z11dump_memoryv
	call	__Z11send_reportv
	jmp	L805
L823:
	movl	$2, 8(%esp)
	movl	$LC383, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_strncmp
	testl	%eax, %eax
	jne	L824
	movl	$LC384, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_strtok
	movl	%eax, -12(%ebp)
	movl	$LC384, 4(%esp)
	movl	$0, (%esp)
	call	_strtok
	movl	%eax, 4(%esp)
	leal	-952(%ebp), %eax
	movl	%eax, (%esp)
	call	_strcpy
	leal	-952(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC385, (%esp)
	call	__Z7d_printPKcz
	leal	-952(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z18exclusive_reactionPc
	call	__Z11send_reportv
	jmp	L805
L824:
	movl	$2, 8(%esp)
	movl	$LC386, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_strncmp
	testl	%eax, %eax
	jne	L825
	movl	$LC384, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_strtok
	movl	%eax, -16(%ebp)
	movl	$LC384, 4(%esp)
	movl	$0, (%esp)
	call	_strtok
	movl	%eax, 4(%esp)
	leal	-952(%ebp), %eax
	movl	%eax, (%esp)
	call	_strcpy
	leal	-952(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC385, (%esp)
	call	__Z7d_printPKcz
	leal	-952(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z18exclusive_reactionPc
	call	__Z11send_reportv
	jmp	L805
L825:
	movl	$2, 8(%esp)
	movl	$LC387, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_strncmp
	testl	%eax, %eax
	jne	L826
	movl	$LC384, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_strtok
	movl	%eax, -20(%ebp)
	movl	$LC384, 4(%esp)
	movl	$0, (%esp)
	call	_strtok
	movl	%eax, 4(%esp)
	leal	-952(%ebp), %eax
	movl	%eax, (%esp)
	call	_strcpy
	leal	-952(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC388, (%esp)
	call	__Z7d_printPKcz
	leal	-952(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z15enable_reactionPc
	call	__Z11send_reportv
	jmp	L805
L826:
	movl	$2, 8(%esp)
	movl	$LC389, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_strncmp
	testl	%eax, %eax
	jne	L827
	movl	$LC384, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_strtok
	movl	%eax, -24(%ebp)
	movl	$LC384, 4(%esp)
	movl	$0, (%esp)
	call	_strtok
	movl	%eax, 4(%esp)
	leal	-952(%ebp), %eax
	movl	%eax, (%esp)
	call	_strcpy
	leal	-952(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC390, (%esp)
	call	__Z7d_printPKcz
	leal	-952(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z16disable_reactionPc
	call	__Z11send_reportv
	jmp	L805
L827:
	movl	$2, 8(%esp)
	movl	$LC391, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_strncmp
	testl	%eax, %eax
	jne	L828
	movl	$LC392, (%esp)
	call	__Z7d_printPKcz
	call	__Z20enable_all_reactionsv
	call	__Z11send_reportv
	jmp	L805
L828:
	movl	$2, 8(%esp)
	movl	$LC393, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_strncmp
	testl	%eax, %eax
	jne	L829
	movl	$LC394, (%esp)
	call	__Z7d_printPKcz
	call	__Z21disable_all_reactionsv
	call	__Z11send_reportv
	jmp	L805
L829:
	movl	$2, 8(%esp)
	movl	$LC395, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_strncmp
	testl	%eax, %eax
	jne	L830
	movl	$LC384, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_strtok
	movl	%eax, -28(%ebp)
	movl	$LC384, 4(%esp)
	movl	$0, (%esp)
	call	_strtok
	movl	%eax, 4(%esp)
	leal	-952(%ebp), %eax
	movl	%eax, (%esp)
	call	_strcpy
	leal	-952(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC396, (%esp)
	call	__Z7d_printPKcz
	leal	-952(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z14raise_reactionPc
	call	__Z11send_reportv
	jmp	L805
L830:
	movl	$2, 8(%esp)
	movl	$LC397, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_strncmp
	testl	%eax, %eax
	jne	L831
	movl	$LC384, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_strtok
	movl	%eax, -32(%ebp)
	movl	$LC384, 4(%esp)
	movl	$0, (%esp)
	call	_strtok
	movl	%eax, 4(%esp)
	leal	-952(%ebp), %eax
	movl	%eax, (%esp)
	call	_strcpy
	leal	-952(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC398, (%esp)
	call	__Z7d_printPKcz
	leal	-952(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z14lower_reactionPc
	call	__Z11send_reportv
	jmp	L805
L831:
	movl	$2, 8(%esp)
	movl	$LC399, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_strncmp
	testl	%eax, %eax
	jne	L832
	movl	8(%ebp), %eax
	addl	$3, %eax
	movl	$16, 8(%esp)
	movl	$0, 4(%esp)
	movl	%eax, (%esp)
	call	_strtoul
	movl	%eax, -36(%ebp)
	movl	-36(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z11run_routinej
	call	__Z11send_reportv
	jmp	L805
L832:
	movl	$2, 8(%esp)
	movl	$LC400, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_strncmp
	testl	%eax, %eax
	jne	L833
	movl	_my_trace, %eax
	movl	70015972(%eax), %eax
	movl	%eax, 4(%esp)
	movl	$LC401, (%esp)
	call	__Z7d_printPKcz
	call	__Z15start_trace_pidv
	movl	_my_trace, %eax
	movb	$1, 68964008(%eax)
	movl	_my_trace, %ebx
	call	__Z18get_pending_eventsv
	movl	%eax, 67915420(%ebx)
	call	__Z11send_reportv
	jmp	L805
L833:
	movl	$2, 8(%esp)
	movl	$LC402, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_strncmp
	testl	%eax, %eax
	jne	L834
	movl	_my_trace, %eax
	leal	70014436(%eax), %edx
	movl	_my_trace, %eax
	addl	$70021356, %eax
	movl	%edx, 4(%esp)
	movl	%eax, (%esp)
	call	_strcpy
	movl	_my_trace, %eax
	leal	70021356(%eax), %edx
	movl	%edx, %eax
	movl	$-1, %ecx
	movl	%eax, %ebx
	movl	$0, %eax
	movl	%ebx, %edi
	repnz scasb
	movl	%ecx, %eax
	notl	%eax
	subl	$1, %eax
	addl	%edx, %eax
	movw	$92, (%eax)
	movl	_my_trace, %eax
	leal	70015204(%eax), %edx
	movl	_my_trace, %eax
	addl	$70021356, %eax
	movl	%edx, 4(%esp)
	movl	%eax, (%esp)
	call	_strcat
	movl	_my_trace, %eax
	addl	$70021356, %eax
	movl	%eax, 4(%esp)
	movl	$LC403, (%esp)
	call	__Z7d_printPKcz
	call	__Z17start_trace_fnamev
	movl	_my_trace, %eax
	movb	$1, 68964008(%eax)
	movl	_my_trace, %ebx
	call	__Z18get_pending_eventsv
	movl	%eax, 67915420(%ebx)
	call	__Z11send_reportv
	jmp	L805
L834:
	movl	$2, 8(%esp)
	movl	$LC404, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_strncmp
	testl	%eax, %eax
	jne	L835
	movl	8(%ebp), %eax
	addl	$3, %eax
	movl	$10, 8(%esp)
	movl	$0, 4(%esp)
	movl	%eax, (%esp)
	call	_strtoul
	movl	%eax, -40(%ebp)
	movl	-40(%ebp), %eax
	addl	$1, %eax
	movl	%eax, (%esp)
	call	__Z12report_arg_xj
	call	__Z11send_reportv
	jmp	L805
L835:
	movl	$2, 8(%esp)
	movl	$LC405, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_strncmp
	testl	%eax, %eax
	jne	L836
	movl	8(%ebp), %eax
	addl	$3, %eax
	movl	$10, 8(%esp)
	movl	$0, 4(%esp)
	movl	%eax, (%esp)
	call	_strtoul
	movl	%eax, -44(%ebp)
	movl	-44(%ebp), %eax
	addl	$1, %eax
	movl	%eax, (%esp)
	call	__Z19report_arg_string_xj
	call	__Z11send_reportv
	jmp	L805
L836:
	movl	$2, 8(%esp)
	movl	$LC406, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_strncmp
	testl	%eax, %eax
	jne	L837
	movl	8(%ebp), %eax
	addl	$3, %eax
	movl	$10, 8(%esp)
	movl	$0, 4(%esp)
	movl	%eax, (%esp)
	call	_strtoul
	movl	%eax, -48(%ebp)
	movl	-48(%ebp), %eax
	addl	$1, %eax
	movl	%eax, (%esp)
	call	__Z27report_arg_unicode_string_xj
	call	__Z11send_reportv
	jmp	L805
L837:
	movl	$2, 8(%esp)
	movl	$LC407, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_strncmp
	testl	%eax, %eax
	jne	L838
	call	__Z9list_tebsv
	call	__Z11send_reportv
	jmp	L805
L838:
	movl	$2, 8(%esp)
	movl	$LC408, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_strncmp
	testl	%eax, %eax
	jne	L839
	call	__Z13list_all_tebsv
	call	__Z11send_reportv
	jmp	L805
L839:
	movl	$2, 8(%esp)
	movl	$LC409, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_strncmp
	testl	%eax, %eax
	jne	L840
	call	__Z14list_reactionsv
	call	__Z11send_reportv
	jmp	L805
L840:
	movl	$2, 8(%esp)
	movl	$LC410, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_strncmp
	testl	%eax, %eax
	jne	L841
	call	__Z9list_bptsv
	call	__Z11send_reportv
	jmp	L805
L841:
	movl	$2, 8(%esp)
	movl	$LC411, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_strncmp
	testl	%eax, %eax
	jne	L842
	call	__Z9list_libsv
	call	__Z11send_reportv
	jmp	L805
L842:
	movl	$2, 8(%esp)
	movl	$LC412, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_strncmp
	testl	%eax, %eax
	jne	L843
	movl	$LC384, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_strtok
	movl	%eax, -52(%ebp)
	movl	$LC384, 4(%esp)
	movl	$0, (%esp)
	call	_strtok
	movl	$16, 8(%esp)
	movl	$0, 4(%esp)
	movl	%eax, (%esp)
	call	_strtoul
	movl	%eax, -56(%ebp)
	movl	$LC384, 4(%esp)
	movl	$0, (%esp)
	call	_strtok
	movl	$16, 8(%esp)
	movl	$0, 4(%esp)
	movl	%eax, (%esp)
	call	_strtoul
	movl	%eax, -60(%ebp)
	movl	_my_trace, %eax
	movl	68964020(%eax), %edx
	movl	68964016(%eax), %eax
	movl	%eax, 12(%esp)
	movl	%edx, 16(%esp)
	movl	-60(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	-56(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC413, (%esp)
	call	__Z7d_printPKcz
	movl	-60(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	-56(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z10out_regionmm
	call	__Z11send_reportv
	jmp	L805
L843:
	movl	$2, 8(%esp)
	movl	$LC414, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_strncmp
	testl	%eax, %eax
	jne	L844
	movl	$LC384, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_strtok
	movl	%eax, -64(%ebp)
	movl	$LC384, 4(%esp)
	movl	$0, (%esp)
	call	_strtok
	movl	$16, 8(%esp)
	movl	$0, 4(%esp)
	movl	%eax, (%esp)
	call	_strtoul
	movl	%eax, -68(%ebp)
	movl	$LC384, 4(%esp)
	movl	$0, (%esp)
	call	_strtok
	movl	$16, 8(%esp)
	movl	$0, 4(%esp)
	movl	%eax, (%esp)
	call	_strtoul
	movl	%eax, -72(%ebp)
	movl	-72(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	-68(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC415, (%esp)
	call	__Z7d_printPKcz
	movl	-72(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	-68(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z13report_regionmm
	call	__Z11send_reportv
	jmp	L805
L844:
	movl	$2, 8(%esp)
	movl	$LC416, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_strncmp
	testl	%eax, %eax
	jne	L845
	movl	$LC384, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_strtok
	movl	%eax, -76(%ebp)
	movl	$LC384, 4(%esp)
	movl	$0, (%esp)
	call	_strtok
	movl	$16, 8(%esp)
	movl	$0, 4(%esp)
	movl	%eax, (%esp)
	call	_strtoul
	movl	%eax, -80(%ebp)
	movl	-80(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC417, (%esp)
	call	__Z7d_printPKcz
	movl	-80(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z12report_dwordm
	call	__Z11send_reportv
	jmp	L805
L845:
	movl	$2, 8(%esp)
	movl	$LC418, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_strncmp
	testl	%eax, %eax
	jne	L846
	movl	$LC384, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_strtok
	movl	%eax, -84(%ebp)
	movl	$LC384, 4(%esp)
	movl	$0, (%esp)
	call	_strtok
	movl	$16, 8(%esp)
	movl	$0, 4(%esp)
	movl	%eax, (%esp)
	call	_strtoul
	movl	%eax, -88(%ebp)
	movl	$LC329, 4(%esp)
	movl	$0, (%esp)
	call	_strtok
	movl	%eax, -92(%ebp)
	movl	-88(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	-92(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC419, (%esp)
	call	__Z7d_printPKcz
	movl	-92(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	-88(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z18write_string_asciijPc
	call	__Z11send_reportv
	jmp	L805
L846:
	movl	$2, 8(%esp)
	movl	$LC420, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_strncmp
	testl	%eax, %eax
	jne	L847
	movl	$LC384, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_strtok
	movl	%eax, -96(%ebp)
	movl	$LC384, 4(%esp)
	movl	$0, (%esp)
	call	_strtok
	movl	$16, 8(%esp)
	movl	$0, 4(%esp)
	movl	%eax, (%esp)
	call	_strtoul
	movl	%eax, -100(%ebp)
	movl	$LC329, 4(%esp)
	movl	$0, (%esp)
	call	_strtok
	movl	%eax, -104(%ebp)
	movl	-100(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	-104(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC419, (%esp)
	call	__Z7d_printPKcz
	movl	-104(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	-100(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z20write_string_unicodejPc
	call	__Z11send_reportv
	jmp	L805
L847:
	movl	$2, 8(%esp)
	movl	$LC421, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_strncmp
	testl	%eax, %eax
	jne	L848
	movl	$LC384, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_strtok
	movl	%eax, -108(%ebp)
	movl	$LC384, 4(%esp)
	movl	$0, (%esp)
	call	_strtok
	movl	$16, 8(%esp)
	movl	$0, 4(%esp)
	movl	%eax, (%esp)
	call	_strtoul
	movl	%eax, -112(%ebp)
	movl	$LC384, 4(%esp)
	movl	$0, (%esp)
	call	_strtok
	movl	$16, 8(%esp)
	movl	$0, 4(%esp)
	movl	%eax, (%esp)
	call	_strtoul
	movl	%eax, -116(%ebp)
	movl	-116(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	-112(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC422, (%esp)
	call	__Z7d_printPKcz
	movl	-116(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	-112(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z11write_dwordmm
	call	__Z11send_reportv
	jmp	L805
L848:
	movl	$2, 8(%esp)
	movl	$LC423, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_strncmp
	testl	%eax, %eax
	jne	L849
	movl	$LC384, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_strtok
	movl	%eax, -120(%ebp)
	movl	$LC384, 4(%esp)
	movl	$0, (%esp)
	call	_strtok
	movl	$16, 8(%esp)
	movl	$0, 4(%esp)
	movl	%eax, (%esp)
	call	_strtoul
	movl	%eax, -124(%ebp)
	movl	-124(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z14release_threadm
	call	__Z11send_reportv
	jmp	L805
L849:
	movl	$2, 8(%esp)
	movl	$LC424, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_strncmp
	testl	%eax, %eax
	jne	L850
	movl	$LC384, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_strtok
	movl	%eax, -128(%ebp)
	movl	$LC384, 4(%esp)
	movl	$0, (%esp)
	call	_strtok
	movl	$16, 8(%esp)
	movl	$0, 4(%esp)
	movl	%eax, (%esp)
	call	_strtoul
	movl	%eax, -132(%ebp)
	movl	$LC384, 4(%esp)
	movl	$0, (%esp)
	call	_strtok
	movl	%eax, -128(%ebp)
	movl	-128(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	-132(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z15report_registermPc
	call	__Z11send_reportv
	jmp	L805
L850:
	movl	$2, 8(%esp)
	movl	$LC425, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_strncmp
	testl	%eax, %eax
	jne	L851
	movl	$LC384, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_strtok
	movl	%eax, -136(%ebp)
	movl	$LC384, 4(%esp)
	movl	$0, (%esp)
	call	_strtok
	movl	$16, 8(%esp)
	movl	$0, 4(%esp)
	movl	%eax, (%esp)
	call	_strtoul
	movl	%eax, -140(%ebp)
	movl	$LC384, 4(%esp)
	movl	$0, (%esp)
	call	_strtok
	movl	%eax, -136(%ebp)
	movl	$LC384, 4(%esp)
	movl	$0, (%esp)
	call	_strtok
	movl	%eax, -144(%ebp)
	movl	-144(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	-136(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	-140(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z14write_registermPcS_
	call	__Z11send_reportv
	jmp	L805
L851:
	movl	$2, 8(%esp)
	movl	$LC426, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_strncmp
	testl	%eax, %eax
	jne	L852
	movl	$LC384, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_strtok
	movl	%eax, -148(%ebp)
	movl	$LC384, 4(%esp)
	movl	$0, (%esp)
	call	_strtok
	movl	$16, 8(%esp)
	movl	$0, 4(%esp)
	movl	%eax, (%esp)
	call	_strtoul
	movl	%eax, -152(%ebp)
	movl	$LC384, 4(%esp)
	movl	$0, (%esp)
	call	_strtok
	movl	$16, 8(%esp)
	movl	$0, 4(%esp)
	movl	%eax, (%esp)
	call	_strtoul
	movl	%eax, -156(%ebp)
	movl	-156(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	-152(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z10read_stackmm
	call	__Z11send_reportv
	jmp	L805
L852:
	movl	$2, 8(%esp)
	movl	$LC427, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_strncmp
	testl	%eax, %eax
	jne	L853
	movl	$LC384, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_strtok
	movl	%eax, -160(%ebp)
	movl	$LC384, 4(%esp)
	movl	$0, (%esp)
	call	_strtok
	movl	%eax, -160(%ebp)
	movl	$16, 8(%esp)
	movl	$0, 4(%esp)
	movl	-160(%ebp), %eax
	movl	%eax, (%esp)
	call	_strtoul
	movl	%eax, -164(%ebp)
	movl	-164(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC428, (%esp)
	call	__Z7d_printPKcz
	movl	-164(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$-1, (%esp)
	call	__Z16continue_routinemj
	call	__Z11send_reportv
	jmp	L805
L853:
	movl	$2, 8(%esp)
	movl	$LC429, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_strncmp
	testl	%eax, %eax
	jne	L854
	movl	$LC384, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_strtok
	movl	$LC384, 4(%esp)
	movl	$0, (%esp)
	call	_strtok
	movl	$16, 8(%esp)
	movl	$0, 4(%esp)
	movl	%eax, (%esp)
	call	_strtoul
	movl	%eax, -168(%ebp)
	movl	$LC384, 4(%esp)
	movl	$0, (%esp)
	call	_strtok
	movl	$16, 8(%esp)
	movl	$0, 4(%esp)
	movl	%eax, (%esp)
	call	_strtoul
	movl	%eax, -172(%ebp)
	movl	-172(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	-168(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z16continue_routinemj
	call	__Z11send_reportv
	jmp	L805
L854:
	movl	$2, 8(%esp)
	movl	$LC430, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_strncmp
	testl	%eax, %eax
	jne	L855
	movl	$LC384, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_strtok
	movl	$LC384, 4(%esp)
	movl	$0, (%esp)
	call	_strtok
	movl	$16, 8(%esp)
	movl	$0, 4(%esp)
	movl	%eax, (%esp)
	call	_strtoul
	movl	%eax, -176(%ebp)
	movl	-176(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	$LC46, 4(%esp)
	leal	-952(%ebp), %eax
	movl	%eax, (%esp)
	call	_sprintf
	movl	$LC353, 4(%esp)
	leal	-952(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z12add_reactionPcS_
	movl	$0, 4(%esp)
	movl	$LC353, (%esp)
	call	__Z7add_ridPcj
	movl	$LC353, (%esp)
	call	__Z15enable_reactionPc
	call	__Z11send_reportv
	jmp	L805
L855:
	movl	$2, 8(%esp)
	movl	$LC431, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_strncmp
	testl	%eax, %eax
	jne	L856
	movl	_my_trace, %eax
	movl	112(%eax), %eax
	movl	%eax, 8(%esp)
	movl	$LC46, 4(%esp)
	leal	-952(%ebp), %eax
	movl	%eax, (%esp)
	call	_sprintf
	movl	$LC353, 4(%esp)
	leal	-952(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z12add_reactionPcS_
	movl	$0, 4(%esp)
	movl	$LC353, (%esp)
	call	__Z7add_ridPcj
	movl	$LC353, (%esp)
	call	__Z15enable_reactionPc
	call	__Z11send_reportv
	jmp	L805
L856:
	movl	$2, 8(%esp)
	movl	$LC432, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_strncmp
	testl	%eax, %eax
	jne	L857
	movl	$LC384, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_strtok
	movl	$LC384, 4(%esp)
	movl	$0, (%esp)
	call	_strtok
	movl	%eax, -180(%ebp)
	movl	$LC433, (%esp)
	call	__Z7d_printPKcz
	movl	-180(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z15parse_reactionsPc
	call	__Z11send_reportv
	jmp	L805
L857:
	movl	$2, 8(%esp)
	movl	$LC434, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_strncmp
	testl	%eax, %eax
	jne	L858
	movl	$LC384, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_strtok
	movl	$LC384, 4(%esp)
	movl	$0, (%esp)
	call	_strtok
	movl	%eax, -184(%ebp)
	movl	-184(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z13parse_regionsPc
	call	__Z11send_reportv
	jmp	L805
L858:
	movl	$2, 8(%esp)
	movl	$LC435, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	_strncmp
L805:
	movl	$0, %eax
	addl	$1744, %esp
	popl	%ebx
	popl	%edi
	popl	%ebp
	ret
	.globl	__Z10init_traceP13TRACE_CONFIG_Pcs
	.def	__Z10init_traceP13TRACE_CONFIG_Pcs;	.scl	2;	.type	32;	.endef
__Z10init_traceP13TRACE_CONFIG_Pcs:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%ebx
	subl	$36, %esp
	movl	16(%ebp), %eax
	movw	%ax, -12(%ebp)
	movl	_my_trace, %eax
	movl	$0, 68964048(%eax)
	movl	$0, 68964052(%eax)
	movl	_my_trace, %eax
	movl	$0, 70021900(%eax)
	movl	_my_trace, %eax
	leal	67914904(%eax), %edx
	movl	12(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	%edx, (%esp)
	call	_strcpy
	movl	_my_trace, %eax
	movswl	-12(%ebp), %edx
	movl	%edx, 67914900(%eax)
	movl	_my_trace, %eax
	addl	$136, %eax
	movl	$67108864, 8(%esp)
	movl	$-1, 4(%esp)
	movl	%eax, (%esp)
	call	_memset
	movl	_my_trace, %ebx
	movl	$0, 8(%esp)
	movl	$0, 4(%esp)
	movl	$0, (%esp)
	movl	__imp__CreateMutexA@12, %eax
	call	*%eax
	subl	$12, %esp
	movl	%eax, 67642512(%ebx)
	movl	_my_trace, %ebx
	movl	$0, 12(%esp)
	movl	$0, 8(%esp)
	movl	$0, 4(%esp)
	movl	$0, (%esp)
	movl	__imp__CreateEventA@16, %eax
	call	*%eax
	subl	$16, %esp
	movl	%eax, 67642504(%ebx)
	movl	_my_trace, %ebx
	movl	$0, 12(%esp)
	movl	$0, 8(%esp)
	movl	$0, 4(%esp)
	movl	$0, (%esp)
	movl	__imp__CreateEventA@16, %eax
	call	*%eax
	subl	$16, %esp
	movl	%eax, 67642508(%ebx)
	movl	_my_trace, %eax
	movl	$0, 68964272(%eax)
	movl	_my_trace, %eax
	movl	$0, 68964276(%eax)
	movl	_my_trace, %ebx
	movl	$96256, %eax
	movl	%eax, (%esp)
	call	_malloc
	movl	%eax, 70021896(%ebx)
	movl	-4(%ebp), %ebx
	leave
	ret
	.section .rdata,"dr"
LC436:
	.ascii "Configuring syscall out args\12\0"
	.text
	.globl	__Z18configure_syscallsv
	.def	__Z18configure_syscallsv;	.scl	2;	.type	32;	.endef
__Z18configure_syscallsv:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$40, %esp
	movl	$LC436, (%esp)
	call	__Z7d_printPKcz
	movl	$0, -12(%ebp)
	jmp	L862
L865:
	movl	$0, -16(%ebp)
	jmp	L863
L864:
	movl	_my_trace, %eax
	movl	-12(%ebp), %edx
	movl	%edx, %ecx
	sall	$4, %ecx
	movl	-16(%ebp), %edx
	addl	%ecx, %edx
	addl	$4376611, %edx
	sall	$4, %edx
	addl	%edx, %eax
	movb	$6, 16(%eax)
	addl	$1, -16(%ebp)
L863:
	cmpl	$15, -16(%ebp)
	jbe	L864
	addl	$1, -12(%ebp)
L862:
	cmpl	$65535, -12(%ebp)
	jbe	L865
	movl	_my_trace, %eax
	movl	$1, 70030648(%eax)
	movl	$4, 70030652(%eax)
	movb	$2, 70030656(%eax)
	movb	$0, 70030657(%eax)
	movl	$-16711936, 70030660(%eax)
	movl	_my_trace, %eax
	movl	$3, 70030664(%eax)
	movl	$4, 70030668(%eax)
	movb	$2, 70030672(%eax)
	movb	$0, 70030673(%eax)
	movl	$-16711936, 70030676(%eax)
	movl	_my_trace, %eax
	movl	$1, 70030680(%eax)
	movl	$3, 70030684(%eax)
	movb	$3, 70030688(%eax)
	movb	$2, 70030689(%eax)
	movl	$0, 70030692(%eax)
	movl	_my_trace, %eax
	movl	_last_arg, %edx
	movl	%edx, 70030696(%eax)
	movl	_last_arg+4, %edx
	movl	%edx, 70030700(%eax)
	movl	_last_arg+8, %edx
	movl	%edx, 70030704(%eax)
	movl	_last_arg+12, %edx
	movl	%edx, 70030708(%eax)
	movl	_my_trace, %eax
	movl	_last_arg, %edx
	movl	%edx, 70038584(%eax)
	movl	_last_arg+4, %edx
	movl	%edx, 70038588(%eax)
	movl	_last_arg+8, %edx
	movl	%edx, 70038592(%eax)
	movl	_last_arg+12, %edx
	movl	%edx, 70038596(%eax)
	movl	_my_trace, %eax
	movl	$0, 70042680(%eax)
	movl	$4, 70042684(%eax)
	movb	$2, 70042688(%eax)
	movb	$0, 70042689(%eax)
	movl	$-16711936, 70042692(%eax)
	movl	_my_trace, %eax
	movl	$3, 70042696(%eax)
	movl	$8, 70042700(%eax)
	movb	$2, 70042704(%eax)
	movb	$0, 70042705(%eax)
	movl	$-16711936, 70042708(%eax)
	movl	_my_trace, %eax
	movl	_last_arg, %edx
	movl	%edx, 70042712(%eax)
	movl	_last_arg+4, %edx
	movl	%edx, 70042716(%eax)
	movl	_last_arg+8, %edx
	movl	%edx, 70042720(%eax)
	movl	_last_arg+12, %edx
	movl	%edx, 70042724(%eax)
	movl	_my_trace, %eax
	movl	$0, 70047288(%eax)
	movl	$4, 70047292(%eax)
	movb	$2, 70047296(%eax)
	movb	$0, 70047297(%eax)
	movl	$-16711936, 70047300(%eax)
	movl	_my_trace, %eax
	movl	_last_arg, %edx
	movl	%edx, 70047304(%eax)
	movl	_last_arg+4, %edx
	movl	%edx, 70047308(%eax)
	movl	_last_arg+8, %edx
	movl	%edx, 70047312(%eax)
	movl	_last_arg+12, %edx
	movl	%edx, 70047316(%eax)
	movl	_my_trace, %eax
	movl	$4, 70053176(%eax)
	movl	$8, 70053180(%eax)
	movb	$2, 70053184(%eax)
	movb	$0, 70053185(%eax)
	movl	$0, 70053188(%eax)
	movl	_my_trace, %eax
	movl	$8, 70053192(%eax)
	movl	$9, 70053196(%eax)
	movb	$3, 70053200(%eax)
	movb	$2, 70053201(%eax)
	movl	$0, 70053204(%eax)
	movl	_my_trace, %eax
	movl	_last_arg, %edx
	movl	%edx, 70053208(%eax)
	movl	_last_arg+4, %edx
	movl	%edx, 70053212(%eax)
	movl	_last_arg+8, %edx
	movl	%edx, 70053216(%eax)
	movl	_last_arg+12, %edx
	movl	%edx, 70053220(%eax)
	movl	_my_trace, %eax
	movl	$1, 70059320(%eax)
	movl	$2, 70059324(%eax)
	movb	$3, 70059328(%eax)
	movb	$2, 70059329(%eax)
	movl	$0, 70059332(%eax)
	movl	_my_trace, %eax
	movl	$1, 70059336(%eax)
	movl	$4, 70059340(%eax)
	movb	$2, 70059344(%eax)
	movb	$0, 70059345(%eax)
	movl	$-16711936, 70059348(%eax)
	movl	_my_trace, %eax
	movl	$2, 70059352(%eax)
	movl	$4, 70059356(%eax)
	movb	$2, 70059360(%eax)
	movb	$0, 70059361(%eax)
	movl	$-16711936, 70059364(%eax)
	movl	_my_trace, %eax
	movl	_last_arg, %edx
	movl	%edx, 70059368(%eax)
	movl	_last_arg+4, %edx
	movl	%edx, 70059372(%eax)
	movl	_last_arg+8, %edx
	movl	%edx, 70059376(%eax)
	movl	_last_arg+12, %edx
	movl	%edx, 70059380(%eax)
	movl	_my_trace, %eax
	movl	$2, 70068792(%eax)
	movl	$6, 70068796(%eax)
	movb	$3, 70068800(%eax)
	movb	$2, 70068801(%eax)
	movl	$0, 70068804(%eax)
	movl	_my_trace, %eax
	movl	$2, 70068808(%eax)
	movl	$4, 70068812(%eax)
	movb	$2, 70068816(%eax)
	movb	$0, 70068817(%eax)
	movl	$-16711936, 70068820(%eax)
	movl	_my_trace, %eax
	movl	$5, 70068824(%eax)
	movl	$8, 70068828(%eax)
	movb	$2, 70068832(%eax)
	movb	$0, 70068833(%eax)
	movl	$-16711936, 70068836(%eax)
	movl	_my_trace, %eax
	movl	$6, 70068840(%eax)
	movl	$4, 70068844(%eax)
	movb	$2, 70068848(%eax)
	movb	$0, 70068849(%eax)
	movl	$-16711936, 70068852(%eax)
	movl	_my_trace, %eax
	movl	_last_arg, %edx
	movl	%edx, 70068856(%eax)
	movl	_last_arg+4, %edx
	movl	%edx, 70068860(%eax)
	movl	_last_arg+8, %edx
	movl	%edx, 70068864(%eax)
	movl	_last_arg+12, %edx
	movl	%edx, 70068868(%eax)
	movl	_my_trace, %eax
	movl	$0, 70070584(%eax)
	movl	$4, 70070588(%eax)
	movb	$2, 70070592(%eax)
	movb	$0, 70070593(%eax)
	movl	$-16711936, 70070596(%eax)
	movl	_my_trace, %eax
	movl	_last_arg, %edx
	movl	%edx, 70070600(%eax)
	movl	_last_arg+4, %edx
	movl	%edx, 70070604(%eax)
	movl	_last_arg+8, %edx
	movl	%edx, 70070608(%eax)
	movl	_last_arg+12, %edx
	movl	%edx, 70070612(%eax)
	movl	_my_trace, %eax
	movl	$0, 70071608(%eax)
	movl	$4, 70071612(%eax)
	movb	$2, 70071616(%eax)
	movb	$0, 70071617(%eax)
	movl	$-16711936, 70071620(%eax)
	movl	_my_trace, %eax
	movl	$3, 70071624(%eax)
	movl	$8, 70071628(%eax)
	movb	$2, 70071632(%eax)
	movb	$0, 70071633(%eax)
	movl	$-16711936, 70071636(%eax)
	movl	_my_trace, %eax
	movl	_last_arg, %edx
	movl	%edx, 70071640(%eax)
	movl	_last_arg+4, %edx
	movl	%edx, 70071644(%eax)
	movl	_last_arg+8, %edx
	movl	%edx, 70071648(%eax)
	movl	_last_arg+12, %edx
	movl	%edx, 70071652(%eax)
	movl	_my_trace, %eax
	movl	$0, 70072376(%eax)
	movl	$4, 70072380(%eax)
	movb	$2, 70072384(%eax)
	movb	$0, 70072385(%eax)
	movl	$-16711936, 70072388(%eax)
	movl	_my_trace, %eax
	movl	_last_arg, %edx
	movl	%edx, 70072392(%eax)
	movl	_last_arg+4, %edx
	movl	%edx, 70072396(%eax)
	movl	_last_arg+8, %edx
	movl	%edx, 70072400(%eax)
	movl	_last_arg+12, %edx
	movl	%edx, 70072404(%eax)
	movl	_my_trace, %eax
	movl	_last_arg, %edx
	movl	%edx, 70073656(%eax)
	movl	_last_arg+4, %edx
	movl	%edx, 70073660(%eax)
	movl	_last_arg+8, %edx
	movl	%edx, 70073664(%eax)
	movl	_last_arg+12, %edx
	movl	%edx, 70073668(%eax)
	movl	_my_trace, %eax
	movl	$2, 70074680(%eax)
	movl	$4, 70074684(%eax)
	movb	$2, 70074688(%eax)
	movb	$0, 70074689(%eax)
	movl	$-16711936, 70074692(%eax)
	movl	_my_trace, %eax
	movl	_last_arg, %edx
	movl	%edx, 70074696(%eax)
	movl	_last_arg+4, %edx
	movl	%edx, 70074700(%eax)
	movl	_last_arg+8, %edx
	movl	%edx, 70074704(%eax)
	movl	_last_arg+12, %edx
	movl	%edx, 70074708(%eax)
	movl	_my_trace, %eax
	movl	$0, 70075448(%eax)
	movl	$4, 70075452(%eax)
	movb	$2, 70075456(%eax)
	movb	$0, 70075457(%eax)
	movl	$-16711936, 70075460(%eax)
	movl	_my_trace, %eax
	movl	_last_arg, %edx
	movl	%edx, 70075464(%eax)
	movl	_last_arg+4, %edx
	movl	%edx, 70075468(%eax)
	movl	_last_arg+8, %edx
	movl	%edx, 70075472(%eax)
	movl	_last_arg+12, %edx
	movl	%edx, 70075476(%eax)
	movl	_my_trace, %eax
	movl	$0, 70076216(%eax)
	movl	$4, 70076220(%eax)
	movb	$2, 70076224(%eax)
	movb	$0, 70076225(%eax)
	movl	$-16711936, 70076228(%eax)
	movl	_my_trace, %eax
	movl	_last_arg, %edx
	movl	%edx, 70076232(%eax)
	movl	_last_arg+4, %edx
	movl	%edx, 70076236(%eax)
	movl	_last_arg+8, %edx
	movl	%edx, 70076240(%eax)
	movl	_last_arg+12, %edx
	movl	%edx, 70076244(%eax)
	movl	_my_trace, %eax
	movl	$1, 70080824(%eax)
	movl	$4, 70080828(%eax)
	movb	$2, 70080832(%eax)
	movb	$0, 70080833(%eax)
	movl	$-16711936, 70080836(%eax)
	movl	_my_trace, %eax
	movl	$3, 70080840(%eax)
	movl	$4, 70080844(%eax)
	movb	$2, 70080848(%eax)
	movb	$0, 70080849(%eax)
	movl	$-16711936, 70080852(%eax)
	movl	_my_trace, %eax
	movl	$4, 70080856(%eax)
	movl	$4, 70080860(%eax)
	movb	$2, 70080864(%eax)
	movb	$0, 70080865(%eax)
	movl	$-16711936, 70080868(%eax)
	movl	_my_trace, %eax
	movl	_last_arg, %edx
	movl	%edx, 70080872(%eax)
	movl	_last_arg+4, %edx
	movl	%edx, 70080876(%eax)
	movl	_last_arg+8, %edx
	movl	%edx, 70080880(%eax)
	movl	_last_arg+12, %edx
	movl	%edx, 70080884(%eax)
	movl	_my_trace, %eax
	movl	$0, 70081336(%eax)
	movl	$40, 70081340(%eax)
	movb	$2, 70081344(%eax)
	movb	$0, 70081345(%eax)
	movl	$-16711936, 70081348(%eax)
	movl	_my_trace, %eax
	movl	_last_arg, %edx
	movl	%edx, 70081352(%eax)
	movl	_last_arg+4, %edx
	movl	%edx, 70081356(%eax)
	movl	_last_arg+8, %edx
	movl	%edx, 70081360(%eax)
	movl	_last_arg+12, %edx
	movl	%edx, 70081364(%eax)
	movl	_my_trace, %eax
	movl	$1, 70082360(%eax)
	movl	$4, 70082364(%eax)
	movb	$2, 70082368(%eax)
	movb	$0, 70082369(%eax)
	movl	$-16711936, 70082372(%eax)
	movl	_my_trace, %eax
	movl	_last_arg, %edx
	movl	%edx, 70082376(%eax)
	movl	_last_arg+4, %edx
	movl	%edx, 70082380(%eax)
	movl	_last_arg+8, %edx
	movl	%edx, 70082384(%eax)
	movl	_last_arg+12, %edx
	movl	%edx, 70082388(%eax)
	movl	_my_trace, %eax
	movl	$4, 70082872(%eax)
	movl	$8, 70082876(%eax)
	movb	$2, 70082880(%eax)
	movb	$0, 70082881(%eax)
	movl	$-16711936, 70082884(%eax)
	movl	_my_trace, %eax
	movl	$5, 70082888(%eax)
	movl	$6, 70082892(%eax)
	movb	$2, 70082896(%eax)
	movb	$1, 70082897(%eax)
	movl	$-16711936, 70082900(%eax)
	movl	_my_trace, %eax
	movl	_last_arg, %edx
	movl	%edx, 70082904(%eax)
	movl	_last_arg+4, %edx
	movl	%edx, 70082908(%eax)
	movl	_last_arg+8, %edx
	movl	%edx, 70082912(%eax)
	movl	_last_arg+12, %edx
	movl	%edx, 70082916(%eax)
	movl	_my_trace, %eax
	movl	$1, 70084920(%eax)
	movl	$8, 70084924(%eax)
	movb	$2, 70084928(%eax)
	movb	$0, 70084929(%eax)
	movl	$-16711936, 70084932(%eax)
	movl	_my_trace, %eax
	movl	$2, 70084936(%eax)
	movl	$3, 70084940(%eax)
	movb	$2, 70084944(%eax)
	movb	$1, 70084945(%eax)
	movl	$0, 70084948(%eax)
	movl	_my_trace, %eax
	movl	_last_arg, %edx
	movl	%edx, 70084952(%eax)
	movl	_last_arg+4, %edx
	movl	%edx, 70084956(%eax)
	movl	_last_arg+8, %edx
	movl	%edx, 70084960(%eax)
	movl	_last_arg+12, %edx
	movl	%edx, 70084964(%eax)
	movl	_my_trace, %eax
	movl	$4, 70085688(%eax)
	movl	$4, 70085692(%eax)
	movb	$2, 70085696(%eax)
	movb	$0, 70085697(%eax)
	movl	$-16711936, 70085700(%eax)
	movl	_my_trace, %eax
	movl	$2, 70085704(%eax)
	movl	$3, 70085708(%eax)
	movb	$2, 70085712(%eax)
	movb	$2, 70085713(%eax)
	movl	$0, 70085716(%eax)
	movl	_my_trace, %eax
	movl	_last_arg, %edx
	movl	%edx, 70085720(%eax)
	movl	_last_arg+4, %edx
	movl	%edx, 70085724(%eax)
	movl	_last_arg+8, %edx
	movl	%edx, 70085728(%eax)
	movl	_last_arg+12, %edx
	movl	%edx, 70085732(%eax)
	movl	_my_trace, %eax
	movl	$4, 70086456(%eax)
	movl	$4, 70086460(%eax)
	movb	$2, 70086464(%eax)
	movb	$0, 70086465(%eax)
	movl	$-16711936, 70086468(%eax)
	movl	_my_trace, %eax
	movl	$2, 70086472(%eax)
	movl	$4, 70086476(%eax)
	movb	$2, 70086480(%eax)
	movb	$2, 70086481(%eax)
	movl	$0, 70086484(%eax)
	movl	_my_trace, %eax
	movl	_last_arg, %edx
	movl	%edx, 70086488(%eax)
	movl	_last_arg+4, %edx
	movl	%edx, 70086492(%eax)
	movl	_last_arg+8, %edx
	movl	%edx, 70086496(%eax)
	movl	_last_arg+12, %edx
	movl	%edx, 70086500(%eax)
	movl	_my_trace, %eax
	movl	$0, 70090040(%eax)
	movl	$8, 70090044(%eax)
	movb	$2, 70090048(%eax)
	movb	$2, 70090049(%eax)
	movl	$-16711936, 70090052(%eax)
	movl	_my_trace, %eax
	movl	$1, 70090056(%eax)
	movl	$8, 70090060(%eax)
	movb	$2, 70090064(%eax)
	movb	$2, 70090065(%eax)
	movl	$-16711936, 70090068(%eax)
	movl	_my_trace, %eax
	movl	_last_arg, %edx
	movl	%edx, 70090072(%eax)
	movl	_last_arg+4, %edx
	movl	%edx, 70090076(%eax)
	movl	_last_arg+8, %edx
	movl	%edx, 70090080(%eax)
	movl	_last_arg+12, %edx
	movl	%edx, 70090084(%eax)
	movl	_my_trace, %eax
	movl	$2, 70090808(%eax)
	movl	$4, 70090812(%eax)
	movb	$3, 70090816(%eax)
	movb	$2, 70090817(%eax)
	movl	$-16711936, 70090820(%eax)
	movl	_my_trace, %eax
	movl	$4, 70090824(%eax)
	movl	$4, 70090828(%eax)
	movb	$2, 70090832(%eax)
	movb	$2, 70090833(%eax)
	movl	$0, 70090836(%eax)
	movl	_my_trace, %eax
	movl	_last_arg, %edx
	movl	%edx, 70090840(%eax)
	movl	_last_arg+4, %edx
	movl	%edx, 70090844(%eax)
	movl	_last_arg+8, %edx
	movl	%edx, 70090848(%eax)
	movl	_last_arg+12, %edx
	movl	%edx, 70090852(%eax)
	movl	_my_trace, %eax
	movl	$1, 70092600(%eax)
	movl	$2, 70092604(%eax)
	movb	$2, 70092608(%eax)
	movb	$1, 70092609(%eax)
	movl	$0, 70092612(%eax)
	movl	_my_trace, %eax
	movl	$3, 70092616(%eax)
	movl	$4, 70092620(%eax)
	movb	$2, 70092624(%eax)
	movb	$0, 70092625(%eax)
	movl	$0, 70092628(%eax)
	movl	_my_trace, %eax
	movl	_last_arg, %edx
	movl	%edx, 70092632(%eax)
	movl	_last_arg+4, %edx
	movl	%edx, 70092636(%eax)
	movl	_last_arg+8, %edx
	movl	%edx, 70092640(%eax)
	movl	_last_arg+12, %edx
	movl	%edx, 70092644(%eax)
	movl	_my_trace, %eax
	movl	$0, 70093112(%eax)
	movl	$8, 70093116(%eax)
	movb	$2, 70093120(%eax)
	movb	$0, 70093121(%eax)
	movl	$0, 70093124(%eax)
	movl	_my_trace, %eax
	movl	_last_arg, %edx
	movl	%edx, 70093128(%eax)
	movl	_last_arg+4, %edx
	movl	%edx, 70093132(%eax)
	movl	_last_arg+8, %edx
	movl	%edx, 70093136(%eax)
	movl	_last_arg+12, %edx
	movl	%edx, 70093140(%eax)
	movl	_my_trace, %eax
	movl	$3, 70093880(%eax)
	movl	$5, 70093884(%eax)
	movb	$2, 70093888(%eax)
	movb	$2, 70093889(%eax)
	movl	$0, 70093892(%eax)
	movl	_my_trace, %eax
	movl	$3, 70093896(%eax)
	movl	$4, 70093900(%eax)
	movb	$2, 70093904(%eax)
	movb	$0, 70093905(%eax)
	movl	$-16711936, 70093908(%eax)
	movl	_my_trace, %eax
	movl	$5, 70093912(%eax)
	movl	$4, 70093916(%eax)
	movb	$2, 70093920(%eax)
	movb	$0, 70093921(%eax)
	movl	$-16711936, 70093924(%eax)
	movl	_my_trace, %eax
	movl	_last_arg, %edx
	movl	%edx, 70093928(%eax)
	movl	_last_arg+4, %edx
	movl	%edx, 70093932(%eax)
	movl	_last_arg+8, %edx
	movl	%edx, 70093936(%eax)
	movl	_last_arg+12, %edx
	movl	%edx, 70093940(%eax)
	movl	_my_trace, %eax
	movl	$1, 70094392(%eax)
	movl	$8, 70094396(%eax)
	movb	$2, 70094400(%eax)
	movb	$0, 70094401(%eax)
	movl	$-16711936, 70094404(%eax)
	movl	_my_trace, %eax
	movl	$2, 70094408(%eax)
	movl	$3, 70094412(%eax)
	movb	$2, 70094416(%eax)
	movb	$1, 70094417(%eax)
	movl	$-16711936, 70094420(%eax)
	movl	_my_trace, %eax
	movl	_last_arg, %edx
	movl	%edx, 70094424(%eax)
	movl	_last_arg+4, %edx
	movl	%edx, 70094428(%eax)
	movl	_last_arg+8, %edx
	movl	%edx, 70094432(%eax)
	movl	_last_arg+12, %edx
	movl	%edx, 70094436(%eax)
	movl	_my_trace, %eax
	movl	$0, 70095672(%eax)
	movl	$4, 70095676(%eax)
	movb	$2, 70095680(%eax)
	movb	$0, 70095681(%eax)
	movl	$-16711936, 70095684(%eax)
	movl	_my_trace, %eax
	movl	$4, 70095688(%eax)
	movl	$8, 70095692(%eax)
	movb	$2, 70095696(%eax)
	movb	$0, 70095697(%eax)
	movl	$-16711936, 70095700(%eax)
	movl	_my_trace, %eax
	movl	$5, 70095704(%eax)
	movl	$4, 70095708(%eax)
	movb	$2, 70095712(%eax)
	movb	$0, 70095713(%eax)
	movl	$-16711936, 70095716(%eax)
	movl	_my_trace, %eax
	movl	$5, 70095720(%eax)
	movl	$6, 70095724(%eax)
	movb	$2, 70095728(%eax)
	movb	$1, 70095729(%eax)
	movl	$0, 70095732(%eax)
	movl	_my_trace, %eax
	movl	_last_arg, %edx
	movl	%edx, 70095736(%eax)
	movl	_last_arg+4, %edx
	movl	%edx, 70095740(%eax)
	movl	_last_arg+8, %edx
	movl	%edx, 70095744(%eax)
	movl	_last_arg+12, %edx
	movl	%edx, 70095748(%eax)
	movl	_my_trace, %eax
	movl	$1, 70102328(%eax)
	movl	$384, 70102332(%eax)
	movb	$2, 70102336(%eax)
	movb	$0, 70102337(%eax)
	movl	$0, 70102340(%eax)
	movl	_my_trace, %eax
	movl	$2, 70102344(%eax)
	movl	$384, 70102348(%eax)
	movb	$2, 70102352(%eax)
	movb	$0, 70102353(%eax)
	movl	$0, 70102356(%eax)
	movl	_my_trace, %eax
	movl	_last_arg, %edx
	movl	%edx, 70102360(%eax)
	movl	_last_arg+4, %edx
	movl	%edx, 70102364(%eax)
	movl	_last_arg+8, %edx
	movl	%edx, 70102368(%eax)
	movl	_last_arg+12, %edx
	movl	%edx, 70102372(%eax)
	movl	_my_trace, %eax
	movl	$1, 70110008(%eax)
	movl	$8, 70110012(%eax)
	movb	$2, 70110016(%eax)
	movb	$0, 70110017(%eax)
	movl	$-16711936, 70110020(%eax)
	movl	_my_trace, %eax
	movl	_last_arg, %edx
	movl	%edx, 70110024(%eax)
	movl	_last_arg+4, %edx
	movl	%edx, 70110028(%eax)
	movl	_last_arg+8, %edx
	movl	%edx, 70110032(%eax)
	movl	_last_arg+12, %edx
	movl	%edx, 70110036(%eax)
	movl	_my_trace, %eax
	movl	$3, 70121784(%eax)
	movl	$4, 70121788(%eax)
	movb	$2, 70121792(%eax)
	movb	$0, 70121793(%eax)
	movl	$0, 70121796(%eax)
	movl	_my_trace, %eax
	movl	$5, 70121800(%eax)
	movl	$4, 70121804(%eax)
	movb	$2, 70121808(%eax)
	movb	$0, 70121809(%eax)
	movl	$0, 70121812(%eax)
	movl	_my_trace, %eax
	movl	_last_arg, %edx
	movl	%edx, 70121816(%eax)
	movl	_last_arg+4, %edx
	movl	%edx, 70121820(%eax)
	movl	_last_arg+8, %edx
	movl	%edx, 70121824(%eax)
	movl	_last_arg+12, %edx
	movl	%edx, 70121828(%eax)
	movl	_my_trace, %eax
	movl	_last_arg, %edx
	movl	%edx, 70124344(%eax)
	movl	_last_arg+4, %edx
	movl	%edx, 70124348(%eax)
	movl	_last_arg+8, %edx
	movl	%edx, 70124352(%eax)
	movl	_last_arg+12, %edx
	movl	%edx, 70124356(%eax)
	movl	_my_trace, %eax
	movl	_last_arg, %edx
	movl	%edx, 70125880(%eax)
	movl	_last_arg+4, %edx
	movl	%edx, 70125884(%eax)
	movl	_last_arg+8, %edx
	movl	%edx, 70125888(%eax)
	movl	_last_arg+12, %edx
	movl	%edx, 70125892(%eax)
	movl	_my_trace, %eax
	movl	$4, 70127928(%eax)
	movl	$4, 70127932(%eax)
	movb	$2, 70127936(%eax)
	movb	$0, 70127937(%eax)
	movl	$0, 70127940(%eax)
	movl	_my_trace, %eax
	movl	_last_arg, %edx
	movl	%edx, 70127944(%eax)
	movl	_last_arg+4, %edx
	movl	%edx, 70127948(%eax)
	movl	_last_arg+8, %edx
	movl	%edx, 70127952(%eax)
	movl	_last_arg+12, %edx
	movl	%edx, 70127956(%eax)
	movl	$0, %eax
	leave
	ret
	.def	___main;	.scl	2;	.type	32;	.endef
	.section .rdata,"dr"
LC437:
	.ascii "%d - %s\12\0"
	.align 4
LC438:
	.ascii "You need do provide host and port\12\0"
LC439:
	.ascii "Version 3.0\12\0"
	.align 4
LC440:
	.ascii "Unable to allocate trace config\12\0"
LC441:
	.ascii "\12Initialising Winsock...\0"
LC442:
	.ascii "Failed. Error Code : %d\0"
LC443:
	.ascii "Initialised\12\0"
LC444:
	.ascii "Could not create socket : %d\0"
LC445:
	.ascii "Socket created\12\0"
	.align 4
LC446:
	.ascii "ioctlsocket failed with error: %ld\12\0"
LC447:
	.ascii "127.0.0.1\0"
LC448:
	.ascii "connect error\0"
LC449:
	.ascii "Connected\12\0"
LC450:
	.ascii "-=OK=-\0"
LC451:
	.ascii "quit\0"
	.text
	.globl	_main
	.def	_main;	.scl	2;	.type	32;	.endef
_main:
	leal	4(%esp), %ecx
	andl	$-16, %esp
	pushl	-4(%ecx)
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%ebx
	pushl	%ecx
	subl	$2000, %esp
	movl	%ecx, %ebx
	call	___main
	movl	$0, -12(%ebp)
	movl	$0, -12(%ebp)
	jmp	L868
L869:
	movl	-12(%ebp), %eax
	leal	0(,%eax,4), %edx
	movl	4(%ebx), %eax
	addl	%edx, %eax
	movl	(%eax), %eax
	movl	%eax, 8(%esp)
	movl	-12(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC437, (%esp)
	call	__Z7d_printPKcz
	addl	$1, -12(%ebp)
L868:
	movl	-12(%ebp), %eax
	cmpl	(%ebx), %eax
	jl	L869
	cmpl	$2, (%ebx)
	jg	L870
	movl	$LC438, (%esp)
	call	__Z7d_printPKcz
	movl	$-1, %eax
	jmp	L887
L870:
	movl	$LC439, (%esp)
	call	__Z7d_printPKcz
	movl	4(%ebx), %eax
	addl	$4, %eax
	movl	(%eax), %eax
	movl	%eax, (%esp)
	call	_strlen
	cmpl	$768, %eax
	jbe	L872
	movl	$-1, %eax
	jmp	L887
L872:
	movl	4(%ebx), %eax
	addl	$8, %eax
	movl	(%eax), %eax
	movl	%eax, (%esp)
	call	_strlen
	cmpl	$768, %eax
	jbe	L873
	movl	$-1, %eax
	jmp	L887
L873:
	movl	$86803128, (%esp)
	call	_malloc
	movl	%eax, _my_trace
	movl	_my_trace, %eax
	testl	%eax, %eax
	jne	L874
	movl	$LC440, (%esp)
	call	__Z7d_printPKcz
	movl	$-1, %eax
	jmp	L887
L874:
	movl	4(%ebx), %eax
	addl	$8, %eax
	movl	(%eax), %eax
	movl	%eax, (%esp)
	call	_atoi
	movswl	%ax, %ecx
	movl	4(%ebx), %eax
	addl	$4, %eax
	movl	(%eax), %edx
	movl	_my_trace, %eax
	movl	%ecx, 8(%esp)
	movl	%edx, 4(%esp)
	movl	%eax, (%esp)
	call	__Z10init_traceP13TRACE_CONFIG_Pcs
	call	__Z18configure_syscallsv
	movl	_my_trace, %eax
	movl	$__Z23react_sysenter_callbackPv, 70021908(%eax)
	movl	_my_trace, %eax
	movl	$__Z21react_sysret_callbackPv, 70021912(%eax)
	movl	_my_trace, %eax
	movl	$__Z20react_sysret_refreshPv, 70021916(%eax)
	movl	_my_trace, %eax
	movl	$__Z22react_sample_routine_1Pv, 70022928(%eax)
	movl	_my_trace, %eax
	movl	$__Z13react_zero_SFPv, 70022932(%eax)
	movl	_my_trace, %eax
	movl	$__Z12react_set_ZFPv, 70022936(%eax)
	movl	_my_trace, %eax
	movl	$__Z13react_zero_ZFPv, 70022940(%eax)
	movl	_my_trace, %eax
	movl	$__Z14react_zero_EAXPv, 70022944(%eax)
	movl	_my_trace, %eax
	movl	$__Z13react_flip_ZFPv, 70022948(%eax)
	movl	_my_trace, %eax
	movl	$__Z13react_flip_CFPv, 70022952(%eax)
	movl	_my_trace, %eax
	movl	$__Z13react_flip_SFPv, 70022956(%eax)
	movl	_my_trace, %eax
	movl	$__Z13react_zero_CFPv, 70022960(%eax)
	movl	_my_trace, %eax
	movl	$__Z12react_set_CFPv, 70022964(%eax)
	movl	_my_trace, %eax
	movl	$__Z21react_update_region_1Pv, 70023956(%eax)
	movl	_my_trace, %eax
	movl	$__Z21react_cry_antidebug_1Pv, 70023960(%eax)
	movl	_my_trace, %eax
	movl	$__Z13react_skip_onPv, 70023964(%eax)
	movl	_my_trace, %eax
	movl	$__Z14react_skip_offPv, 70023968(%eax)
	movl	_my_trace, %eax
	movl	$__Z21react_update_region_2Pv, 70023972(%eax)
	movl	_my_trace, %eax
	movl	$__Z18react_output_arg_0Pv, 70024976(%eax)
	movl	_my_trace, %eax
	movl	$__Z18react_output_arg_1Pv, 70024980(%eax)
	movl	_my_trace, %eax
	movl	$__Z18react_output_arg_2Pv, 70024984(%eax)
	movl	_my_trace, %eax
	movl	$__Z18react_output_arg_3Pv, 70024988(%eax)
	movl	_my_trace, %eax
	movl	$__Z18react_output_arg_4Pv, 70024992(%eax)
	movl	_my_trace, %eax
	movl	$__Z18react_output_arg_5Pv, 70024996(%eax)
	movl	_my_trace, %eax
	movl	$__Z18react_output_arg_6Pv, 70025000(%eax)
	movl	_my_trace, %eax
	movl	$__Z18react_output_arg_7Pv, 70025004(%eax)
	movl	_my_trace, %eax
	movl	$__Z18react_output_arg_8Pv, 70025008(%eax)
	movl	_my_trace, %eax
	movl	$__Z22react_output_arg_str_0Pv, 70025040(%eax)
	movl	_my_trace, %eax
	movl	$__Z22react_output_arg_str_1Pv, 70025044(%eax)
	movl	_my_trace, %eax
	movl	$__Z22react_output_arg_str_2Pv, 70025048(%eax)
	movl	_my_trace, %eax
	movl	$__Z22react_output_arg_str_3Pv, 70025052(%eax)
	movl	_my_trace, %eax
	movl	$__Z22react_output_arg_str_4Pv, 70025056(%eax)
	movl	_my_trace, %eax
	movl	$__Z22react_output_arg_str_5Pv, 70025060(%eax)
	movl	_my_trace, %eax
	movl	$__Z22react_output_arg_str_6Pv, 70025064(%eax)
	movl	_my_trace, %eax
	movl	$__Z22react_output_arg_str_7Pv, 70025068(%eax)
	movl	_my_trace, %eax
	movl	$__Z22react_output_arg_str_8Pv, 70025072(%eax)
	movl	_my_trace, %eax
	movl	$__Z30react_output_arg_unicode_str_0Pv, 70025104(%eax)
	movl	_my_trace, %eax
	movl	$__Z30react_output_arg_unicode_str_1Pv, 70025108(%eax)
	movl	_my_trace, %eax
	movl	$__Z30react_output_arg_unicode_str_2Pv, 70025112(%eax)
	movl	_my_trace, %eax
	movl	$__Z30react_output_arg_unicode_str_3Pv, 70025116(%eax)
	movl	_my_trace, %eax
	movl	$__Z30react_output_arg_unicode_str_4Pv, 70025120(%eax)
	movl	_my_trace, %eax
	movl	$__Z30react_output_arg_unicode_str_5Pv, 70025124(%eax)
	movl	_my_trace, %eax
	movl	$__Z30react_output_arg_unicode_str_6Pv, 70025128(%eax)
	movl	_my_trace, %eax
	movl	$__Z30react_output_arg_unicode_str_7Pv, 70025132(%eax)
	movl	_my_trace, %eax
	movl	$__Z30react_output_arg_unicode_str_8Pv, 70025136(%eax)
	movl	_my_trace, %eax
	movl	$__Z16react_output_eaxPv, 70025168(%eax)
	movl	_my_trace, %eax
	movl	$__Z16react_output_ebxPv, 70025172(%eax)
	movl	_my_trace, %eax
	movl	$__Z16react_output_ecxPv, 70025176(%eax)
	movl	_my_trace, %eax
	movl	$__Z16react_output_edxPv, 70025180(%eax)
	movl	_my_trace, %eax
	movl	$__Z16react_output_esiPv, 70025184(%eax)
	movl	_my_trace, %eax
	movl	$__Z16react_output_ediPv, 70025188(%eax)
	movl	_my_trace, %eax
	movl	$__Z16react_output_espPv, 70025192(%eax)
	movl	_my_trace, %eax
	movl	$__Z16react_output_eipPv, 70025196(%eax)
	movl	_my_trace, %eax
	movl	$__Z20react_output_p_arg_0Pv, 70025232(%eax)
	movl	_my_trace, %eax
	movl	$__Z20react_output_p_arg_1Pv, 70025236(%eax)
	movl	_my_trace, %eax
	movl	$__Z20react_output_p_arg_2Pv, 70025240(%eax)
	movl	_my_trace, %eax
	movl	$__Z20react_output_p_arg_3Pv, 70025244(%eax)
	movl	_my_trace, %eax
	movl	$__Z20react_output_p_arg_4Pv, 70025248(%eax)
	movl	_my_trace, %eax
	movl	$__Z20react_output_p_arg_5Pv, 70025252(%eax)
	movl	_my_trace, %eax
	movl	$__Z20react_output_p_arg_6Pv, 70025256(%eax)
	movl	_my_trace, %eax
	movl	$__Z20react_output_p_arg_7Pv, 70025260(%eax)
	movl	_my_trace, %eax
	movl	$__Z20react_output_p_arg_8Pv, 70025264(%eax)
	movl	$LC441, (%esp)
	call	__Z7d_printPKcz
	leal	-1984(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$514, (%esp)
	movl	__imp__WSAStartup@8, %eax
	call	*%eax
	subl	$8, %esp
	testl	%eax, %eax
	setne	%al
	testb	%al, %al
	je	L875
	movl	__imp__WSAGetLastError@0, %eax
	call	*%eax
	movl	%eax, 4(%esp)
	movl	$LC442, (%esp)
	call	__Z7d_printPKcz
	movl	$1, %eax
	jmp	L887
L875:
	movl	$LC443, (%esp)
	call	__Z7d_printPKcz
	movl	$0, 8(%esp)
	movl	$1, 4(%esp)
	movl	$2, (%esp)
	movl	__imp__socket@12, %eax
	call	*%eax
	subl	$12, %esp
	movl	%eax, -20(%ebp)
	cmpl	$-1, -20(%ebp)
	sete	%al
	testb	%al, %al
	je	L876
	movl	__imp__WSAGetLastError@0, %eax
	call	*%eax
	movl	%eax, 4(%esp)
	movl	$LC444, (%esp)
	call	__Z7d_printPKcz
L876:
	movl	$LC445, (%esp)
	call	__Z7d_printPKcz
	movl	$0, -48(%ebp)
	movl	$4, %eax
	andl	$127, %eax
	sall	$16, %eax
	orl	$-2147457410, %eax
	movl	%eax, %edx
	leal	-48(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	%edx, 4(%esp)
	movl	-20(%ebp), %eax
	movl	%eax, (%esp)
	movl	__imp__ioctlsocket@12, %eax
	call	*%eax
	subl	$12, %esp
	movl	%eax, -24(%ebp)
	cmpl	$0, -24(%ebp)
	je	L877
	movl	-24(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC446, (%esp)
	call	__Z7d_printPKcz
L877:
	movl	$LC447, (%esp)
	movl	__imp__inet_addr@4, %eax
	call	*%eax
	subl	$4, %esp
	movl	%eax, -40(%ebp)
	movw	$2, -44(%ebp)
	movl	$12341, (%esp)
	movl	__imp__htons@4, %eax
	call	*%eax
	subl	$4, %esp
	movw	%ax, -42(%ebp)
	movl	$4, %eax
	andl	$127, %eax
	sall	$16, %eax
	orl	$-2147457410, %eax
	movl	%eax, %edx
	leal	-48(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	%edx, 4(%esp)
	movl	-20(%ebp), %eax
	movl	%eax, (%esp)
	movl	__imp__ioctlsocket@12, %eax
	call	*%eax
	subl	$12, %esp
	movl	%eax, -24(%ebp)
	cmpl	$0, -24(%ebp)
	je	L878
	movl	-24(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC446, (%esp)
	call	__Z7d_printPKcz
L878:
	movl	$16, %eax
	movl	%eax, 8(%esp)
	leal	-44(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	-20(%ebp), %eax
	movl	%eax, (%esp)
	movl	__imp__connect@12, %eax
	call	*%eax
	subl	$12, %esp
	shrl	$31, %eax
	testb	%al, %al
	je	L879
	movl	$LC448, (%esp)
	call	__Z7d_printPKcz
	movl	$1, %eax
	jmp	L887
L879:
	movl	_my_trace, %eax
	movl	-20(%ebp), %edx
	movl	%edx, 70012892(%eax)
	movl	$4, %eax
	andl	$127, %eax
	sall	$16, %eax
	orl	$-2147457410, %eax
	movl	%eax, %edx
	leal	-48(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	%edx, 4(%esp)
	movl	-20(%ebp), %eax
	movl	%eax, (%esp)
	movl	__imp__ioctlsocket@12, %eax
	call	*%eax
	subl	$12, %esp
	movl	%eax, -24(%ebp)
	cmpl	$0, -24(%ebp)
	je	L880
	movl	-24(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$LC446, (%esp)
	call	__Z7d_printPKcz
L880:
	movl	$LC449, (%esp)
	call	__Z7d_printPKcz
L886:
	movl	$0, %eax
	movb	%al, -1584(%ebp)
	movl	$0, %eax
	movb	%al, -816(%ebp)
	movl	$0, -16(%ebp)
L883:
	movl	$0, 12(%esp)
	movl	$767, 8(%esp)
	leal	-816(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	-20(%ebp), %eax
	movl	%eax, (%esp)
	movl	__imp__recv@16, %eax
	call	*%eax
	subl	$16, %esp
	movl	%eax, -28(%ebp)
	leal	-816(%ebp), %eax
	movl	%eax, 4(%esp)
	leal	-1584(%ebp), %eax
	movl	%eax, (%esp)
	call	_strcat
	movl	-28(%ebp), %eax
	addl	%eax, -16(%ebp)
	movl	-16(%ebp), %eax
	leal	-6(%eax), %edx
	leal	-1584(%ebp), %eax
	addl	%edx, %eax
	movl	$6, 8(%esp)
	movl	$LC450, 4(%esp)
	movl	%eax, (%esp)
	call	_strncmp
	testl	%eax, %eax
	jne	L881
	nop
	movl	-16(%ebp), %eax
	subl	$6, %eax
	movb	$0, -1584(%ebp,%eax)
	movl	$LC451, 4(%esp)
	leal	-1584(%ebp), %eax
	movl	%eax, (%esp)
	call	_strcmp
	testl	%eax, %eax
	jne	L884
	jmp	L888
L881:
	jmp	L883
L888:
	nop
	movl	$0, %eax
	jmp	L887
L884:
	leal	-1584(%ebp), %eax
	movl	%eax, (%esp)
	call	__Z10handle_cmdPc
	jmp	L886
L887:
	leal	-8(%ebp), %esp
	popl	%ecx
	popl	%ebx
	popl	%ebp
	leal	-4(%ecx), %esp
	ret
	.ident	"GCC: (GNU) 4.9.3"
	.def	_vfprintf;	.scl	2;	.type	32;	.endef
	.def	_fflush;	.scl	2;	.type	32;	.endef
	.def	_ftell;	.scl	2;	.type	32;	.endef
	.def	_sprintf;	.scl	2;	.type	32;	.endef
	.def	_strcpy;	.scl	2;	.type	32;	.endef
	.def	_strcpy;	.scl	2;	.type	32;	.endef
	.def	_strlen;	.scl	2;	.type	32;	.endef
	.def	_malloc;	.scl	2;	.type	32;	.endef
	.def	_free;	.scl	2;	.type	32;	.endef
	.def	_strchr;	.scl	2;	.type	32;	.endef
	.def	_strtok;	.scl	2;	.type	32;	.endef
	.def	_strcmp;	.scl	2;	.type	32;	.endef
	.def	_fwrite;	.scl	2;	.type	32;	.endef
	.def	_sscanf;	.scl	2;	.type	32;	.endef
	.def	_strcmpi;	.scl	2;	.type	32;	.endef
	.def	_CreateToolhelp32Snapshot@8;	.scl	2;	.type	32;	.endef
	.def	_Thread32First@8;	.scl	2;	.type	32;	.endef
	.def	_Thread32Next@8;	.scl	2;	.type	32;	.endef
	.def	_exit;	.scl	2;	.type	32;	.endef
	.def	_memset;	.scl	2;	.type	32;	.endef
	.def	_fopen;	.scl	2;	.type	32;	.endef
	.def	_fclose;	.scl	2;	.type	32;	.endef
	.def	_itoa;	.scl	2;	.type	32;	.endef
	.def	_strcspn;	.scl	2;	.type	32;	.endef
	.def	_putchar;	.scl	2;	.type	32;	.endef
	.def	_strtoul;	.scl	2;	.type	32;	.endef
	.def	_strstr;	.scl	2;	.type	32;	.endef
	.def	_fgets;	.scl	2;	.type	32;	.endef
	.def	_strcat;	.scl	2;	.type	32;	.endef
	.def	_memcpy;	.scl	2;	.type	32;	.endef
	.def	_strtol;	.scl	2;	.type	32;	.endef
	.def	_strncmp;	.scl	2;	.type	32;	.endef
	.def	_setvbuf;	.scl	2;	.type	32;	.endef
	.def	_atoi;	.scl	2;	.type	32;	.endef
