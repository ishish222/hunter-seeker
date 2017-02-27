	.file	"trace6_64.c"
	.globl	my_trace
	.bss
	.align 8
my_trace:
	.space 8
	.globl	spawned
spawned:
	.space 1
	.globl	log
	.align 8
log:
	.space 8
	.globl	started
started:
	.space 1
	.globl	instr_count_s
	.align 32
instr_count_s:
	.space 32
	.globl	full_log
	.align 4
full_log:
	.space 4
	.globl	scan_on
	.align 4
scan_on:
	.space 4
	.globl	scan_count
	.align 4
scan_count:
	.space 4
	.globl	last_read_record
	.align 32
last_read_record:
	.space 40
	.globl	watched
	.align 64
watched:
	.space 8192
	.globl	file_handle
	.align 8
file_handle:
	.space 8
	.globl	sysenter_esp
	.align 8
sysenter_esp:
	.space 8
	.globl	sysenter_no
	.align 8
sysenter_no:
	.space 8
	.globl	file
	.align 8
file:
	.space 8
	.globl	buffer
	.align 64
buffer:
	.space 1048576
	.globl	index
	.align 4
index:
	.space 4
	.globl	instr
	.align 64
instr:
	.space 96
	.globl	target_lib
	.section .rdata,"dr"
	.align 2
.LC0:
	.ascii "n\0t\0d\0l\0l\0.\0d\0l\0l\0\0\0"
	.data
	.align 8
target_lib:
	.quad	.LC0
	.globl	target_lib_off
	.bss
	.align 8
target_lib_off:
	.space 8
	.globl	target_call
	.section .rdata,"dr"
.LC1:
	.ascii "NtCreateFile\0"
	.data
	.align 8
target_call:
	.quad	.LC1
	.globl	target_call_off
	.align 8
target_call_off:
	.quad	281104
	.globl	target_off
	.bss
	.align 8
target_off:
	.space 8
	.globl	cpdi
	.align 64
cpdi:
	.space 72
	.globl	myPID
	.align 4
myPID:
	.space 4
	.globl	myTID
	.align 4
myTID:
	.space 4
	.globl	img_base
	.align 8
img_base:
	.space 8
	.globl	nt1_off
	.align 8
nt1_off:
	.space 8
	.globl	nt2_off
	.align 8
nt2_off:
	.space 8
	.globl	nt3_off
	.align 8
nt3_off:
	.space 8
	.globl	nt4_off
	.align 8
nt4_off:
	.space 8
	.globl	sysenter_off
	.align 8
sysenter_off:
	.space 8
	.globl	sysret_off
	.align 8
sysret_off:
	.space 8
	.globl	last_arg
	.data
	.align 16
last_arg:
	.quad	0
	.quad	0
	.byte	6
	.byte	6
	.space 2
	.long	0
	.globl	last_location
	.bss
	.align 16
last_location:
	.space 16
	.globl	syscall_out_args
	.align 64
syscall_out_args:
	.space 25165824
	.globl	syscall_out_args_dump_list
	.align 64
syscall_out_args_dump_list:
	.space 256
	.globl	buffer_addr
	.align 8
buffer_addr:
	.space 8
	.globl	size_addr
	.align 4
size_addr:
	.space 4
	.globl	threads
	.align 64
threads:
	.space 137438953472
	.globl	blacklist_lib
	.section .rdata,"dr"
.LC2:
	.ascii "kernel32.dll\0"
.LC3:
	.ascii "ntdll.dll\0"
.LC4:
	.ascii "user32.dll\0"
	.data
	.align 16
blacklist_lib:
	.quad	.LC2
	.quad	.LC3
	.quad	.LC4
	.globl	blacklist_addr
	.bss
	.align 8
blacklist_addr:
	.space 1
	.globl	line2
	.align 64
line2:
	.space 768
	.text
	.globl	_Z7d_printPKcz
	.def	_Z7d_printPKcz;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z7d_printPKcz
_Z7d_printPKcz:
.LFB2381:
	pushq	%rbp
	.seh_pushreg	%rbp
	pushq	%rbx
	.seh_pushreg	%rbx
	subq	$824, %rsp
	.seh_stackalloc	824
	leaq	128(%rsp), %rbp
	.seh_setframe	%rbp, 128
	.seh_endprologue
	movq	%rdx, 728(%rbp)
	movq	%r8, 736(%rbp)
	movq	%r9, 744(%rbp)
	movq	%rcx, 720(%rbp)
	leaq	728(%rbp), %rax
	movq	%rax, 680(%rbp)
	movq	680(%rbp), %rbx
	movq	__imp___iob_func(%rip), %rax
	call	*%rax
	addq	$48, %rax
	movq	%rbx, %r8
	movq	720(%rbp), %rdx
	movq	%rax, %rcx
	call	vfprintf
	movq	__imp___iob_func(%rip), %rax
	call	*%rax
	addq	$48, %rax
	movq	%rax, %rcx
	call	fflush
	movl	$0, %eax
	addq	$824, %rsp
	popq	%rbx
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC5:
	.ascii "Sample routine 1\12\0"
	.text
	.globl	_Z22react_sample_routine_1Pv
	.def	_Z22react_sample_routine_1Pv;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z22react_sample_routine_1Pv
_Z22react_sample_routine_1Pv:
.LFB2382:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$32, %rsp
	.seh_stackalloc	32
	.seh_endprologue
	movq	%rcx, 16(%rbp)
	leaq	.LC5(%rip), %rcx
	call	_Z7d_printPKcz
	nop
	addq	$32, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC6:
	.ascii "Setting ZF\12\0"
	.align 8
.LC7:
	.ascii "Failed to get context, error: 0x%08x\12\0"
.LC8:
	.ascii "before flipping: 0x%08x\12\0"
.LC9:
	.ascii "after flipping: 0x%08x\12\0"
	.text
	.globl	_Z13react_flip_ZFPv
	.def	_Z13react_flip_ZFPv;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z13react_flip_ZFPv
_Z13react_flip_ZFPv:
.LFB2383:
	pushq	%rbp
	.seh_pushreg	%rbp
	subq	$1296, %rsp
	.seh_stackalloc	1296
	leaq	128(%rsp), %rbp
	.seh_setframe	%rbp, 128
	.seh_endprologue
	movq	%rcx, 1184(%rbp)
	leaq	.LC6(%rip), %rcx
	call	_Z7d_printPKcz
	movq	$-1, 1160(%rbp)
	movq	my_trace(%rip), %rax
	movl	68981632(%rax), %eax
	movl	%eax, 1156(%rbp)
	movq	my_trace(%rip), %rax
	movl	1156(%rbp), %edx
	addq	$56, %rdx
	movl	(%rax,%rdx,4), %eax
	movl	%eax, 1152(%rbp)
	movq	my_trace(%rip), %rax
	movl	1152(%rbp), %edx
	addq	$2097159, %rdx
	salq	$5, %rdx
	addq	%rdx, %rax
	movq	8(%rax), %rax
	movq	%rax, 1160(%rbp)
	movl	$1048577, -48(%rbp)
	leaq	-96(%rbp), %rdx
	movq	1160(%rbp), %rax
	movq	%rax, %rcx
	movq	__imp_GetThreadContext(%rip), %rax
	call	*%rax
	testl	%eax, %eax
	sete	%al
	testb	%al, %al
	je	.L6
	movq	__imp_GetLastError(%rip), %rax
	call	*%rax
	movl	%eax, %edx
	leaq	.LC7(%rip), %rcx
	call	_Z7d_printPKcz
.L6:
	movl	-28(%rbp), %eax
	movl	%eax, %edx
	leaq	.LC8(%rip), %rcx
	call	_Z7d_printPKcz
	leaq	-96(%rbp), %rax
	movq	%rax, %rcx
	call	_Z13print_contextP8_CONTEXT
	movl	-28(%rbp), %eax
	andl	$64, %eax
	movb	%al, 1151(%rbp)
	cmpb	$0, 1151(%rbp)
	je	.L7
	movl	-28(%rbp), %eax
	andl	$-65, %eax
	movl	%eax, -28(%rbp)
	jmp	.L8
.L7:
	movl	-28(%rbp), %eax
	orl	$64, %eax
	movl	%eax, -28(%rbp)
.L8:
	movl	-28(%rbp), %eax
	movl	%eax, %edx
	leaq	.LC9(%rip), %rcx
	call	_Z7d_printPKcz
	leaq	-96(%rbp), %rax
	movq	%rax, %rcx
	call	_Z13print_contextP8_CONTEXT
	leaq	-96(%rbp), %rdx
	movq	1160(%rbp), %rax
	movq	%rax, %rcx
	movq	__imp_SetThreadContext(%rip), %rax
	call	*%rax
	nop
	addq	$1296, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC10:
	.ascii "Setting CF\12\0"
.LC11:
	.ascii "before setting: 0x%08x\12\0"
.LC12:
	.ascii "after setting: 0x%08x\12\0"
	.text
	.globl	_Z12react_set_CFPv
	.def	_Z12react_set_CFPv;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z12react_set_CFPv
_Z12react_set_CFPv:
.LFB2384:
	pushq	%rbp
	.seh_pushreg	%rbp
	subq	$1280, %rsp
	.seh_stackalloc	1280
	leaq	128(%rsp), %rbp
	.seh_setframe	%rbp, 128
	.seh_endprologue
	movq	%rcx, 1168(%rbp)
	leaq	.LC10(%rip), %rcx
	call	_Z7d_printPKcz
	movq	$-1, 1144(%rbp)
	movq	my_trace(%rip), %rax
	movl	68981632(%rax), %eax
	movl	%eax, 1140(%rbp)
	movq	my_trace(%rip), %rax
	movl	1140(%rbp), %edx
	addq	$56, %rdx
	movl	(%rax,%rdx,4), %eax
	movl	%eax, 1136(%rbp)
	movq	my_trace(%rip), %rax
	movl	1136(%rbp), %edx
	addq	$2097159, %rdx
	salq	$5, %rdx
	addq	%rdx, %rax
	movq	8(%rax), %rax
	movq	%rax, 1144(%rbp)
	movl	$1048577, -48(%rbp)
	leaq	-96(%rbp), %rdx
	movq	1144(%rbp), %rax
	movq	%rax, %rcx
	movq	__imp_GetThreadContext(%rip), %rax
	call	*%rax
	testl	%eax, %eax
	sete	%al
	testb	%al, %al
	je	.L11
	movq	__imp_GetLastError(%rip), %rax
	call	*%rax
	movl	%eax, %edx
	leaq	.LC7(%rip), %rcx
	call	_Z7d_printPKcz
.L11:
	movl	-28(%rbp), %eax
	movl	%eax, %edx
	leaq	.LC11(%rip), %rcx
	call	_Z7d_printPKcz
	leaq	-96(%rbp), %rax
	movq	%rax, %rcx
	call	_Z13print_contextP8_CONTEXT
	movl	-28(%rbp), %eax
	orl	$1, %eax
	movl	%eax, -28(%rbp)
	movl	-28(%rbp), %eax
	movl	%eax, %edx
	leaq	.LC12(%rip), %rcx
	call	_Z7d_printPKcz
	leaq	-96(%rbp), %rax
	movq	%rax, %rcx
	call	_Z13print_contextP8_CONTEXT
	leaq	-96(%rbp), %rdx
	movq	1144(%rbp), %rax
	movq	%rax, %rcx
	movq	__imp_SetThreadContext(%rip), %rax
	call	*%rax
	nop
	addq	$1280, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.globl	_Z12react_set_ZFPv
	.def	_Z12react_set_ZFPv;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z12react_set_ZFPv
_Z12react_set_ZFPv:
.LFB2385:
	pushq	%rbp
	.seh_pushreg	%rbp
	subq	$1280, %rsp
	.seh_stackalloc	1280
	leaq	128(%rsp), %rbp
	.seh_setframe	%rbp, 128
	.seh_endprologue
	movq	%rcx, 1168(%rbp)
	leaq	.LC6(%rip), %rcx
	call	_Z7d_printPKcz
	movq	$-1, 1144(%rbp)
	movq	my_trace(%rip), %rax
	movl	68981632(%rax), %eax
	movl	%eax, 1140(%rbp)
	movq	my_trace(%rip), %rax
	movl	1140(%rbp), %edx
	addq	$56, %rdx
	movl	(%rax,%rdx,4), %eax
	movl	%eax, 1136(%rbp)
	movq	my_trace(%rip), %rax
	movl	1136(%rbp), %edx
	addq	$2097159, %rdx
	salq	$5, %rdx
	addq	%rdx, %rax
	movq	8(%rax), %rax
	movq	%rax, 1144(%rbp)
	movl	$1048577, -48(%rbp)
	leaq	-96(%rbp), %rdx
	movq	1144(%rbp), %rax
	movq	%rax, %rcx
	movq	__imp_GetThreadContext(%rip), %rax
	call	*%rax
	testl	%eax, %eax
	sete	%al
	testb	%al, %al
	je	.L14
	movq	__imp_GetLastError(%rip), %rax
	call	*%rax
	movl	%eax, %edx
	leaq	.LC7(%rip), %rcx
	call	_Z7d_printPKcz
.L14:
	movl	-28(%rbp), %eax
	movl	%eax, %edx
	leaq	.LC11(%rip), %rcx
	call	_Z7d_printPKcz
	leaq	-96(%rbp), %rax
	movq	%rax, %rcx
	call	_Z13print_contextP8_CONTEXT
	movl	-28(%rbp), %eax
	orl	$64, %eax
	movl	%eax, -28(%rbp)
	movl	-28(%rbp), %eax
	movl	%eax, %edx
	leaq	.LC12(%rip), %rcx
	call	_Z7d_printPKcz
	leaq	-96(%rbp), %rax
	movq	%rax, %rcx
	call	_Z13print_contextP8_CONTEXT
	leaq	-96(%rbp), %rdx
	movq	1144(%rbp), %rax
	movq	%rax, %rcx
	movq	__imp_SetThreadContext(%rip), %rax
	call	*%rax
	nop
	addq	$1280, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC13:
	.ascii "Zeroing ZF\12\0"
.LC14:
	.ascii "before zeroing: 0x%08x\12\0"
.LC15:
	.ascii "after zeroing: 0x%08x\12\0"
	.text
	.globl	_Z13react_zero_ZFPv
	.def	_Z13react_zero_ZFPv;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z13react_zero_ZFPv
_Z13react_zero_ZFPv:
.LFB2386:
	pushq	%rbp
	.seh_pushreg	%rbp
	subq	$1280, %rsp
	.seh_stackalloc	1280
	leaq	128(%rsp), %rbp
	.seh_setframe	%rbp, 128
	.seh_endprologue
	movq	%rcx, 1168(%rbp)
	leaq	.LC13(%rip), %rcx
	call	_Z7d_printPKcz
	movq	my_trace(%rip), %rax
	movl	68981632(%rax), %eax
	movl	%eax, 1148(%rbp)
	movl	1148(%rbp), %eax
	leaq	-96(%rbp), %rdx
	movq	%rax, %rcx
	call	_Z12read_contextyP8_CONTEXT
	movl	-28(%rbp), %eax
	movl	%eax, %edx
	leaq	.LC14(%rip), %rcx
	call	_Z7d_printPKcz
	leaq	-96(%rbp), %rax
	movq	%rax, %rcx
	call	_Z13print_contextP8_CONTEXT
	movl	-28(%rbp), %eax
	andl	$-65, %eax
	movl	%eax, -28(%rbp)
	movl	-28(%rbp), %eax
	movl	%eax, %edx
	leaq	.LC15(%rip), %rcx
	call	_Z7d_printPKcz
	leaq	-96(%rbp), %rax
	movq	%rax, %rcx
	call	_Z13print_contextP8_CONTEXT
	movl	1148(%rbp), %eax
	leaq	-96(%rbp), %rdx
	movq	%rax, %rcx
	call	_Z13write_contextyP8_CONTEXT
	nop
	addq	$1280, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC16:
	.ascii "Flipping CF\12\0"
	.text
	.globl	_Z13react_flip_CFPv
	.def	_Z13react_flip_CFPv;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z13react_flip_CFPv
_Z13react_flip_CFPv:
.LFB2387:
	pushq	%rbp
	.seh_pushreg	%rbp
	subq	$1296, %rsp
	.seh_stackalloc	1296
	leaq	128(%rsp), %rbp
	.seh_setframe	%rbp, 128
	.seh_endprologue
	movq	%rcx, 1184(%rbp)
	leaq	.LC16(%rip), %rcx
	call	_Z7d_printPKcz
	movq	$-1, 1160(%rbp)
	movq	my_trace(%rip), %rax
	movl	68981632(%rax), %eax
	movl	%eax, 1156(%rbp)
	movq	my_trace(%rip), %rax
	movl	1156(%rbp), %edx
	addq	$56, %rdx
	movl	(%rax,%rdx,4), %eax
	movl	%eax, 1152(%rbp)
	movq	my_trace(%rip), %rax
	movl	1152(%rbp), %edx
	addq	$2097159, %rdx
	salq	$5, %rdx
	addq	%rdx, %rax
	movq	8(%rax), %rax
	movq	%rax, 1160(%rbp)
	movl	$1048577, -48(%rbp)
	leaq	-96(%rbp), %rdx
	movq	1160(%rbp), %rax
	movq	%rax, %rcx
	movq	__imp_GetThreadContext(%rip), %rax
	call	*%rax
	testl	%eax, %eax
	sete	%al
	testb	%al, %al
	je	.L19
	movq	__imp_GetLastError(%rip), %rax
	call	*%rax
	movl	%eax, %edx
	leaq	.LC7(%rip), %rcx
	call	_Z7d_printPKcz
.L19:
	movl	-28(%rbp), %eax
	movl	%eax, %edx
	leaq	.LC8(%rip), %rcx
	call	_Z7d_printPKcz
	leaq	-96(%rbp), %rax
	movq	%rax, %rcx
	call	_Z13print_contextP8_CONTEXT
	movl	-28(%rbp), %eax
	andl	$1, %eax
	movb	%al, 1151(%rbp)
	cmpb	$0, 1151(%rbp)
	je	.L20
	movl	-28(%rbp), %eax
	andl	$-2, %eax
	movl	%eax, -28(%rbp)
	jmp	.L21
.L20:
	movl	-28(%rbp), %eax
	orl	$1, %eax
	movl	%eax, -28(%rbp)
.L21:
	movl	-28(%rbp), %eax
	movl	%eax, %edx
	leaq	.LC9(%rip), %rcx
	call	_Z7d_printPKcz
	leaq	-96(%rbp), %rax
	movq	%rax, %rcx
	call	_Z13print_contextP8_CONTEXT
	leaq	-96(%rbp), %rdx
	movq	1160(%rbp), %rax
	movq	%rax, %rcx
	movq	__imp_SetThreadContext(%rip), %rax
	call	*%rax
	nop
	addq	$1296, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC17:
	.ascii "Flipping SF\12\0"
	.text
	.globl	_Z13react_flip_SFPv
	.def	_Z13react_flip_SFPv;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z13react_flip_SFPv
_Z13react_flip_SFPv:
.LFB2388:
	pushq	%rbp
	.seh_pushreg	%rbp
	subq	$1296, %rsp
	.seh_stackalloc	1296
	leaq	128(%rsp), %rbp
	.seh_setframe	%rbp, 128
	.seh_endprologue
	movq	%rcx, 1184(%rbp)
	leaq	.LC17(%rip), %rcx
	call	_Z7d_printPKcz
	movq	$-1, 1160(%rbp)
	movq	my_trace(%rip), %rax
	movl	68981632(%rax), %eax
	movl	%eax, 1156(%rbp)
	movq	my_trace(%rip), %rax
	movl	1156(%rbp), %edx
	addq	$56, %rdx
	movl	(%rax,%rdx,4), %eax
	movl	%eax, 1152(%rbp)
	movq	my_trace(%rip), %rax
	movl	1152(%rbp), %edx
	addq	$2097159, %rdx
	salq	$5, %rdx
	addq	%rdx, %rax
	movq	8(%rax), %rax
	movq	%rax, 1160(%rbp)
	movl	$1048577, -48(%rbp)
	leaq	-96(%rbp), %rdx
	movq	1160(%rbp), %rax
	movq	%rax, %rcx
	movq	__imp_GetThreadContext(%rip), %rax
	call	*%rax
	testl	%eax, %eax
	sete	%al
	testb	%al, %al
	je	.L24
	movq	__imp_GetLastError(%rip), %rax
	call	*%rax
	movl	%eax, %edx
	leaq	.LC7(%rip), %rcx
	call	_Z7d_printPKcz
.L24:
	movl	-28(%rbp), %eax
	movl	%eax, %edx
	leaq	.LC8(%rip), %rcx
	call	_Z7d_printPKcz
	leaq	-96(%rbp), %rax
	movq	%rax, %rcx
	call	_Z13print_contextP8_CONTEXT
	movl	-28(%rbp), %eax
	andl	$-128, %eax
	movb	%al, 1151(%rbp)
	cmpb	$0, 1151(%rbp)
	je	.L25
	movl	-28(%rbp), %eax
	andb	$127, %al
	movl	%eax, -28(%rbp)
	jmp	.L26
.L25:
	movl	-28(%rbp), %eax
	orb	$-128, %al
	movl	%eax, -28(%rbp)
.L26:
	movl	-28(%rbp), %eax
	movl	%eax, %edx
	leaq	.LC9(%rip), %rcx
	call	_Z7d_printPKcz
	leaq	-96(%rbp), %rax
	movq	%rax, %rcx
	call	_Z13print_contextP8_CONTEXT
	leaq	-96(%rbp), %rdx
	movq	1160(%rbp), %rax
	movq	%rax, %rcx
	movq	__imp_SetThreadContext(%rip), %rax
	call	*%rax
	nop
	addq	$1296, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC18:
	.ascii "Zeroing CF\12\0"
	.text
	.globl	_Z13react_zero_CFPv
	.def	_Z13react_zero_CFPv;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z13react_zero_CFPv
_Z13react_zero_CFPv:
.LFB2389:
	pushq	%rbp
	.seh_pushreg	%rbp
	subq	$1280, %rsp
	.seh_stackalloc	1280
	leaq	128(%rsp), %rbp
	.seh_setframe	%rbp, 128
	.seh_endprologue
	movq	%rcx, 1168(%rbp)
	leaq	.LC18(%rip), %rcx
	call	_Z7d_printPKcz
	movq	my_trace(%rip), %rax
	movl	68981632(%rax), %eax
	movl	%eax, 1148(%rbp)
	movl	1148(%rbp), %eax
	leaq	-96(%rbp), %rdx
	movq	%rax, %rcx
	call	_Z12read_contextyP8_CONTEXT
	movl	-28(%rbp), %eax
	movl	%eax, %edx
	leaq	.LC14(%rip), %rcx
	call	_Z7d_printPKcz
	leaq	-96(%rbp), %rax
	movq	%rax, %rcx
	call	_Z13print_contextP8_CONTEXT
	movl	-28(%rbp), %eax
	andl	$-2, %eax
	movl	%eax, -28(%rbp)
	movl	-28(%rbp), %eax
	movl	%eax, %edx
	leaq	.LC15(%rip), %rcx
	call	_Z7d_printPKcz
	leaq	-96(%rbp), %rax
	movq	%rax, %rcx
	call	_Z13print_contextP8_CONTEXT
	movl	1148(%rbp), %eax
	leaq	-96(%rbp), %rdx
	movq	%rax, %rcx
	call	_Z13write_contextyP8_CONTEXT
	nop
	addq	$1280, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC19:
	.ascii "Zeroing EAX\12\0"
	.text
	.globl	_Z14react_zero_EAXPv
	.def	_Z14react_zero_EAXPv;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z14react_zero_EAXPv
_Z14react_zero_EAXPv:
.LFB2390:
	pushq	%rbp
	.seh_pushreg	%rbp
	subq	$1280, %rsp
	.seh_stackalloc	1280
	leaq	128(%rsp), %rbp
	.seh_setframe	%rbp, 128
	.seh_endprologue
	movq	%rcx, 1168(%rbp)
	leaq	.LC19(%rip), %rcx
	call	_Z7d_printPKcz
	movq	my_trace(%rip), %rax
	movl	68981632(%rax), %eax
	movl	%eax, 1148(%rbp)
	movl	1148(%rbp), %eax
	leaq	-96(%rbp), %rdx
	movq	%rax, %rcx
	call	_Z12read_contextyP8_CONTEXT
	movl	-28(%rbp), %eax
	movl	%eax, %edx
	leaq	.LC14(%rip), %rcx
	call	_Z7d_printPKcz
	leaq	-96(%rbp), %rax
	movq	%rax, %rcx
	call	_Z13print_contextP8_CONTEXT
	movq	$0, 24(%rbp)
	movl	-28(%rbp), %eax
	movl	%eax, %edx
	leaq	.LC15(%rip), %rcx
	call	_Z7d_printPKcz
	leaq	-96(%rbp), %rax
	movq	%rax, %rcx
	call	_Z13print_contextP8_CONTEXT
	movl	1148(%rbp), %eax
	leaq	-96(%rbp), %rdx
	movq	%rax, %rcx
	call	_Z13write_contextyP8_CONTEXT
	nop
	addq	$1280, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC20:
	.ascii "Zeroing SF\12\0"
	.text
	.globl	_Z13react_zero_SFPv
	.def	_Z13react_zero_SFPv;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z13react_zero_SFPv
_Z13react_zero_SFPv:
.LFB2391:
	pushq	%rbp
	.seh_pushreg	%rbp
	subq	$1280, %rsp
	.seh_stackalloc	1280
	leaq	128(%rsp), %rbp
	.seh_setframe	%rbp, 128
	.seh_endprologue
	movq	%rcx, 1168(%rbp)
	leaq	.LC20(%rip), %rcx
	call	_Z7d_printPKcz
	movq	my_trace(%rip), %rax
	movl	68981632(%rax), %eax
	movl	%eax, 1148(%rbp)
	movl	1148(%rbp), %eax
	leaq	-96(%rbp), %rdx
	movq	%rax, %rcx
	call	_Z12read_contextyP8_CONTEXT
	movl	-28(%rbp), %eax
	movl	%eax, %edx
	leaq	.LC14(%rip), %rcx
	call	_Z7d_printPKcz
	leaq	-96(%rbp), %rax
	movq	%rax, %rcx
	call	_Z13print_contextP8_CONTEXT
	movl	-28(%rbp), %eax
	andb	$127, %al
	movl	%eax, -28(%rbp)
	movl	-28(%rbp), %eax
	movl	%eax, %edx
	leaq	.LC15(%rip), %rcx
	call	_Z7d_printPKcz
	leaq	-96(%rbp), %rax
	movq	%rax, %rcx
	call	_Z13print_contextP8_CONTEXT
	movl	1148(%rbp), %eax
	leaq	-96(%rbp), %rdx
	movq	%rax, %rcx
	call	_Z13write_contextyP8_CONTEXT
	nop
	addq	$1280, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
	.align 8
.LC21:
	.ascii "# Current mod position: 0x%08x\12\0"
	.align 8
.LC22:
	.ascii "[Updated location: 0x%08x, size: 0x%08x]\12\0"
.LC23:
	.ascii "UP,0x%08x,0x%08x\12\0"
	.text
	.globl	_Z17update_region_oldP9LOCATION_
	.def	_Z17update_region_oldP9LOCATION_;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z17update_region_oldP9LOCATION_
_Z17update_region_oldP9LOCATION_:
.LFB2392:
	pushq	%rbp
	.seh_pushreg	%rbp
	subq	$816, %rsp
	.seh_stackalloc	816
	leaq	128(%rsp), %rbp
	.seh_setframe	%rbp, 128
	.seh_endprologue
	movq	%rcx, 704(%rbp)
	movq	my_trace(%rip), %rax
	movq	70030592(%rax), %rax
	movq	%rax, %rcx
	call	ftell
	movl	%eax, %edx
	leaq	-96(%rbp), %rax
	movl	%edx, %r8d
	leaq	.LC21(%rip), %rdx
	movq	%rax, %rcx
	call	sprintf
	leaq	-96(%rbp), %rax
	movq	%rax, %rcx
	call	_Z13add_to_bufferPc
	movq	704(%rbp), %rax
	movq	8(%rax), %rdx
	movq	704(%rbp), %rax
	movq	(%rax), %rax
	movq	%rax, %rcx
	movq	my_trace(%rip), %rax
	movq	70030592(%rax), %rax
	movq	%rdx, %r8
	movq	%rcx, %rdx
	movq	%rax, %rcx
	call	_Z8dump_memP6_iobufPvy
	movl	%eax, 684(%rbp)
	movl	684(%rbp), %edx
	movq	704(%rbp), %rax
	movq	8(%rax), %rax
	cmpq	%rax, %rdx
	jne	.L35
	movq	704(%rbp), %rax
	movq	8(%rax), %rdx
	movq	704(%rbp), %rax
	movq	(%rax), %rax
	movq	%rdx, %r8
	movq	%rax, %rdx
	leaq	.LC22(%rip), %rcx
	call	_Z7d_printPKcz
	movq	704(%rbp), %rax
	movq	8(%rax), %rcx
	movq	704(%rbp), %rax
	movq	(%rax), %rdx
	leaq	-96(%rbp), %rax
	movq	%rcx, %r9
	movq	%rdx, %r8
	leaq	.LC23(%rip), %rdx
	movq	%rax, %rcx
	call	sprintf
	leaq	-96(%rbp), %rax
	movq	%rax, %rcx
	call	_Z13add_to_bufferPc
	movq	my_trace(%rip), %rax
	movq	70030592(%rax), %rax
	movq	%rax, %rcx
	call	ftell
	movl	%eax, %edx
	leaq	-96(%rbp), %rax
	movl	%edx, %r8d
	leaq	.LC21(%rip), %rdx
	movq	%rax, %rcx
	call	sprintf
	leaq	-96(%rbp), %rax
	movq	%rax, %rcx
	call	_Z13add_to_bufferPc
.L35:
	nop
	addq	$816, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.globl	_Z13update_regionj
	.def	_Z13update_regionj;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z13update_regionj
_Z13update_regionj:
.LFB2393:
	pushq	%rbp
	.seh_pushreg	%rbp
	subq	$832, %rsp
	.seh_stackalloc	832
	leaq	128(%rsp), %rbp
	.seh_setframe	%rbp, 128
	.seh_endprologue
	movl	%ecx, 720(%rbp)
	movq	my_trace(%rip), %rax
	movl	720(%rbp), %edx
	addq	$4377912, %rdx
	salq	$4, %rdx
	addq	%rdx, %rax
	movq	%rax, 696(%rbp)
	movq	696(%rbp), %rax
	movq	(%rax), %rax
	movq	%rax, %rcx
	call	_Z16resolve_loc_descP24LOCATION_DESCRIPTOR_NEW_
	movq	%rax, 688(%rbp)
	movq	696(%rbp), %rax
	movq	8(%rax), %rax
	movq	%rax, %rcx
	call	_Z16resolve_loc_descP24LOCATION_DESCRIPTOR_NEW_
	movq	%rax, 680(%rbp)
	movq	my_trace(%rip), %rax
	movq	70030592(%rax), %rax
	movq	%rax, %rcx
	call	ftell
	movl	%eax, %edx
	leaq	-96(%rbp), %rax
	movl	%edx, %r8d
	leaq	.LC21(%rip), %rdx
	movq	%rax, %rcx
	call	sprintf
	leaq	-96(%rbp), %rax
	movq	%rax, %rcx
	call	_Z13add_to_bufferPc
	movq	688(%rbp), %rdx
	movq	my_trace(%rip), %rax
	movq	70030592(%rax), %rax
	movq	680(%rbp), %rcx
	movq	%rcx, %r8
	movq	%rax, %rcx
	call	_Z8dump_memP6_iobufPvy
	movl	%eax, 676(%rbp)
	movl	676(%rbp), %eax
	cmpq	680(%rbp), %rax
	jne	.L38
	movq	680(%rbp), %rdx
	movq	688(%rbp), %rax
	movq	%rdx, %r8
	movq	%rax, %rdx
	leaq	.LC22(%rip), %rcx
	call	_Z7d_printPKcz
	movq	680(%rbp), %rcx
	movq	688(%rbp), %rdx
	leaq	-96(%rbp), %rax
	movq	%rcx, %r9
	movq	%rdx, %r8
	leaq	.LC23(%rip), %rdx
	movq	%rax, %rcx
	call	sprintf
	leaq	-96(%rbp), %rax
	movq	%rax, %rcx
	call	_Z13add_to_bufferPc
	movq	my_trace(%rip), %rax
	movq	70030592(%rax), %rax
	movq	%rax, %rcx
	call	ftell
	movl	%eax, %edx
	leaq	-96(%rbp), %rax
	movl	%edx, %r8d
	leaq	.LC21(%rip), %rdx
	movq	%rax, %rcx
	call	sprintf
	leaq	-96(%rbp), %rax
	movq	%rax, %rcx
	call	_Z13add_to_bufferPc
.L38:
	nop
	addq	$832, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC24:
	.ascii "Updating region 1\12\0"
	.text
	.globl	_Z21react_update_region_1Pv
	.def	_Z21react_update_region_1Pv;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z21react_update_region_1Pv
_Z21react_update_region_1Pv:
.LFB2394:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$32, %rsp
	.seh_stackalloc	32
	.seh_endprologue
	movq	%rcx, 16(%rbp)
	leaq	.LC24(%rip), %rcx
	call	_Z7d_printPKcz
	movl	$0, %ecx
	call	_Z13update_regionj
	nop
	addq	$32, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC25:
	.ascii "Updating region 2\12\0"
	.text
	.globl	_Z21react_update_region_2Pv
	.def	_Z21react_update_region_2Pv;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z21react_update_region_2Pv
_Z21react_update_region_2Pv:
.LFB2395:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$32, %rsp
	.seh_stackalloc	32
	.seh_endprologue
	movq	%rcx, 16(%rbp)
	leaq	.LC25(%rip), %rcx
	call	_Z7d_printPKcz
	movl	$1, %ecx
	call	_Z13update_regionj
	nop
	addq	$32, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC26:
	.ascii "Outputting register %s\12\0"
.LC27:
	.ascii "OU,0x%x,%s: 0x%08x\12\0"
	.text
	.globl	_Z15output_registerPc
	.def	_Z15output_registerPc;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z15output_registerPc
_Z15output_registerPc:
.LFB2396:
	pushq	%rbp
	.seh_pushreg	%rbp
	subq	$832, %rsp
	.seh_stackalloc	832
	leaq	128(%rsp), %rbp
	.seh_setframe	%rbp, 128
	.seh_endprologue
	movq	%rcx, 720(%rbp)
	movq	720(%rbp), %rdx
	leaq	.LC26(%rip), %rcx
	call	_Z7d_printPKcz
	movq	my_trace(%rip), %rax
	movl	68981968(%rax), %eax
	movq	720(%rbp), %rdx
	movl	%eax, %ecx
	call	_Z13read_registermPc
	movq	%rax, 696(%rbp)
	movq	my_trace(%rip), %rax
	movl	68981968(%rax), %ecx
	leaq	-80(%rbp), %rax
	movq	696(%rbp), %rdx
	movq	%rdx, 32(%rsp)
	movq	720(%rbp), %r9
	movl	%ecx, %r8d
	leaq	.LC27(%rip), %rdx
	movq	%rax, %rcx
	call	sprintf
	leaq	-80(%rbp), %rax
	movq	%rax, %rcx
	call	_Z13add_to_bufferPc
	nop
	addq	$832, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC28:
	.ascii "EAX\0"
	.text
	.globl	_Z16react_output_eaxPv
	.def	_Z16react_output_eaxPv;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z16react_output_eaxPv
_Z16react_output_eaxPv:
.LFB2397:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$32, %rsp
	.seh_stackalloc	32
	.seh_endprologue
	movq	%rcx, 16(%rbp)
	leaq	.LC28(%rip), %rcx
	call	_Z15output_registerPc
	nop
	addq	$32, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC29:
	.ascii "EBX\0"
	.text
	.globl	_Z16react_output_ebxPv
	.def	_Z16react_output_ebxPv;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z16react_output_ebxPv
_Z16react_output_ebxPv:
.LFB2398:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$32, %rsp
	.seh_stackalloc	32
	.seh_endprologue
	movq	%rcx, 16(%rbp)
	leaq	.LC29(%rip), %rcx
	call	_Z15output_registerPc
	nop
	addq	$32, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC30:
	.ascii "ECX\0"
	.text
	.globl	_Z16react_output_ecxPv
	.def	_Z16react_output_ecxPv;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z16react_output_ecxPv
_Z16react_output_ecxPv:
.LFB2399:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$32, %rsp
	.seh_stackalloc	32
	.seh_endprologue
	movq	%rcx, 16(%rbp)
	leaq	.LC30(%rip), %rcx
	call	_Z15output_registerPc
	nop
	addq	$32, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC31:
	.ascii "EDX\0"
	.text
	.globl	_Z16react_output_edxPv
	.def	_Z16react_output_edxPv;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z16react_output_edxPv
_Z16react_output_edxPv:
.LFB2400:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$32, %rsp
	.seh_stackalloc	32
	.seh_endprologue
	movq	%rcx, 16(%rbp)
	leaq	.LC31(%rip), %rcx
	call	_Z15output_registerPc
	nop
	addq	$32, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC32:
	.ascii "EDI\0"
	.text
	.globl	_Z16react_output_ediPv
	.def	_Z16react_output_ediPv;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z16react_output_ediPv
_Z16react_output_ediPv:
.LFB2401:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$32, %rsp
	.seh_stackalloc	32
	.seh_endprologue
	movq	%rcx, 16(%rbp)
	leaq	.LC32(%rip), %rcx
	call	_Z15output_registerPc
	nop
	addq	$32, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC33:
	.ascii "ESI\0"
	.text
	.globl	_Z16react_output_esiPv
	.def	_Z16react_output_esiPv;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z16react_output_esiPv
_Z16react_output_esiPv:
.LFB2402:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$32, %rsp
	.seh_stackalloc	32
	.seh_endprologue
	movq	%rcx, 16(%rbp)
	leaq	.LC33(%rip), %rcx
	call	_Z15output_registerPc
	nop
	addq	$32, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC34:
	.ascii "ESP\0"
	.text
	.globl	_Z16react_output_espPv
	.def	_Z16react_output_espPv;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z16react_output_espPv
_Z16react_output_espPv:
.LFB2403:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$32, %rsp
	.seh_stackalloc	32
	.seh_endprologue
	movq	%rcx, 16(%rbp)
	leaq	.LC34(%rip), %rcx
	call	_Z15output_registerPc
	nop
	addq	$32, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC35:
	.ascii "EIP\0"
	.text
	.globl	_Z16react_output_eipPv
	.def	_Z16react_output_eipPv;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z16react_output_eipPv
_Z16react_output_eipPv:
.LFB2404:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$32, %rsp
	.seh_stackalloc	32
	.seh_endprologue
	movq	%rcx, 16(%rbp)
	leaq	.LC35(%rip), %rcx
	call	_Z15output_registerPc
	nop
	addq	$32, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC36:
	.ascii "Outputting arg %d\12\0"
.LC37:
	.ascii "%ls\0"
	.align 8
.LC38:
	.ascii "# Failed to read UNICODE string @ 0x%08x\12\0"
	.text
	.globl	_Z27report_arg_unicode_string_xj
	.def	_Z27report_arg_unicode_string_xj;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z27report_arg_unicode_string_xj
_Z27report_arg_unicode_string_xj:
.LFB2405:
	pushq	%rbp
	.seh_pushreg	%rbp
	subq	$2592, %rsp
	.seh_stackalloc	2592
	leaq	128(%rsp), %rbp
	.seh_setframe	%rbp, 128
	.seh_endprologue
	movl	%ecx, 2480(%rbp)
	movl	2480(%rbp), %edx
	leaq	.LC36(%rip), %rcx
	call	_Z7d_printPKcz
	leaq	1200(%rbp), %rax
	movq	%rax, %rdx
	movl	$0, %ecx
	call	_Z12read_contextyP8_CONTEXT
	movq	1352(%rbp), %rax
	movq	%rax, 2456(%rbp)
	movl	2480(%rbp), %eax
	sall	$2, %eax
	movl	%eax, %eax
	addq	%rax, 2456(%rbp)
	movq	2456(%rbp), %rax
	movq	%rax, %rcx
	call	_Z10read_dwordy
	cltq
	movq	%rax, 2448(%rbp)
	movq	2448(%rbp), %rdx
	movq	my_trace(%rip), %rax
	movq	136(%rax), %rax
	leaq	-80(%rbp), %r8
	leaq	2440(%rbp), %rcx
	movq	%rcx, 32(%rsp)
	movl	$512, %r9d
	movq	%rax, %rcx
	call	_Z11read_memoryPvS_S_yPy
	movq	2440(%rbp), %rax
	testq	%rax, %rax
	je	.L55
	movq	my_trace(%rip), %rax
	movl	$7, 67932936(%rax)
	leaq	-80(%rbp), %rdx
	leaq	432(%rbp), %rax
	movq	%rdx, %r8
	leaq	.LC37(%rip), %rdx
	movq	%rax, %rcx
	call	sprintf
	movq	my_trace(%rip), %rax
	leaq	67932940(%rax), %rcx
	leaq	432(%rbp), %rax
	movq	%rax, %rdx
	call	strcpy
	jmp	.L56
.L55:
	movq	my_trace(%rip), %rax
	movl	$7, 67932936(%rax)
	movq	2456(%rbp), %rdx
	leaq	432(%rbp), %rax
	movq	%rdx, %r8
	leaq	.LC38(%rip), %rdx
	movq	%rax, %rcx
	call	sprintf
	movq	my_trace(%rip), %rax
	leaq	67932940(%rax), %rcx
	leaq	432(%rbp), %rax
	movq	%rax, %rdx
	call	strcpy
.L56:
	nop
	addq	$2592, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC39:
	.ascii "OU,0x%x,Arg%d: %ls\12\0"
	.text
	.globl	_Z27output_arg_unicode_string_xj
	.def	_Z27output_arg_unicode_string_xj;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z27output_arg_unicode_string_xj
_Z27output_arg_unicode_string_xj:
.LFB2406:
	pushq	%rbp
	.seh_pushreg	%rbp
	subq	$2592, %rsp
	.seh_stackalloc	2592
	leaq	128(%rsp), %rbp
	.seh_setframe	%rbp, 128
	.seh_endprologue
	movl	%ecx, 2480(%rbp)
	movl	2480(%rbp), %edx
	leaq	.LC36(%rip), %rcx
	call	_Z7d_printPKcz
	leaq	1200(%rbp), %rax
	movq	%rax, %rdx
	movl	$0, %ecx
	call	_Z12read_contextyP8_CONTEXT
	movq	1352(%rbp), %rax
	movq	%rax, 2456(%rbp)
	movl	2480(%rbp), %eax
	sall	$2, %eax
	movl	%eax, %eax
	addq	%rax, 2456(%rbp)
	movq	2456(%rbp), %rax
	movq	%rax, %rcx
	call	_Z10read_dwordy
	cltq
	movq	%rax, 2448(%rbp)
	movq	2448(%rbp), %rdx
	movq	my_trace(%rip), %rax
	movq	136(%rax), %rax
	leaq	-80(%rbp), %r8
	leaq	2440(%rbp), %rcx
	movq	%rcx, 32(%rsp)
	movl	$512, %r9d
	movq	%rax, %rcx
	call	_Z11read_memoryPvS_S_yPy
	movq	2440(%rbp), %rax
	testq	%rax, %rax
	je	.L59
	movq	my_trace(%rip), %rax
	movl	68981968(%rax), %ecx
	leaq	432(%rbp), %rax
	leaq	-80(%rbp), %rdx
	movq	%rdx, 32(%rsp)
	movl	2480(%rbp), %r9d
	movl	%ecx, %r8d
	leaq	.LC39(%rip), %rdx
	movq	%rax, %rcx
	call	sprintf
	leaq	432(%rbp), %rax
	movq	%rax, %rcx
	call	_Z13add_to_bufferPc
	jmp	.L60
.L59:
	movq	2456(%rbp), %rdx
	leaq	432(%rbp), %rax
	movq	%rdx, %r8
	leaq	.LC38(%rip), %rdx
	movq	%rax, %rcx
	call	sprintf
	leaq	432(%rbp), %rax
	movq	%rax, %rcx
	call	_Z13add_to_bufferPc
.L60:
	nop
	addq	$2592, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.globl	_Z30react_output_arg_unicode_str_0Pv
	.def	_Z30react_output_arg_unicode_str_0Pv;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z30react_output_arg_unicode_str_0Pv
_Z30react_output_arg_unicode_str_0Pv:
.LFB2407:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$32, %rsp
	.seh_stackalloc	32
	.seh_endprologue
	movq	%rcx, 16(%rbp)
	movl	$0, %ecx
	call	_Z27output_arg_unicode_string_xj
	nop
	addq	$32, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.globl	_Z30react_output_arg_unicode_str_1Pv
	.def	_Z30react_output_arg_unicode_str_1Pv;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z30react_output_arg_unicode_str_1Pv
_Z30react_output_arg_unicode_str_1Pv:
.LFB2408:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$32, %rsp
	.seh_stackalloc	32
	.seh_endprologue
	movq	%rcx, 16(%rbp)
	movl	$1, %ecx
	call	_Z27output_arg_unicode_string_xj
	nop
	addq	$32, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.globl	_Z30react_output_arg_unicode_str_2Pv
	.def	_Z30react_output_arg_unicode_str_2Pv;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z30react_output_arg_unicode_str_2Pv
_Z30react_output_arg_unicode_str_2Pv:
.LFB2409:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$32, %rsp
	.seh_stackalloc	32
	.seh_endprologue
	movq	%rcx, 16(%rbp)
	movl	$2, %ecx
	call	_Z27output_arg_unicode_string_xj
	nop
	addq	$32, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.globl	_Z30react_output_arg_unicode_str_3Pv
	.def	_Z30react_output_arg_unicode_str_3Pv;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z30react_output_arg_unicode_str_3Pv
_Z30react_output_arg_unicode_str_3Pv:
.LFB2410:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$32, %rsp
	.seh_stackalloc	32
	.seh_endprologue
	movq	%rcx, 16(%rbp)
	movl	$3, %ecx
	call	_Z27output_arg_unicode_string_xj
	nop
	addq	$32, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.globl	_Z30react_output_arg_unicode_str_4Pv
	.def	_Z30react_output_arg_unicode_str_4Pv;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z30react_output_arg_unicode_str_4Pv
_Z30react_output_arg_unicode_str_4Pv:
.LFB2411:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$32, %rsp
	.seh_stackalloc	32
	.seh_endprologue
	movq	%rcx, 16(%rbp)
	movl	$4, %ecx
	call	_Z27output_arg_unicode_string_xj
	nop
	addq	$32, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.globl	_Z30react_output_arg_unicode_str_5Pv
	.def	_Z30react_output_arg_unicode_str_5Pv;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z30react_output_arg_unicode_str_5Pv
_Z30react_output_arg_unicode_str_5Pv:
.LFB2412:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$32, %rsp
	.seh_stackalloc	32
	.seh_endprologue
	movq	%rcx, 16(%rbp)
	movl	$5, %ecx
	call	_Z27output_arg_unicode_string_xj
	nop
	addq	$32, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.globl	_Z30react_output_arg_unicode_str_6Pv
	.def	_Z30react_output_arg_unicode_str_6Pv;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z30react_output_arg_unicode_str_6Pv
_Z30react_output_arg_unicode_str_6Pv:
.LFB2413:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$32, %rsp
	.seh_stackalloc	32
	.seh_endprologue
	movq	%rcx, 16(%rbp)
	movl	$6, %ecx
	call	_Z27output_arg_unicode_string_xj
	nop
	addq	$32, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.globl	_Z30react_output_arg_unicode_str_7Pv
	.def	_Z30react_output_arg_unicode_str_7Pv;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z30react_output_arg_unicode_str_7Pv
_Z30react_output_arg_unicode_str_7Pv:
.LFB2414:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$32, %rsp
	.seh_stackalloc	32
	.seh_endprologue
	movq	%rcx, 16(%rbp)
	movl	$7, %ecx
	call	_Z27output_arg_unicode_string_xj
	nop
	addq	$32, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.globl	_Z30react_output_arg_unicode_str_8Pv
	.def	_Z30react_output_arg_unicode_str_8Pv;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z30react_output_arg_unicode_str_8Pv
_Z30react_output_arg_unicode_str_8Pv:
.LFB2415:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$32, %rsp
	.seh_stackalloc	32
	.seh_endprologue
	movq	%rcx, 16(%rbp)
	movl	$8, %ecx
	call	_Z27output_arg_unicode_string_xj
	nop
	addq	$32, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
	.align 8
.LC40:
	.ascii "Outputting arg %d as ANSI string\12\0"
.LC41:
	.ascii "ESP: 0x%08x\12\0"
.LC42:
	.ascii "arg addr: 0x%08x\12\0"
.LC43:
	.ascii "str addr: 0x%08x\12\0"
	.align 8
.LC44:
	.ascii "# Failed to read ANSI string @ 0x%08x\12\0"
	.text
	.globl	_Z19report_arg_string_xj
	.def	_Z19report_arg_string_xj;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z19report_arg_string_xj
_Z19report_arg_string_xj:
.LFB2416:
	pushq	%rbp
	.seh_pushreg	%rbp
	subq	$2336, %rsp
	.seh_stackalloc	2336
	leaq	128(%rsp), %rbp
	.seh_setframe	%rbp, 128
	.seh_endprologue
	movl	%ecx, 2224(%rbp)
	movl	2224(%rbp), %edx
	leaq	.LC40(%rip), %rcx
	call	_Z7d_printPKcz
	leaq	944(%rbp), %rax
	movq	%rax, %rdx
	movl	$0, %ecx
	call	_Z12read_contextyP8_CONTEXT
	movq	2200(%rbp), %rax
	movq	%rax, %rdx
	leaq	.LC41(%rip), %rcx
	call	_Z7d_printPKcz
	movq	1096(%rbp), %rax
	movq	%rax, 2200(%rbp)
	movl	2224(%rbp), %eax
	sall	$2, %eax
	movl	%eax, %eax
	addq	%rax, 2200(%rbp)
	movq	2200(%rbp), %rax
	movq	%rax, %rdx
	leaq	.LC42(%rip), %rcx
	call	_Z7d_printPKcz
	movq	2200(%rbp), %rax
	movq	%rax, %rcx
	call	_Z10read_dwordy
	cltq
	movq	%rax, 2192(%rbp)
	movq	2192(%rbp), %rax
	movq	%rax, %rdx
	leaq	.LC43(%rip), %rcx
	call	_Z7d_printPKcz
	movq	2192(%rbp), %rdx
	movq	my_trace(%rip), %rax
	movq	136(%rax), %rax
	leaq	-80(%rbp), %r8
	leaq	2184(%rbp), %rcx
	movq	%rcx, 32(%rsp)
	movl	$256, %r9d
	movq	%rax, %rcx
	call	_Z11read_memoryPvS_S_yPy
	movq	2184(%rbp), %rax
	testq	%rax, %rax
	je	.L72
	movq	my_trace(%rip), %rax
	movl	$7, 67932936(%rax)
	leaq	-80(%rbp), %rdx
	leaq	176(%rbp), %rax
	movq	%rax, %rcx
	call	strcpy
	movq	my_trace(%rip), %rax
	leaq	67932940(%rax), %rcx
	leaq	176(%rbp), %rax
	movq	%rax, %rdx
	call	strcpy
	jmp	.L73
.L72:
	movq	my_trace(%rip), %rax
	movl	$7, 67932936(%rax)
	movq	2200(%rbp), %rdx
	leaq	176(%rbp), %rax
	movq	%rdx, %r8
	leaq	.LC44(%rip), %rdx
	movq	%rax, %rcx
	call	sprintf
	movq	my_trace(%rip), %rax
	leaq	67932940(%rax), %rcx
	leaq	176(%rbp), %rax
	movq	%rax, %rdx
	call	strcpy
.L73:
	nop
	addq	$2336, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC45:
	.ascii "OU,0x%x,Arg%d: %s\12\0"
	.text
	.globl	_Z19output_arg_string_xj
	.def	_Z19output_arg_string_xj;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z19output_arg_string_xj
_Z19output_arg_string_xj:
.LFB2417:
	pushq	%rbp
	.seh_pushreg	%rbp
	subq	$2336, %rsp
	.seh_stackalloc	2336
	leaq	128(%rsp), %rbp
	.seh_setframe	%rbp, 128
	.seh_endprologue
	movl	%ecx, 2224(%rbp)
	movl	2224(%rbp), %edx
	leaq	.LC40(%rip), %rcx
	call	_Z7d_printPKcz
	leaq	944(%rbp), %rax
	movq	%rax, %rdx
	movl	$0, %ecx
	call	_Z12read_contextyP8_CONTEXT
	movq	2200(%rbp), %rax
	movq	%rax, %rdx
	leaq	.LC41(%rip), %rcx
	call	_Z7d_printPKcz
	movq	1096(%rbp), %rax
	movq	%rax, 2200(%rbp)
	movl	2224(%rbp), %eax
	sall	$2, %eax
	movl	%eax, %eax
	addq	%rax, 2200(%rbp)
	movq	2200(%rbp), %rax
	movq	%rax, %rdx
	leaq	.LC42(%rip), %rcx
	call	_Z7d_printPKcz
	movq	2200(%rbp), %rax
	movq	%rax, %rcx
	call	_Z10read_dwordy
	cltq
	movq	%rax, 2192(%rbp)
	movq	2192(%rbp), %rax
	movq	%rax, %rdx
	leaq	.LC43(%rip), %rcx
	call	_Z7d_printPKcz
	movq	2192(%rbp), %rdx
	movq	my_trace(%rip), %rax
	movq	136(%rax), %rax
	leaq	-80(%rbp), %r8
	leaq	2184(%rbp), %rcx
	movq	%rcx, 32(%rsp)
	movl	$256, %r9d
	movq	%rax, %rcx
	call	_Z11read_memoryPvS_S_yPy
	movq	2184(%rbp), %rax
	testq	%rax, %rax
	je	.L76
	movq	my_trace(%rip), %rax
	movl	68981968(%rax), %ecx
	leaq	176(%rbp), %rax
	leaq	-80(%rbp), %rdx
	movq	%rdx, 32(%rsp)
	movl	2224(%rbp), %r9d
	movl	%ecx, %r8d
	leaq	.LC45(%rip), %rdx
	movq	%rax, %rcx
	call	sprintf
	leaq	176(%rbp), %rax
	movq	%rax, %rcx
	call	_Z13add_to_bufferPc
	jmp	.L77
.L76:
	movq	2200(%rbp), %rdx
	leaq	176(%rbp), %rax
	movq	%rdx, %r8
	leaq	.LC44(%rip), %rdx
	movq	%rax, %rcx
	call	sprintf
	leaq	176(%rbp), %rax
	movq	%rax, %rcx
	call	_Z13add_to_bufferPc
.L77:
	nop
	addq	$2336, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.globl	_Z22react_output_arg_str_0Pv
	.def	_Z22react_output_arg_str_0Pv;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z22react_output_arg_str_0Pv
_Z22react_output_arg_str_0Pv:
.LFB2418:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$32, %rsp
	.seh_stackalloc	32
	.seh_endprologue
	movq	%rcx, 16(%rbp)
	movl	$0, %ecx
	call	_Z19output_arg_string_xj
	nop
	addq	$32, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.globl	_Z22react_output_arg_str_1Pv
	.def	_Z22react_output_arg_str_1Pv;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z22react_output_arg_str_1Pv
_Z22react_output_arg_str_1Pv:
.LFB2419:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$32, %rsp
	.seh_stackalloc	32
	.seh_endprologue
	movq	%rcx, 16(%rbp)
	movl	$1, %ecx
	call	_Z19output_arg_string_xj
	nop
	addq	$32, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.globl	_Z22react_output_arg_str_2Pv
	.def	_Z22react_output_arg_str_2Pv;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z22react_output_arg_str_2Pv
_Z22react_output_arg_str_2Pv:
.LFB2420:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$32, %rsp
	.seh_stackalloc	32
	.seh_endprologue
	movq	%rcx, 16(%rbp)
	movl	$2, %ecx
	call	_Z19output_arg_string_xj
	nop
	addq	$32, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.globl	_Z22react_output_arg_str_3Pv
	.def	_Z22react_output_arg_str_3Pv;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z22react_output_arg_str_3Pv
_Z22react_output_arg_str_3Pv:
.LFB2421:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$32, %rsp
	.seh_stackalloc	32
	.seh_endprologue
	movq	%rcx, 16(%rbp)
	movl	$3, %ecx
	call	_Z19output_arg_string_xj
	nop
	addq	$32, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.globl	_Z22react_output_arg_str_4Pv
	.def	_Z22react_output_arg_str_4Pv;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z22react_output_arg_str_4Pv
_Z22react_output_arg_str_4Pv:
.LFB2422:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$32, %rsp
	.seh_stackalloc	32
	.seh_endprologue
	movq	%rcx, 16(%rbp)
	movl	$4, %ecx
	call	_Z19output_arg_string_xj
	nop
	addq	$32, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.globl	_Z22react_output_arg_str_5Pv
	.def	_Z22react_output_arg_str_5Pv;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z22react_output_arg_str_5Pv
_Z22react_output_arg_str_5Pv:
.LFB2423:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$32, %rsp
	.seh_stackalloc	32
	.seh_endprologue
	movq	%rcx, 16(%rbp)
	movl	$5, %ecx
	call	_Z19output_arg_string_xj
	nop
	addq	$32, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.globl	_Z22react_output_arg_str_6Pv
	.def	_Z22react_output_arg_str_6Pv;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z22react_output_arg_str_6Pv
_Z22react_output_arg_str_6Pv:
.LFB2424:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$32, %rsp
	.seh_stackalloc	32
	.seh_endprologue
	movq	%rcx, 16(%rbp)
	movl	$6, %ecx
	call	_Z19output_arg_string_xj
	nop
	addq	$32, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.globl	_Z22react_output_arg_str_7Pv
	.def	_Z22react_output_arg_str_7Pv;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z22react_output_arg_str_7Pv
_Z22react_output_arg_str_7Pv:
.LFB2425:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$32, %rsp
	.seh_stackalloc	32
	.seh_endprologue
	movq	%rcx, 16(%rbp)
	movl	$7, %ecx
	call	_Z19output_arg_string_xj
	nop
	addq	$32, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.globl	_Z22react_output_arg_str_8Pv
	.def	_Z22react_output_arg_str_8Pv;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z22react_output_arg_str_8Pv
_Z22react_output_arg_str_8Pv:
.LFB2426:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$32, %rsp
	.seh_stackalloc	32
	.seh_endprologue
	movq	%rcx, 16(%rbp)
	movl	$8, %ecx
	call	_Z19output_arg_string_xj
	nop
	addq	$32, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC46:
	.ascii "0x%08x\0"
	.text
	.globl	_Z12report_arg_xj
	.def	_Z12report_arg_xj;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z12report_arg_xj
_Z12report_arg_xj:
.LFB2427:
	pushq	%rbp
	.seh_pushreg	%rbp
	subq	$2048, %rsp
	.seh_stackalloc	2048
	leaq	128(%rsp), %rbp
	.seh_setframe	%rbp, 128
	.seh_endprologue
	movl	%ecx, 1936(%rbp)
	movl	1936(%rbp), %edx
	leaq	.LC36(%rip), %rcx
	call	_Z7d_printPKcz
	leaq	672(%rbp), %rax
	movq	%rax, %rdx
	movl	$0, %ecx
	call	_Z12read_contextyP8_CONTEXT
	movq	824(%rbp), %rax
	movq	%rax, 1912(%rbp)
	movl	1936(%rbp), %eax
	sall	$2, %eax
	movl	%eax, %eax
	addq	%rax, 1912(%rbp)
	movq	1912(%rbp), %rax
	movq	%rax, %rcx
	call	_Z10read_dwordy
	cltq
	movq	%rax, 1904(%rbp)
	movq	my_trace(%rip), %rax
	movl	$7, 67932936(%rax)
	movq	1904(%rbp), %rdx
	leaq	-96(%rbp), %rax
	movq	%rdx, %r8
	leaq	.LC46(%rip), %rdx
	movq	%rax, %rcx
	call	sprintf
	movq	my_trace(%rip), %rax
	leaq	67932940(%rax), %rcx
	leaq	-96(%rbp), %rax
	movq	%rax, %rdx
	call	strcpy
	nop
	addq	$2048, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC47:
	.ascii "OU,0x%x,Arg%d: %08x\12\0"
	.text
	.globl	_Z12output_arg_xj
	.def	_Z12output_arg_xj;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z12output_arg_xj
_Z12output_arg_xj:
.LFB2428:
	pushq	%rbp
	.seh_pushreg	%rbp
	subq	$2064, %rsp
	.seh_stackalloc	2064
	leaq	128(%rsp), %rbp
	.seh_setframe	%rbp, 128
	.seh_endprologue
	movl	%ecx, 1952(%rbp)
	movl	1952(%rbp), %edx
	leaq	.LC36(%rip), %rcx
	call	_Z7d_printPKcz
	leaq	688(%rbp), %rax
	movq	%rax, %rdx
	movl	$0, %ecx
	call	_Z12read_contextyP8_CONTEXT
	movq	840(%rbp), %rax
	movq	%rax, 1928(%rbp)
	movl	1952(%rbp), %eax
	sall	$2, %eax
	movl	%eax, %eax
	addq	%rax, 1928(%rbp)
	movq	1928(%rbp), %rax
	movq	%rax, %rcx
	call	_Z10read_dwordy
	cltq
	movq	%rax, 1920(%rbp)
	movq	my_trace(%rip), %rax
	movl	68981968(%rax), %ecx
	leaq	-80(%rbp), %rax
	movq	1920(%rbp), %rdx
	movq	%rdx, 32(%rsp)
	movl	1952(%rbp), %r9d
	movl	%ecx, %r8d
	leaq	.LC47(%rip), %rdx
	movq	%rax, %rcx
	call	sprintf
	leaq	-80(%rbp), %rax
	movq	%rax, %rcx
	call	_Z13add_to_bufferPc
	nop
	addq	$2064, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.globl	_Z18react_output_arg_0Pv
	.def	_Z18react_output_arg_0Pv;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z18react_output_arg_0Pv
_Z18react_output_arg_0Pv:
.LFB2429:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$32, %rsp
	.seh_stackalloc	32
	.seh_endprologue
	movq	%rcx, 16(%rbp)
	movl	$0, %ecx
	call	_Z12output_arg_xj
	nop
	addq	$32, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.globl	_Z18react_output_arg_1Pv
	.def	_Z18react_output_arg_1Pv;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z18react_output_arg_1Pv
_Z18react_output_arg_1Pv:
.LFB2430:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$32, %rsp
	.seh_stackalloc	32
	.seh_endprologue
	movq	%rcx, 16(%rbp)
	movl	$1, %ecx
	call	_Z12output_arg_xj
	nop
	addq	$32, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.globl	_Z18react_output_arg_2Pv
	.def	_Z18react_output_arg_2Pv;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z18react_output_arg_2Pv
_Z18react_output_arg_2Pv:
.LFB2431:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$32, %rsp
	.seh_stackalloc	32
	.seh_endprologue
	movq	%rcx, 16(%rbp)
	movl	$2, %ecx
	call	_Z12output_arg_xj
	nop
	addq	$32, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.globl	_Z18react_output_arg_3Pv
	.def	_Z18react_output_arg_3Pv;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z18react_output_arg_3Pv
_Z18react_output_arg_3Pv:
.LFB2432:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$32, %rsp
	.seh_stackalloc	32
	.seh_endprologue
	movq	%rcx, 16(%rbp)
	movl	$3, %ecx
	call	_Z12output_arg_xj
	nop
	addq	$32, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.globl	_Z18react_output_arg_4Pv
	.def	_Z18react_output_arg_4Pv;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z18react_output_arg_4Pv
_Z18react_output_arg_4Pv:
.LFB2433:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$32, %rsp
	.seh_stackalloc	32
	.seh_endprologue
	movq	%rcx, 16(%rbp)
	movl	$4, %ecx
	call	_Z12output_arg_xj
	nop
	addq	$32, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.globl	_Z18react_output_arg_5Pv
	.def	_Z18react_output_arg_5Pv;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z18react_output_arg_5Pv
_Z18react_output_arg_5Pv:
.LFB2434:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$32, %rsp
	.seh_stackalloc	32
	.seh_endprologue
	movq	%rcx, 16(%rbp)
	movl	$5, %ecx
	call	_Z12output_arg_xj
	nop
	addq	$32, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.globl	_Z18react_output_arg_6Pv
	.def	_Z18react_output_arg_6Pv;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z18react_output_arg_6Pv
_Z18react_output_arg_6Pv:
.LFB2435:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$32, %rsp
	.seh_stackalloc	32
	.seh_endprologue
	movq	%rcx, 16(%rbp)
	movl	$6, %ecx
	call	_Z12output_arg_xj
	nop
	addq	$32, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.globl	_Z18react_output_arg_7Pv
	.def	_Z18react_output_arg_7Pv;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z18react_output_arg_7Pv
_Z18react_output_arg_7Pv:
.LFB2436:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$32, %rsp
	.seh_stackalloc	32
	.seh_endprologue
	movq	%rcx, 16(%rbp)
	movl	$7, %ecx
	call	_Z12output_arg_xj
	nop
	addq	$32, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.globl	_Z18react_output_arg_8Pv
	.def	_Z18react_output_arg_8Pv;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z18react_output_arg_8Pv
_Z18react_output_arg_8Pv:
.LFB2437:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$32, %rsp
	.seh_stackalloc	32
	.seh_endprologue
	movq	%rcx, 16(%rbp)
	movl	$8, %ecx
	call	_Z12output_arg_xj
	nop
	addq	$32, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC48:
	.ascii "OU,0x%x,Arg_p%d: %08x\12\0"
	.text
	.globl	_Z14output_p_arg_xj
	.def	_Z14output_p_arg_xj;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z14output_p_arg_xj
_Z14output_p_arg_xj:
.LFB2438:
	pushq	%rbp
	.seh_pushreg	%rbp
	subq	$2064, %rsp
	.seh_stackalloc	2064
	leaq	128(%rsp), %rbp
	.seh_setframe	%rbp, 128
	.seh_endprologue
	movl	%ecx, 1952(%rbp)
	movl	1952(%rbp), %edx
	leaq	.LC36(%rip), %rcx
	call	_Z7d_printPKcz
	leaq	688(%rbp), %rax
	movq	%rax, %rdx
	movl	$0, %ecx
	call	_Z12read_contextyP8_CONTEXT
	movq	840(%rbp), %rax
	movq	%rax, 1928(%rbp)
	movl	1952(%rbp), %eax
	sall	$2, %eax
	movl	%eax, %eax
	addq	%rax, 1928(%rbp)
	movq	1928(%rbp), %rax
	movq	%rax, %rcx
	call	_Z10read_dwordy
	cltq
	movq	%rax, 1920(%rbp)
	movq	1920(%rbp), %rax
	movq	%rax, %rcx
	call	_Z10read_dwordy
	cltq
	movq	%rax, 1920(%rbp)
	movq	my_trace(%rip), %rax
	movl	68981968(%rax), %ecx
	leaq	-80(%rbp), %rax
	movq	1920(%rbp), %rdx
	movq	%rdx, 32(%rsp)
	movl	1952(%rbp), %r9d
	movl	%ecx, %r8d
	leaq	.LC48(%rip), %rdx
	movq	%rax, %rcx
	call	sprintf
	leaq	-80(%rbp), %rax
	movq	%rax, %rcx
	call	_Z13add_to_bufferPc
	nop
	addq	$2064, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.globl	_Z20react_output_p_arg_0Pv
	.def	_Z20react_output_p_arg_0Pv;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z20react_output_p_arg_0Pv
_Z20react_output_p_arg_0Pv:
.LFB2439:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$32, %rsp
	.seh_stackalloc	32
	.seh_endprologue
	movq	%rcx, 16(%rbp)
	movl	$0, %ecx
	call	_Z14output_p_arg_xj
	nop
	addq	$32, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.globl	_Z20react_output_p_arg_1Pv
	.def	_Z20react_output_p_arg_1Pv;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z20react_output_p_arg_1Pv
_Z20react_output_p_arg_1Pv:
.LFB2440:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$32, %rsp
	.seh_stackalloc	32
	.seh_endprologue
	movq	%rcx, 16(%rbp)
	movl	$1, %ecx
	call	_Z14output_p_arg_xj
	nop
	addq	$32, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.globl	_Z20react_output_p_arg_2Pv
	.def	_Z20react_output_p_arg_2Pv;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z20react_output_p_arg_2Pv
_Z20react_output_p_arg_2Pv:
.LFB2441:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$32, %rsp
	.seh_stackalloc	32
	.seh_endprologue
	movq	%rcx, 16(%rbp)
	movl	$2, %ecx
	call	_Z14output_p_arg_xj
	nop
	addq	$32, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.globl	_Z20react_output_p_arg_3Pv
	.def	_Z20react_output_p_arg_3Pv;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z20react_output_p_arg_3Pv
_Z20react_output_p_arg_3Pv:
.LFB2442:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$32, %rsp
	.seh_stackalloc	32
	.seh_endprologue
	movq	%rcx, 16(%rbp)
	movl	$3, %ecx
	call	_Z14output_p_arg_xj
	nop
	addq	$32, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.globl	_Z20react_output_p_arg_4Pv
	.def	_Z20react_output_p_arg_4Pv;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z20react_output_p_arg_4Pv
_Z20react_output_p_arg_4Pv:
.LFB2443:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$32, %rsp
	.seh_stackalloc	32
	.seh_endprologue
	movq	%rcx, 16(%rbp)
	movl	$4, %ecx
	call	_Z14output_p_arg_xj
	nop
	addq	$32, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.globl	_Z20react_output_p_arg_5Pv
	.def	_Z20react_output_p_arg_5Pv;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z20react_output_p_arg_5Pv
_Z20react_output_p_arg_5Pv:
.LFB2444:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$32, %rsp
	.seh_stackalloc	32
	.seh_endprologue
	movq	%rcx, 16(%rbp)
	movl	$5, %ecx
	call	_Z14output_p_arg_xj
	nop
	addq	$32, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.globl	_Z20react_output_p_arg_6Pv
	.def	_Z20react_output_p_arg_6Pv;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z20react_output_p_arg_6Pv
_Z20react_output_p_arg_6Pv:
.LFB2445:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$32, %rsp
	.seh_stackalloc	32
	.seh_endprologue
	movq	%rcx, 16(%rbp)
	movl	$6, %ecx
	call	_Z14output_p_arg_xj
	nop
	addq	$32, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.globl	_Z20react_output_p_arg_7Pv
	.def	_Z20react_output_p_arg_7Pv;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z20react_output_p_arg_7Pv
_Z20react_output_p_arg_7Pv:
.LFB2446:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$32, %rsp
	.seh_stackalloc	32
	.seh_endprologue
	movq	%rcx, 16(%rbp)
	movl	$7, %ecx
	call	_Z14output_p_arg_xj
	nop
	addq	$32, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.globl	_Z20react_output_p_arg_8Pv
	.def	_Z20react_output_p_arg_8Pv;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z20react_output_p_arg_8Pv
_Z20react_output_p_arg_8Pv:
.LFB2447:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$32, %rsp
	.seh_stackalloc	32
	.seh_endprologue
	movq	%rcx, 16(%rbp)
	movl	$8, %ecx
	call	_Z14output_p_arg_xj
	nop
	addq	$32, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC49:
	.ascii "Modifying address: 0x%08x\12\0"
	.text
	.globl	_Z21react_cry_antidebug_1Pv
	.def	_Z21react_cry_antidebug_1Pv;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z21react_cry_antidebug_1Pv
_Z21react_cry_antidebug_1Pv:
.LFB2448:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$80, %rsp
	.seh_stackalloc	80
	.seh_endprologue
	movq	%rcx, 16(%rbp)
	movq	$4260836, -8(%rbp)
	movb	$-7, -17(%rbp)
	movb	$1, -18(%rbp)
	movq	-8(%rbp), %rax
	movq	%rax, %rdx
	leaq	.LC49(%rip), %rcx
	call	_Z7d_printPKcz
	movq	-8(%rbp), %rdx
	movq	my_trace(%rip), %rax
	movq	208(%rax), %rax
	leaq	-17(%rbp), %r8
	leaq	-16(%rbp), %rcx
	movq	%rcx, 32(%rsp)
	movl	$1, %r9d
	movq	%rax, %rcx
	call	_Z12write_memoryPvS_S_yPy
	addq	$1, -8(%rbp)
	movq	-8(%rbp), %rdx
	movq	my_trace(%rip), %rax
	movq	208(%rax), %rax
	leaq	-18(%rbp), %r8
	leaq	-16(%rbp), %rcx
	movq	%rcx, 32(%rsp)
	movl	$1, %r9d
	movq	%rax, %rcx
	call	_Z12write_memoryPvS_S_yPy
	nop
	addq	$80, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC50:
	.ascii "Executing routine 0x%02x\12\0"
	.text
	.globl	_Z11run_routinej
	.def	_Z11run_routinej;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z11run_routinej
_Z11run_routinej:
.LFB2449:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$32, %rsp
	.seh_stackalloc	32
	.seh_endprologue
	movl	%ecx, 16(%rbp)
	movl	16(%rbp), %edx
	leaq	.LC50(%rip), %rcx
	call	_Z7d_printPKcz
	movq	my_trace(%rip), %rax
	movl	16(%rbp), %edx
	addq	$8754958, %rdx
	movq	8(%rax,%rdx,8), %rax
	movl	$0, %ecx
	call	*%rax
	nop
	addq	$32, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
	.align 8
.LC51:
	.ascii "Writing ANSI string to 0x%08x\12\0"
.LC52:
	.ascii "# Wrote ANSI string @ 0x%08x\12\0"
	.align 8
.LC53:
	.ascii "# Failed to write ANSI string @ 0x%08x\12\0"
	.text
	.globl	_Z18write_string_asciiyPc
	.def	_Z18write_string_asciiyPc;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z18write_string_asciiyPc
_Z18write_string_asciiyPc:
.LFB2450:
	pushq	%rbp
	.seh_pushreg	%rbp
	subq	$832, %rsp
	.seh_stackalloc	832
	leaq	128(%rsp), %rbp
	.seh_setframe	%rbp, 128
	.seh_endprologue
	movq	%rcx, 720(%rbp)
	movq	%rdx, 728(%rbp)
	movq	720(%rbp), %rdx
	leaq	.LC51(%rip), %rcx
	call	_Z7d_printPKcz
	movq	728(%rbp), %rax
	movq	%rax, %rcx
	call	strlen
	movq	%rax, %r9
	movq	720(%rbp), %rdx
	movq	my_trace(%rip), %rax
	movq	136(%rax), %rax
	movq	728(%rbp), %r8
	leaq	696(%rbp), %rcx
	movq	%rcx, 32(%rsp)
	movq	%rax, %rcx
	call	_Z12write_memoryPvS_S_yPy
	movq	696(%rbp), %rax
	testq	%rax, %rax
	je	.L116
	leaq	-80(%rbp), %rax
	movq	720(%rbp), %r8
	leaq	.LC52(%rip), %rdx
	movq	%rax, %rcx
	call	sprintf
	leaq	-80(%rbp), %rax
	movq	%rax, %rcx
	call	_Z13add_to_bufferPc
	jmp	.L117
.L116:
	leaq	-80(%rbp), %rax
	movq	720(%rbp), %r8
	leaq	.LC53(%rip), %rdx
	movq	%rax, %rcx
	call	sprintf
	leaq	-80(%rbp), %rax
	movq	%rax, %rcx
	call	_Z13add_to_bufferPc
.L117:
	nop
	addq	$832, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
	.align 8
.LC54:
	.ascii "Writing UNICODE string to 0x%08x\12\0"
	.align 8
.LC55:
	.ascii "# Wrote UNICODE string @ 0x%08x\12\0"
.LC56:
	.ascii "Success\12\0"
	.align 8
.LC57:
	.ascii "# Failed to write UNICODE string @ 0x%08x\12\0"
.LC58:
	.ascii "Failed\12\0"
	.text
	.globl	_Z20write_string_unicodeyPc
	.def	_Z20write_string_unicodeyPc;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z20write_string_unicodeyPc
_Z20write_string_unicodeyPc:
.LFB2451:
	pushq	%rbp
	.seh_pushreg	%rbp
	subq	$848, %rsp
	.seh_stackalloc	848
	leaq	128(%rsp), %rbp
	.seh_setframe	%rbp, 128
	.seh_endprologue
	movq	%rcx, 736(%rbp)
	movq	%rdx, 744(%rbp)
	movq	736(%rbp), %rdx
	leaq	.LC54(%rip), %rcx
	call	_Z7d_printPKcz
	movq	744(%rbp), %rax
	movq	%rax, %rcx
	call	strlen
	addq	$1, %rax
	movl	%eax, %edx
	movl	$2, %eax
	imull	%edx, %eax
	movl	%eax, 716(%rbp)
	movl	716(%rbp), %eax
	movq	%rax, %rcx
	call	malloc
	movq	%rax, 704(%rbp)
	movq	744(%rbp), %rax
	movq	%rax, %rcx
	call	strlen
	addl	$1, %eax
	movq	744(%rbp), %rdx
	movl	%eax, 40(%rsp)
	movq	704(%rbp), %rax
	movq	%rax, 32(%rsp)
	movl	$-1, %r9d
	movq	%rdx, %r8
	movl	$0, %edx
	movl	$1, %ecx
	movq	__imp_MultiByteToWideChar(%rip), %rax
	call	*%rax
	movl	%eax, 700(%rbp)
	movl	716(%rbp), %r9d
	movq	736(%rbp), %rdx
	movq	my_trace(%rip), %rax
	movq	136(%rax), %rax
	movq	704(%rbp), %r8
	leaq	688(%rbp), %rcx
	movq	%rcx, 32(%rsp)
	movq	%rax, %rcx
	call	_Z12write_memoryPvS_S_yPy
	movl	716(%rbp), %edx
	movq	688(%rbp), %rax
	cmpq	%rax, %rdx
	jne	.L120
	leaq	-80(%rbp), %rax
	movq	736(%rbp), %r8
	leaq	.LC55(%rip), %rdx
	movq	%rax, %rcx
	call	sprintf
	leaq	.LC56(%rip), %rcx
	call	_Z7d_printPKcz
	leaq	-80(%rbp), %rax
	movq	%rax, %rcx
	call	_Z13add_to_bufferPc
	jmp	.L121
.L120:
	leaq	-80(%rbp), %rax
	movq	736(%rbp), %r8
	leaq	.LC57(%rip), %rdx
	movq	%rax, %rcx
	call	sprintf
	leaq	.LC58(%rip), %rcx
	call	_Z7d_printPKcz
	leaq	-80(%rbp), %rax
	movq	%rax, %rcx
	call	_Z13add_to_bufferPc
.L121:
	movq	704(%rbp), %rax
	movq	%rax, %rcx
	call	free
	nop
	addq	$848, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
	.align 8
.LC59:
	.ascii "# Increasing skipping in TID: 0x%08x @ %p\12\0"
.LC60:
	.ascii "Skipping in TID: 0x%08x...\12\0"
	.align 8
.LC61:
	.ascii "# Started skipping in TID: 0x%08x\12\0"
	.text
	.globl	_Z13react_skip_onPv
	.def	_Z13react_skip_onPv;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z13react_skip_onPv
_Z13react_skip_onPv:
.LFB2452:
	pushq	%rbp
	.seh_pushreg	%rbp
	subq	$816, %rsp
	.seh_stackalloc	816
	leaq	128(%rsp), %rbp
	.seh_setframe	%rbp, 128
	.seh_endprologue
	movq	%rcx, 704(%rbp)
	movq	704(%rbp), %rax
	movq	%rax, 680(%rbp)
	movq	680(%rbp), %rax
	movl	8(%rax), %eax
	movl	%eax, 676(%rbp)
	movq	my_trace(%rip), %rdx
	movq	my_trace(%rip), %rax
	movl	676(%rbp), %ecx
	addq	$56, %rcx
	movl	(%rax,%rcx,4), %ecx
	movq	my_trace(%rip), %r8
	movq	my_trace(%rip), %rax
	movl	676(%rbp), %r9d
	addq	$56, %r9
	movl	(%rax,%r9,4), %eax
	movl	%eax, %eax
	salq	$5, %rax
	addq	%r8, %rax
	addq	$67109104, %rax
	movzbl	(%rax), %eax
	addl	$1, %eax
	movl	%eax, %r8d
	movl	%ecx, %eax
	salq	$5, %rax
	addq	%rdx, %rax
	addq	$67109104, %rax
	movb	%r8b, (%rax)
	movq	my_trace(%rip), %rax
	movq	68981816(%rax), %rax
	movq	%rax, %rcx
	movl	676(%rbp), %edx
	leaq	-96(%rbp), %rax
	movq	%rcx, %r9
	movl	%edx, %r8d
	leaq	.LC59(%rip), %rdx
	movq	%rax, %rcx
	call	sprintf
	leaq	-96(%rbp), %rax
	movq	%rax, %rcx
	call	_Z13add_to_bufferPc
	movq	my_trace(%rip), %rdx
	movq	my_trace(%rip), %rax
	movl	676(%rbp), %ecx
	addq	$56, %rcx
	movl	(%rax,%rcx,4), %eax
	movl	%eax, %eax
	salq	$5, %rax
	addq	%rdx, %rax
	addq	$67109104, %rax
	movzbl	(%rax), %eax
	cmpb	$1, %al
	jne	.L124
	movl	676(%rbp), %eax
	movl	%eax, %ecx
	call	_Z8unset_ssm
	movl	676(%rbp), %eax
	movl	%eax, %edx
	leaq	.LC60(%rip), %rcx
	call	_Z7d_printPKcz
	movl	676(%rbp), %edx
	leaq	-96(%rbp), %rax
	movl	%edx, %r8d
	leaq	.LC61(%rip), %rdx
	movq	%rax, %rcx
	call	sprintf
	leaq	-96(%rbp), %rax
	movq	%rax, %rcx
	call	_Z13add_to_bufferPc
.L124:
	nop
	addq	$816, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
	.align 8
.LC62:
	.ascii "# Decreasing skipping in TID: 0x%08x @ %p\12\0"
	.align 8
.LC63:
	.ascii "Finished skippingin TID: 0x%08x\12\0"
	.align 8
.LC64:
	.ascii "# Stopped skipping in TID: 0x%08x\12\0"
	.text
	.globl	_Z14react_skip_offPv
	.def	_Z14react_skip_offPv;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z14react_skip_offPv
_Z14react_skip_offPv:
.LFB2453:
	pushq	%rbp
	.seh_pushreg	%rbp
	subq	$816, %rsp
	.seh_stackalloc	816
	leaq	128(%rsp), %rbp
	.seh_setframe	%rbp, 128
	.seh_endprologue
	movq	%rcx, 704(%rbp)
	movq	704(%rbp), %rax
	movq	%rax, 680(%rbp)
	movq	680(%rbp), %rax
	movl	8(%rax), %eax
	movl	%eax, 676(%rbp)
	movq	my_trace(%rip), %rdx
	movq	my_trace(%rip), %rax
	movl	676(%rbp), %ecx
	addq	$56, %rcx
	movl	(%rax,%rcx,4), %ecx
	movq	my_trace(%rip), %r8
	movq	my_trace(%rip), %rax
	movl	676(%rbp), %r9d
	addq	$56, %r9
	movl	(%rax,%r9,4), %eax
	movl	%eax, %eax
	salq	$5, %rax
	addq	%r8, %rax
	addq	$67109104, %rax
	movzbl	(%rax), %eax
	subl	$1, %eax
	movl	%eax, %r8d
	movl	%ecx, %eax
	salq	$5, %rax
	addq	%rdx, %rax
	addq	$67109104, %rax
	movb	%r8b, (%rax)
	movq	my_trace(%rip), %rax
	movq	68981816(%rax), %rax
	movq	%rax, %rcx
	movl	676(%rbp), %edx
	leaq	-96(%rbp), %rax
	movq	%rcx, %r9
	movl	%edx, %r8d
	leaq	.LC62(%rip), %rdx
	movq	%rax, %rcx
	call	sprintf
	leaq	-96(%rbp), %rax
	movq	%rax, %rcx
	call	_Z13add_to_bufferPc
	movq	my_trace(%rip), %rdx
	movq	my_trace(%rip), %rax
	movl	676(%rbp), %ecx
	addq	$56, %rcx
	movl	(%rax,%rcx,4), %eax
	movl	%eax, %eax
	salq	$5, %rax
	addq	%rdx, %rax
	addq	$67109104, %rax
	movzbl	(%rax), %eax
	testb	%al, %al
	jg	.L127
	movl	676(%rbp), %eax
	movl	%eax, %edx
	leaq	.LC63(%rip), %rcx
	call	_Z7d_printPKcz
	movq	my_trace(%rip), %rdx
	movq	my_trace(%rip), %rax
	movl	676(%rbp), %ecx
	addq	$56, %rcx
	movl	(%rax,%rcx,4), %eax
	movl	%eax, %eax
	salq	$5, %rax
	addq	%rdx, %rax
	addq	$67109104, %rax
	movb	$0, (%rax)
	movl	676(%rbp), %eax
	movl	%eax, %ecx
	call	_Z6set_ssm
	movl	676(%rbp), %edx
	leaq	-96(%rbp), %rax
	movl	%edx, %r8d
	leaq	.LC64(%rip), %rdx
	movq	%rax, %rcx
	call	sprintf
	leaq	-96(%rbp), %rax
	movq	%rax, %rcx
	call	_Z13add_to_bufferPc
.L127:
	nop
	addq	$816, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.globl	_Z20enable_all_reactionsv
	.def	_Z20enable_all_reactionsv;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z20enable_all_reactionsv
_Z20enable_all_reactionsv:
.LFB2454:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$48, %rsp
	.seh_stackalloc	48
	.seh_endprologue
	movl	$0, -4(%rbp)
	jmp	.L130
.L131:
	movq	my_trace(%rip), %rax
	movq	70039656(%rax), %rcx
	movl	-4(%rbp), %edx
	movq	%rdx, %rax
	salq	$2, %rax
	addq	%rdx, %rax
	leaq	0(,%rax,4), %rdx
	addq	%rdx, %rax
	salq	$3, %rax
	addq	%rcx, %rax
	movq	%rax, %rcx
	call	_Z15enable_reactionPc
	addl	$1, -4(%rbp)
.L130:
	movq	my_trace(%rip), %rax
	movl	70039664(%rax), %eax
	cmpl	-4(%rbp), %eax
	ja	.L131
	movl	$0, %eax
	addq	$48, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.globl	_Z21disable_all_reactionsv
	.def	_Z21disable_all_reactionsv;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z21disable_all_reactionsv
_Z21disable_all_reactionsv:
.LFB2455:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$48, %rsp
	.seh_stackalloc	48
	.seh_endprologue
	movl	$0, -4(%rbp)
	jmp	.L134
.L135:
	movq	my_trace(%rip), %rax
	movq	70039656(%rax), %rcx
	movl	-4(%rbp), %edx
	movq	%rdx, %rax
	salq	$2, %rax
	addq	%rdx, %rax
	leaq	0(,%rax,4), %rdx
	addq	%rdx, %rax
	salq	$3, %rax
	addq	%rcx, %rax
	movq	%rax, %rcx
	call	_Z16disable_reactionPc
	addl	$1, -4(%rbp)
.L134:
	movq	my_trace(%rip), %rax
	movl	70039664(%rax), %eax
	cmpl	-4(%rbp), %eax
	ja	.L135
	movl	$0, %eax
	addq	$48, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC65:
	.ascii "[raise_reaction]\12\0"
.LC66:
	.ascii "React string: %s\12\0"
.LC67:
	.ascii ":\0"
.LC68:
	.ascii "Found another reaction: %s\12\0"
	.align 8
.LC69:
	.ascii "Setting exclusive reaction %s\12\0"
.LC70:
	.ascii "[raise_reaction ends]\12\0"
	.text
	.globl	_Z14raise_reactionPc
	.def	_Z14raise_reactionPc;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z14raise_reactionPc
_Z14raise_reactionPc:
.LFB2456:
	pushq	%rbp
	.seh_pushreg	%rbp
	subq	$848, %rsp
	.seh_stackalloc	848
	leaq	128(%rsp), %rbp
	.seh_setframe	%rbp, 128
	.seh_endprologue
	movq	%rcx, 736(%rbp)
	leaq	.LC65(%rip), %rcx
	call	_Z7d_printPKcz
	leaq	-96(%rbp), %rax
	movq	736(%rbp), %rdx
	movq	%rax, %rcx
	call	strcpy
	leaq	-96(%rbp), %rax
	movq	%rax, %rdx
	leaq	.LC66(%rip), %rcx
	call	_Z7d_printPKcz
	leaq	-96(%rbp), %rax
	movl	$58, %edx
	movq	%rax, %rcx
	call	strchr
	testq	%rax, %rax
	je	.L138
	leaq	-96(%rbp), %rax
	leaq	.LC67(%rip), %rdx
	movq	%rax, %rcx
	call	strtok
	movq	%rax, 704(%rbp)
	jmp	.L139
.L140:
	movq	704(%rbp), %rax
	movq	%rax, %rdx
	leaq	.LC68(%rip), %rcx
	call	_Z7d_printPKcz
	movq	704(%rbp), %rax
	movq	%rax, %rcx
	call	_Z18exclusive_reactionPc
	leaq	.LC67(%rip), %rdx
	movl	$0, %ecx
	call	strtok
	movq	%rax, 704(%rbp)
.L139:
	cmpq	$0, 704(%rbp)
	jne	.L140
	movl	$0, %eax
	jmp	.L148
.L138:
	movl	$0, 716(%rbp)
	jmp	.L142
.L147:
	movq	my_trace(%rip), %rax
	movq	70039656(%rax), %rcx
	movl	716(%rbp), %edx
	movq	%rdx, %rax
	salq	$2, %rax
	addq	%rdx, %rax
	leaq	0(,%rax,4), %rdx
	addq	%rdx, %rax
	salq	$3, %rax
	addq	%rcx, %rax
	movq	%rax, %rdx
	movq	736(%rbp), %rcx
	call	strcmp
	testl	%eax, %eax
	jne	.L143
	movq	736(%rbp), %rdx
	leaq	.LC69(%rip), %rcx
	call	_Z7d_printPKcz
	movq	my_trace(%rip), %rax
	movq	70039656(%rax), %rcx
	movl	716(%rbp), %edx
	movq	%rdx, %rax
	salq	$2, %rax
	addq	%rdx, %rax
	leaq	0(,%rax,4), %rdx
	addq	%rdx, %rax
	salq	$3, %rax
	addq	%rcx, %rax
	movl	176(%rax), %edx
	addl	$1, %edx
	movl	%edx, 176(%rax)
	movq	my_trace(%rip), %rax
	movq	70039656(%rax), %rcx
	movl	716(%rbp), %edx
	movq	%rdx, %rax
	salq	$2, %rax
	addq	%rdx, %rax
	leaq	0(,%rax,4), %rdx
	addq	%rdx, %rax
	salq	$3, %rax
	addq	%rcx, %rax
	movq	%rax, 688(%rbp)
	movl	$0, 700(%rbp)
	jmp	.L144
.L146:
	movq	688(%rbp), %rcx
	movl	700(%rbp), %edx
	movq	%rdx, %rax
	addq	%rax, %rax
	addq	%rdx, %rax
	addq	%rcx, %rax
	movzbl	3(%rax), %eax
	testb	%al, %al
	je	.L145
	movl	700(%rbp), %edx
	movq	%rdx, %rax
	addq	%rax, %rax
	addq	%rdx, %rax
	movq	688(%rbp), %rdx
	addq	%rdx, %rax
	addq	$3, %rax
	movq	%rax, %rcx
	call	_Z13find_reactionPc
	movq	%rax, 680(%rbp)
	movq	680(%rbp), %rax
	movl	176(%rax), %eax
	leal	1(%rax), %edx
	movq	680(%rbp), %rax
	movl	%edx, 176(%rax)
.L145:
	addl	$1, 700(%rbp)
.L144:
	cmpl	$31, 700(%rbp)
	jbe	.L146
.L143:
	addl	$1, 716(%rbp)
.L142:
	movq	my_trace(%rip), %rax
	movl	70039664(%rax), %eax
	cmpl	716(%rbp), %eax
	ja	.L147
	leaq	.LC70(%rip), %rcx
	call	_Z7d_printPKcz
	movl	$0, %eax
.L148:
	addq	$848, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC71:
	.ascii "[lower_reaction]\12\0"
.LC72:
	.ascii "Lowering reaction %s\12\0"
.LC73:
	.ascii "Lowering coupled reaction %s\12\0"
.LC74:
	.ascii "[lower_reaction ends]\12\0"
	.text
	.globl	_Z14lower_reactionPc
	.def	_Z14lower_reactionPc;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z14lower_reactionPc
_Z14lower_reactionPc:
.LFB2457:
	pushq	%rbp
	.seh_pushreg	%rbp
	subq	$848, %rsp
	.seh_stackalloc	848
	leaq	128(%rsp), %rbp
	.seh_setframe	%rbp, 128
	.seh_endprologue
	movq	%rcx, 736(%rbp)
	leaq	.LC71(%rip), %rcx
	call	_Z7d_printPKcz
	leaq	-96(%rbp), %rax
	movq	736(%rbp), %rdx
	movq	%rax, %rcx
	call	strcpy
	leaq	-96(%rbp), %rax
	movq	%rax, %rdx
	leaq	.LC66(%rip), %rcx
	call	_Z7d_printPKcz
	leaq	-96(%rbp), %rax
	movl	$58, %edx
	movq	%rax, %rcx
	call	strchr
	testq	%rax, %rax
	je	.L150
	leaq	-96(%rbp), %rax
	leaq	.LC67(%rip), %rdx
	movq	%rax, %rcx
	call	strtok
	movq	%rax, 704(%rbp)
	jmp	.L151
.L152:
	movq	704(%rbp), %rax
	movq	%rax, %rdx
	leaq	.LC68(%rip), %rcx
	call	_Z7d_printPKcz
	movq	704(%rbp), %rax
	movq	%rax, %rcx
	call	_Z14lower_reactionPc
	leaq	.LC67(%rip), %rdx
	movl	$0, %ecx
	call	strtok
	movq	%rax, 704(%rbp)
.L151:
	cmpq	$0, 704(%rbp)
	jne	.L152
	movl	$0, %eax
	jmp	.L160
.L150:
	movl	$0, 716(%rbp)
	jmp	.L154
.L159:
	movq	my_trace(%rip), %rax
	movq	70039656(%rax), %rcx
	movl	716(%rbp), %edx
	movq	%rdx, %rax
	salq	$2, %rax
	addq	%rdx, %rax
	leaq	0(,%rax,4), %rdx
	addq	%rdx, %rax
	salq	$3, %rax
	addq	%rcx, %rax
	movq	%rax, %rdx
	movq	736(%rbp), %rcx
	call	strcmp
	testl	%eax, %eax
	jne	.L155
	movq	736(%rbp), %rdx
	leaq	.LC72(%rip), %rcx
	call	_Z7d_printPKcz
	movq	my_trace(%rip), %rax
	movq	70039656(%rax), %rcx
	movl	716(%rbp), %edx
	movq	%rdx, %rax
	salq	$2, %rax
	addq	%rdx, %rax
	leaq	0(,%rax,4), %rdx
	addq	%rdx, %rax
	salq	$3, %rax
	addq	%rcx, %rax
	movl	176(%rax), %edx
	subl	$1, %edx
	movl	%edx, 176(%rax)
	movq	my_trace(%rip), %rax
	movq	70039656(%rax), %rcx
	movl	716(%rbp), %edx
	movq	%rdx, %rax
	salq	$2, %rax
	addq	%rdx, %rax
	leaq	0(,%rax,4), %rdx
	addq	%rdx, %rax
	salq	$3, %rax
	addq	%rcx, %rax
	movq	%rax, 688(%rbp)
	movl	$0, 700(%rbp)
	jmp	.L156
.L158:
	movq	688(%rbp), %rcx
	movl	700(%rbp), %edx
	movq	%rdx, %rax
	addq	%rax, %rax
	addq	%rdx, %rax
	addq	%rcx, %rax
	movzbl	3(%rax), %eax
	testb	%al, %al
	je	.L157
	movl	700(%rbp), %edx
	movq	%rdx, %rax
	addq	%rax, %rax
	addq	%rdx, %rax
	movq	688(%rbp), %rdx
	addq	%rdx, %rax
	addq	$3, %rax
	movq	%rax, %rcx
	call	_Z13find_reactionPc
	movq	%rax, 680(%rbp)
	movq	680(%rbp), %rax
	movq	%rax, %rdx
	leaq	.LC73(%rip), %rcx
	call	_Z7d_printPKcz
	movq	680(%rbp), %rax
	movl	176(%rax), %eax
	leal	-1(%rax), %edx
	movq	680(%rbp), %rax
	movl	%edx, 176(%rax)
.L157:
	addl	$1, 700(%rbp)
.L156:
	cmpl	$31, 700(%rbp)
	jbe	.L158
.L155:
	addl	$1, 716(%rbp)
.L154:
	movq	my_trace(%rip), %rax
	movl	70039664(%rax), %eax
	cmpl	716(%rbp), %eax
	ja	.L159
	leaq	.LC74(%rip), %rcx
	call	_Z7d_printPKcz
	movl	$0, %eax
.L160:
	addq	$848, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC75:
	.ascii "[exclusive_reaction]\12\0"
.LC76:
	.ascii "[exclusive_reaction ends]\12\0"
	.text
	.globl	_Z18exclusive_reactionPc
	.def	_Z18exclusive_reactionPc;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z18exclusive_reactionPc
_Z18exclusive_reactionPc:
.LFB2458:
	pushq	%rbp
	.seh_pushreg	%rbp
	subq	$816, %rsp
	.seh_stackalloc	816
	leaq	128(%rsp), %rbp
	.seh_setframe	%rbp, 128
	.seh_endprologue
	movq	%rcx, 704(%rbp)
	leaq	.LC75(%rip), %rcx
	call	_Z7d_printPKcz
	leaq	-96(%rbp), %rax
	movq	704(%rbp), %rdx
	movq	%rax, %rcx
	call	strcpy
	leaq	-96(%rbp), %rax
	movq	%rax, %rdx
	leaq	.LC66(%rip), %rcx
	call	_Z7d_printPKcz
	leaq	-96(%rbp), %rax
	movl	$58, %edx
	movq	%rax, %rcx
	call	strchr
	testq	%rax, %rax
	je	.L162
	leaq	-96(%rbp), %rax
	leaq	.LC67(%rip), %rdx
	movq	%rax, %rcx
	call	strtok
	movq	%rax, 672(%rbp)
	jmp	.L163
.L164:
	movq	672(%rbp), %rax
	movq	%rax, %rdx
	leaq	.LC68(%rip), %rcx
	call	_Z7d_printPKcz
	movq	672(%rbp), %rax
	movq	%rax, %rcx
	call	_Z18exclusive_reactionPc
	leaq	.LC67(%rip), %rdx
	movl	$0, %ecx
	call	strtok
	movq	%rax, 672(%rbp)
.L163:
	cmpq	$0, 672(%rbp)
	jne	.L164
	movl	$0, %eax
	jmp	.L169
.L162:
	movl	$0, 684(%rbp)
	jmp	.L166
.L168:
	movq	my_trace(%rip), %rax
	movq	70039656(%rax), %rcx
	movl	684(%rbp), %edx
	movq	%rdx, %rax
	salq	$2, %rax
	addq	%rdx, %rax
	leaq	0(,%rax,4), %rdx
	addq	%rdx, %rax
	salq	$3, %rax
	addq	%rcx, %rax
	movq	%rax, %rdx
	movq	704(%rbp), %rcx
	call	strcmp
	testl	%eax, %eax
	jne	.L167
	movq	704(%rbp), %rdx
	leaq	.LC69(%rip), %rcx
	call	_Z7d_printPKcz
	movq	my_trace(%rip), %rax
	movq	70039656(%rax), %rcx
	movl	684(%rbp), %edx
	movq	%rdx, %rax
	salq	$2, %rax
	addq	%rdx, %rax
	leaq	0(,%rax,4), %rdx
	addq	%rdx, %rax
	salq	$3, %rax
	addq	%rcx, %rax
	movb	$1, 173(%rax)
.L167:
	addl	$1, 684(%rbp)
.L166:
	movq	my_trace(%rip), %rax
	movl	70039664(%rax), %eax
	cmpl	684(%rbp), %eax
	ja	.L168
	leaq	.LC76(%rip), %rcx
	call	_Z7d_printPKcz
	movl	$0, %eax
.L169:
	addq	$816, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC77:
	.ascii "[enable_reaction]\12\0"
.LC78:
	.ascii "Enabling reaction %s\12\0"
.LC79:
	.ascii "[enable_reaction ends]\12\0"
	.text
	.globl	_Z15enable_reactionPc
	.def	_Z15enable_reactionPc;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z15enable_reactionPc
_Z15enable_reactionPc:
.LFB2459:
	pushq	%rbp
	.seh_pushreg	%rbp
	subq	$832, %rsp
	.seh_stackalloc	832
	leaq	128(%rsp), %rbp
	.seh_setframe	%rbp, 128
	.seh_endprologue
	movq	%rcx, 720(%rbp)
	leaq	.LC77(%rip), %rcx
	call	_Z7d_printPKcz
	leaq	-96(%rbp), %rax
	movq	720(%rbp), %rdx
	movq	%rax, %rcx
	call	strcpy
	leaq	-96(%rbp), %rax
	movq	%rax, %rdx
	leaq	.LC66(%rip), %rcx
	call	_Z7d_printPKcz
	leaq	-96(%rbp), %rax
	movl	$58, %edx
	movq	%rax, %rcx
	call	strchr
	testq	%rax, %rax
	je	.L171
	leaq	-96(%rbp), %rax
	leaq	.LC67(%rip), %rdx
	movq	%rax, %rcx
	call	strtok
	movq	%rax, 688(%rbp)
	jmp	.L172
.L173:
	movq	688(%rbp), %rax
	movq	%rax, %rdx
	leaq	.LC68(%rip), %rcx
	call	_Z7d_printPKcz
	movq	688(%rbp), %rax
	movq	%rax, %rcx
	call	_Z15enable_reactionPc
	leaq	.LC67(%rip), %rdx
	movl	$0, %ecx
	call	strtok
	movq	%rax, 688(%rbp)
.L172:
	cmpq	$0, 688(%rbp)
	jne	.L173
	movl	$0, %eax
	jmp	.L178
.L171:
	movl	$0, 700(%rbp)
	jmp	.L175
.L177:
	movq	my_trace(%rip), %rax
	movq	70039656(%rax), %rcx
	movl	700(%rbp), %edx
	movq	%rdx, %rax
	salq	$2, %rax
	addq	%rdx, %rax
	leaq	0(,%rax,4), %rdx
	addq	%rdx, %rax
	salq	$3, %rax
	addq	%rcx, %rax
	movq	%rax, %rdx
	movq	720(%rbp), %rcx
	call	strcmp
	testl	%eax, %eax
	jne	.L176
	movq	720(%rbp), %rdx
	leaq	.LC78(%rip), %rcx
	call	_Z7d_printPKcz
	movq	my_trace(%rip), %rax
	movq	70039656(%rax), %rcx
	movl	700(%rbp), %edx
	movq	%rdx, %rax
	salq	$2, %rax
	addq	%rdx, %rax
	leaq	0(,%rax,4), %rdx
	addq	%rdx, %rax
	salq	$3, %rax
	addq	%rcx, %rax
	movb	$1, 172(%rax)
	movq	my_trace(%rip), %rax
	movq	70039656(%rax), %rcx
	movl	700(%rbp), %edx
	movq	%rdx, %rax
	salq	$2, %rax
	addq	%rdx, %rax
	leaq	0(,%rax,4), %rdx
	addq	%rdx, %rax
	salq	$3, %rax
	addq	%rcx, %rax
	movq	184(%rax), %rax
	movq	%rax, 680(%rbp)
	movq	680(%rbp), %rax
	movb	$1, 529(%rax)
	movq	680(%rbp), %rax
	movq	%rax, %rcx
	call	_Z17update_breakpointP11BREAKPOINT_
.L176:
	addl	$1, 700(%rbp)
.L175:
	movq	my_trace(%rip), %rax
	movl	70039664(%rax), %eax
	cmpl	700(%rbp), %eax
	ja	.L177
	leaq	.LC79(%rip), %rcx
	call	_Z7d_printPKcz
	movl	$0, %eax
.L178:
	addq	$832, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC80:
	.ascii "[disable_reaction]\12\0"
.LC81:
	.ascii "Disabling reaction %s\12\0"
	.align 8
.LC82:
	.ascii "Verifying if BP is still enabled\12\0"
.LC83:
	.ascii "Got BP @ %p\12\0"
.LC84:
	.ascii "Current reaction is broken!\12\0"
.LC85:
	.ascii "[disable_reaction ends]\12\0"
	.text
	.globl	_Z16disable_reactionPc
	.def	_Z16disable_reactionPc;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z16disable_reactionPc
_Z16disable_reactionPc:
.LFB2460:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$64, %rsp
	.seh_stackalloc	64
	.seh_endprologue
	movq	%rcx, 16(%rbp)
	leaq	.LC80(%rip), %rcx
	call	_Z7d_printPKcz
	movl	$0, -8(%rbp)
	movl	$0, -4(%rbp)
	jmp	.L180
.L183:
	movq	16(%rbp), %rax
	movzbl	(%rax), %ecx
	movq	my_trace(%rip), %rax
	movq	70039656(%rax), %r8
	movl	-4(%rbp), %edx
	movq	%rdx, %rax
	salq	$2, %rax
	addq	%rdx, %rax
	leaq	0(,%rax,4), %rdx
	addq	%rdx, %rax
	salq	$3, %rax
	addq	%r8, %rax
	movzbl	(%rax), %eax
	cmpb	%al, %cl
	jne	.L181
	movq	16(%rbp), %rax
	addq	$1, %rax
	movzbl	(%rax), %ecx
	movq	my_trace(%rip), %rax
	movq	70039656(%rax), %r8
	movl	-4(%rbp), %edx
	movq	%rdx, %rax
	salq	$2, %rax
	addq	%rdx, %rax
	leaq	0(,%rax,4), %rdx
	addq	%rdx, %rax
	salq	$3, %rax
	addq	%r8, %rax
	movzbl	1(%rax), %eax
	cmpb	%al, %cl
	jne	.L181
	movq	16(%rbp), %rdx
	leaq	.LC81(%rip), %rcx
	call	_Z7d_printPKcz
	movq	my_trace(%rip), %rax
	movq	70039656(%rax), %rcx
	movl	-4(%rbp), %edx
	movq	%rdx, %rax
	salq	$2, %rax
	addq	%rdx, %rax
	leaq	0(,%rax,4), %rdx
	addq	%rdx, %rax
	salq	$3, %rax
	addq	%rcx, %rax
	movb	$0, 172(%rax)
	movl	$1, -8(%rbp)
	jmp	.L182
.L181:
	addl	$1, -4(%rbp)
.L180:
	movq	my_trace(%rip), %rax
	movl	70039664(%rax), %eax
	cmpl	-4(%rbp), %eax
	ja	.L183
.L182:
	cmpl	$0, -8(%rbp)
	jne	.L184
	movl	$0, %eax
	jmp	.L185
.L184:
	leaq	.LC82(%rip), %rcx
	call	_Z7d_printPKcz
	movq	my_trace(%rip), %rax
	movq	70039656(%rax), %rcx
	movl	-4(%rbp), %edx
	movq	%rdx, %rax
	salq	$2, %rax
	addq	%rdx, %rax
	leaq	0(,%rax,4), %rdx
	addq	%rdx, %rax
	salq	$3, %rax
	addq	%rcx, %rax
	movq	184(%rax), %rax
	movq	%rax, -24(%rbp)
	movq	-24(%rbp), %rax
	movq	%rax, %rdx
	leaq	.LC83(%rip), %rcx
	call	_Z7d_printPKcz
	movb	$0, -9(%rbp)
	movl	$0, -4(%rbp)
	jmp	.L186
.L190:
	movq	-24(%rbp), %rax
	movq	536(%rax), %rax
	movl	-4(%rbp), %edx
	salq	$3, %rdx
	addq	%rdx, %rax
	movq	(%rax), %rax
	movq	%rax, -32(%rbp)
	cmpq	$0, -32(%rbp)
	jne	.L187
	leaq	.LC84(%rip), %rcx
	call	_Z7d_printPKcz
.L187:
	movq	-32(%rbp), %rax
	movzbl	172(%rax), %eax
	testb	%al, %al
	je	.L188
	movb	$1, -9(%rbp)
	jmp	.L189
.L188:
	addl	$1, -4(%rbp)
.L186:
	movq	-24(%rbp), %rax
	movl	544(%rax), %eax
	cmpl	-4(%rbp), %eax
	ja	.L190
.L189:
	cmpb	$0, -9(%rbp)
	jne	.L191
	movq	-24(%rbp), %rax
	movb	$0, 529(%rax)
.L191:
	movq	-24(%rbp), %rax
	movq	%rax, %rcx
	call	_Z17update_breakpointP11BREAKPOINT_
	leaq	.LC85(%rip), %rcx
	call	_Z7d_printPKcz
	movl	$0, %eax
.L185:
	addq	$64, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.globl	_Z13add_to_bufferPc
	.def	_Z13add_to_bufferPc;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z13add_to_bufferPc
_Z13add_to_bufferPc:
.LFB2461:
	pushq	%rbp
	.seh_pushreg	%rbp
	pushq	%rbx
	.seh_pushreg	%rbx
	subq	$56, %rsp
	.seh_stackalloc	56
	leaq	128(%rsp), %rbp
	.seh_setframe	%rbp, 128
	.seh_endprologue
	movq	%rcx, -48(%rbp)
	movl	$0, -84(%rbp)
	movq	my_trace(%rip), %rax
	movq	70030584(%rax), %rbx
	movq	-48(%rbp), %rcx
	call	strlen
	movq	%rbx, %r9
	movl	$1, %r8d
	movq	%rax, %rdx
	movq	-48(%rbp), %rcx
	call	fwrite
	movl	%eax, -84(%rbp)
	movq	my_trace(%rip), %rax
	movq	70030584(%rax), %rax
	movq	%rax, %rcx
	call	fflush
	movl	-84(%rbp), %eax
	addq	$56, %rsp
	popq	%rbx
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC86:
	.ascii "Context:\12\0"
.LC87:
	.ascii "EAX:\11"
	.ascii "0x%08x\12\0"
.LC88:
	.ascii "EBX:\11"
	.ascii "0x%08x\12\0"
.LC89:
	.ascii "ECX:\11"
	.ascii "0x%08x\12\0"
.LC90:
	.ascii "EDX:\11"
	.ascii "0x%08x\12\0"
.LC91:
	.ascii "ESI:\11"
	.ascii "0x%08x\12\0"
.LC92:
	.ascii "EDI:\11"
	.ascii "0x%08x\12\0"
.LC93:
	.ascii "EBP:\11"
	.ascii "0x%08x\12\0"
.LC94:
	.ascii "ESP:\11"
	.ascii "0x%08x\12\0"
.LC95:
	.ascii "EFLAGS:\11"
	.ascii "0x%08x\12\0"
.LC96:
	.ascii "EIP:\11"
	.ascii "0x%08x\12\0"
	.text
	.globl	_Z13print_contextP8_CONTEXT
	.def	_Z13print_contextP8_CONTEXT;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z13print_contextP8_CONTEXT
_Z13print_contextP8_CONTEXT:
.LFB2462:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$32, %rsp
	.seh_stackalloc	32
	.seh_endprologue
	movq	%rcx, 16(%rbp)
	leaq	.LC86(%rip), %rcx
	call	_Z7d_printPKcz
	movq	16(%rbp), %rax
	movq	120(%rax), %rax
	movq	%rax, %rdx
	leaq	.LC87(%rip), %rcx
	call	_Z7d_printPKcz
	movq	16(%rbp), %rax
	movq	144(%rax), %rax
	movq	%rax, %rdx
	leaq	.LC88(%rip), %rcx
	call	_Z7d_printPKcz
	movq	16(%rbp), %rax
	movq	128(%rax), %rax
	movq	%rax, %rdx
	leaq	.LC89(%rip), %rcx
	call	_Z7d_printPKcz
	movq	16(%rbp), %rax
	movq	136(%rax), %rax
	movq	%rax, %rdx
	leaq	.LC90(%rip), %rcx
	call	_Z7d_printPKcz
	movq	16(%rbp), %rax
	movq	168(%rax), %rax
	movq	%rax, %rdx
	leaq	.LC91(%rip), %rcx
	call	_Z7d_printPKcz
	movq	16(%rbp), %rax
	movq	176(%rax), %rax
	movq	%rax, %rdx
	leaq	.LC92(%rip), %rcx
	call	_Z7d_printPKcz
	movq	16(%rbp), %rax
	movq	160(%rax), %rax
	movq	%rax, %rdx
	leaq	.LC93(%rip), %rcx
	call	_Z7d_printPKcz
	movq	16(%rbp), %rax
	movq	152(%rax), %rax
	movq	%rax, %rdx
	leaq	.LC94(%rip), %rcx
	call	_Z7d_printPKcz
	movq	16(%rbp), %rax
	movl	68(%rax), %eax
	movl	%eax, %edx
	leaq	.LC95(%rip), %rcx
	call	_Z7d_printPKcz
	movq	16(%rbp), %rax
	movq	248(%rax), %rax
	movq	%rax, %rdx
	leaq	.LC96(%rip), %rcx
	call	_Z7d_printPKcz
	nop
	addq	$32, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC97:
	.ascii "Refreshing\12\0"
.LC98:
	.ascii "Unsetting SS for 0x%08x\12\0"
	.text
	.globl	_Z20react_sysret_refreshPv
	.def	_Z20react_sysret_refreshPv;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z20react_sysret_refreshPv
_Z20react_sysret_refreshPv:
.LFB2463:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$32, %rsp
	.seh_stackalloc	32
	.seh_endprologue
	movq	%rcx, 16(%rbp)
	leaq	.LC97(%rip), %rcx
	call	_Z7d_printPKcz
	movq	my_trace(%rip), %rax
	movl	68981968(%rax), %eax
	movl	%eax, %edx
	leaq	.LC98(%rip), %rcx
	call	_Z7d_printPKcz
	movq	my_trace(%rip), %rax
	movl	68981968(%rax), %eax
	movl	%eax, %ecx
	call	_Z8unset_ssm
	movl	$0, %ecx
	call	_Z6set_ssm
	nop
	addq	$32, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC99:
	.ascii "Error: 0x%08x\12\0"
	.text
	.globl	_Z11read_memoryPvS_S_yPy
	.def	_Z11read_memoryPvS_S_yPy;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z11read_memoryPvS_S_yPy
_Z11read_memoryPvS_S_yPy:
.LFB2464:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$64, %rsp
	.seh_stackalloc	64
	.seh_endprologue
	movq	%rcx, 16(%rbp)
	movq	%rdx, 24(%rbp)
	movq	%r8, 32(%rbp)
	movq	%r9, 40(%rbp)
	movq	40(%rbp), %rcx
	movq	32(%rbp), %rax
	leaq	-8(%rbp), %rdx
	movq	%rdx, 32(%rsp)
	movl	$64, %r9d
	movq	%rcx, %r8
	movq	%rax, %rdx
	movq	16(%rbp), %rcx
	movq	__imp_VirtualProtectEx(%rip), %rax
	call	*%rax
	movq	40(%rbp), %r8
	movq	32(%rbp), %rcx
	movq	24(%rbp), %rax
	movq	48(%rbp), %rdx
	movq	%rdx, 32(%rsp)
	movq	%r8, %r9
	movq	%rcx, %r8
	movq	%rax, %rdx
	movq	16(%rbp), %rcx
	movq	__imp_ReadProcessMemory(%rip), %rax
	call	*%rax
	movl	%eax, -4(%rbp)
	movl	-8(%rbp), %r8d
	movq	40(%rbp), %rcx
	movq	32(%rbp), %rax
	leaq	-8(%rbp), %rdx
	movq	%rdx, 32(%rsp)
	movl	%r8d, %r9d
	movq	%rcx, %r8
	movq	%rax, %rdx
	movq	16(%rbp), %rcx
	movq	__imp_VirtualProtectEx(%rip), %rax
	call	*%rax
	cmpl	$0, -4(%rbp)
	jne	.L197
	movq	__imp_GetLastError(%rip), %rax
	call	*%rax
	movl	%eax, %edx
	leaq	.LC99(%rip), %rcx
	call	_Z7d_printPKcz
.L197:
	nop
	addq	$64, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC100:
	.ascii "write_memory, handle: 0x%08x\12\0"
	.text
	.globl	_Z12write_memoryPvS_S_yPy
	.def	_Z12write_memoryPvS_S_yPy;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z12write_memoryPvS_S_yPy
_Z12write_memoryPvS_S_yPy:
.LFB2465:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$64, %rsp
	.seh_stackalloc	64
	.seh_endprologue
	movq	%rcx, 16(%rbp)
	movq	%rdx, 24(%rbp)
	movq	%r8, 32(%rbp)
	movq	%r9, 40(%rbp)
	movq	16(%rbp), %rdx
	leaq	.LC100(%rip), %rcx
	call	_Z7d_printPKcz
	movq	40(%rbp), %rcx
	movq	24(%rbp), %rax
	leaq	-8(%rbp), %rdx
	movq	%rdx, 32(%rsp)
	movl	$64, %r9d
	movq	%rcx, %r8
	movq	%rax, %rdx
	movq	16(%rbp), %rcx
	movq	__imp_VirtualProtectEx(%rip), %rax
	call	*%rax
	movq	40(%rbp), %r8
	movq	32(%rbp), %rcx
	movq	24(%rbp), %rax
	movq	48(%rbp), %rdx
	movq	%rdx, 32(%rsp)
	movq	%r8, %r9
	movq	%rcx, %r8
	movq	%rax, %rdx
	movq	16(%rbp), %rcx
	movq	__imp_WriteProcessMemory(%rip), %rax
	call	*%rax
	movl	%eax, -4(%rbp)
	cmpl	$0, -4(%rbp)
	jne	.L200
	movq	__imp_GetLastError(%rip), %rax
	call	*%rax
	movl	%eax, %edx
	leaq	.LC99(%rip), %rcx
	call	_Z7d_printPKcz
.L200:
	movl	-8(%rbp), %r8d
	movq	40(%rbp), %rcx
	movq	24(%rbp), %rax
	leaq	-8(%rbp), %rdx
	movq	%rdx, 32(%rsp)
	movl	%r8d, %r9d
	movq	%rcx, %r8
	movq	%rax, %rdx
	movq	16(%rbp), %rcx
	movq	__imp_VirtualProtectEx(%rip), %rax
	call	*%rax
	nop
	addq	$64, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC101:
	.ascii "before: 0x%08x\12\0"
.LC102:
	.ascii "after: 0x%08x\12\0"
	.text
	.globl	_Z7dec_eipm
	.def	_Z7dec_eipm;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z7dec_eipm
_Z7dec_eipm:
.LFB2466:
	pushq	%rbp
	.seh_pushreg	%rbp
	subq	$1280, %rsp
	.seh_stackalloc	1280
	leaq	128(%rsp), %rbp
	.seh_setframe	%rbp, 128
	.seh_endprologue
	movl	%ecx, 1168(%rbp)
	movq	$-1, 1144(%rbp)
	movq	my_trace(%rip), %rax
	movl	1168(%rbp), %edx
	addq	$56, %rdx
	movl	(%rax,%rdx,4), %eax
	movl	%eax, 1140(%rbp)
	movq	my_trace(%rip), %rax
	movl	1140(%rbp), %edx
	addq	$2097159, %rdx
	salq	$5, %rdx
	addq	%rdx, %rax
	movq	8(%rax), %rax
	movq	%rax, 1144(%rbp)
	movl	$1048577, -48(%rbp)
	leaq	-96(%rbp), %rdx
	movq	1144(%rbp), %rax
	movq	%rax, %rcx
	movq	__imp_GetThreadContext(%rip), %rax
	call	*%rax
	testl	%eax, %eax
	sete	%al
	testb	%al, %al
	je	.L202
	movq	__imp_GetLastError(%rip), %rax
	call	*%rax
	movl	%eax, %edx
	leaq	.LC7(%rip), %rcx
	call	_Z7d_printPKcz
.L202:
	movq	152(%rbp), %rax
	movq	%rax, %rdx
	leaq	.LC101(%rip), %rcx
	call	_Z7d_printPKcz
	movq	152(%rbp), %rax
	subq	$1, %rax
	movq	%rax, 152(%rbp)
	movq	152(%rbp), %rax
	movq	%rax, %rdx
	leaq	.LC102(%rip), %rcx
	call	_Z7d_printPKcz
	leaq	-96(%rbp), %rdx
	movq	1144(%rbp), %rax
	movq	%rax, %rcx
	movq	__imp_SetThreadContext(%rip), %rax
	call	*%rax
	movl	$0, %eax
	addq	$1280, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC103:
	.ascii "0x%08x,0x%08x,0x%08x\0"
	.text
	.globl	_Z19serialize_exception17_EXCEPTION_RECORDPc
	.def	_Z19serialize_exception17_EXCEPTION_RECORDPc;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z19serialize_exception17_EXCEPTION_RECORDPc
_Z19serialize_exception17_EXCEPTION_RECORDPc:
.LFB2467:
	pushq	%rbp
	.seh_pushreg	%rbp
	pushq	%rbx
	.seh_pushreg	%rbx
	subq	$56, %rsp
	.seh_stackalloc	56
	leaq	128(%rsp), %rbp
	.seh_setframe	%rbp, 128
	.seh_endprologue
	movq	%rcx, %rbx
	movq	%rdx, -40(%rbp)
	movq	16(%rbx), %rdx
	movl	4(%rbx), %r8d
	movl	(%rbx), %ecx
	movq	-40(%rbp), %rax
	movq	%rdx, 32(%rsp)
	movl	%r8d, %r9d
	movl	%ecx, %r8d
	leaq	.LC103(%rip), %rdx
	movq	%rax, %rcx
	call	sprintf
	nop
	addq	$56, %rsp
	popq	%rbx
	popq	%rbp
	ret
	.seh_endproc
	.globl	_Z21deserialize_exceptionP17_EXCEPTION_RECORDPc
	.def	_Z21deserialize_exceptionP17_EXCEPTION_RECORDPc;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z21deserialize_exceptionP17_EXCEPTION_RECORDPc
_Z21deserialize_exceptionP17_EXCEPTION_RECORDPc:
.LFB2468:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$48, %rsp
	.seh_stackalloc	48
	.seh_endprologue
	movq	%rcx, 16(%rbp)
	movq	%rdx, 24(%rbp)
	movq	16(%rbp), %rax
	movq	16(%rax), %rdx
	movq	16(%rbp), %rax
	movl	4(%rax), %r8d
	movq	16(%rbp), %rax
	movl	(%rax), %ecx
	movq	24(%rbp), %rax
	movq	%rdx, 32(%rsp)
	movl	%r8d, %r9d
	movl	%ecx, %r8d
	leaq	.LC103(%rip), %rdx
	movq	%rax, %rcx
	call	sscanf
	nop
	addq	$48, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.globl	_Z16serialize_threadP13_THREAD_ENTRYPc
	.def	_Z16serialize_threadP13_THREAD_ENTRYPc;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z16serialize_threadP13_THREAD_ENTRYPc
_Z16serialize_threadP13_THREAD_ENTRYPc:
.LFB2469:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$48, %rsp
	.seh_stackalloc	48
	.seh_endprologue
	movq	%rcx, 16(%rbp)
	movq	%rdx, 24(%rbp)
	movq	16(%rbp), %rax
	movzbl	4(%rax), %eax
	movsbl	%al, %edx
	movq	16(%rbp), %rax
	movq	8(%rax), %r8
	movq	16(%rbp), %rax
	movl	(%rax), %ecx
	movq	24(%rbp), %rax
	movl	%edx, 32(%rsp)
	movq	%r8, %r9
	movl	%ecx, %r8d
	leaq	.LC103(%rip), %rdx
	movq	%rax, %rcx
	call	sprintf
	nop
	addq	$48, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC104:
	.ascii "0x%08x,%s\0"
	.text
	.globl	_Z13serialize_libP10_LIB_ENTRYPc
	.def	_Z13serialize_libP10_LIB_ENTRYPc;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z13serialize_libP10_LIB_ENTRYPc
_Z13serialize_libP10_LIB_ENTRYPc:
.LFB2470:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$32, %rsp
	.seh_stackalloc	32
	.seh_endprologue
	movq	%rcx, 16(%rbp)
	movq	%rdx, 24(%rbp)
	movq	16(%rbp), %rax
	leaq	16(%rax), %rcx
	movq	16(%rbp), %rax
	movq	8(%rax), %rdx
	movq	24(%rbp), %rax
	movq	%rcx, %r9
	movq	%rdx, %r8
	leaq	.LC104(%rip), %rdx
	movq	%rax, %rcx
	call	sprintf
	nop
	addq	$32, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
	.align 8
.LC105:
	.ascii "0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x\0"
	.text
	.globl	_Z17serialize_context8_CONTEXTP10_LDT_ENTRYPc
	.def	_Z17serialize_context8_CONTEXTP10_LDT_ENTRYPc;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z17serialize_context8_CONTEXTP10_LDT_ENTRYPc
_Z17serialize_context8_CONTEXTP10_LDT_ENTRYPc:
.LFB2471:
	pushq	%rbp
	.seh_pushreg	%rbp
	pushq	%r15
	.seh_pushreg	%r15
	pushq	%r14
	.seh_pushreg	%r14
	pushq	%r13
	.seh_pushreg	%r13
	pushq	%r12
	.seh_pushreg	%r12
	pushq	%rdi
	.seh_pushreg	%rdi
	pushq	%rsi
	.seh_pushreg	%rsi
	pushq	%rbx
	.seh_pushreg	%rbx
	subq	$408, %rsp
	.seh_stackalloc	408
	leaq	128(%rsp), %rbp
	.seh_setframe	%rbp, 128
	.seh_endprologue
	movq	%rcx, %rbx
	movq	%rdx, 360(%rbp)
	movq	%r8, 368(%rbp)
	movq	360(%rbp), %rax
	addq	$40, %rax
	movq	%rax, 264(%rbp)
	movq	360(%rbp), %rax
	addq	$40, %rax
	movzwl	2(%rax), %eax
	movzwl	%ax, %esi
	movl	%esi, 260(%rbp)
	movq	360(%rbp), %rax
	addq	$40, %rax
	movzwl	(%rax), %eax
	movzwl	%ax, %edi
	movl	%edi, 256(%rbp)
	movq	360(%rbp), %rax
	leaq	32(%rax), %rdx
	movq	%rdx, 248(%rbp)
	movq	360(%rbp), %rax
	addq	$32, %rax
	movzwl	2(%rax), %eax
	movzwl	%ax, %ecx
	movl	%ecx, 244(%rbp)
	movq	360(%rbp), %rax
	addq	$32, %rax
	movzwl	(%rax), %eax
	movzwl	%ax, %r10d
	movl	%r10d, 240(%rbp)
	movq	360(%rbp), %rax
	leaq	24(%rax), %r11
	movq	%r11, 232(%rbp)
	movq	360(%rbp), %rax
	addq	$24, %rax
	movzwl	2(%rax), %eax
	movzwl	%ax, %r14d
	movl	%r14d, 228(%rbp)
	movq	360(%rbp), %rax
	addq	$24, %rax
	movzwl	(%rax), %eax
	movzwl	%ax, %r15d
	movl	%r15d, 224(%rbp)
	movq	360(%rbp), %rax
	leaq	16(%rax), %r9
	movq	%r9, 216(%rbp)
	movq	360(%rbp), %rax
	addq	$16, %rax
	movzwl	2(%rax), %eax
	movzwl	%ax, %r8d
	movl	%r8d, 212(%rbp)
	movq	360(%rbp), %rax
	addq	$16, %rax
	movzwl	(%rax), %eax
	movzwl	%ax, %r12d
	movl	%r12d, 208(%rbp)
	movq	360(%rbp), %rax
	leaq	8(%rax), %r13
	movq	%r13, 200(%rbp)
	movq	360(%rbp), %rax
	addq	$8, %rax
	movzwl	2(%rax), %eax
	movzwl	%ax, %esi
	movl	%esi, 196(%rbp)
	movq	360(%rbp), %rax
	addq	$8, %rax
	movzwl	(%rax), %eax
	movzwl	%ax, %edi
	movl	%edi, 192(%rbp)
	movq	360(%rbp), %rax
	movzwl	2(%rax), %eax
	movzwl	%ax, %edx
	movl	%edx, 188(%rbp)
	movq	360(%rbp), %rax
	movzwl	(%rax), %eax
	movzwl	%ax, %ecx
	movl	%ecx, 184(%rbp)
	movzwl	66(%rbx), %eax
	movzwl	%ax, %r10d
	movl	%r10d, 180(%rbp)
	movzwl	56(%rbx), %eax
	movzwl	%ax, %r11d
	movl	%r11d, 176(%rbp)
	movzwl	58(%rbx), %eax
	movzwl	%ax, %r14d
	movl	%r14d, 172(%rbp)
	movzwl	60(%rbx), %eax
	movzwl	%ax, %r15d
	movl	%r15d, 168(%rbp)
	movzwl	62(%rbx), %eax
	movzwl	%ax, %r9d
	movl	%r9d, 164(%rbp)
	movzwl	64(%rbx), %eax
	movzwl	%ax, %r15d
	movq	248(%rbx), %r14
	movl	68(%rbx), %r13d
	movq	152(%rbx), %r12
	movq	160(%rbx), %rdi
	movq	176(%rbx), %r11
	movq	168(%rbx), %r9
	movq	144(%rbx), %r8
	movq	136(%rbx), %rdx
	movq	128(%rbx), %rsi
	movq	120(%rbx), %r10
	movq	368(%rbp), %rcx
	movq	264(%rbp), %rax
	movl	4(%rax), %eax
	movl	%eax, 280(%rsp)
	movl	260(%rbp), %eax
	movl	%eax, 272(%rsp)
	movl	256(%rbp), %eax
	movl	%eax, 264(%rsp)
	movq	248(%rbp), %rax
	movl	4(%rax), %eax
	movl	%eax, 256(%rsp)
	movl	244(%rbp), %eax
	movl	%eax, 248(%rsp)
	movl	240(%rbp), %eax
	movl	%eax, 240(%rsp)
	movq	232(%rbp), %rax
	movl	4(%rax), %eax
	movl	%eax, 232(%rsp)
	movl	228(%rbp), %eax
	movl	%eax, 224(%rsp)
	movl	224(%rbp), %eax
	movl	%eax, 216(%rsp)
	movq	216(%rbp), %rax
	movl	4(%rax), %eax
	movl	%eax, 208(%rsp)
	movl	212(%rbp), %eax
	movl	%eax, 200(%rsp)
	movl	208(%rbp), %eax
	movl	%eax, 192(%rsp)
	movq	200(%rbp), %rax
	movl	4(%rax), %eax
	movl	%eax, 184(%rsp)
	movl	196(%rbp), %eax
	movl	%eax, 176(%rsp)
	movl	192(%rbp), %eax
	movl	%eax, 168(%rsp)
	movq	360(%rbp), %rax
	movl	4(%rax), %eax
	movl	%eax, 160(%rsp)
	movl	188(%rbp), %eax
	movl	%eax, 152(%rsp)
	movl	184(%rbp), %eax
	movl	%eax, 144(%rsp)
	movl	180(%rbp), %eax
	movl	%eax, 136(%rsp)
	movl	176(%rbp), %eax
	movl	%eax, 128(%rsp)
	movl	172(%rbp), %eax
	movl	%eax, 120(%rsp)
	movl	168(%rbp), %eax
	movl	%eax, 112(%rsp)
	movl	164(%rbp), %eax
	movl	%eax, 104(%rsp)
	movl	%r15d, 96(%rsp)
	movq	%r14, 88(%rsp)
	movl	%r13d, 80(%rsp)
	movq	%r12, 72(%rsp)
	movq	%rdi, 64(%rsp)
	movq	%r11, 56(%rsp)
	movq	%r9, 48(%rsp)
	movq	%r8, 40(%rsp)
	movq	%rdx, 32(%rsp)
	movq	%rsi, %r9
	movq	%r10, %r8
	leaq	.LC105(%rip), %rdx
	call	sprintf
	nop
	addq	$408, %rsp
	popq	%rbx
	popq	%rsi
	popq	%rdi
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
	.align 8
.LC106:
	.ascii "0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x\0"
	.text
	.globl	_Z19deserialize_contextP8_CONTEXTPc
	.def	_Z19deserialize_contextP8_CONTEXTPc;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z19deserialize_contextP8_CONTEXTPc
_Z19deserialize_contextP8_CONTEXTPc:
.LFB2472:
	pushq	%rbp
	.seh_pushreg	%rbp
	pushq	%r12
	.seh_pushreg	%r12
	pushq	%rdi
	.seh_pushreg	%rdi
	pushq	%rsi
	.seh_pushreg	%rsi
	pushq	%rbx
	.seh_pushreg	%rbx
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$96, %rsp
	.seh_stackalloc	96
	.seh_endprologue
	movq	%rcx, 48(%rbp)
	movq	%rdx, 56(%rbp)
	movq	48(%rbp), %rax
	movq	248(%rax), %rsi
	movq	48(%rbp), %rax
	movl	68(%rax), %ebx
	movq	48(%rbp), %rax
	movq	152(%rax), %r11
	movq	48(%rbp), %rax
	movq	160(%rax), %r10
	movq	48(%rbp), %rax
	movq	176(%rax), %r9
	movq	48(%rbp), %rax
	movq	168(%rax), %r8
	movq	48(%rbp), %rax
	movq	144(%rax), %rcx
	movq	48(%rbp), %rax
	movq	136(%rax), %rdx
	movq	48(%rbp), %rax
	movq	128(%rax), %r12
	movq	48(%rbp), %rax
	movq	120(%rax), %rdi
	movq	56(%rbp), %rax
	movq	%rsi, 88(%rsp)
	movl	%ebx, 80(%rsp)
	movq	%r11, 72(%rsp)
	movq	%r10, 64(%rsp)
	movq	%r9, 56(%rsp)
	movq	%r8, 48(%rsp)
	movq	%rcx, 40(%rsp)
	movq	%rdx, 32(%rsp)
	movq	%r12, %r9
	movq	%rdi, %r8
	leaq	.LC106(%rip), %rdx
	movq	%rax, %rcx
	call	sscanf
	nop
	addq	$96, %rsp
	popq	%rbx
	popq	%rsi
	popq	%rdi
	popq	%r12
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
	.align 8
.LC107:
	.ascii "Registering Exception: code 0x%08x at: 0x%08x\12\0"
.LC108:
	.ascii "EX,0x%08x,%s\12\0"
	.text
	.globl	_Z18register_exceptionm17_EXCEPTION_RECORD
	.def	_Z18register_exceptionm17_EXCEPTION_RECORD;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z18register_exceptionm17_EXCEPTION_RECORD
_Z18register_exceptionm17_EXCEPTION_RECORD:
.LFB2473:
	pushq	%rbp
	.seh_pushreg	%rbp
	pushq	%rdi
	.seh_pushreg	%rdi
	pushq	%rsi
	.seh_pushreg	%rsi
	pushq	%rbx
	.seh_pushreg	%rbx
	subq	$1224, %rsp
	.seh_stackalloc	1224
	leaq	128(%rsp), %rbp
	.seh_setframe	%rbp, 128
	.seh_endprologue
	movl	%ecx, 1136(%rbp)
	movq	%rdx, %rbx
	movq	16(%rbx), %rdx
	movl	(%rbx), %eax
	movq	%rdx, %r8
	movl	%eax, %edx
	leaq	.LC107(%rip), %rcx
	call	_Z7d_printPKcz
	leaq	-96(%rbp), %rax
	movq	%rbx, %r8
	movl	$19, %edx
	movq	%rax, %rdi
	movq	%r8, %rsi
	movq	%rdx, %rcx
	rep movsq
	leaq	64(%rbp), %rdx
	leaq	-96(%rbp), %rax
	movq	%rax, %rcx
	call	_Z19serialize_exception17_EXCEPTION_RECORDPc
	leaq	64(%rbp), %rdx
	leaq	320(%rbp), %rax
	movq	%rdx, %r9
	movl	1136(%rbp), %r8d
	leaq	.LC108(%rip), %rdx
	movq	%rax, %rcx
	call	sprintf
	leaq	320(%rbp), %rax
	movq	%rax, %rcx
	call	_Z13add_to_bufferPc
	nop
	addq	$1224, %rsp
	popq	%rbx
	popq	%rsi
	popq	%rdi
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC109:
	.ascii "Failed: 0x%08x\12\0"
	.text
	.globl	_Z18getSelectorEntriesPv8_CONTEXTP10_LDT_ENTRY
	.def	_Z18getSelectorEntriesPv8_CONTEXTP10_LDT_ENTRY;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z18getSelectorEntriesPv8_CONTEXTP10_LDT_ENTRY
_Z18getSelectorEntriesPv8_CONTEXTP10_LDT_ENTRY:
.LFB2474:
	pushq	%rbp
	.seh_pushreg	%rbp
	pushq	%rbx
	.seh_pushreg	%rbx
	subq	$40, %rsp
	.seh_stackalloc	40
	leaq	128(%rsp), %rbp
	.seh_setframe	%rbp, 128
	.seh_endprologue
	movq	%rcx, -64(%rbp)
	movq	%rdx, %rbx
	movq	%r8, -48(%rbp)
	movzwl	64(%rbx), %eax
	movzwl	%ax, %eax
	movq	-48(%rbp), %rdx
	movq	%rdx, %r8
	movl	%eax, %edx
	movq	-64(%rbp), %rcx
	movq	__imp_GetThreadSelectorEntry(%rip), %rax
	call	*%rax
	testl	%eax, %eax
	sete	%al
	testb	%al, %al
	je	.L217
	movq	__imp_GetLastError(%rip), %rax
	call	*%rax
	movl	%eax, %edx
	leaq	.LC109(%rip), %rcx
	call	_Z7d_printPKcz
.L217:
	movq	-48(%rbp), %rax
	leaq	8(%rax), %rdx
	movzwl	62(%rbx), %eax
	movzwl	%ax, %eax
	movq	%rdx, %r8
	movl	%eax, %edx
	movq	-64(%rbp), %rcx
	movq	__imp_GetThreadSelectorEntry(%rip), %rax
	call	*%rax
	testl	%eax, %eax
	sete	%al
	testb	%al, %al
	je	.L218
	leaq	.LC58(%rip), %rcx
	call	_Z7d_printPKcz
.L218:
	movq	-48(%rbp), %rax
	leaq	16(%rax), %rdx
	movzwl	60(%rbx), %eax
	movzwl	%ax, %eax
	movq	%rdx, %r8
	movl	%eax, %edx
	movq	-64(%rbp), %rcx
	movq	__imp_GetThreadSelectorEntry(%rip), %rax
	call	*%rax
	testl	%eax, %eax
	sete	%al
	testb	%al, %al
	je	.L219
	leaq	.LC58(%rip), %rcx
	call	_Z7d_printPKcz
.L219:
	movq	-48(%rbp), %rax
	leaq	24(%rax), %rdx
	movzwl	58(%rbx), %eax
	movzwl	%ax, %eax
	movq	%rdx, %r8
	movl	%eax, %edx
	movq	-64(%rbp), %rcx
	movq	__imp_GetThreadSelectorEntry(%rip), %rax
	call	*%rax
	testl	%eax, %eax
	sete	%al
	testb	%al, %al
	je	.L220
	leaq	.LC58(%rip), %rcx
	call	_Z7d_printPKcz
.L220:
	movq	-48(%rbp), %rax
	leaq	32(%rax), %rdx
	movzwl	56(%rbx), %eax
	movzwl	%ax, %eax
	movq	%rdx, %r8
	movl	%eax, %edx
	movq	-64(%rbp), %rcx
	movq	__imp_GetThreadSelectorEntry(%rip), %rax
	call	*%rax
	testl	%eax, %eax
	sete	%al
	testb	%al, %al
	je	.L221
	leaq	.LC58(%rip), %rcx
	call	_Z7d_printPKcz
.L221:
	movq	-48(%rbp), %rax
	leaq	40(%rax), %rdx
	movzwl	66(%rbx), %eax
	movzwl	%ax, %eax
	movq	%rdx, %r8
	movl	%eax, %edx
	movq	-64(%rbp), %rcx
	movq	__imp_GetThreadSelectorEntry(%rip), %rax
	call	*%rax
	testl	%eax, %eax
	sete	%al
	testb	%al, %al
	je	.L222
	leaq	.LC58(%rip), %rcx
	call	_Z7d_printPKcz
.L222:
	nop
	addq	$40, %rsp
	popq	%rbx
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
	.align 8
.LC110:
	.ascii "Registering: TID 0x%08x, handle 0x%08x\12\0"
.LC111:
	.ascii "# Thread count: 0x%08x\12\0"
	.align 8
.LC112:
	.ascii "Updating: TID 0x%08x, handle 0x%08x\12\0"
.LC113:
	.ascii "RT,0x%08x,%s\12\0"
	.text
	.globl	_Z15register_threadmPv
	.def	_Z15register_threadmPv;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z15register_threadmPv
_Z15register_threadmPv:
.LFB2475:
	pushq	%rbp
	.seh_pushreg	%rbp
	pushq	%rdi
	.seh_pushreg	%rdi
	pushq	%rsi
	.seh_pushreg	%rsi
	subq	$3840, %rsp
	.seh_stackalloc	3840
	leaq	128(%rsp), %rbp
	.seh_setframe	%rbp, 128
	.seh_endprologue
	movl	%ecx, 3744(%rbp)
	movq	%rdx, 3752(%rbp)
	cmpq	$0, 3752(%rbp)
	jne	.L225
	movl	3744(%rbp), %r8d
	movl	$0, %edx
	movl	$2097151, %ecx
	movq	__imp_OpenThread(%rip), %rax
	call	*%rax
	movq	%rax, 3752(%rbp)
.L225:
	movq	my_trace(%rip), %rax
	movl	3744(%rbp), %edx
	addq	$56, %rdx
	movl	(%rax,%rdx,4), %eax
	cmpl	$-1, %eax
	jne	.L226
	movq	my_trace(%rip), %rax
	movq	68981560(%rax), %rax
	movl	%eax, 3708(%rbp)
	movq	my_trace(%rip), %rax
	movl	3708(%rbp), %edx
	addq	$2097159, %rdx
	salq	$5, %rdx
	addq	%rdx, %rax
	movb	$1, 4(%rax)
	movq	my_trace(%rip), %rax
	movl	3708(%rbp), %edx
	addq	$2097159, %rdx
	salq	$5, %rdx
	addq	%rax, %rdx
	movq	3752(%rbp), %rax
	movq	%rax, 8(%rdx)
	movq	my_trace(%rip), %rax
	movl	3708(%rbp), %edx
	addq	$2097159, %rdx
	salq	$5, %rdx
	addq	%rdx, %rax
	movb	$1, 5(%rax)
	movq	my_trace(%rip), %rax
	movl	3708(%rbp), %edx
	addq	$2097159, %rdx
	salq	$5, %rdx
	addq	%rdx, %rax
	movb	$1, 6(%rax)
	movq	my_trace(%rip), %rax
	movl	3708(%rbp), %edx
	addq	$2097159, %rdx
	salq	$5, %rdx
	addq	%rax, %rdx
	movl	3744(%rbp), %eax
	movl	%eax, (%rdx)
	movq	my_trace(%rip), %rax
	movl	3708(%rbp), %edx
	salq	$5, %rdx
	addq	%rdx, %rax
	addq	$67109104, %rax
	movq	$0, 8(%rax)
	movq	3752(%rbp), %rax
	movq	%rax, %r8
	movl	3744(%rbp), %edx
	leaq	.LC110(%rip), %rcx
	call	_Z7d_printPKcz
	movq	my_trace(%rip), %rax
	movl	3744(%rbp), %edx
	leaq	56(%rdx), %rcx
	movl	3708(%rbp), %edx
	movl	%edx, (%rax,%rcx,4)
	movq	my_trace(%rip), %rax
	movq	68981560(%rax), %rdx
	addq	$1, %rdx
	movq	%rdx, 68981560(%rax)
	movq	my_trace(%rip), %rax
	movq	68981560(%rax), %rdx
	leaq	1696(%rbp), %rax
	movq	%rdx, %r8
	leaq	.LC111(%rip), %rdx
	movq	%rax, %rcx
	call	sprintf
	leaq	1696(%rbp), %rax
	movq	%rax, %rcx
	call	_Z13add_to_bufferPc
	jmp	.L227
.L226:
	movq	my_trace(%rip), %rax
	movl	3744(%rbp), %edx
	addq	$56, %rdx
	movl	(%rax,%rdx,4), %eax
	movl	%eax, 3708(%rbp)
	movq	3752(%rbp), %rax
	movq	%rax, %r8
	movl	3744(%rbp), %edx
	leaq	.LC112(%rip), %rcx
	call	_Z7d_printPKcz
	movq	my_trace(%rip), %rax
	movl	3708(%rbp), %edx
	addq	$2097159, %rdx
	salq	$5, %rdx
	addq	%rdx, %rax
	movb	$1, 4(%rax)
	movq	my_trace(%rip), %rax
	movl	3708(%rbp), %edx
	addq	$2097159, %rdx
	salq	$5, %rdx
	addq	%rax, %rdx
	movq	3752(%rbp), %rax
	movq	%rax, 8(%rdx)
.L227:
	movl	$1048587, 2512(%rbp)
	movq	my_trace(%rip), %rax
	movl	3708(%rbp), %edx
	addq	$2097159, %rdx
	salq	$5, %rdx
	addq	%rdx, %rax
	movq	8(%rax), %rax
	leaq	2464(%rbp), %rdx
	movq	%rax, %rcx
	movq	__imp_GetThreadContext(%rip), %rax
	call	*%rax
	leaq	-96(%rbp), %rax
	leaq	2464(%rbp), %rdx
	movl	$154, %ecx
	movq	%rax, %rdi
	movq	%rdx, %rsi
	rep movsq
	leaq	1136(%rbp), %rcx
	leaq	-96(%rbp), %rdx
	movq	3752(%rbp), %rax
	movq	%rcx, %r8
	movq	%rax, %rcx
	call	_Z18getSelectorEntriesPv8_CONTEXTP10_LDT_ENTRY
	leaq	-96(%rbp), %rax
	leaq	2464(%rbp), %rdx
	movl	$154, %ecx
	movq	%rax, %rdi
	movq	%rdx, %rsi
	rep movsq
	leaq	1184(%rbp), %rcx
	leaq	1136(%rbp), %rdx
	leaq	-96(%rbp), %rax
	movq	%rcx, %r8
	movq	%rax, %rcx
	call	_Z17serialize_context8_CONTEXTP10_LDT_ENTRYPc
	leaq	1184(%rbp), %rdx
	leaq	1696(%rbp), %rax
	movq	%rdx, %r9
	movl	3744(%rbp), %r8d
	leaq	.LC113(%rip), %rdx
	movq	%rax, %rcx
	call	sprintf
	leaq	1696(%rbp), %rax
	movq	%rax, %rcx
	call	_Z13add_to_bufferPc
	nop
	addq	$3840, %rsp
	popq	%rsi
	popq	%rdi
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC114:
	.ascii "CT,0x%08x,%s\12\0"
	.text
	.globl	_Z21register_thread_debugmPv
	.def	_Z21register_thread_debugmPv;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z21register_thread_debugmPv
_Z21register_thread_debugmPv:
.LFB2476:
	pushq	%rbp
	.seh_pushreg	%rbp
	pushq	%rdi
	.seh_pushreg	%rdi
	pushq	%rsi
	.seh_pushreg	%rsi
	subq	$3840, %rsp
	.seh_stackalloc	3840
	leaq	128(%rsp), %rbp
	.seh_setframe	%rbp, 128
	.seh_endprologue
	movl	%ecx, 3744(%rbp)
	movq	%rdx, 3752(%rbp)
	movl	$0, 3708(%rbp)
	cmpq	$0, 3752(%rbp)
	jne	.L230
	movl	3744(%rbp), %r8d
	movl	$0, %edx
	movl	$2097151, %ecx
	movq	__imp_OpenThread(%rip), %rax
	call	*%rax
	movq	%rax, 3752(%rbp)
.L230:
	movl	$1048587, 2512(%rbp)
	movq	my_trace(%rip), %rax
	movl	3744(%rbp), %edx
	addq	$56, %rdx
	movl	(%rax,%rdx,4), %eax
	movl	%eax, 3704(%rbp)
	movq	my_trace(%rip), %rax
	movl	3704(%rbp), %edx
	addq	$2097159, %rdx
	salq	$5, %rdx
	addq	%rdx, %rax
	movq	8(%rax), %rax
	leaq	2464(%rbp), %rdx
	movq	%rax, %rcx
	movq	__imp_GetThreadContext(%rip), %rax
	call	*%rax
	leaq	-96(%rbp), %rax
	leaq	2464(%rbp), %rdx
	movl	$154, %ecx
	movq	%rax, %rdi
	movq	%rdx, %rsi
	rep movsq
	leaq	1136(%rbp), %rcx
	leaq	-96(%rbp), %rdx
	movq	3752(%rbp), %rax
	movq	%rcx, %r8
	movq	%rax, %rcx
	call	_Z18getSelectorEntriesPv8_CONTEXTP10_LDT_ENTRY
	leaq	-96(%rbp), %rax
	leaq	2464(%rbp), %rdx
	movl	$154, %ecx
	movq	%rax, %rdi
	movq	%rdx, %rsi
	rep movsq
	leaq	1184(%rbp), %rcx
	leaq	1136(%rbp), %rdx
	leaq	-96(%rbp), %rax
	movq	%rcx, %r8
	movq	%rax, %rcx
	call	_Z17serialize_context8_CONTEXTP10_LDT_ENTRYPc
	leaq	1184(%rbp), %rdx
	leaq	1696(%rbp), %rax
	movq	%rdx, %r9
	movl	3744(%rbp), %r8d
	leaq	.LC114(%rip), %rdx
	movq	%rax, %rcx
	call	sprintf
	leaq	1696(%rbp), %rax
	movq	%rax, %rcx
	call	_Z13add_to_bufferPc
	movl	%eax, 3708(%rbp)
	movl	3708(%rbp), %eax
	addq	$3840, %rsp
	popq	%rsi
	popq	%rdi
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC115:
	.ascii "DT,0x%08x,%s\12\0"
	.text
	.globl	_Z17deregister_threadmPv
	.def	_Z17deregister_threadmPv;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z17deregister_threadmPv
_Z17deregister_threadmPv:
.LFB2477:
	pushq	%rbp
	.seh_pushreg	%rbp
	pushq	%rdi
	.seh_pushreg	%rdi
	pushq	%rsi
	.seh_pushreg	%rsi
	subq	$3840, %rsp
	.seh_stackalloc	3840
	leaq	128(%rsp), %rbp
	.seh_setframe	%rbp, 128
	.seh_endprologue
	movl	%ecx, 3744(%rbp)
	movq	%rdx, 3752(%rbp)
	cmpq	$0, 3752(%rbp)
	jne	.L233
	movl	3744(%rbp), %r8d
	movl	$0, %edx
	movl	$2097151, %ecx
	movq	__imp_OpenThread(%rip), %rax
	call	*%rax
	movq	%rax, 3752(%rbp)
.L233:
	movq	my_trace(%rip), %rax
	movl	3744(%rbp), %edx
	addq	$56, %rdx
	movl	(%rax,%rdx,4), %eax
	movl	%eax, 3708(%rbp)
	movq	my_trace(%rip), %rax
	movl	3708(%rbp), %edx
	addq	$2097159, %rdx
	salq	$5, %rdx
	addq	%rdx, %rax
	movb	$0, 4(%rax)
	movq	my_trace(%rip), %rax
	movl	3708(%rbp), %edx
	addq	$2097159, %rdx
	salq	$5, %rdx
	addq	%rax, %rdx
	movq	3752(%rbp), %rax
	movq	%rax, 8(%rdx)
	movq	my_trace(%rip), %rax
	movl	3708(%rbp), %edx
	addq	$2097159, %rdx
	salq	$5, %rdx
	addq	%rdx, %rax
	movb	$1, 5(%rax)
	movl	$1048587, 2512(%rbp)
	movq	my_trace(%rip), %rax
	movl	3708(%rbp), %edx
	addq	$2097159, %rdx
	salq	$5, %rdx
	addq	%rdx, %rax
	movq	8(%rax), %rax
	leaq	2464(%rbp), %rdx
	movq	%rax, %rcx
	movq	__imp_GetThreadContext(%rip), %rax
	call	*%rax
	leaq	-96(%rbp), %rax
	leaq	2464(%rbp), %rdx
	movl	$154, %ecx
	movq	%rax, %rdi
	movq	%rdx, %rsi
	rep movsq
	leaq	1136(%rbp), %rcx
	leaq	-96(%rbp), %rdx
	movq	3752(%rbp), %rax
	movq	%rcx, %r8
	movq	%rax, %rcx
	call	_Z18getSelectorEntriesPv8_CONTEXTP10_LDT_ENTRY
	leaq	-96(%rbp), %rax
	leaq	2464(%rbp), %rdx
	movl	$154, %ecx
	movq	%rax, %rdi
	movq	%rdx, %rsi
	rep movsq
	leaq	1184(%rbp), %rcx
	leaq	1136(%rbp), %rdx
	leaq	-96(%rbp), %rax
	movq	%rcx, %r8
	movq	%rax, %rcx
	call	_Z17serialize_context8_CONTEXTP10_LDT_ENTRYPc
	leaq	1184(%rbp), %rdx
	leaq	1696(%rbp), %rax
	movq	%rdx, %r9
	movl	3744(%rbp), %r8d
	leaq	.LC115(%rip), %rdx
	movq	%rax, %rcx
	call	sprintf
	leaq	1696(%rbp), %rax
	movq	%rax, %rcx
	call	_Z13add_to_bufferPc
	nop
	addq	$3840, %rsp
	popq	%rsi
	popq	%rdi
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC116:
	.ascii "DT,0x%08x\12\0"
	.text
	.globl	_Z18deregister_thread2m
	.def	_Z18deregister_thread2m;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z18deregister_thread2m
_Z18deregister_thread2m:
.LFB2478:
	pushq	%rbp
	.seh_pushreg	%rbp
	subq	$816, %rsp
	.seh_stackalloc	816
	leaq	128(%rsp), %rbp
	.seh_setframe	%rbp, 128
	.seh_endprologue
	movl	%ecx, 704(%rbp)
	movq	my_trace(%rip), %rax
	movl	704(%rbp), %edx
	addq	$56, %rdx
	movl	(%rax,%rdx,4), %eax
	movl	%eax, 684(%rbp)
	movq	my_trace(%rip), %rax
	movl	684(%rbp), %edx
	addq	$2097159, %rdx
	salq	$5, %rdx
	addq	%rdx, %rax
	movq	8(%rax), %rax
	testq	%rax, %rax
	je	.L236
	movq	my_trace(%rip), %rax
	movl	684(%rbp), %edx
	addq	$2097159, %rdx
	salq	$5, %rdx
	addq	%rdx, %rax
	movq	8(%rax), %rax
	movq	%rax, %rcx
	movq	__imp_CloseHandle(%rip), %rax
	call	*%rax
.L236:
	movq	my_trace(%rip), %rax
	movl	684(%rbp), %edx
	addq	$2097159, %rdx
	salq	$5, %rdx
	addq	%rdx, %rax
	movq	$0, 8(%rax)
	movq	my_trace(%rip), %rax
	movl	684(%rbp), %edx
	addq	$2097159, %rdx
	salq	$5, %rdx
	addq	%rdx, %rax
	movb	$0, 5(%rax)
	leaq	-96(%rbp), %rax
	movl	704(%rbp), %r8d
	leaq	.LC116(%rip), %rdx
	movq	%rax, %rcx
	call	sprintf
	leaq	-96(%rbp), %rax
	movq	%rax, %rcx
	call	_Z13add_to_bufferPc
	nop
	addq	$816, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.globl	_Z9find_filePc
	.def	_Z9find_filePc;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z9find_filePc
_Z9find_filePc:
.LFB2479:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$64, %rsp
	.seh_stackalloc	64
	.seh_endprologue
	movq	%rcx, 16(%rbp)
	movq	16(%rbp), %rcx
	call	strlen
	movl	%eax, -16(%rbp)
	movq	16(%rbp), %rax
	movq	%rax, -24(%rbp)
	movl	$0, -12(%rbp)
	jmp	.L239
.L241:
	movl	-12(%rbp), %edx
	movq	16(%rbp), %rax
	addq	%rdx, %rax
	movzbl	(%rax), %eax
	cmpb	$92, %al
	jne	.L240
	movl	-12(%rbp), %edx
	movq	16(%rbp), %rax
	addq	%rdx, %rax
	movq	%rax, -8(%rbp)
.L240:
	addl	$1, -12(%rbp)
.L239:
	movl	-12(%rbp), %eax
	cmpl	-16(%rbp), %eax
	jb	.L241
	movq	-8(%rbp), %rax
	addq	$1, %rax
	addq	$64, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC117:
	.ascii "self\0"
.LC118:
	.ascii "self is always loaded dummy\12\0"
.LC119:
	.ascii "Lib %s is loaded\12\0"
	.text
	.globl	_Z16check_lib_loadedPc
	.def	_Z16check_lib_loadedPc;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z16check_lib_loadedPc
_Z16check_lib_loadedPc:
.LFB2480:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$48, %rsp
	.seh_stackalloc	48
	.seh_endprologue
	movq	%rcx, 16(%rbp)
	leaq	.LC117(%rip), %rdx
	movq	16(%rbp), %rcx
	call	strcmpi
	testl	%eax, %eax
	sete	%al
	testb	%al, %al
	je	.L244
	leaq	.LC118(%rip), %rcx
	call	_Z7d_printPKcz
	movl	$1, %eax
	jmp	.L245
.L244:
	movl	$0, -4(%rbp)
	jmp	.L246
.L248:
	movq	my_trace(%rip), %rdx
	movl	-4(%rbp), %eax
	salq	$4, %rax
	movq	%rax, %rcx
	salq	$6, %rcx
	addq	%rcx, %rax
	addq	$67117296, %rax
	addq	%rdx, %rax
	movq	16(%rbp), %rdx
	movq	%rax, %rcx
	call	strcmpi
	testl	%eax, %eax
	sete	%al
	testb	%al, %al
	je	.L247
	movq	my_trace(%rip), %rdx
	movl	-4(%rbp), %eax
	salq	$4, %rax
	movq	%rax, %rcx
	salq	$6, %rcx
	addq	%rcx, %rax
	addq	%rdx, %rax
	addq	$67117280, %rax
	movzbl	(%rax), %eax
	testb	%al, %al
	je	.L247
	movq	16(%rbp), %rdx
	leaq	.LC119(%rip), %rcx
	call	_Z7d_printPKcz
	movl	-4(%rbp), %eax
	jmp	.L245
.L247:
	addl	$1, -4(%rbp)
.L246:
	movl	-4(%rbp), %edx
	movq	my_trace(%rip), %rax
	movq	68981568(%rax), %rax
	cmpq	%rax, %rdx
	jb	.L248
	movl	$-1, %eax
.L245:
	addq	$48, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC120:
	.ascii "RL,0x%08x,%s\12\0"
	.text
	.globl	_Z12register_lib20_LOAD_DLL_DEBUG_INFO
	.def	_Z12register_lib20_LOAD_DLL_DEBUG_INFO;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z12register_lib20_LOAD_DLL_DEBUG_INFO
_Z12register_lib20_LOAD_DLL_DEBUG_INFO:
.LFB2481:
	pushq	%rbp
	.seh_pushreg	%rbp
	pushq	%rbx
	.seh_pushreg	%rbx
	subq	$824, %rsp
	.seh_stackalloc	824
	leaq	128(%rsp), %rbp
	.seh_setframe	%rbp, 128
	.seh_endprologue
	movq	%rcx, %rbx
	movl	$0, 684(%rbp)
	movq	my_trace(%rip), %rdx
	movq	my_trace(%rip), %rax
	movq	68981568(%rax), %rax
	salq	$4, %rax
	movq	%rax, %rcx
	salq	$6, %rcx
	addq	%rcx, %rax
	addq	$67117808, %rax
	addq	%rax, %rdx
	movq	my_trace(%rip), %rax
	movq	68981640(%rax), %rax
	movl	$4, %r9d
	movl	$768, %r8d
	movq	%rax, %rcx
	movq	__imp_GetFinalPathNameByHandleA(%rip), %rax
	call	*%rax
	movq	my_trace(%rip), %rdx
	movq	my_trace(%rip), %rax
	movq	68981568(%rax), %rax
	salq	$4, %rax
	movq	%rax, %rcx
	salq	$6, %rcx
	addq	%rcx, %rax
	addq	$67117808, %rax
	addq	%rdx, %rax
	movq	%rax, %rcx
	call	_Z9find_filePc
	movq	%rax, %r8
	movq	my_trace(%rip), %rdx
	movq	my_trace(%rip), %rax
	movq	68981568(%rax), %rax
	salq	$4, %rax
	movq	%rax, %rcx
	salq	$6, %rcx
	addq	%rcx, %rax
	addq	$67117296, %rax
	addq	%rdx, %rax
	movq	%r8, %rdx
	movq	%rax, %rcx
	call	strcpy
	movq	my_trace(%rip), %rdx
	movq	my_trace(%rip), %rax
	movq	68981568(%rax), %rax
	movq	8(%rbx), %rcx
	movq	%rcx, %r8
	salq	$4, %rax
	movq	%rax, %rcx
	salq	$6, %rcx
	addq	%rcx, %rax
	addq	%rdx, %rax
	addq	$67117280, %rax
	movq	%r8, 8(%rax)
	movq	my_trace(%rip), %rdx
	movq	my_trace(%rip), %rax
	movq	68981568(%rax), %rax
	salq	$4, %rax
	movq	%rax, %rcx
	salq	$6, %rcx
	addq	%rcx, %rax
	addq	$67117296, %rax
	leaq	(%rdx,%rax), %rcx
	movq	my_trace(%rip), %rdx
	movq	my_trace(%rip), %rax
	movq	68981568(%rax), %rax
	salq	$4, %rax
	movq	%rax, %r8
	salq	$6, %r8
	addq	%r8, %rax
	addq	%rdx, %rax
	addq	$67117280, %rax
	movq	8(%rax), %rax
	movq	%rcx, %r8
	movq	%rax, %rdx
	leaq	.LC120(%rip), %rcx
	call	_Z7d_printPKcz
	movq	my_trace(%rip), %rdx
	movq	my_trace(%rip), %rax
	movq	68981568(%rax), %rax
	salq	$4, %rax
	movq	%rax, %rcx
	salq	$6, %rcx
	addq	%rcx, %rax
	addq	$67117296, %rax
	leaq	(%rdx,%rax), %rcx
	movq	my_trace(%rip), %rdx
	movq	my_trace(%rip), %rax
	movq	68981568(%rax), %rax
	salq	$4, %rax
	movq	%rax, %r8
	salq	$6, %r8
	addq	%r8, %rax
	addq	%rdx, %rax
	addq	$67117280, %rax
	movq	8(%rax), %rdx
	leaq	-96(%rbp), %rax
	movq	%rcx, %r9
	movq	%rdx, %r8
	leaq	.LC120(%rip), %rdx
	movq	%rax, %rcx
	call	sprintf
	movq	my_trace(%rip), %rdx
	movq	my_trace(%rip), %rax
	movq	68981568(%rax), %rax
	salq	$4, %rax
	movq	%rax, %rcx
	salq	$6, %rcx
	addq	%rcx, %rax
	addq	%rdx, %rax
	addq	$67117280, %rax
	movb	$1, (%rax)
	movq	my_trace(%rip), %rax
	movq	68981568(%rax), %rdx
	addq	$1, %rdx
	movq	%rdx, 68981568(%rax)
	leaq	-96(%rbp), %rax
	movq	%rax, %rcx
	call	_Z13add_to_bufferPc
	nop
	addq	$824, %rsp
	popq	%rbx
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC121:
	.ascii "DL,0x%08x,%s\12\0"
	.text
	.globl	_Z14deregister_libm
	.def	_Z14deregister_libm;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z14deregister_libm
_Z14deregister_libm:
.LFB2482:
	pushq	%rbp
	.seh_pushreg	%rbp
	subq	$800, %rsp
	.seh_stackalloc	800
	leaq	128(%rsp), %rbp
	.seh_setframe	%rbp, 128
	.seh_endprologue
	movl	%ecx, 688(%rbp)
	movq	my_trace(%rip), %rdx
	movl	688(%rbp), %eax
	salq	$4, %rax
	movq	%rax, %rcx
	salq	$6, %rcx
	addq	%rcx, %rax
	addq	$67117296, %rax
	leaq	(%rdx,%rax), %rcx
	movq	my_trace(%rip), %rdx
	movl	688(%rbp), %eax
	salq	$4, %rax
	movq	%rax, %r8
	salq	$6, %r8
	addq	%r8, %rax
	addq	%rdx, %rax
	addq	$67117280, %rax
	movq	8(%rax), %rdx
	leaq	-96(%rbp), %rax
	movq	%rcx, %r9
	movq	%rdx, %r8
	leaq	.LC121(%rip), %rdx
	movq	%rax, %rcx
	call	sprintf
	movq	my_trace(%rip), %rdx
	movl	688(%rbp), %eax
	salq	$4, %rax
	movq	%rax, %rcx
	salq	$6, %rcx
	addq	%rcx, %rax
	addq	%rdx, %rax
	addq	$67117280, %rax
	movb	$0, (%rax)
	leaq	-96(%rbp), %rax
	movq	%rax, %rcx
	call	_Z13add_to_bufferPc
	nop
	addq	$800, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.globl	_Z14deregister_lib22_UNLOAD_DLL_DEBUG_INFO
	.def	_Z14deregister_lib22_UNLOAD_DLL_DEBUG_INFO;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z14deregister_lib22_UNLOAD_DLL_DEBUG_INFO
_Z14deregister_lib22_UNLOAD_DLL_DEBUG_INFO:
.LFB2483:
	pushq	%rbp
	.seh_pushreg	%rbp
	subq	$816, %rsp
	.seh_stackalloc	816
	leaq	128(%rsp), %rbp
	.seh_setframe	%rbp, 128
	.seh_endprologue
	movq	%rcx, 704(%rbp)
	movl	$0, 684(%rbp)
	jmp	.L253
.L256:
	movq	my_trace(%rip), %rdx
	movl	684(%rbp), %eax
	salq	$4, %rax
	movq	%rax, %rcx
	salq	$6, %rcx
	addq	%rcx, %rax
	addq	%rdx, %rax
	addq	$67117280, %rax
	movq	8(%rax), %rax
	movq	704(%rbp), %rdx
	cmpq	%rdx, %rax
	jne	.L254
	jmp	.L255
.L254:
	addl	$1, 684(%rbp)
.L253:
	movl	684(%rbp), %edx
	movq	my_trace(%rip), %rax
	movq	68981568(%rax), %rax
	cmpq	%rax, %rdx
	jb	.L256
.L255:
	movq	my_trace(%rip), %rdx
	movl	684(%rbp), %eax
	salq	$4, %rax
	movq	%rax, %rcx
	salq	$6, %rcx
	addq	%rcx, %rax
	addq	$67117296, %rax
	leaq	(%rdx,%rax), %rcx
	movq	my_trace(%rip), %rdx
	movl	684(%rbp), %eax
	salq	$4, %rax
	movq	%rax, %r8
	salq	$6, %r8
	addq	%r8, %rax
	addq	%rdx, %rax
	addq	$67117280, %rax
	movq	8(%rax), %rdx
	leaq	-96(%rbp), %rax
	movq	%rcx, %r9
	movq	%rdx, %r8
	leaq	.LC121(%rip), %rdx
	movq	%rax, %rcx
	call	sprintf
	movq	my_trace(%rip), %rdx
	movl	684(%rbp), %eax
	salq	$4, %rax
	movq	%rax, %rcx
	salq	$6, %rcx
	addq	%rcx, %rax
	addq	%rdx, %rax
	addq	$67117280, %rax
	movb	$0, (%rax)
	leaq	-96(%rbp), %rax
	movq	%rax, %rcx
	call	_Z13add_to_bufferPc
	nop
	addq	$816, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC122:
	.ascii "sysenter\12\0"
.LC123:
	.ascii "ESP: %p\12\0"
	.align 8
.LC124:
	.ascii "# Syscall in TID: 0x%08x no: 0x%08x, stack@ 0x%08x\12\0"
	.text
	.globl	_Z23react_sysenter_callbackPv
	.def	_Z23react_sysenter_callbackPv;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z23react_sysenter_callbackPv
_Z23react_sysenter_callbackPv:
.LFB2484:
	pushq	%rbp
	.seh_pushreg	%rbp
	subq	$2064, %rsp
	.seh_stackalloc	2064
	leaq	128(%rsp), %rbp
	.seh_setframe	%rbp, 128
	.seh_endprologue
	movq	%rcx, 1952(%rbp)
	leaq	.LC122(%rip), %rcx
	call	_Z7d_printPKcz
	movq	1952(%rbp), %rax
	movq	%rax, 1928(%rbp)
	movq	1928(%rbp), %rax
	movl	8(%rax), %eax
	movl	%eax, 1924(%rbp)
	movq	my_trace(%rip), %rax
	movl	1924(%rbp), %edx
	addq	$56, %rdx
	movl	(%rax,%rdx,4), %eax
	movl	%eax, 1920(%rbp)
	movl	$1048587, 736(%rbp)
	movq	my_trace(%rip), %rax
	movl	1920(%rbp), %edx
	addq	$2097159, %rdx
	salq	$5, %rdx
	addq	%rdx, %rax
	movq	8(%rax), %rax
	leaq	688(%rbp), %rdx
	movq	%rax, %rcx
	movq	__imp_GetThreadContext(%rip), %rax
	call	*%rax
	testl	%eax, %eax
	sete	%al
	testb	%al, %al
	je	.L259
	movq	__imp_GetLastError(%rip), %rax
	call	*%rax
	movl	%eax, %edx
	leaq	.LC7(%rip), %rcx
	call	_Z7d_printPKcz
	jmp	.L258
.L259:
	movq	840(%rbp), %rax
	movq	%rax, %rdx
	leaq	.LC123(%rip), %rcx
	call	_Z7d_printPKcz
	movq	808(%rbp), %rax
	movq	%rax, sysenter_no(%rip)
	movq	840(%rbp), %rax
	movq	%rax, sysenter_esp(%rip)
	movq	sysenter_esp(%rip), %rdx
	movq	sysenter_no(%rip), %r8
	movl	1924(%rbp), %ecx
	leaq	-80(%rbp), %rax
	movq	%rdx, 32(%rsp)
	movq	%r8, %r9
	movl	%ecx, %r8d
	leaq	.LC124(%rip), %rdx
	movq	%rax, %rcx
	call	sprintf
	leaq	-80(%rbp), %rax
	movq	%rax, %rcx
	call	_Z13add_to_bufferPc
	movq	my_trace(%rip), %rax
	movl	1920(%rbp), %edx
	addq	$2097159, %rdx
	salq	$5, %rdx
	addq	%rdx, %rax
	movq	8(%rax), %rdx
	movl	1924(%rbp), %eax
	movl	%eax, %ecx
	call	_Z17deregister_threadmPv
	movl	$0, %ecx
	call	_Z6set_ssm
	nop
.L258:
	addq	$2064, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC125:
	.ascii "sysret\12\0"
.LC126:
	.ascii "EAX: 0x%08x\12\0"
.LC127:
	.ascii "[[Syscall: 0x%08x @ 0x%08x]]\12\0"
	.align 8
.LC128:
	.ascii "Hunting for reads from: 0x%08x\12\0"
.LC129:
	.ascii "RN,0x%08x,0x%08x\12\0"
.LC130:
	.ascii "Marked taint: 0x%08x,0x%08x\12\0"
	.text
	.globl	_Z21react_sysret_callbackPv
	.def	_Z21react_sysret_callbackPv;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z21react_sysret_callbackPv
_Z21react_sysret_callbackPv:
.LFB2485:
	pushq	%rbp
	.seh_pushreg	%rbp
	subq	$2128, %rsp
	.seh_stackalloc	2128
	leaq	128(%rsp), %rbp
	.seh_setframe	%rbp, 128
	.seh_endprologue
	movq	%rcx, 2016(%rbp)
	leaq	.LC125(%rip), %rcx
	call	_Z7d_printPKcz
	movq	2016(%rbp), %rax
	movq	%rax, 1984(%rbp)
	movq	1984(%rbp), %rax
	movl	8(%rax), %eax
	movl	%eax, 1980(%rbp)
	movq	my_trace(%rip), %rax
	movl	1980(%rbp), %edx
	addq	$56, %rdx
	movl	(%rax,%rdx,4), %eax
	movl	%eax, 1976(%rbp)
	movq	my_trace(%rip), %rax
	movl	1976(%rbp), %edx
	addq	$2097159, %rdx
	salq	$5, %rdx
	addq	%rdx, %rax
	movq	8(%rax), %rdx
	movl	1980(%rbp), %eax
	movl	%eax, %ecx
	call	_Z15register_threadmPv
	movl	$1048587, 736(%rbp)
	movq	my_trace(%rip), %rax
	movl	1976(%rbp), %edx
	addq	$2097159, %rdx
	salq	$5, %rdx
	addq	%rdx, %rax
	movq	8(%rax), %rax
	leaq	688(%rbp), %rdx
	movq	%rax, %rcx
	movq	__imp_GetThreadContext(%rip), %rax
	call	*%rax
	testl	%eax, %eax
	sete	%al
	testb	%al, %al
	je	.L262
	movq	__imp_GetLastError(%rip), %rax
	call	*%rax
	movl	%eax, %edx
	leaq	.LC7(%rip), %rcx
	call	_Z7d_printPKcz
	jmp	.L261
.L262:
	movq	808(%rbp), %rax
	movq	%rax, %rdx
	leaq	.LC126(%rip), %rcx
	call	_Z7d_printPKcz
	movq	936(%rbp), %rdx
	movq	sysenter_no(%rip), %rax
	movq	%rdx, %r8
	movq	%rax, %rdx
	leaq	.LC127(%rip), %rcx
	call	_Z7d_printPKcz
	movl	$0, 1996(%rbp)
	jmp	.L264
.L269:
	movq	my_trace(%rip), %r8
	movq	sysenter_no(%rip), %rcx
	movl	1996(%rbp), %eax
	movq	%rax, %rdx
	addq	%rdx, %rdx
	addq	%rax, %rdx
	leaq	0(,%rdx,8), %rax
	movq	%rax, %rdx
	movq	%rcx, %rax
	addq	%rax, %rax
	addq	%rcx, %rax
	salq	$7, %rax
	addq	%rdx, %rax
	addq	%r8, %rax
	addq	$70047184, %rax
	movzbl	(%rax), %edx
	movzbl	16+last_arg(%rip), %eax
	cmpb	%al, %dl
	je	.L266
	movq	my_trace(%rip), %r8
	movq	sysenter_no(%rip), %rcx
	movl	1996(%rbp), %eax
	movq	%rax, %rdx
	addq	%rdx, %rdx
	addq	%rax, %rdx
	leaq	0(,%rdx,8), %rax
	movq	%rax, %rdx
	movq	%rcx, %rax
	addq	%rax, %rax
	addq	%rcx, %rax
	salq	$7, %rax
	addq	%rdx, %rax
	addq	%r8, %rax
	addq	$70047184, %rax
	movl	4(%rax), %eax
	cmpl	$-16711936, %eax
	je	.L267
	movq	my_trace(%rip), %r8
	movq	sysenter_no(%rip), %rcx
	movl	1996(%rbp), %eax
	movq	%rax, %rdx
	addq	%rdx, %rdx
	addq	%rax, %rdx
	leaq	0(,%rdx,8), %rax
	movq	%rax, %rdx
	movq	%rcx, %rax
	addq	%rax, %rax
	addq	%rcx, %rax
	salq	$7, %rax
	addq	%rdx, %rax
	addq	%r8, %rax
	addq	$70047184, %rax
	movl	4(%rax), %eax
	movl	%eax, %edx
	movq	808(%rbp), %rax
	cmpq	%rax, %rdx
	jne	.L268
.L267:
	movq	my_trace(%rip), %r8
	movq	sysenter_no(%rip), %rcx
	movl	1996(%rbp), %eax
	movq	%rax, %rdx
	addq	%rdx, %rdx
	addq	%rax, %rdx
	leaq	0(,%rdx,8), %rax
	movq	%rax, %rdx
	movq	%rcx, %rax
	addq	%rax, %rax
	addq	%rcx, %rax
	salq	$7, %rax
	addq	%rdx, %rax
	addq	$70047168, %rax
	leaq	(%r8,%rax), %rcx
	leaq	1920(%rbp), %rax
	movq	%rax, %rdx
	call	_Z14resolve_regionP20LOCATION_DESCRIPTOR_P9LOCATION_
	leaq	1920(%rbp), %rax
	movq	%rax, %rcx
	call	_Z17update_region_oldP9LOCATION_
.L268:
	addl	$1, 1996(%rbp)
.L264:
	cmpl	$15, 1996(%rbp)
	jbe	.L269
.L266:
	movq	sysenter_no(%rip), %rax
	cmpq	$66, %rax
	jne	.L270
	movq	file_handle(%rip), %rax
	testq	%rax, %rax
	jne	.L270
	movq	sysenter_esp(%rip), %rax
	addl	$8, %eax
	movl	%eax, 1972(%rbp)
	movq	my_trace(%rip), %r8
	movq	sysenter_no(%rip), %rcx
	movl	1996(%rbp), %eax
	movq	%rax, %rdx
	addq	%rdx, %rdx
	addq	%rax, %rdx
	leaq	0(,%rdx,8), %rax
	movq	%rax, %rdx
	movq	%rcx, %rax
	addq	%rax, %rax
	addq	%rcx, %rax
	salq	$7, %rax
	addq	%rdx, %rax
	addq	%r8, %rax
	addq	$70047168, %rax
	movq	8(%rax), %rax
	sall	$2, %eax
	addl	%eax, 1972(%rbp)
	movl	1972(%rbp), %eax
	movq	%rax, %r10
	movq	my_trace(%rip), %rax
	movq	136(%rax), %rax
	leaq	1948(%rbp), %rcx
	leaq	1936(%rbp), %rdx
	movq	%rdx, 32(%rsp)
	movl	$4, %r9d
	movq	%rcx, %r8
	movq	%r10, %rdx
	movq	%rax, %rcx
	call	_Z11read_memoryPvS_S_yPy
	movl	1948(%rbp), %eax
	movl	%eax, 1972(%rbp)
	movl	1972(%rbp), %eax
	movq	%rax, %rcx
	movq	my_trace(%rip), %rax
	movq	136(%rax), %rax
	leaq	1936(%rbp), %rdx
	movq	%rdx, 32(%rsp)
	movl	$4, %r9d
	leaq	file_handle(%rip), %r8
	movq	%rcx, %rdx
	movq	%rax, %rcx
	call	_Z11read_memoryPvS_S_yPy
	movq	file_handle(%rip), %rax
	movq	%rax, %rdx
	leaq	.LC128(%rip), %rcx
	call	_Z7d_printPKcz
.L270:
	movq	sysenter_no(%rip), %rax
	cmpq	$273, %rax
	jne	.L271
	movq	my_trace(%rip), %rax
	movq	95212992(%rax), %rax
	movq	%rax, %rdx
	movq	file_handle(%rip), %rax
	cmpq	%rax, %rdx
	jne	.L271
	movq	my_trace(%rip), %rax
	movq	95213048(%rax), %rcx
	movq	my_trace(%rip), %rax
	movq	95213040(%rax), %rdx
	leaq	-80(%rbp), %rax
	movq	%rcx, %r9
	movq	%rdx, %r8
	leaq	.LC129(%rip), %rdx
	movq	%rax, %rcx
	call	sprintf
	leaq	-80(%rbp), %rax
	movq	%rax, %rcx
	call	_Z13add_to_bufferPc
	movq	1952(%rbp), %rdx
	movq	1960(%rbp), %rax
	movq	%rdx, %r8
	movq	%rax, %rdx
	leaq	.LC130(%rip), %rcx
	call	_Z7d_printPKcz
.L271:
	movl	1980(%rbp), %eax
	movl	%eax, %ecx
	call	_Z6set_ssm
	nop
.L261:
	addq	$2128, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.globl	_Z9get_stackPvj
	.def	_Z9get_stackPvj;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z9get_stackPvj
_Z9get_stackPvj:
.LFB2486:
	pushq	%rbp
	.seh_pushreg	%rbp
	subq	$1312, %rsp
	.seh_stackalloc	1312
	leaq	128(%rsp), %rbp
	.seh_setframe	%rbp, 128
	.seh_endprologue
	movq	%rcx, 1200(%rbp)
	movl	%edx, 1208(%rbp)
	leaq	-80(%rbp), %rax
	movq	%rax, %rdx
	movq	1200(%rbp), %rcx
	movq	__imp_GetThreadContext(%rip), %rax
	call	*%rax
	movl	$0, 1172(%rbp)
	jmp	.L273
.L274:
	addq	$4, 1176(%rbp)
	addl	$1, 1172(%rbp)
.L273:
	movl	1172(%rbp), %eax
	cmpl	1208(%rbp), %eax
	jb	.L274
	movq	1176(%rbp), %rdx
	movq	my_trace(%rip), %rax
	movq	136(%rax), %rax
	leaq	1160(%rbp), %r8
	leaq	1152(%rbp), %rcx
	movq	%rcx, 32(%rsp)
	movl	$4, %r9d
	movq	%rax, %rcx
	movq	__imp_ReadProcessMemory(%rip), %rax
	call	*%rax
	movq	1160(%rbp), %rax
	addq	$1312, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC131:
	.ascii "TS,0x%08x\12\0"
	.text
	.globl	_Z21createthread_callbackPv
	.def	_Z21createthread_callbackPv;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z21createthread_callbackPv
_Z21createthread_callbackPv:
.LFB2487:
	pushq	%rbp
	.seh_pushreg	%rbp
	subq	$832, %rsp
	.seh_stackalloc	832
	leaq	128(%rsp), %rbp
	.seh_setframe	%rbp, 128
	.seh_endprologue
	movq	%rcx, 720(%rbp)
	movq	720(%rbp), %rax
	movq	%rax, 696(%rbp)
	movq	696(%rbp), %rax
	movl	8(%rax), %eax
	movl	%eax, 692(%rbp)
	movl	692(%rbp), %eax
	movl	%eax, %r8d
	movl	$0, %edx
	movl	$2097151, %ecx
	movq	__imp_OpenThread(%rip), %rax
	call	*%rax
	movq	%rax, 680(%rbp)
	movq	680(%rbp), %rax
	movl	$3, %edx
	movq	%rax, %rcx
	call	_Z9get_stackPvj
	movq	%rax, 672(%rbp)
	movq	672(%rbp), %rdx
	leaq	-96(%rbp), %rax
	movq	%rdx, %r8
	leaq	.LC131(%rip), %rdx
	movq	%rax, %rcx
	call	sprintf
	movq	672(%rbp), %rax
	movq	%rax, %rdx
	leaq	.LC131(%rip), %rcx
	call	_Z7d_printPKcz
	leaq	-96(%rbp), %rax
	movq	%rax, %rcx
	call	_Z13add_to_bufferPc
	nop
	addq	$832, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC132:
	.ascii "[antidebug] enter\12\0"
	.text
	.globl	_Z28isdebuggerpresent_callback_1Pv
	.def	_Z28isdebuggerpresent_callback_1Pv;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z28isdebuggerpresent_callback_1Pv
_Z28isdebuggerpresent_callback_1Pv:
.LFB2488:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$32, %rsp
	.seh_stackalloc	32
	.seh_endprologue
	movq	%rcx, 16(%rbp)
	leaq	.LC132(%rip), %rcx
	call	_Z7d_printPKcz
	nop
	addq	$32, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC133:
	.ascii "[antidebug] EAX: 0x%08x\12\0"
	.align 8
.LC134:
	.ascii "[antidebug] IsDebuggerPresent handled\12\0"
.LC135:
	.ascii "[antidebug] exit\12\0"
	.text
	.globl	_Z28isdebuggerpresent_callback_2Pv
	.def	_Z28isdebuggerpresent_callback_2Pv;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z28isdebuggerpresent_callback_2Pv
_Z28isdebuggerpresent_callback_2Pv:
.LFB2489:
	pushq	%rbp
	.seh_pushreg	%rbp
	subq	$1296, %rsp
	.seh_stackalloc	1296
	leaq	128(%rsp), %rbp
	.seh_setframe	%rbp, 128
	.seh_endprologue
	movq	%rcx, 1184(%rbp)
	movq	1184(%rbp), %rax
	movq	%rax, 1160(%rbp)
	movq	1160(%rbp), %rax
	movl	8(%rax), %eax
	movl	%eax, 1156(%rbp)
	movl	$1048587, -48(%rbp)
	movl	1156(%rbp), %eax
	movl	%eax, %r8d
	movl	$0, %edx
	movl	$2097151, %ecx
	movq	__imp_OpenThread(%rip), %rax
	call	*%rax
	movq	%rax, 1144(%rbp)
	leaq	-96(%rbp), %rdx
	movq	1144(%rbp), %rax
	movq	%rax, %rcx
	movq	__imp_GetThreadContext(%rip), %rax
	call	*%rax
	movq	24(%rbp), %rax
	movq	%rax, %rdx
	leaq	.LC133(%rip), %rcx
	call	_Z7d_printPKcz
	movq	$0, 24(%rbp)
	leaq	-96(%rbp), %rdx
	movq	1144(%rbp), %rax
	movq	%rax, %rcx
	movq	__imp_SetThreadContext(%rip), %rax
	call	*%rax
	movq	1144(%rbp), %rax
	movq	%rax, %rcx
	movq	__imp_CloseHandle(%rip), %rax
	call	*%rax
	movl	1156(%rbp), %eax
	movl	%eax, %r8d
	movl	$0, %edx
	movl	$2097151, %ecx
	movq	__imp_OpenThread(%rip), %rax
	call	*%rax
	movq	%rax, 1144(%rbp)
	leaq	-96(%rbp), %rdx
	movq	1144(%rbp), %rax
	movq	%rax, %rcx
	movq	__imp_GetThreadContext(%rip), %rax
	call	*%rax
	movq	1144(%rbp), %rax
	movq	%rax, %rcx
	movq	__imp_CloseHandle(%rip), %rax
	call	*%rax
	leaq	.LC134(%rip), %rcx
	call	_Z7d_printPKcz
	movq	24(%rbp), %rax
	movq	%rax, %rdx
	leaq	.LC133(%rip), %rcx
	call	_Z7d_printPKcz
	leaq	.LC135(%rip), %rcx
	call	_Z7d_printPKcz
	nop
	addq	$1296, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.globl	_Z14empty_callbackPv
	.def	_Z14empty_callbackPv;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z14empty_callbackPv
_Z14empty_callbackPv:
.LFB2490:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	.seh_endprologue
	movq	%rcx, 16(%rbp)
	nop
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC136:
	.ascii "Thread32First\12\0"
	.text
	.globl	_Z20register_all_threadsv
	.def	_Z20register_all_threadsv;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z20register_all_threadsv
_Z20register_all_threadsv:
.LFB2491:
	pushq	%rbp
	.seh_pushreg	%rbp
	pushq	%rsi
	.seh_pushreg	%rsi
	pushq	%rbx
	.seh_pushreg	%rbx
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$80, %rsp
	.seh_stackalloc	80
	.seh_endprologue
	movq	$-1, -8(%rbp)
	movl	$0, %edx
	movl	$4, %ecx
	call	CreateToolhelp32Snapshot
	movq	%rax, -8(%rbp)
	cmpq	$-1, -8(%rbp)
	jne	.L284
	movl	$0, %eax
	jmp	.L288
.L284:
	movl	$28, %eax
	movl	%eax, -48(%rbp)
	leaq	-48(%rbp), %rdx
	movq	-8(%rbp), %rax
	movq	%rax, %rcx
	call	Thread32First
	testl	%eax, %eax
	sete	%al
	testb	%al, %al
	je	.L286
	leaq	.LC136(%rip), %rcx
	call	_Z7d_printPKcz
	movq	-8(%rbp), %rax
	movq	%rax, %rcx
	movq	__imp_CloseHandle(%rip), %rax
	call	*%rax
	movl	$-1, %eax
	jmp	.L288
.L286:
	movl	-36(%rbp), %eax
	movl	myPID(%rip), %edx
	cmpl	%edx, %eax
	jne	.L287
	movq	my_trace(%rip), %rax
	movl	-40(%rbp), %edx
	movl	%edx, %edx
	addq	$2097159, %rdx
	salq	$5, %rdx
	addq	%rdx, %rax
	movb	$1, 4(%rax)
	movq	my_trace(%rip), %rbx
	movl	-40(%rbp), %esi
	movl	-40(%rbp), %eax
	movl	%eax, %r8d
	movl	$0, %edx
	movl	$2097151, %ecx
	movq	__imp_OpenThread(%rip), %rax
	call	*%rax
	movq	%rax, %rdx
	movl	%esi, %eax
	addq	$2097159, %rax
	salq	$5, %rax
	addq	%rbx, %rax
	movq	%rdx, 8(%rax)
	movq	my_trace(%rip), %rax
	movl	-40(%rbp), %edx
	movl	%edx, %edx
	addq	$2097159, %rdx
	salq	$5, %rdx
	addq	%rdx, %rax
	movb	$1, 5(%rax)
	movq	my_trace(%rip), %rax
	movq	my_trace(%rip), %rdx
	movq	68981560(%rdx), %rcx
	movl	-40(%rbp), %edx
	addq	$56, %rcx
	movl	%edx, (%rax,%rcx,4)
	movq	my_trace(%rip), %rax
	movq	68981560(%rax), %rdx
	addq	$1, %rdx
	movq	%rdx, 68981560(%rax)
.L287:
	leaq	-48(%rbp), %rdx
	movq	-8(%rbp), %rax
	movq	%rax, %rcx
	call	Thread32Next
	testl	%eax, %eax
	setne	%al
	testb	%al, %al
	jne	.L286
	movq	-8(%rbp), %rax
	movq	%rax, %rcx
	movq	__imp_CloseHandle(%rip), %rax
	call	*%rax
	movl	$0, %eax
.L288:
	addq	$80, %rsp
	popq	%rbx
	popq	%rsi
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC137:
	.ascii "# Trap is set for 0x%08x\12\0"
	.text
	.globl	_Z9verify_ssm
	.def	_Z9verify_ssm;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z9verify_ssm
_Z9verify_ssm:
.LFB2492:
	pushq	%rbp
	.seh_pushreg	%rbp
	subq	$2048, %rsp
	.seh_stackalloc	2048
	leaq	128(%rsp), %rbp
	.seh_setframe	%rbp, 128
	.seh_endprologue
	movl	%ecx, 1936(%rbp)
	cmpl	$0, 1936(%rbp)
	jne	.L290
	movl	$0, 1916(%rbp)
	jmp	.L291
.L292:
	movq	my_trace(%rip), %rax
	movl	1916(%rbp), %edx
	movslq	%edx, %rdx
	addq	$56, %rdx
	movl	(%rax,%rdx,4), %eax
	movl	%eax, 1912(%rbp)
	movl	1912(%rbp), %eax
	movl	%eax, %ecx
	call	_Z9verify_ssm
	addl	$1, 1916(%rbp)
.L291:
	movl	1916(%rbp), %eax
	movslq	%eax, %rdx
	movq	my_trace(%rip), %rax
	movq	68981560(%rax), %rax
	cmpq	%rax, %rdx
	jb	.L292
	jmp	.L293
.L290:
	movl	$1048577, 720(%rbp)
	movl	1936(%rbp), %r8d
	movl	$0, %edx
	movl	$2097151, %ecx
	movq	__imp_OpenThread(%rip), %rax
	call	*%rax
	movq	%rax, 1904(%rbp)
	leaq	672(%rbp), %rdx
	movq	1904(%rbp), %rax
	movq	%rax, %rcx
	movq	__imp_GetThreadContext(%rip), %rax
	call	*%rax
	movl	740(%rbp), %eax
	andb	$254, %ah
	testl	%eax, %eax
	je	.L294
	leaq	-96(%rbp), %rax
	movl	1936(%rbp), %r8d
	leaq	.LC137(%rip), %rdx
	movq	%rax, %rcx
	call	sprintf
	leaq	-96(%rbp), %rax
	movq	%rax, %rcx
	call	_Z13add_to_bufferPc
.L294:
	leaq	672(%rbp), %rdx
	movq	1904(%rbp), %rax
	movq	%rax, %rcx
	movq	__imp_SetThreadContext(%rip), %rax
	call	*%rax
	movq	1904(%rbp), %rax
	movq	%rax, %rcx
	movq	__imp_CloseHandle(%rip), %rax
	call	*%rax
.L293:
	movl	$0, %eax
	addq	$2048, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.globl	_Z8unset_ssm
	.def	_Z8unset_ssm;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z8unset_ssm
_Z8unset_ssm:
.LFB2493:
	pushq	%rbp
	.seh_pushreg	%rbp
	subq	$1296, %rsp
	.seh_stackalloc	1296
	leaq	128(%rsp), %rbp
	.seh_setframe	%rbp, 128
	.seh_endprologue
	movl	%ecx, 1184(%rbp)
	cmpl	$0, 1184(%rbp)
	jne	.L297
	movl	$0, 1164(%rbp)
	jmp	.L298
.L299:
	movq	my_trace(%rip), %rax
	movl	1164(%rbp), %edx
	movslq	%edx, %rdx
	addq	$56, %rdx
	movl	(%rax,%rdx,4), %eax
	movl	%eax, 1148(%rbp)
	movl	1148(%rbp), %eax
	movl	%eax, %ecx
	call	_Z8unset_ssm
	addl	$1, 1164(%rbp)
.L298:
	movl	1164(%rbp), %eax
	movslq	%eax, %rdx
	movq	my_trace(%rip), %rax
	movq	68981560(%rax), %rax
	cmpq	%rax, %rdx
	jb	.L299
	jmp	.L300
.L297:
	movl	$1048577, -48(%rbp)
	movl	1184(%rbp), %r8d
	movl	$0, %edx
	movl	$2097151, %ecx
	movq	__imp_OpenThread(%rip), %rax
	call	*%rax
	movq	%rax, 1152(%rbp)
	cmpq	$0, 1152(%rbp)
	jne	.L301
	movl	$-1, %eax
	jmp	.L303
.L301:
	leaq	-96(%rbp), %rdx
	movq	1152(%rbp), %rax
	movq	%rax, %rcx
	movq	__imp_GetThreadContext(%rip), %rax
	call	*%rax
	movl	-28(%rbp), %eax
	andb	$254, %ah
	movl	%eax, -28(%rbp)
	leaq	-96(%rbp), %rdx
	movq	1152(%rbp), %rax
	movq	%rax, %rcx
	movq	__imp_SetThreadContext(%rip), %rax
	call	*%rax
	movq	1152(%rbp), %rax
	movq	%rax, %rcx
	movq	__imp_CloseHandle(%rip), %rax
	call	*%rax
.L300:
	movl	$0, %eax
.L303:
	addq	$1296, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC138:
	.ascii "Failed to open TID: 0x%08x\12\0"
	.text
	.globl	_Z6set_ssm
	.def	_Z6set_ssm;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z6set_ssm
_Z6set_ssm:
.LFB2494:
	pushq	%rbp
	.seh_pushreg	%rbp
	subq	$1296, %rsp
	.seh_stackalloc	1296
	leaq	128(%rsp), %rbp
	.seh_setframe	%rbp, 128
	.seh_endprologue
	movl	%ecx, 1184(%rbp)
	movq	my_trace(%rip), %rdx
	movq	my_trace(%rip), %rax
	movl	1184(%rbp), %ecx
	addq	$56, %rcx
	movl	(%rax,%rcx,4), %eax
	movl	%eax, %eax
	salq	$5, %rax
	addq	%rdx, %rax
	addq	$67109104, %rax
	movzbl	(%rax), %eax
	cmpb	$1, %al
	jne	.L305
	movl	$0, %eax
	jmp	.L312
.L305:
	cmpl	$0, 1184(%rbp)
	jne	.L307
	movl	$0, 1164(%rbp)
	jmp	.L308
.L309:
	movq	my_trace(%rip), %rax
	movl	1164(%rbp), %edx
	movslq	%edx, %rdx
	addq	$2097159, %rdx
	salq	$5, %rdx
	addq	%rdx, %rax
	movl	(%rax), %eax
	movl	%eax, 1148(%rbp)
	movl	1148(%rbp), %eax
	movl	%eax, %ecx
	call	_Z6set_ssm
	addl	$1, 1164(%rbp)
.L308:
	movl	1164(%rbp), %eax
	movslq	%eax, %rdx
	movq	my_trace(%rip), %rax
	movq	68981560(%rax), %rax
	cmpq	%rax, %rdx
	jb	.L309
	jmp	.L310
.L307:
	movl	$1048577, -48(%rbp)
	movl	1184(%rbp), %r8d
	movl	$0, %edx
	movl	$2097151, %ecx
	movq	__imp_OpenThread(%rip), %rax
	call	*%rax
	movq	%rax, 1152(%rbp)
	cmpq	$0, 1152(%rbp)
	jne	.L311
	movl	1184(%rbp), %edx
	leaq	.LC138(%rip), %rcx
	call	_Z7d_printPKcz
	movl	$-1, %eax
	jmp	.L312
.L311:
	leaq	-96(%rbp), %rdx
	movq	1152(%rbp), %rax
	movq	%rax, %rcx
	movq	__imp_GetThreadContext(%rip), %rax
	call	*%rax
	movl	-28(%rbp), %eax
	orb	$1, %ah
	movl	%eax, -28(%rbp)
	leaq	-96(%rbp), %rdx
	movq	1152(%rbp), %rax
	movq	%rax, %rcx
	movq	__imp_SetThreadContext(%rip), %rax
	call	*%rax
	movq	1152(%rbp), %rax
	movq	%rax, %rcx
	movq	__imp_CloseHandle(%rip), %rax
	call	*%rax
.L310:
	movl	$0, %eax
.L312:
	addq	$1296, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC139:
	.ascii "Offhit: %08x\12\0"
.LC140:
	.ascii "Instr_no: %lld\12\0"
.LC141:
	.ascii "Print debug info @ 0x%08x\12\0"
	.align 8
.LC142:
	.ascii "Scan start triggered by watchpoint %d @ instr no %lld\12\0"
.LC143:
	.ascii "# Scan start\12\0"
.LC144:
	.ascii "Previous not hit\12\0"
	.text
	.globl	_Z11check_debugyym
	.def	_Z11check_debugyym;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z11check_debugyym
_Z11check_debugyym:
.LFB2495:
	pushq	%rbp
	.seh_pushreg	%rbp
	subq	$1280, %rsp
	.seh_stackalloc	1280
	leaq	128(%rsp), %rbp
	.seh_setframe	%rbp, 128
	.seh_endprologue
	movq	%rcx, 1168(%rbp)
	movq	%rdx, 1176(%rbp)
	movl	%r8d, 1184(%rbp)
	movq	$-1, 1136(%rbp)
	movq	my_trace(%rip), %rax
	movl	1184(%rbp), %edx
	addq	$2097159, %rdx
	salq	$5, %rdx
	addq	%rdx, %rax
	movq	8(%rax), %rax
	movq	%rax, 1136(%rbp)
	movl	$0, 1148(%rbp)
	jmp	.L314
.L321:
	movl	1148(%rbp), %eax
	salq	$5, %rax
	movq	%rax, %rdx
	leaq	watched(%rip), %rax
	movq	(%rdx,%rax), %rax
	cmpq	1168(%rbp), %rax
	je	.L315
	movl	1148(%rbp), %eax
	salq	$5, %rax
	movq	%rax, %rdx
	leaq	16+watched(%rip), %rax
	movq	8(%rdx,%rax), %rax
	cmpq	1176(%rbp), %rax
	jne	.L316
.L315:
	movq	1168(%rbp), %rdx
	leaq	.LC139(%rip), %rcx
	call	_Z7d_printPKcz
	movq	my_trace(%rip), %rax
	movq	68981528(%rax), %rax
	movq	%rax, %rdx
	leaq	.LC140(%rip), %rcx
	call	_Z7d_printPKcz
	movl	1148(%rbp), %eax
	salq	$5, %rax
	movq	%rax, %rdx
	leaq	watched(%rip), %rax
	movl	8(%rdx,%rax), %edx
	movl	1148(%rbp), %eax
	salq	$5, %rax
	movq	%rax, %rcx
	leaq	16+watched(%rip), %rax
	movl	(%rcx,%rax), %eax
	cmpl	%eax, %edx
	jg	.L317
	movl	$1048587, -48(%rbp)
	leaq	-96(%rbp), %rdx
	movq	1136(%rbp), %rax
	movq	%rax, %rcx
	movq	__imp_GetThreadContext(%rip), %rax
	call	*%rax
	testl	%eax, %eax
	sete	%al
	testb	%al, %al
	je	.L318
	movq	__imp_GetLastError(%rip), %rax
	call	*%rax
	movl	%eax, %edx
	leaq	.LC7(%rip), %rcx
	call	_Z7d_printPKcz
.L318:
	movq	1168(%rbp), %rdx
	leaq	.LC141(%rip), %rcx
	call	_Z7d_printPKcz
	leaq	-96(%rbp), %rax
	movq	%rax, %rcx
	call	_Z13print_contextP8_CONTEXT
	movl	1148(%rbp), %eax
	salq	$5, %rax
	movq	%rax, %rdx
	leaq	16+watched(%rip), %rax
	movl	(%rdx,%rax), %eax
	leal	1(%rax), %ecx
	movl	1148(%rbp), %eax
	salq	$5, %rax
	movq	%rax, %rdx
	leaq	16+watched(%rip), %rax
	movl	%ecx, (%rdx,%rax)
	movl	1148(%rbp), %eax
	salq	$5, %rax
	movq	%rax, %rdx
	leaq	watched(%rip), %rax
	movl	12(%rdx,%rax), %eax
	cmpl	$1, %eax
	jne	.L319
	movq	1176(%rbp), %rdx
	movl	1148(%rbp), %eax
	movq	%rdx, %r8
	movl	%eax, %edx
	leaq	.LC142(%rip), %rcx
	call	_Z7d_printPKcz
	movq	file(%rip), %rax
	movq	%rax, %r9
	movl	$1, %r8d
	movl	$13, %edx
	leaq	.LC143(%rip), %rcx
	call	fwrite
	movl	$1, scan_on(%rip)
.L319:
	jmp	.L320
.L317:
	movl	1148(%rbp), %eax
	salq	$5, %rax
	movq	%rax, %rdx
	leaq	16+watched(%rip), %rax
	movl	(%rdx,%rax), %eax
	leal	1(%rax), %ecx
	movl	1148(%rbp), %eax
	salq	$5, %rax
	movq	%rax, %rdx
	leaq	16+watched(%rip), %rax
	movl	%ecx, (%rdx,%rax)
.L320:
	leaq	.LC144(%rip), %rcx
	call	_Z7d_printPKcz
.L316:
	addl	$1, 1148(%rbp)
.L314:
	cmpl	$255, 1148(%rbp)
	jbe	.L321
	nop
	addq	$1280, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.globl	_Z7is_cally
	.def	_Z7is_cally;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z7is_cally
_Z7is_cally:
.LFB2496:
	pushq	%rbp
	.seh_pushreg	%rbp
	subq	$816, %rsp
	.seh_stackalloc	816
	leaq	128(%rsp), %rbp
	.seh_setframe	%rbp, 128
	.seh_endprologue
	movq	%rcx, 704(%rbp)
	movq	704(%rbp), %rcx
	call	_Z9read_bytey
	movb	%al, 687(%rbp)
	cmpb	$-24, 687(%rbp)
	je	.L324
	cmpb	$-1, 687(%rbp)
	jne	.L325
.L324:
	movl	$1, %eax
	jmp	.L327
.L325:
	movl	$0, %eax
.L327:
	addq	$816, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC145:
	.ascii "Error writing to file: 0x%x\12\0"
.LC146:
	.ascii "0x%x 0x%08x %lld\12\0"
	.text
	.globl	_Z11ss_callbackPv
	.def	_Z11ss_callbackPv;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z11ss_callbackPv
_Z11ss_callbackPv:
.LFB2497:
	pushq	%rbp
	.seh_pushreg	%rbp
	subq	$864, %rsp
	.seh_stackalloc	864
	leaq	128(%rsp), %rbp
	.seh_setframe	%rbp, 128
	.seh_endprologue
	movq	%rcx, 752(%rbp)
	movq	752(%rbp), %rax
	movq	%rax, 728(%rbp)
	movl	$0, 724(%rbp)
	movq	728(%rbp), %rax
	movq	32(%rax), %rax
	movq	%rax, 712(%rbp)
	movq	my_trace(%rip), %rax
	movzbl	68981524(%rax), %eax
	cmpb	$3, %al
	je	.L329
	movq	my_trace(%rip), %rax
	movzbl	68981524(%rax), %eax
	cmpb	$4, %al
	je	.L329
	movq	my_trace(%rip), %rax
	movzbl	68981524(%rax), %eax
	cmpb	$5, %al
	je	.L329
	jmp	.L328
.L329:
	movq	728(%rbp), %rax
	movl	8(%rax), %eax
	movl	%eax, 708(%rbp)
	movq	my_trace(%rip), %rax
	movl	708(%rbp), %edx
	addq	$56, %rdx
	movl	(%rax,%rdx,4), %eax
	movl	%eax, 704(%rbp)
	movq	my_trace(%rip), %rax
	movq	67649776(%rax), %rax
	movl	$-1, %edx
	movq	%rax, %rcx
	movq	__imp_WaitForSingleObject(%rip), %rax
	call	*%rax
	movq	728(%rbp), %rax
	movq	32(%rax), %rax
	movq	%rax, 712(%rbp)
	movq	my_trace(%rip), %rax
	movq	712(%rbp), %rdx
	movq	%rdx, 68981960(%rax)
	movq	my_trace(%rip), %rax
	movl	708(%rbp), %edx
	movl	%edx, 68981968(%rax)
	movq	my_trace(%rip), %rax
	movzbl	68981524(%rax), %eax
	cmpb	$4, %al
	jne	.L331
	movq	my_trace(%rip), %rax
	movl	704(%rbp), %edx
	addq	$2097159, %rdx
	salq	$5, %rdx
	addq	%rdx, %rax
	movq	8(%rax), %rdx
	movl	708(%rbp), %eax
	movl	%eax, %ecx
	call	_Z21register_thread_debugmPv
	testl	%eax, %eax
	setle	%al
	testb	%al, %al
	je	.L331
	movq	__imp_GetLastError(%rip), %rax
	call	*%rax
	movl	%eax, %edx
	leaq	.LC145(%rip), %rcx
	call	_Z7d_printPKcz
	movl	$1, %ecx
	call	exit
.L331:
	movq	my_trace(%rip), %rax
	movzbl	68981524(%rax), %eax
	cmpb	$5, %al
	jne	.L332
	movq	712(%rbp), %rax
	movq	%rax, %rcx
	call	_Z7is_cally
	testl	%eax, %eax
	setne	%al
	testb	%al, %al
	je	.L332
	movq	my_trace(%rip), %rax
	movl	704(%rbp), %edx
	addq	$2097159, %rdx
	salq	$5, %rdx
	addq	%rdx, %rax
	movq	8(%rax), %rdx
	movl	708(%rbp), %eax
	movl	%eax, %ecx
	call	_Z21register_thread_debugmPv
	testl	%eax, %eax
	setle	%al
	testb	%al, %al
	je	.L332
	movq	__imp_GetLastError(%rip), %rax
	call	*%rax
	movl	%eax, %edx
	leaq	.LC145(%rip), %rcx
	call	_Z7d_printPKcz
	movl	$1, %ecx
	call	exit
.L332:
	movq	my_trace(%rip), %rax
	movq	68981528(%rax), %rcx
	movabsq	$-2972493582642298179, %rdx
	movq	%rcx, %rax
	mulq	%rdx
	movq	%rdx, %rax
	shrq	$23, %rax
	imulq	$10000000, %rax, %rax
	subq	%rax, %rcx
	movq	%rcx, %rax
	testq	%rax, %rax
	jne	.L333
	movq	my_trace(%rip), %rax
	movq	68981528(%rax), %rax
	testq	%rax, %rax
	je	.L333
	call	_Z15reload_out_filev
.L333:
	movq	my_trace(%rip), %rax
	movq	68981528(%rax), %rdx
	addq	$1, %rdx
	movq	%rdx, 68981528(%rax)
	movq	my_trace(%rip), %rax
	movq	68981528(%rax), %rdx
	movq	712(%rbp), %r8
	movl	708(%rbp), %ecx
	leaq	-80(%rbp), %rax
	movq	%rdx, 32(%rsp)
	movq	%r8, %r9
	movl	%ecx, %r8d
	leaq	.LC146(%rip), %rdx
	movq	%rax, %rcx
	call	sprintf
	leaq	-80(%rbp), %rax
	movq	%rax, %rcx
	call	_Z13add_to_bufferPc
	movq	my_trace(%rip), %rax
	movq	68981544(%rax), %rax
	testq	%rax, %rax
	je	.L334
	movq	my_trace(%rip), %rax
	movq	68981528(%rax), %rdx
	movq	my_trace(%rip), %rax
	movq	68981544(%rax), %rax
	cmpq	%rax, %rdx
	jb	.L334
	movl	$0, %ecx
	call	_Z14HandlerRoutinem
.L334:
	movq	my_trace(%rip), %rax
	movq	67649776(%rax), %rax
	movq	%rax, %rcx
	movq	__imp_ReleaseMutex(%rip), %rax
	call	*%rax
	nop
.L328:
	addq	$864, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.globl	_Z15page_accessible25_MEMORY_BASIC_INFORMATION
	.def	_Z15page_accessible25_MEMORY_BASIC_INFORMATION;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z15page_accessible25_MEMORY_BASIC_INFORMATION
_Z15page_accessible25_MEMORY_BASIC_INFORMATION:
.LFB2498:
	pushq	%rbp
	.seh_pushreg	%rbp
	pushq	%rbx
	.seh_pushreg	%rbx
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	.seh_endprologue
	movq	%rcx, %rbx
	movl	16(%rbx), %eax
	andl	$256, %eax
	testl	%eax, %eax
	je	.L337
	movl	$0, %eax
	jmp	.L338
.L337:
	movl	36(%rbx), %eax
	andl	$256, %eax
	testl	%eax, %eax
	je	.L339
	movl	$0, %eax
	jmp	.L338
.L339:
	movl	16(%rbx), %eax
	andl	$1, %eax
	testl	%eax, %eax
	je	.L340
	movl	$0, %eax
	jmp	.L338
.L340:
	movl	36(%rbx), %eax
	andl	$1, %eax
	testl	%eax, %eax
	je	.L341
	movl	$0, %eax
	jmp	.L338
.L341:
	movl	$1, %eax
.L338:
	popq	%rbx
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC147:
	.ascii "Position before: 0x%08x\12\0"
.LC148:
	.ascii "Read: 0x%08x\12\0"
	.align 8
.LC149:
	.ascii "Failed to read from %p to %p\12Error: 0x%08x\12\0"
.LC150:
	.ascii "0x%02x \0"
.LC151:
	.ascii "\12\0"
.LC152:
	.ascii "Position after: 0x%08x\12\0"
	.text
	.globl	_Z8dump_memP6_iobufPvy
	.def	_Z8dump_memP6_iobufPvy;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z8dump_memP6_iobufPvy
_Z8dump_memP6_iobufPvy:
.LFB2499:
	pushq	%rbp
	.seh_pushreg	%rbp
	movl	$1048672, %eax
	call	___chkstk_ms
	subq	%rax, %rsp
	.seh_stackalloc	1048672
	leaq	128(%rsp), %rbp
	.seh_setframe	%rbp, 128
	.seh_endprologue
	movq	%rcx, 1048560(%rbp)
	movq	%rdx, 1048568(%rbp)
	movq	%r8, 1048576(%rbp)
	movq	$0, 1048528(%rbp)
	movq	1048576(%rbp), %rax
	andl	$1048575, %eax
	movq	%rax, 1048512(%rbp)
	movq	1048576(%rbp), %rax
	subq	1048512(%rbp), %rax
	movq	%rax, 1048504(%rbp)
	movq	1048560(%rbp), %rcx
	call	ftell
	movl	%eax, %edx
	leaq	.LC147(%rip), %rcx
	call	_Z7d_printPKcz
	movq	$0, 1048536(%rbp)
	jmp	.L343
.L346:
	movq	1048568(%rbp), %rdx
	movq	1048536(%rbp), %rax
	leaq	(%rdx,%rax), %rcx
	movq	my_trace(%rip), %rax
	movq	208(%rax), %rax
	leaq	-80(%rbp), %r8
	leaq	1048496(%rbp), %rdx
	movq	%rdx, 32(%rsp)
	movl	$1048576, %r9d
	movq	%rcx, %rdx
	movq	%rax, %rcx
	movq	__imp_ReadProcessMemory(%rip), %rax
	call	*%rax
	movq	1048496(%rbp), %rax
	movq	%rax, %rdx
	leaq	.LC148(%rip), %rcx
	call	_Z7d_printPKcz
	movq	1048496(%rbp), %rax
	testq	%rax, %rax
	jne	.L344
	movq	__imp_GetLastError(%rip), %rax
	call	*%rax
	movl	%eax, %ecx
	movq	1048568(%rbp), %rdx
	movq	1048512(%rbp), %rax
	addq	%rax, %rdx
	movq	1048568(%rbp), %rax
	movl	%ecx, %r9d
	movq	%rdx, %r8
	movq	%rax, %rdx
	leaq	.LC149(%rip), %rcx
	call	_Z7d_printPKcz
	jmp	.L345
.L344:
	movq	1048496(%rbp), %rdx
	leaq	-80(%rbp), %rax
	movq	1048560(%rbp), %r9
	movl	$1, %r8d
	movq	%rax, %rcx
	call	fwrite
	movq	1048496(%rbp), %rax
	addq	%rax, 1048528(%rbp)
	addq	$1048576, 1048536(%rbp)
.L343:
	movq	1048536(%rbp), %rax
	cmpq	1048504(%rbp), %rax
	jb	.L346
.L345:
	movl	$0, 1048524(%rbp)
	jmp	.L347
.L348:
	movl	1048524(%rbp), %eax
	movzbl	-80(%rbp,%rax), %eax
	movsbl	%al, %eax
	movl	%eax, %edx
	leaq	.LC150(%rip), %rcx
	call	_Z7d_printPKcz
	addl	$1, 1048524(%rbp)
.L347:
	cmpl	$4, 1048524(%rbp)
	jbe	.L348
	leaq	.LC151(%rip), %rcx
	call	_Z7d_printPKcz
	cmpq	$0, 1048512(%rbp)
	je	.L349
	movq	1048568(%rbp), %rdx
	movq	1048536(%rbp), %rax
	leaq	(%rdx,%rax), %rcx
	movq	my_trace(%rip), %rax
	movq	208(%rax), %rax
	movq	1048512(%rbp), %r9
	leaq	-80(%rbp), %r8
	leaq	1048496(%rbp), %rdx
	movq	%rdx, 32(%rsp)
	movq	%rcx, %rdx
	movq	%rax, %rcx
	movq	__imp_ReadProcessMemory(%rip), %rax
	call	*%rax
	movq	1048496(%rbp), %rax
	movq	%rax, %rdx
	leaq	.LC148(%rip), %rcx
	call	_Z7d_printPKcz
	movq	1048496(%rbp), %rax
	testq	%rax, %rax
	jne	.L350
	movq	__imp_GetLastError(%rip), %rax
	call	*%rax
	movl	%eax, %ecx
	movq	1048568(%rbp), %rdx
	movq	1048512(%rbp), %rax
	addq	%rax, %rdx
	movq	1048568(%rbp), %rax
	movl	%ecx, %r9d
	movq	%rdx, %r8
	movq	%rax, %rdx
	leaq	.LC149(%rip), %rcx
	call	_Z7d_printPKcz
.L350:
	movq	1048496(%rbp), %rdx
	leaq	-80(%rbp), %rax
	movq	1048560(%rbp), %r9
	movl	$1, %r8d
	movq	%rax, %rcx
	call	fwrite
	movq	1048496(%rbp), %rax
	addq	%rax, 1048528(%rbp)
.L349:
	movl	$0, 1048524(%rbp)
	jmp	.L351
.L352:
	movl	1048524(%rbp), %eax
	movzbl	-80(%rbp,%rax), %eax
	movsbl	%al, %eax
	movl	%eax, %edx
	leaq	.LC150(%rip), %rcx
	call	_Z7d_printPKcz
	addl	$1, 1048524(%rbp)
.L351:
	cmpl	$4, 1048524(%rbp)
	jbe	.L352
	leaq	.LC151(%rip), %rcx
	call	_Z7d_printPKcz
	movq	1048560(%rbp), %rcx
	call	ftell
	movl	%eax, %edx
	leaq	.LC152(%rip), %rcx
	call	_Z7d_printPKcz
	movq	1048528(%rbp), %rax
	addq	$1048672, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.globl	_Z10dump_zerosP6_iobufy
	.def	_Z10dump_zerosP6_iobufy;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z10dump_zerosP6_iobufy
_Z10dump_zerosP6_iobufy:
.LFB2500:
	pushq	%rbp
	.seh_pushreg	%rbp
	movl	$1048640, %eax
	call	___chkstk_ms
	subq	%rax, %rsp
	.seh_stackalloc	1048640
	leaq	128(%rsp), %rbp
	.seh_setframe	%rbp, 128
	.seh_endprologue
	movq	%rcx, 1048528(%rbp)
	movq	%rdx, 1048536(%rbp)
	movq	$0, 1048496(%rbp)
	movq	1048536(%rbp), %rax
	andl	$1048575, %eax
	movq	%rax, 1048488(%rbp)
	movq	1048536(%rbp), %rax
	subq	1048488(%rbp), %rax
	movq	%rax, 1048480(%rbp)
	leaq	-96(%rbp), %rax
	movl	$1048576, %r8d
	movl	$0, %edx
	movq	%rax, %rcx
	call	memset
	movq	$0, 1048504(%rbp)
	jmp	.L355
.L356:
	leaq	-96(%rbp), %rax
	movq	1048528(%rbp), %r9
	movl	$1, %r8d
	movl	$1048576, %edx
	movq	%rax, %rcx
	call	fwrite
	addq	$1048576, 1048496(%rbp)
	addq	$1048576, 1048504(%rbp)
.L355:
	movq	1048504(%rbp), %rax
	cmpq	1048480(%rbp), %rax
	jb	.L356
	cmpq	$0, 1048488(%rbp)
	je	.L357
	movq	1048488(%rbp), %rdx
	leaq	-96(%rbp), %rax
	movq	1048528(%rbp), %r9
	movl	$1, %r8d
	movq	%rax, %rcx
	call	fwrite
	movq	1048488(%rbp), %rax
	addq	%rax, 1048496(%rbp)
.L357:
	movq	1048496(%rbp), %rax
	addq	$1048640, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC153:
	.ascii "Dumping mem start\12\0"
.LC154:
	.ascii "wb\0"
	.align 8
.LC155:
	.ascii "Block 0x%08x - 0x%08x, 0x%08x bytes\12\0"
.LC156:
	.ascii "dumping mem end\12\0"
.LC157:
	.ascii "LM,%s.dump\12\0"
	.text
	.globl	_Z11dump_memoryv
	.def	_Z11dump_memoryv;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z11dump_memoryv
_Z11dump_memoryv:
.LFB2501:
	pushq	%rbp
	.seh_pushreg	%rbp
	pushq	%rbx
	.seh_pushreg	%rbx
	subq	$920, %rsp
	.seh_stackalloc	920
	leaq	128(%rsp), %rbp
	.seh_setframe	%rbp, 128
	.seh_endprologue
	leaq	.LC153(%rip), %rcx
	call	_Z7d_printPKcz
	movq	my_trace(%rip), %rbx
	movq	my_trace(%rip), %rax
	addq	$70036012, %rax
	leaq	.LC154(%rip), %rdx
	movq	%rax, %rcx
	call	fopen
	movq	%rax, 70030600(%rbx)
	movq	$0, 776(%rbp)
	jmp	.L360
.L367:
	movq	776(%rbp), %rdx
	movq	my_trace(%rip), %rax
	movq	208(%rax), %rax
	leaq	-48(%rbp), %rcx
	movl	$48, %r9d
	movq	%rcx, %r8
	movq	%rax, %rcx
	movq	__imp_VirtualQueryEx(%rip), %rax
	call	*%rax
	movq	__imp_GetLastError(%rip), %rax
	call	*%rax
	cmpl	$87, %eax
	sete	%al
	testb	%al, %al
	je	.L361
	jmp	.L362
.L361:
	movq	-24(%rbp), %rdx
	movq	-48(%rbp), %rcx
	movq	-24(%rbp), %rax
	addq	%rax, %rcx
	movq	-48(%rbp), %rax
	movq	%rdx, %r9
	movq	%rcx, %r8
	movq	%rax, %rdx
	leaq	.LC155(%rip), %rcx
	call	_Z7d_printPKcz
	movl	-16(%rbp), %eax
	cmpl	$4096, %eax
	jne	.L363
	movq	-48(%rbp), %rax
	movq	%rax, -96(%rbp)
	movq	-40(%rbp), %rax
	movq	%rax, -88(%rbp)
	movq	-32(%rbp), %rax
	movq	%rax, -80(%rbp)
	movq	-24(%rbp), %rax
	movq	%rax, -72(%rbp)
	movq	-16(%rbp), %rax
	movq	%rax, -64(%rbp)
	movq	-8(%rbp), %rax
	movq	%rax, -56(%rbp)
	leaq	-96(%rbp), %rax
	movq	%rax, %rcx
	call	_Z15page_accessible25_MEMORY_BASIC_INFORMATION
	testl	%eax, %eax
	je	.L363
	movl	$1, %eax
	jmp	.L364
.L363:
	movl	$0, %eax
.L364:
	testb	%al, %al
	je	.L365
	movq	-24(%rbp), %rcx
	movq	-48(%rbp), %rdx
	movq	my_trace(%rip), %rax
	movq	70030600(%rax), %rax
	movq	%rcx, %r8
	movq	%rax, %rcx
	call	_Z8dump_memP6_iobufPvy
	movq	%rax, 768(%rbp)
	jmp	.L366
.L365:
	movq	-24(%rbp), %rdx
	movq	my_trace(%rip), %rax
	movq	70030600(%rax), %rax
	movq	%rax, %rcx
	call	_Z10dump_zerosP6_iobufy
	movq	%rax, 768(%rbp)
.L366:
	movq	-24(%rbp), %rax
	addq	%rax, 776(%rbp)
.L360:
	movl	$4294967294, %eax
	cmpq	%rax, 776(%rbp)
	jbe	.L367
.L362:
	movq	my_trace(%rip), %rax
	movq	70030600(%rax), %rax
	movq	%rax, %rcx
	call	fclose
	leaq	.LC156(%rip), %rcx
	call	_Z7d_printPKcz
	movq	my_trace(%rip), %rax
	leaq	70034472(%rax), %rdx
	movq	%rbp, %rax
	movq	%rdx, %r8
	leaq	.LC157(%rip), %rdx
	movq	%rax, %rcx
	call	sprintf
	movq	%rbp, %rax
	movq	%rax, %rcx
	call	_Z13add_to_bufferPc
	nop
	addq	$920, %rsp
	popq	%rbx
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC158:
	.ascii "Dumping contexts\12\0"
	.text
	.globl	_Z13dump_contextsv
	.def	_Z13dump_contextsv;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z13dump_contextsv
_Z13dump_contextsv:
.LFB2502:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$32, %rsp
	.seh_stackalloc	32
	.seh_endprologue
	leaq	.LC158(%rip), %rcx
	call	_Z7d_printPKcz
	nop
	addq	$32, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
	.align 8
.LC159:
	.ascii "Finishing @ 0x%08x, detaching\12\0"
.LC160:
	.ascii "FI,0x%08x\12\0"
	.text
	.globl	_Z12end_callbackPv
	.def	_Z12end_callbackPv;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z12end_callbackPv
_Z12end_callbackPv:
.LFB2503:
	pushq	%rbp
	.seh_pushreg	%rbp
	subq	$816, %rsp
	.seh_stackalloc	816
	leaq	128(%rsp), %rbp
	.seh_setframe	%rbp, 128
	.seh_endprologue
	movq	%rcx, 704(%rbp)
	movq	704(%rbp), %rcx
	call	_Z11ss_callbackPv
	movq	704(%rbp), %rax
	movq	%rax, 680(%rbp)
	movq	680(%rbp), %rax
	movq	32(%rax), %rax
	movq	%rax, 672(%rbp)
	movq	672(%rbp), %rax
	movq	%rax, %rdx
	leaq	.LC159(%rip), %rcx
	call	_Z7d_printPKcz
	movq	672(%rbp), %rdx
	leaq	-96(%rbp), %rax
	movq	%rdx, %r8
	leaq	.LC160(%rip), %rdx
	movq	%rax, %rcx
	call	sprintf
	leaq	-96(%rbp), %rax
	movq	%rax, %rcx
	call	_Z13add_to_bufferPc
	movl	$0, %ecx
	call	_Z14HandlerRoutinem
	nop
	addq	$816, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC161:
	.ascii "Detected ctrl-c\12\0"
.LC162:
	.ascii "Deregistering threads\12\0"
.LC163:
	.ascii "Deregistering: 0x%08x\12\0"
.LC164:
	.ascii "Deregistering libs\12\0"
.LC165:
	.ascii "instruction count\0"
.LC166:
	.ascii "general\0"
	.text
	.globl	_Z14HandlerRoutinem
	.def	_Z14HandlerRoutinem;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z14HandlerRoutinem
_Z14HandlerRoutinem:
.LFB2504:
	pushq	%rbp
	.seh_pushreg	%rbp
	pushq	%rbx
	.seh_pushreg	%rbx
	subq	$56, %rsp
	.seh_stackalloc	56
	leaq	128(%rsp), %rbp
	.seh_setframe	%rbp, 128
	.seh_endprologue
	movl	%ecx, -48(%rbp)
	leaq	.LC161(%rip), %rcx
	call	_Z7d_printPKcz
	leaq	.LC162(%rip), %rcx
	call	_Z7d_printPKcz
	movl	$0, -84(%rbp)
	jmp	.L371
.L373:
	movl	-84(%rbp), %eax
	movl	%eax, -88(%rbp)
	movq	my_trace(%rip), %rax
	movl	-88(%rbp), %edx
	addq	$2097159, %rdx
	salq	$5, %rdx
	addq	%rdx, %rax
	movzbl	4(%rax), %eax
	cmpb	$1, %al
	jne	.L372
	movq	my_trace(%rip), %rax
	movl	-88(%rbp), %edx
	addq	$2097159, %rdx
	salq	$5, %rdx
	addq	%rdx, %rax
	movl	(%rax), %eax
	movl	%eax, %edx
	leaq	.LC163(%rip), %rcx
	call	_Z7d_printPKcz
	movq	my_trace(%rip), %rax
	movl	-88(%rbp), %edx
	addq	$2097159, %rdx
	salq	$5, %rdx
	addq	%rdx, %rax
	movq	8(%rax), %rdx
	movq	my_trace(%rip), %rax
	movl	-88(%rbp), %ecx
	addq	$2097159, %rcx
	salq	$5, %rcx
	addq	%rcx, %rax
	movl	(%rax), %eax
	movl	%eax, %ecx
	call	_Z17deregister_threadmPv
.L372:
	addl	$1, -84(%rbp)
.L371:
	movl	-84(%rbp), %edx
	movq	my_trace(%rip), %rax
	movq	68981560(%rax), %rax
	cmpq	%rax, %rdx
	jb	.L373
	leaq	.LC164(%rip), %rcx
	call	_Z7d_printPKcz
	movl	$0, -84(%rbp)
	jmp	.L374
.L376:
	movq	my_trace(%rip), %rdx
	movl	-84(%rbp), %eax
	salq	$4, %rax
	movq	%rax, %rcx
	salq	$6, %rcx
	addq	%rcx, %rax
	addq	%rdx, %rax
	addq	$67117280, %rax
	movzbl	(%rax), %eax
	cmpb	$1, %al
	jne	.L375
	movl	-84(%rbp), %eax
	movl	%eax, %ecx
	call	_Z14deregister_libm
.L375:
	addl	$1, -84(%rbp)
.L374:
	movl	-84(%rbp), %edx
	movq	my_trace(%rip), %rax
	movq	68981568(%rax), %rax
	cmpq	%rax, %rdx
	jb	.L376
	movq	my_trace(%rip), %rax
	leaq	70036780(%rax), %rbx
	movq	my_trace(%rip), %rax
	movq	68981528(%rax), %rax
	movl	$10, %r8d
	leaq	instr_count_s(%rip), %rdx
	movl	%eax, %ecx
	call	itoa
	movq	%rbx, %r9
	movq	%rax, %r8
	leaq	.LC165(%rip), %rdx
	leaq	.LC166(%rip), %rcx
	movq	__imp_WritePrivateProfileStringA(%rip), %rax
	call	*%rax
	movq	my_trace(%rip), %rax
	movq	67649760(%rax), %rax
	movq	%rax, %rcx
	movq	__imp_SetEvent(%rip), %rax
	call	*%rax
	movq	my_trace(%rip), %rax
	movq	67649768(%rax), %rax
	movl	$-1, %edx
	movq	%rax, %rcx
	movq	__imp_WaitForSingleObject(%rip), %rax
	call	*%rax
	movl	$1, %ecx
	call	exit
	nop
	.seh_endproc
	.globl	_Z6writerPv
	.def	_Z6writerPv;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z6writerPv
_Z6writerPv:
.LFB2505:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$32, %rsp
	.seh_stackalloc	32
	.seh_endprologue
	movq	%rcx, 16(%rbp)
.L378:
	movq	my_trace(%rip), %rax
	movq	67649760(%rax), %rax
	movl	$-1, %edx
	movq	%rax, %rcx
	movq	__imp_WaitForSingleObject(%rip), %rax
	call	*%rax
	movq	my_trace(%rip), %rax
	movq	67649760(%rax), %rax
	movq	%rax, %rcx
	movq	__imp_ResetEvent(%rip), %rax
	call	*%rax
	movq	my_trace(%rip), %rax
	movq	70030584(%rax), %rdx
	movl	index(%rip), %eax
	cltq
	movq	%rdx, %r9
	movl	$1, %r8d
	movq	%rax, %rdx
	leaq	buffer(%rip), %rcx
	call	fwrite
	movq	my_trace(%rip), %rax
	movq	70030584(%rax), %rax
	movq	%rax, %rcx
	call	fflush
	movq	my_trace(%rip), %rax
	movq	67649768(%rax), %rax
	movq	%rax, %rcx
	movq	__imp_SetEvent(%rip), %rax
	call	*%rax
	jmp	.L378
	.seh_endproc
	.section .rdata,"dr"
.LC167:
	.ascii "Default handler!\12\0"
	.text
	.globl	_Z15default_handlerPv
	.def	_Z15default_handlerPv;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z15default_handlerPv
_Z15default_handlerPv:
.LFB2506:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$32, %rsp
	.seh_stackalloc	32
	.seh_endprologue
	movq	%rcx, 16(%rbp)
	leaq	.LC167(%rip), %rcx
	call	_Z7d_printPKcz
	nop
	addq	$32, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.globl	_Z18del_breakpoint_idxj
	.def	_Z18del_breakpoint_idxj;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z18del_breakpoint_idxj
_Z18del_breakpoint_idxj:
.LFB2507:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$32, %rsp
	.seh_stackalloc	32
	.seh_endprologue
	movl	%ecx, 16(%rbp)
	movq	my_trace(%rip), %rax
	movl	16(%rbp), %edx
	imulq	$552, %rdx, %rdx
	addq	$67649776, %rdx
	addq	%rdx, %rax
	addq	$8, %rax
	movq	%rax, %rcx
	call	_Z18unwrite_breakpointP11BREAKPOINT_
	movq	my_trace(%rip), %rdx
	movl	16(%rbp), %eax
	imulq	$552, %rax, %rax
	addq	%rdx, %rax
	addq	$67650304, %rax
	movb	$0, 9(%rax)
	movl	$0, %eax
	addq	$32, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
	.align 8
.LC168:
	.ascii "Unable to find breakpoint to delete\12\0"
	.text
	.globl	_Z14del_breakpointy
	.def	_Z14del_breakpointy;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z14del_breakpointy
_Z14del_breakpointy:
.LFB2508:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$48, %rsp
	.seh_stackalloc	48
	.seh_endprologue
	movq	%rcx, 16(%rbp)
	movb	$-52, -9(%rbp)
	movl	$-1, -8(%rbp)
	movl	$0, -4(%rbp)
	jmp	.L384
.L387:
	movq	my_trace(%rip), %rdx
	movl	-4(%rbp), %eax
	cltq
	imulq	$552, %rax, %rax
	addq	%rdx, %rax
	addq	$67650288, %rax
	movq	16(%rax), %rax
	cmpq	16(%rbp), %rax
	jne	.L385
	movl	-4(%rbp), %eax
	movl	%eax, -8(%rbp)
	jmp	.L386
.L385:
	addl	$1, -4(%rbp)
.L384:
	movl	-4(%rbp), %eax
	movslq	%eax, %rdx
	movq	my_trace(%rip), %rax
	movq	68981576(%rax), %rax
	cmpq	%rax, %rdx
	jb	.L387
.L386:
	cmpl	$-1, -8(%rbp)
	jne	.L388
	leaq	.LC168(%rip), %rcx
	call	_Z7d_printPKcz
	movl	$1, %eax
	jmp	.L389
.L388:
	movq	my_trace(%rip), %rax
	movl	-4(%rbp), %edx
	movslq	%edx, %rdx
	imulq	$552, %rdx, %rdx
	addq	$67649776, %rdx
	addq	%rdx, %rax
	addq	$8, %rax
	movq	%rax, %rcx
	call	_Z18unwrite_breakpointP11BREAKPOINT_
	movq	my_trace(%rip), %rdx
	movl	-8(%rbp), %eax
	cltq
	imulq	$552, %rax, %rax
	addq	%rdx, %rax
	addq	$67650304, %rax
	movb	$0, 9(%rax)
	movl	$0, %eax
.L389:
	addq	$48, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC169:
	.ascii "Current reaction: %s\12\0"
	.align 8
.LC170:
	.ascii "Enabling coupled reaction: %s\12\0"
.LC171:
	.ascii "Found coupled reaction: %s\12\0"
	.align 8
.LC172:
	.ascii "ER3 Reaction lock is active, continuing, missing reaction %s due to lock by %s\12\0"
	.align 8
.LC173:
	.ascii "ER3 Reaction lock %s overriden by %s\12\0"
	.align 8
.LC174:
	.ascii "ER32 in handle_reaction: %p, %s\12\0"
	.align 8
.LC175:
	.ascii "ER3 Locking reaction lock with: %s\12\0"
	.align 8
.LC176:
	.ascii "Routine is null, reporting to controller\12\0"
	.align 8
.LC177:
	.ascii "ER3 Executing routine 0x%08x @ %d\12\0"
	.align 8
.LC178:
	.ascii "OU,0x%x,0x%08x Routine 0x%08x\12\0"
	.text
	.globl	_Z15handle_reactionP9REACTION_Pv
	.def	_Z15handle_reactionP9REACTION_Pv;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z15handle_reactionP9REACTION_Pv
_Z15handle_reactionP9REACTION_Pv:
.LFB2509:
	pushq	%rbp
	.seh_pushreg	%rbp
	subq	$864, %rsp
	.seh_stackalloc	864
	leaq	128(%rsp), %rbp
	.seh_setframe	%rbp, 128
	.seh_endprologue
	movq	%rcx, 752(%rbp)
	movq	%rdx, 760(%rbp)
	movq	760(%rbp), %rax
	movq	%rax, 720(%rbp)
	movq	720(%rbp), %rax
	movl	8(%rax), %eax
	movl	%eax, 716(%rbp)
	movq	my_trace(%rip), %rax
	movl	716(%rbp), %edx
	addq	$56, %rdx
	movl	(%rax,%rdx,4), %eax
	movl	%eax, 712(%rbp)
	movl	$0, 732(%rbp)
	jmp	.L391
.L393:
	movq	752(%rbp), %rcx
	movl	732(%rbp), %edx
	movq	%rdx, %rax
	addq	%rax, %rax
	addq	%rdx, %rax
	addq	%rcx, %rax
	movzbl	3(%rax), %eax
	testb	%al, %al
	je	.L392
	movq	752(%rbp), %rax
	movq	%rax, %rdx
	leaq	.LC169(%rip), %rcx
	call	_Z7d_printPKcz
	movl	732(%rbp), %edx
	movq	%rdx, %rax
	addq	%rax, %rax
	addq	%rdx, %rax
	movq	752(%rbp), %rdx
	addq	%rdx, %rax
	addq	$3, %rax
	movq	%rax, %rdx
	leaq	.LC170(%rip), %rcx
	call	_Z7d_printPKcz
	movl	732(%rbp), %edx
	movq	%rdx, %rax
	addq	%rax, %rax
	addq	%rdx, %rax
	movq	752(%rbp), %rdx
	addq	%rdx, %rax
	addq	$3, %rax
	movq	%rax, %rcx
	call	_Z13find_reactionPc
	movq	%rax, 704(%rbp)
	movq	704(%rbp), %rax
	movq	%rax, %rdx
	leaq	.LC171(%rip), %rcx
	call	_Z7d_printPKcz
	movq	704(%rbp), %rax
	movq	%rax, %rcx
	call	_Z15enable_reactionPc
.L392:
	addl	$1, 732(%rbp)
.L391:
	cmpl	$31, 732(%rbp)
	jbe	.L393
	movq	my_trace(%rip), %rax
	movl	712(%rbp), %edx
	salq	$5, %rdx
	addq	%rdx, %rax
	addq	$67109104, %rax
	movq	8(%rax), %rax
	movq	%rax, 696(%rbp)
	cmpq	$0, 696(%rbp)
	je	.L394
	movq	752(%rbp), %rax
	movl	176(%rax), %edx
	movq	696(%rbp), %rax
	movl	176(%rax), %eax
	cmpl	%eax, %edx
	jg	.L395
	movq	696(%rbp), %rdx
	movq	752(%rbp), %rax
	movq	%rdx, %r8
	movq	%rax, %rdx
	leaq	.LC172(%rip), %rcx
	call	_Z7d_printPKcz
	movl	$0, %eax
	jmp	.L403
.L395:
	movq	752(%rbp), %rdx
	movq	696(%rbp), %rax
	movq	%rdx, %r8
	movq	%rax, %rdx
	leaq	.LC173(%rip), %rcx
	call	_Z7d_printPKcz
.L394:
	movq	752(%rbp), %rax
	movzbl	173(%rax), %eax
	testb	%al, %al
	je	.L397
	movq	752(%rbp), %rax
	movq	%rax, %r8
	movq	752(%rbp), %rdx
	leaq	.LC174(%rip), %rcx
	call	_Z7d_printPKcz
	movq	752(%rbp), %rax
	movq	%rax, %rdx
	leaq	.LC175(%rip), %rcx
	call	_Z7d_printPKcz
	movq	my_trace(%rip), %rax
	movl	712(%rbp), %edx
	salq	$5, %rdx
	addq	%rdx, %rax
	leaq	67109104(%rax), %rdx
	movq	752(%rbp), %rax
	movq	%rax, 8(%rdx)
.L397:
	movl	$0, 732(%rbp)
	jmp	.L398
.L402:
	movq	752(%rbp), %rax
	movl	732(%rbp), %edx
	addq	$24, %rdx
	movl	8(%rax,%rdx,4), %eax
	testl	%eax, %eax
	jne	.L399
	leaq	.LC176(%rip), %rcx
	call	_Z7d_printPKcz
	movq	my_trace(%rip), %rax
	movq	752(%rbp), %rdx
	movq	%rdx, 68981976(%rax)
	movq	my_trace(%rip), %rax
	movl	$5, 67932936(%rax)
	jmp	.L400
.L399:
	movq	752(%rbp), %rax
	movl	732(%rbp), %edx
	addq	$24, %rdx
	movl	8(%rax,%rdx,4), %eax
	cmpl	$767, %eax
	ja	.L401
	movq	752(%rbp), %rax
	movl	732(%rbp), %edx
	addq	$24, %rdx
	movl	8(%rax,%rdx,4), %eax
	cmpl	$256, %eax
	jbe	.L401
	movq	my_trace(%rip), %rax
	movq	68981528(%rax), %rdx
	movq	752(%rbp), %rax
	movl	732(%rbp), %ecx
	addq	$24, %rcx
	movl	8(%rax,%rcx,4), %eax
	movq	%rdx, %r8
	movl	%eax, %edx
	leaq	.LC177(%rip), %rcx
	call	_Z7d_printPKcz
	movq	752(%rbp), %rax
	movl	732(%rbp), %edx
	addq	$24, %rdx
	movl	8(%rax,%rdx,4), %edx
	movq	my_trace(%rip), %rax
	movq	68981960(%rax), %r8
	movq	my_trace(%rip), %rax
	movl	68981968(%rax), %ecx
	leaq	-80(%rbp), %rax
	movl	%edx, 32(%rsp)
	movq	%r8, %r9
	movl	%ecx, %r8d
	leaq	.LC178(%rip), %rdx
	movq	%rax, %rcx
	call	sprintf
	leaq	-80(%rbp), %rax
	movq	%rax, %rcx
	call	_Z13add_to_bufferPc
.L401:
	movq	my_trace(%rip), %rax
	movq	752(%rbp), %rdx
	movl	732(%rbp), %ecx
	addq	$24, %rcx
	movl	8(%rdx,%rcx,4), %edx
	movl	%edx, %edx
	addq	$8754958, %rdx
	movq	8(%rax,%rdx,8), %rax
	movq	760(%rbp), %rdx
	movq	%rdx, %rcx
	call	*%rax
.L400:
	addl	$1, 732(%rbp)
.L398:
	movq	752(%rbp), %rax
	movl	168(%rax), %eax
	cmpl	732(%rbp), %eax
	ja	.L402
	movl	$0, %eax
.L403:
	addq	$864, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC179:
	.ascii "[handle_breakpoint]\12\0"
.LC180:
	.ascii "ER4 TID: 0x%08x\12\0"
	.align 8
.LC181:
	.ascii "ER5 TID1: 0x%08x instr_count: %d\12\0"
	.align 8
.LC182:
	.ascii "ER3 TID: 0x%08x, thread_no=0x%08x, locking_reaction=%p\12\0"
.LC183:
	.ascii "ER3 Reaction no %d: %p, %s\12\0"
	.align 8
.LC184:
	.ascii "Reaction not enabled, continuing\12\0"
	.align 8
.LC185:
	.ascii "ER3 Unlocking reaction lock with: %s in TID: 0x%08x\12\0"
.LC186:
	.ascii "ER6 TID: 0x%08x\12\0"
.LC187:
	.ascii "[handle_breakpoint ends]\12\0"
	.text
	.globl	_Z17handle_breakpointyPv
	.def	_Z17handle_breakpointyPv;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z17handle_breakpointyPv
_Z17handle_breakpointyPv:
.LFB2510:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$112, %rsp
	.seh_stackalloc	112
	.seh_endprologue
	movq	%rcx, 16(%rbp)
	movq	%rdx, 24(%rbp)
	leaq	.LC179(%rip), %rcx
	call	_Z7d_printPKcz
	movq	24(%rbp), %rax
	movq	%rax, -8(%rbp)
	movq	-8(%rbp), %rax
	movl	8(%rax), %eax
	movl	%eax, %edx
	leaq	.LC180(%rip), %rcx
	call	_Z7d_printPKcz
	movl	$-1, -24(%rbp)
	movl	$0, -28(%rbp)
	movl	$0, -12(%rbp)
	jmp	.L405
.L416:
	movq	my_trace(%rip), %rdx
	movl	-12(%rbp), %eax
	cltq
	imulq	$552, %rax, %rax
	addq	%rdx, %rax
	addq	$67650288, %rax
	movq	16(%rax), %rax
	cmpq	16(%rbp), %rax
	jne	.L406
	movq	my_trace(%rip), %rax
	movl	-12(%rbp), %edx
	movslq	%edx, %rdx
	imulq	$552, %rdx, %rdx
	addq	$67649776, %rdx
	addq	%rdx, %rax
	addq	$8, %rax
	movq	%rax, -40(%rbp)
	movq	$0, -48(%rbp)
	movq	-8(%rbp), %rax
	movl	8(%rax), %eax
	movl	%eax, -52(%rbp)
	movq	my_trace(%rip), %rax
	movl	-52(%rbp), %edx
	addq	$56, %rdx
	movl	(%rax,%rdx,4), %eax
	movl	%eax, -56(%rbp)
	cmpl	$-1, -56(%rbp)
	jne	.L407
	jmp	.L406
.L407:
	movq	my_trace(%rip), %rax
	movq	68981528(%rax), %rdx
	movq	-8(%rbp), %rax
	movl	8(%rax), %eax
	movq	%rdx, %r8
	movl	%eax, %edx
	leaq	.LC181(%rip), %rcx
	call	_Z7d_printPKcz
	movq	$0, -64(%rbp)
	movq	my_trace(%rip), %rax
	movl	-56(%rbp), %edx
	salq	$5, %rdx
	addq	%rdx, %rax
	addq	$67109104, %rax
	movq	8(%rax), %rax
	movq	%rax, -64(%rbp)
	movq	-64(%rbp), %rcx
	movl	-56(%rbp), %edx
	movl	-52(%rbp), %eax
	movq	%rcx, %r9
	movl	%edx, %r8d
	movl	%eax, %edx
	leaq	.LC182(%rip), %rcx
	call	_Z7d_printPKcz
	movl	$0, -16(%rbp)
	jmp	.L408
.L415:
	movq	-40(%rbp), %rax
	movq	536(%rax), %rax
	movl	-16(%rbp), %edx
	movslq	%edx, %rdx
	salq	$3, %rdx
	addq	%rdx, %rax
	movq	(%rax), %rax
	movq	%rax, -48(%rbp)
	movq	-48(%rbp), %rcx
	movq	-48(%rbp), %rdx
	movl	-16(%rbp), %eax
	movq	%rcx, %r9
	movq	%rdx, %r8
	movl	%eax, %edx
	leaq	.LC183(%rip), %rcx
	call	_Z7d_printPKcz
	movq	-48(%rbp), %rax
	movzbl	172(%rax), %eax
	testb	%al, %al
	jne	.L409
	leaq	.LC184(%rip), %rcx
	call	_Z7d_printPKcz
	jmp	.L410
.L409:
	cmpq	$0, -64(%rbp)
	je	.L411
	movq	$0, -72(%rbp)
	movl	$0, -20(%rbp)
	jmp	.L412
.L414:
	movl	-20(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	addq	%rax, %rax
	addq	%rdx, %rax
	movq	-64(%rbp), %rdx
	addq	%rdx, %rax
	addq	$3, %rax
	movq	%rax, %rcx
	call	_Z13find_reactionPc
	movq	%rax, -72(%rbp)
	movq	-72(%rbp), %rax
	cmpq	-48(%rbp), %rax
	jne	.L413
	movq	-48(%rbp), %rax
	movl	-52(%rbp), %edx
	movl	%edx, %r8d
	movq	%rax, %rdx
	leaq	.LC185(%rip), %rcx
	call	_Z7d_printPKcz
	movq	my_trace(%rip), %rax
	movl	-56(%rbp), %edx
	salq	$5, %rdx
	addq	%rdx, %rax
	addq	$67109104, %rax
	movq	$0, 8(%rax)
.L413:
	addl	$1, -20(%rbp)
.L412:
	movl	-20(%rbp), %edx
	movq	-64(%rbp), %rax
	movl	100(%rax), %eax
	cmpl	%eax, %edx
	jb	.L414
.L411:
	movq	24(%rbp), %rax
	movq	%rax, -8(%rbp)
	movq	-8(%rbp), %rax
	movl	8(%rax), %eax
	movl	%eax, %edx
	leaq	.LC186(%rip), %rcx
	call	_Z7d_printPKcz
	movq	-48(%rbp), %rcx
	movq	-48(%rbp), %rdx
	movl	-16(%rbp), %eax
	movq	%rcx, %r9
	movq	%rdx, %r8
	movl	%eax, %edx
	leaq	.LC183(%rip), %rcx
	call	_Z7d_printPKcz
	movq	24(%rbp), %rdx
	movq	-48(%rbp), %rax
	movq	%rax, %rcx
	call	_Z15handle_reactionP9REACTION_Pv
.L410:
	addl	$1, -16(%rbp)
.L408:
	movl	-16(%rbp), %edx
	movq	-40(%rbp), %rax
	movl	544(%rax), %eax
	cmpl	%eax, %edx
	jb	.L415
.L406:
	addl	$1, -12(%rbp)
.L405:
	movl	-12(%rbp), %eax
	movslq	%eax, %rdx
	movq	my_trace(%rip), %rax
	movq	68981576(%rax), %rax
	cmpq	%rax, %rdx
	jb	.L416
	movq	16(%rbp), %rcx
	call	_Z14del_breakpointy
	movq	-8(%rbp), %rax
	movl	8(%rax), %eax
	movl	%eax, %ecx
	call	_Z7dec_eipm
	leaq	.LC187(%rip), %rcx
	call	_Z7d_printPKcz
	movl	$0, %eax
	addq	$112, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC188:
	.ascii "[write_breakpoint]\12\0"
.LC189:
	.ascii "[write_breakpoint ends]\12\0"
	.text
	.globl	_Z16write_breakpointP11BREAKPOINT_
	.def	_Z16write_breakpointP11BREAKPOINT_;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z16write_breakpointP11BREAKPOINT_
_Z16write_breakpointP11BREAKPOINT_:
.LFB2511:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$80, %rsp
	.seh_stackalloc	80
	.seh_endprologue
	movq	%rcx, 16(%rbp)
	leaq	.LC188(%rip), %rcx
	call	_Z7d_printPKcz
	movb	$-52, -17(%rbp)
	movq	16(%rbp), %rax
	movq	(%rax), %rax
	movq	%rax, %rcx
	call	_Z16resolve_loc_descP24LOCATION_DESCRIPTOR_NEW_
	movq	%rax, -8(%rbp)
	movq	16(%rbp), %rax
	leaq	531(%rax), %r8
	movq	-8(%rbp), %rdx
	movq	my_trace(%rip), %rax
	movq	208(%rax), %rax
	leaq	-16(%rbp), %rcx
	movq	%rcx, 32(%rsp)
	movl	$1, %r9d
	movq	%rax, %rcx
	call	_Z11read_memoryPvS_S_yPy
	movq	-8(%rbp), %rdx
	movq	my_trace(%rip), %rax
	movq	208(%rax), %rax
	leaq	-17(%rbp), %r8
	leaq	-16(%rbp), %rcx
	movq	%rcx, 32(%rsp)
	movl	$1, %r9d
	movq	%rax, %rcx
	call	_Z12write_memoryPvS_S_yPy
	movq	-8(%rbp), %rdx
	movq	my_trace(%rip), %rax
	movq	208(%rax), %rax
	leaq	-17(%rbp), %r8
	leaq	-16(%rbp), %rcx
	movq	%rcx, 32(%rsp)
	movl	$1, %r9d
	movq	%rax, %rcx
	call	_Z11read_memoryPvS_S_yPy
	movq	16(%rbp), %rax
	movb	$1, 530(%rax)
	leaq	.LC189(%rip), %rcx
	call	_Z7d_printPKcz
	movl	$0, %eax
	addq	$80, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC190:
	.ascii "[unwrite_breakpoint]\12\0"
.LC191:
	.ascii "[unwrite_breakpoint ends]\12\0"
	.text
	.globl	_Z18unwrite_breakpointP11BREAKPOINT_
	.def	_Z18unwrite_breakpointP11BREAKPOINT_;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z18unwrite_breakpointP11BREAKPOINT_
_Z18unwrite_breakpointP11BREAKPOINT_:
.LFB2512:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$80, %rsp
	.seh_stackalloc	80
	.seh_endprologue
	movq	%rcx, 16(%rbp)
	leaq	.LC190(%rip), %rcx
	call	_Z7d_printPKcz
	movb	$-52, -9(%rbp)
	movq	16(%rbp), %rax
	movq	520(%rax), %rax
	movq	%rax, -8(%rbp)
	movq	16(%rbp), %rax
	leaq	531(%rax), %r8
	movq	-8(%rbp), %rdx
	movq	my_trace(%rip), %rax
	movq	208(%rax), %rax
	leaq	-24(%rbp), %rcx
	movq	%rcx, 32(%rsp)
	movl	$1, %r9d
	movq	%rax, %rcx
	call	_Z12write_memoryPvS_S_yPy
	movq	-8(%rbp), %rdx
	movq	my_trace(%rip), %rax
	movq	208(%rax), %rax
	leaq	-9(%rbp), %r8
	leaq	-24(%rbp), %rcx
	movq	%rcx, 32(%rsp)
	movl	$1, %r9d
	movq	%rax, %rcx
	call	_Z11read_memoryPvS_S_yPy
	movq	16(%rbp), %rax
	movb	$0, 530(%rax)
	leaq	.LC191(%rip), %rcx
	call	_Z7d_printPKcz
	movl	$0, %eax
	addq	$80, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC192:
	.ascii "[print_loc_desc]\12\0"
.LC193:
	.ascii "%s \0"
.LC194:
	.ascii "[print_loc_desc ends]\12\0"
	.text
	.globl	_Z14print_loc_descP24LOCATION_DESCRIPTOR_NEW_
	.def	_Z14print_loc_descP24LOCATION_DESCRIPTOR_NEW_;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z14print_loc_descP24LOCATION_DESCRIPTOR_NEW_
_Z14print_loc_descP24LOCATION_DESCRIPTOR_NEW_:
.LFB2513:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$32, %rsp
	.seh_stackalloc	32
	.seh_endprologue
	movq	%rcx, 16(%rbp)
	leaq	.LC192(%rip), %rcx
	call	_Z7d_printPKcz
	cmpq	$0, 16(%rbp)
	jne	.L423
	movl	$0, %eax
	jmp	.L424
.L423:
	movq	16(%rbp), %rax
	addq	$16, %rax
	movq	%rax, %rdx
	leaq	.LC193(%rip), %rcx
	call	_Z7d_printPKcz
	movq	16(%rbp), %rax
	movq	(%rax), %rax
	movq	%rax, %rcx
	call	_Z14print_loc_descP24LOCATION_DESCRIPTOR_NEW_
	movq	16(%rbp), %rax
	movq	8(%rax), %rax
	movq	%rax, %rcx
	call	_Z14print_loc_descP24LOCATION_DESCRIPTOR_NEW_
	leaq	.LC194(%rip), %rcx
	call	_Z7d_printPKcz
	movl	$0, %eax
.L424:
	addq	$32, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC195:
	.ascii "[print_loc_desc_rev]\12\0"
.LC196:
	.ascii "%s\12\0"
.LC197:
	.ascii "[print_loc_desc_rev ends]\12\0"
	.text
	.globl	_Z18print_loc_desc_revP24LOCATION_DESCRIPTOR_NEW_
	.def	_Z18print_loc_desc_revP24LOCATION_DESCRIPTOR_NEW_;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z18print_loc_desc_revP24LOCATION_DESCRIPTOR_NEW_
_Z18print_loc_desc_revP24LOCATION_DESCRIPTOR_NEW_:
.LFB2514:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$32, %rsp
	.seh_stackalloc	32
	.seh_endprologue
	movq	%rcx, 16(%rbp)
	leaq	.LC195(%rip), %rcx
	call	_Z7d_printPKcz
	cmpq	$0, 16(%rbp)
	jne	.L426
	movl	$0, %eax
	jmp	.L427
.L426:
	movq	16(%rbp), %rax
	movq	(%rax), %rax
	movq	%rax, %rcx
	call	_Z18print_loc_desc_revP24LOCATION_DESCRIPTOR_NEW_
	movq	16(%rbp), %rax
	movq	8(%rax), %rax
	movq	%rax, %rcx
	call	_Z18print_loc_desc_revP24LOCATION_DESCRIPTOR_NEW_
	movq	16(%rbp), %rax
	addq	$16, %rax
	movq	%rax, %rdx
	leaq	.LC196(%rip), %rcx
	call	_Z7d_printPKcz
	leaq	.LC197(%rip), %rcx
	call	_Z7d_printPKcz
	movl	$0, %eax
.L427:
	addq	$32, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.globl	_Z7findanyPcS_
	.def	_Z7findanyPcS_;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z7findanyPcS_
_Z7findanyPcS_:
.LFB2515:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$48, %rsp
	.seh_stackalloc	48
	.seh_endprologue
	movq	%rcx, 16(%rbp)
	movq	%rdx, 24(%rbp)
	movq	24(%rbp), %rax
	movq	%rax, %rdx
	movq	16(%rbp), %rcx
	call	strcspn
	movq	%rax, %rdx
	movq	16(%rbp), %rax
	addq	%rdx, %rax
	movq	%rax, -8(%rbp)
	movq	-8(%rbp), %rax
	movzbl	(%rax), %eax
	testb	%al, %al
	je	.L429
	movq	-8(%rbp), %rax
	jmp	.L430
.L429:
	movl	$0, %eax
.L430:
	addq	$48, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.globl	_Z5paintPcj
	.def	_Z5paintPcj;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z5paintPcj
_Z5paintPcj:
.LFB2516:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$48, %rsp
	.seh_stackalloc	48
	.seh_endprologue
	movq	%rcx, 16(%rbp)
	movl	%edx, 24(%rbp)
	movl	$0, -8(%rbp)
	movb	$0, -9(%rbp)
	movl	$0, -4(%rbp)
	jmp	.L433
.L442:
	movl	-4(%rbp), %edx
	movq	16(%rbp), %rax
	addq	%rdx, %rax
	movzbl	(%rax), %eax
	cmpb	$91, %al
	jne	.L434
	addl	$1, -8(%rbp)
	cmpl	$0, -8(%rbp)
	je	.L435
	movb	$1, -9(%rbp)
	jmp	.L436
.L435:
	movb	$0, -9(%rbp)
.L436:
	cmpb	$0, -9(%rbp)
	je	.L437
	movl	-4(%rbp), %edx
	movq	16(%rbp), %rax
	addq	%rdx, %rax
	movl	-4(%rbp), %ecx
	movq	16(%rbp), %rdx
	addq	%rcx, %rdx
	movzbl	(%rdx), %edx
	addl	$3, %edx
	movb	%dl, (%rax)
	jmp	.L437
.L434:
	movl	-4(%rbp), %edx
	movq	16(%rbp), %rax
	addq	%rdx, %rax
	movzbl	(%rax), %eax
	cmpb	$93, %al
	jne	.L438
	cmpb	$0, -9(%rbp)
	je	.L439
	movl	-4(%rbp), %edx
	movq	16(%rbp), %rax
	addq	%rdx, %rax
	movl	-4(%rbp), %ecx
	movq	16(%rbp), %rdx
	addq	%rcx, %rdx
	movzbl	(%rdx), %edx
	addl	$3, %edx
	movb	%dl, (%rax)
.L439:
	subl	$1, -8(%rbp)
	cmpl	$0, -8(%rbp)
	je	.L440
	movb	$1, -9(%rbp)
	jmp	.L437
.L440:
	movb	$0, -9(%rbp)
	jmp	.L437
.L438:
	cmpb	$0, -9(%rbp)
	je	.L437
	movl	-4(%rbp), %edx
	movq	16(%rbp), %rax
	addq	%rdx, %rax
	movl	-4(%rbp), %ecx
	movq	16(%rbp), %rdx
	addq	%rcx, %rdx
	movzbl	(%rdx), %edx
	addl	$3, %edx
	movb	%dl, (%rax)
.L437:
	movl	-4(%rbp), %edx
	movq	16(%rbp), %rax
	addq	%rdx, %rax
	movzbl	(%rax), %eax
	movsbl	%al, %eax
	movl	%eax, %ecx
	call	putchar
	addl	$1, -4(%rbp)
.L433:
	movl	-4(%rbp), %eax
	cmpl	24(%rbp), %eax
	jb	.L442
	movl	$10, %ecx
	call	putchar
	movl	$0, %eax
	addq	$48, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.globl	_Z7unpaintPcj
	.def	_Z7unpaintPcj;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z7unpaintPcj
_Z7unpaintPcj:
.LFB2517:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$48, %rsp
	.seh_stackalloc	48
	.seh_endprologue
	movq	%rcx, 16(%rbp)
	movl	%edx, 24(%rbp)
	movl	$0, -8(%rbp)
	movb	$0, -9(%rbp)
	movl	$0, -4(%rbp)
	jmp	.L445
.L454:
	movl	-4(%rbp), %edx
	movq	16(%rbp), %rax
	addq	%rdx, %rax
	movzbl	(%rax), %eax
	cmpb	$94, %al
	jne	.L446
	addl	$1, -8(%rbp)
	cmpl	$0, -8(%rbp)
	je	.L447
	movb	$1, -9(%rbp)
	jmp	.L448
.L447:
	movb	$0, -9(%rbp)
.L448:
	cmpb	$0, -9(%rbp)
	je	.L449
	movl	-4(%rbp), %edx
	movq	16(%rbp), %rax
	addq	%rdx, %rax
	movl	-4(%rbp), %ecx
	movq	16(%rbp), %rdx
	addq	%rcx, %rdx
	movzbl	(%rdx), %edx
	subl	$3, %edx
	movb	%dl, (%rax)
	jmp	.L449
.L446:
	movl	-4(%rbp), %edx
	movq	16(%rbp), %rax
	addq	%rdx, %rax
	movzbl	(%rax), %eax
	cmpb	$96, %al
	jne	.L450
	cmpb	$0, -9(%rbp)
	je	.L451
	movl	-4(%rbp), %edx
	movq	16(%rbp), %rax
	addq	%rdx, %rax
	movl	-4(%rbp), %ecx
	movq	16(%rbp), %rdx
	addq	%rcx, %rdx
	movzbl	(%rdx), %edx
	subl	$3, %edx
	movb	%dl, (%rax)
.L451:
	subl	$1, -8(%rbp)
	cmpl	$0, -8(%rbp)
	je	.L452
	movb	$1, -9(%rbp)
	jmp	.L449
.L452:
	movb	$0, -9(%rbp)
	jmp	.L449
.L450:
	cmpb	$0, -9(%rbp)
	je	.L449
	movl	-4(%rbp), %edx
	movq	16(%rbp), %rax
	addq	%rdx, %rax
	movl	-4(%rbp), %ecx
	movq	16(%rbp), %rdx
	addq	%rcx, %rdx
	movzbl	(%rdx), %edx
	subl	$3, %edx
	movb	%dl, (%rax)
.L449:
	movl	-4(%rbp), %edx
	movq	16(%rbp), %rax
	addq	%rdx, %rax
	movzbl	(%rax), %eax
	movsbl	%al, %eax
	movl	%eax, %ecx
	call	putchar
	addl	$1, -4(%rbp)
.L445:
	movl	-4(%rbp), %eax
	cmpl	24(%rbp), %eax
	jb	.L454
	movl	$10, %ecx
	call	putchar
	movl	$0, %eax
	addq	$48, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC198:
	.ascii "[+-\0"
.LC199:
	.ascii "[\0"
.LC200:
	.ascii "-\0"
.LC201:
	.ascii "+\0"
.LC202:
	.ascii "Reading register ESP: 0x%08x\12\0"
.LC203:
	.ascii "Reading register EAX: 0x%08x\12\0"
.LC204:
	.ascii "Reading register EBX: 0x%08x\12\0"
.LC205:
	.ascii "Reading register ECX: 0x%08x\12\0"
.LC206:
	.ascii "Reading register EDX: 0x%08x\12\0"
.LC207:
	.ascii "Reading register ESI: 0x%08x\12\0"
.LC208:
	.ascii "Reading register EDI: 0x%08x\12\0"
.LC209:
	.ascii "EBP\0"
.LC210:
	.ascii "Reading register EBP: 0x%08x\12\0"
.LC211:
	.ascii "Reading register EIP: 0x%08x\12\0"
	.text
	.globl	_Z16resolve_loc_descP24LOCATION_DESCRIPTOR_NEW_
	.def	_Z16resolve_loc_descP24LOCATION_DESCRIPTOR_NEW_;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z16resolve_loc_descP24LOCATION_DESCRIPTOR_NEW_
_Z16resolve_loc_descP24LOCATION_DESCRIPTOR_NEW_:
.LFB2518:
	pushq	%rbp
	.seh_pushreg	%rbp
	subq	$1296, %rsp
	.seh_stackalloc	1296
	leaq	128(%rsp), %rbp
	.seh_setframe	%rbp, 128
	.seh_endprologue
	movq	%rcx, 1184(%rbp)
	cmpq	$0, 1184(%rbp)
	jne	.L457
	movq	$-1, %rax
	jmp	.L458
.L457:
	movq	1184(%rbp), %rax
	addq	$16, %rax
	leaq	.LC198(%rip), %rdx
	movq	%rax, %rcx
	call	_Z7findanyPcS_
	testq	%rax, %rax
	setne	%al
	testb	%al, %al
	je	.L459
	movq	1184(%rbp), %rax
	addq	$16, %rax
	leaq	.LC199(%rip), %rdx
	movq	%rax, %rcx
	call	strcmp
	testl	%eax, %eax
	jne	.L460
	movq	1184(%rbp), %rax
	movq	(%rax), %rax
	movq	%rax, %rcx
	call	_Z16resolve_loc_descP24LOCATION_DESCRIPTOR_NEW_
	movq	%rax, 1152(%rbp)
	cmpq	$-1, 1152(%rbp)
	jne	.L461
	movq	$-1, %rax
	jmp	.L458
.L461:
	movq	1152(%rbp), %rax
	movq	%rax, %rcx
	call	_Z10read_dwordy
	movl	%eax, 1148(%rbp)
	movl	1148(%rbp), %eax
	jmp	.L458
.L460:
	movq	1184(%rbp), %rax
	addq	$16, %rax
	leaq	.LC200(%rip), %rdx
	movq	%rax, %rcx
	call	strcmp
	testl	%eax, %eax
	jne	.L462
	movq	1184(%rbp), %rax
	movq	(%rax), %rax
	movq	%rax, %rcx
	call	_Z16resolve_loc_descP24LOCATION_DESCRIPTOR_NEW_
	movq	%rax, 1152(%rbp)
	movq	1184(%rbp), %rax
	movq	8(%rax), %rax
	movq	%rax, %rcx
	call	_Z16resolve_loc_descP24LOCATION_DESCRIPTOR_NEW_
	movq	%rax, 1136(%rbp)
	cmpq	$-1, 1152(%rbp)
	jne	.L463
	movq	$-1, %rax
	jmp	.L458
.L463:
	cmpq	$-1, 1136(%rbp)
	jne	.L464
	movq	$-1, %rax
	jmp	.L458
.L464:
	movq	1152(%rbp), %rax
	subq	1136(%rbp), %rax
	movq	%rax, 1160(%rbp)
	jmp	.L465
.L462:
	movq	1184(%rbp), %rax
	addq	$16, %rax
	leaq	.LC201(%rip), %rdx
	movq	%rax, %rcx
	call	strcmp
	testl	%eax, %eax
	jne	.L465
	movq	1184(%rbp), %rax
	movq	(%rax), %rax
	movq	%rax, %rcx
	call	_Z16resolve_loc_descP24LOCATION_DESCRIPTOR_NEW_
	movq	%rax, 1152(%rbp)
	movq	1184(%rbp), %rax
	movq	8(%rax), %rax
	movq	%rax, %rcx
	call	_Z16resolve_loc_descP24LOCATION_DESCRIPTOR_NEW_
	movq	%rax, 1136(%rbp)
	cmpq	$-1, 1152(%rbp)
	jne	.L466
	movq	$-1, %rax
	jmp	.L458
.L466:
	cmpq	$-1, 1136(%rbp)
	jne	.L467
	movq	$-1, %rax
	jmp	.L458
.L467:
	movq	1152(%rbp), %rdx
	movq	1136(%rbp), %rax
	addq	%rdx, %rax
	movq	%rax, 1160(%rbp)
.L465:
	jmp	.L468
.L459:
	movq	1184(%rbp), %rax
	addq	$16, %rax
	leaq	.LC117(%rip), %rdx
	movq	%rax, %rcx
	call	strcmp
	testl	%eax, %eax
	jne	.L469
	movq	my_trace(%rip), %rax
	movq	152(%rax), %rax
	movq	%rax, 1160(%rbp)
	cmpq	$0, 1160(%rbp)
	jne	.L468
	movq	$-1, %rax
	jmp	.L458
.L469:
	movq	1184(%rbp), %rax
	movzbl	16(%rax), %eax
	cmpb	$48, %al
	jne	.L470
	movq	1184(%rbp), %rax
	movzbl	17(%rax), %eax
	cmpb	$120, %al
	jne	.L470
	movq	1184(%rbp), %rax
	addq	$16, %rax
	movl	$16, %r8d
	movl	$0, %edx
	movq	%rax, %rcx
	call	strtoul
	movl	%eax, %eax
	movq	%rax, 1160(%rbp)
	jmp	.L468
.L470:
	movq	1184(%rbp), %rax
	addq	$16, %rax
	leaq	.LC34(%rip), %rdx
	movq	%rax, %rcx
	call	strcmp
	testl	%eax, %eax
	jne	.L471
	leaq	-96(%rbp), %rax
	movq	%rax, %rdx
	movl	$0, %ecx
	call	_Z12read_contextyP8_CONTEXT
	movq	56(%rbp), %rax
	movq	%rax, 1160(%rbp)
	movq	1160(%rbp), %rax
	movq	%rax, %rdx
	leaq	.LC202(%rip), %rcx
	call	_Z7d_printPKcz
	jmp	.L468
.L471:
	movq	1184(%rbp), %rax
	addq	$16, %rax
	leaq	.LC28(%rip), %rdx
	movq	%rax, %rcx
	call	strcmp
	testl	%eax, %eax
	jne	.L472
	leaq	-96(%rbp), %rax
	movq	%rax, %rdx
	movl	$0, %ecx
	call	_Z12read_contextyP8_CONTEXT
	movq	24(%rbp), %rax
	movq	%rax, 1160(%rbp)
	movq	1160(%rbp), %rax
	movq	%rax, %rdx
	leaq	.LC203(%rip), %rcx
	call	_Z7d_printPKcz
	jmp	.L468
.L472:
	movq	1184(%rbp), %rax
	addq	$16, %rax
	leaq	.LC29(%rip), %rdx
	movq	%rax, %rcx
	call	strcmp
	testl	%eax, %eax
	jne	.L473
	leaq	-96(%rbp), %rax
	movq	%rax, %rdx
	movl	$0, %ecx
	call	_Z12read_contextyP8_CONTEXT
	movq	48(%rbp), %rax
	movq	%rax, 1160(%rbp)
	movq	1160(%rbp), %rax
	movq	%rax, %rdx
	leaq	.LC204(%rip), %rcx
	call	_Z7d_printPKcz
	jmp	.L468
.L473:
	movq	1184(%rbp), %rax
	addq	$16, %rax
	leaq	.LC30(%rip), %rdx
	movq	%rax, %rcx
	call	strcmp
	testl	%eax, %eax
	jne	.L474
	leaq	-96(%rbp), %rax
	movq	%rax, %rdx
	movl	$0, %ecx
	call	_Z12read_contextyP8_CONTEXT
	movq	32(%rbp), %rax
	movq	%rax, 1160(%rbp)
	movq	1160(%rbp), %rax
	movq	%rax, %rdx
	leaq	.LC205(%rip), %rcx
	call	_Z7d_printPKcz
	jmp	.L468
.L474:
	movq	1184(%rbp), %rax
	addq	$16, %rax
	leaq	.LC31(%rip), %rdx
	movq	%rax, %rcx
	call	strcmp
	testl	%eax, %eax
	jne	.L475
	leaq	-96(%rbp), %rax
	movq	%rax, %rdx
	movl	$0, %ecx
	call	_Z12read_contextyP8_CONTEXT
	movq	40(%rbp), %rax
	movq	%rax, 1160(%rbp)
	movq	1160(%rbp), %rax
	movq	%rax, %rdx
	leaq	.LC206(%rip), %rcx
	call	_Z7d_printPKcz
	jmp	.L468
.L475:
	movq	1184(%rbp), %rax
	addq	$16, %rax
	leaq	.LC33(%rip), %rdx
	movq	%rax, %rcx
	call	strcmp
	testl	%eax, %eax
	jne	.L476
	leaq	-96(%rbp), %rax
	movq	%rax, %rdx
	movl	$0, %ecx
	call	_Z12read_contextyP8_CONTEXT
	movq	72(%rbp), %rax
	movq	%rax, 1160(%rbp)
	movq	1160(%rbp), %rax
	movq	%rax, %rdx
	leaq	.LC207(%rip), %rcx
	call	_Z7d_printPKcz
	jmp	.L468
.L476:
	movq	1184(%rbp), %rax
	addq	$16, %rax
	leaq	.LC32(%rip), %rdx
	movq	%rax, %rcx
	call	strcmp
	testl	%eax, %eax
	jne	.L477
	leaq	-96(%rbp), %rax
	movq	%rax, %rdx
	movl	$0, %ecx
	call	_Z12read_contextyP8_CONTEXT
	movq	80(%rbp), %rax
	movq	%rax, 1160(%rbp)
	movq	1160(%rbp), %rax
	movq	%rax, %rdx
	leaq	.LC208(%rip), %rcx
	call	_Z7d_printPKcz
	jmp	.L468
.L477:
	movq	1184(%rbp), %rax
	addq	$16, %rax
	leaq	.LC209(%rip), %rdx
	movq	%rax, %rcx
	call	strcmp
	testl	%eax, %eax
	jne	.L478
	leaq	-96(%rbp), %rax
	movq	%rax, %rdx
	movl	$0, %ecx
	call	_Z12read_contextyP8_CONTEXT
	movq	64(%rbp), %rax
	movq	%rax, 1160(%rbp)
	movq	1160(%rbp), %rax
	movq	%rax, %rdx
	leaq	.LC210(%rip), %rcx
	call	_Z7d_printPKcz
	jmp	.L468
.L478:
	movq	1184(%rbp), %rax
	addq	$16, %rax
	leaq	.LC35(%rip), %rdx
	movq	%rax, %rcx
	call	strcmp
	testl	%eax, %eax
	jne	.L479
	leaq	-96(%rbp), %rax
	movq	%rax, %rdx
	movl	$0, %ecx
	call	_Z12read_contextyP8_CONTEXT
	movq	152(%rbp), %rax
	movq	%rax, 1160(%rbp)
	movq	1160(%rbp), %rax
	movq	%rax, %rdx
	leaq	.LC211(%rip), %rcx
	call	_Z7d_printPKcz
	jmp	.L468
.L479:
	movq	1184(%rbp), %rax
	addq	$16, %rax
	movq	%rax, %rcx
	call	_Z8find_libPc
	movq	%rax, 1160(%rbp)
	cmpq	$0, 1160(%rbp)
	jne	.L468
	cmpq	$0, 1160(%rbp)
	jne	.L468
	movq	$-1, 1160(%rbp)
.L468:
	movq	1160(%rbp), %rax
.L458:
	addq	$1296, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC212:
	.ascii "Out of memory\12\0"
.LC213:
	.ascii "Creating new descriptor node\12\0"
.LC214:
	.ascii "+-\0"
	.align 8
.LC215:
	.ascii "No operators found, assuming leaf: %s\12\0"
.LC216:
	.ascii "Found operator: %s\12\0"
	.text
	.globl	_Z19parse_location_descPc
	.def	_Z19parse_location_descPc;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z19parse_location_descPc
_Z19parse_location_descPc:
.LFB2519:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$48, %rsp
	.seh_stackalloc	48
	.seh_endprologue
	movq	%rcx, 16(%rbp)
	movl	$528, %ecx
	call	malloc
	movq	%rax, -8(%rbp)
	movq	-8(%rbp), %rax
	movl	$528, %r8d
	movl	$0, %edx
	movq	%rax, %rcx
	call	memset
	movq	-8(%rbp), %rax
	movq	$0, (%rax)
	movq	-8(%rbp), %rax
	movq	$0, 8(%rax)
	cmpq	$0, -8(%rbp)
	jne	.L481
	leaq	.LC212(%rip), %rcx
	call	_Z7d_printPKcz
.L481:
	leaq	.LC213(%rip), %rcx
	call	_Z7d_printPKcz
	movq	16(%rbp), %rcx
	call	strlen
	movl	%eax, %edx
	movq	16(%rbp), %rcx
	call	_Z5paintPcj
	leaq	.LC214(%rip), %rdx
	movq	16(%rbp), %rcx
	call	_Z7findanyPcS_
	movq	%rax, -16(%rbp)
	cmpq	$0, -16(%rbp)
	jne	.L482
	movq	16(%rbp), %rcx
	call	strlen
	movl	%eax, %edx
	movq	16(%rbp), %rcx
	call	_Z7unpaintPcj
	leaq	.LC199(%rip), %rdx
	movq	16(%rbp), %rcx
	call	_Z7findanyPcS_
	movq	%rax, -16(%rbp)
	cmpq	$0, -16(%rbp)
	setne	%al
	testb	%al, %al
	je	.L483
	movq	-8(%rbp), %rax
	addq	$16, %rax
	movw	$91, (%rax)
	movq	-16(%rbp), %rax
	movb	$0, (%rax)
	movq	-16(%rbp), %rax
	addq	$1, %rax
	movq	%rax, %rcx
	call	_Z19parse_location_descPc
	movq	%rax, %rdx
	movq	-8(%rbp), %rax
	movq	%rdx, (%rax)
	movq	-8(%rbp), %rax
	movq	$0, 8(%rax)
	jmp	.L485
.L483:
	movq	16(%rbp), %rdx
	leaq	.LC215(%rip), %rcx
	call	_Z7d_printPKcz
	movq	-8(%rbp), %rax
	addq	$16, %rax
	movq	16(%rbp), %rdx
	movq	%rax, %rcx
	call	strcpy
	jmp	.L485
.L482:
	movq	16(%rbp), %rcx
	call	strlen
	movl	%eax, %edx
	movq	16(%rbp), %rcx
	call	_Z7unpaintPcj
	movq	-16(%rbp), %rax
	movq	%rax, %rdx
	leaq	.LC216(%rip), %rcx
	call	_Z7d_printPKcz
	movq	-16(%rbp), %rax
	movzbl	(%rax), %eax
	cmpb	$43, %al
	jne	.L486
	movq	-8(%rbp), %rax
	addq	$16, %rax
	movw	$43, (%rax)
	movq	-16(%rbp), %rax
	movb	$0, (%rax)
	movq	16(%rbp), %rcx
	call	_Z19parse_location_descPc
	movq	%rax, %rdx
	movq	-8(%rbp), %rax
	movq	%rdx, (%rax)
	movq	-16(%rbp), %rax
	addq	$1, %rax
	movq	%rax, %rcx
	call	_Z19parse_location_descPc
	movq	%rax, %rdx
	movq	-8(%rbp), %rax
	movq	%rdx, 8(%rax)
	jmp	.L485
.L486:
	movq	-16(%rbp), %rax
	movzbl	(%rax), %eax
	cmpb	$45, %al
	jne	.L485
	movq	-8(%rbp), %rax
	addq	$16, %rax
	movw	$45, (%rax)
	movq	-16(%rbp), %rax
	movb	$0, (%rax)
	movq	16(%rbp), %rcx
	call	_Z19parse_location_descPc
	movq	%rax, %rdx
	movq	-8(%rbp), %rax
	movq	%rdx, (%rax)
	movq	-16(%rbp), %rax
	addq	$1, %rax
	movq	%rax, %rcx
	call	_Z19parse_location_descPc
	movq	%rax, %rdx
	movq	-8(%rbp), %rax
	movq	%rdx, 8(%rax)
.L485:
	movq	-8(%rbp), %rax
	addq	$48, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC217:
	.ascii "Trying to resolve BP addr\12\0"
	.align 8
.LC218:
	.ascii "Unable to resolve BP address, will not be updated at this time\12\0"
.LC219:
	.ascii "BP written @ 0x%08x\12\0"
.LC220:
	.ascii "[update_breakpoint ends]\12\0"
	.text
	.globl	_Z17update_breakpointP11BREAKPOINT_
	.def	_Z17update_breakpointP11BREAKPOINT_;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z17update_breakpointP11BREAKPOINT_
_Z17update_breakpointP11BREAKPOINT_:
.LFB2520:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$48, %rsp
	.seh_stackalloc	48
	.seh_endprologue
	movq	%rcx, 16(%rbp)
	leaq	.LC217(%rip), %rcx
	call	_Z7d_printPKcz
	movq	16(%rbp), %rax
	movq	(%rax), %rax
	movq	%rax, %rcx
	call	_Z16resolve_loc_descP24LOCATION_DESCRIPTOR_NEW_
	movq	%rax, -16(%rbp)
	movq	16(%rbp), %rax
	movq	-16(%rbp), %rdx
	movq	%rdx, 520(%rax)
	cmpq	$-1, -16(%rbp)
	jne	.L489
	leaq	.LC218(%rip), %rcx
	call	_Z7d_printPKcz
	movl	$0, %eax
	jmp	.L490
.L489:
	movq	16(%rbp), %rax
	movzbl	529(%rax), %eax
	testb	%al, %al
	je	.L491
	movq	16(%rbp), %rax
	movzbl	530(%rax), %eax
	testb	%al, %al
	jne	.L492
	movq	16(%rbp), %rcx
	call	_Z16write_breakpointP11BREAKPOINT_
	movq	16(%rbp), %rax
	movb	$1, 530(%rax)
.L492:
	movq	-16(%rbp), %rax
	movq	%rax, %rdx
	leaq	.LC219(%rip), %rcx
	call	_Z7d_printPKcz
	movl	$1, -4(%rbp)
	jmp	.L493
.L491:
	movq	16(%rbp), %rax
	movzbl	530(%rax), %eax
	testb	%al, %al
	je	.L494
	movq	16(%rbp), %rcx
	call	_Z18unwrite_breakpointP11BREAKPOINT_
	movq	16(%rbp), %rax
	movb	$0, 530(%rax)
.L494:
	movl	$0, -4(%rbp)
.L493:
	leaq	.LC220(%rip), %rcx
	call	_Z7d_printPKcz
	movl	-4(%rbp), %eax
.L490:
	addq	$48, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC221:
	.ascii "[add_breakpoint]\12\0"
.LC222:
	.ascii "Found!\12\0"
.LC223:
	.ascii "Creating new bp\12\0"
	.align 8
.LC224:
	.ascii "Failed creating new bp- out of memory\12\0"
.LC225:
	.ascii "Created new bp\12\0"
	.align 8
.LC226:
	.ascii "Attempt to parse location string: %s\12\0"
	.align 8
.LC227:
	.ascii "Bp created. Current number of breakpoints: %d\12\0"
	.align 8
.LC228:
	.ascii "Current reaction count for this BP: %d\12\0"
.LC229:
	.ascii "Assigned new reaction\12\0"
.LC230:
	.ascii "[add_breakpoint ends]\12\0"
	.text
	.globl	_Z14add_breakpointPcP9REACTION_
	.def	_Z14add_breakpointPcP9REACTION_;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z14add_breakpointPcP9REACTION_
_Z14add_breakpointPcP9REACTION_:
.LFB2521:
	pushq	%rbp
	.seh_pushreg	%rbp
	pushq	%rbx
	.seh_pushreg	%rbx
	subq	$56, %rsp
	.seh_stackalloc	56
	leaq	128(%rsp), %rbp
	.seh_setframe	%rbp, 128
	.seh_endprologue
	movq	%rcx, -48(%rbp)
	movq	%rdx, -40(%rbp)
	leaq	.LC221(%rip), %rcx
	call	_Z7d_printPKcz
	movl	$-1, -88(%rbp)
	movl	$0, -84(%rbp)
	jmp	.L496
.L499:
	movq	my_trace(%rip), %rax
	movl	-84(%rbp), %edx
	movslq	%edx, %rdx
	imulq	$552, %rdx, %rdx
	addq	$67649776, %rdx
	addq	%rdx, %rax
	addq	$16, %rax
	movq	-48(%rbp), %rdx
	movq	%rax, %rcx
	call	strcmp
	testl	%eax, %eax
	jne	.L497
	leaq	.LC222(%rip), %rcx
	call	_Z7d_printPKcz
	movl	-84(%rbp), %eax
	movl	%eax, -88(%rbp)
	jmp	.L498
.L497:
	addl	$1, -84(%rbp)
.L496:
	movl	-84(%rbp), %eax
	movslq	%eax, %rdx
	movq	my_trace(%rip), %rax
	movq	68981576(%rax), %rax
	cmpq	%rax, %rdx
	jb	.L499
.L498:
	cmpl	$-1, -88(%rbp)
	jne	.L500
	movq	my_trace(%rip), %rax
	movq	68981576(%rax), %rax
	movl	%eax, -88(%rbp)
	leaq	.LC223(%rip), %rcx
	call	_Z7d_printPKcz
	movq	my_trace(%rip), %rbx
	movl	$32768, %eax
	movq	%rax, %rcx
	call	malloc
	movq	%rax, %rdx
	movl	-88(%rbp), %eax
	cltq
	imulq	$552, %rax, %rax
	addq	%rbx, %rax
	addq	$67650304, %rax
	movq	%rdx, 16(%rax)
	movq	my_trace(%rip), %rdx
	movl	-88(%rbp), %eax
	cltq
	imulq	$552, %rax, %rax
	addq	%rdx, %rax
	addq	$67650304, %rax
	movq	16(%rax), %rax
	testq	%rax, %rax
	jne	.L501
	leaq	.LC224(%rip), %rcx
	call	_Z7d_printPKcz
.L501:
	movq	my_trace(%rip), %rdx
	movl	-88(%rbp), %eax
	cltq
	imulq	$552, %rax, %rax
	addq	%rdx, %rax
	addq	$67650304, %rax
	movb	$1, 9(%rax)
	movq	my_trace(%rip), %rdx
	movl	-88(%rbp), %eax
	cltq
	imulq	$552, %rax, %rax
	addq	%rdx, %rax
	addq	$67650304, %rax
	movb	$0, 10(%rax)
	leaq	.LC225(%rip), %rcx
	call	_Z7d_printPKcz
	movq	my_trace(%rip), %rax
	movl	-88(%rbp), %edx
	movslq	%edx, %rdx
	imulq	$552, %rdx, %rdx
	addq	$67649776, %rdx
	addq	%rdx, %rax
	addq	$16, %rax
	movq	-48(%rbp), %rdx
	movq	%rax, %rcx
	call	strcpy
	movq	-48(%rbp), %rdx
	leaq	.LC226(%rip), %rcx
	call	_Z7d_printPKcz
	movq	my_trace(%rip), %rbx
	movq	-48(%rbp), %rcx
	call	_Z19parse_location_descPc
	movq	%rax, %rdx
	movl	-88(%rbp), %eax
	cltq
	imulq	$552, %rax, %rax
	addq	%rbx, %rax
	addq	$67649776, %rax
	movq	%rdx, 8(%rax)
	movq	my_trace(%rip), %rax
	movq	68981576(%rax), %rdx
	addq	$1, %rdx
	movq	%rdx, 68981576(%rax)
.L500:
	movq	my_trace(%rip), %rax
	movq	68981576(%rax), %rax
	movq	%rax, %rdx
	leaq	.LC227(%rip), %rcx
	call	_Z7d_printPKcz
	movq	my_trace(%rip), %rdx
	movl	-88(%rbp), %eax
	cltq
	imulq	$552, %rax, %rax
	addq	%rdx, %rax
	addq	$67650320, %rax
	movl	8(%rax), %eax
	movl	%eax, -92(%rbp)
	movl	-92(%rbp), %eax
	movl	%eax, %edx
	leaq	.LC228(%rip), %rcx
	call	_Z7d_printPKcz
	movq	my_trace(%rip), %rdx
	movl	-88(%rbp), %eax
	cltq
	imulq	$552, %rax, %rax
	addq	%rdx, %rax
	addq	$67650304, %rax
	movq	16(%rax), %rax
	movl	-92(%rbp), %edx
	salq	$3, %rdx
	addq	%rax, %rdx
	movq	-40(%rbp), %rax
	movq	%rax, (%rdx)
	leaq	.LC229(%rip), %rcx
	call	_Z7d_printPKcz
	movq	my_trace(%rip), %rdx
	movl	-88(%rbp), %eax
	cltq
	imulq	$552, %rax, %rax
	addq	%rdx, %rax
	addq	$67650304, %rax
	movb	$1, 9(%rax)
	movq	my_trace(%rip), %rax
	movl	-88(%rbp), %edx
	movslq	%edx, %rdx
	imulq	$552, %rdx, %rdx
	addq	%rax, %rdx
	addq	$67650320, %rdx
	movl	8(%rdx), %edx
	addl	$1, %edx
	movl	-88(%rbp), %ecx
	movslq	%ecx, %rcx
	imulq	$552, %rcx, %rcx
	addq	%rcx, %rax
	addq	$67650320, %rax
	movl	%edx, 8(%rax)
	leaq	.LC230(%rip), %rcx
	call	_Z7d_printPKcz
	movq	my_trace(%rip), %rax
	movl	-88(%rbp), %edx
	movslq	%edx, %rdx
	imulq	$552, %rdx, %rdx
	addq	$67649776, %rdx
	addq	%rdx, %rax
	addq	$8, %rax
	addq	$56, %rsp
	popq	%rbx
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC231:
	.ascii "Creating process: %s\12\0"
	.align 8
.LC232:
	.ascii "Error creating process: 0x%08x\12\0"
	.text
	.globl	_Z17start_trace_fnamev
	.def	_Z17start_trace_fnamev;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z17start_trace_fnamev
_Z17start_trace_fnamev:
.LFB2522:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$96, %rsp
	.seh_stackalloc	96
	.seh_endprologue
	movq	my_trace(%rip), %rax
	addq	$70039084, %rax
	movq	%rax, %rdx
	leaq	.LC231(%rip), %rcx
	call	_Z7d_printPKcz
	movq	my_trace(%rip), %rax
	movq	%rax, %rcx
	movq	my_trace(%rip), %rax
	leaq	24(%rax), %rdx
	movq	my_trace(%rip), %rax
	addq	$70039084, %rax
	movq	%rcx, 72(%rsp)
	movq	%rdx, 64(%rsp)
	movq	$0, 56(%rsp)
	movq	$0, 48(%rsp)
	movl	$2, 40(%rsp)
	movl	$0, 32(%rsp)
	movl	$0, %r9d
	movl	$0, %r8d
	movl	$0, %edx
	movq	%rax, %rcx
	movq	__imp_CreateProcessA(%rip), %rax
	call	*%rax
	movl	%eax, -4(%rbp)
	cmpl	$0, -4(%rbp)
	jne	.L504
	movq	__imp_GetLastError(%rip), %rax
	call	*%rax
	movl	%eax, %edx
	leaq	.LC232(%rip), %rcx
	call	_Z7d_printPKcz
.L504:
	movq	my_trace(%rip), %rax
	movq	my_trace(%rip), %rdx
	movl	16(%rdx), %edx
	movl	%edx, 200(%rax)
	addq	$96, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC233:
	.ascii "Attaching debugger\12\0"
	.align 8
.LC234:
	.ascii "Successfully attached to PID: 0x%x, handle: 0x%x\12\0"
.LC235:
	.ascii "Attach failed\12\0"
	.text
	.globl	_Z15start_trace_pidv
	.def	_Z15start_trace_pidv;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z15start_trace_pidv
_Z15start_trace_pidv:
.LFB2523:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$32, %rsp
	.seh_stackalloc	32
	.seh_endprologue
	leaq	.LC233(%rip), %rcx
	call	_Z7d_printPKcz
	movq	my_trace(%rip), %rax
	movl	70033700(%rax), %eax
	movl	%eax, %ecx
	movq	__imp_DebugActiveProcess(%rip), %rax
	call	*%rax
	testl	%eax, %eax
	setne	%al
	testb	%al, %al
	je	.L506
	movq	my_trace(%rip), %rax
	movl	70033700(%rax), %eax
	movl	%eax, %edx
	leaq	.LC234(%rip), %rcx
	call	_Z7d_printPKcz
	jmp	.L505
.L506:
	leaq	.LC235(%rip), %rcx
	call	_Z7d_printPKcz
	nop
.L505:
	addq	$32, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.globl	_Z8find_libPc
	.def	_Z8find_libPc;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z8find_libPc
_Z8find_libPc:
.LFB2524:
	pushq	%rbp
	.seh_pushreg	%rbp
	subq	$1072, %rsp
	.seh_stackalloc	1072
	leaq	128(%rsp), %rbp
	.seh_setframe	%rbp, 128
	.seh_endprologue
	movq	%rcx, 960(%rbp)
	movl	$0, 932(%rbp)
	leaq	-96(%rbp), %rax
	movq	960(%rbp), %rdx
	movq	%rax, %rcx
	call	strcpy
	movl	$0, 936(%rbp)
	jmp	.L509
.L510:
	movl	936(%rbp), %eax
	movzbl	-96(%rbp,%rax), %eax
	movsbl	%al, %eax
	movl	%eax, %ecx
	movq	__imp_tolower(%rip), %rax
	call	*%rax
	movl	%eax, %edx
	movl	936(%rbp), %eax
	movb	%dl, -96(%rbp,%rax)
	addl	$1, 936(%rbp)
.L509:
	movl	936(%rbp), %eax
	movzbl	-96(%rbp,%rax), %eax
	testb	%al, %al
	jne	.L510
	movl	$0, 940(%rbp)
	jmp	.L511
.L516:
	movq	my_trace(%rip), %rdx
	movl	940(%rbp), %eax
	salq	$4, %rax
	movq	%rax, %rcx
	salq	$6, %rcx
	addq	%rcx, %rax
	addq	$67117296, %rax
	addq	%rax, %rdx
	leaq	416(%rbp), %rax
	movq	%rax, %rcx
	call	strcpy
	movl	$0, 936(%rbp)
	jmp	.L512
.L513:
	movl	936(%rbp), %eax
	movzbl	416(%rbp,%rax), %eax
	movsbl	%al, %eax
	movl	%eax, %ecx
	movq	__imp_tolower(%rip), %rax
	call	*%rax
	movl	%eax, %edx
	movl	936(%rbp), %eax
	movb	%dl, 416(%rbp,%rax)
	addl	$1, 936(%rbp)
.L512:
	movl	936(%rbp), %eax
	movzbl	416(%rbp,%rax), %eax
	testb	%al, %al
	jne	.L513
	leaq	-96(%rbp), %rdx
	leaq	416(%rbp), %rax
	movq	%rax, %rcx
	call	strstr
	testq	%rax, %rax
	je	.L514
	movq	my_trace(%rip), %rdx
	movl	940(%rbp), %eax
	salq	$4, %rax
	movq	%rax, %rcx
	salq	$6, %rcx
	addq	%rcx, %rax
	addq	%rdx, %rax
	addq	$67117280, %rax
	movq	8(%rax), %rax
	movl	%eax, 932(%rbp)
	jmp	.L515
.L514:
	addl	$1, 940(%rbp)
.L511:
	movl	940(%rbp), %edx
	movq	my_trace(%rip), %rax
	movq	68981568(%rax), %rax
	cmpq	%rax, %rdx
	jb	.L516
.L515:
	movl	932(%rbp), %eax
	addq	$1072, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC236:
	.ascii "rb\0"
	.text
	.globl	_Z16parse_descriptorPc
	.def	_Z16parse_descriptorPc;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z16parse_descriptorPc
_Z16parse_descriptorPc:
.LFB2525:
	pushq	%rbp
	.seh_pushreg	%rbp
	subq	$816, %rsp
	.seh_stackalloc	816
	leaq	128(%rsp), %rbp
	.seh_setframe	%rbp, 128
	.seh_endprologue
	movq	%rcx, 704(%rbp)
	leaq	.LC236(%rip), %rdx
	movq	704(%rbp), %rcx
	call	fopen
	movq	%rax, 680(%rbp)
	nop
.L519:
	movq	680(%rbp), %rdx
	leaq	-96(%rbp), %rax
	movq	%rdx, %r8
	movl	$80, %edx
	movq	%rax, %rcx
	call	fgets
	testq	%rax, %rax
	setne	%al
	testb	%al, %al
	jne	.L519
	movl	$0, %eax
	addq	$816, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC237:
	.ascii "Sending report: %s\12\0"
	.text
	.globl	_Z11send_reportv
	.def	_Z11send_reportv;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z11send_reportv
_Z11send_reportv:
.LFB2526:
	pushq	%rbp
	.seh_pushreg	%rbp
	movl	$1048640, %eax
	call	___chkstk_ms
	subq	%rax, %rsp
	.seh_stackalloc	1048640
	leaq	128(%rsp), %rbp
	.seh_setframe	%rbp, 128
	.seh_endprologue
	movl	$1263484205, 1048480(%rbp)
	movw	$11581, 1048484(%rbp)
	movb	$0, 1048486(%rbp)
	movq	my_trace(%rip), %rax
	movl	67932936(%rax), %eax
	cmpl	$4, %eax
	je	.L523
	cmpl	$4, %eax
	ja	.L524
	cmpl	$1, %eax
	je	.L525
	cmpl	$1, %eax
	jb	.L526
	cmpl	$2, %eax
	je	.L527
	cmpl	$3, %eax
	je	.L528
	jmp	.L522
.L524:
	cmpl	$6, %eax
	je	.L529
	cmpl	$6, %eax
	jb	.L530
	cmpl	$7, %eax
	je	.L531
	cmpl	$153, %eax
	je	.L532
	jmp	.L522
.L526:
	movw	$17234, 1048496(%rbp)
	movb	$0, 1048498(%rbp)
	jmp	.L522
.L525:
	movw	$21074, 1048496(%rbp)
	movb	$0, 1048498(%rbp)
	jmp	.L522
.L527:
	movw	$22610, 1048496(%rbp)
	movb	$0, 1048498(%rbp)
	jmp	.L522
.L528:
	movw	$21330, 1048496(%rbp)
	movb	$0, 1048498(%rbp)
	jmp	.L522
.L523:
	movw	$17490, 1048496(%rbp)
	movb	$0, 1048498(%rbp)
	jmp	.L522
.L530:
	movw	$16978, 1048496(%rbp)
	movb	$0, 1048498(%rbp)
	jmp	.L522
.L529:
	movw	$17746, 1048496(%rbp)
	movb	$0, 1048498(%rbp)
	jmp	.L522
.L531:
	movw	$18770, 1048496(%rbp)
	movb	$0, 1048498(%rbp)
	jmp	.L522
.L532:
	movw	$20050, 1048496(%rbp)
	movb	$0, 1048498(%rbp)
	nop
.L522:
	leaq	1048496(%rbp), %rdx
	leaq	-96(%rbp), %rax
	movq	%rax, %rcx
	call	strcpy
	movq	my_trace(%rip), %rax
	leaq	67932940(%rax), %rdx
	leaq	-96(%rbp), %rax
	movq	%rax, %rcx
	call	strcat
	leaq	1048480(%rbp), %rdx
	leaq	-96(%rbp), %rax
	movq	%rax, %rcx
	call	strcat
	leaq	-96(%rbp), %rax
	movq	%rax, %rdx
	leaq	.LC237(%rip), %rcx
	call	_Z7d_printPKcz
	leaq	-96(%rbp), %rax
	movq	%rax, %rcx
	call	strlen
	movl	%eax, %ecx
	movq	my_trace(%rip), %rax
	movq	70030616(%rax), %rax
	leaq	-96(%rbp), %rdx
	movl	$0, %r9d
	movl	%ecx, %r8d
	movq	%rax, %rcx
	movq	__imp_send(%rip), %rax
	call	*%rax
	movl	$0, %eax
	addq	$1048640, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC238:
	.ascii "Listing reactions\12\0"
.LC239:
	.ascii "%s:\12\0"
.LC240:
	.ascii "%s:0x%08x\12\0"
	.text
	.globl	_Z14list_reactionsv
	.def	_Z14list_reactionsv;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z14list_reactionsv
_Z14list_reactionsv:
.LFB2527:
	pushq	%rbp
	.seh_pushreg	%rbp
	pushq	%rdi
	.seh_pushreg	%rdi
	subq	$840, %rsp
	.seh_stackalloc	840
	leaq	128(%rsp), %rbp
	.seh_setframe	%rbp, 128
	.seh_endprologue
	leaq	.LC238(%rip), %rcx
	call	_Z7d_printPKcz
	movq	my_trace(%rip), %rax
	movl	$7, 67932936(%rax)
	movq	my_trace(%rip), %rax
	addq	$67932940, %rax
	movb	$0, (%rax)
	movl	$0, 700(%rbp)
	jmp	.L535
.L538:
	movq	my_trace(%rip), %rax
	movq	70039656(%rax), %rcx
	movl	700(%rbp), %edx
	movq	%rdx, %rax
	salq	$2, %rax
	addq	%rdx, %rax
	leaq	0(,%rax,4), %rdx
	addq	%rdx, %rax
	salq	$3, %rax
	addq	%rcx, %rax
	movq	%rax, 688(%rbp)
	movq	688(%rbp), %rdx
	leaq	-96(%rbp), %rax
	movq	%rdx, %r8
	leaq	.LC239(%rip), %rdx
	movq	%rax, %rcx
	call	sprintf
	movl	$0, 696(%rbp)
	jmp	.L536
.L537:
	movq	688(%rbp), %rax
	movq	184(%rax), %rax
	movq	%rax, 680(%rbp)
	movq	680(%rbp), %rax
	movq	520(%rax), %rdx
	movq	680(%rbp), %rax
	leaq	8(%rax), %rcx
	leaq	-96(%rbp), %rax
	movq	%rdx, %r9
	movq	%rcx, %r8
	leaq	.LC240(%rip), %rdx
	movq	%rax, %rcx
	call	sprintf
	addl	$1, 696(%rbp)
.L536:
	movq	688(%rbp), %rax
	movl	192(%rax), %eax
	cmpl	696(%rbp), %eax
	ja	.L537
	addl	$1, 700(%rbp)
.L535:
	movq	my_trace(%rip), %rax
	movl	70039664(%rax), %eax
	cmpl	700(%rbp), %eax
	ja	.L538
	movq	my_trace(%rip), %rax
	leaq	67932940(%rax), %rcx
	leaq	-96(%rbp), %rax
	movq	%rax, %rdx
	call	strcat
	movq	my_trace(%rip), %rax
	leaq	67932940(%rax), %rdx
	movq	%rdx, %rax
	movq	$-1, %rcx
	movq	%rax, %r8
	movl	$0, %eax
	movq	%r8, %rdi
	repnz scasb
	movq	%rcx, %rax
	notq	%rax
	subq	$1, %rax
	addq	%rdx, %rax
	movw	$10, (%rax)
	movl	$0, %eax
	addq	$840, %rsp
	popq	%rdi
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC241:
	.ascii "Listing BPTs\12\0"
.LC242:
	.ascii "0x%08x\12\0"
	.text
	.globl	_Z9list_bptsv
	.def	_Z9list_bptsv;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z9list_bptsv
_Z9list_bptsv:
.LFB2528:
	pushq	%rbp
	.seh_pushreg	%rbp
	pushq	%rdi
	.seh_pushreg	%rdi
	subq	$824, %rsp
	.seh_stackalloc	824
	leaq	128(%rsp), %rbp
	.seh_setframe	%rbp, 128
	.seh_endprologue
	leaq	.LC241(%rip), %rcx
	call	_Z7d_printPKcz
	movq	my_trace(%rip), %rax
	movl	$7, 67932936(%rax)
	movq	my_trace(%rip), %rax
	addq	$67932940, %rax
	movb	$0, (%rax)
	movl	$0, 684(%rbp)
	jmp	.L541
.L542:
	movq	my_trace(%rip), %rdx
	movl	684(%rbp), %eax
	imulq	$552, %rax, %rax
	addq	%rdx, %rax
	addq	$67650288, %rax
	movq	16(%rax), %rdx
	leaq	-96(%rbp), %rax
	movq	%rdx, %r8
	leaq	.LC242(%rip), %rdx
	movq	%rax, %rcx
	call	sprintf
	movq	my_trace(%rip), %rax
	leaq	67932940(%rax), %rcx
	leaq	-96(%rbp), %rax
	movq	%rax, %rdx
	call	strcat
	movq	my_trace(%rip), %rax
	leaq	67932940(%rax), %rdx
	movq	%rdx, %rax
	movq	$-1, %rcx
	movq	%rax, %r8
	movl	$0, %eax
	movq	%r8, %rdi
	repnz scasb
	movq	%rcx, %rax
	notq	%rax
	subq	$1, %rax
	addq	%rdx, %rax
	movw	$10, (%rax)
	addl	$1, 684(%rbp)
.L541:
	movl	684(%rbp), %edx
	movq	my_trace(%rip), %rax
	movq	68981576(%rax), %rax
	cmpq	%rax, %rdx
	jb	.L542
	movl	$0, %eax
	addq	$824, %rsp
	popq	%rdi
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC243:
	.ascii "Listing LIBs\12\0"
.LC244:
	.ascii "Currently have %d libs\12\0"
.LC245:
	.ascii "Reporting: %s\12\0"
	.text
	.globl	_Z9list_libsv
	.def	_Z9list_libsv;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z9list_libsv
_Z9list_libsv:
.LFB2529:
	pushq	%rbp
	.seh_pushreg	%rbp
	pushq	%rdi
	.seh_pushreg	%rdi
	subq	$824, %rsp
	.seh_stackalloc	824
	leaq	128(%rsp), %rbp
	.seh_setframe	%rbp, 128
	.seh_endprologue
	leaq	.LC243(%rip), %rcx
	call	_Z7d_printPKcz
	movq	my_trace(%rip), %rax
	movl	$7, 67932936(%rax)
	movq	my_trace(%rip), %rax
	movq	68981568(%rax), %rax
	movq	%rax, %rdx
	leaq	.LC244(%rip), %rcx
	call	_Z7d_printPKcz
	movq	my_trace(%rip), %rax
	addq	$67932940, %rax
	movb	$0, (%rax)
	movl	$0, 684(%rbp)
	jmp	.L545
.L546:
	movq	my_trace(%rip), %rdx
	movl	684(%rbp), %eax
	salq	$4, %rax
	movq	%rax, %rcx
	salq	$6, %rcx
	addq	%rcx, %rax
	addq	$67117280, %rax
	leaq	(%rdx,%rax), %rcx
	leaq	-96(%rbp), %rax
	movq	%rax, %rdx
	call	_Z13serialize_libP10_LIB_ENTRYPc
	movq	my_trace(%rip), %rax
	leaq	67932940(%rax), %rcx
	leaq	-96(%rbp), %rax
	movq	%rax, %rdx
	call	strcat
	movq	my_trace(%rip), %rax
	leaq	67932940(%rax), %rdx
	movq	%rdx, %rax
	movq	$-1, %rcx
	movq	%rax, %r8
	movl	$0, %eax
	movq	%r8, %rdi
	repnz scasb
	movq	%rcx, %rax
	notq	%rax
	subq	$1, %rax
	addq	%rdx, %rax
	movw	$10, (%rax)
	addl	$1, 684(%rbp)
.L545:
	movl	684(%rbp), %edx
	movq	my_trace(%rip), %rax
	movq	68981568(%rax), %rax
	cmpq	%rax, %rdx
	jb	.L546
	movq	my_trace(%rip), %rax
	addq	$67932940, %rax
	movq	%rax, %rdx
	leaq	.LC245(%rip), %rcx
	call	_Z7d_printPKcz
	movl	$0, %eax
	addq	$824, %rsp
	popq	%rdi
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC246:
	.ascii "Listings TEBs\12\0"
.LC247:
	.ascii "Currently have %d threads\12\0"
	.text
	.globl	_Z9list_tebsv
	.def	_Z9list_tebsv;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z9list_tebsv
_Z9list_tebsv:
.LFB2530:
	pushq	%rbp
	.seh_pushreg	%rbp
	pushq	%rdi
	.seh_pushreg	%rdi
	subq	$824, %rsp
	.seh_stackalloc	824
	leaq	128(%rsp), %rbp
	.seh_setframe	%rbp, 128
	.seh_endprologue
	leaq	.LC246(%rip), %rcx
	call	_Z7d_printPKcz
	movq	my_trace(%rip), %rax
	movl	$7, 67932936(%rax)
	movq	my_trace(%rip), %rax
	movq	68981560(%rax), %rax
	movq	%rax, %rdx
	leaq	.LC247(%rip), %rcx
	call	_Z7d_printPKcz
	movq	my_trace(%rip), %rax
	addq	$67932940, %rax
	movb	$0, (%rax)
	movl	$0, 684(%rbp)
	jmp	.L549
.L550:
	movq	my_trace(%rip), %rax
	leaq	67932940(%rax), %rdx
	movq	%rdx, %rax
	movq	$-1, %rcx
	movq	%rax, %r8
	movl	$0, %eax
	movq	%r8, %rdi
	repnz scasb
	movq	%rcx, %rax
	notq	%rax
	subq	$1, %rax
	addq	%rdx, %rax
	movw	$120, (%rax)
	addl	$1, 684(%rbp)
.L549:
	movl	684(%rbp), %edx
	movq	my_trace(%rip), %rax
	movq	68981560(%rax), %rax
	cmpq	%rax, %rdx
	jb	.L550
	movq	my_trace(%rip), %rax
	addq	$67932940, %rax
	movq	%rax, %rdx
	leaq	.LC245(%rip), %rcx
	call	_Z7d_printPKcz
	movl	$0, %eax
	addq	$824, %rsp
	popq	%rdi
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
	.align 8
.LC248:
	.ascii "Failed to set context, error: 0x%08x\12\0"
	.text
	.globl	_Z13write_contextmP8_CONTEXT
	.def	_Z13write_contextmP8_CONTEXT;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z13write_contextmP8_CONTEXT
_Z13write_contextmP8_CONTEXT:
.LFB2531:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$48, %rsp
	.seh_stackalloc	48
	.seh_endprologue
	movl	%ecx, 16(%rbp)
	movq	%rdx, 24(%rbp)
	movq	my_trace(%rip), %rax
	movl	16(%rbp), %edx
	addq	$56, %rdx
	movl	(%rax,%rdx,4), %eax
	movl	%eax, -4(%rbp)
	movq	my_trace(%rip), %rax
	movl	-4(%rbp), %edx
	addq	$2097159, %rdx
	salq	$5, %rdx
	addq	%rdx, %rax
	movq	8(%rax), %rax
	movq	%rax, -16(%rbp)
	movq	24(%rbp), %rax
	movl	$1048587, 48(%rax)
	movq	24(%rbp), %rdx
	movq	-16(%rbp), %rax
	movq	%rax, %rcx
	movq	__imp_SetThreadContext(%rip), %rax
	call	*%rax
	testl	%eax, %eax
	sete	%al
	testb	%al, %al
	je	.L553
	movq	__imp_GetLastError(%rip), %rax
	call	*%rax
	movl	%eax, %edx
	leaq	.LC248(%rip), %rcx
	call	_Z7d_printPKcz
.L553:
	movl	$0, %eax
	addq	$48, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC249:
	.ascii "Thread32First\0"
.LC250:
	.ascii "0x%08x - %08x\12\0"
	.text
	.globl	_Z13list_all_tebsv
	.def	_Z13list_all_tebsv;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z13list_all_tebsv
_Z13list_all_tebsv:
.LFB2532:
	pushq	%rbp
	.seh_pushreg	%rbp
	subq	$848, %rsp
	.seh_stackalloc	848
	leaq	128(%rsp), %rbp
	.seh_setframe	%rbp, 128
	.seh_endprologue
	movq	$-1, 712(%rbp)
	movl	$0, %edx
	movl	$4, %ecx
	call	CreateToolhelp32Snapshot
	movq	%rax, 712(%rbp)
	cmpq	$-1, 712(%rbp)
	jne	.L556
	movl	$0, %eax
	jmp	.L559
.L556:
	movl	$28, %eax
	movl	%eax, 672(%rbp)
	leaq	672(%rbp), %rdx
	movq	712(%rbp), %rax
	movq	%rax, %rcx
	call	Thread32First
	testl	%eax, %eax
	sete	%al
	testb	%al, %al
	je	.L558
	leaq	.LC249(%rip), %rcx
	call	_Z7d_printPKcz
	movq	712(%rbp), %rax
	movq	%rax, %rcx
	movq	__imp_CloseHandle(%rip), %rax
	call	*%rax
	movl	$0, %eax
	jmp	.L559
.L558:
	movl	680(%rbp), %ecx
	movl	684(%rbp), %edx
	leaq	-96(%rbp), %rax
	movl	%ecx, %r9d
	movl	%edx, %r8d
	leaq	.LC250(%rip), %rdx
	movq	%rax, %rcx
	call	sprintf
	movq	my_trace(%rip), %rax
	leaq	67932940(%rax), %rcx
	leaq	-96(%rbp), %rax
	movq	%rax, %rdx
	call	strcat
	leaq	672(%rbp), %rdx
	movq	712(%rbp), %rax
	movq	%rax, %rcx
	call	Thread32Next
	testl	%eax, %eax
	setne	%al
	testb	%al, %al
	jne	.L558
	movq	712(%rbp), %rax
	movq	%rax, %rcx
	movq	__imp_CloseHandle(%rip), %rax
	call	*%rax
	movl	$0, %eax
.L559:
	addq	$848, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC251:
	.ascii "Trying relese TID: 0x%08x\12\0"
	.align 8
.LC252:
	.ascii "Failed to open thread, error: 0x%08x\12\0"
	.align 8
.LC253:
	.ascii "Failed to resume thread, error: 0x%08x\12\0"
	.text
	.globl	_Z14release_threadm
	.def	_Z14release_threadm;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z14release_threadm
_Z14release_threadm:
.LFB2533:
	pushq	%rbp
	.seh_pushreg	%rbp
	subq	$816, %rsp
	.seh_stackalloc	816
	leaq	128(%rsp), %rbp
	.seh_setframe	%rbp, 128
	.seh_endprologue
	movl	%ecx, 704(%rbp)
	movq	$-1, 680(%rbp)
	movl	704(%rbp), %edx
	leaq	.LC251(%rip), %rcx
	call	_Z7d_printPKcz
	movl	704(%rbp), %r8d
	movl	$0, %edx
	movl	$2, %ecx
	movq	__imp_OpenThread(%rip), %rax
	call	*%rax
	movq	%rax, 680(%rbp)
	cmpq	$0, 680(%rbp)
	sete	%al
	testb	%al, %al
	je	.L561
	movq	__imp_GetLastError(%rip), %rax
	call	*%rax
	movl	%eax, %edx
	leaq	.LC252(%rip), %rcx
	call	_Z7d_printPKcz
	movq	__imp_GetLastError(%rip), %rax
	call	*%rax
	movl	%eax, %edx
	leaq	-96(%rbp), %rax
	movl	%edx, %r8d
	leaq	.LC99(%rip), %rdx
	movq	%rax, %rcx
	call	sprintf
	movq	my_trace(%rip), %rax
	leaq	67932940(%rax), %rcx
	leaq	-96(%rbp), %rax
	movq	%rax, %rdx
	call	strcpy
.L561:
	movq	680(%rbp), %rax
	movq	%rax, %rcx
	movq	__imp_ResumeThread(%rip), %rax
	call	*%rax
	cmpl	$-1, %eax
	sete	%al
	testb	%al, %al
	je	.L562
	movq	__imp_GetLastError(%rip), %rax
	call	*%rax
	movl	%eax, %edx
	leaq	.LC253(%rip), %rcx
	call	_Z7d_printPKcz
	movq	__imp_GetLastError(%rip), %rax
	call	*%rax
	movl	%eax, %edx
	leaq	-96(%rbp), %rax
	movl	%edx, %r8d
	leaq	.LC99(%rip), %rdx
	movq	%rax, %rcx
	call	sprintf
	movq	my_trace(%rip), %rax
	leaq	67932940(%rax), %rcx
	leaq	-96(%rbp), %rax
	movq	%rax, %rdx
	call	strcpy
.L562:
	movl	$0, %eax
	addq	$816, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
	.align 8
.LC254:
	.ascii "Trying to get context of TID: 0x%08x, index: 0x%08x, handle: 0x%08x\12\0"
	.text
	.globl	_Z12read_contextmP8_CONTEXT
	.def	_Z12read_contextmP8_CONTEXT;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z12read_contextmP8_CONTEXT
_Z12read_contextmP8_CONTEXT:
.LFB2534:
	pushq	%rbp
	.seh_pushreg	%rbp
	subq	$816, %rsp
	.seh_stackalloc	816
	leaq	128(%rsp), %rbp
	.seh_setframe	%rbp, 128
	.seh_endprologue
	movl	%ecx, 704(%rbp)
	movq	%rdx, 712(%rbp)
	movq	$-1, 680(%rbp)
	cmpl	$0, 704(%rbp)
	jne	.L565
	movq	my_trace(%rip), %rax
	movl	68981632(%rax), %eax
	movl	%eax, 704(%rbp)
.L565:
	movq	my_trace(%rip), %rax
	movl	704(%rbp), %edx
	addq	$56, %rdx
	movl	(%rax,%rdx,4), %eax
	movl	%eax, 676(%rbp)
	movq	my_trace(%rip), %rax
	movl	676(%rbp), %edx
	addq	$2097159, %rdx
	salq	$5, %rdx
	addq	%rdx, %rax
	movq	8(%rax), %rax
	movq	%rax, 680(%rbp)
	movq	680(%rbp), %rdx
	movl	676(%rbp), %eax
	movq	%rdx, %r9
	movl	%eax, %r8d
	movl	704(%rbp), %edx
	leaq	.LC254(%rip), %rcx
	call	_Z7d_printPKcz
	movq	712(%rbp), %rax
	movl	$1048587, 48(%rax)
	movq	712(%rbp), %rdx
	movq	680(%rbp), %rax
	movq	%rax, %rcx
	movq	__imp_GetThreadContext(%rip), %rax
	call	*%rax
	testl	%eax, %eax
	sete	%al
	testb	%al, %al
	je	.L566
	movq	__imp_GetLastError(%rip), %rax
	call	*%rax
	movl	%eax, %edx
	leaq	.LC7(%rip), %rcx
	call	_Z7d_printPKcz
	movq	__imp_GetLastError(%rip), %rax
	call	*%rax
	movl	%eax, %edx
	leaq	-96(%rbp), %rax
	movl	%edx, %r8d
	leaq	.LC99(%rip), %rdx
	movq	%rax, %rcx
	call	sprintf
	movq	my_trace(%rip), %rax
	leaq	67932940(%rax), %rcx
	leaq	-96(%rbp), %rax
	movq	%rax, %rdx
	call	strcpy
.L566:
	movl	$0, %eax
	addq	$816, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
	.align 8
.LC255:
	.ascii "Trying to write: 0x%08x @ %p, handle: 0x%08x\12\0"
.LC256:
	.ascii "New val @ %p: 0x%08x\12\0"
	.text
	.globl	_Z11write_dwordyy
	.def	_Z11write_dwordyy;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z11write_dwordyy
_Z11write_dwordyy:
.LFB2535:
	pushq	%rbp
	.seh_pushreg	%rbp
	subq	$1296, %rsp
	.seh_stackalloc	1296
	leaq	128(%rsp), %rbp
	.seh_setframe	%rbp, 128
	.seh_endprologue
	movq	%rcx, 1184(%rbp)
	movq	%rdx, 1192(%rbp)
	movq	my_trace(%rip), %rax
	movq	208(%rax), %rax
	movq	%rax, 1160(%rbp)
	movq	my_trace(%rip), %rax
	movq	208(%rax), %rdx
	movq	1192(%rbp), %rax
	movq	%rdx, %r9
	movq	1184(%rbp), %r8
	movq	%rax, %rdx
	leaq	.LC255(%rip), %rcx
	call	_Z7d_printPKcz
	movq	1184(%rbp), %rdx
	movq	my_trace(%rip), %rax
	movq	208(%rax), %rax
	leaq	1192(%rbp), %r8
	leaq	1152(%rbp), %rcx
	movq	%rcx, 32(%rsp)
	movl	$4, %r9d
	movq	%rax, %rcx
	call	_Z12write_memoryPvS_S_yPy
	movq	1152(%rbp), %rax
	cmpq	$4, %rax
	jne	.L569
	leaq	-80(%rbp), %rax
	movabsq	$2320539375069255492, %rdx
	movq	%rdx, (%rax)
	movabsq	$31073798281851511, %rcx
	movq	%rcx, 8(%rax)
	movq	my_trace(%rip), %rax
	leaq	67932940(%rax), %rcx
	leaq	-80(%rbp), %rax
	movq	%rax, %rdx
	call	strcpy
	jmp	.L570
.L569:
	movq	__imp_GetLastError(%rip), %rax
	call	*%rax
	movl	%eax, %edx
	leaq	-80(%rbp), %rax
	movl	%edx, %r8d
	leaq	.LC99(%rip), %rdx
	movq	%rax, %rcx
	call	sprintf
	movq	my_trace(%rip), %rax
	leaq	67932940(%rax), %rcx
	leaq	-80(%rbp), %rax
	movq	%rax, %rdx
	call	strcpy
.L570:
	movq	1184(%rbp), %rdx
	movq	my_trace(%rip), %rax
	movq	208(%rax), %rax
	leaq	1192(%rbp), %r8
	leaq	1152(%rbp), %rcx
	movq	%rcx, 32(%rsp)
	movl	$4, %r9d
	movq	%rax, %rcx
	call	_Z11read_memoryPvS_S_yPy
	movq	1192(%rbp), %rax
	movq	%rax, %r8
	movq	1184(%rbp), %rdx
	leaq	.LC256(%rip), %rcx
	call	_Z7d_printPKcz
	movl	$0, %eax
	addq	$1296, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.globl	_Z9read_bytey
	.def	_Z9read_bytey;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z9read_bytey
_Z9read_bytey:
.LFB2536:
	pushq	%rbp
	.seh_pushreg	%rbp
	subq	$832, %rsp
	.seh_stackalloc	832
	leaq	128(%rsp), %rbp
	.seh_setframe	%rbp, 128
	.seh_endprologue
	movq	%rcx, 720(%rbp)
	movq	720(%rbp), %rdx
	movq	my_trace(%rip), %rax
	movq	208(%rax), %rax
	leaq	703(%rbp), %r8
	leaq	688(%rbp), %rcx
	movq	%rcx, 32(%rsp)
	movl	$1, %r9d
	movq	%rax, %rcx
	call	_Z11read_memoryPvS_S_yPy
	movzbl	703(%rbp), %eax
	addq	$832, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC257:
	.ascii "BYTE written\0"
	.text
	.globl	_Z10write_byteyPc
	.def	_Z10write_byteyPc;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z10write_byteyPc
_Z10write_byteyPc:
.LFB2537:
	pushq	%rbp
	.seh_pushreg	%rbp
	subq	$832, %rsp
	.seh_stackalloc	832
	leaq	128(%rsp), %rbp
	.seh_setframe	%rbp, 128
	.seh_endprologue
	movq	%rcx, 720(%rbp)
	movq	%rdx, 728(%rbp)
	movq	728(%rbp), %rax
	movl	$16, %r8d
	movl	$0, %edx
	movq	%rax, %rcx
	call	strtoul
	movb	%al, 695(%rbp)
	movq	720(%rbp), %rdx
	movq	my_trace(%rip), %rax
	movq	208(%rax), %rax
	leaq	695(%rbp), %r8
	leaq	696(%rbp), %rcx
	movq	%rcx, 32(%rsp)
	movl	$1, %r9d
	movq	%rax, %rcx
	call	_Z12write_memoryPvS_S_yPy
	movq	696(%rbp), %rax
	cmpq	$1, %rax
	jne	.L575
	movq	728(%rbp), %rdx
	leaq	-80(%rbp), %rax
	movq	%rdx, %r8
	leaq	.LC257(%rip), %rdx
	movq	%rax, %rcx
	call	sprintf
	movq	my_trace(%rip), %rax
	leaq	67932940(%rax), %rcx
	leaq	-80(%rbp), %rax
	movq	%rax, %rdx
	call	strcpy
	jmp	.L576
.L575:
	leaq	-80(%rbp), %rax
	movl	$1869771333, (%rax)
	movw	$114, 4(%rax)
	movq	my_trace(%rip), %rax
	leaq	67932940(%rax), %rcx
	leaq	-80(%rbp), %rax
	movq	%rax, %rdx
	call	strcpy
.L576:
	movl	$0, %eax
	addq	$832, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC258:
	.ascii "%04x\0"
	.text
	.globl	_Z9read_wordy
	.def	_Z9read_wordy;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z9read_wordy
_Z9read_wordy:
.LFB2538:
	pushq	%rbp
	.seh_pushreg	%rbp
	subq	$832, %rsp
	.seh_stackalloc	832
	leaq	128(%rsp), %rbp
	.seh_setframe	%rbp, 128
	.seh_endprologue
	movq	%rcx, 720(%rbp)
	movq	720(%rbp), %rdx
	movq	my_trace(%rip), %rax
	movq	208(%rax), %rax
	leaq	702(%rbp), %r8
	leaq	688(%rbp), %rcx
	movq	%rcx, 32(%rsp)
	movl	$2, %r9d
	movq	%rax, %rcx
	call	_Z11read_memoryPvS_S_yPy
	movq	688(%rbp), %rax
	cmpq	$2, %rax
	jne	.L579
	movzwl	702(%rbp), %eax
	movzwl	%ax, %edx
	leaq	-80(%rbp), %rax
	movl	%edx, %r8d
	leaq	.LC258(%rip), %rdx
	movq	%rax, %rcx
	call	sprintf
	movq	my_trace(%rip), %rax
	leaq	67932940(%rax), %rcx
	leaq	-80(%rbp), %rax
	movq	%rax, %rdx
	call	strcpy
	jmp	.L580
.L579:
	leaq	-80(%rbp), %rax
	movl	$1869771333, (%rax)
	movw	$114, 4(%rax)
	movq	my_trace(%rip), %rax
	leaq	67932940(%rax), %rcx
	leaq	-80(%rbp), %rax
	movq	%rax, %rdx
	call	strcpy
.L580:
	movzwl	702(%rbp), %eax
	movzwl	%ax, %eax
	addq	$832, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC259:
	.ascii "WORD written\0"
	.text
	.globl	_Z10write_wordyPc
	.def	_Z10write_wordyPc;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z10write_wordyPc
_Z10write_wordyPc:
.LFB2539:
	pushq	%rbp
	.seh_pushreg	%rbp
	subq	$832, %rsp
	.seh_stackalloc	832
	leaq	128(%rsp), %rbp
	.seh_setframe	%rbp, 128
	.seh_endprologue
	movq	%rcx, 720(%rbp)
	movq	%rdx, 728(%rbp)
	movq	728(%rbp), %rax
	movl	$16, %r8d
	movl	$0, %edx
	movq	%rax, %rcx
	call	strtoul
	movw	%ax, 694(%rbp)
	movq	720(%rbp), %rdx
	movq	my_trace(%rip), %rax
	movq	208(%rax), %rax
	leaq	694(%rbp), %r8
	leaq	696(%rbp), %rcx
	movq	%rcx, 32(%rsp)
	movl	$2, %r9d
	movq	%rax, %rcx
	call	_Z12write_memoryPvS_S_yPy
	movq	696(%rbp), %rax
	cmpq	$2, %rax
	jne	.L583
	movq	728(%rbp), %rdx
	leaq	-80(%rbp), %rax
	movq	%rdx, %r8
	leaq	.LC259(%rip), %rdx
	movq	%rax, %rcx
	call	sprintf
	movq	my_trace(%rip), %rax
	leaq	67932940(%rax), %rcx
	leaq	-80(%rbp), %rax
	movq	%rax, %rdx
	call	strcpy
	jmp	.L584
.L583:
	leaq	-80(%rbp), %rax
	movl	$1869771333, (%rax)
	movw	$114, 4(%rax)
	movq	my_trace(%rip), %rax
	leaq	67932940(%rax), %rcx
	leaq	-80(%rbp), %rax
	movq	%rax, %rdx
	call	strcpy
.L584:
	movl	$0, %eax
	addq	$832, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.globl	_Z10read_dwordy
	.def	_Z10read_dwordy;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z10read_dwordy
_Z10read_dwordy:
.LFB2540:
	pushq	%rbp
	.seh_pushreg	%rbp
	subq	$832, %rsp
	.seh_stackalloc	832
	leaq	128(%rsp), %rbp
	.seh_setframe	%rbp, 128
	.seh_endprologue
	movq	%rcx, 720(%rbp)
	movq	720(%rbp), %rdx
	movq	my_trace(%rip), %rax
	movq	208(%rax), %rax
	leaq	696(%rbp), %r8
	leaq	688(%rbp), %rcx
	movq	%rcx, 32(%rsp)
	movl	$4, %r9d
	movq	%rax, %rcx
	call	_Z11read_memoryPvS_S_yPy
	movq	696(%rbp), %rax
	addq	$832, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC260:
	.ascii "%02x \0"
.LC261:
	.ascii "%c\0"
	.text
	.globl	_Z6hexifyPcS_j
	.def	_Z6hexifyPcS_j;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z6hexifyPcS_j
_Z6hexifyPcS_j:
.LFB2541:
	pushq	%rbp
	.seh_pushreg	%rbp
	pushq	%rdi
	.seh_pushreg	%rdi
	subq	$1624, %rsp
	.seh_stackalloc	1624
	leaq	128(%rsp), %rbp
	.seh_setframe	%rbp, 128
	.seh_endprologue
	movq	%rcx, 1520(%rbp)
	movq	%rdx, 1528(%rbp)
	movl	%r8d, 1536(%rbp)
	movl	1536(%rbp), %eax
	shrl	$5, %eax
	movl	%eax, 1476(%rbp)
	movl	1536(%rbp), %eax
	andl	$31, %eax
	movl	%eax, 1472(%rbp)
	movl	$0, 1480(%rbp)
	jmp	.L589
.L598:
	leaq	-64(%rbp), %rax
	movl	$768, %r8d
	movl	$0, %edx
	movq	%rax, %rcx
	call	memset
	leaq	-96(%rbp), %rax
	movl	$32, %r8d
	movl	$0, %edx
	movq	%rax, %rcx
	call	memset
	movl	1480(%rbp), %eax
	sall	$5, %eax
	movl	%eax, %edx
	movq	1520(%rbp), %rax
	addq	%rdx, %rax
	movq	(%rax), %rdx
	movq	%rdx, -96(%rbp)
	movq	8(%rax), %rdx
	movq	%rdx, -88(%rbp)
	movq	16(%rax), %rdx
	movq	%rdx, -80(%rbp)
	movq	24(%rax), %rax
	movq	%rax, -72(%rbp)
	movl	$0, 1484(%rbp)
	jmp	.L590
.L593:
	movl	1484(%rbp), %eax
	movzbl	-96(%rbp,%rax), %eax
	cmpb	$10, %al
	jne	.L591
	movl	1484(%rbp), %eax
	movb	$46, -96(%rbp,%rax)
.L591:
	movl	1484(%rbp), %eax
	movzbl	-96(%rbp,%rax), %eax
	cmpb	$13, %al
	jne	.L592
	movl	1484(%rbp), %eax
	movb	$46, -96(%rbp,%rax)
.L592:
	movl	1484(%rbp), %eax
	movzbl	-96(%rbp,%rax), %eax
	movsbl	%al, %eax
	movzbl	%al, %edx
	leaq	704(%rbp), %rax
	movl	%edx, %r8d
	leaq	.LC260(%rip), %rdx
	movq	%rax, %rcx
	call	sprintf
	leaq	704(%rbp), %rdx
	leaq	-64(%rbp), %rax
	movq	%rax, %rcx
	call	strcat
	addl	$1, 1484(%rbp)
.L590:
	cmpl	$31, 1484(%rbp)
	jbe	.L593
	leaq	-64(%rbp), %rax
	movq	$-1, %rcx
	movq	%rax, %rdx
	movl	$0, %eax
	movq	%rdx, %rdi
	repnz scasb
	movq	%rcx, %rax
	notq	%rax
	leaq	-1(%rax), %rdx
	leaq	-64(%rbp), %rax
	addq	%rdx, %rax
	movw	$8224, (%rax)
	movb	$0, 2(%rax)
	movl	$0, 1484(%rbp)
	jmp	.L594
.L597:
	movl	1484(%rbp), %eax
	movzbl	-96(%rbp,%rax), %eax
	cmpb	$10, %al
	jne	.L595
	movl	1484(%rbp), %eax
	movb	$46, -96(%rbp,%rax)
.L595:
	movl	1484(%rbp), %eax
	movzbl	-96(%rbp,%rax), %eax
	cmpb	$13, %al
	jne	.L596
	movl	1484(%rbp), %eax
	movb	$46, -96(%rbp,%rax)
.L596:
	movl	1484(%rbp), %eax
	movzbl	-96(%rbp,%rax), %eax
	movsbl	%al, %edx
	leaq	704(%rbp), %rax
	movl	%edx, %r8d
	leaq	.LC261(%rip), %rdx
	movq	%rax, %rcx
	call	sprintf
	leaq	704(%rbp), %rdx
	leaq	-64(%rbp), %rax
	movq	%rax, %rcx
	call	strcat
	addl	$1, 1484(%rbp)
.L594:
	cmpl	$31, 1484(%rbp)
	jbe	.L597
	leaq	-64(%rbp), %rax
	movq	$-1, %rcx
	movq	%rax, %rdx
	movl	$0, %eax
	movq	%rdx, %rdi
	repnz scasb
	movq	%rcx, %rax
	notq	%rax
	leaq	-1(%rax), %rdx
	leaq	-64(%rbp), %rax
	addq	%rdx, %rax
	movw	$10, (%rax)
	leaq	-64(%rbp), %rdx
	movq	1528(%rbp), %rax
	movq	%rax, %rcx
	call	strcat
	addl	$1, 1480(%rbp)
.L589:
	movl	1480(%rbp), %eax
	cmpl	1476(%rbp), %eax
	jb	.L598
	leaq	-64(%rbp), %rax
	movl	$768, %r8d
	movl	$0, %edx
	movq	%rax, %rcx
	call	memset
	leaq	-96(%rbp), %rax
	movl	$32, %r8d
	movl	$0, %edx
	movq	%rax, %rcx
	call	memset
	movl	1472(%rbp), %ecx
	movl	1480(%rbp), %eax
	sall	$5, %eax
	movl	%eax, %edx
	movq	1520(%rbp), %rax
	addq	%rax, %rdx
	leaq	-96(%rbp), %rax
	movq	%rcx, %r8
	movq	%rax, %rcx
	call	memcpy
	movl	$0, 1484(%rbp)
	jmp	.L599
.L602:
	movl	1484(%rbp), %eax
	movzbl	-96(%rbp,%rax), %eax
	cmpb	$10, %al
	jne	.L600
	movl	1484(%rbp), %eax
	movb	$46, -96(%rbp,%rax)
.L600:
	movl	1484(%rbp), %eax
	movzbl	-96(%rbp,%rax), %eax
	cmpb	$13, %al
	jne	.L601
	movl	1484(%rbp), %eax
	movb	$46, -96(%rbp,%rax)
.L601:
	movl	1484(%rbp), %eax
	movzbl	-96(%rbp,%rax), %eax
	movsbl	%al, %eax
	movzbl	%al, %edx
	leaq	704(%rbp), %rax
	movl	%edx, %r8d
	leaq	.LC260(%rip), %rdx
	movq	%rax, %rcx
	call	sprintf
	leaq	704(%rbp), %rdx
	leaq	-64(%rbp), %rax
	movq	%rax, %rcx
	call	strcat
	addl	$1, 1484(%rbp)
.L599:
	movl	1484(%rbp), %eax
	cmpl	1472(%rbp), %eax
	jb	.L602
	leaq	-64(%rbp), %rax
	movq	$-1, %rcx
	movq	%rax, %rdx
	movl	$0, %eax
	movq	%rdx, %rdi
	repnz scasb
	movq	%rcx, %rax
	notq	%rax
	leaq	-1(%rax), %rdx
	leaq	-64(%rbp), %rax
	addq	%rdx, %rax
	movw	$8224, (%rax)
	movb	$0, 2(%rax)
	movl	$0, 1484(%rbp)
	jmp	.L603
.L606:
	movl	1484(%rbp), %eax
	movzbl	-96(%rbp,%rax), %eax
	cmpb	$10, %al
	jne	.L604
	movl	1484(%rbp), %eax
	movb	$46, -96(%rbp,%rax)
.L604:
	movl	1484(%rbp), %eax
	movzbl	-96(%rbp,%rax), %eax
	cmpb	$13, %al
	jne	.L605
	movl	1484(%rbp), %eax
	movb	$46, -96(%rbp,%rax)
.L605:
	movl	1484(%rbp), %eax
	movzbl	-96(%rbp,%rax), %eax
	movsbl	%al, %edx
	leaq	704(%rbp), %rax
	movl	%edx, %r8d
	leaq	.LC261(%rip), %rdx
	movq	%rax, %rcx
	call	sprintf
	leaq	704(%rbp), %rdx
	leaq	-64(%rbp), %rax
	movq	%rax, %rcx
	call	strcat
	addl	$1, 1484(%rbp)
.L603:
	movl	1484(%rbp), %eax
	cmpl	1472(%rbp), %eax
	jb	.L606
	leaq	-64(%rbp), %rax
	movq	$-1, %rcx
	movq	%rax, %rdx
	movl	$0, %eax
	movq	%rdx, %rdi
	repnz scasb
	movq	%rcx, %rax
	notq	%rax
	leaq	-1(%rax), %rdx
	leaq	-64(%rbp), %rax
	addq	%rdx, %rax
	movw	$10, (%rax)
	leaq	-64(%rbp), %rdx
	movq	1528(%rbp), %rax
	movq	%rax, %rcx
	call	strcat
	movl	$0, %eax
	addq	$1624, %rsp
	popq	%rdi
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC262:
	.ascii "# out_arg1 @ %d\12\0"
	.align 8
.LC263:
	.ascii "Trying to read 0x%08x bytes: @ %p, handle: 0x%08x\12\0"
.LC264:
	.ascii "# out_arg2 @ %d\12\0"
.LC265:
	.ascii "OU,0x%x,%s\12\0"
.LC266:
	.ascii "Error: 0x%08x\0"
.LC267:
	.ascii "# OU,0x%x error\12\0"
	.text
	.globl	_Z10out_regionym
	.def	_Z10out_regionym;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z10out_regionym
_Z10out_regionym:
.LFB2542:
	pushq	%rbp
	.seh_pushreg	%rbp
	subq	$1616, %rsp
	.seh_stackalloc	1616
	leaq	128(%rsp), %rbp
	.seh_setframe	%rbp, 128
	.seh_endprologue
	movq	%rcx, 1504(%rbp)
	movl	%edx, 1512(%rbp)
	cmpl	$32, 1512(%rbp)
	jbe	.L609
	movl	$32, 1512(%rbp)
.L609:
	movl	1512(%rbp), %eax
	addl	$1, %eax
	movl	%eax, %eax
	movq	%rax, %rcx
	call	malloc
	movq	%rax, 1480(%rbp)
	movl	1512(%rbp), %eax
	sall	$5, %eax
	movl	%eax, %eax
	movq	%rax, %rcx
	call	malloc
	movq	%rax, 1472(%rbp)
	movl	1512(%rbp), %eax
	sall	$5, %eax
	movl	%eax, %edx
	movq	1472(%rbp), %rax
	movq	%rdx, %r8
	movl	$0, %edx
	movq	%rax, %rcx
	call	memset
	movq	my_trace(%rip), %rax
	movq	68981528(%rax), %rdx
	leaq	688(%rbp), %rax
	movq	%rdx, %r8
	leaq	.LC262(%rip), %rdx
	movq	%rax, %rcx
	call	sprintf
	cmpq	$0, 1480(%rbp)
	jne	.L610
	leaq	-80(%rbp), %rax
	movabsq	$2318545059033805381, %rdx
	movq	%rdx, (%rax)
	movabsq	$8462103605467705198, %rdx
	movq	%rdx, 8(%rax)
	movabsq	$8245929725128632423, %rdx
	movq	%rdx, 16(%rax)
	movw	$121, 24(%rax)
	movq	my_trace(%rip), %rax
	leaq	67932940(%rax), %rcx
	leaq	-80(%rbp), %rax
	movq	%rax, %rdx
	call	strcpy
	movl	$0, %eax
	jmp	.L616
.L610:
	cmpq	$0, 1472(%rbp)
	jne	.L612
	leaq	-80(%rbp), %rax
	movabsq	$2318545059033805381, %rcx
	movq	%rcx, (%rax)
	movabsq	$8462103605467705198, %rdx
	movq	%rdx, 8(%rax)
	movabsq	$8245929725128632423, %rcx
	movq	%rcx, 16(%rax)
	movw	$121, 24(%rax)
	movq	my_trace(%rip), %rax
	leaq	67932940(%rax), %rcx
	leaq	-80(%rbp), %rax
	movq	%rax, %rdx
	call	strcpy
	movl	$0, %eax
	jmp	.L616
.L612:
	movq	my_trace(%rip), %rax
	movq	136(%rax), %rdx
	movl	1512(%rbp), %eax
	movq	%rdx, %r9
	movq	1504(%rbp), %r8
	movl	%eax, %edx
	leaq	.LC263(%rip), %rcx
	call	_Z7d_printPKcz
	movl	1512(%rbp), %r9d
	movq	1504(%rbp), %rdx
	movq	my_trace(%rip), %rax
	movq	136(%rax), %rax
	movq	1480(%rbp), %r8
	leaq	1456(%rbp), %rcx
	movq	%rcx, 32(%rsp)
	movq	%rax, %rcx
	call	_Z11read_memoryPvS_S_yPy
	movq	my_trace(%rip), %rax
	movq	68981528(%rax), %rdx
	leaq	688(%rbp), %rax
	movq	%rdx, %r8
	leaq	.LC264(%rip), %rdx
	movq	%rax, %rcx
	call	sprintf
	leaq	688(%rbp), %rax
	movq	%rax, %rcx
	call	_Z13add_to_bufferPc
	movq	1456(%rbp), %rax
	testq	%rax, %rax
	je	.L613
	movl	1512(%rbp), %ecx
	movq	1472(%rbp), %rdx
	movq	1480(%rbp), %rax
	movl	%ecx, %r8d
	movq	%rax, %rcx
	call	_Z6hexifyPcS_j
	movl	%eax, 1468(%rbp)
	cmpl	$0, 1468(%rbp)
	je	.L614
	leaq	-80(%rbp), %rax
	movabsq	$8458921675976766021, %rdx
	movq	%rdx, (%rax)
	movabsq	$8675454745582659954, %rcx
	movq	%rcx, 8(%rax)
	movl	$7956073, 16(%rax)
	movq	my_trace(%rip), %rax
	leaq	67932940(%rax), %rcx
	leaq	-80(%rbp), %rax
	movq	%rax, %rdx
	call	strcpy
	movl	$0, %eax
	jmp	.L616
.L614:
	movq	my_trace(%rip), %rax
	movl	68981968(%rax), %edx
	movq	1472(%rbp), %rcx
	leaq	688(%rbp), %rax
	movq	%rcx, %r9
	movl	%edx, %r8d
	leaq	.LC265(%rip), %rdx
	movq	%rax, %rcx
	call	sprintf
	leaq	688(%rbp), %rax
	movq	%rax, %rcx
	call	_Z13add_to_bufferPc
	jmp	.L615
.L613:
	movq	__imp_GetLastError(%rip), %rax
	call	*%rax
	movl	%eax, %edx
	leaq	-80(%rbp), %rax
	movl	%edx, %r8d
	leaq	.LC266(%rip), %rdx
	movq	%rax, %rcx
	call	sprintf
	movq	my_trace(%rip), %rax
	leaq	67932940(%rax), %rcx
	leaq	-80(%rbp), %rax
	movq	%rax, %rdx
	call	strcpy
	movq	my_trace(%rip), %rax
	movl	68981968(%rax), %edx
	movq	1472(%rbp), %rcx
	leaq	688(%rbp), %rax
	movq	%rcx, %r9
	movl	%edx, %r8d
	leaq	.LC267(%rip), %rdx
	movq	%rax, %rcx
	call	sprintf
	leaq	688(%rbp), %rax
	movq	%rax, %rcx
	call	_Z13add_to_bufferPc
.L615:
	movq	1480(%rbp), %rax
	movq	%rax, %rcx
	call	free
	movq	1472(%rbp), %rax
	movq	%rax, %rcx
	call	free
	movl	$0, %eax
.L616:
	addq	$1616, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.globl	_Z13report_regionym
	.def	_Z13report_regionym;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z13report_regionym
_Z13report_regionym:
.LFB2543:
	pushq	%rbp
	.seh_pushreg	%rbp
	subq	$848, %rsp
	.seh_stackalloc	848
	leaq	128(%rsp), %rbp
	.seh_setframe	%rbp, 128
	.seh_endprologue
	movq	%rcx, 736(%rbp)
	movl	%edx, 744(%rbp)
	cmpl	$256, 744(%rbp)
	jbe	.L618
	movl	$256, 744(%rbp)
.L618:
	movl	744(%rbp), %eax
	addl	$1, %eax
	movl	%eax, %eax
	movq	%rax, %rcx
	call	malloc
	movq	%rax, 712(%rbp)
	movl	744(%rbp), %eax
	sall	$5, %eax
	movl	%eax, %eax
	movq	%rax, %rcx
	call	malloc
	movq	%rax, 704(%rbp)
	cmpq	$0, 712(%rbp)
	jne	.L619
	leaq	-80(%rbp), %rax
	movabsq	$2318545059033805381, %rdx
	movq	%rdx, (%rax)
	movabsq	$8462103605467705198, %rdx
	movq	%rdx, 8(%rax)
	movabsq	$8245929725128632423, %rdx
	movq	%rdx, 16(%rax)
	movw	$121, 24(%rax)
	movq	my_trace(%rip), %rax
	leaq	67932940(%rax), %rcx
	leaq	-80(%rbp), %rax
	movq	%rax, %rdx
	call	strcpy
	movl	$0, %eax
	jmp	.L625
.L619:
	cmpq	$0, 704(%rbp)
	jne	.L621
	leaq	-80(%rbp), %rax
	movabsq	$2318545059033805381, %rdx
	movq	%rdx, (%rax)
	movabsq	$8462103605467705198, %rdx
	movq	%rdx, 8(%rax)
	movabsq	$8245929725128632423, %rcx
	movq	%rcx, 16(%rax)
	movw	$121, 24(%rax)
	movq	my_trace(%rip), %rax
	leaq	67932940(%rax), %rcx
	leaq	-80(%rbp), %rax
	movq	%rax, %rdx
	call	strcpy
	movl	$0, %eax
	jmp	.L625
.L621:
	movq	my_trace(%rip), %rax
	movq	136(%rax), %rdx
	movl	744(%rbp), %eax
	movq	%rdx, %r9
	movq	736(%rbp), %r8
	movl	%eax, %edx
	leaq	.LC263(%rip), %rcx
	call	_Z7d_printPKcz
	movl	744(%rbp), %r9d
	movq	736(%rbp), %rdx
	movq	my_trace(%rip), %rax
	movq	136(%rax), %rax
	movq	712(%rbp), %r8
	leaq	688(%rbp), %rcx
	movq	%rcx, 32(%rsp)
	movq	%rax, %rcx
	call	_Z11read_memoryPvS_S_yPy
	movl	744(%rbp), %edx
	movq	688(%rbp), %rax
	cmpq	%rax, %rdx
	jne	.L622
	movl	744(%rbp), %ecx
	movq	704(%rbp), %rdx
	movq	712(%rbp), %rax
	movl	%ecx, %r8d
	movq	%rax, %rcx
	call	_Z6hexifyPcS_j
	movl	%eax, 700(%rbp)
	cmpl	$0, 700(%rbp)
	je	.L623
	leaq	-80(%rbp), %rax
	movabsq	$8458921675976766021, %rdx
	movq	%rdx, (%rax)
	movabsq	$8675454745582659954, %rcx
	movq	%rcx, 8(%rax)
	movl	$7956073, 16(%rax)
	movq	my_trace(%rip), %rax
	leaq	67932940(%rax), %rcx
	leaq	-80(%rbp), %rax
	movq	%rax, %rdx
	call	strcpy
	movl	$0, %eax
	jmp	.L625
.L623:
	movq	my_trace(%rip), %rax
	addq	$67932940, %rax
	movw	$10, (%rax)
	movq	my_trace(%rip), %rax
	leaq	67932940(%rax), %rcx
	movq	704(%rbp), %rax
	movq	%rax, %rdx
	call	strcat
	jmp	.L624
.L622:
	movq	__imp_GetLastError(%rip), %rax
	call	*%rax
	movl	%eax, %edx
	leaq	-80(%rbp), %rax
	movl	%edx, %r8d
	leaq	.LC266(%rip), %rdx
	movq	%rax, %rcx
	call	sprintf
	movq	my_trace(%rip), %rax
	leaq	67932940(%rax), %rcx
	leaq	-80(%rbp), %rax
	movq	%rax, %rdx
	call	strcpy
.L624:
	movq	712(%rbp), %rax
	movq	%rax, %rcx
	call	free
	movq	704(%rbp), %rax
	movq	%rax, %rcx
	call	free
	movl	$0, %eax
.L625:
	addq	$848, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
	.align 8
.LC268:
	.ascii "Trying to read: @ %p, handle: 0x%08x\12\0"
.LC269:
	.ascii "%08x\0"
	.text
	.globl	_Z12report_dwordy
	.def	_Z12report_dwordy;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z12report_dwordy
_Z12report_dwordy:
.LFB2544:
	pushq	%rbp
	.seh_pushreg	%rbp
	subq	$832, %rsp
	.seh_stackalloc	832
	leaq	128(%rsp), %rbp
	.seh_setframe	%rbp, 128
	.seh_endprologue
	movq	%rcx, 720(%rbp)
	movq	my_trace(%rip), %rax
	movq	136(%rax), %rax
	movq	%rax, %r8
	movq	720(%rbp), %rdx
	leaq	.LC268(%rip), %rcx
	call	_Z7d_printPKcz
	movq	720(%rbp), %rdx
	movq	my_trace(%rip), %rax
	movq	136(%rax), %rax
	leaq	696(%rbp), %r8
	leaq	688(%rbp), %rcx
	movq	%rcx, 32(%rsp)
	movl	$4, %r9d
	movq	%rax, %rcx
	call	_Z11read_memoryPvS_S_yPy
	movq	688(%rbp), %rax
	cmpq	$4, %rax
	jne	.L627
	movq	696(%rbp), %rdx
	leaq	-80(%rbp), %rax
	movq	%rdx, %r8
	leaq	.LC269(%rip), %rdx
	movq	%rax, %rcx
	call	sprintf
	movq	my_trace(%rip), %rax
	leaq	67932940(%rax), %rcx
	leaq	-80(%rbp), %rax
	movq	%rax, %rdx
	call	strcpy
	jmp	.L628
.L627:
	movq	__imp_GetLastError(%rip), %rax
	call	*%rax
	movl	%eax, %edx
	leaq	-80(%rbp), %rax
	movl	%edx, %r8d
	leaq	.LC266(%rip), %rdx
	movq	%rax, %rcx
	call	sprintf
	movq	my_trace(%rip), %rax
	leaq	67932940(%rax), %rcx
	leaq	-80(%rbp), %rax
	movq	%rax, %rdx
	call	strcpy
.L628:
	movq	696(%rbp), %rax
	addq	$832, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC270:
	.ascii "%s written\12\0"
	.text
	.globl	_Z14write_registermPcS_
	.def	_Z14write_registermPcS_;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z14write_registermPcS_
_Z14write_registermPcS_:
.LFB2545:
	pushq	%rbp
	.seh_pushreg	%rbp
	subq	$2048, %rsp
	.seh_stackalloc	2048
	leaq	128(%rsp), %rbp
	.seh_setframe	%rbp, 128
	.seh_endprologue
	movl	%ecx, 1936(%rbp)
	movq	%rdx, 1944(%rbp)
	movq	%r8, 1952(%rbp)
	leaq	672(%rbp), %rax
	movq	%rax, %rdx
	movl	1936(%rbp), %ecx
	call	_Z12read_contextmP8_CONTEXT
	movq	1944(%rbp), %rax
	leaq	.LC28(%rip), %rdx
	movq	%rax, %rcx
	call	strcmp
	testl	%eax, %eax
	jne	.L631
	movq	1952(%rbp), %rax
	movl	$16, %r8d
	movl	$0, %edx
	movq	%rax, %rcx
	call	strtoul
	movl	%eax, %eax
	movq	%rax, 1912(%rbp)
	movq	1912(%rbp), %rax
	movq	%rax, 792(%rbp)
	leaq	672(%rbp), %rax
	movq	%rax, %rdx
	movl	1936(%rbp), %ecx
	call	_Z13write_contextmP8_CONTEXT
	movq	my_trace(%rip), %rax
	leaq	67932940(%rax), %rcx
	leaq	-96(%rbp), %rax
	movq	%rax, %rdx
	call	strcpy
	jmp	.L632
.L631:
	movq	1944(%rbp), %rax
	leaq	.LC29(%rip), %rdx
	movq	%rax, %rcx
	call	strcmp
	testl	%eax, %eax
	jne	.L633
	movq	1952(%rbp), %rax
	movl	$16, %r8d
	movl	$0, %edx
	movq	%rax, %rcx
	call	strtoul
	movl	%eax, %eax
	movq	%rax, 1912(%rbp)
	movq	1912(%rbp), %rax
	movq	%rax, 816(%rbp)
	leaq	672(%rbp), %rax
	movq	%rax, %rdx
	movl	1936(%rbp), %ecx
	call	_Z13write_contextmP8_CONTEXT
	movq	my_trace(%rip), %rax
	leaq	67932940(%rax), %rcx
	leaq	-96(%rbp), %rax
	movq	%rax, %rdx
	call	strcpy
	jmp	.L632
.L633:
	movq	1944(%rbp), %rax
	leaq	.LC30(%rip), %rdx
	movq	%rax, %rcx
	call	strcmp
	testl	%eax, %eax
	jne	.L634
	movq	1952(%rbp), %rax
	movl	$16, %r8d
	movl	$0, %edx
	movq	%rax, %rcx
	call	strtoul
	movl	%eax, %eax
	movq	%rax, 1912(%rbp)
	movq	1912(%rbp), %rax
	movq	%rax, 800(%rbp)
	leaq	672(%rbp), %rax
	movq	%rax, %rdx
	movl	1936(%rbp), %ecx
	call	_Z13write_contextmP8_CONTEXT
	movq	my_trace(%rip), %rax
	leaq	67932940(%rax), %rcx
	leaq	-96(%rbp), %rax
	movq	%rax, %rdx
	call	strcpy
	jmp	.L632
.L634:
	movq	1944(%rbp), %rax
	leaq	.LC31(%rip), %rdx
	movq	%rax, %rcx
	call	strcmp
	testl	%eax, %eax
	jne	.L635
	movq	1952(%rbp), %rax
	movl	$16, %r8d
	movl	$0, %edx
	movq	%rax, %rcx
	call	strtoul
	movl	%eax, %eax
	movq	%rax, 1912(%rbp)
	movq	1912(%rbp), %rax
	movq	%rax, 808(%rbp)
	leaq	672(%rbp), %rax
	movq	%rax, %rdx
	movl	1936(%rbp), %ecx
	call	_Z13write_contextmP8_CONTEXT
	movq	my_trace(%rip), %rax
	leaq	67932940(%rax), %rcx
	leaq	-96(%rbp), %rax
	movq	%rax, %rdx
	call	strcpy
	jmp	.L632
.L635:
	movq	1944(%rbp), %rax
	leaq	.LC33(%rip), %rdx
	movq	%rax, %rcx
	call	strcmp
	testl	%eax, %eax
	jne	.L636
	movq	1952(%rbp), %rax
	movl	$16, %r8d
	movl	$0, %edx
	movq	%rax, %rcx
	call	strtoul
	movl	%eax, %eax
	movq	%rax, 1912(%rbp)
	movq	1912(%rbp), %rax
	movq	%rax, 840(%rbp)
	leaq	672(%rbp), %rax
	movq	%rax, %rdx
	movl	1936(%rbp), %ecx
	call	_Z13write_contextmP8_CONTEXT
	movq	my_trace(%rip), %rax
	leaq	67932940(%rax), %rcx
	leaq	-96(%rbp), %rax
	movq	%rax, %rdx
	call	strcpy
	jmp	.L632
.L636:
	movq	1944(%rbp), %rax
	leaq	.LC32(%rip), %rdx
	movq	%rax, %rcx
	call	strcmp
	testl	%eax, %eax
	jne	.L637
	movq	1952(%rbp), %rax
	movl	$16, %r8d
	movl	$0, %edx
	movq	%rax, %rcx
	call	strtoul
	movl	%eax, %eax
	movq	%rax, 1912(%rbp)
	movq	1912(%rbp), %rax
	movq	%rax, 848(%rbp)
	leaq	672(%rbp), %rax
	movq	%rax, %rdx
	movl	1936(%rbp), %ecx
	call	_Z13write_contextmP8_CONTEXT
	movq	my_trace(%rip), %rax
	leaq	67932940(%rax), %rcx
	leaq	-96(%rbp), %rax
	movq	%rax, %rdx
	call	strcpy
	jmp	.L632
.L637:
	movq	1944(%rbp), %rax
	leaq	.LC209(%rip), %rdx
	movq	%rax, %rcx
	call	strcmp
	testl	%eax, %eax
	jne	.L638
	movq	1952(%rbp), %rax
	movl	$16, %r8d
	movl	$0, %edx
	movq	%rax, %rcx
	call	strtoul
	movl	%eax, %eax
	movq	%rax, 1912(%rbp)
	movq	1912(%rbp), %rax
	movq	%rax, 832(%rbp)
	leaq	672(%rbp), %rax
	movq	%rax, %rdx
	movl	1936(%rbp), %ecx
	call	_Z13write_contextmP8_CONTEXT
	movq	my_trace(%rip), %rax
	leaq	67932940(%rax), %rcx
	leaq	-96(%rbp), %rax
	movq	%rax, %rdx
	call	strcpy
	jmp	.L632
.L638:
	movq	1944(%rbp), %rax
	leaq	.LC34(%rip), %rdx
	movq	%rax, %rcx
	call	strcmp
	testl	%eax, %eax
	jne	.L639
	movq	1952(%rbp), %rax
	movl	$16, %r8d
	movl	$0, %edx
	movq	%rax, %rcx
	call	strtoul
	movl	%eax, %eax
	movq	%rax, 1912(%rbp)
	movq	1912(%rbp), %rax
	movq	%rax, 824(%rbp)
	leaq	672(%rbp), %rax
	movq	%rax, %rdx
	movl	1936(%rbp), %ecx
	call	_Z13write_contextmP8_CONTEXT
	movq	my_trace(%rip), %rax
	leaq	67932940(%rax), %rcx
	leaq	-96(%rbp), %rax
	movq	%rax, %rdx
	call	strcpy
	jmp	.L632
.L639:
	movq	1944(%rbp), %rax
	leaq	.LC35(%rip), %rdx
	movq	%rax, %rcx
	call	strcmp
	testl	%eax, %eax
	jne	.L632
	movq	1952(%rbp), %rax
	movl	$16, %r8d
	movl	$0, %edx
	movq	%rax, %rcx
	call	strtoul
	movl	%eax, %eax
	movq	%rax, 1912(%rbp)
	movq	1912(%rbp), %rax
	movq	%rax, 920(%rbp)
	leaq	672(%rbp), %rax
	movq	%rax, %rdx
	movl	1936(%rbp), %ecx
	call	_Z13write_contextmP8_CONTEXT
	movq	my_trace(%rip), %rax
	leaq	67932940(%rax), %rcx
	leaq	-96(%rbp), %rax
	movq	%rax, %rdx
	call	strcpy
.L632:
	movq	my_trace(%rip), %rax
	movl	$7, 67932936(%rax)
	movq	1944(%rbp), %rdx
	leaq	-96(%rbp), %rax
	movq	%rdx, %r8
	leaq	.LC270(%rip), %rdx
	movq	%rax, %rcx
	call	sprintf
	movq	my_trace(%rip), %rax
	leaq	67932940(%rax), %rcx
	leaq	-96(%rbp), %rax
	movq	%rax, %rdx
	call	strcpy
	movl	$0, %eax
	addq	$2048, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC271:
	.ascii "Rip\0"
.LC272:
	.ascii "EFLAGS\0"
.LC273:
	.ascii "Error reading register %s\12\0"
	.text
	.globl	_Z13read_registermPc
	.def	_Z13read_registermPc;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z13read_registermPc
_Z13read_registermPc:
.LFB2546:
	pushq	%rbp
	.seh_pushreg	%rbp
	subq	$1264, %rsp
	.seh_stackalloc	1264
	leaq	128(%rsp), %rbp
	.seh_setframe	%rbp, 128
	.seh_endprologue
	movl	%ecx, 1152(%rbp)
	movq	%rdx, 1160(%rbp)
	cmpl	$0, 1152(%rbp)
	jne	.L642
	movq	my_trace(%rip), %rax
	movl	68981632(%rax), %eax
	movl	%eax, 1152(%rbp)
.L642:
	leaq	-96(%rbp), %rax
	movq	%rax, %rdx
	movl	1152(%rbp), %ecx
	call	_Z12read_contextmP8_CONTEXT
	movq	1160(%rbp), %rax
	leaq	.LC28(%rip), %rdx
	movq	%rax, %rcx
	call	strcmp
	testl	%eax, %eax
	jne	.L643
	movq	24(%rbp), %rax
	jmp	.L653
.L643:
	movq	1160(%rbp), %rax
	leaq	.LC29(%rip), %rdx
	movq	%rax, %rcx
	call	strcmp
	testl	%eax, %eax
	jne	.L645
	movq	48(%rbp), %rax
	jmp	.L653
.L645:
	movq	1160(%rbp), %rax
	leaq	.LC30(%rip), %rdx
	movq	%rax, %rcx
	call	strcmp
	testl	%eax, %eax
	jne	.L646
	movq	32(%rbp), %rax
	jmp	.L653
.L646:
	movq	1160(%rbp), %rax
	leaq	.LC31(%rip), %rdx
	movq	%rax, %rcx
	call	strcmp
	testl	%eax, %eax
	jne	.L647
	movq	40(%rbp), %rax
	jmp	.L653
.L647:
	movq	1160(%rbp), %rax
	leaq	.LC32(%rip), %rdx
	movq	%rax, %rcx
	call	strcmp
	testl	%eax, %eax
	jne	.L648
	movq	80(%rbp), %rax
	jmp	.L653
.L648:
	movq	1160(%rbp), %rax
	leaq	.LC209(%rip), %rdx
	movq	%rax, %rcx
	call	strcmp
	testl	%eax, %eax
	jne	.L649
	movq	64(%rbp), %rax
	jmp	.L653
.L649:
	movq	1160(%rbp), %rax
	leaq	.LC34(%rip), %rdx
	movq	%rax, %rcx
	call	strcmp
	testl	%eax, %eax
	jne	.L650
	movq	56(%rbp), %rax
	jmp	.L653
.L650:
	movq	1160(%rbp), %rax
	leaq	.LC271(%rip), %rdx
	movq	%rax, %rcx
	call	strcmp
	testl	%eax, %eax
	jne	.L651
	movq	152(%rbp), %rax
	jmp	.L653
.L651:
	movq	1160(%rbp), %rax
	leaq	.LC272(%rip), %rdx
	movq	%rax, %rcx
	call	strcmp
	testl	%eax, %eax
	jne	.L652
	movl	-28(%rbp), %eax
	movl	%eax, %eax
	jmp	.L653
.L652:
	movq	1160(%rbp), %rax
	movq	%rax, %rdx
	leaq	.LC273(%rip), %rcx
	call	_Z7d_printPKcz
	movq	$-1, %rax
.L653:
	addq	$1264, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.globl	_Z15report_registermPc
	.def	_Z15report_registermPc;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z15report_registermPc
_Z15report_registermPc:
.LFB2547:
	pushq	%rbp
	.seh_pushreg	%rbp
	subq	$1280, %rsp
	.seh_stackalloc	1280
	leaq	128(%rsp), %rbp
	.seh_setframe	%rbp, 128
	.seh_endprologue
	movl	%ecx, 1168(%rbp)
	movq	%rdx, 1176(%rbp)
	movq	my_trace(%rip), %rax
	movl	$7, 67932936(%rax)
	movq	1176(%rbp), %rax
	movq	%rax, %rdx
	movl	1168(%rbp), %ecx
	call	_Z13read_registermPc
	movq	%rax, 1144(%rbp)
	movq	1144(%rbp), %rdx
	leaq	-96(%rbp), %rax
	movq	%rdx, %r8
	leaq	.LC269(%rip), %rdx
	movq	%rax, %rcx
	call	sprintf
	movq	my_trace(%rip), %rax
	leaq	67932940(%rax), %rcx
	leaq	-96(%rbp), %rax
	movq	%rax, %rdx
	call	strcpy
	movl	$0, %eax
	addq	$1280, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC274:
	.ascii "0x%08x:0x%08x\0"
.LC275:
	.ascii "new buffer: %p - %s\12\0"
	.text
	.globl	_Z10read_stackmm
	.def	_Z10read_stackmm;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z10read_stackmm
_Z10read_stackmm:
.LFB2548:
	pushq	%rbp
	.seh_pushreg	%rbp
	pushq	%rdi
	.seh_pushreg	%rdi
	subq	$2072, %rsp
	.seh_stackalloc	2072
	leaq	128(%rsp), %rbp
	.seh_setframe	%rbp, 128
	.seh_endprologue
	movl	%ecx, 1968(%rbp)
	movl	%edx, 1976(%rbp)
	movq	my_trace(%rip), %rax
	movl	$7, 67932936(%rax)
	leaq	672(%rbp), %rax
	movq	%rax, %rdx
	movl	1968(%rbp), %ecx
	call	_Z12read_contextmP8_CONTEXT
	movq	824(%rbp), %rax
	movq	%rax, 1928(%rbp)
	movq	my_trace(%rip), %rax
	addq	$67932940, %rax
	movl	$1048576, %r8d
	movl	$0, %edx
	movq	%rax, %rcx
	call	memset
	movl	$0, 1924(%rbp)
	jmp	.L657
.L658:
	movq	1928(%rbp), %rax
	movq	%rax, %rcx
	call	_Z10read_dwordy
	cltq
	movq	%rax, 1912(%rbp)
	leaq	-96(%rbp), %rax
	movl	$768, %r8d
	movl	$0, %edx
	movq	%rax, %rcx
	call	memset
	movq	1912(%rbp), %rcx
	movq	1928(%rbp), %rdx
	leaq	-96(%rbp), %rax
	movq	%rcx, %r9
	movq	%rdx, %r8
	leaq	.LC274(%rip), %rdx
	movq	%rax, %rcx
	call	sprintf
	movq	my_trace(%rip), %rax
	leaq	67932940(%rax), %rcx
	leaq	-96(%rbp), %rax
	movq	%rax, %rdx
	call	strcat
	movq	my_trace(%rip), %rax
	leaq	67932940(%rax), %rdx
	movq	%rdx, %rax
	movq	$-1, %rcx
	movq	%rax, %r8
	movl	$0, %eax
	movq	%r8, %rdi
	repnz scasb
	movq	%rcx, %rax
	notq	%rax
	subq	$1, %rax
	addq	%rdx, %rax
	movw	$10, (%rax)
	addl	$1, 1924(%rbp)
	addq	$4, 1928(%rbp)
.L657:
	movl	1924(%rbp), %eax
	cmpl	1976(%rbp), %eax
	jb	.L658
	movq	my_trace(%rip), %rax
	leaq	67932940(%rax), %rdx
	movq	my_trace(%rip), %rax
	addq	$67932940, %rax
	movq	%rdx, %r8
	movq	%rax, %rdx
	leaq	.LC275(%rip), %rcx
	call	_Z7d_printPKcz
	movl	$0, %eax
	addq	$2072, %rsp
	popq	%rdi
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC276:
	.ascii "RL,0x%08x,self\12\0"
	.text
	.globl	_Z13register_selfy
	.def	_Z13register_selfy;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z13register_selfy
_Z13register_selfy:
.LFB2549:
	pushq	%rbp
	.seh_pushreg	%rbp
	subq	$800, %rsp
	.seh_stackalloc	800
	leaq	128(%rsp), %rbp
	.seh_setframe	%rbp, 128
	.seh_endprologue
	movq	%rcx, 688(%rbp)
	movq	688(%rbp), %rdx
	leaq	.LC276(%rip), %rcx
	call	_Z7d_printPKcz
	leaq	-96(%rbp), %rax
	movq	688(%rbp), %r8
	leaq	.LC276(%rip), %rdx
	movq	%rax, %rcx
	call	sprintf
	leaq	-96(%rbp), %rax
	movq	%rax, %rcx
	call	_Z13add_to_bufferPc
	movl	$0, %eax
	addq	$800, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
	.align 8
.LC277:
	.ascii "Created process, checking breakpoints\12\0"
	.align 8
.LC278:
	.ascii "bp: %s, resolved_location: 0x%08x\12\0"
	.align 8
.LC279:
	.ascii "Breakpoint hit! @ 0x%08x, instr_count: %d\12\0"
.LC280:
	.ascii "[BP handling]\12\0"
.LC281:
	.ascii "Handling breakpoint @ 0x%08x\12\0"
	.align 8
.LC282:
	.ascii "This BP is not our, we pass it to the debugee\12\0"
.LC283:
	.ascii "[BP handling ends]\12\0"
.LC284:
	.ascii "other\12\0"
	.align 8
.LC285:
	.ascii "Loaded library, checking breakpoints\12\0"
.LC286:
	.ascii "Exiting thread\12\0"
.LC287:
	.ascii "Exiting process\12\0"
	.text
	.globl	_Z18process_last_eventv
	.def	_Z18process_last_eventv;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z18process_last_eventv
_Z18process_last_eventv:
.LFB2550:
	pushq	%rbp
	.seh_pushreg	%rbp
	pushq	%rdi
	.seh_pushreg	%rdi
	pushq	%rsi
	.seh_pushreg	%rsi
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$224, %rsp
	.seh_stackalloc	224
	.seh_endprologue
	movq	my_trace(%rip), %rax
	movq	my_trace(%rip), %rdx
	movl	68981632(%rdx), %edx
	movl	%edx, 68981968(%rax)
	movq	my_trace(%rip), %rax
	movl	68981624(%rax), %eax
	cmpl	$7, %eax
	ja	.L663
	movl	%eax, %eax
	leaq	0(,%rax,4), %rdx
	leaq	.L665(%rip), %rax
	movl	(%rdx,%rax), %eax
	movslq	%eax, %rdx
	leaq	.L665(%rip), %rax
	addq	%rdx, %rax
	jmp	*%rax
	.section .rdata,"dr"
	.align 4
.L665:
	.long	.L663-.L665
	.long	.L664-.L665
	.long	.L666-.L665
	.long	.L667-.L665
	.long	.L668-.L665
	.long	.L669-.L665
	.long	.L670-.L665
	.long	.L671-.L665
	.text
.L667:
	movq	my_trace(%rip), %rax
	movq	my_trace(%rip), %rdx
	movq	68981640(%rdx), %rcx
	movq	%rcx, 128(%rax)
	movq	68981648(%rdx), %rcx
	movq	%rcx, 136(%rax)
	movq	68981656(%rdx), %rcx
	movq	%rcx, 144(%rax)
	movq	68981664(%rdx), %rcx
	movq	%rcx, 152(%rax)
	movq	68981672(%rdx), %rcx
	movq	%rcx, 160(%rax)
	movq	68981680(%rdx), %rcx
	movq	%rcx, 168(%rax)
	movq	68981688(%rdx), %rcx
	movq	%rcx, 176(%rax)
	movq	68981696(%rdx), %rcx
	movq	%rcx, 184(%rax)
	movq	68981704(%rdx), %rdx
	movq	%rdx, 192(%rax)
	movq	my_trace(%rip), %rax
	movq	my_trace(%rip), %rdx
	movq	136(%rdx), %rdx
	movq	%rdx, 208(%rax)
	movq	my_trace(%rip), %rax
	movq	68981656(%rax), %rdx
	movq	my_trace(%rip), %rax
	movl	68981632(%rax), %eax
	movl	%eax, %ecx
	call	_Z15register_threadmPv
	movq	my_trace(%rip), %rax
	movq	152(%rax), %rax
	movq	%rax, %rcx
	call	_Z13register_selfy
	leaq	.LC277(%rip), %rcx
	call	_Z7d_printPKcz
	movl	$0, -4(%rbp)
	jmp	.L672
.L674:
	movq	my_trace(%rip), %rdx
	movl	-4(%rbp), %eax
	imulq	$552, %rax, %rax
	addq	%rdx, %rax
	addq	$67650288, %rax
	movq	16(%rax), %rdx
	movq	my_trace(%rip), %rax
	movl	-4(%rbp), %ecx
	imulq	$552, %rcx, %rcx
	addq	$67649776, %rcx
	addq	%rcx, %rax
	addq	$16, %rax
	movq	%rdx, %r8
	movq	%rax, %rdx
	leaq	.LC278(%rip), %rcx
	call	_Z7d_printPKcz
	movq	my_trace(%rip), %rdx
	movl	-4(%rbp), %eax
	imulq	$552, %rax, %rax
	addq	%rdx, %rax
	addq	$67650288, %rax
	movq	16(%rax), %rax
	cmpq	$-1, %rax
	jne	.L673
	movq	my_trace(%rip), %rax
	movl	-4(%rbp), %edx
	imulq	$552, %rdx, %rdx
	addq	$67649776, %rdx
	addq	%rdx, %rax
	addq	$8, %rax
	movq	%rax, %rcx
	call	_Z17update_breakpointP11BREAKPOINT_
.L673:
	addl	$1, -4(%rbp)
.L672:
	movl	-4(%rbp), %edx
	movq	my_trace(%rip), %rax
	movq	68981576(%rax), %rax
	cmpq	%rax, %rdx
	jb	.L674
	movl	$1, %eax
	jmp	.L662
.L664:
	movl	$-2147418111, -16(%rbp)
	movq	my_trace(%rip), %rax
	movq	my_trace(%rip), %rdx
	addq	$68981800, %rax
	addq	$68981640, %rdx
	movl	$19, %ecx
	movq	%rax, %rdi
	movq	%rdx, %rsi
	rep movsq
	movq	my_trace(%rip), %rax
	movl	68981800(%rax), %eax
	cmpl	$-2147483645, %eax
	je	.L677
	cmpl	$-2147483644, %eax
	jne	.L687
	movq	my_trace(%rip), %rax
	addq	$68981624, %rax
	movq	%rax, %rcx
	call	_Z11ss_callbackPv
	movq	my_trace(%rip), %rax
	movl	68981632(%rax), %eax
	movl	%eax, %ecx
	call	_Z6set_ssm
	movq	my_trace(%rip), %rax
	movl	$65538, 68981952(%rax)
	movl	$0, %eax
	jmp	.L662
.L677:
	movq	my_trace(%rip), %rax
	movq	68981816(%rax), %rax
	movq	%rax, -24(%rbp)
	movq	my_trace(%rip), %rax
	movq	68981528(%rax), %rdx
	movq	-24(%rbp), %rax
	movq	%rdx, %r8
	movq	%rax, %rdx
	leaq	.LC279(%rip), %rcx
	call	_Z7d_printPKcz
	leaq	.LC280(%rip), %rcx
	call	_Z7d_printPKcz
	movb	$0, -9(%rbp)
	movq	my_trace(%rip), %rax
	movl	$0, 67932936(%rax)
	movl	$0, -8(%rbp)
	jmp	.L679
.L681:
	movq	my_trace(%rip), %rdx
	movl	-8(%rbp), %eax
	imulq	$552, %rax, %rax
	addq	%rdx, %rax
	addq	$67650288, %rax
	movq	16(%rax), %rax
	cmpq	-24(%rbp), %rax
	jne	.L680
	movq	-24(%rbp), %rax
	movq	%rax, %rdx
	leaq	.LC281(%rip), %rcx
	call	_Z7d_printPKcz
	movq	my_trace(%rip), %rax
	leaq	68981624(%rax), %rdx
	movq	my_trace(%rip), %rax
	movq	68981816(%rax), %rax
	movq	%rax, %rcx
	call	_Z17handle_breakpointyPv
	movb	$1, -9(%rbp)
.L680:
	addl	$1, -8(%rbp)
.L679:
	movl	-8(%rbp), %edx
	movq	my_trace(%rip), %rax
	movq	68981576(%rax), %rax
	cmpq	%rax, %rdx
	jb	.L681
	cmpb	$0, -9(%rbp)
	jne	.L682
	leaq	.LC282(%rip), %rcx
	call	_Z7d_printPKcz
	movq	my_trace(%rip), %rax
	movl	$8, 67932936(%rax)
.L682:
	leaq	.LC283(%rip), %rcx
	call	_Z7d_printPKcz
	movq	my_trace(%rip), %rax
	movl	67932936(%rax), %eax
	jmp	.L662
.L687:
	leaq	.LC284(%rip), %rcx
	call	_Z7d_printPKcz
	movq	my_trace(%rip), %rdx
	movq	my_trace(%rip), %rax
	movl	68981632(%rax), %r8d
	leaq	-192(%rbp), %rax
	addq	$68981800, %rdx
	movl	$19, %ecx
	movq	%rax, %rdi
	movq	%rdx, %rsi
	rep movsq
	leaq	-192(%rbp), %rax
	movq	%rax, %rdx
	movl	%r8d, %ecx
	call	_Z18register_exceptionm17_EXCEPTION_RECORD
	movq	my_trace(%rip), %rax
	addq	$68981624, %rax
	movq	%rax, %rcx
	call	_Z11ss_callbackPv
	movl	$6, %eax
	jmp	.L662
.L666:
	movq	my_trace(%rip), %rax
	movq	68981640(%rax), %rdx
	movq	my_trace(%rip), %rax
	movl	68981632(%rax), %eax
	movl	%eax, %ecx
	call	_Z15register_threadmPv
	movq	my_trace(%rip), %rax
	movl	$65538, 68981952(%rax)
	movl	$0, %eax
	jmp	.L662
.L670:
	movq	my_trace(%rip), %rax
	movq	68981640(%rax), %rdx
	movq	%rdx, -192(%rbp)
	movq	68981648(%rax), %rdx
	movq	%rdx, -184(%rbp)
	movq	68981656(%rax), %rdx
	movq	%rdx, -176(%rbp)
	movq	68981664(%rax), %rdx
	movq	%rdx, -168(%rbp)
	movq	68981672(%rax), %rax
	movq	%rax, -160(%rbp)
	leaq	-192(%rbp), %rax
	movq	%rax, %rcx
	call	_Z12register_lib20_LOAD_DLL_DEBUG_INFO
	leaq	.LC285(%rip), %rcx
	call	_Z7d_printPKcz
	movl	$0, -4(%rbp)
	jmp	.L683
.L685:
	movq	my_trace(%rip), %rdx
	movl	-4(%rbp), %eax
	imulq	$552, %rax, %rax
	addq	%rdx, %rax
	addq	$67650288, %rax
	movq	16(%rax), %rdx
	movq	my_trace(%rip), %rax
	movl	-4(%rbp), %ecx
	imulq	$552, %rcx, %rcx
	addq	$67649776, %rcx
	addq	%rcx, %rax
	addq	$16, %rax
	movq	%rdx, %r8
	movq	%rax, %rdx
	leaq	.LC278(%rip), %rcx
	call	_Z7d_printPKcz
	movq	my_trace(%rip), %rdx
	movl	-4(%rbp), %eax
	imulq	$552, %rax, %rax
	addq	%rdx, %rax
	addq	$67650288, %rax
	movq	16(%rax), %rax
	cmpq	$-1, %rax
	jne	.L684
	movq	my_trace(%rip), %rax
	movl	-4(%rbp), %edx
	imulq	$552, %rdx, %rdx
	addq	$67649776, %rdx
	addq	%rdx, %rax
	addq	$8, %rax
	movq	%rax, %rcx
	call	_Z17update_breakpointP11BREAKPOINT_
.L684:
	addl	$1, -4(%rbp)
.L683:
	movl	-4(%rbp), %edx
	movq	my_trace(%rip), %rax
	movq	68981576(%rax), %rax
	cmpq	%rax, %rdx
	jb	.L685
	movq	my_trace(%rip), %rax
	movl	$65538, 68981952(%rax)
	movl	$0, %eax
	jmp	.L662
.L671:
	movq	my_trace(%rip), %rax
	movq	68981640(%rax), %rax
	movq	%rax, %rcx
	call	_Z14deregister_lib22_UNLOAD_DLL_DEBUG_INFO
	movl	$0, %eax
	jmp	.L662
.L668:
	leaq	.LC286(%rip), %rcx
	call	_Z7d_printPKcz
	movq	my_trace(%rip), %rax
	movq	68981640(%rax), %rdx
	movq	my_trace(%rip), %rax
	movl	68981632(%rax), %eax
	movl	%eax, %ecx
	call	_Z17deregister_threadmPv
	movq	my_trace(%rip), %rdx
	movq	my_trace(%rip), %rax
	movl	68981632(%rax), %eax
	movl	%eax, %eax
	addq	$2097159, %rax
	salq	$5, %rax
	addq	%rdx, %rax
	movb	$0, 4(%rax)
	movq	my_trace(%rip), %rdx
	movq	my_trace(%rip), %rax
	movl	68981632(%rax), %eax
	movl	%eax, %eax
	addq	$2097159, %rax
	salq	$5, %rax
	addq	%rdx, %rax
	movq	8(%rax), %rax
	testq	%rax, %rax
	je	.L686
	movq	my_trace(%rip), %rdx
	movq	my_trace(%rip), %rax
	movl	68981632(%rax), %eax
	movl	%eax, %eax
	addq	$2097159, %rax
	salq	$5, %rax
	addq	%rdx, %rax
	movq	8(%rax), %rax
	movq	%rax, %rcx
	movq	__imp_CloseHandle(%rip), %rax
	call	*%rax
.L686:
	movq	my_trace(%rip), %rdx
	movq	my_trace(%rip), %rax
	movl	68981632(%rax), %eax
	movl	%eax, %eax
	addq	$2097159, %rax
	salq	$5, %rax
	addq	%rdx, %rax
	movq	$0, 8(%rax)
	movq	my_trace(%rip), %rdx
	movq	my_trace(%rip), %rax
	movl	68981632(%rax), %eax
	movl	%eax, %eax
	addq	$2097159, %rax
	salq	$5, %rax
	addq	%rdx, %rax
	movb	$0, 5(%rax)
	movq	my_trace(%rip), %rax
	movl	$65538, 68981952(%rax)
	movl	$0, %eax
	jmp	.L662
.L669:
	leaq	.LC287(%rip), %rcx
	call	_Z7d_printPKcz
	movl	$2, %eax
	jmp	.L662
.L663:
.L662:
	addq	$224, %rsp
	popq	%rsi
	popq	%rdi
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC288:
	.ascii "Exception: 0x%08x\12\0"
.LC289:
	.ascii "at: 0x%08x\12\0"
.LC290:
	.ascii "First chance: 0x%08x\12\0"
.LC291:
	.ascii "instr: %d\12\0"
	.text
	.globl	_Z13create_reportm
	.def	_Z13create_reportm;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z13create_reportm
_Z13create_reportm:
.LFB2551:
	pushq	%rbp
	.seh_pushreg	%rbp
	pushq	%rdi
	.seh_pushreg	%rdi
	pushq	%rsi
	.seh_pushreg	%rsi
	subq	$1728, %rsp
	.seh_stackalloc	1728
	leaq	128(%rsp), %rbp
	.seh_setframe	%rbp, 128
	.seh_endprologue
	movl	%ecx, 1632(%rbp)
	movl	1632(%rbp), %eax
	cmpl	$2, %eax
	je	.L690
	cmpl	$2, %eax
	ja	.L691
	cmpl	$1, %eax
	je	.L692
	jmp	.L689
.L691:
	cmpl	$5, %eax
	je	.L693
	cmpl	$6, %eax
	je	.L694
	jmp	.L689
.L692:
	leaq	64(%rbp), %rax
	movabsq	$5791440358316459346, %rsi
	movq	%rsi, (%rax)
	movabsq	$4854690540438114130, %rdi
	movq	%rdi, 8(%rax)
	movabsq	$2889814324102482, %rsi
	movq	%rsi, 16(%rax)
	jmp	.L695
.L693:
	movq	my_trace(%rip), %rax
	movq	68981976(%rax), %rax
	movq	%rax, %rdx
	leaq	64(%rbp), %rax
	movq	%rdx, %r8
	leaq	.LC196(%rip), %rdx
	movq	%rax, %rcx
	call	sprintf
	jmp	.L695
.L694:
	movq	my_trace(%rip), %rdx
	leaq	-96(%rbp), %rax
	addq	$68981640, %rdx
	movl	$19, %ecx
	movq	%rax, %rdi
	movq	%rdx, %rsi
	rep movsq
	leaq	64(%rbp), %rax
	movabsq	$4998806823899252050, %rdi
	movq	%rdi, (%rax)
	movabsq	$5642809484591317848, %rsi
	movq	%rsi, 8(%rax)
	movw	$10, 16(%rax)
	movl	-96(%rbp), %edx
	leaq	832(%rbp), %rax
	movl	%edx, %r8d
	leaq	.LC288(%rip), %rdx
	movq	%rax, %rcx
	call	sprintf
	leaq	832(%rbp), %rdx
	leaq	64(%rbp), %rax
	movq	%rax, %rcx
	call	strcat
	movq	-80(%rbp), %rdx
	leaq	832(%rbp), %rax
	movq	%rdx, %r8
	leaq	.LC289(%rip), %rdx
	movq	%rax, %rcx
	call	sprintf
	leaq	832(%rbp), %rdx
	leaq	64(%rbp), %rax
	movq	%rax, %rcx
	call	strcat
	movq	my_trace(%rip), %rax
	movl	68981792(%rax), %edx
	leaq	832(%rbp), %rax
	movl	%edx, %r8d
	leaq	.LC290(%rip), %rdx
	movq	%rax, %rcx
	call	sprintf
	leaq	832(%rbp), %rdx
	leaq	64(%rbp), %rax
	movq	%rax, %rcx
	call	strcat
	jmp	.L695
.L690:
	leaq	64(%rbp), %rax
	movabsq	$5791440358316459346, %rdi
	movq	%rdi, (%rax)
	movabsq	$4998805728513970002, %rsi
	movq	%rsi, 8(%rax)
	movl	$173295960, 16(%rax)
	movb	$0, 20(%rax)
	jmp	.L695
.L689:
	leaq	64(%rbp), %rax
	movabsq	$5719382764278531410, %rdi
	movq	%rdi, (%rax)
	movl	$1380272212, 8(%rax)
	movw	$10, 12(%rax)
	movq	my_trace(%rip), %rax
	movq	68981960(%rax), %rdx
	leaq	832(%rbp), %rax
	movq	%rdx, %r8
	leaq	.LC289(%rip), %rdx
	movq	%rax, %rcx
	call	sprintf
	leaq	832(%rbp), %rdx
	leaq	64(%rbp), %rax
	movq	%rax, %rcx
	call	strcat
	movq	my_trace(%rip), %rax
	movq	68981528(%rax), %rdx
	leaq	832(%rbp), %rax
	movq	%rdx, %r8
	leaq	.LC291(%rip), %rdx
	movq	%rax, %rcx
	call	sprintf
	leaq	832(%rbp), %rdx
	leaq	64(%rbp), %rax
	movq	%rax, %rcx
	call	strcat
	nop
.L695:
	movq	my_trace(%rip), %rax
	leaq	67932940(%rax), %rcx
	leaq	64(%rbp), %rax
	movq	%rax, %rdx
	call	strcpy
	movl	$0, %eax
	addq	$1728, %rsp
	popq	%rsi
	popq	%rdi
	popq	%rbp
	ret
	.seh_endproc
	.globl	_Z15handle_continuemmj
	.def	_Z15handle_continuemmj;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z15handle_continuemmj
_Z15handle_continuemmj:
.LFB2552:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$32, %rsp
	.seh_stackalloc	32
	.seh_endprologue
	movl	%ecx, 16(%rbp)
	movl	%edx, 24(%rbp)
	movl	%r8d, 32(%rbp)
	movl	32(%rbp), %edx
	movl	24(%rbp), %eax
	movl	%edx, %r8d
	movl	%eax, %edx
	movl	16(%rbp), %ecx
	movq	__imp_ContinueDebugEvent(%rip), %rax
	call	*%rax
	movl	$0, %eax
	addq	$32, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
	.align 8
.LC292:
	.ascii "Continuing: PID: 0x%08x, TID: 0x%08x\12\0"
.LC293:
	.ascii "Continuing\12\0"
.LC294:
	.ascii "Report buffer: %s\12\0"
	.text
	.globl	_Z18get_pending_eventsv
	.def	_Z18get_pending_eventsv;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z18get_pending_eventsv
_Z18get_pending_eventsv:
.LFB2553:
	pushq	%rbp
	.seh_pushreg	%rbp
	subq	$816, %rsp
	.seh_stackalloc	816
	leaq	128(%rsp), %rbp
	.seh_setframe	%rbp, 128
	.seh_endprologue
	movq	my_trace(%rip), %rax
	movl	68981632(%rax), %edx
	movq	my_trace(%rip), %rax
	movl	68981628(%rax), %eax
	movl	%edx, %r8d
	movl	%eax, %edx
	leaq	.LC292(%rip), %rcx
	call	_Z7d_printPKcz
	movq	my_trace(%rip), %rax
	movl	68981628(%rax), %eax
	testl	%eax, %eax
	jne	.L700
	movq	my_trace(%rip), %rax
	movl	20(%rax), %edx
	movq	my_trace(%rip), %rax
	movl	16(%rax), %eax
	movl	$65538, %r8d
	movl	%eax, %ecx
	call	_Z15handle_continuemmj
	jmp	.L701
.L700:
	movq	my_trace(%rip), %rax
	movl	68981632(%rax), %edx
	movq	my_trace(%rip), %rax
	movl	68981628(%rax), %eax
	movl	$65538, %r8d
	movl	%eax, %ecx
	call	_Z15handle_continuemmj
.L701:
	leaq	.LC293(%rip), %rcx
	call	_Z7d_printPKcz
.L708:
	movq	my_trace(%rip), %rax
	addq	$68981624, %rax
	movl	$0, %edx
	movq	%rax, %rcx
	movq	__imp_WaitForDebugEvent(%rip), %rax
	call	*%rax
	movq	my_trace(%rip), %rax
	movl	68981624(%rax), %eax
	testl	%eax, %eax
	jne	.L702
	movl	$153, %eax
	jmp	.L709
.L702:
	call	_Z18process_last_eventv
	movl	%eax, 680(%rbp)
	cmpl	$0, 680(%rbp)
	jne	.L704
	movl	$65538, 684(%rbp)
	jmp	.L705
.L704:
	cmpl	$8, 680(%rbp)
	jne	.L706
	movl	$-2147418111, 684(%rbp)
	jmp	.L705
.L706:
	nop
	movl	680(%rbp), %eax
	movl	%eax, %ecx
	call	_Z13create_reportm
	movq	my_trace(%rip), %rax
	addq	$67932940, %rax
	movq	%rax, %rdx
	leaq	.LC294(%rip), %rcx
	call	_Z7d_printPKcz
	movq	my_trace(%rip), %rax
	movl	680(%rbp), %edx
	movl	%edx, 67932936(%rax)
	movl	680(%rbp), %eax
	jmp	.L709
.L705:
	movq	my_trace(%rip), %rax
	movl	68981632(%rax), %edx
	movq	my_trace(%rip), %rax
	movl	68981628(%rax), %eax
	movl	684(%rbp), %ecx
	movl	%ecx, %r8d
	movl	%eax, %ecx
	call	_Z15handle_continuemmj
	jmp	.L708
.L709:
	addq	$816, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.globl	_Z16continue_routinemj
	.def	_Z16continue_routinemj;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z16continue_routinemj
_Z16continue_routinemj:
.LFB2554:
	pushq	%rbp
	.seh_pushreg	%rbp
	subq	$816, %rsp
	.seh_stackalloc	816
	leaq	128(%rsp), %rbp
	.seh_setframe	%rbp, 128
	.seh_endprologue
	movl	%ecx, 704(%rbp)
	movl	%edx, 712(%rbp)
	movl	712(%rbp), %eax
	movl	%eax, 684(%rbp)
	movq	my_trace(%rip), %rax
	movl	68981628(%rax), %eax
	testl	%eax, %eax
	jne	.L711
	movq	my_trace(%rip), %rax
	movl	20(%rax), %edx
	movq	my_trace(%rip), %rax
	movl	16(%rax), %eax
	movl	684(%rbp), %ecx
	movl	%ecx, %r8d
	movl	%eax, %ecx
	call	_Z15handle_continuemmj
	jmp	.L712
.L711:
	movq	my_trace(%rip), %rax
	movl	68981632(%rax), %edx
	movq	my_trace(%rip), %rax
	movl	68981628(%rax), %eax
	movl	684(%rbp), %ecx
	movl	%ecx, %r8d
	movl	%eax, %ecx
	call	_Z15handle_continuemmj
.L712:
	leaq	.LC293(%rip), %rcx
	call	_Z7d_printPKcz
.L717:
	movq	my_trace(%rip), %rax
	addq	$68981624, %rax
	movl	704(%rbp), %edx
	movq	%rax, %rcx
	movq	__imp_WaitForDebugEvent(%rip), %rax
	call	*%rax
	call	_Z18process_last_eventv
	movl	%eax, 680(%rbp)
	cmpl	$0, 680(%rbp)
	jne	.L713
	movl	$65538, 684(%rbp)
	jmp	.L714
.L713:
	cmpl	$8, 680(%rbp)
	jne	.L715
	movl	$-2147418111, 684(%rbp)
	jmp	.L714
.L715:
	jmp	.L719
.L714:
	movq	my_trace(%rip), %rax
	movl	68981632(%rax), %edx
	movq	my_trace(%rip), %rax
	movl	68981628(%rax), %eax
	movl	684(%rbp), %ecx
	movl	%ecx, %r8d
	movl	%eax, %ecx
	call	_Z15handle_continuemmj
	jmp	.L717
.L719:
	movl	680(%rbp), %eax
	movl	%eax, %ecx
	call	_Z13create_reportm
	movq	my_trace(%rip), %rax
	addq	$67932940, %rax
	movq	%rax, %rdx
	leaq	.LC294(%rip), %rcx
	call	_Z7d_printPKcz
	movq	my_trace(%rip), %rax
	movl	680(%rbp), %edx
	movl	%edx, 67932936(%rax)
	movl	680(%rbp), %eax
	addq	$816, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC295:
	.ascii "Resolving region\12\0"
.LC296:
	.ascii "Locating buffer\12\0"
.LC297:
	.ascii "LOCATION_CONST\12\0"
.LC298:
	.ascii "Arg off: 0x%08x\12\0"
.LC299:
	.ascii "LOCATION_MEM\12\0"
.LC300:
	.ascii "0x%08x: 0x%08x\12\0"
.LC301:
	.ascii "LOCATION_STACK\12\0"
.LC302:
	.ascii "LOCATION_ADDR_STACK\12\0"
.LC303:
	.ascii "LOCATION_ADDR_ADDR_STACK\12\0"
.LC304:
	.ascii "Locating size\12\0"
.LC305:
	.ascii "Arg size: 0x%08x\12\0"
	.align 8
.LC306:
	.ascii "[Resolved location: 0x%08x, size: 0x%08x]\12\0"
	.text
	.globl	_Z14resolve_regionP20LOCATION_DESCRIPTOR_P9LOCATION_
	.def	_Z14resolve_regionP20LOCATION_DESCRIPTOR_P9LOCATION_;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z14resolve_regionP20LOCATION_DESCRIPTOR_P9LOCATION_
_Z14resolve_regionP20LOCATION_DESCRIPTOR_P9LOCATION_:
.LFB2555:
	pushq	%rbp
	.seh_pushreg	%rbp
	subq	$1328, %rsp
	.seh_stackalloc	1328
	leaq	128(%rsp), %rbp
	.seh_setframe	%rbp, 128
	.seh_endprologue
	movq	%rcx, 1216(%rbp)
	movq	%rdx, 1224(%rbp)
	leaq	.LC295(%rip), %rcx
	call	_Z7d_printPKcz
	leaq	.LC296(%rip), %rcx
	call	_Z7d_printPKcz
	movq	my_trace(%rip), %rax
	movl	68981632(%rax), %eax
	leaq	-80(%rbp), %rdx
	movl	%eax, %ecx
	call	_Z12read_contextmP8_CONTEXT
	movq	72(%rbp), %rax
	movq	%rax, 1192(%rbp)
	movq	$0, 1160(%rbp)
	movq	$0, 1184(%rbp)
	movq	$0, 1176(%rbp)
	movq	$0, 1168(%rbp)
	movq	1192(%rbp), %rax
	movq	%rax, %rdx
	leaq	.LC41(%rip), %rcx
	call	_Z7d_printPKcz
	movq	1216(%rbp), %rax
	movzbl	16(%rax), %eax
	movsbl	%al, %eax
	cmpl	$5, %eax
	ja	.L721
	movl	%eax, %eax
	leaq	0(,%rax,4), %rdx
	leaq	.L723(%rip), %rax
	movl	(%rdx,%rax), %eax
	movslq	%eax, %rdx
	leaq	.L723(%rip), %rax
	addq	%rdx, %rax
	jmp	*%rax
	.section .rdata,"dr"
	.align 4
.L723:
	.long	.L722-.L723
	.long	.L724-.L723
	.long	.L725-.L723
	.long	.L726-.L723
	.long	.L721-.L723
	.long	.L727-.L723
	.text
.L722:
	leaq	.LC297(%rip), %rcx
	call	_Z7d_printPKcz
	movq	1216(%rbp), %rax
	movq	(%rax), %rdx
	movq	1224(%rbp), %rax
	movq	%rdx, (%rax)
	movq	1224(%rbp), %rax
	movq	(%rax), %rax
	movq	%rax, %rdx
	leaq	.LC298(%rip), %rcx
	call	_Z7d_printPKcz
	movq	1224(%rbp), %rax
	movq	(%rax), %rax
	movq	%rax, 1160(%rbp)
	jmp	.L721
.L727:
	leaq	.LC299(%rip), %rcx
	call	_Z7d_printPKcz
	movq	1216(%rbp), %rax
	movq	(%rax), %rax
	movq	%rax, 1184(%rbp)
	movq	1184(%rbp), %rdx
	movq	my_trace(%rip), %rax
	movq	136(%rax), %rax
	leaq	1160(%rbp), %r8
	leaq	1152(%rbp), %rcx
	movq	%rcx, 32(%rsp)
	movl	$4, %r9d
	movq	%rax, %rcx
	call	_Z11read_memoryPvS_S_yPy
	movq	1160(%rbp), %rdx
	movq	1184(%rbp), %rax
	movq	%rdx, %r8
	movq	%rax, %rdx
	leaq	.LC300(%rip), %rcx
	call	_Z7d_printPKcz
	movq	1160(%rbp), %rax
	testq	%rax, %rax
	jne	.L728
	jmp	.L721
.L728:
	movq	1160(%rbp), %rax
	movq	%rax, %rdx
	leaq	.LC298(%rip), %rcx
	call	_Z7d_printPKcz
	movq	1160(%rbp), %rdx
	movq	1224(%rbp), %rax
	movq	%rdx, (%rax)
	jmp	.L721
.L724:
	leaq	.LC301(%rip), %rcx
	call	_Z7d_printPKcz
	movq	1192(%rbp), %rax
	addq	$8, %rax
	movq	%rax, 1184(%rbp)
	movq	1216(%rbp), %rax
	movq	(%rax), %rax
	salq	$2, %rax
	addq	%rax, 1184(%rbp)
	movq	1184(%rbp), %rax
	movq	%rax, 1160(%rbp)
	movq	1160(%rbp), %rdx
	movq	1184(%rbp), %rax
	movq	%rdx, %r8
	movq	%rax, %rdx
	leaq	.LC300(%rip), %rcx
	call	_Z7d_printPKcz
	movq	1160(%rbp), %rax
	movq	%rax, %rdx
	leaq	.LC298(%rip), %rcx
	call	_Z7d_printPKcz
	movq	1160(%rbp), %rax
	testq	%rax, %rax
	jne	.L729
	jmp	.L721
.L729:
	movq	1160(%rbp), %rdx
	movq	1224(%rbp), %rax
	movq	%rdx, (%rax)
	jmp	.L721
.L725:
	leaq	.LC302(%rip), %rcx
	call	_Z7d_printPKcz
	movq	1192(%rbp), %rax
	addq	$8, %rax
	movq	%rax, 1184(%rbp)
	movq	1216(%rbp), %rax
	movq	(%rax), %rax
	salq	$2, %rax
	addq	%rax, 1184(%rbp)
	movq	1184(%rbp), %rdx
	movq	my_trace(%rip), %rax
	movq	136(%rax), %rax
	leaq	1160(%rbp), %r8
	leaq	1152(%rbp), %rcx
	movq	%rcx, 32(%rsp)
	movl	$4, %r9d
	movq	%rax, %rcx
	call	_Z11read_memoryPvS_S_yPy
	movq	1160(%rbp), %rdx
	movq	1184(%rbp), %rax
	movq	%rdx, %r8
	movq	%rax, %rdx
	leaq	.LC300(%rip), %rcx
	call	_Z7d_printPKcz
	movq	1160(%rbp), %rax
	testq	%rax, %rax
	jne	.L730
	jmp	.L721
.L730:
	movq	1160(%rbp), %rax
	movq	%rax, %rdx
	leaq	.LC298(%rip), %rcx
	call	_Z7d_printPKcz
	movq	1160(%rbp), %rdx
	movq	1224(%rbp), %rax
	movq	%rdx, (%rax)
	jmp	.L721
.L726:
	leaq	.LC303(%rip), %rcx
	call	_Z7d_printPKcz
	movq	1192(%rbp), %rax
	addq	$8, %rax
	movq	%rax, 1184(%rbp)
	movq	1216(%rbp), %rax
	movq	(%rax), %rax
	salq	$2, %rax
	addq	%rax, 1184(%rbp)
	movq	1184(%rbp), %rdx
	movq	my_trace(%rip), %rax
	movq	136(%rax), %rax
	leaq	1160(%rbp), %r8
	leaq	1152(%rbp), %rcx
	movq	%rcx, 32(%rsp)
	movl	$4, %r9d
	movq	%rax, %rcx
	call	_Z11read_memoryPvS_S_yPy
	movq	1160(%rbp), %rdx
	movq	1184(%rbp), %rax
	movq	%rdx, %r8
	movq	%rax, %rdx
	leaq	.LC300(%rip), %rcx
	call	_Z7d_printPKcz
	movq	1160(%rbp), %rax
	movq	%rax, 1184(%rbp)
	movq	1184(%rbp), %rdx
	movq	my_trace(%rip), %rax
	movq	136(%rax), %rax
	leaq	1160(%rbp), %r8
	leaq	1152(%rbp), %rcx
	movq	%rcx, 32(%rsp)
	movl	$4, %r9d
	movq	%rax, %rcx
	call	_Z11read_memoryPvS_S_yPy
	movq	1160(%rbp), %rdx
	movq	1184(%rbp), %rax
	movq	%rdx, %r8
	movq	%rax, %rdx
	leaq	.LC300(%rip), %rcx
	call	_Z7d_printPKcz
	movq	1160(%rbp), %rax
	testq	%rax, %rax
	jne	.L731
	jmp	.L721
.L731:
	movq	1160(%rbp), %rax
	movq	%rax, %rdx
	leaq	.LC298(%rip), %rcx
	call	_Z7d_printPKcz
	movq	1160(%rbp), %rdx
	movq	1224(%rbp), %rax
	movq	%rdx, (%rax)
	nop
.L721:
	movq	1160(%rbp), %rax
	testq	%rax, %rax
	jne	.L732
	movl	$-1, %eax
	jmp	.L740
.L732:
	movq	1160(%rbp), %rax
	movq	%rax, 1176(%rbp)
	movq	$0, 1160(%rbp)
	movq	$0, 1184(%rbp)
	leaq	.LC304(%rip), %rcx
	call	_Z7d_printPKcz
	movq	1216(%rbp), %rax
	movzbl	17(%rax), %eax
	movsbl	%al, %eax
	cmpl	$1, %eax
	je	.L735
	cmpl	$1, %eax
	jg	.L736
	testl	%eax, %eax
	je	.L737
	jmp	.L734
.L736:
	cmpl	$2, %eax
	je	.L738
	cmpl	$5, %eax
	je	.L739
	jmp	.L734
.L737:
	leaq	.LC297(%rip), %rcx
	call	_Z7d_printPKcz
	movq	1216(%rbp), %rax
	movq	8(%rax), %rdx
	movq	1224(%rbp), %rax
	movq	%rdx, 8(%rax)
	movq	1216(%rbp), %rax
	movq	8(%rax), %rax
	movq	%rax, 1160(%rbp)
	movq	1160(%rbp), %rax
	movq	%rax, %rdx
	leaq	.LC305(%rip), %rcx
	call	_Z7d_printPKcz
	jmp	.L734
.L739:
	leaq	.LC299(%rip), %rcx
	call	_Z7d_printPKcz
	movq	1216(%rbp), %rax
	movq	8(%rax), %rax
	movq	%rax, 1184(%rbp)
	movq	1184(%rbp), %rdx
	movq	my_trace(%rip), %rax
	movq	136(%rax), %rax
	leaq	1160(%rbp), %r8
	leaq	1152(%rbp), %rcx
	movq	%rcx, 32(%rsp)
	movl	$4, %r9d
	movq	%rax, %rcx
	call	_Z11read_memoryPvS_S_yPy
	movq	1160(%rbp), %rdx
	movq	1184(%rbp), %rax
	movq	%rdx, %r8
	movq	%rax, %rdx
	leaq	.LC300(%rip), %rcx
	call	_Z7d_printPKcz
	movq	1160(%rbp), %rax
	movq	%rax, %rdx
	leaq	.LC305(%rip), %rcx
	call	_Z7d_printPKcz
	movq	1160(%rbp), %rdx
	movq	1224(%rbp), %rax
	movq	%rdx, 8(%rax)
	jmp	.L734
.L735:
	leaq	.LC301(%rip), %rcx
	call	_Z7d_printPKcz
	movq	1192(%rbp), %rax
	addq	$8, %rax
	movq	%rax, 1184(%rbp)
	movq	1216(%rbp), %rax
	movq	8(%rax), %rax
	salq	$2, %rax
	addq	%rax, 1184(%rbp)
	movq	1184(%rbp), %rdx
	movq	my_trace(%rip), %rax
	movq	136(%rax), %rax
	leaq	1160(%rbp), %r8
	leaq	1152(%rbp), %rcx
	movq	%rcx, 32(%rsp)
	movl	$4, %r9d
	movq	%rax, %rcx
	call	_Z11read_memoryPvS_S_yPy
	movq	1160(%rbp), %rdx
	movq	1184(%rbp), %rax
	movq	%rdx, %r8
	movq	%rax, %rdx
	leaq	.LC300(%rip), %rcx
	call	_Z7d_printPKcz
	movq	1160(%rbp), %rax
	movq	%rax, %rdx
	leaq	.LC305(%rip), %rcx
	call	_Z7d_printPKcz
	movq	1160(%rbp), %rdx
	movq	1224(%rbp), %rax
	movq	%rdx, 8(%rax)
	jmp	.L734
.L738:
	leaq	.LC302(%rip), %rcx
	call	_Z7d_printPKcz
	movq	1192(%rbp), %rax
	addq	$8, %rax
	movq	%rax, 1184(%rbp)
	movq	1216(%rbp), %rax
	movq	8(%rax), %rax
	salq	$2, %rax
	addq	%rax, 1184(%rbp)
	movq	1184(%rbp), %rdx
	movq	my_trace(%rip), %rax
	movq	136(%rax), %rax
	leaq	1160(%rbp), %r8
	leaq	1152(%rbp), %rcx
	movq	%rcx, 32(%rsp)
	movl	$4, %r9d
	movq	%rax, %rcx
	call	_Z11read_memoryPvS_S_yPy
	movq	1160(%rbp), %rdx
	movq	1184(%rbp), %rax
	movq	%rdx, %r8
	movq	%rax, %rdx
	leaq	.LC300(%rip), %rcx
	call	_Z7d_printPKcz
	movq	1160(%rbp), %rax
	movq	%rax, 1184(%rbp)
	movq	1184(%rbp), %rdx
	movq	my_trace(%rip), %rax
	movq	136(%rax), %rax
	leaq	1160(%rbp), %r8
	leaq	1152(%rbp), %rcx
	movq	%rcx, 32(%rsp)
	movl	$4, %r9d
	movq	%rax, %rcx
	call	_Z11read_memoryPvS_S_yPy
	movq	1160(%rbp), %rdx
	movq	1184(%rbp), %rax
	movq	%rdx, %r8
	movq	%rax, %rdx
	leaq	.LC300(%rip), %rcx
	call	_Z7d_printPKcz
	movq	1160(%rbp), %rax
	movq	%rax, %rdx
	leaq	.LC305(%rip), %rcx
	call	_Z7d_printPKcz
	movq	1160(%rbp), %rdx
	movq	1224(%rbp), %rax
	movq	%rdx, 8(%rax)
	nop
.L734:
	movq	1160(%rbp), %rax
	movq	%rax, 1168(%rbp)
	movq	1168(%rbp), %rdx
	movq	1176(%rbp), %rax
	movq	%rdx, %r8
	movq	%rax, %rdx
	leaq	.LC306(%rip), %rcx
	call	_Z7d_printPKcz
	movl	$0, %eax
.L740:
	addq	$1328, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC307:
	.ascii "Registered region id 0x%08x\12\0"
	.text
	.globl	_Z14add_region_selymcc
	.def	_Z14add_region_selymcc;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z14add_region_selymcc
_Z14add_region_selymcc:
.LFB2556:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$48, %rsp
	.seh_stackalloc	48
	.seh_endprologue
	movq	%rcx, 16(%rbp)
	movl	%edx, 24(%rbp)
	movl	%r8d, %edx
	movl	%r9d, %eax
	movb	%dl, 32(%rbp)
	movb	%al, 40(%rbp)
	movq	my_trace(%rip), %rax
	movl	70046584(%rax), %eax
	movl	%eax, -4(%rbp)
	movq	my_trace(%rip), %rcx
	movl	-4(%rbp), %edx
	movq	%rdx, %rax
	addq	%rax, %rax
	addq	%rdx, %rax
	salq	$3, %rax
	addq	%rcx, %rax
	leaq	70045808(%rax), %rdx
	movq	16(%rbp), %rax
	movq	%rax, 8(%rdx)
	movq	my_trace(%rip), %r8
	movl	24(%rbp), %ecx
	movl	-4(%rbp), %edx
	movq	%rdx, %rax
	addq	%rax, %rax
	addq	%rdx, %rax
	salq	$3, %rax
	addq	%r8, %rax
	addq	$70045808, %rax
	movq	%rcx, 16(%rax)
	movq	my_trace(%rip), %rcx
	movl	-4(%rbp), %edx
	movq	%rdx, %rax
	addq	%rax, %rax
	addq	%rdx, %rax
	salq	$3, %rax
	addq	%rcx, %rax
	leaq	70045824(%rax), %rdx
	movzbl	32(%rbp), %eax
	movb	%al, 8(%rdx)
	movq	my_trace(%rip), %rcx
	movl	-4(%rbp), %edx
	movq	%rdx, %rax
	addq	%rax, %rax
	addq	%rdx, %rax
	salq	$3, %rax
	addq	%rcx, %rax
	leaq	70045824(%rax), %rdx
	movzbl	40(%rbp), %eax
	movb	%al, 9(%rdx)
	movl	-4(%rbp), %eax
	movl	%eax, %edx
	leaq	.LC307(%rip), %rcx
	call	_Z7d_printPKcz
	addl	$1, -4(%rbp)
	movq	my_trace(%rip), %rax
	movl	-4(%rbp), %edx
	movl	%edx, 70046584(%rax)
	movl	$0, %eax
	addq	$48, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC308:
	.ascii "xx\0"
	.align 8
.LC309:
	.ascii "Found reaction dor id: %s - %s\12\0"
	.text
	.globl	_Z13find_reactionPc
	.def	_Z13find_reactionPc;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z13find_reactionPc
_Z13find_reactionPc:
.LFB2557:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$48, %rsp
	.seh_stackalloc	48
	.seh_endprologue
	movq	%rcx, 16(%rbp)
	movq	16(%rbp), %rax
	addq	$2, %rax
	movb	$0, (%rax)
	movl	$0, -4(%rbp)
	jmp	.L744
.L747:
	movq	my_trace(%rip), %rax
	movq	70039656(%rax), %rcx
	movl	-4(%rbp), %edx
	movq	%rdx, %rax
	salq	$2, %rax
	addq	%rdx, %rax
	leaq	0(,%rax,4), %rdx
	addq	%rdx, %rax
	salq	$3, %rax
	addq	%rcx, %rax
	movq	16(%rbp), %rdx
	movq	%rax, %rcx
	call	strcmp
	testl	%eax, %eax
	jne	.L745
	movq	my_trace(%rip), %rax
	movq	70039656(%rax), %rcx
	movl	-4(%rbp), %edx
	movq	%rdx, %rax
	salq	$2, %rax
	addq	%rdx, %rax
	leaq	0(,%rax,4), %rdx
	addq	%rdx, %rax
	salq	$3, %rax
	addq	%rcx, %rax
	leaq	.LC308(%rip), %rdx
	movq	%rax, %rcx
	call	strcmp
	testl	%eax, %eax
	je	.L745
	movq	my_trace(%rip), %rax
	movq	70039656(%rax), %rcx
	movl	-4(%rbp), %edx
	movq	%rdx, %rax
	salq	$2, %rax
	addq	%rdx, %rax
	leaq	0(,%rax,4), %rdx
	addq	%rdx, %rax
	salq	$3, %rax
	addq	%rcx, %rax
	movq	16(%rbp), %r8
	movq	%rax, %rdx
	leaq	.LC309(%rip), %rcx
	call	_Z7d_printPKcz
	movq	my_trace(%rip), %rax
	movq	70039656(%rax), %rcx
	movl	-4(%rbp), %edx
	movq	%rdx, %rax
	salq	$2, %rax
	addq	%rdx, %rax
	leaq	0(,%rax,4), %rdx
	addq	%rdx, %rax
	salq	$3, %rax
	addq	%rcx, %rax
	jmp	.L746
.L745:
	addl	$1, -4(%rbp)
.L744:
	movq	my_trace(%rip), %rax
	movl	70039664(%rax), %eax
	cmpl	-4(%rbp), %eax
	ja	.L747
	movl	$0, %eax
.L746:
	addq	$48, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC310:
	.ascii "[add_couple]\12\0"
.LC311:
	.ascii "Added couple: %s\12\0"
.LC312:
	.ascii "[add_couple ends]\12\0"
	.text
	.globl	_Z10add_couplePcS_
	.def	_Z10add_couplePcS_;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z10add_couplePcS_
_Z10add_couplePcS_:
.LFB2558:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$48, %rsp
	.seh_stackalloc	48
	.seh_endprologue
	movq	%rcx, 16(%rbp)
	movq	%rdx, 24(%rbp)
	leaq	.LC310(%rip), %rcx
	call	_Z7d_printPKcz
	movq	24(%rbp), %rax
	movzbl	(%rax), %eax
	movb	%al, -16(%rbp)
	movq	24(%rbp), %rax
	movzbl	1(%rax), %eax
	movb	%al, -15(%rbp)
	movb	$0, -14(%rbp)
	movq	16(%rbp), %rcx
	call	_Z13find_reactionPc
	movq	%rax, -8(%rbp)
	movq	-8(%rbp), %rax
	movl	100(%rax), %eax
	movl	%eax, %edx
	movq	%rdx, %rax
	addq	%rax, %rax
	addq	%rdx, %rax
	movq	-8(%rbp), %rdx
	addq	%rdx, %rax
	leaq	3(%rax), %rcx
	movq	24(%rbp), %rax
	movq	%rax, %rdx
	call	strcpy
	movq	-8(%rbp), %rax
	movl	100(%rax), %eax
	leal	1(%rax), %edx
	movq	-8(%rbp), %rax
	movl	%edx, 100(%rax)
	leaq	-16(%rbp), %rax
	movq	%rax, %rdx
	leaq	.LC311(%rip), %rcx
	call	_Z7d_printPKcz
	leaq	.LC312(%rip), %rcx
	call	_Z7d_printPKcz
	movl	$0, %eax
	addq	$48, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC313:
	.ascii "[add_rid]\12\0"
.LC314:
	.ascii "Added rid: 0x%02x\12\0"
.LC315:
	.ascii "[add_rid ends]\12\0"
	.text
	.globl	_Z7add_ridPcj
	.def	_Z7add_ridPcj;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z7add_ridPcj
_Z7add_ridPcj:
.LFB2559:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$48, %rsp
	.seh_stackalloc	48
	.seh_endprologue
	movq	%rcx, 16(%rbp)
	movl	%edx, 24(%rbp)
	leaq	.LC313(%rip), %rcx
	call	_Z7d_printPKcz
	movq	16(%rbp), %rcx
	call	_Z13find_reactionPc
	movq	%rax, -8(%rbp)
	movq	-8(%rbp), %rax
	movl	168(%rax), %edx
	movq	-8(%rbp), %rax
	movl	%edx, %edx
	leaq	24(%rdx), %rcx
	movl	24(%rbp), %edx
	movl	%edx, 8(%rax,%rcx,4)
	movq	-8(%rbp), %rax
	movl	168(%rax), %eax
	leal	1(%rax), %edx
	movq	-8(%rbp), %rax
	movl	%edx, 168(%rax)
	movl	24(%rbp), %eax
	movl	%eax, %edx
	leaq	.LC314(%rip), %rcx
	call	_Z7d_printPKcz
	leaq	.LC315(%rip), %rcx
	call	_Z7d_printPKcz
	movl	$0, %eax
	addq	$48, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC316:
	.ascii "[add_reaction]\12\0"
	.align 8
.LC317:
	.ascii "Trying to add reaction at: %s with id: %s\12\0"
.LC318:
	.ascii "New reaction: %s:%s\12\0"
.LC319:
	.ascii "[add_reaction ends]\12\0"
	.text
	.globl	_Z12add_reactionPcS_
	.def	_Z12add_reactionPcS_;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z12add_reactionPcS_
_Z12add_reactionPcS_:
.LFB2560:
	pushq	%rbp
	.seh_pushreg	%rbp
	pushq	%rbx
	.seh_pushreg	%rbx
	subq	$56, %rsp
	.seh_stackalloc	56
	leaq	128(%rsp), %rbp
	.seh_setframe	%rbp, 128
	.seh_endprologue
	movq	%rcx, -48(%rbp)
	movq	%rdx, -40(%rbp)
	leaq	.LC316(%rip), %rcx
	call	_Z7d_printPKcz
	movq	my_trace(%rip), %rax
	movl	70039664(%rax), %eax
	movl	%eax, -84(%rbp)
	movq	-40(%rbp), %rax
	movq	%rax, %r8
	movq	-48(%rbp), %rdx
	leaq	.LC317(%rip), %rcx
	call	_Z7d_printPKcz
	movq	my_trace(%rip), %rax
	movq	70039656(%rax), %rcx
	movl	-84(%rbp), %edx
	movq	%rdx, %rax
	salq	$2, %rax
	addq	%rdx, %rax
	leaq	0(,%rax,4), %rdx
	addq	%rdx, %rax
	salq	$3, %rax
	addq	%rcx, %rax
	movl	$200, %r8d
	movl	$0, %edx
	movq	%rax, %rcx
	call	memset
	movq	my_trace(%rip), %rax
	movq	70039656(%rax), %rdx
	movq	my_trace(%rip), %rax
	movl	70039664(%rax), %eax
	movl	%eax, %ecx
	movq	%rcx, %rax
	salq	$2, %rax
	addq	%rcx, %rax
	leaq	0(,%rax,4), %rcx
	addq	%rcx, %rax
	salq	$3, %rax
	addq	%rdx, %rax
	movq	%rax, %rcx
	movq	-40(%rbp), %rax
	movq	%rax, %rdx
	call	strcpy
	movq	my_trace(%rip), %rax
	movq	70039656(%rax), %rcx
	movl	-84(%rbp), %edx
	movq	%rdx, %rax
	salq	$2, %rax
	addq	%rdx, %rax
	leaq	0(,%rax,4), %rdx
	addq	%rdx, %rax
	salq	$3, %rax
	addq	%rcx, %rax
	movb	$0, 2(%rax)
	movq	my_trace(%rip), %rax
	movq	70039656(%rax), %rcx
	movl	-84(%rbp), %edx
	movq	%rdx, %rax
	salq	$2, %rax
	addq	%rdx, %rax
	leaq	0(,%rax,4), %rdx
	addq	%rdx, %rax
	salq	$3, %rax
	leaq	(%rcx,%rax), %rbx
	movq	my_trace(%rip), %rax
	movq	70039656(%rax), %rcx
	movl	-84(%rbp), %edx
	movq	%rdx, %rax
	salq	$2, %rax
	addq	%rdx, %rax
	leaq	0(,%rax,4), %rdx
	addq	%rdx, %rax
	salq	$3, %rax
	addq	%rcx, %rax
	movq	%rax, %rdx
	movq	-48(%rbp), %rcx
	call	_Z14add_breakpointPcP9REACTION_
	movq	%rax, 184(%rbx)
	movq	my_trace(%rip), %rax
	movq	70039656(%rax), %rcx
	movl	-84(%rbp), %edx
	movq	%rdx, %rax
	salq	$2, %rax
	addq	%rdx, %rax
	leaq	0(,%rax,4), %rdx
	addq	%rdx, %rax
	salq	$3, %rax
	addq	%rcx, %rax
	movq	184(%rax), %rax
	movb	$1, 529(%rax)
	movq	my_trace(%rip), %rax
	movq	70039656(%rax), %rcx
	movl	-84(%rbp), %edx
	movq	%rdx, %rax
	salq	$2, %rax
	addq	%rdx, %rax
	leaq	0(,%rax,4), %rdx
	addq	%rdx, %rax
	salq	$3, %rax
	addq	%rcx, %rax
	movb	$1, 172(%rax)
	movq	my_trace(%rip), %rax
	movq	70039656(%rax), %rcx
	movl	-84(%rbp), %edx
	movq	%rdx, %rax
	salq	$2, %rax
	addq	%rdx, %rax
	leaq	0(,%rax,4), %rdx
	addq	%rdx, %rax
	salq	$3, %rax
	addq	%rcx, %rax
	movb	$0, 173(%rax)
	movq	my_trace(%rip), %rax
	movq	70039656(%rax), %rcx
	movl	-84(%rbp), %edx
	movq	%rdx, %rax
	salq	$2, %rax
	addq	%rdx, %rax
	leaq	0(,%rax,4), %rdx
	addq	%rdx, %rax
	salq	$3, %rax
	addq	%rcx, %rax
	movq	184(%rax), %rax
	movq	%rax, %rcx
	call	_Z17update_breakpointP11BREAKPOINT_
	movq	my_trace(%rip), %rax
	movq	70039656(%rax), %rcx
	movl	-84(%rbp), %edx
	movq	%rdx, %rax
	salq	$2, %rax
	addq	%rdx, %rax
	leaq	0(,%rax,4), %rdx
	addq	%rdx, %rax
	salq	$3, %rax
	addq	%rcx, %rax
	movq	%rax, %r8
	movq	-48(%rbp), %rdx
	leaq	.LC318(%rip), %rcx
	call	_Z7d_printPKcz
	movq	my_trace(%rip), %rax
	movl	70039664(%rax), %edx
	addl	$1, %edx
	movl	%edx, 70039664(%rax)
	leaq	.LC319(%rip), %rcx
	call	_Z7d_printPKcz
	movl	$0, %eax
	addq	$56, %rsp
	popq	%rbx
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC320:
	.ascii "[parse_region]\12\0"
.LC321:
	.ascii "loc_str: %s\12\0"
.LC322:
	.ascii "[parse_region finishes]\12\0"
	.text
	.globl	_Z12parse_regionPc
	.def	_Z12parse_regionPc;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z12parse_regionPc
_Z12parse_regionPc:
.LFB2561:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$64, %rsp
	.seh_stackalloc	64
	.seh_endprologue
	movq	%rcx, 16(%rbp)
	leaq	.LC320(%rip), %rcx
	call	_Z7d_printPKcz
	leaq	.LC67(%rip), %rdx
	movq	16(%rbp), %rcx
	call	strtok
	movq	%rax, -8(%rbp)
	movq	-8(%rbp), %rax
	movq	%rax, %rdx
	leaq	.LC321(%rip), %rcx
	call	_Z7d_printPKcz
	leaq	.LC67(%rip), %rdx
	movl	$0, %ecx
	call	strtok
	movq	%rax, -16(%rbp)
	movq	-8(%rbp), %rax
	movq	%rax, %rdx
	leaq	.LC321(%rip), %rcx
	call	_Z7d_printPKcz
	movq	my_trace(%rip), %rdx
	movq	my_trace(%rip), %rax
	movl	70047104(%rax), %eax
	movl	%eax, %eax
	addq	$4377912, %rax
	salq	$4, %rax
	addq	%rdx, %rax
	movq	%rax, -24(%rbp)
	movq	-8(%rbp), %rax
	movq	%rax, %rcx
	call	_Z19parse_location_descPc
	movq	%rax, %rdx
	movq	-24(%rbp), %rax
	movq	%rdx, (%rax)
	movq	-16(%rbp), %rax
	movq	%rax, %rcx
	call	_Z19parse_location_descPc
	movq	%rax, %rdx
	movq	-24(%rbp), %rax
	movq	%rdx, 8(%rax)
	movq	my_trace(%rip), %rax
	movl	70047104(%rax), %edx
	addl	$1, %edx
	movl	%edx, 70047104(%rax)
	leaq	.LC322(%rip), %rcx
	call	_Z7d_printPKcz
	movl	$0, %eax
	addq	$64, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC323:
	.ascii "[parse_reaction]\12\0"
.LC324:
	.ascii "Parsing until ;: %s\12\0"
.LC325:
	.ascii ",\0"
.LC326:
	.ascii "id_str: %s\12\0"
.LC327:
	.ascii ";\0"
.LC328:
	.ascii "rids_str: %s\12\0"
.LC329:
	.ascii "Processing rids\12\0"
.LC330:
	.ascii "id_str: %s, rid: %s\12\0"
.LC331:
	.ascii "Processing couples\12\0"
.LC332:
	.ascii "id_str: %s, coupled_id: %s\12\0"
.LC333:
	.ascii "[parse_reaction ends]\12\0"
	.text
	.globl	_Z14parse_reactionPc
	.def	_Z14parse_reactionPc;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z14parse_reactionPc
_Z14parse_reactionPc:
.LFB2562:
	pushq	%rbp
	.seh_pushreg	%rbp
	subq	$3152, %rsp
	.seh_stackalloc	3152
	leaq	128(%rsp), %rbp
	.seh_setframe	%rbp, 128
	.seh_endprologue
	movq	%rcx, 3040(%rbp)
	leaq	.LC323(%rip), %rcx
	call	_Z7d_printPKcz
	movq	3040(%rbp), %rdx
	leaq	.LC324(%rip), %rcx
	call	_Z7d_printPKcz
	leaq	.LC325(%rip), %rdx
	movq	3040(%rbp), %rcx
	call	strtok
	movq	%rax, 3016(%rbp)
	movq	3016(%rbp), %rdx
	leaq	-96(%rbp), %rax
	movq	%rax, %rcx
	call	strcpy
	leaq	-96(%rbp), %rax
	movq	%rax, %rdx
	leaq	.LC321(%rip), %rcx
	call	_Z7d_printPKcz
	leaq	.LC325(%rip), %rdx
	movl	$0, %ecx
	call	strtok
	movq	%rax, 3008(%rbp)
	movq	3008(%rbp), %rdx
	leaq	672(%rbp), %rax
	movq	%rax, %rcx
	call	strcpy
	leaq	672(%rbp), %rax
	movq	%rax, %rdx
	leaq	.LC326(%rip), %rcx
	call	_Z7d_printPKcz
	leaq	.LC327(%rip), %rdx
	movl	$0, %ecx
	call	strtok
	movq	%rax, 3000(%rbp)
	movq	3000(%rbp), %rdx
	leaq	2208(%rbp), %rax
	movq	%rax, %rcx
	call	strcpy
	leaq	2208(%rbp), %rax
	movq	%rax, %rdx
	leaq	.LC328(%rip), %rcx
	call	_Z7d_printPKcz
	leaq	672(%rbp), %rdx
	leaq	1440(%rbp), %rax
	movq	%rax, %rcx
	call	strcpy
	leaq	672(%rbp), %rdx
	movq	3016(%rbp), %rax
	movq	%rax, %rcx
	call	_Z12add_reactionPcS_
	leaq	.LC329(%rip), %rcx
	call	_Z7d_printPKcz
	leaq	2208(%rbp), %rax
	leaq	.LC67(%rip), %rdx
	movq	%rax, %rcx
	call	strtok
	movq	%rax, 2992(%rbp)
	movq	2992(%rbp), %rdx
	leaq	672(%rbp), %rax
	movq	%rdx, %r8
	movq	%rax, %rdx
	leaq	.LC330(%rip), %rcx
	call	_Z7d_printPKcz
	movq	2992(%rbp), %rax
	movl	$16, %r8d
	movl	$0, %edx
	movq	%rax, %rcx
	call	strtol
	movl	%eax, %edx
	leaq	672(%rbp), %rax
	movq	%rax, %rcx
	call	_Z7add_ridPcj
	jmp	.L757
.L758:
	movq	2992(%rbp), %rdx
	leaq	672(%rbp), %rax
	movq	%rdx, %r8
	movq	%rax, %rdx
	leaq	.LC330(%rip), %rcx
	call	_Z7d_printPKcz
	movq	2992(%rbp), %rax
	movl	$16, %r8d
	movl	$0, %edx
	movq	%rax, %rcx
	call	strtol
	movl	%eax, %edx
	leaq	672(%rbp), %rax
	movq	%rax, %rcx
	call	_Z7add_ridPcj
.L757:
	leaq	.LC67(%rip), %rdx
	movl	$0, %ecx
	call	strtok
	movq	%rax, 2992(%rbp)
	cmpq	$0, 2992(%rbp)
	setne	%al
	testb	%al, %al
	jne	.L758
	leaq	.LC331(%rip), %rcx
	call	_Z7d_printPKcz
	leaq	1440(%rbp), %rax
	leaq	.LC67(%rip), %rdx
	movq	%rax, %rcx
	call	strtok
	movq	%rax, 2984(%rbp)
	jmp	.L759
.L760:
	movq	2984(%rbp), %rdx
	leaq	672(%rbp), %rax
	movq	%rdx, %r8
	movq	%rax, %rdx
	leaq	.LC332(%rip), %rcx
	call	_Z7d_printPKcz
	movq	2984(%rbp), %rdx
	leaq	672(%rbp), %rax
	movq	%rax, %rcx
	call	_Z10add_couplePcS_
.L759:
	leaq	.LC67(%rip), %rdx
	movl	$0, %ecx
	call	strtok
	movq	%rax, 2984(%rbp)
	cmpq	$0, 2984(%rbp)
	setne	%al
	testb	%al, %al
	jne	.L760
	leaq	.LC333(%rip), %rcx
	call	_Z7d_printPKcz
	addq	$3152, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.globl	_Z15parse_reactionsPc
	.def	_Z15parse_reactionsPc;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z15parse_reactionsPc
_Z15parse_reactionsPc:
.LFB2563:
	pushq	%rbp
	.seh_pushreg	%rbp
	subq	$816, %rsp
	.seh_stackalloc	816
	leaq	128(%rsp), %rbp
	.seh_setframe	%rbp, 128
	.seh_endprologue
	movq	%rcx, 704(%rbp)
	movq	704(%rbp), %rax
	movq	%rax, 680(%rbp)
	jmp	.L762
.L764:
	movq	680(%rbp), %rdx
	leaq	-96(%rbp), %rax
	movq	%rax, %rcx
	call	strcpy
	leaq	-96(%rbp), %rax
	movq	%rax, %rcx
	call	_Z14parse_reactionPc
	movq	680(%rbp), %rax
	movl	$59, %edx
	movq	%rax, %rcx
	call	strchr
	movq	%rax, 680(%rbp)
	cmpq	$0, 680(%rbp)
	je	.L762
	addq	$1, 680(%rbp)
.L762:
	cmpq	$0, 680(%rbp)
	je	.L763
	movq	680(%rbp), %rax
	movzbl	(%rax), %eax
	testb	%al, %al
	jne	.L764
.L763:
	movl	$0, %eax
	addq	$816, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.globl	_Z13parse_regionsPc
	.def	_Z13parse_regionsPc;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z13parse_regionsPc
_Z13parse_regionsPc:
.LFB2564:
	pushq	%rbp
	.seh_pushreg	%rbp
	subq	$816, %rsp
	.seh_stackalloc	816
	leaq	128(%rsp), %rbp
	.seh_setframe	%rbp, 128
	.seh_endprologue
	movq	%rcx, 704(%rbp)
	movq	704(%rbp), %rax
	movq	%rax, 680(%rbp)
	jmp	.L767
.L769:
	movq	680(%rbp), %rdx
	leaq	-96(%rbp), %rax
	movq	%rax, %rcx
	call	strcpy
	leaq	-96(%rbp), %rax
	movq	%rax, %rcx
	call	_Z12parse_regionPc
	movq	680(%rbp), %rax
	movl	$59, %edx
	movq	%rax, %rcx
	call	strchr
	movq	%rax, 680(%rbp)
	cmpq	$0, 680(%rbp)
	je	.L767
	addq	$1, 680(%rbp)
.L767:
	cmpq	$0, 680(%rbp)
	je	.L768
	movq	680(%rbp), %rax
	movzbl	(%rax), %eax
	testb	%al, %al
	jne	.L769
.L768:
	movl	$0, %eax
	addq	$816, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC334:
	.ascii "%s\\%s_%d.out\0"
.LC335:
	.ascii "LF,%s_%d.out\12\0"
.LC336:
	.ascii "w+\0"
.LC337:
	.ascii "Setting out file to: %s\12\0"
	.text
	.globl	_Z15reload_out_filev
	.def	_Z15reload_out_filev;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z15reload_out_filev
_Z15reload_out_filev:
.LFB2565:
	pushq	%rbp
	.seh_pushreg	%rbp
	pushq	%rbx
	.seh_pushreg	%rbx
	subq	$1592, %rsp
	.seh_stackalloc	1592
	leaq	128(%rsp), %rbp
	.seh_setframe	%rbp, 128
	.seh_endprologue
	movq	my_trace(%rip), %rax
	movl	70035240(%rax), %edx
	addl	$1, %edx
	movl	%edx, 70035240(%rax)
	movl	$0, %eax
	movb	%al, -80(%rbp)
	movq	my_trace(%rip), %rax
	movl	70035240(%rax), %edx
	movq	my_trace(%rip), %rax
	leaq	70034472(%rax), %r8
	movq	my_trace(%rip), %rax
	leaq	70033704(%rax), %rcx
	leaq	-80(%rbp), %rax
	movl	%edx, 32(%rsp)
	movq	%r8, %r9
	movq	%rcx, %r8
	leaq	.LC334(%rip), %rdx
	movq	%rax, %rcx
	call	sprintf
	movq	my_trace(%rip), %rax
	leaq	70035244(%rax), %rcx
	leaq	-80(%rbp), %rax
	movq	%rax, %rdx
	call	strcpy
	movq	my_trace(%rip), %rax
	movl	70035240(%rax), %edx
	movq	my_trace(%rip), %rax
	leaq	70034472(%rax), %rcx
	leaq	688(%rbp), %rax
	movl	%edx, %r9d
	movq	%rcx, %r8
	leaq	.LC335(%rip), %rdx
	movq	%rax, %rcx
	call	sprintf
	leaq	688(%rbp), %rax
	movq	%rax, %rcx
	call	_Z13add_to_bufferPc
	movq	my_trace(%rip), %rax
	movq	70030584(%rax), %rax
	movq	%rax, %rcx
	call	fclose
	movq	my_trace(%rip), %rbx
	movq	my_trace(%rip), %rax
	addq	$70035244, %rax
	leaq	.LC336(%rip), %rdx
	movq	%rax, %rcx
	call	fopen
	movq	%rax, 70030584(%rbx)
	movq	my_trace(%rip), %rax
	addq	$70035244, %rax
	movq	%rax, %rdx
	leaq	.LC337(%rip), %rcx
	call	_Z7d_printPKcz
	movl	$0, %eax
	addq	$1592, %rsp
	popq	%rbx
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC338:
	.ascii "SN\0"
.LC339:
	.ascii "Sample path set to: %s\12\0"
.LC340:
	.ascii "SI\0"
.LC341:
	.ascii "Sample PID set to: 0x%08x\12\0"
.LC342:
	.ascii "SD\0"
.LC343:
	.ascii "Research dir set to: %s\12\0"
.LC344:
	.ascii "Sd\0"
.LC345:
	.ascii "Out dir set to: %s\12\0"
.LC346:
	.ascii "Sp\0"
.LC347:
	.ascii "Out prefix set to: %s\12\0"
.LC348:
	.ascii "Rp\0"
.LC349:
	.ascii "SL\0"
.LC350:
	.ascii "Trace limit set to: %d\12\0"
.LC351:
	.ascii "ST\0"
.LC352:
	.ascii "Trace name set to: %s\12\0"
.LC353:
	.ascii "sD\0"
.LC354:
	.ascii "Dump name set to: %s\12\0"
.LC355:
	.ascii "Info name set to: %s\12\0"
.LC356:
	.ascii "PT\0"
.LC357:
	.ascii "Error, out dir not set\0"
.LC358:
	.ascii "%s\\%s.log\0"
.LC359:
	.ascii "w\0"
.LC360:
	.ascii "Log file: %s\12\0"
.LC361:
	.ascii "%s\\%s.dump\0"
.LC362:
	.ascii "Dump file: %s\12\0"
.LC363:
	.ascii "%s\\%s.mod\0"
.LC364:
	.ascii "Mods file: %s\12\0"
.LC365:
	.ascii "OM,%s.mod\12\0"
.LC366:
	.ascii "%s\\%s.ini\0"
.LC367:
	.ascii "Ini file: %s\12\0"
.LC368:
	.ascii "ET\0"
.LC369:
	.ascii "Tracing enabled\12\0"
.LC370:
	.ascii "Starting @ 0x%08x\12\0"
.LC371:
	.ascii "ST,0x%08x\12\0"
.LC372:
	.ascii "EL\0"
	.align 8
.LC373:
	.ascii "Light tracing debugged enabled\12\0"
.LC374:
	.ascii "ED\0"
.LC375:
	.ascii "Tracing debugged enabled\12\0"
.LC376:
	.ascii "DT\0"
.LC377:
	.ascii "Tracing disabled\12\0"
.LC378:
	.ascii "Ending @ 0x%08x\12\0"
.LC379:
	.ascii "END,0x%08x\12\0"
.LC380:
	.ascii "DM\0"
.LC381:
	.ascii "ER\0"
.LC382:
	.ascii " \0"
	.align 8
.LC383:
	.ascii "Setting exclusion for reaction no %s\12\0"
.LC384:
	.ascii "NR\0"
.LC385:
	.ascii "eR\0"
.LC386:
	.ascii "Enabling reaction no %s\12\0"
.LC387:
	.ascii "dR\0"
.LC388:
	.ascii "Disabling single reaction %s\12\0"
.LC389:
	.ascii "eA\0"
.LC390:
	.ascii "Enabling all reactions\12\0"
.LC391:
	.ascii "dA\0"
.LC392:
	.ascii "Disabling all reactions\12\0"
.LC393:
	.ascii "rR\0"
.LC394:
	.ascii "Raising reaction no %s\12\0"
.LC395:
	.ascii "lR\0"
.LC396:
	.ascii "Lowering reaction no %s\12\0"
.LC397:
	.ascii "Rx\0"
.LC398:
	.ascii "ad\0"
.LC399:
	.ascii "Starting debugging: 0x%08x\12\0"
.LC400:
	.ascii "sd\0"
.LC401:
	.ascii "Starting debugging: %s\12\0"
.LC402:
	.ascii "ra\0"
.LC403:
	.ascii "rs\0"
.LC404:
	.ascii "ru\0"
.LC405:
	.ascii "lt\0"
.LC406:
	.ascii "la\0"
.LC407:
	.ascii "lm\0"
.LC408:
	.ascii "lb\0"
.LC409:
	.ascii "ll\0"
.LC410:
	.ascii "or\0"
	.align 8
.LC411:
	.ascii "Ouputting region: 0x%08x, 0x%08x @ %d\12\0"
.LC412:
	.ascii "Rr\0"
	.align 8
.LC413:
	.ascii "Reading region: 0x%08x, 0x%08x\12\0"
.LC414:
	.ascii "RM\0"
.LC415:
	.ascii "Reading addr: 0x%08x\12\0"
.LC416:
	.ascii "WS\0"
.LC417:
	.ascii "Writing str: %s @ 0x%08x\12\0"
.LC418:
	.ascii "WU\0"
.LC419:
	.ascii "WM\0"
	.align 8
.LC420:
	.ascii "Writing addr: 0x%08x with 0x%08x\12\0"
.LC421:
	.ascii "RE\0"
.LC422:
	.ascii "RR\0"
.LC423:
	.ascii "WR\0"
.LC424:
	.ascii "RS\0"
.LC425:
	.ascii "cn\0"
	.align 8
.LC426:
	.ascii "Continuing with status: 0x%08x\12\0"
.LC427:
	.ascii "cN\0"
.LC428:
	.ascii "SS\0"
.LC429:
	.ascii "AS\0"
.LC430:
	.ascii "cR\0"
.LC431:
	.ascii "Configuring reactions\12\0"
.LC432:
	.ascii "cr\0"
.LC433:
	.ascii "R1\0"
	.text
	.globl	_Z10handle_cmdPc
	.def	_Z10handle_cmdPc;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z10handle_cmdPc
_Z10handle_cmdPc:
.LFB2566:
	pushq	%rbp
	.seh_pushreg	%rbp
	pushq	%rdi
	.seh_pushreg	%rdi
	pushq	%rbx
	.seh_pushreg	%rbx
	subq	$1888, %rsp
	.seh_stackalloc	1888
	leaq	128(%rsp), %rbp
	.seh_setframe	%rbp, 128
	.seh_endprologue
	movq	%rcx, 1792(%rbp)
	movq	1792(%rbp), %rdx
	leaq	.LC196(%rip), %rcx
	call	_Z7d_printPKcz
	movq	my_trace(%rip), %rax
	movl	$153, 67932936(%rax)
	movq	my_trace(%rip), %rax
	addq	$67932940, %rax
	movl	$1048576, %r8d
	movl	$0, %edx
	movq	%rax, %rcx
	call	memset
	movq	my_trace(%rip), %rax
	addq	$67932940, %rax
	movb	$0, (%rax)
	movl	$2, %r8d
	leaq	.LC338(%rip), %rdx
	movq	1792(%rbp), %rcx
	call	strncmp
	testl	%eax, %eax
	jne	.L774
	movq	1792(%rbp), %rax
	leaq	3(%rax), %rdx
	movq	my_trace(%rip), %rax
	addq	$70032932, %rax
	movq	%rax, %rcx
	call	strcpy
	movq	my_trace(%rip), %rax
	addq	$70032932, %rax
	movq	%rax, %rdx
	leaq	.LC339(%rip), %rcx
	call	_Z7d_printPKcz
	call	_Z11send_reportv
	jmp	.L775
.L774:
	movl	$2, %r8d
	leaq	.LC340(%rip), %rdx
	movq	1792(%rbp), %rcx
	call	strncmp
	testl	%eax, %eax
	jne	.L776
	movq	my_trace(%rip), %rbx
	movq	1792(%rbp), %rax
	addq	$3, %rax
	movl	$16, %r8d
	movl	$0, %edx
	movq	%rax, %rcx
	call	strtol
	movl	%eax, 70033700(%rbx)
	movq	my_trace(%rip), %rax
	movl	70033700(%rax), %eax
	movl	%eax, %edx
	leaq	.LC341(%rip), %rcx
	call	_Z7d_printPKcz
	call	_Z11send_reportv
	jmp	.L775
.L776:
	movl	$2, %r8d
	leaq	.LC342(%rip), %rdx
	movq	1792(%rbp), %rcx
	call	strncmp
	testl	%eax, %eax
	jne	.L777
	movq	1792(%rbp), %rax
	leaq	3(%rax), %rdx
	movq	my_trace(%rip), %rax
	addq	$70032164, %rax
	movq	%rax, %rcx
	call	strcpy
	movq	my_trace(%rip), %rax
	addq	$70032164, %rax
	movq	%rax, %rdx
	leaq	.LC343(%rip), %rcx
	call	_Z7d_printPKcz
	call	_Z11send_reportv
	jmp	.L775
.L777:
	movl	$2, %r8d
	leaq	.LC344(%rip), %rdx
	movq	1792(%rbp), %rcx
	call	strncmp
	testl	%eax, %eax
	jne	.L778
	movq	1792(%rbp), %rax
	leaq	3(%rax), %rdx
	movq	my_trace(%rip), %rax
	addq	$70033704, %rax
	movq	%rax, %rcx
	call	strcpy
	movq	my_trace(%rip), %rax
	addq	$70033704, %rax
	movq	%rax, %rdx
	leaq	.LC345(%rip), %rcx
	call	_Z7d_printPKcz
	call	_Z11send_reportv
	jmp	.L775
.L778:
	movl	$2, %r8d
	leaq	.LC346(%rip), %rdx
	movq	1792(%rbp), %rcx
	call	strncmp
	testl	%eax, %eax
	jne	.L779
	movq	1792(%rbp), %rax
	leaq	3(%rax), %rdx
	movq	my_trace(%rip), %rax
	addq	$70034472, %rax
	movq	%rax, %rcx
	call	strcpy
	movq	my_trace(%rip), %rax
	addq	$70034472, %rax
	movq	%rax, %rdx
	leaq	.LC347(%rip), %rcx
	call	_Z7d_printPKcz
	call	_Z11send_reportv
	jmp	.L775
.L779:
	movl	$2, %r8d
	leaq	.LC348(%rip), %rdx
	movq	1792(%rbp), %rcx
	call	strncmp
	testl	%eax, %eax
	jne	.L780
	movq	my_trace(%rip), %rax
	movl	$7, 67932936(%rax)
	movq	my_trace(%rip), %rax
	leaq	70034472(%rax), %rdx
	movq	my_trace(%rip), %rax
	addq	$67932940, %rax
	movq	%rax, %rcx
	call	strcpy
	call	_Z11send_reportv
	jmp	.L775
.L780:
	movl	$2, %r8d
	leaq	.LC349(%rip), %rdx
	movq	1792(%rbp), %rcx
	call	strncmp
	testl	%eax, %eax
	jne	.L781
	movq	my_trace(%rip), %rbx
	movq	1792(%rbp), %rax
	addq	$3, %rax
	movl	$10, %r8d
	movl	$0, %edx
	movq	%rax, %rcx
	call	strtoul
	movl	%eax, %eax
	movq	%rax, 68981544(%rbx)
	movq	my_trace(%rip), %rax
	movq	68981544(%rax), %rax
	movq	%rax, %rdx
	leaq	.LC350(%rip), %rcx
	call	_Z7d_printPKcz
	call	_Z11send_reportv
	jmp	.L775
.L781:
	movl	$2, %r8d
	leaq	.LC351(%rip), %rdx
	movq	1792(%rbp), %rcx
	call	strncmp
	testl	%eax, %eax
	jne	.L782
	movq	my_trace(%rip), %rax
	addq	$70035244, %rax
	movq	%rax, %rdx
	leaq	.LC352(%rip), %rcx
	call	_Z7d_printPKcz
	call	_Z11send_reportv
	jmp	.L775
.L782:
	movl	$2, %r8d
	leaq	.LC353(%rip), %rdx
	movq	1792(%rbp), %rcx
	call	strncmp
	testl	%eax, %eax
	jne	.L783
	movq	my_trace(%rip), %rax
	addq	$70036012, %rax
	movq	%rax, %rdx
	leaq	.LC354(%rip), %rcx
	call	_Z7d_printPKcz
	call	_Z11send_reportv
	jmp	.L775
.L783:
	movl	$2, %r8d
	leaq	.LC340(%rip), %rdx
	movq	1792(%rbp), %rcx
	call	strncmp
	testl	%eax, %eax
	jne	.L784
	movq	my_trace(%rip), %rax
	addq	$70036780, %rax
	movq	%rax, %rdx
	leaq	.LC355(%rip), %rcx
	call	_Z7d_printPKcz
	call	_Z11send_reportv
	jmp	.L775
.L784:
	movl	$2, %r8d
	leaq	.LC356(%rip), %rdx
	movq	1792(%rbp), %rcx
	call	strncmp
	testl	%eax, %eax
	jne	.L785
	movq	my_trace(%rip), %rax
	addq	$70033704, %rax
	testq	%rax, %rax
	je	.L786
	movq	my_trace(%rip), %rax
	addq	$70034472, %rax
	testq	%rax, %rax
	jne	.L787
.L786:
	leaq	.LC357(%rip), %rcx
	call	_Z7d_printPKcz
	movl	$-1, %ecx
	call	exit
.L787:
	movq	my_trace(%rip), %rax
	movl	70033700(%rax), %eax
	testl	%eax, %eax
	je	.L788
	movq	my_trace(%rip), %rax
	movl	70033700(%rax), %edx
	movq	my_trace(%rip), %rax
	addq	$70034472, %rax
	movl	%edx, %r8d
	leaq	.LC269(%rip), %rdx
	movq	%rax, %rcx
	call	sprintf
.L788:
	movq	my_trace(%rip), %rax
	movl	$-1, 70035240(%rax)
	call	_Z15reload_out_filev
	movl	$0, %eax
	movb	%al, -96(%rbp)
	movq	my_trace(%rip), %rax
	leaq	70034472(%rax), %rcx
	movq	my_trace(%rip), %rax
	leaq	70033704(%rax), %rdx
	leaq	-96(%rbp), %rax
	movq	%rcx, %r9
	movq	%rdx, %r8
	leaq	.LC358(%rip), %rdx
	movq	%rax, %rcx
	call	sprintf
	movq	my_trace(%rip), %rax
	leaq	70038316(%rax), %rcx
	leaq	-96(%rbp), %rax
	movq	%rax, %rdx
	call	strcpy
	movq	my_trace(%rip), %rbx
	movq	my_trace(%rip), %rax
	addq	$70038316, %rax
	leaq	.LC359(%rip), %rdx
	movq	%rax, %rcx
	call	fopen
	movq	%rax, 70030568(%rbx)
	movq	my_trace(%rip), %rax
	movq	70030568(%rax), %rax
	movl	$0, %r9d
	movl	$4, %r8d
	movl	$0, %edx
	movq	%rax, %rcx
	call	setvbuf
	movq	my_trace(%rip), %rax
	addq	$70038316, %rax
	movq	%rax, %rdx
	leaq	.LC360(%rip), %rcx
	call	_Z7d_printPKcz
	movl	$0, %eax
	movb	%al, -96(%rbp)
	movq	my_trace(%rip), %rax
	leaq	70034472(%rax), %rcx
	movq	my_trace(%rip), %rax
	leaq	70033704(%rax), %rdx
	leaq	-96(%rbp), %rax
	movq	%rcx, %r9
	movq	%rdx, %r8
	leaq	.LC361(%rip), %rdx
	movq	%rax, %rcx
	call	sprintf
	movq	my_trace(%rip), %rax
	leaq	70036012(%rax), %rcx
	leaq	-96(%rbp), %rax
	movq	%rax, %rdx
	call	strcpy
	movq	my_trace(%rip), %rax
	addq	$70036012, %rax
	movq	%rax, %rdx
	leaq	.LC362(%rip), %rcx
	call	_Z7d_printPKcz
	movl	$0, %eax
	movb	%al, -96(%rbp)
	movq	my_trace(%rip), %rax
	leaq	70034472(%rax), %rcx
	movq	my_trace(%rip), %rax
	leaq	70033704(%rax), %rdx
	leaq	-96(%rbp), %rax
	movq	%rcx, %r9
	movq	%rdx, %r8
	leaq	.LC363(%rip), %rdx
	movq	%rax, %rcx
	call	sprintf
	movq	my_trace(%rip), %rax
	leaq	70037548(%rax), %rcx
	leaq	-96(%rbp), %rax
	movq	%rax, %rdx
	call	strcpy
	movq	my_trace(%rip), %rbx
	movq	my_trace(%rip), %rax
	addq	$70037548, %rax
	leaq	.LC154(%rip), %rdx
	movq	%rax, %rcx
	call	fopen
	movq	%rax, 70030592(%rbx)
	movq	my_trace(%rip), %rax
	addq	$70037548, %rax
	movq	%rax, %rdx
	leaq	.LC364(%rip), %rcx
	call	_Z7d_printPKcz
	movq	my_trace(%rip), %rax
	leaq	70034472(%rax), %rdx
	leaq	672(%rbp), %rax
	movq	%rdx, %r8
	leaq	.LC365(%rip), %rdx
	movq	%rax, %rcx
	call	sprintf
	leaq	672(%rbp), %rax
	movq	%rax, %rcx
	call	_Z13add_to_bufferPc
	movl	$0, %eax
	movb	%al, -96(%rbp)
	movq	my_trace(%rip), %rax
	leaq	70034472(%rax), %rcx
	movq	my_trace(%rip), %rax
	leaq	70033704(%rax), %rdx
	leaq	-96(%rbp), %rax
	movq	%rcx, %r9
	movq	%rdx, %r8
	leaq	.LC366(%rip), %rdx
	movq	%rax, %rcx
	call	sprintf
	movq	my_trace(%rip), %rax
	leaq	70036780(%rax), %rcx
	leaq	-96(%rbp), %rax
	movq	%rax, %rdx
	call	strcpy
	movq	my_trace(%rip), %rbx
	movq	my_trace(%rip), %rax
	addq	$70036780, %rax
	leaq	.LC359(%rip), %rdx
	movq	%rax, %rcx
	call	fopen
	movq	%rax, 70030608(%rbx)
	movq	my_trace(%rip), %rax
	addq	$70036780, %rax
	movq	%rax, %rdx
	leaq	.LC367(%rip), %rcx
	call	_Z7d_printPKcz
	call	_Z11send_reportv
	movq	my_trace(%rip), %rax
	movb	$1, 68981524(%rax)
	jmp	.L775
.L785:
	movl	$2, %r8d
	leaq	.LC368(%rip), %rdx
	movq	1792(%rbp), %rcx
	call	strncmp
	testl	%eax, %eax
	jne	.L789
	movq	my_trace(%rip), %rax
	movb	$3, 68981524(%rax)
	movq	my_trace(%rip), %rax
	addq	$68981624, %rax
	movq	%rax, %rcx
	call	_Z11ss_callbackPv
	movl	$0, %ecx
	call	_Z6set_ssm
	leaq	.LC369(%rip), %rcx
	call	_Z7d_printPKcz
	movq	my_trace(%rip), %rax
	movq	68981960(%rax), %rax
	movq	%rax, %rdx
	leaq	.LC370(%rip), %rcx
	call	_Z7d_printPKcz
	movq	my_trace(%rip), %rax
	movq	68981960(%rax), %rdx
	leaq	672(%rbp), %rax
	movq	%rdx, %r8
	leaq	.LC371(%rip), %rdx
	movq	%rax, %rcx
	call	sprintf
	leaq	672(%rbp), %rax
	movq	%rax, %rcx
	call	_Z13add_to_bufferPc
	call	_Z11send_reportv
	jmp	.L775
.L789:
	movl	$2, %r8d
	leaq	.LC372(%rip), %rdx
	movq	1792(%rbp), %rcx
	call	strncmp
	testl	%eax, %eax
	jne	.L790
	movq	my_trace(%rip), %rax
	movb	$5, 68981524(%rax)
	movq	my_trace(%rip), %rax
	addq	$68981624, %rax
	movq	%rax, %rcx
	call	_Z11ss_callbackPv
	movl	$0, %ecx
	call	_Z6set_ssm
	leaq	.LC373(%rip), %rcx
	call	_Z7d_printPKcz
	movq	my_trace(%rip), %rax
	movq	68981960(%rax), %rax
	movq	%rax, %rdx
	leaq	.LC370(%rip), %rcx
	call	_Z7d_printPKcz
	movq	my_trace(%rip), %rax
	movq	68981960(%rax), %rdx
	leaq	672(%rbp), %rax
	movq	%rdx, %r8
	leaq	.LC371(%rip), %rdx
	movq	%rax, %rcx
	call	sprintf
	leaq	672(%rbp), %rax
	movq	%rax, %rcx
	call	_Z13add_to_bufferPc
	call	_Z11send_reportv
	jmp	.L775
.L790:
	movl	$2, %r8d
	leaq	.LC374(%rip), %rdx
	movq	1792(%rbp), %rcx
	call	strncmp
	testl	%eax, %eax
	jne	.L791
	movq	my_trace(%rip), %rax
	movb	$4, 68981524(%rax)
	movq	my_trace(%rip), %rax
	addq	$68981624, %rax
	movq	%rax, %rcx
	call	_Z11ss_callbackPv
	movl	$0, %ecx
	call	_Z6set_ssm
	leaq	.LC375(%rip), %rcx
	call	_Z7d_printPKcz
	movq	my_trace(%rip), %rax
	movq	68981960(%rax), %rax
	movq	%rax, %rdx
	leaq	.LC370(%rip), %rcx
	call	_Z7d_printPKcz
	movq	my_trace(%rip), %rax
	movq	68981960(%rax), %rdx
	leaq	672(%rbp), %rax
	movq	%rdx, %r8
	leaq	.LC371(%rip), %rdx
	movq	%rax, %rcx
	call	sprintf
	leaq	672(%rbp), %rax
	movq	%rax, %rcx
	call	_Z13add_to_bufferPc
	call	_Z11send_reportv
	jmp	.L775
.L791:
	movl	$2, %r8d
	leaq	.LC376(%rip), %rdx
	movq	1792(%rbp), %rcx
	call	strncmp
	testl	%eax, %eax
	jne	.L792
	movq	my_trace(%rip), %rax
	movb	$6, 68981524(%rax)
	leaq	.LC377(%rip), %rcx
	call	_Z7d_printPKcz
	movq	my_trace(%rip), %rax
	movq	68981960(%rax), %rax
	movq	%rax, %rdx
	leaq	.LC378(%rip), %rcx
	call	_Z7d_printPKcz
	movq	my_trace(%rip), %rax
	movq	68981960(%rax), %rdx
	leaq	672(%rbp), %rax
	movq	%rdx, %r8
	leaq	.LC379(%rip), %rdx
	movq	%rax, %rcx
	call	sprintf
	leaq	672(%rbp), %rax
	movq	%rax, %rcx
	call	_Z13add_to_bufferPc
	call	_Z11send_reportv
	jmp	.L775
.L792:
	movl	$2, %r8d
	leaq	.LC380(%rip), %rdx
	movq	1792(%rbp), %rcx
	call	strncmp
	testl	%eax, %eax
	jne	.L793
	call	_Z11dump_memoryv
	call	_Z11send_reportv
	jmp	.L775
.L793:
	movl	$2, %r8d
	leaq	.LC381(%rip), %rdx
	movq	1792(%rbp), %rcx
	call	strncmp
	testl	%eax, %eax
	jne	.L794
	leaq	.LC382(%rip), %rdx
	movq	1792(%rbp), %rcx
	call	strtok
	movq	%rax, 1752(%rbp)
	leaq	.LC382(%rip), %rdx
	movl	$0, %ecx
	call	strtok
	movq	%rax, %rdx
	leaq	672(%rbp), %rax
	movq	%rax, %rcx
	call	strcpy
	leaq	672(%rbp), %rax
	movq	%rax, %rdx
	leaq	.LC383(%rip), %rcx
	call	_Z7d_printPKcz
	leaq	672(%rbp), %rax
	movq	%rax, %rcx
	call	_Z18exclusive_reactionPc
	call	_Z11send_reportv
	jmp	.L775
.L794:
	movl	$2, %r8d
	leaq	.LC384(%rip), %rdx
	movq	1792(%rbp), %rcx
	call	strncmp
	testl	%eax, %eax
	jne	.L795
	leaq	.LC382(%rip), %rdx
	movq	1792(%rbp), %rcx
	call	strtok
	movq	%rax, 1744(%rbp)
	leaq	.LC382(%rip), %rdx
	movl	$0, %ecx
	call	strtok
	movq	%rax, %rdx
	leaq	672(%rbp), %rax
	movq	%rax, %rcx
	call	strcpy
	leaq	672(%rbp), %rax
	movq	%rax, %rdx
	leaq	.LC383(%rip), %rcx
	call	_Z7d_printPKcz
	leaq	672(%rbp), %rax
	movq	%rax, %rcx
	call	_Z18exclusive_reactionPc
	call	_Z11send_reportv
	jmp	.L775
.L795:
	movl	$2, %r8d
	leaq	.LC385(%rip), %rdx
	movq	1792(%rbp), %rcx
	call	strncmp
	testl	%eax, %eax
	jne	.L796
	leaq	.LC382(%rip), %rdx
	movq	1792(%rbp), %rcx
	call	strtok
	movq	%rax, 1736(%rbp)
	leaq	.LC382(%rip), %rdx
	movl	$0, %ecx
	call	strtok
	movq	%rax, %rdx
	leaq	672(%rbp), %rax
	movq	%rax, %rcx
	call	strcpy
	leaq	672(%rbp), %rax
	movq	%rax, %rdx
	leaq	.LC386(%rip), %rcx
	call	_Z7d_printPKcz
	leaq	672(%rbp), %rax
	movq	%rax, %rcx
	call	_Z15enable_reactionPc
	call	_Z11send_reportv
	jmp	.L775
.L796:
	movl	$2, %r8d
	leaq	.LC387(%rip), %rdx
	movq	1792(%rbp), %rcx
	call	strncmp
	testl	%eax, %eax
	jne	.L797
	leaq	.LC382(%rip), %rdx
	movq	1792(%rbp), %rcx
	call	strtok
	movq	%rax, 1728(%rbp)
	leaq	.LC382(%rip), %rdx
	movl	$0, %ecx
	call	strtok
	movq	%rax, %rdx
	leaq	672(%rbp), %rax
	movq	%rax, %rcx
	call	strcpy
	leaq	672(%rbp), %rax
	movq	%rax, %rdx
	leaq	.LC388(%rip), %rcx
	call	_Z7d_printPKcz
	leaq	672(%rbp), %rax
	movq	%rax, %rcx
	call	_Z16disable_reactionPc
	call	_Z11send_reportv
	jmp	.L775
.L797:
	movl	$2, %r8d
	leaq	.LC389(%rip), %rdx
	movq	1792(%rbp), %rcx
	call	strncmp
	testl	%eax, %eax
	jne	.L798
	leaq	.LC390(%rip), %rcx
	call	_Z7d_printPKcz
	call	_Z20enable_all_reactionsv
	call	_Z11send_reportv
	jmp	.L775
.L798:
	movl	$2, %r8d
	leaq	.LC391(%rip), %rdx
	movq	1792(%rbp), %rcx
	call	strncmp
	testl	%eax, %eax
	jne	.L799
	leaq	.LC392(%rip), %rcx
	call	_Z7d_printPKcz
	call	_Z21disable_all_reactionsv
	call	_Z11send_reportv
	jmp	.L775
.L799:
	movl	$2, %r8d
	leaq	.LC393(%rip), %rdx
	movq	1792(%rbp), %rcx
	call	strncmp
	testl	%eax, %eax
	jne	.L800
	leaq	.LC382(%rip), %rdx
	movq	1792(%rbp), %rcx
	call	strtok
	movq	%rax, 1720(%rbp)
	leaq	.LC382(%rip), %rdx
	movl	$0, %ecx
	call	strtok
	movq	%rax, %rdx
	leaq	672(%rbp), %rax
	movq	%rax, %rcx
	call	strcpy
	leaq	672(%rbp), %rax
	movq	%rax, %rdx
	leaq	.LC394(%rip), %rcx
	call	_Z7d_printPKcz
	leaq	672(%rbp), %rax
	movq	%rax, %rcx
	call	_Z14raise_reactionPc
	call	_Z11send_reportv
	jmp	.L775
.L800:
	movl	$2, %r8d
	leaq	.LC395(%rip), %rdx
	movq	1792(%rbp), %rcx
	call	strncmp
	testl	%eax, %eax
	jne	.L801
	leaq	.LC382(%rip), %rdx
	movq	1792(%rbp), %rcx
	call	strtok
	movq	%rax, 1712(%rbp)
	leaq	.LC382(%rip), %rdx
	movl	$0, %ecx
	call	strtok
	movq	%rax, %rdx
	leaq	672(%rbp), %rax
	movq	%rax, %rcx
	call	strcpy
	leaq	672(%rbp), %rax
	movq	%rax, %rdx
	leaq	.LC396(%rip), %rcx
	call	_Z7d_printPKcz
	leaq	672(%rbp), %rax
	movq	%rax, %rcx
	call	_Z14lower_reactionPc
	call	_Z11send_reportv
	jmp	.L775
.L801:
	movl	$2, %r8d
	leaq	.LC397(%rip), %rdx
	movq	1792(%rbp), %rcx
	call	strncmp
	testl	%eax, %eax
	jne	.L802
	movq	1792(%rbp), %rax
	addq	$3, %rax
	movl	$16, %r8d
	movl	$0, %edx
	movq	%rax, %rcx
	call	strtoul
	movl	%eax, 1708(%rbp)
	movl	1708(%rbp), %eax
	movl	%eax, %ecx
	call	_Z11run_routinej
	call	_Z11send_reportv
	jmp	.L775
.L802:
	movl	$2, %r8d
	leaq	.LC398(%rip), %rdx
	movq	1792(%rbp), %rcx
	call	strncmp
	testl	%eax, %eax
	jne	.L803
	movq	my_trace(%rip), %rax
	movl	70033700(%rax), %eax
	movl	%eax, %edx
	leaq	.LC399(%rip), %rcx
	call	_Z7d_printPKcz
	call	_Z15start_trace_pidv
	movq	my_trace(%rip), %rax
	movb	$1, 68981524(%rax)
	movq	my_trace(%rip), %rbx
	call	_Z18get_pending_eventsv
	movl	%eax, 67932936(%rbx)
	call	_Z11send_reportv
	jmp	.L775
.L803:
	movl	$2, %r8d
	leaq	.LC400(%rip), %rdx
	movq	1792(%rbp), %rcx
	call	strncmp
	testl	%eax, %eax
	jne	.L804
	movq	my_trace(%rip), %rax
	leaq	70032164(%rax), %rdx
	movq	my_trace(%rip), %rax
	addq	$70039084, %rax
	movq	%rax, %rcx
	call	strcpy
	movq	my_trace(%rip), %rax
	leaq	70039084(%rax), %rdx
	movq	%rdx, %rax
	movq	$-1, %rcx
	movq	%rax, %r8
	movl	$0, %eax
	movq	%r8, %rdi
	repnz scasb
	movq	%rcx, %rax
	notq	%rax
	subq	$1, %rax
	addq	%rdx, %rax
	movw	$92, (%rax)
	movq	my_trace(%rip), %rax
	leaq	70032932(%rax), %rdx
	movq	my_trace(%rip), %rax
	addq	$70039084, %rax
	movq	%rax, %rcx
	call	strcat
	movq	my_trace(%rip), %rax
	addq	$70039084, %rax
	movq	%rax, %rdx
	leaq	.LC401(%rip), %rcx
	call	_Z7d_printPKcz
	call	_Z17start_trace_fnamev
	movq	my_trace(%rip), %rax
	movb	$1, 68981524(%rax)
	movq	my_trace(%rip), %rbx
	call	_Z18get_pending_eventsv
	movl	%eax, 67932936(%rbx)
	call	_Z11send_reportv
	jmp	.L775
.L804:
	movl	$2, %r8d
	leaq	.LC402(%rip), %rdx
	movq	1792(%rbp), %rcx
	call	strncmp
	testl	%eax, %eax
	jne	.L805
	movq	1792(%rbp), %rax
	addq	$3, %rax
	movl	$10, %r8d
	movl	$0, %edx
	movq	%rax, %rcx
	call	strtoul
	movl	%eax, 1704(%rbp)
	movl	1704(%rbp), %eax
	addl	$1, %eax
	movl	%eax, %ecx
	call	_Z12report_arg_xj
	call	_Z11send_reportv
	jmp	.L775
.L805:
	movl	$2, %r8d
	leaq	.LC403(%rip), %rdx
	movq	1792(%rbp), %rcx
	call	strncmp
	testl	%eax, %eax
	jne	.L806
	movq	1792(%rbp), %rax
	addq	$3, %rax
	movl	$10, %r8d
	movl	$0, %edx
	movq	%rax, %rcx
	call	strtoul
	movl	%eax, 1700(%rbp)
	movl	1700(%rbp), %eax
	addl	$1, %eax
	movl	%eax, %ecx
	call	_Z19report_arg_string_xj
	call	_Z11send_reportv
	jmp	.L775
.L806:
	movl	$2, %r8d
	leaq	.LC404(%rip), %rdx
	movq	1792(%rbp), %rcx
	call	strncmp
	testl	%eax, %eax
	jne	.L807
	movq	1792(%rbp), %rax
	addq	$3, %rax
	movl	$10, %r8d
	movl	$0, %edx
	movq	%rax, %rcx
	call	strtoul
	movl	%eax, 1696(%rbp)
	movl	1696(%rbp), %eax
	addl	$1, %eax
	movl	%eax, %ecx
	call	_Z27report_arg_unicode_string_xj
	call	_Z11send_reportv
	jmp	.L775
.L807:
	movl	$2, %r8d
	leaq	.LC405(%rip), %rdx
	movq	1792(%rbp), %rcx
	call	strncmp
	testl	%eax, %eax
	jne	.L808
	call	_Z9list_tebsv
	call	_Z11send_reportv
	jmp	.L775
.L808:
	movl	$2, %r8d
	leaq	.LC406(%rip), %rdx
	movq	1792(%rbp), %rcx
	call	strncmp
	testl	%eax, %eax
	jne	.L809
	call	_Z13list_all_tebsv
	call	_Z11send_reportv
	jmp	.L775
.L809:
	movl	$2, %r8d
	leaq	.LC407(%rip), %rdx
	movq	1792(%rbp), %rcx
	call	strncmp
	testl	%eax, %eax
	jne	.L810
	call	_Z14list_reactionsv
	call	_Z11send_reportv
	jmp	.L775
.L810:
	movl	$2, %r8d
	leaq	.LC408(%rip), %rdx
	movq	1792(%rbp), %rcx
	call	strncmp
	testl	%eax, %eax
	jne	.L811
	call	_Z9list_bptsv
	call	_Z11send_reportv
	jmp	.L775
.L811:
	movl	$2, %r8d
	leaq	.LC409(%rip), %rdx
	movq	1792(%rbp), %rcx
	call	strncmp
	testl	%eax, %eax
	jne	.L812
	call	_Z9list_libsv
	call	_Z11send_reportv
	jmp	.L775
.L812:
	movl	$2, %r8d
	leaq	.LC410(%rip), %rdx
	movq	1792(%rbp), %rcx
	call	strncmp
	testl	%eax, %eax
	jne	.L813
	leaq	.LC382(%rip), %rdx
	movq	1792(%rbp), %rcx
	call	strtok
	movq	%rax, 1688(%rbp)
	leaq	.LC382(%rip), %rdx
	movl	$0, %ecx
	call	strtok
	movl	$16, %r8d
	movl	$0, %edx
	movq	%rax, %rcx
	call	strtoul
	movl	%eax, %eax
	movq	%rax, 1680(%rbp)
	leaq	.LC382(%rip), %rdx
	movl	$0, %ecx
	call	strtok
	movl	$16, %r8d
	movl	$0, %edx
	movq	%rax, %rcx
	call	strtoul
	movl	%eax, %eax
	movq	%rax, 1672(%rbp)
	movq	my_trace(%rip), %rax
	movq	68981528(%rax), %rcx
	movq	1672(%rbp), %rdx
	movq	1680(%rbp), %rax
	movq	%rcx, %r9
	movq	%rdx, %r8
	movq	%rax, %rdx
	leaq	.LC411(%rip), %rcx
	call	_Z7d_printPKcz
	movq	1672(%rbp), %rax
	movl	%eax, %edx
	movq	1680(%rbp), %rax
	movq	%rax, %rcx
	call	_Z10out_regionym
	call	_Z11send_reportv
	jmp	.L775
.L813:
	movl	$2, %r8d
	leaq	.LC412(%rip), %rdx
	movq	1792(%rbp), %rcx
	call	strncmp
	testl	%eax, %eax
	jne	.L814
	leaq	.LC382(%rip), %rdx
	movq	1792(%rbp), %rcx
	call	strtok
	movq	%rax, 1664(%rbp)
	leaq	.LC382(%rip), %rdx
	movl	$0, %ecx
	call	strtok
	movl	$16, %r8d
	movl	$0, %edx
	movq	%rax, %rcx
	call	strtoul
	movl	%eax, %eax
	movq	%rax, 1656(%rbp)
	leaq	.LC382(%rip), %rdx
	movl	$0, %ecx
	call	strtok
	movl	$16, %r8d
	movl	$0, %edx
	movq	%rax, %rcx
	call	strtoul
	movl	%eax, 1652(%rbp)
	movl	1652(%rbp), %edx
	movq	1656(%rbp), %rax
	movl	%edx, %r8d
	movq	%rax, %rdx
	leaq	.LC413(%rip), %rcx
	call	_Z7d_printPKcz
	movl	1652(%rbp), %edx
	movq	1656(%rbp), %rax
	movq	%rax, %rcx
	call	_Z13report_regionym
	call	_Z11send_reportv
	jmp	.L775
.L814:
	movl	$2, %r8d
	leaq	.LC414(%rip), %rdx
	movq	1792(%rbp), %rcx
	call	strncmp
	testl	%eax, %eax
	jne	.L815
	leaq	.LC382(%rip), %rdx
	movq	1792(%rbp), %rcx
	call	strtok
	movq	%rax, 1640(%rbp)
	leaq	.LC382(%rip), %rdx
	movl	$0, %ecx
	call	strtok
	movl	$16, %r8d
	movl	$0, %edx
	movq	%rax, %rcx
	call	strtoul
	movl	%eax, %eax
	movq	%rax, 1632(%rbp)
	movq	1632(%rbp), %rax
	movq	%rax, %rdx
	leaq	.LC415(%rip), %rcx
	call	_Z7d_printPKcz
	movq	1632(%rbp), %rax
	movq	%rax, %rcx
	call	_Z12report_dwordy
	call	_Z11send_reportv
	jmp	.L775
.L815:
	movl	$2, %r8d
	leaq	.LC416(%rip), %rdx
	movq	1792(%rbp), %rcx
	call	strncmp
	testl	%eax, %eax
	jne	.L816
	leaq	.LC382(%rip), %rdx
	movq	1792(%rbp), %rcx
	call	strtok
	movq	%rax, 1624(%rbp)
	leaq	.LC382(%rip), %rdx
	movl	$0, %ecx
	call	strtok
	movl	$16, %r8d
	movl	$0, %edx
	movq	%rax, %rcx
	call	strtoul
	movl	%eax, %eax
	movq	%rax, 1616(%rbp)
	leaq	.LC327(%rip), %rdx
	movl	$0, %ecx
	call	strtok
	movq	%rax, 1608(%rbp)
	movq	1616(%rbp), %rdx
	movq	1608(%rbp), %rax
	movq	%rdx, %r8
	movq	%rax, %rdx
	leaq	.LC417(%rip), %rcx
	call	_Z7d_printPKcz
	movq	1608(%rbp), %rdx
	movq	1616(%rbp), %rax
	movq	%rax, %rcx
	call	_Z18write_string_asciiyPc
	call	_Z11send_reportv
	jmp	.L775
.L816:
	movl	$2, %r8d
	leaq	.LC418(%rip), %rdx
	movq	1792(%rbp), %rcx
	call	strncmp
	testl	%eax, %eax
	jne	.L817
	leaq	.LC382(%rip), %rdx
	movq	1792(%rbp), %rcx
	call	strtok
	movq	%rax, 1600(%rbp)
	leaq	.LC382(%rip), %rdx
	movl	$0, %ecx
	call	strtok
	movl	$16, %r8d
	movl	$0, %edx
	movq	%rax, %rcx
	call	strtoul
	movl	%eax, %eax
	movq	%rax, 1592(%rbp)
	leaq	.LC327(%rip), %rdx
	movl	$0, %ecx
	call	strtok
	movq	%rax, 1584(%rbp)
	movq	1592(%rbp), %rdx
	movq	1584(%rbp), %rax
	movq	%rdx, %r8
	movq	%rax, %rdx
	leaq	.LC417(%rip), %rcx
	call	_Z7d_printPKcz
	movq	1584(%rbp), %rdx
	movq	1592(%rbp), %rax
	movq	%rax, %rcx
	call	_Z20write_string_unicodeyPc
	call	_Z11send_reportv
	jmp	.L775
.L817:
	movl	$2, %r8d
	leaq	.LC419(%rip), %rdx
	movq	1792(%rbp), %rcx
	call	strncmp
	testl	%eax, %eax
	jne	.L818
	leaq	.LC382(%rip), %rdx
	movq	1792(%rbp), %rcx
	call	strtok
	movq	%rax, 1576(%rbp)
	leaq	.LC382(%rip), %rdx
	movl	$0, %ecx
	call	strtok
	movl	$16, %r8d
	movl	$0, %edx
	movq	%rax, %rcx
	call	strtoul
	movl	%eax, %eax
	movq	%rax, 1568(%rbp)
	leaq	.LC382(%rip), %rdx
	movl	$0, %ecx
	call	strtok
	movl	$16, %r8d
	movl	$0, %edx
	movq	%rax, %rcx
	call	strtoul
	movl	%eax, 1564(%rbp)
	movl	1564(%rbp), %edx
	movq	1568(%rbp), %rax
	movl	%edx, %r8d
	movq	%rax, %rdx
	leaq	.LC420(%rip), %rcx
	call	_Z7d_printPKcz
	movl	1564(%rbp), %edx
	movq	1568(%rbp), %rax
	movq	%rax, %rcx
	call	_Z11write_dwordyy
	call	_Z11send_reportv
	jmp	.L775
.L818:
	movl	$2, %r8d
	leaq	.LC421(%rip), %rdx
	movq	1792(%rbp), %rcx
	call	strncmp
	testl	%eax, %eax
	jne	.L819
	leaq	.LC382(%rip), %rdx
	movq	1792(%rbp), %rcx
	call	strtok
	movq	%rax, 1552(%rbp)
	leaq	.LC382(%rip), %rdx
	movl	$0, %ecx
	call	strtok
	movl	$16, %r8d
	movl	$0, %edx
	movq	%rax, %rcx
	call	strtoul
	movl	%eax, 1548(%rbp)
	movl	1548(%rbp), %eax
	movl	%eax, %ecx
	call	_Z14release_threadm
	call	_Z11send_reportv
	jmp	.L775
.L819:
	movl	$2, %r8d
	leaq	.LC422(%rip), %rdx
	movq	1792(%rbp), %rcx
	call	strncmp
	testl	%eax, %eax
	jne	.L820
	leaq	.LC382(%rip), %rdx
	movq	1792(%rbp), %rcx
	call	strtok
	movq	%rax, 1536(%rbp)
	leaq	.LC382(%rip), %rdx
	movl	$0, %ecx
	call	strtok
	movl	$16, %r8d
	movl	$0, %edx
	movq	%rax, %rcx
	call	strtoul
	movl	%eax, 1532(%rbp)
	leaq	.LC382(%rip), %rdx
	movl	$0, %ecx
	call	strtok
	movq	%rax, 1536(%rbp)
	movq	1536(%rbp), %rdx
	movl	1532(%rbp), %eax
	movl	%eax, %ecx
	call	_Z15report_registermPc
	call	_Z11send_reportv
	jmp	.L775
.L820:
	movl	$2, %r8d
	leaq	.LC423(%rip), %rdx
	movq	1792(%rbp), %rcx
	call	strncmp
	testl	%eax, %eax
	jne	.L821
	leaq	.LC382(%rip), %rdx
	movq	1792(%rbp), %rcx
	call	strtok
	movq	%rax, 1520(%rbp)
	leaq	.LC382(%rip), %rdx
	movl	$0, %ecx
	call	strtok
	movl	$16, %r8d
	movl	$0, %edx
	movq	%rax, %rcx
	call	strtoul
	movl	%eax, 1516(%rbp)
	leaq	.LC382(%rip), %rdx
	movl	$0, %ecx
	call	strtok
	movq	%rax, 1520(%rbp)
	leaq	.LC382(%rip), %rdx
	movl	$0, %ecx
	call	strtok
	movq	%rax, 1504(%rbp)
	movq	1504(%rbp), %rcx
	movq	1520(%rbp), %rdx
	movl	1516(%rbp), %eax
	movq	%rcx, %r8
	movl	%eax, %ecx
	call	_Z14write_registermPcS_
	call	_Z11send_reportv
	jmp	.L775
.L821:
	movl	$2, %r8d
	leaq	.LC424(%rip), %rdx
	movq	1792(%rbp), %rcx
	call	strncmp
	testl	%eax, %eax
	jne	.L822
	leaq	.LC382(%rip), %rdx
	movq	1792(%rbp), %rcx
	call	strtok
	movq	%rax, 1496(%rbp)
	leaq	.LC382(%rip), %rdx
	movl	$0, %ecx
	call	strtok
	movl	$16, %r8d
	movl	$0, %edx
	movq	%rax, %rcx
	call	strtoul
	movl	%eax, 1492(%rbp)
	leaq	.LC382(%rip), %rdx
	movl	$0, %ecx
	call	strtok
	movl	$16, %r8d
	movl	$0, %edx
	movq	%rax, %rcx
	call	strtoul
	movl	%eax, 1488(%rbp)
	movl	1488(%rbp), %edx
	movl	1492(%rbp), %eax
	movl	%eax, %ecx
	call	_Z10read_stackmm
	call	_Z11send_reportv
	jmp	.L775
.L822:
	movl	$2, %r8d
	leaq	.LC425(%rip), %rdx
	movq	1792(%rbp), %rcx
	call	strncmp
	testl	%eax, %eax
	jne	.L823
	leaq	.LC382(%rip), %rdx
	movq	1792(%rbp), %rcx
	call	strtok
	movq	%rax, 1480(%rbp)
	leaq	.LC382(%rip), %rdx
	movl	$0, %ecx
	call	strtok
	movq	%rax, 1480(%rbp)
	movq	1480(%rbp), %rax
	movl	$16, %r8d
	movl	$0, %edx
	movq	%rax, %rcx
	call	strtoul
	movl	%eax, 1476(%rbp)
	movl	1476(%rbp), %eax
	movl	%eax, %edx
	leaq	.LC426(%rip), %rcx
	call	_Z7d_printPKcz
	movl	1476(%rbp), %eax
	movl	%eax, %edx
	movl	$-1, %ecx
	call	_Z16continue_routinemj
	call	_Z11send_reportv
	jmp	.L775
.L823:
	movl	$2, %r8d
	leaq	.LC427(%rip), %rdx
	movq	1792(%rbp), %rcx
	call	strncmp
	testl	%eax, %eax
	jne	.L824
	leaq	.LC382(%rip), %rdx
	movq	1792(%rbp), %rcx
	call	strtok
	leaq	.LC382(%rip), %rdx
	movl	$0, %ecx
	call	strtok
	movl	$16, %r8d
	movl	$0, %edx
	movq	%rax, %rcx
	call	strtoul
	movl	%eax, 1472(%rbp)
	leaq	.LC382(%rip), %rdx
	movl	$0, %ecx
	call	strtok
	movl	$16, %r8d
	movl	$0, %edx
	movq	%rax, %rcx
	call	strtoul
	movl	%eax, 1468(%rbp)
	movl	1468(%rbp), %edx
	movl	1472(%rbp), %eax
	movl	%eax, %ecx
	call	_Z16continue_routinemj
	call	_Z11send_reportv
	jmp	.L775
.L824:
	movl	$2, %r8d
	leaq	.LC428(%rip), %rdx
	movq	1792(%rbp), %rcx
	call	strncmp
	testl	%eax, %eax
	jne	.L825
	leaq	.LC382(%rip), %rdx
	movq	1792(%rbp), %rcx
	call	strtok
	leaq	.LC382(%rip), %rdx
	movl	$0, %ecx
	call	strtok
	movl	$16, %r8d
	movl	$0, %edx
	movq	%rax, %rcx
	call	strtoul
	movl	%eax, 1464(%rbp)
	movl	1464(%rbp), %edx
	leaq	672(%rbp), %rax
	movl	%edx, %r8d
	leaq	.LC46(%rip), %rdx
	movq	%rax, %rcx
	call	sprintf
	leaq	672(%rbp), %rax
	leaq	.LC351(%rip), %rdx
	movq	%rax, %rcx
	call	_Z12add_reactionPcS_
	movl	$0, %edx
	leaq	.LC351(%rip), %rcx
	call	_Z7add_ridPcj
	leaq	.LC351(%rip), %rcx
	call	_Z15enable_reactionPc
	call	_Z11send_reportv
	jmp	.L775
.L825:
	movl	$2, %r8d
	leaq	.LC429(%rip), %rdx
	movq	1792(%rbp), %rcx
	call	strncmp
	testl	%eax, %eax
	jne	.L826
	movq	my_trace(%rip), %rax
	movq	176(%rax), %rax
	movq	%rax, %rdx
	leaq	672(%rbp), %rax
	movq	%rdx, %r8
	leaq	.LC46(%rip), %rdx
	movq	%rax, %rcx
	call	sprintf
	leaq	672(%rbp), %rax
	leaq	.LC351(%rip), %rdx
	movq	%rax, %rcx
	call	_Z12add_reactionPcS_
	movl	$0, %edx
	leaq	.LC351(%rip), %rcx
	call	_Z7add_ridPcj
	leaq	.LC351(%rip), %rcx
	call	_Z15enable_reactionPc
	call	_Z11send_reportv
	jmp	.L775
.L826:
	movl	$2, %r8d
	leaq	.LC430(%rip), %rdx
	movq	1792(%rbp), %rcx
	call	strncmp
	testl	%eax, %eax
	jne	.L827
	leaq	.LC382(%rip), %rdx
	movq	1792(%rbp), %rcx
	call	strtok
	leaq	.LC382(%rip), %rdx
	movl	$0, %ecx
	call	strtok
	movq	%rax, 1456(%rbp)
	leaq	.LC431(%rip), %rcx
	call	_Z7d_printPKcz
	movq	1456(%rbp), %rax
	movq	%rax, %rcx
	call	_Z15parse_reactionsPc
	call	_Z11send_reportv
	jmp	.L775
.L827:
	movl	$2, %r8d
	leaq	.LC432(%rip), %rdx
	movq	1792(%rbp), %rcx
	call	strncmp
	testl	%eax, %eax
	jne	.L828
	leaq	.LC382(%rip), %rdx
	movq	1792(%rbp), %rcx
	call	strtok
	leaq	.LC382(%rip), %rdx
	movl	$0, %ecx
	call	strtok
	movq	%rax, 1448(%rbp)
	movq	1448(%rbp), %rax
	movq	%rax, %rcx
	call	_Z13parse_regionsPc
	call	_Z11send_reportv
	jmp	.L775
.L828:
	movl	$2, %r8d
	leaq	.LC433(%rip), %rdx
	movq	1792(%rbp), %rcx
	call	strncmp
.L775:
	movl	$0, %eax
	addq	$1888, %rsp
	popq	%rbx
	popq	%rdi
	popq	%rbp
	ret
	.seh_endproc
	.globl	_Z10init_traceP13TRACE_CONFIG_Pcs
	.def	_Z10init_traceP13TRACE_CONFIG_Pcs;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z10init_traceP13TRACE_CONFIG_Pcs
_Z10init_traceP13TRACE_CONFIG_Pcs:
.LFB2567:
	pushq	%rbp
	.seh_pushreg	%rbp
	pushq	%rbx
	.seh_pushreg	%rbx
	subq	$40, %rsp
	.seh_stackalloc	40
	leaq	128(%rsp), %rbp
	.seh_setframe	%rbp, 128
	.seh_endprologue
	movq	%rcx, -64(%rbp)
	movq	%rdx, -56(%rbp)
	movl	%r8d, %eax
	movw	%ax, -48(%rbp)
	movq	my_trace(%rip), %rax
	movq	$0, 68981560(%rax)
	movq	my_trace(%rip), %rax
	movl	$0, 70039664(%rax)
	movq	my_trace(%rip), %rax
	leaq	67932412(%rax), %rcx
	movq	-56(%rbp), %rax
	movq	%rax, %rdx
	call	strcpy
	movq	my_trace(%rip), %rax
	movswl	-48(%rbp), %edx
	movl	%edx, 67932408(%rax)
	movq	my_trace(%rip), %rax
	addq	$224, %rax
	movl	$67108864, %r8d
	movl	$-1, %edx
	movq	%rax, %rcx
	call	memset
	movq	my_trace(%rip), %rbx
	movl	$0, %r8d
	movl	$0, %edx
	movl	$0, %ecx
	movq	__imp_CreateMutexA(%rip), %rax
	call	*%rax
	movq	%rax, 67649776(%rbx)
	movq	my_trace(%rip), %rbx
	movl	$0, %r9d
	movl	$0, %r8d
	movl	$0, %edx
	movl	$0, %ecx
	movq	__imp_CreateEventA(%rip), %rax
	call	*%rax
	movq	%rax, 67649760(%rbx)
	movq	my_trace(%rip), %rbx
	movl	$0, %r9d
	movl	$0, %r8d
	movl	$0, %edx
	movl	$0, %ecx
	movq	__imp_CreateEventA(%rip), %rax
	call	*%rax
	movq	%rax, 67649768(%rbx)
	movq	my_trace(%rip), %rax
	movq	$0, 68981960(%rax)
	movq	my_trace(%rip), %rax
	movl	$0, 68981968(%rax)
	movq	my_trace(%rip), %rbx
	movl	$102400, %eax
	movq	%rax, %rcx
	call	malloc
	movq	%rax, 70039656(%rbx)
	addq	$40, %rsp
	popq	%rbx
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC434:
	.ascii "Configuring syscall out args\12\0"
	.text
	.globl	_Z18configure_syscallsv
	.def	_Z18configure_syscallsv;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z18configure_syscallsv
_Z18configure_syscallsv:
.LFB2568:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$48, %rsp
	.seh_stackalloc	48
	.seh_endprologue
	leaq	.LC434(%rip), %rcx
	call	_Z7d_printPKcz
	movl	$0, -4(%rbp)
	jmp	.L832
.L835:
	movl	$0, -8(%rbp)
	jmp	.L833
.L834:
	movq	my_trace(%rip), %r8
	movl	-8(%rbp), %eax
	movl	-4(%rbp), %ecx
	movq	%rax, %rdx
	addq	%rdx, %rdx
	addq	%rax, %rdx
	leaq	0(,%rdx,8), %rax
	movq	%rax, %rdx
	movq	%rcx, %rax
	addq	%rax, %rax
	addq	%rcx, %rax
	salq	$7, %rax
	addq	%rdx, %rax
	addq	%r8, %rax
	addq	$70047184, %rax
	movb	$6, (%rax)
	addl	$1, -8(%rbp)
.L833:
	cmpl	$15, -8(%rbp)
	jbe	.L834
	addl	$1, -4(%rbp)
.L832:
	cmpl	$65535, -4(%rbp)
	jbe	.L835
	movq	my_trace(%rip), %rax
	movq	$1, 70054464(%rax)
	movq	$4, 70054472(%rax)
	movb	$2, 70054480(%rax)
	movb	$0, 70054481(%rax)
	movl	$-16711936, 70054484(%rax)
	movq	my_trace(%rip), %rax
	movq	$3, 70054488(%rax)
	movq	$4, 70054496(%rax)
	movb	$2, 70054504(%rax)
	movb	$0, 70054505(%rax)
	movl	$-16711936, 70054508(%rax)
	movq	my_trace(%rip), %rax
	movq	$1, 70054512(%rax)
	movq	$3, 70054520(%rax)
	movb	$3, 70054528(%rax)
	movb	$2, 70054529(%rax)
	movl	$0, 70054532(%rax)
	movq	my_trace(%rip), %rax
	movq	last_arg(%rip), %rdx
	movq	%rdx, 70054536(%rax)
	movq	8+last_arg(%rip), %rdx
	movq	%rdx, 70054544(%rax)
	movq	16+last_arg(%rip), %rdx
	movq	%rdx, 70054552(%rax)
	movq	my_trace(%rip), %rax
	movq	last_arg(%rip), %rdx
	movq	%rdx, 70066368(%rax)
	movq	8+last_arg(%rip), %rdx
	movq	%rdx, 70066376(%rax)
	movq	16+last_arg(%rip), %rdx
	movq	%rdx, 70066384(%rax)
	movq	my_trace(%rip), %rax
	movq	$0, 70072512(%rax)
	movq	$4, 70072520(%rax)
	movb	$2, 70072528(%rax)
	movb	$0, 70072529(%rax)
	movl	$-16711936, 70072532(%rax)
	movq	my_trace(%rip), %rax
	movq	$3, 70072536(%rax)
	movq	$16, 70072544(%rax)
	movb	$2, 70072552(%rax)
	movb	$0, 70072553(%rax)
	movl	$-16711936, 70072556(%rax)
	movq	my_trace(%rip), %rax
	movq	last_arg(%rip), %rdx
	movq	%rdx, 70072560(%rax)
	movq	8+last_arg(%rip), %rdx
	movq	%rdx, 70072568(%rax)
	movq	16+last_arg(%rip), %rdx
	movq	%rdx, 70072576(%rax)
	movq	my_trace(%rip), %rax
	movq	$0, 70079424(%rax)
	movq	$4, 70079432(%rax)
	movb	$2, 70079440(%rax)
	movb	$0, 70079441(%rax)
	movl	$-16711936, 70079444(%rax)
	movq	my_trace(%rip), %rax
	movq	last_arg(%rip), %rdx
	movq	%rdx, 70079448(%rax)
	movq	8+last_arg(%rip), %rdx
	movq	%rdx, 70079456(%rax)
	movq	16+last_arg(%rip), %rdx
	movq	%rdx, 70079464(%rax)
	movq	my_trace(%rip), %rax
	movq	$4, 70088256(%rax)
	movq	$16, 70088264(%rax)
	movb	$2, 70088272(%rax)
	movb	$0, 70088273(%rax)
	movl	$0, 70088276(%rax)
	movq	my_trace(%rip), %rax
	movq	$8, 70088280(%rax)
	movq	$9, 70088288(%rax)
	movb	$3, 70088296(%rax)
	movb	$2, 70088297(%rax)
	movl	$0, 70088300(%rax)
	movq	my_trace(%rip), %rax
	movq	last_arg(%rip), %rdx
	movq	%rdx, 70088304(%rax)
	movq	8+last_arg(%rip), %rdx
	movq	%rdx, 70088312(%rax)
	movq	16+last_arg(%rip), %rdx
	movq	%rdx, 70088320(%rax)
	movq	my_trace(%rip), %rax
	movq	$1, 70097472(%rax)
	movq	$2, 70097480(%rax)
	movb	$3, 70097488(%rax)
	movb	$2, 70097489(%rax)
	movl	$0, 70097492(%rax)
	movq	my_trace(%rip), %rax
	movq	$1, 70097496(%rax)
	movq	$4, 70097504(%rax)
	movb	$2, 70097512(%rax)
	movb	$0, 70097513(%rax)
	movl	$-16711936, 70097516(%rax)
	movq	my_trace(%rip), %rax
	movq	$2, 70097520(%rax)
	movq	$4, 70097528(%rax)
	movb	$2, 70097536(%rax)
	movb	$0, 70097537(%rax)
	movl	$-16711936, 70097540(%rax)
	movq	my_trace(%rip), %rax
	movq	last_arg(%rip), %rdx
	movq	%rdx, 70097544(%rax)
	movq	8+last_arg(%rip), %rdx
	movq	%rdx, 70097552(%rax)
	movq	16+last_arg(%rip), %rdx
	movq	%rdx, 70097560(%rax)
	movq	my_trace(%rip), %rax
	movq	$2, 70111680(%rax)
	movq	$6, 70111688(%rax)
	movb	$3, 70111696(%rax)
	movb	$2, 70111697(%rax)
	movl	$0, 70111700(%rax)
	movq	my_trace(%rip), %rax
	movq	$2, 70111704(%rax)
	movq	$4, 70111712(%rax)
	movb	$2, 70111720(%rax)
	movb	$0, 70111721(%rax)
	movl	$-16711936, 70111724(%rax)
	movq	my_trace(%rip), %rax
	movq	$5, 70111728(%rax)
	movq	$8, 70111736(%rax)
	movb	$2, 70111744(%rax)
	movb	$0, 70111745(%rax)
	movl	$-16711936, 70111748(%rax)
	movq	my_trace(%rip), %rax
	movq	$6, 70111752(%rax)
	movq	$4, 70111760(%rax)
	movb	$2, 70111768(%rax)
	movb	$0, 70111769(%rax)
	movl	$-16711936, 70111772(%rax)
	movq	my_trace(%rip), %rax
	movq	last_arg(%rip), %rdx
	movq	%rdx, 70111776(%rax)
	movq	8+last_arg(%rip), %rdx
	movq	%rdx, 70111784(%rax)
	movq	16+last_arg(%rip), %rdx
	movq	%rdx, 70111792(%rax)
	movq	my_trace(%rip), %rax
	movq	$0, 70114368(%rax)
	movq	$4, 70114376(%rax)
	movb	$2, 70114384(%rax)
	movb	$0, 70114385(%rax)
	movl	$-16711936, 70114388(%rax)
	movq	my_trace(%rip), %rax
	movq	last_arg(%rip), %rdx
	movq	%rdx, 70114392(%rax)
	movq	8+last_arg(%rip), %rdx
	movq	%rdx, 70114400(%rax)
	movq	16+last_arg(%rip), %rdx
	movq	%rdx, 70114408(%rax)
	movq	my_trace(%rip), %rax
	movq	$0, 70115904(%rax)
	movq	$4, 70115912(%rax)
	movb	$2, 70115920(%rax)
	movb	$0, 70115921(%rax)
	movl	$-16711936, 70115924(%rax)
	movq	my_trace(%rip), %rax
	movq	$3, 70115928(%rax)
	movq	$16, 70115936(%rax)
	movb	$2, 70115944(%rax)
	movb	$0, 70115945(%rax)
	movl	$-16711936, 70115948(%rax)
	movq	my_trace(%rip), %rax
	movq	last_arg(%rip), %rdx
	movq	%rdx, 70115952(%rax)
	movq	8+last_arg(%rip), %rdx
	movq	%rdx, 70115960(%rax)
	movq	16+last_arg(%rip), %rdx
	movq	%rdx, 70115968(%rax)
	movq	my_trace(%rip), %rax
	movq	$0, 70117056(%rax)
	movq	$4, 70117064(%rax)
	movb	$2, 70117072(%rax)
	movb	$0, 70117073(%rax)
	movl	$-16711936, 70117076(%rax)
	movq	my_trace(%rip), %rax
	movq	last_arg(%rip), %rdx
	movq	%rdx, 70117080(%rax)
	movq	8+last_arg(%rip), %rdx
	movq	%rdx, 70117088(%rax)
	movq	16+last_arg(%rip), %rdx
	movq	%rdx, 70117096(%rax)
	movq	my_trace(%rip), %rax
	movq	last_arg(%rip), %rdx
	movq	%rdx, 70118976(%rax)
	movq	8+last_arg(%rip), %rdx
	movq	%rdx, 70118984(%rax)
	movq	16+last_arg(%rip), %rdx
	movq	%rdx, 70118992(%rax)
	movq	my_trace(%rip), %rax
	movq	$2, 70120512(%rax)
	movq	$4, 70120520(%rax)
	movb	$2, 70120528(%rax)
	movb	$0, 70120529(%rax)
	movl	$-16711936, 70120532(%rax)
	movq	my_trace(%rip), %rax
	movq	last_arg(%rip), %rdx
	movq	%rdx, 70120536(%rax)
	movq	8+last_arg(%rip), %rdx
	movq	%rdx, 70120544(%rax)
	movq	16+last_arg(%rip), %rdx
	movq	%rdx, 70120552(%rax)
	movq	my_trace(%rip), %rax
	movq	$0, 70121664(%rax)
	movq	$4, 70121672(%rax)
	movb	$2, 70121680(%rax)
	movb	$0, 70121681(%rax)
	movl	$-16711936, 70121684(%rax)
	movq	my_trace(%rip), %rax
	movq	last_arg(%rip), %rdx
	movq	%rdx, 70121688(%rax)
	movq	8+last_arg(%rip), %rdx
	movq	%rdx, 70121696(%rax)
	movq	16+last_arg(%rip), %rdx
	movq	%rdx, 70121704(%rax)
	movq	my_trace(%rip), %rax
	movq	$0, 70122816(%rax)
	movq	$4, 70122824(%rax)
	movb	$2, 70122832(%rax)
	movb	$0, 70122833(%rax)
	movl	$-16711936, 70122836(%rax)
	movq	my_trace(%rip), %rax
	movq	last_arg(%rip), %rdx
	movq	%rdx, 70122840(%rax)
	movq	8+last_arg(%rip), %rdx
	movq	%rdx, 70122848(%rax)
	movq	16+last_arg(%rip), %rdx
	movq	%rdx, 70122856(%rax)
	movq	my_trace(%rip), %rax
	movq	$1, 70129728(%rax)
	movq	$4, 70129736(%rax)
	movb	$2, 70129744(%rax)
	movb	$0, 70129745(%rax)
	movl	$-16711936, 70129748(%rax)
	movq	my_trace(%rip), %rax
	movq	$3, 70129752(%rax)
	movq	$4, 70129760(%rax)
	movb	$2, 70129768(%rax)
	movb	$0, 70129769(%rax)
	movl	$-16711936, 70129772(%rax)
	movq	my_trace(%rip), %rax
	movq	$4, 70129776(%rax)
	movq	$4, 70129784(%rax)
	movb	$2, 70129792(%rax)
	movb	$0, 70129793(%rax)
	movl	$-16711936, 70129796(%rax)
	movq	my_trace(%rip), %rax
	movq	last_arg(%rip), %rdx
	movq	%rdx, 70129800(%rax)
	movq	8+last_arg(%rip), %rdx
	movq	%rdx, 70129808(%rax)
	movq	16+last_arg(%rip), %rdx
	movq	%rdx, 70129816(%rax)
	movq	my_trace(%rip), %rax
	movq	$0, 70130496(%rax)
	movq	$40, 70130504(%rax)
	movb	$2, 70130512(%rax)
	movb	$0, 70130513(%rax)
	movl	$-16711936, 70130516(%rax)
	movq	my_trace(%rip), %rax
	movq	last_arg(%rip), %rdx
	movq	%rdx, 70130520(%rax)
	movq	8+last_arg(%rip), %rdx
	movq	%rdx, 70130528(%rax)
	movq	16+last_arg(%rip), %rdx
	movq	%rdx, 70130536(%rax)
	movq	my_trace(%rip), %rax
	movq	$1, 70132032(%rax)
	movq	$4, 70132040(%rax)
	movb	$2, 70132048(%rax)
	movb	$0, 70132049(%rax)
	movl	$-16711936, 70132052(%rax)
	movq	my_trace(%rip), %rax
	movq	last_arg(%rip), %rdx
	movq	%rdx, 70132056(%rax)
	movq	8+last_arg(%rip), %rdx
	movq	%rdx, 70132064(%rax)
	movq	16+last_arg(%rip), %rdx
	movq	%rdx, 70132072(%rax)
	movq	my_trace(%rip), %rax
	movq	$4, 70132800(%rax)
	movq	$16, 70132808(%rax)
	movb	$2, 70132816(%rax)
	movb	$0, 70132817(%rax)
	movl	$-16711936, 70132820(%rax)
	movq	my_trace(%rip), %rax
	movq	$5, 70132824(%rax)
	movq	$6, 70132832(%rax)
	movb	$2, 70132840(%rax)
	movb	$1, 70132841(%rax)
	movl	$-16711936, 70132844(%rax)
	movq	my_trace(%rip), %rax
	movq	last_arg(%rip), %rdx
	movq	%rdx, 70132848(%rax)
	movq	8+last_arg(%rip), %rdx
	movq	%rdx, 70132856(%rax)
	movq	16+last_arg(%rip), %rdx
	movq	%rdx, 70132864(%rax)
	movq	my_trace(%rip), %rax
	movq	$1, 70135872(%rax)
	movq	$16, 70135880(%rax)
	movb	$2, 70135888(%rax)
	movb	$0, 70135889(%rax)
	movl	$-16711936, 70135892(%rax)
	movq	my_trace(%rip), %rax
	movq	$2, 70135896(%rax)
	movq	$3, 70135904(%rax)
	movb	$2, 70135912(%rax)
	movb	$1, 70135913(%rax)
	movl	$0, 70135916(%rax)
	movq	my_trace(%rip), %rax
	movq	last_arg(%rip), %rdx
	movq	%rdx, 70135920(%rax)
	movq	8+last_arg(%rip), %rdx
	movq	%rdx, 70135928(%rax)
	movq	16+last_arg(%rip), %rdx
	movq	%rdx, 70135936(%rax)
	movq	my_trace(%rip), %rax
	movq	$4, 70137024(%rax)
	movq	$4, 70137032(%rax)
	movb	$2, 70137040(%rax)
	movb	$0, 70137041(%rax)
	movl	$-16711936, 70137044(%rax)
	movq	my_trace(%rip), %rax
	movq	$2, 70137048(%rax)
	movq	$3, 70137056(%rax)
	movb	$2, 70137064(%rax)
	movb	$2, 70137065(%rax)
	movl	$0, 70137068(%rax)
	movq	my_trace(%rip), %rax
	movq	last_arg(%rip), %rdx
	movq	%rdx, 70137072(%rax)
	movq	8+last_arg(%rip), %rdx
	movq	%rdx, 70137080(%rax)
	movq	16+last_arg(%rip), %rdx
	movq	%rdx, 70137088(%rax)
	movq	my_trace(%rip), %rax
	movq	$4, 70138176(%rax)
	movq	$4, 70138184(%rax)
	movb	$2, 70138192(%rax)
	movb	$0, 70138193(%rax)
	movl	$-16711936, 70138196(%rax)
	movq	my_trace(%rip), %rax
	movq	$2, 70138200(%rax)
	movq	$4, 70138208(%rax)
	movb	$2, 70138216(%rax)
	movb	$2, 70138217(%rax)
	movl	$0, 70138220(%rax)
	movq	my_trace(%rip), %rax
	movq	last_arg(%rip), %rdx
	movq	%rdx, 70138224(%rax)
	movq	8+last_arg(%rip), %rdx
	movq	%rdx, 70138232(%rax)
	movq	16+last_arg(%rip), %rdx
	movq	%rdx, 70138240(%rax)
	movq	my_trace(%rip), %rax
	movq	$0, 70143552(%rax)
	movq	$8, 70143560(%rax)
	movb	$2, 70143568(%rax)
	movb	$2, 70143569(%rax)
	movl	$-16711936, 70143572(%rax)
	movq	my_trace(%rip), %rax
	movq	$1, 70143576(%rax)
	movq	$8, 70143584(%rax)
	movb	$2, 70143592(%rax)
	movb	$2, 70143593(%rax)
	movl	$-16711936, 70143596(%rax)
	movq	my_trace(%rip), %rax
	movq	last_arg(%rip), %rdx
	movq	%rdx, 70143600(%rax)
	movq	8+last_arg(%rip), %rdx
	movq	%rdx, 70143608(%rax)
	movq	16+last_arg(%rip), %rdx
	movq	%rdx, 70143616(%rax)
	movq	my_trace(%rip), %rax
	movq	$2, 70144704(%rax)
	movq	$4, 70144712(%rax)
	movb	$3, 70144720(%rax)
	movb	$2, 70144721(%rax)
	movl	$-16711936, 70144724(%rax)
	movq	my_trace(%rip), %rax
	movq	$4, 70144728(%rax)
	movq	$4, 70144736(%rax)
	movb	$2, 70144744(%rax)
	movb	$2, 70144745(%rax)
	movl	$0, 70144748(%rax)
	movq	my_trace(%rip), %rax
	movq	last_arg(%rip), %rdx
	movq	%rdx, 70144752(%rax)
	movq	8+last_arg(%rip), %rdx
	movq	%rdx, 70144760(%rax)
	movq	16+last_arg(%rip), %rdx
	movq	%rdx, 70144768(%rax)
	movq	my_trace(%rip), %rax
	movq	$1, 70147392(%rax)
	movq	$2, 70147400(%rax)
	movb	$2, 70147408(%rax)
	movb	$1, 70147409(%rax)
	movl	$0, 70147412(%rax)
	movq	my_trace(%rip), %rax
	movq	$3, 70147416(%rax)
	movq	$4, 70147424(%rax)
	movb	$2, 70147432(%rax)
	movb	$0, 70147433(%rax)
	movl	$0, 70147436(%rax)
	movq	my_trace(%rip), %rax
	movq	last_arg(%rip), %rdx
	movq	%rdx, 70147440(%rax)
	movq	8+last_arg(%rip), %rdx
	movq	%rdx, 70147448(%rax)
	movq	16+last_arg(%rip), %rdx
	movq	%rdx, 70147456(%rax)
	movq	my_trace(%rip), %rax
	movq	$0, 70148160(%rax)
	movq	$8, 70148168(%rax)
	movb	$2, 70148176(%rax)
	movb	$0, 70148177(%rax)
	movl	$0, 70148180(%rax)
	movq	my_trace(%rip), %rax
	movq	last_arg(%rip), %rdx
	movq	%rdx, 70148184(%rax)
	movq	8+last_arg(%rip), %rdx
	movq	%rdx, 70148192(%rax)
	movq	16+last_arg(%rip), %rdx
	movq	%rdx, 70148200(%rax)
	movq	my_trace(%rip), %rax
	movq	$3, 70149312(%rax)
	movq	$5, 70149320(%rax)
	movb	$2, 70149328(%rax)
	movb	$2, 70149329(%rax)
	movl	$0, 70149332(%rax)
	movq	my_trace(%rip), %rax
	movq	$3, 70149336(%rax)
	movq	$4, 70149344(%rax)
	movb	$2, 70149352(%rax)
	movb	$0, 70149353(%rax)
	movl	$-16711936, 70149356(%rax)
	movq	my_trace(%rip), %rax
	movq	$5, 70149360(%rax)
	movq	$4, 70149368(%rax)
	movb	$2, 70149376(%rax)
	movb	$0, 70149377(%rax)
	movl	$-16711936, 70149380(%rax)
	movq	my_trace(%rip), %rax
	movq	last_arg(%rip), %rdx
	movq	%rdx, 70149384(%rax)
	movq	8+last_arg(%rip), %rdx
	movq	%rdx, 70149392(%rax)
	movq	16+last_arg(%rip), %rdx
	movq	%rdx, 70149400(%rax)
	movq	my_trace(%rip), %rax
	movq	$1, 70150080(%rax)
	movq	$16, 70150088(%rax)
	movb	$2, 70150096(%rax)
	movb	$0, 70150097(%rax)
	movl	$-16711936, 70150100(%rax)
	movq	my_trace(%rip), %rax
	movq	$2, 70150104(%rax)
	movq	$3, 70150112(%rax)
	movb	$2, 70150120(%rax)
	movb	$1, 70150121(%rax)
	movl	$-16711936, 70150124(%rax)
	movq	my_trace(%rip), %rax
	movq	last_arg(%rip), %rdx
	movq	%rdx, 70150128(%rax)
	movq	8+last_arg(%rip), %rdx
	movq	%rdx, 70150136(%rax)
	movq	16+last_arg(%rip), %rdx
	movq	%rdx, 70150144(%rax)
	movq	my_trace(%rip), %rax
	movq	$0, 70152000(%rax)
	movq	$4, 70152008(%rax)
	movb	$2, 70152016(%rax)
	movb	$0, 70152017(%rax)
	movl	$-16711936, 70152020(%rax)
	movq	my_trace(%rip), %rax
	movq	$4, 70152024(%rax)
	movq	$16, 70152032(%rax)
	movb	$2, 70152040(%rax)
	movb	$0, 70152041(%rax)
	movl	$-16711936, 70152044(%rax)
	movq	my_trace(%rip), %rax
	movq	$5, 70152048(%rax)
	movq	$4, 70152056(%rax)
	movb	$2, 70152064(%rax)
	movb	$0, 70152065(%rax)
	movl	$-16711936, 70152068(%rax)
	movq	my_trace(%rip), %rax
	movq	$5, 70152072(%rax)
	movq	$6, 70152080(%rax)
	movb	$2, 70152088(%rax)
	movb	$1, 70152089(%rax)
	movl	$0, 70152092(%rax)
	movq	my_trace(%rip), %rax
	movq	last_arg(%rip), %rdx
	movq	%rdx, 70152096(%rax)
	movq	8+last_arg(%rip), %rdx
	movq	%rdx, 70152104(%rax)
	movq	16+last_arg(%rip), %rdx
	movq	%rdx, 70152112(%rax)
	movq	my_trace(%rip), %rax
	movq	$1, 70161984(%rax)
	movq	$384, 70161992(%rax)
	movb	$2, 70162000(%rax)
	movb	$0, 70162001(%rax)
	movl	$0, 70162004(%rax)
	movq	my_trace(%rip), %rax
	movq	$2, 70162008(%rax)
	movq	$384, 70162016(%rax)
	movb	$2, 70162024(%rax)
	movb	$0, 70162025(%rax)
	movl	$0, 70162028(%rax)
	movq	my_trace(%rip), %rax
	movq	last_arg(%rip), %rdx
	movq	%rdx, 70162032(%rax)
	movq	8+last_arg(%rip), %rdx
	movq	%rdx, 70162040(%rax)
	movq	16+last_arg(%rip), %rdx
	movq	%rdx, 70162048(%rax)
	movq	my_trace(%rip), %rax
	movq	$1, 70173504(%rax)
	movq	$16, 70173512(%rax)
	movb	$2, 70173520(%rax)
	movb	$0, 70173521(%rax)
	movl	$-16711936, 70173524(%rax)
	movq	my_trace(%rip), %rax
	movq	last_arg(%rip), %rdx
	movq	%rdx, 70173528(%rax)
	movq	8+last_arg(%rip), %rdx
	movq	%rdx, 70173536(%rax)
	movq	16+last_arg(%rip), %rdx
	movq	%rdx, 70173544(%rax)
	movq	my_trace(%rip), %rax
	movq	$3, 70191168(%rax)
	movq	$4, 70191176(%rax)
	movb	$2, 70191184(%rax)
	movb	$0, 70191185(%rax)
	movl	$0, 70191188(%rax)
	movq	my_trace(%rip), %rax
	movq	$5, 70191192(%rax)
	movq	$4, 70191200(%rax)
	movb	$2, 70191208(%rax)
	movb	$0, 70191209(%rax)
	movl	$0, 70191212(%rax)
	movq	my_trace(%rip), %rax
	movq	last_arg(%rip), %rdx
	movq	%rdx, 70191216(%rax)
	movq	8+last_arg(%rip), %rdx
	movq	%rdx, 70191224(%rax)
	movq	16+last_arg(%rip), %rdx
	movq	%rdx, 70191232(%rax)
	movq	my_trace(%rip), %rax
	movq	last_arg(%rip), %rdx
	movq	%rdx, 70195008(%rax)
	movq	8+last_arg(%rip), %rdx
	movq	%rdx, 70195016(%rax)
	movq	16+last_arg(%rip), %rdx
	movq	%rdx, 70195024(%rax)
	movq	my_trace(%rip), %rax
	movq	last_arg(%rip), %rdx
	movq	%rdx, 70197312(%rax)
	movq	8+last_arg(%rip), %rdx
	movq	%rdx, 70197320(%rax)
	movq	16+last_arg(%rip), %rdx
	movq	%rdx, 70197328(%rax)
	movq	my_trace(%rip), %rax
	movq	$4, 70200384(%rax)
	movq	$4, 70200392(%rax)
	movb	$2, 70200400(%rax)
	movb	$0, 70200401(%rax)
	movl	$0, 70200404(%rax)
	movq	my_trace(%rip), %rax
	movq	last_arg(%rip), %rdx
	movq	%rdx, 70200408(%rax)
	movq	8+last_arg(%rip), %rdx
	movq	%rdx, 70200416(%rax)
	movq	16+last_arg(%rip), %rdx
	movq	%rdx, 70200424(%rax)
	movl	$0, %eax
	addq	$48, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.def	__main;	.scl	2;	.type	32;	.endef
	.section .rdata,"dr"
.LC435:
	.ascii "%d - %s\12\0"
	.align 8
.LC436:
	.ascii "You need do provide host and port\12\0"
.LC437:
	.ascii "Version 3.0\12\0"
	.align 8
.LC438:
	.ascii "Unable to allocate trace config\12\0"
.LC439:
	.ascii "\12Initialising Winsock...\0"
.LC440:
	.ascii "Failed. Error Code : %d\0"
.LC441:
	.ascii "Initialised\12\0"
.LC442:
	.ascii "Could not create socket : %d\0"
.LC443:
	.ascii "Socket created\12\0"
	.align 8
.LC444:
	.ascii "ioctlsocket failed with error: %ld\12\0"
.LC445:
	.ascii "127.0.0.1\0"
.LC446:
	.ascii "connect error\0"
.LC447:
	.ascii "Connected\12\0"
.LC448:
	.ascii "-=OK=-\0"
.LC449:
	.ascii "quit\0"
	.text
	.globl	main
	.def	main;	.scl	2;	.type	32;	.endef
	.seh_proc	main
main:
.LFB2569:
	pushq	%rbp
	.seh_pushreg	%rbp
	subq	$2048, %rsp
	.seh_stackalloc	2048
	leaq	128(%rsp), %rbp
	.seh_setframe	%rbp, 128
	.seh_endprologue
	movl	%ecx, 1936(%rbp)
	movq	%rdx, 1944(%rbp)
	call	__main
	movl	$0, 1916(%rbp)
	movl	$0, 1916(%rbp)
	jmp	.L838
.L839:
	movl	1916(%rbp), %eax
	cltq
	leaq	0(,%rax,8), %rdx
	movq	1944(%rbp), %rax
	addq	%rdx, %rax
	movq	(%rax), %rdx
	movl	1916(%rbp), %eax
	movq	%rdx, %r8
	movl	%eax, %edx
	leaq	.LC435(%rip), %rcx
	call	_Z7d_printPKcz
	addl	$1, 1916(%rbp)
.L838:
	movl	1916(%rbp), %eax
	cmpl	1936(%rbp), %eax
	jl	.L839
	cmpl	$2, 1936(%rbp)
	jg	.L840
	leaq	.LC436(%rip), %rcx
	call	_Z7d_printPKcz
	movl	$-1, %eax
	jmp	.L857
.L840:
	leaq	.LC437(%rip), %rcx
	call	_Z7d_printPKcz
	movq	1944(%rbp), %rax
	addq	$8, %rax
	movq	(%rax), %rax
	movq	%rax, %rcx
	call	strlen
	cmpq	$768, %rax
	jbe	.L842
	movl	$-1, %eax
	jmp	.L857
.L842:
	movq	1944(%rbp), %rax
	addq	$16, %rax
	movq	(%rax), %rax
	movq	%rax, %rcx
	call	strlen
	cmpq	$768, %rax
	jbe	.L843
	movl	$-1, %eax
	jmp	.L857
.L843:
	movl	$95213248, %ecx
	call	malloc
	movq	%rax, my_trace(%rip)
	movq	my_trace(%rip), %rax
	testq	%rax, %rax
	jne	.L844
	leaq	.LC438(%rip), %rcx
	call	_Z7d_printPKcz
	movl	$-1, %eax
	jmp	.L857
.L844:
	movq	1944(%rbp), %rax
	addq	$16, %rax
	movq	(%rax), %rax
	movq	%rax, %rcx
	call	atoi
	movswl	%ax, %ecx
	movq	1944(%rbp), %rax
	addq	$8, %rax
	movq	(%rax), %rdx
	movq	my_trace(%rip), %rax
	movl	%ecx, %r8d
	movq	%rax, %rcx
	call	_Z10init_traceP13TRACE_CONFIG_Pcs
	call	_Z18configure_syscallsv
	movq	my_trace(%rip), %rax
	leaq	_Z23react_sysenter_callbackPv(%rip), %rdx
	movq	%rdx, 70039680(%rax)
	movq	my_trace(%rip), %rax
	leaq	_Z21react_sysret_callbackPv(%rip), %rdx
	movq	%rdx, 70039688(%rax)
	movq	my_trace(%rip), %rax
	leaq	_Z20react_sysret_refreshPv(%rip), %rdx
	movq	%rdx, 70039696(%rax)
	movq	my_trace(%rip), %rax
	leaq	_Z22react_sample_routine_1Pv(%rip), %rdx
	movq	%rdx, 70041720(%rax)
	movq	my_trace(%rip), %rax
	leaq	_Z13react_zero_SFPv(%rip), %rdx
	movq	%rdx, 70041728(%rax)
	movq	my_trace(%rip), %rax
	leaq	_Z12react_set_ZFPv(%rip), %rdx
	movq	%rdx, 70041736(%rax)
	movq	my_trace(%rip), %rax
	leaq	_Z13react_zero_ZFPv(%rip), %rdx
	movq	%rdx, 70041744(%rax)
	movq	my_trace(%rip), %rax
	leaq	_Z14react_zero_EAXPv(%rip), %rdx
	movq	%rdx, 70041752(%rax)
	movq	my_trace(%rip), %rax
	leaq	_Z13react_flip_ZFPv(%rip), %rdx
	movq	%rdx, 70041760(%rax)
	movq	my_trace(%rip), %rax
	leaq	_Z13react_flip_CFPv(%rip), %rdx
	movq	%rdx, 70041768(%rax)
	movq	my_trace(%rip), %rax
	leaq	_Z13react_flip_SFPv(%rip), %rdx
	movq	%rdx, 70041776(%rax)
	movq	my_trace(%rip), %rax
	leaq	_Z13react_zero_CFPv(%rip), %rdx
	movq	%rdx, 70041784(%rax)
	movq	my_trace(%rip), %rax
	leaq	_Z12react_set_CFPv(%rip), %rdx
	movq	%rdx, 70041792(%rax)
	movq	my_trace(%rip), %rax
	leaq	_Z21react_update_region_1Pv(%rip), %rdx
	movq	%rdx, 70043776(%rax)
	movq	my_trace(%rip), %rax
	leaq	_Z21react_cry_antidebug_1Pv(%rip), %rdx
	movq	%rdx, 70043784(%rax)
	movq	my_trace(%rip), %rax
	leaq	_Z13react_skip_onPv(%rip), %rdx
	movq	%rdx, 70043792(%rax)
	movq	my_trace(%rip), %rax
	leaq	_Z14react_skip_offPv(%rip), %rdx
	movq	%rdx, 70043800(%rax)
	movq	my_trace(%rip), %rax
	leaq	_Z21react_update_region_2Pv(%rip), %rdx
	movq	%rdx, 70043808(%rax)
	movq	my_trace(%rip), %rax
	leaq	_Z18react_output_arg_0Pv(%rip), %rdx
	movq	%rdx, 70045816(%rax)
	movq	my_trace(%rip), %rax
	leaq	_Z18react_output_arg_1Pv(%rip), %rdx
	movq	%rdx, 70045824(%rax)
	movq	my_trace(%rip), %rax
	leaq	_Z18react_output_arg_2Pv(%rip), %rdx
	movq	%rdx, 70045832(%rax)
	movq	my_trace(%rip), %rax
	leaq	_Z18react_output_arg_3Pv(%rip), %rdx
	movq	%rdx, 70045840(%rax)
	movq	my_trace(%rip), %rax
	leaq	_Z18react_output_arg_4Pv(%rip), %rdx
	movq	%rdx, 70045848(%rax)
	movq	my_trace(%rip), %rax
	leaq	_Z18react_output_arg_5Pv(%rip), %rdx
	movq	%rdx, 70045856(%rax)
	movq	my_trace(%rip), %rax
	leaq	_Z18react_output_arg_6Pv(%rip), %rdx
	movq	%rdx, 70045864(%rax)
	movq	my_trace(%rip), %rax
	leaq	_Z18react_output_arg_7Pv(%rip), %rdx
	movq	%rdx, 70045872(%rax)
	movq	my_trace(%rip), %rax
	leaq	_Z18react_output_arg_8Pv(%rip), %rdx
	movq	%rdx, 70045880(%rax)
	movq	my_trace(%rip), %rax
	leaq	_Z22react_output_arg_str_0Pv(%rip), %rdx
	movq	%rdx, 70045944(%rax)
	movq	my_trace(%rip), %rax
	leaq	_Z22react_output_arg_str_1Pv(%rip), %rdx
	movq	%rdx, 70045952(%rax)
	movq	my_trace(%rip), %rax
	leaq	_Z22react_output_arg_str_2Pv(%rip), %rdx
	movq	%rdx, 70045960(%rax)
	movq	my_trace(%rip), %rax
	leaq	_Z22react_output_arg_str_3Pv(%rip), %rdx
	movq	%rdx, 70045968(%rax)
	movq	my_trace(%rip), %rax
	leaq	_Z22react_output_arg_str_4Pv(%rip), %rdx
	movq	%rdx, 70045976(%rax)
	movq	my_trace(%rip), %rax
	leaq	_Z22react_output_arg_str_5Pv(%rip), %rdx
	movq	%rdx, 70045984(%rax)
	movq	my_trace(%rip), %rax
	leaq	_Z22react_output_arg_str_6Pv(%rip), %rdx
	movq	%rdx, 70045992(%rax)
	movq	my_trace(%rip), %rax
	leaq	_Z22react_output_arg_str_7Pv(%rip), %rdx
	movq	%rdx, 70046000(%rax)
	movq	my_trace(%rip), %rax
	leaq	_Z22react_output_arg_str_8Pv(%rip), %rdx
	movq	%rdx, 70046008(%rax)
	movq	my_trace(%rip), %rax
	leaq	_Z30react_output_arg_unicode_str_0Pv(%rip), %rdx
	movq	%rdx, 70046072(%rax)
	movq	my_trace(%rip), %rax
	leaq	_Z30react_output_arg_unicode_str_1Pv(%rip), %rdx
	movq	%rdx, 70046080(%rax)
	movq	my_trace(%rip), %rax
	leaq	_Z30react_output_arg_unicode_str_2Pv(%rip), %rdx
	movq	%rdx, 70046088(%rax)
	movq	my_trace(%rip), %rax
	leaq	_Z30react_output_arg_unicode_str_3Pv(%rip), %rdx
	movq	%rdx, 70046096(%rax)
	movq	my_trace(%rip), %rax
	leaq	_Z30react_output_arg_unicode_str_4Pv(%rip), %rdx
	movq	%rdx, 70046104(%rax)
	movq	my_trace(%rip), %rax
	leaq	_Z30react_output_arg_unicode_str_5Pv(%rip), %rdx
	movq	%rdx, 70046112(%rax)
	movq	my_trace(%rip), %rax
	leaq	_Z30react_output_arg_unicode_str_6Pv(%rip), %rdx
	movq	%rdx, 70046120(%rax)
	movq	my_trace(%rip), %rax
	leaq	_Z30react_output_arg_unicode_str_7Pv(%rip), %rdx
	movq	%rdx, 70046128(%rax)
	movq	my_trace(%rip), %rax
	leaq	_Z30react_output_arg_unicode_str_8Pv(%rip), %rdx
	movq	%rdx, 70046136(%rax)
	movq	my_trace(%rip), %rax
	leaq	_Z16react_output_eaxPv(%rip), %rdx
	movq	%rdx, 70046200(%rax)
	movq	my_trace(%rip), %rax
	leaq	_Z16react_output_ebxPv(%rip), %rdx
	movq	%rdx, 70046208(%rax)
	movq	my_trace(%rip), %rax
	leaq	_Z16react_output_ecxPv(%rip), %rdx
	movq	%rdx, 70046216(%rax)
	movq	my_trace(%rip), %rax
	leaq	_Z16react_output_edxPv(%rip), %rdx
	movq	%rdx, 70046224(%rax)
	movq	my_trace(%rip), %rax
	leaq	_Z16react_output_esiPv(%rip), %rdx
	movq	%rdx, 70046232(%rax)
	movq	my_trace(%rip), %rax
	leaq	_Z16react_output_ediPv(%rip), %rdx
	movq	%rdx, 70046240(%rax)
	movq	my_trace(%rip), %rax
	leaq	_Z16react_output_espPv(%rip), %rdx
	movq	%rdx, 70046248(%rax)
	movq	my_trace(%rip), %rax
	leaq	_Z16react_output_eipPv(%rip), %rdx
	movq	%rdx, 70046256(%rax)
	movq	my_trace(%rip), %rax
	leaq	_Z20react_output_p_arg_0Pv(%rip), %rdx
	movq	%rdx, 70046328(%rax)
	movq	my_trace(%rip), %rax
	leaq	_Z20react_output_p_arg_1Pv(%rip), %rdx
	movq	%rdx, 70046336(%rax)
	movq	my_trace(%rip), %rax
	leaq	_Z20react_output_p_arg_2Pv(%rip), %rdx
	movq	%rdx, 70046344(%rax)
	movq	my_trace(%rip), %rax
	leaq	_Z20react_output_p_arg_3Pv(%rip), %rdx
	movq	%rdx, 70046352(%rax)
	movq	my_trace(%rip), %rax
	leaq	_Z20react_output_p_arg_4Pv(%rip), %rdx
	movq	%rdx, 70046360(%rax)
	movq	my_trace(%rip), %rax
	leaq	_Z20react_output_p_arg_5Pv(%rip), %rdx
	movq	%rdx, 70046368(%rax)
	movq	my_trace(%rip), %rax
	leaq	_Z20react_output_p_arg_6Pv(%rip), %rdx
	movq	%rdx, 70046376(%rax)
	movq	my_trace(%rip), %rax
	leaq	_Z20react_output_p_arg_7Pv(%rip), %rdx
	movq	%rdx, 70046384(%rax)
	movq	my_trace(%rip), %rax
	leaq	_Z20react_output_p_arg_8Pv(%rip), %rdx
	movq	%rdx, 70046392(%rax)
	leaq	.LC439(%rip), %rcx
	call	_Z7d_printPKcz
	leaq	-96(%rbp), %rax
	movq	%rax, %rdx
	movl	$514, %ecx
	movq	__imp_WSAStartup(%rip), %rax
	call	*%rax
	testl	%eax, %eax
	setne	%al
	testb	%al, %al
	je	.L845
	movq	__imp_WSAGetLastError(%rip), %rax
	call	*%rax
	movl	%eax, %edx
	leaq	.LC440(%rip), %rcx
	call	_Z7d_printPKcz
	movl	$1, %eax
	jmp	.L857
.L845:
	leaq	.LC441(%rip), %rcx
	call	_Z7d_printPKcz
	movl	$0, %r8d
	movl	$1, %edx
	movl	$2, %ecx
	movq	__imp_socket(%rip), %rax
	call	*%rax
	movq	%rax, 1904(%rbp)
	cmpq	$-1, 1904(%rbp)
	sete	%al
	testb	%al, %al
	je	.L846
	movq	__imp_WSAGetLastError(%rip), %rax
	call	*%rax
	movl	%eax, %edx
	leaq	.LC442(%rip), %rcx
	call	_Z7d_printPKcz
.L846:
	leaq	.LC443(%rip), %rcx
	call	_Z7d_printPKcz
	movl	$0, 1868(%rbp)
	movl	$4, %eax
	andl	$127, %eax
	sall	$16, %eax
	orl	$-2147457410, %eax
	movl	%eax, %ecx
	leaq	1868(%rbp), %rdx
	movq	1904(%rbp), %rax
	movq	%rdx, %r8
	movl	%ecx, %edx
	movq	%rax, %rcx
	movq	__imp_ioctlsocket(%rip), %rax
	call	*%rax
	movl	%eax, 1900(%rbp)
	cmpl	$0, 1900(%rbp)
	je	.L847
	movl	1900(%rbp), %eax
	movl	%eax, %edx
	leaq	.LC444(%rip), %rcx
	call	_Z7d_printPKcz
.L847:
	leaq	.LC445(%rip), %rcx
	movq	__imp_inet_addr(%rip), %rax
	call	*%rax
	movl	%eax, 1876(%rbp)
	movw	$2, 1872(%rbp)
	movl	$12341, %ecx
	movq	__imp_htons(%rip), %rax
	call	*%rax
	movw	%ax, 1874(%rbp)
	movl	$4, %eax
	andl	$127, %eax
	sall	$16, %eax
	orl	$-2147457410, %eax
	movl	%eax, %ecx
	leaq	1868(%rbp), %rdx
	movq	1904(%rbp), %rax
	movq	%rdx, %r8
	movl	%ecx, %edx
	movq	%rax, %rcx
	movq	__imp_ioctlsocket(%rip), %rax
	call	*%rax
	movl	%eax, 1900(%rbp)
	cmpl	$0, 1900(%rbp)
	je	.L848
	movl	1900(%rbp), %eax
	movl	%eax, %edx
	leaq	.LC444(%rip), %rcx
	call	_Z7d_printPKcz
.L848:
	movl	$16, %ecx
	leaq	1872(%rbp), %rdx
	movq	1904(%rbp), %rax
	movl	%ecx, %r8d
	movq	%rax, %rcx
	movq	__imp_connect(%rip), %rax
	call	*%rax
	shrl	$31, %eax
	testb	%al, %al
	je	.L849
	leaq	.LC446(%rip), %rcx
	call	_Z7d_printPKcz
	movl	$1, %eax
	jmp	.L857
.L849:
	movq	my_trace(%rip), %rax
	movq	1904(%rbp), %rdx
	movq	%rdx, 70030616(%rax)
	movl	$4, %eax
	andl	$127, %eax
	sall	$16, %eax
	orl	$-2147457410, %eax
	movl	%eax, %ecx
	leaq	1868(%rbp), %rdx
	movq	1904(%rbp), %rax
	movq	%rdx, %r8
	movl	%ecx, %edx
	movq	%rax, %rcx
	movq	__imp_ioctlsocket(%rip), %rax
	call	*%rax
	movl	%eax, 1900(%rbp)
	cmpl	$0, 1900(%rbp)
	je	.L850
	movl	1900(%rbp), %eax
	movl	%eax, %edx
	leaq	.LC444(%rip), %rcx
	call	_Z7d_printPKcz
.L850:
	leaq	.LC447(%rip), %rcx
	call	_Z7d_printPKcz
.L856:
	movl	$0, %eax
	movb	%al, 320(%rbp)
	movl	$0, %eax
	movb	%al, 1088(%rbp)
	movl	$0, 1912(%rbp)
.L853:
	leaq	1088(%rbp), %rdx
	movq	1904(%rbp), %rax
	movl	$0, %r9d
	movl	$767, %r8d
	movq	%rax, %rcx
	movq	__imp_recv(%rip), %rax
	call	*%rax
	movl	%eax, 1896(%rbp)
	leaq	1088(%rbp), %rdx
	leaq	320(%rbp), %rax
	movq	%rax, %rcx
	call	strcat
	movl	1896(%rbp), %eax
	addl	%eax, 1912(%rbp)
	movl	1912(%rbp), %eax
	leaq	-6(%rax), %rdx
	leaq	320(%rbp), %rax
	addq	%rdx, %rax
	movl	$6, %r8d
	leaq	.LC448(%rip), %rdx
	movq	%rax, %rcx
	call	strncmp
	testl	%eax, %eax
	jne	.L851
	nop
	movl	1912(%rbp), %eax
	subl	$6, %eax
	movl	%eax, %eax
	movb	$0, 320(%rbp,%rax)
	leaq	320(%rbp), %rax
	leaq	.LC449(%rip), %rdx
	movq	%rax, %rcx
	call	strcmp
	testl	%eax, %eax
	jne	.L854
	jmp	.L858
.L851:
	jmp	.L853
.L858:
	nop
	movl	$0, %eax
	jmp	.L857
.L854:
	leaq	320(%rbp), %rax
	movq	%rax, %rcx
	call	_Z10handle_cmdPc
	jmp	.L856
.L857:
	addq	$2048, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.ident	"GCC: (GNU) 4.9.3"
	.def	vfprintf;	.scl	2;	.type	32;	.endef
	.def	fflush;	.scl	2;	.type	32;	.endef
	.def	_Z12read_contextyP8_CONTEXT;	.scl	2;	.type	32;	.endef
	.def	_Z13write_contextyP8_CONTEXT;	.scl	2;	.type	32;	.endef
	.def	ftell;	.scl	2;	.type	32;	.endef
	.def	sprintf;	.scl	2;	.type	32;	.endef
	.def	strcpy;	.scl	2;	.type	32;	.endef
	.def	strcpy;	.scl	2;	.type	32;	.endef
	.def	strlen;	.scl	2;	.type	32;	.endef
	.def	malloc;	.scl	2;	.type	32;	.endef
	.def	free;	.scl	2;	.type	32;	.endef
	.def	strchr;	.scl	2;	.type	32;	.endef
	.def	strtok;	.scl	2;	.type	32;	.endef
	.def	strcmp;	.scl	2;	.type	32;	.endef
	.def	fwrite;	.scl	2;	.type	32;	.endef
	.def	sscanf;	.scl	2;	.type	32;	.endef
	.def	strcmpi;	.scl	2;	.type	32;	.endef
	.def	CreateToolhelp32Snapshot;	.scl	2;	.type	32;	.endef
	.def	Thread32First;	.scl	2;	.type	32;	.endef
	.def	Thread32Next;	.scl	2;	.type	32;	.endef
	.def	exit;	.scl	2;	.type	32;	.endef
	.def	memset;	.scl	2;	.type	32;	.endef
	.def	fopen;	.scl	2;	.type	32;	.endef
	.def	fclose;	.scl	2;	.type	32;	.endef
	.def	itoa;	.scl	2;	.type	32;	.endef
	.def	strcspn;	.scl	2;	.type	32;	.endef
	.def	putchar;	.scl	2;	.type	32;	.endef
	.def	strtoul;	.scl	2;	.type	32;	.endef
	.def	strstr;	.scl	2;	.type	32;	.endef
	.def	fgets;	.scl	2;	.type	32;	.endef
	.def	strcat;	.scl	2;	.type	32;	.endef
	.def	memcpy;	.scl	2;	.type	32;	.endef
	.def	strtol;	.scl	2;	.type	32;	.endef
	.def	strncmp;	.scl	2;	.type	32;	.endef
	.def	setvbuf;	.scl	2;	.type	32;	.endef
	.def	atoi;	.scl	2;	.type	32;	.endef
