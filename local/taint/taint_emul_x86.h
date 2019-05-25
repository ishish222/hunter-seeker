/*
*   Tomasz Salacinski
*   Korrino
*   info@korrino.com
*/


/*

BYTE_t:
[00]

WORD_t:
[00][01] , 00 - MSB, 01 - LSB
e.g. 0x1234 -> [12][34]

DWORD_t:
[00][01][02][03], 00 - MSB, 03 - LSB
e.g. 0x12345678 -> [12][34][56][78]

Change of endiannes takes place when reading and writing to memory (to_mem, from_mem)

*/

#ifndef TAINT_EMUL_H
#define TAINT_EMUL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/queue.h>
#include <errno.h>
#include <types.h>
#include <breakpoint.h>
#include <plugin.h>

#define _GLIBCXX_USE_CXX11_ABI 1 

// compile-time options and parameters
#define ANALYZE_JUMPS 
#define ANALYZE_LOOPS 
//#define UNMATCHED_RET_INVALIDATES_STACK
//#define UNMATCHED_RET_CREATES_CALL
#define NOT_EMITTING_BLACKLISTED

#define NO_LOOP 0xffffffff

#define MOD_OFF     0x6
#define MOD_MASK    0x3
#define RM_OFF      0x0
#define RM_MASK     0x7
#define REG_OFF     0x3
#define REG_MASK    0x7

#define MOD_MEM_REG         0x0
#define MOD_MEM_REG_DISP_8  0x1
#define MOD_MEM_REG_DISP_32 0x2
#define MOD_REG             0x3

#define RM_EAX_BASED    0x0
#define RM_ECX_BASED    0x1
#define RM_EDX_BASED    0x2
#define RM_EBX_BASED    0x3
#define RM_UNKN         0x4
#define RM_EBP_BASED    0x5
#define RM_ESI_BASED    0x6
#define RM_EDI_BASED    0x7

#define REG_EAX_BASED   0x0
#define REG_ECX_BASED   0x1
#define REG_EDX_BASED   0x2
#define REG_EBX_BASED   0x3
#define REG_ESP_BASED   0x4
#define REG_EBP_BASED   0x5
#define REG_ESI_BASED   0x6
#define REG_EDI_BASED   0x7

// SIB byte fields
#define SIB_BASE_OFF   0x0
#define SIB_BASE_MASK  0x7
#define SIB_IDX_OFF    0x3
#define SIB_IDX_MASK   0x7
#define SIB_SS_OFF     0x6
#define SIB_SS_MASK    0x3

// SIB bases
#define SIB_BASE_EAX  0x0
#define SIB_BASE_ECX  0x1
#define SIB_BASE_EDX  0x2
#define SIB_BASE_EBX  0x3
#define SIB_BASE_ESP  0x4
#define SIB_BASE_UNKN 0x5
#define SIB_BASE_ESI  0x6
#define SIB_BASE_EDI  0x7

// SIB scale
#define SIB_SCALE_1 0x0
#define SIB_SCALE_2 0x1
#define SIB_SCALE_4 0x2
#define SIB_SCALE_8 0x3

// SIB indexes
#define SIB_INDEX_EAX_BASED 0x0
#define SIB_INDEX_ECX_BASED 0x1
#define SIB_INDEX_EDX_BASED 0x2
#define SIB_INDEX_EBX_BASED 0x3
#define SIB_INDEX_UNKN      0x4
#define SIB_INDEX_EBP_BASED 0x5
#define SIB_INDEX_ESI_BASED 0x6
#define SIB_INDEX_EDI_BASED 0x7

// offsets of register data in BYTE_t array
#define EAX     0x00
#define EBX     0x04
#define ECX     0x08
#define EDX     0x0c
#define ESI     0x10
#define EDI     0x14
#define EBP     0x18
#define ESP     0x1c
#define EFLAGS  0x20
#define EIP     0x24

//add 16 bit, 8bit offsets, other regs

#define AX     0x02
#define BX     0x06
#define CX     0x0a
#define DX     0x0e
#define SI     0x12
#define DI     0x16
#define BP     0x1a
#define SP     0x1e
#define FLAGS  0x22
#define IP     0x26

#define AL     0x03
#define BL     0x07
#define CL     0x0b
#define DL     0x0f
#define AH     0x02
#define BH     0x06
#define CH     0x0a
#define DH     0x0e

// flag masks

#define EFLAGS_CF 0x1
#define EFLAGS_PF 0x4
#define EFLAGS_AF 0x10
#define EFLAGS_ZF 0x20
#define EFLAGS_SF 0x40
#define EFLAGS_TF 0x80
#define EFLAGS_IF 0x200
#define EFLAGS_DF 0x400
#define EFLAGS_OF 0x800

#define EFLAGS_CLEAR_CF 0xFFFFFFFE
#define EFLAGS_CLEAR_PF 0xFFFFFFFD
#define EFLAGS_CLEAR_AF 0xFFFFFFF7
#define EFLAGS_CLEAR_ZF 0xFFFFFFDF
#define EFLAGS_CLEAR_SF 0xFFFFFFBF
#define EFLAGS_CLEAR_TF 0xFFFFFF7F
#define EFLAGS_CLEAR_IF 0xFFFFFEFF
#define EFLAGS_CLEAR_DF 0xFFFFFDFF
#define EFLAGS_CLEAR_OF 0xFFFFFBFF

/* the rest is unnecesarry at the moment */

// decoding modes

# define MODE_32        0x0
# define MODE_16        0x1
# define MODE_8         0x2
# define MODE_UNDEFINED 0x3

// segment regs

#define SEG_GS 0x0
#define SEG_FS 0x1
#define SEG_ES 0x2
#define SEG_DS 0x3
#define SEG_CS 0x4
#define SEG_SS 0x5

// legacy instruction prefixes
#define PREFIX_LOCK                     0x1    // 0xf0
#define PREFIX_REPNE                    0x2    // 0xf2
#define PREFIX_REP                      0x4    // 0xf3
#define PREFIX_CS_OVERRIDE              0x8    // 0x2e, also branch not taken
#define PREFIX_SS_OVERRIDE              0x10   // 0x36
#define PREFIX_DS_OVERRIDE              0x20   // 0x3e, also branch taken
#define PREFIX_ES_OVERRIDE              0x40   // 0x26
#define PREFIX_FS_OVERRIDE              0x80   // 0x64
#define PREFIX_GS_OVERRIDE              0x100  // 0x65
#define PREFIX_OPER_SIZE_OVERRIDE       0x200  // 0x66
#define PREFIX_ADDR_SIZE_OVERRIDE       0x400  // 0x67

#define MEM_SIZE    0x100000000
#define BUFF_SIZE   0x500000
#define REG_SIZE    0x750

/* modrm target & sources */

// regions
#define MODRM_REG   0x1
#define MODRM_MEM   0x2

// sizes
#define MODRM_SIZE_8     0x1
#define MODRM_SIZE_16    0x2
#define MODRM_SIZE_32    0x3
#define MODRM_SIZE_64    0x4

/* options */
#define OPTION_ANALYZE_JUMPS                    0x1
#define OPTION_ANALYZE_LOOPS                    0x2
#define OPTION_UNMATCHED_RET_INVALIDATES_STACK  0x4
#define OPTION_UNMATCHED_RET_CREATES_CALL       0x8
#define OPTION_NOT_EMITTING_BLACKLISTED         0x10
#define OPTION_VERIFY_ROP_RETS                  0x20
#define OPTION_VERIFY_SEG_SEC                   0x40
#define OPTION_ANALYZE_WANTED_IN_SYMBOLS        0x80
#define OPTION_COUNT_INSTRUCTIONS               0x100
#define OPTION_HANDLE_BREAKPOINTS               0x200
#define OPTION_VERIFY_OOB                       0x400

/* dont options */
#define OPTION_DONT_ANALYZE_JUMPS                    0xFFFFFFFE
#define OPTION_DONT_ANALYZE_LOOPS                    0xFFFFFFFD
#define OPTION_DONT_UNMATCHED_RET_INVALIDATES_STACK  0xFFFFFFFB
#define OPTION_DONT_UNMATCHED_RET_CREATES_CALL       0xFFFFFFF7
#define OPTION_DONT_NOT_EMITTING_BLACKLISTED         0xFFFFFFEF
#define OPTION_DONT_VERIFY_ROP_RETS                  0xFFFFFFDF
#define OPTION_DONT_VERIFY_SEG_SEC                   0xFFFFFFBF
#define OPTION_DONT_ANALYZE_WANTED_IN_SYMBOLS        0xFFFFFF7F
#define OPTION_DONT_COUNT_INSTRUCTIONS               0xFFFFFEFF
#define OPTION_DONT_HANDLE_BREAKPOINTS               0xFFFFFDFF
#define OPTION_DONT_VERIFY_OOB                       0xFFFFFBFF

/* jumping codes */

#define JMP_CODE_JB_JC_JNAE     0x0
#define JMP_CODE_JAE_JNB_JNC    0x1
#define JMP_CODE_JE_JZ          0x2
#define JMP_CODE_JNE_JNZ        0x3
#define JMP_CODE_JBE_JNA        0x4
#define JMP_CODE_JA_JNBE        0x5
#define JMP_CODE_JS             0x6
#define JMP_CODE_JNS            0x7
#define JMP_CODE_JP_JPE         0x8
#define JMP_CODE_JNP_JPO        0x9
#define JMP_CODE_JL_JNGE        0xa
#define JMP_CODE_JGE_JNL        0xb
#define JMP_CODE_JLE_JNG        0xc
#define JMP_CODE_JG_JNLE        0xd
#define JMP_CODE_RM             0xe
#define JMP_CODE_JXX            0xf

typedef struct modrm_op_ready_32_
{
    OFFSET reg;
    OFFSET rm;
    char type;
} modrm_op_ready_32;

typedef struct modrm_ptr_
{
    OFFSET offset;
    BYTE region;
    BYTE size;
} modrm_ptr;

/* structures for tracing taint propagation */

typedef struct TAINTED_
{
    BYTE_t* tainted;
    struct TAINTED_* next;
} TAINTED;

typedef struct PROPAGATION_ELEM_
{
    unsigned cause_id;
    struct PROPAGATION_ELEM_* next;
} PROPAGATION_ELEM;

typedef struct PROPAGATION_
{
    OFFSET instruction;
    unsigned instr_count;
    TAINTED* first_op;    
    unsigned elem_count;
    PROPAGATION_ELEM* causes;
    BYTE_t* result[0x4];
} PROPAGATION;

#define BP_MODE_READ    0x1
#define BP_MODE_WRITE   0x2
#define BP_MODE_EXECUTE 0x4

typedef struct REGION_
{
    OFFSET off;
    OFFSET size;
} REGION;


typedef struct _CONTEXT_INFO
{
    DWORD tid;
    BYTE_t registers[REG_SIZE];
    OFFSET seg_map[0x6];

    /* to tutaj czy do pluginu? */
    OFFSET source;
    OFFSET target;
    OFFSET next;
    OFFSET last_eip;
} CONTEXT_INFO;


/* tha actual class */
class taint_x86
{
    public:
    FILE* dump_file;
    FILE* instr_file;
    FILE* mod_file;

    DWORD start_addr;
    DWORD end_addr;
    OFFSET start_instr;
    OFFSET instr_limit;

    DWORD started;
    DWORD finished;
    DWORD aborted;

    /* memory parameters */
    BYTE_t* memory;
    DWORD mem_length;

    /* analysis options */
    DWORD options;

    /* identifying invelid bytes */
    BYTE_t invalid_byte;
    WORD_t invalid_word;
    DWORD_t invalid_dword;

    /* handling prefixes */
    int a_is_prefix(BYTE_t*);
    int current_prefixes;
    BYTE current_prefix_length;
    
    /* counters */
    BYTE_t* last_instr_byte;
    BYTE_t* current_instr_byte;
    OFFSET current_eip;
    DWORD current_instr_is_jump;
    OFFSET last_eip;
    unsigned long long  last_instr_count;
    unsigned long long  current_instr_count;
    BYTE current_instr_length;

    /* consistency checking */
    char last_inconsistent;
    int check_thread(CONTEXT_OUT);

    /* breakpoints & watchpoints */
    DWORD bp_hit;
    DWORD rw_bp;
    BREAKPOINT bps[MAX_BREAKPOINTS];
    BREAKPOINT bps_t[MAX_BREAKPOINTS];
    TRACE_WATCHPOINT wps[MAX_BREAKPOINTS];   
    unsigned bpt_count;
    unsigned bpt_t_count;
    unsigned wpt_count;
    int add_breakpoint(BREAKPOINT);
    int add_taint_breakpoint(BREAKPOINT);
    int add_trace_watchpoint(TRACE_WATCHPOINT);
    int check_execution_bps();
    int update_watchpoints(DWORD);

    /* securing memory areas */
    REGION  security_layer[MAX_SECURITY_LAYERS];
    unsigned security_layer_count;
    int verify_seg_sec(OFFSET);
 
    /* handling exceptions */
    EXCEPTION_INFO exceptions[MAX_EXCEPTIONS_COUNT];
    DWORD exceptions_count;
    int add_exception(EXCEPTION_INFO);


    int modrm_table_32[0x100];

    typedef int (taint_x86::*instruction_routine)(BYTE_t* args);
    instruction_routine instructions_32[0x100];
    instruction_routine instructions_32_extended[0x100];

    // instruction routines
    int r_noop(BYTE_t*);
    int r_noop_un(BYTE_t*);
    int r_jxx(BYTE_t*);
    int r_jb_jc_jnae(BYTE_t*);
    int r_jae_jnb_jnc(BYTE_t*);
    int r_je_jz(BYTE_t*);
    int r_jne_jnz(BYTE_t*);
    int r_jbe_jna(BYTE_t*);
    int r_ja_jnbe(BYTE_t*);
    int r_js(BYTE_t*);
    int r_jns(BYTE_t*);
    int r_jp_jpe(BYTE_t*);
    int r_jnp_jpo(BYTE_t*);
    int r_jl_jnge(BYTE_t*);
    int r_jge_jnl(BYTE_t*);
    int r_jle_jng(BYTE_t*);
    int r_jg_jnle(BYTE_t*);
    int r_push_ax_eax(BYTE_t*);
    int r_push_cx_ecx(BYTE_t*);
    int r_push_dx_edx(BYTE_t*);
    int r_push_bx_ebx(BYTE_t*);
    int r_push_sp_esp(BYTE_t*);
    int r_push_bp_ebp(BYTE_t*);
    int r_push_si_esi(BYTE_t*);
    int r_push_di_edi(BYTE_t*);
    int r_pop_ax_eax(BYTE_t*);
    int r_pop_cx_ecx(BYTE_t*);
    int r_pop_dx_edx(BYTE_t*);
    int r_pop_bx_ebx(BYTE_t*);
    int r_pop_sp_esp(BYTE_t*);
    int r_pop_bp_ebp(BYTE_t*);
    int r_pop_si_esi(BYTE_t*);
    int r_pop_di_edi(BYTE_t*);
    int r_pusha(BYTE_t*);
    int r_popa(BYTE_t*);
    int r_pushf(BYTE_t*);
    int r_popf(BYTE_t*);
    int r_push_imm_8(BYTE_t*);
    int r_push_imm_16_32(BYTE_t*);
    int r_push_rm_16_32(BYTE_t*);

    int r_lea(BYTE_t*);

    int r_xchg_r_rm_8(BYTE_t*);
    int r_xchg_r_rm_16_32(BYTE_t*);

//    int r_xchg_ax_eax_r_16_32(BYTE_t*);
    int r_xchg_ax_eax_ax_eax(BYTE_t*);
    int r_xchg_ax_eax_cx_ecx(BYTE_t*);
    int r_xchg_ax_eax_dx_edx(BYTE_t*);
    int r_xchg_ax_eax_bx_ebx(BYTE_t*);
    int r_xchg_ax_eax_sp_esp(BYTE_t*);
    int r_xchg_ax_eax_bp_ebp(BYTE_t*);
    int r_xchg_ax_eax_si_esi(BYTE_t*);
    int r_xchg_ax_eax_di_edi(BYTE_t*);
    int r_cdq(BYTE_t*);

    int r_sahf(BYTE_t*);
    int r_lahf(BYTE_t*);

    int r_mov_r_rm_16_32(BYTE_t*);
    int r_mov_r_rm_8(BYTE_t*);
    int r_mov_rm_r_16_32(BYTE_t*);
    int r_mov_rm_r_8(BYTE_t*);
    int r_mov_rm_imm_8(BYTE_t*);
    int r_mov_rm_imm_16_32(BYTE_t*);

    int r_mov_al_imm8(BYTE_t*);
    int r_mov_cl_imm8(BYTE_t*);
    int r_mov_dl_imm8(BYTE_t*);
    int r_mov_bl_imm8(BYTE_t*);
    int r_mov_ah_imm8(BYTE_t*);
    int r_mov_ch_imm8(BYTE_t*);
    int r_mov_dh_imm8(BYTE_t*);
    int r_mov_bh_imm8(BYTE_t*);

    int r_mov_eax_imm_16_32(BYTE_t*);    
    int r_mov_ecx_imm_16_32(BYTE_t*);
    int r_mov_edx_imm_16_32(BYTE_t*);
    int r_mov_ebx_imm_16_32(BYTE_t*);
    int r_mov_esp_imm_16_32(BYTE_t*);
    int r_mov_ebp_imm_16_32(BYTE_t*);
    int r_mov_esi_imm_16_32(BYTE_t*);
    int r_mov_edi_imm_16_32(BYTE_t*);

        
    int r_mov_al_moffset_8(BYTE_t*);
    int r_mov_ax_eax_moffset_16_32(BYTE_t*);
    int r_mov_moffset_8_al(BYTE_t*);
    int r_mov_moffset_16_32_ax_eax(BYTE_t*);
    int r_movs_moffset_16_32(BYTE_t*);
    int r_movs_moffset_8(BYTE_t*);

    int r_cmps_m_8(BYTE_t*);
    int r_cmps_m_16_32(BYTE_t*);

    int r_inc_ax_eax(BYTE_t*);
    int r_inc_cx_ecx(BYTE_t*);
    int r_inc_dx_edx(BYTE_t*);
    int r_inc_bx_ebx(BYTE_t*);
    int r_inc_sp_esp(BYTE_t*);
    int r_inc_bp_ebp(BYTE_t*);
    int r_inc_si_esi(BYTE_t*);
    int r_inc_di_edi(BYTE_t*);
    int r_inc_rm_8(BYTE_t*);
    int r_inc_rm_16_32(BYTE_t*);

    int r_dec_ax_eax(BYTE_t*);
    int r_dec_cx_ecx(BYTE_t*);
    int r_dec_dx_edx(BYTE_t*);
    int r_dec_bx_ebx(BYTE_t*);
    int r_dec_sp_esp(BYTE_t*);
    int r_dec_bp_ebp(BYTE_t*);
    int r_dec_si_esi(BYTE_t*);
    int r_dec_di_edi(BYTE_t*);
    int r_dec_rm_8(BYTE_t*);
    int r_dec_rm_16_32(BYTE_t*);

    // add, sub, mul & div
    // add    

    int r_add_rm_imm_16_32(BYTE_t*);
    int r_add_rm_16_32_imm_8(BYTE_t*);
    int r_add_al_imm_8(BYTE_t*);
    int r_add_ax_eax_imm_16_32(BYTE_t*);
    int r_add_rm_imm_8(BYTE_t*);
    int r_add_rm_r_8(BYTE_t*);
    int r_add_rm_r_16_32(BYTE_t*);
    int r_add_r_rm_8(BYTE_t*);
    int r_add_r_rm_16_32(BYTE_t*);

    // sub
    int r_sub_rm_imm_16_32(BYTE_t*);
    int r_sub_rm_16_32_imm_8(BYTE_t*);
    int r_sub_al_imm_8(BYTE_t*);
    int r_sub_ax_eax_imm_16_32(BYTE_t*);
    int r_sub_rm_imm8(BYTE_t*);
    int r_sub_rm_r_8(BYTE_t*);
    int r_sub_rm_r_16_32(BYTE_t*);
    int r_sub_r_rm_8(BYTE_t*);
    int r_sub_r_rm_16_32(BYTE_t*);

    // sbb
    int r_sbb_rm_imm_16_32(BYTE_t*);
    int r_sbb_rm_16_32_imm_8(BYTE_t*);
    int r_sbb_al_imm_8(BYTE_t*);
    int r_sbb_ax_eax_imm_16_32(BYTE_t*);
    int r_sbb_rm_imm8(BYTE_t*);
    int r_sbb_rm_r_8(BYTE_t*);
    int r_sbb_rm_r_16_32(BYTE_t*);
    int r_sbb_r_rm_8(BYTE_t*);
    int r_sbb_r_rm_16_32(BYTE_t*);

    // mul
    int r_mul_rm_8(BYTE_t*);
    int r_mul_rm_16_32(BYTE_t*);

    // imul
    int r_imul_rm_8(BYTE_t*);
    int r_imul_rm_16_32(BYTE_t*);
    int r_imul_r_rm_imm_16_32(BYTE_t*);
    int r_imul_r_rm_16_32_imm_8(BYTE_t*);

    // div
    int r_div_rm_8(BYTE_t*);
    int r_div_rm_16_32(BYTE_t*);

    // div
    int r_idiv_rm_8(BYTE_t*);
    int r_idiv_rm_16_32(BYTE_t*);

    // xor ops
    int r_xor_r_rm(BYTE_t*);
    int r_xor_rm_r_8(BYTE_t*);
    int r_xor_rm_r_16_32(BYTE_t*);
    int r_xor_r_rm_8(BYTE_t*);
    int r_xor_al_imm_8(BYTE_t*);
    int r_xor_ax_eax_imm_16_32(BYTE_t*);
    int r_xor_rm_imm_8(BYTE_t*);
    int r_xor_rm_imm_16_32(BYTE_t*);
    int r_xor_rm_16_32_imm_8(BYTE_t*);

    // sal, sar, shl, shr, after reduction
    int r_sal_shl_rm_imm_8(BYTE_t*); 
    int r_shr_rm_imm_8(BYTE_t*); 
    int r_sar_rm_imm_8(BYTE_t*); 
    int r_sal_shl_rm_16_32_imm_8(BYTE_t*); 
    int r_shr_rm_32_imm_8(BYTE_t*); 
    int r_sar_rm_16_32_imm_8(BYTE_t*); 
    int r_sal_shl_rm_8_1(BYTE_t*); 
    int r_shr_rm_8_1(BYTE_t*); 
    int r_sar_rm_8_1(BYTE_t*); 
    int r_sal_shl_rm_16_32_1(BYTE_t*); 
    int r_shr_rm_16_32_1(BYTE_t*); 
    int r_sar_rm_16_32_1(BYTE_t*); 
    int r_sal_shl_rm_8_cl(BYTE_t*);  
    int r_shr_rm_8_cl(BYTE_t*); 
    int r_sar_rm_8_cl(BYTE_t*); 
    int r_sal_shl_rm_16_32_cl(BYTE_t*);  
    int r_shr_rm_16_32_cl(BYTE_t*); 
    int r_sar_rm_16_32_cl(BYTE_t*); 

    // rol, ror, rcl, rcr
    int r_rcl_rm_8_1(BYTE_t*);
    int r_rcr_rm_8_1(BYTE_t*);
    int r_rol_rm_8_1(BYTE_t*);
    int r_ror_rm_8_1(BYTE_t*);
    int r_rcl_rm_16_32_1(BYTE_t*);
    int r_rcr_rm_16_32_1(BYTE_t*);
    int r_rol_rm_16_32_1(BYTE_t*);
    int r_ror_rm_16_32_1(BYTE_t*);
    int r_rol_rm_8_cl(BYTE_t*);
    int r_ror_rm_8_cl(BYTE_t*);
    int r_rcl_rm_8_cl(BYTE_t*);
    int r_rcr_rm_8_cl(BYTE_t*);
    int r_rcl_rm_16_32_cl(BYTE_t*);
    int r_rcr_rm_16_32_cl(BYTE_t*);
    int r_rol_rm_16_32_cl(BYTE_t*);
    int r_ror_rm_16_32_cl(BYTE_t*);
    int r_rcl_rm_imm_8(BYTE_t*);
    int r_rcr_rm_imm_8(BYTE_t*);
    int r_rol_rm_imm_8(BYTE_t*);
    int r_ror_rm_imm_8(BYTE_t*);
    int r_rcl_rm_16_32_imm_8(BYTE_t*);
    int r_rcr_rm_16_32_imm_8(BYTE_t*);
    int r_rol_rm_16_32_imm_8(BYTE_t*);
    int r_ror_rm_16_32_imm_8(BYTE_t*);

    // and, or
    int r_and_rm_imm_16_32(BYTE_t*);
    int r_and_rm_16_32_imm_8(BYTE_t*);
    int r_and_al_imm_8(BYTE_t*);
    int r_and_ax_eax_imm_16_32(BYTE_t*);
    int r_and_rm_imm8(BYTE_t*);
    int r_and_rm_r_8(BYTE_t*);
    int r_and_rm_r_16_32(BYTE_t*);
    int r_and_r_rm_8(BYTE_t*);
    int r_and_r_rm_16_32(BYTE_t*);

    int r_or_rm_16_32_imm_8(BYTE_t*);
    int r_or_rm_imm_8(BYTE_t*);
    int r_or_rm_imm_16_32(BYTE_t*);
    int r_or_rm_r_8(BYTE_t*);
    int r_or_rm_r_16_32(BYTE_t*);
    int r_or_r_rm_8(BYTE_t*);
    int r_or_r_rm_16_32(BYTE_t*);
    int r_or_al_imm_8(BYTE_t*);
    int r_or_ax_eax_imm_16_32(BYTE_t*);

    // adc
    int r_adc_rm_r_8(BYTE_t*);          
    int r_adc_rm_r_16_32(BYTE_t*);      
    int r_adc_r_rm_8(BYTE_t*);          
    int r_adc_r_rm_16_32(BYTE_t*);      
    int r_adc_al_imm_8(BYTE_t*);        
    int r_adc_ax_eax_imm_16_32(BYTE_t*);
    int r_adc_rm_imm_8(BYTE_t*);
    int r_adc_rm_imm_16_32(BYTE_t*);
    int r_adc_rm_16_32_imm_8(BYTE_t*);

    // neg & not
    int r_neg_rm_8(BYTE_t*);
    int r_neg_rm_16_32(BYTE_t*);
    int r_not_rm_8(BYTE_t*);
    int r_not_rm_16_32(BYTE_t*);

    // string operations
    int r_stosd_8(BYTE_t*);
    int r_stosd_16_32(BYTE_t*);
    int r_lods_8(BYTE_t*);
    int r_lods_16_32(BYTE_t*);
    int r_scas_m_8(BYTE_t*);
    int r_scas_m_16_32(BYTE_t*);

    //cmp
    int r_cmp_rm_r_8(BYTE_t*);
    int r_cmp_rm_r_16_32(BYTE_t*);
    int r_cmp_r_rm_8(BYTE_t*);
    int r_cmp_r_rm_16_32(BYTE_t*);
    int r_cmp_al_imm_8(BYTE_t*);
    int r_cmp_ax_eax_imm_16_32(BYTE_t*);

    //connected
    int r_cmp_rm_imm_8(BYTE_t*);
    int r_cmp_rm_imm_16_32(BYTE_t*);
    int r_cmp_rm_16_32_imm_8(BYTE_t*);

    // ret
    int r_retn(BYTE_t*);
    int r_ret(BYTE_t*);
    int r_leave(BYTE_t*);

    // call
    int r_call_rel(BYTE_t*);
    int r_call_abs_near(BYTE_t*);
    int r_call_abs_far(BYTE_t*);
    int r_jmp_rel_16_32(BYTE_t*);
    int r_jmp_rel_8(BYTE_t*);
    int r_jmp_rm_16_32(BYTE_t*);

    // sysenter
    int r_sysenter(BYTE_t*);

    // shld/shrd
    int r_shld_r_rm_16_32_imm_8(BYTE_t*);
    int r_shld_r_rm_16_32_cl(BYTE_t*);
    int r_shrd_r_rm_16_32_imm_8(BYTE_t*);
    int r_shrd_r_rm_16_32_cl(BYTE_t*);

    // set/clear
    int r_clc(BYTE_t*);
    int r_stc(BYTE_t*);
    int r_cli(BYTE_t*);
    int r_sti(BYTE_t*);
    int r_cld(BYTE_t*);
    int r_std(BYTE_t*);

    // prefixes
    int r_lock(BYTE_t*);
    int r_repne(BYTE_t*);
    int r_rep(BYTE_t*);
    int r_cs_override(BYTE_t*);
    int r_ss_override(BYTE_t*);
    int r_ds_override(BYTE_t*);
    int r_es_override(BYTE_t*);
    int r_fs_override(BYTE_t*);
    int r_gs_override(BYTE_t*);
    int r_oper_size_override(BYTE_t*);
    int r_addr_size_override(BYTE_t*);

    // need decoding
    int r_decode_execute_80(BYTE_t*);
    int r_decode_execute_81(BYTE_t*);
    int r_decode_execute_83(BYTE_t*);
    int r_decode_execute_c6(BYTE_t*);
    int r_decode_execute_c7(BYTE_t*);
    int r_decode_execute_d0(BYTE_t*);
    int r_decode_execute_d1(BYTE_t*);
    int r_decode_execute_d2(BYTE_t*);
    int r_decode_execute_d3(BYTE_t*);
    int r_decode_execute_c0(BYTE_t*);
    int r_decode_execute_c1(BYTE_t*);
    int r_decode_execute_f6(BYTE_t*);
    int r_decode_execute_f7(BYTE_t*);
    int r_decode_execute_fe(BYTE_t*);
    int r_decode_execute_ff(BYTE_t*);
   
    int r_decode_execute_0fba(BYTE_t*);

    // extended
    int r_cmpxchg_rm_r_8(BYTE_t*);
    int r_cmpxchg_rm_r_16_32(BYTE_t*);
    int r_btr_rm_r_16_32(BYTE_t*);
    int r_btr_rm_16_32_imm_8(BYTE_t*);
    int r_movzx_r_16_32_rm_8(BYTE_t*);
    int r_movzx_r_32_rm_16(BYTE_t*);
    int r_bsf_r_rm_16_32(BYTE_t*);
    int r_movsx_r_16_32_rm_8(BYTE_t*);
    int r_movsx_r_32_rm_16(BYTE_t*);
    int r_xadd_rm_r_8(BYTE_t*);
    int r_xadd_rm_r_16_32(BYTE_t*);
    int r_imul_r_rm_16_32(BYTE_t*);
 
    // process extended operands
    DWORD extended;
    int r_extend(BYTE_t*);

    // auxilliary
    OFFSET a_calculate_base(LDT_ENTRY);
    int a_check_segment_override();
    int a_push_8(BYTE_t);
    int a_push_16(WORD_t);
    int a_push_32(DWORD_t);

    BYTE_t a_pop_8();
    WORD_t a_pop_16();
    DWORD_t a_pop_32();
    DWORD_t a_peek_32();
    int a_calculate_instr_length(BYTE_t*);
    int a_decode_modrm(BYTE_t*, modrm_ptr*, modrm_ptr*, int, int);
    OFFSET a_decode_sib(BYTE_t*);
    OFFSET a_decode_sib_mod(BYTE_t*, BYTE);
    OFFSET a_offset_recalculate_16(OFFSET);
    OFFSET a_offset_recalculate_8(OFFSET);
    int verify_oob_offset(OFFSET, OFFSET);

    int a_check_sign(BYTE);
    int a_check_sign(WORD);
    int a_check_sign(DWORD);
    int a_check_cf();
    int a_check_pf();
    int a_check_af();
    int a_check_zf();
    int a_check_sf();
    int a_check_if();
    int a_check_df();
    int a_check_of();
    int a_set_cf();
    int a_set_pf();
    int a_set_af();
    int a_set_zf();
    int a_set_sf();
    int a_set_if();
    int a_set_df();
    int a_set_of();
    int a_clear_cf();
    int a_clear_pf();
    int a_clear_af();
    int a_clear_zf();
    int a_clear_sf();
    int a_clear_if();
    int a_clear_df();
    int a_clear_of();

    QWORD a_f_mul_32(DWORD_t, DWORD_t);
    QWORD a_f_imul_32(DWORD_t, DWORD_t);
    DWORD a_f_mul_16(WORD_t, WORD_t);
    DWORD a_f_imul_16(WORD_t, WORD_t);
    WORD  a_f_mul_8(BYTE_t, BYTE_t);
    WORD  a_f_imul_8(BYTE_t, BYTE_t);

    BYTE a_div_f_8(WORD_t, BYTE);
    BYTE a_mod_f_8(WORD_t, BYTE);

    DWORD_t a_mul_32(DWORD_t, DWORD_t);
    DWORD_t a_imul_32(DWORD_t, DWORD_t);
    DWORD_t a_div_32(DWORD_t, DWORD_t);
    DWORD_t a_idiv_32(DWORD_t, DWORD_t);
    DWORD_t a_mod_32(DWORD_t, DWORD_t);
    DWORD_t a_imod_32(DWORD_t, DWORD_t);
    WORD_t a_mul_16(WORD_t, WORD_t);
    WORD_t a_imul_16(WORD_t, WORD_t);
    WORD_t a_div_16(WORD_t, WORD_t);
    WORD_t a_idiv_16(WORD_t, WORD_t);
    WORD_t a_mod_16(WORD_t, WORD_t);
    WORD_t a_imod_16(WORD_t, WORD_t);
    BYTE_t a_mul_8(BYTE_t, BYTE_t);
    BYTE_t a_imul_8(BYTE_t, BYTE_t);
    BYTE_t a_div_8(BYTE_t, BYTE_t);
    BYTE_t a_idiv_8(BYTE_t, BYTE_t);
    BYTE_t a_mod_8(BYTE_t, BYTE_t);
    BYTE_t a_imod_8(BYTE_t, BYTE_t);

    DWORD_t a_mul_32(DWORD_t, DWORD);
    DWORD_t a_imul_32(DWORD_t, DWORD);
    DWORD_t a_div_32(DWORD_t, DWORD);
    DWORD_t a_idiv_32(DWORD_t, DWORD);
    DWORD_t a_mod_32(DWORD_t, DWORD);
    DWORD_t a_imod_32(DWORD_t, DWORD);

    int a_rol_32(DWORD_t&);
    int a_ror_32(DWORD_t&);
    int a_rcl_32(DWORD_t&);
    int a_rcr_32(DWORD_t&);

    WORD_t a_mul_16(WORD_t, WORD);
    WORD_t a_imul_16(WORD_t, WORD);
    WORD_t a_div_16(WORD_t, WORD);
    WORD_t a_idiv_16(WORD_t, WORD);
    WORD_t a_mod_16(WORD_t, WORD);
    WORD_t a_imod_16(WORD_t, WORD);

    int a_rol_16(WORD_t&);
    int a_ror_16(WORD_t&);
    int a_rcl_16(WORD_t&);
    int a_rcr_16(WORD_t&);

    BYTE_t a_mul_8(BYTE_t, BYTE);
    BYTE_t a_imul_8(BYTE_t, BYTE);
    BYTE_t a_div_8(BYTE_t, BYTE);
    BYTE_t a_idiv_8(BYTE_t, BYTE);
    BYTE_t a_mod_8(BYTE_t, BYTE);
    BYTE_t a_imod_8(BYTE_t, BYTE);

    int a_rol_8(BYTE_t&);
    int a_ror_8(BYTE_t&);
    int a_rcl_8(BYTE_t&);
    int a_rcr_8(BYTE_t&);

    /* thread-related functions */
    int already_added(DWORD);
    int add_thread(CONTEXT_OUT);
    int mod_thread(CONTEXT_OUT);
    int del_thread(DWORD);
    int del_thread_srsly(DWORD);
    CONTEXT_INFO* get_context_info(DWORD);

    /* I/O */
    int load_mem_from_file(char*);
    int load_instr_from_file(char*);
    int open_mod_file(char*);
    int close_files();

    // other
    DWORD le2dword(char*);

    int apply_memory(DWORD, DWORD);
    int apply_security(DWORD, DWORD);
    int register_syscall(DWORD, DWORD);
    void handle_sigsegv(int);
    void handle_sigint(int);

    // overriden for more effective debugging
    int d_print(int, const char*, ...);

    /* propagation */

    int print_taint_ops(unsigned);
    int print_taint_history(unsigned);
    int print_taint_history(BYTE_t*);
    int print_taint_history(BYTE_t*, OFFSET);
    int reg_propagation_op(BYTE_t*);
    int reg_propagation_op_r_8(OFFSET);
    int reg_propagation_op_r_16(OFFSET);
    int reg_propagation_op_r_32(OFFSET);
    int reg_propagation_op_m_8(OFFSET);
    int reg_propagation_op_m_16(OFFSET);
    int reg_propagation_op_m_32(OFFSET);
    int attach_current_propagation_r_8(OFFSET);
    int attach_current_propagation_r_16(OFFSET);
    int attach_current_propagation_r_32(OFFSET);
    int attach_current_propagation_m_8(OFFSET);
    int attach_current_propagation_m_16(OFFSET);
    int attach_current_propagation_m_32(OFFSET);

    /* 32 bit store / restore */
    void store_32(OFFSET, DWORD_t);
    void store_32(DWORD_t, DWORD_t);
    void restore_32(OFFSET, DWORD_t&);
    void restore_32(DWORD_t, DWORD_t&);

    /* 32 bit reg store / restore */
    void reg_store_32(OFFSET, DWORD_t);
    void reg_store_32(DWORD_t, DWORD_t);
    DWORD_t reg_restore_32(OFFSET);
    DWORD_t reg_restore_32(DWORD_t);
    void reg_store_32(OFFSET, DWORD_t, int);
    void reg_store_32(DWORD_t, DWORD_t, int);
    DWORD_t reg_restore_32(OFFSET, int);
    DWORD_t reg_restore_32(DWORD_t, int);

    /* 16 bit store / restore */
    void store_16(OFFSET, WORD_t);
    void store_16(DWORD_t, WORD_t);
    void restore_16(OFFSET, WORD_t&);
    void restore_16(DWORD_t, WORD_t&);
    
    /* 16 bit reg store / restore */
    void reg_store_16(OFFSET, WORD_t);
    void reg_store_16(DWORD_t, WORD_t);
    WORD_t reg_restore_16(OFFSET);
    WORD_t reg_restore_16(DWORD_t);
    void reg_store_16(OFFSET, WORD_t, int);
    void reg_store_16(DWORD_t, WORD_t, int);
    WORD_t reg_restore_16(OFFSET, int);
    WORD_t reg_restore_16(DWORD_t, int);

    /* 8 bit store / restore */
    void store_8(OFFSET, BYTE_t);
    void store_8(DWORD_t, BYTE_t);
    void restore_8(OFFSET, BYTE_t&);
    void restore_8(DWORD_t, BYTE_t&);

    /* 8 bit reg store / restore */
    void reg_store_8(OFFSET, BYTE_t);
    void reg_store_8(DWORD_t, BYTE_t);
    BYTE_t reg_restore_8(OFFSET);
    BYTE_t reg_restore_8(DWORD_t);
    void reg_store_8(OFFSET, BYTE_t, int);
    void reg_store_8(DWORD_t, BYTE_t, int);
    BYTE_t reg_restore_8(OFFSET, int);
    BYTE_t reg_restore_8(DWORD_t, int);

    /* !!! engine methods */
    int pre_execute_instruction(DWORD);
    int execute_instruction(DWORD, DWORD);
    int post_execute_instruction(DWORD);
    int execute_instruction_at_eip(DWORD);
    int execute_instruction_at_eip(DWORD, DWORD);

    int execute_instruction_32(DWORD eip, DWORD tid);           /* contains plugin callbacks */
    int execute_instruction_32_extended(DWORD eip, DWORD tid);  /* contains plugin callbacks */

    int verify_t_context(int);
    int decode_modrm_byte(BYTE_t*, DWORD_t*, DWORD_t*);
    int decode_modrm_byte(BYTE_t*, OFFSET*, OFFSET*);
    int start();
    int finish();

    /* plugins */
    class Plugin* plugin;
    //int register_plugin(Plugin*);

    /* printing methods */
    int print_err_all_contexts();
    int print_err_all_t_contexts();
    int print_err_all_stacks();
    int print_err_all_t_stacks();
    int print_err_context(int);
    int print_err_t_context(int);
    int print_err_stack(DWORD, DWORD);
    int print_err_t_stack(DWORD, DWORD);
    int print_context(int);
    int print_t_context(int);
    int print_context();
    int print_t_context();
    int print_all_contexts();
    int print_all_t_contexts();
    int print_stack(DWORD, int, DWORD);
    int print_t_stack(DWORD, int, DWORD);
    int print_stack(int, DWORD);
    int print_t_stack(int, DWORD);
    int print_all_stacks();
    int print_all_t_stacks();
    int print_err_mem(OFFSET, DWORD);
    int print_err_t_mem(OFFSET, DWORD);
    int print_mem(int, OFFSET, DWORD);
    int print_t_mem(int, OFFSET, DWORD);
    int print_security_layers(int);
    int print_security_layers(int, OFFSET);
    int print_bt_buffer(BYTE_t*, DWORD);
    int print_all_regs();

    /* handling context info */
    DWORD tids[MAX_THREAD_NUMBER];
    DWORD tid_count;
    DWORD cur_tid;
    CONTEXT_INFO* cur_info;
    CONTEXT_INFO* ctx_info;

    /* taint stuff */
    REGION* taints;
    unsigned taint_count;
    unsigned current_propagation_count;
    PROPAGATION* propagations;

    /* taint stuff - dumping taint transfer history */
    int add_taint(OFFSET start, UDWORD length);
    int write_history(FILE*);
    int dump_cmd(char*);
    int prompt_taint();
    TRACE_WATCHPOINT parse_trace_string(char* string);
    int query_history(TRACE_WATCHPOINT twp);

    taint_x86()
    {
        printf("Initializing taint emulator\n");

        // information on rm bytes for instructions 
        modrm_table_32[0x14] = 0x0;
        modrm_table_32[0x15] = 0x0;
        modrm_table_32[0x37] = 0x0;
        modrm_table_32[0x37] = 0x0;
       
        modrm_table_32[0x37] = 0x0;
        modrm_table_32[0x3f] = 0x0;
        modrm_table_32[0xd4] = 0x0;
        modrm_table_32[0xd5] = 0x0;
 
        memset(this->instructions_32, 0x0, sizeof(this->instructions_32));

        // taint tracing routines for instructions
        
        this->instructions_32[0x00] = &taint_x86::r_add_rm_r_8;                 // cf
        this->instructions_32[0x01] = &taint_x86::r_add_rm_r_16_32;             // cf
        this->instructions_32[0x02] = &taint_x86::r_add_r_rm_8;                 // cf
        this->instructions_32[0x03] = &taint_x86::r_add_r_rm_16_32;             // cf
        this->instructions_32[0x04] = &taint_x86::r_add_al_imm_8;               // cf
        this->instructions_32[0x05] = &taint_x86::r_add_ax_eax_imm_16_32;       // cf

        this->instructions_32[0x08] = &taint_x86::r_or_rm_r_8;                  // cf
        this->instructions_32[0x09] = &taint_x86::r_or_rm_r_16_32;              // cf
        this->instructions_32[0x0a] = &taint_x86::r_or_r_rm_8;                  // cf
        this->instructions_32[0x0b] = &taint_x86::r_or_r_rm_16_32;              // cf
        this->instructions_32[0x0c] = &taint_x86::r_or_al_imm_8;                // cf
        this->instructions_32[0x0d] = &taint_x86::r_or_ax_eax_imm_16_32;        // cf
        this->instructions_32[0x0f] = &taint_x86::r_extend;                     // cf
        this->instructions_32[0x10] = &taint_x86::r_adc_rm_r_8;                 // cf
        this->instructions_32[0x11] = &taint_x86::r_adc_rm_r_16_32;             // cf
        this->instructions_32[0x12] = &taint_x86::r_adc_r_rm_8;                 // cf
        this->instructions_32[0x13] = &taint_x86::r_adc_r_rm_16_32;             // cf
        this->instructions_32[0x14] = &taint_x86::r_adc_al_imm_8;               // cf
        this->instructions_32[0x15] = &taint_x86::r_adc_ax_eax_imm_16_32;       // cf

        this->instructions_32[0x18] = &taint_x86::r_sbb_rm_r_8;                 // cf
        this->instructions_32[0x19] = &taint_x86::r_sbb_rm_r_16_32;             // cf
        this->instructions_32[0x1a] = &taint_x86::r_sbb_r_rm_8;                 // cf
        this->instructions_32[0x1b] = &taint_x86::r_sbb_r_rm_16_32;             // cf
        this->instructions_32[0x1c] = &taint_x86::r_sbb_al_imm_8;               // cf
        this->instructions_32[0x1d] = &taint_x86::r_sbb_ax_eax_imm_16_32;       // cf

        this->instructions_32[0x20] = &taint_x86::r_and_rm_r_8;                 // cf
        this->instructions_32[0x21] = &taint_x86::r_and_rm_r_16_32;             // cf
        this->instructions_32[0x22] = &taint_x86::r_and_r_rm_8;                 // cf
        this->instructions_32[0x23] = &taint_x86::r_and_r_rm_16_32;             // cf
        this->instructions_32[0x24] = &taint_x86::r_and_al_imm_8;               // cf
        this->instructions_32[0x25] = &taint_x86::r_and_ax_eax_imm_16_32;       // cf
        this->instructions_32[0x26] = &taint_x86::r_es_override;                // cf

        this->instructions_32[0x28] = &taint_x86::r_sub_rm_r_8;                 // cf
        this->instructions_32[0x29] = &taint_x86::r_sub_rm_r_16_32;             // cf
        this->instructions_32[0x2a] = &taint_x86::r_sub_r_rm_8;                 // cf
        this->instructions_32[0x2b] = &taint_x86::r_sub_r_rm_16_32;             // cf
        this->instructions_32[0x2c] = &taint_x86::r_sub_al_imm_8;               // cf
        this->instructions_32[0x2d] = &taint_x86::r_sub_ax_eax_imm_16_32;       // cf
        this->instructions_32[0x2e] = &taint_x86::r_cs_override;                // cf
                                
        this->instructions_32[0x30] = &taint_x86::r_xor_rm_r_8;                 // cf
        this->instructions_32[0x31] = &taint_x86::r_xor_rm_r_16_32;             // cf
        this->instructions_32[0x32] = &taint_x86::r_xor_r_rm_8;                 // cf
        this->instructions_32[0x33] = &taint_x86::r_xor_r_rm;                   // cf
        this->instructions_32[0x34] = &taint_x86::r_xor_al_imm_8;               // cf
        this->instructions_32[0x35] = &taint_x86::r_xor_ax_eax_imm_16_32;       // cf
        this->instructions_32[0x36] = &taint_x86::r_ss_override;                // cf

        this->instructions_32[0x38] = &taint_x86::r_cmp_rm_r_8;
        this->instructions_32[0x39] = &taint_x86::r_cmp_rm_r_16_32;
        this->instructions_32[0x3a] = &taint_x86::r_cmp_r_rm_8;
        this->instructions_32[0x3b] = &taint_x86::r_cmp_r_rm_16_32;
        this->instructions_32[0x3c] = &taint_x86::r_cmp_al_imm_8;
        this->instructions_32[0x3d] = &taint_x86::r_cmp_ax_eax_imm_16_32;
        this->instructions_32[0x3e] = &taint_x86::r_ds_override;                // cf
        
        this->instructions_32[0x40] = &taint_x86::r_inc_ax_eax;                 // cf
        this->instructions_32[0x41] = &taint_x86::r_inc_cx_ecx;                 // cf
        this->instructions_32[0x42] = &taint_x86::r_inc_dx_edx;                 // cf
        this->instructions_32[0x43] = &taint_x86::r_inc_bx_ebx;                 // cf
        this->instructions_32[0x44] = &taint_x86::r_inc_sp_esp;                 // cf
        this->instructions_32[0x45] = &taint_x86::r_inc_bp_ebp;                 // cf
        this->instructions_32[0x46] = &taint_x86::r_inc_si_esi;                 // cf
        this->instructions_32[0x47] = &taint_x86::r_inc_di_edi;                 // cf
        this->instructions_32[0x48] = &taint_x86::r_dec_ax_eax;                 // cf
        this->instructions_32[0x49] = &taint_x86::r_dec_cx_ecx;                 // cf
        this->instructions_32[0x4a] = &taint_x86::r_dec_dx_edx;                 // cf
        this->instructions_32[0x4b] = &taint_x86::r_dec_bx_ebx;                 // cf
        this->instructions_32[0x4c] = &taint_x86::r_dec_sp_esp;                 // cf
        this->instructions_32[0x4d] = &taint_x86::r_dec_bp_ebp;                 // cf
        this->instructions_32[0x4e] = &taint_x86::r_dec_si_esi;                 // cf
        this->instructions_32[0x4f] = &taint_x86::r_dec_di_edi;                 // cf
        this->instructions_32[0x50] = &taint_x86::r_push_ax_eax;                // cf
        this->instructions_32[0x51] = &taint_x86::r_push_cx_ecx;                // cf
        this->instructions_32[0x52] = &taint_x86::r_push_dx_edx;                // cf
        this->instructions_32[0x53] = &taint_x86::r_push_bx_ebx;                // cf
        this->instructions_32[0x54] = &taint_x86::r_push_sp_esp;                // cf
        this->instructions_32[0x55] = &taint_x86::r_push_bp_ebp;                // cf
        this->instructions_32[0x56] = &taint_x86::r_push_si_esi;                // cf
        this->instructions_32[0x57] = &taint_x86::r_push_di_edi;                // cf
        this->instructions_32[0x58] = &taint_x86::r_pop_ax_eax;                 // cf
        this->instructions_32[0x59] = &taint_x86::r_pop_cx_ecx;                 // cf
        this->instructions_32[0x5a] = &taint_x86::r_pop_dx_edx;                 // cf
        this->instructions_32[0x5b] = &taint_x86::r_pop_bx_ebx;                 // cf
        this->instructions_32[0x5c] = &taint_x86::r_pop_sp_esp;                 // cf
        this->instructions_32[0x5d] = &taint_x86::r_pop_bp_ebp;                 // cf
        this->instructions_32[0x5e] = &taint_x86::r_pop_si_esi;                 // cf
        this->instructions_32[0x5f] = &taint_x86::r_pop_di_edi;                 // cf
        this->instructions_32[0x60] = &taint_x86::r_pusha;                      // cf
        this->instructions_32[0x61] = &taint_x86::r_popa;                       // cf

        this->instructions_32[0x64] = &taint_x86::r_fs_override;                // cf
        this->instructions_32[0x65] = &taint_x86::r_gs_override;                // cf
        this->instructions_32[0x66] = &taint_x86::r_oper_size_override;         // cf
        this->instructions_32[0x67] = &taint_x86::r_addr_size_override;         // cf
        this->instructions_32[0x68] = &taint_x86::r_push_imm_16_32;             // cf
        this->instructions_32[0x69] = &taint_x86::r_imul_r_rm_imm_16_32;        // cf
        this->instructions_32[0x6a] = &taint_x86::r_push_imm_8;                 // cf
        this->instructions_32[0x6b] = &taint_x86::r_imul_r_rm_16_32_imm_8;      // cf
        this->instructions_32[0x6f] = &taint_x86::r_noop_un;                    // cf

        this->instructions_32[0x72] = &taint_x86::r_jb_jc_jnae; 
        this->instructions_32[0x73] = &taint_x86::r_jae_jnb_jnc;
        this->instructions_32[0x74] = &taint_x86::r_je_jz;
        this->instructions_32[0x75] = &taint_x86::r_jne_jnz;
        this->instructions_32[0x76] = &taint_x86::r_jbe_jna;
        this->instructions_32[0x77] = &taint_x86::r_ja_jnbe;
        this->instructions_32[0x78] = &taint_x86::r_js;
        this->instructions_32[0x79] = &taint_x86::r_jns;
        this->instructions_32[0x7a] = &taint_x86::r_jp_jpe;
        this->instructions_32[0x7b] = &taint_x86::r_jnp_jpo;
        this->instructions_32[0x7c] = &taint_x86::r_jl_jnge;
        this->instructions_32[0x7d] = &taint_x86::r_jge_jnl;
        this->instructions_32[0x7e] = &taint_x86::r_jle_jng;
        this->instructions_32[0x7f] = &taint_x86::r_jg_jnle;
        this->instructions_32[0x80] = &taint_x86::r_decode_execute_80;          // cf
        this->instructions_32[0x81] = &taint_x86::r_decode_execute_81;          // cf
        this->instructions_32[0x83] = &taint_x86::r_decode_execute_83;          // cf
        this->instructions_32[0x84] = &taint_x86::r_noop_un;                    // cf
        this->instructions_32[0x85] = &taint_x86::r_noop_un;                    // cf
        this->instructions_32[0x86] = &taint_x86::r_xchg_r_rm_8;                // cf
        this->instructions_32[0x87] = &taint_x86::r_xchg_r_rm_16_32;            // cf
        this->instructions_32[0x88] = &taint_x86::r_mov_rm_r_8;                 // cf
        this->instructions_32[0x89] = &taint_x86::r_mov_rm_r_16_32;             // cf
        this->instructions_32[0x8a] = &taint_x86::r_mov_r_rm_8;                 // cf
        this->instructions_32[0x8b] = &taint_x86::r_mov_r_rm_16_32;             // cf
        this->instructions_32[0x8d] = &taint_x86::r_lea;

        this->instructions_32[0x90] = &taint_x86::r_xchg_ax_eax_ax_eax;         // cf
        this->instructions_32[0x91] = &taint_x86::r_xchg_ax_eax_cx_ecx;         // cf
        this->instructions_32[0x92] = &taint_x86::r_xchg_ax_eax_dx_edx;         // cf
        this->instructions_32[0x93] = &taint_x86::r_xchg_ax_eax_bx_ebx;         // cf
        this->instructions_32[0x94] = &taint_x86::r_xchg_ax_eax_sp_esp;         // cf
        this->instructions_32[0x95] = &taint_x86::r_xchg_ax_eax_bp_ebp;         // cf
        this->instructions_32[0x96] = &taint_x86::r_xchg_ax_eax_si_esi;         // cf
        this->instructions_32[0x97] = &taint_x86::r_xchg_ax_eax_di_edi;         // cf

        this->instructions_32[0x99] = &taint_x86::r_cdq;                        // 

        this->instructions_32[0x9b] = &taint_x86::r_noop_un;                    // 

        this->instructions_32[0x9e] = &taint_x86::r_sahf;                       // 
        this->instructions_32[0x9f] = &taint_x86::r_lahf;                       // 

        this->instructions_32[0xa0] = &taint_x86::r_mov_al_moffset_8;           // cf
        this->instructions_32[0xa1] = &taint_x86::r_mov_ax_eax_moffset_16_32;   // cf
        this->instructions_32[0xa2] = &taint_x86::r_mov_moffset_8_al;           // cf
        this->instructions_32[0xa3] = &taint_x86::r_mov_moffset_16_32_ax_eax;   // cf
        this->instructions_32[0xa4] = &taint_x86::r_movs_moffset_8;             // cf
        this->instructions_32[0xa5] = &taint_x86::r_movs_moffset_16_32;         // cf
        this->instructions_32[0xa6] = &taint_x86::r_cmps_m_8;
        this->instructions_32[0xa7] = &taint_x86::r_cmps_m_16_32;
        this->instructions_32[0xa8] = &taint_x86::r_noop_un;                    // cf
        this->instructions_32[0xa9] = &taint_x86::r_noop_un;                    // cf
        this->instructions_32[0xaa] = &taint_x86::r_stosd_8;                    // cf
        this->instructions_32[0xab] = &taint_x86::r_stosd_16_32;                // cf
        this->instructions_32[0xac] = &taint_x86::r_lods_8;                     // cf
        this->instructions_32[0xad] = &taint_x86::r_lods_16_32;                 // cf
        this->instructions_32[0xae] = &taint_x86::r_scas_m_8;                   // cf
        this->instructions_32[0xaf] = &taint_x86::r_scas_m_16_32;               // cf
        this->instructions_32[0xb0] = &taint_x86::r_mov_al_imm8;                // cf
        this->instructions_32[0xb1] = &taint_x86::r_mov_cl_imm8;                // cf
        this->instructions_32[0xb2] = &taint_x86::r_mov_dl_imm8;                // cf
        this->instructions_32[0xb3] = &taint_x86::r_mov_bl_imm8;                // cf
        this->instructions_32[0xb4] = &taint_x86::r_mov_ah_imm8;                // cf
        this->instructions_32[0xb5] = &taint_x86::r_mov_ch_imm8;                // cf
        this->instructions_32[0xb6] = &taint_x86::r_mov_dh_imm8;                // cf
        this->instructions_32[0xb7] = &taint_x86::r_mov_bh_imm8;                // cf
        this->instructions_32[0xb8] = &taint_x86::r_mov_eax_imm_16_32;          // cf
        this->instructions_32[0xb9] = &taint_x86::r_mov_ecx_imm_16_32;          // cf
        this->instructions_32[0xba] = &taint_x86::r_mov_edx_imm_16_32;          // cf
        this->instructions_32[0xbb] = &taint_x86::r_mov_ebx_imm_16_32;          // cf
        this->instructions_32[0xbc] = &taint_x86::r_mov_esp_imm_16_32;          // cf
        this->instructions_32[0xbd] = &taint_x86::r_mov_ebp_imm_16_32;          // cf
        this->instructions_32[0xbe] = &taint_x86::r_mov_esi_imm_16_32;          // cf
        this->instructions_32[0xbf] = &taint_x86::r_mov_edi_imm_16_32;          // cf
        this->instructions_32[0xc0] = &taint_x86::r_decode_execute_c0;          // 
        this->instructions_32[0xc1] = &taint_x86::r_decode_execute_c1;          // 
        this->instructions_32[0xc2] = &taint_x86::r_retn;                       // cf
        this->instructions_32[0xc3] = &taint_x86::r_ret;                        // cf

        this->instructions_32[0xc6] = &taint_x86::r_decode_execute_c6;          // cf
        this->instructions_32[0xc7] = &taint_x86::r_decode_execute_c7;          // cf

        this->instructions_32[0xc9] = &taint_x86::r_leave;                      // cf

        this->instructions_32[0xd0] = &taint_x86::r_decode_execute_d0;          // 
        this->instructions_32[0xd1] = &taint_x86::r_decode_execute_d1;          // 
        this->instructions_32[0xd2] = &taint_x86::r_decode_execute_d2;          // 
        this->instructions_32[0xd3] = &taint_x86::r_decode_execute_d3;          // 

        this->instructions_32[0xe8] = &taint_x86::r_call_rel;                   // cf
        this->instructions_32[0xe9] = &taint_x86::r_jmp_rel_16_32;              // cf
        this->instructions_32[0xeb] = &taint_x86::r_jmp_rel_8;                   // cf

        this->instructions_32[0xf0] = &taint_x86::r_lock;                       // cf
        this->instructions_32[0xf2] = &taint_x86::r_repne;                      // cf
        this->instructions_32[0xf3] = &taint_x86::r_rep;                        // cf

        this->instructions_32[0xf6] = &taint_x86::r_decode_execute_f6;          // 
        this->instructions_32[0xf7] = &taint_x86::r_decode_execute_f7;          // 
        this->instructions_32[0xf8] = &taint_x86::r_clc;                        // cf 
        this->instructions_32[0xf9] = &taint_x86::r_stc;                        // cf
        this->instructions_32[0xfa] = &taint_x86::r_cli;                        // cf
        this->instructions_32[0xfb] = &taint_x86::r_sti;                        // cf
        this->instructions_32[0xfc] = &taint_x86::r_cld;                        // cf
        this->instructions_32[0xfd] = &taint_x86::r_std;                        // cf
        this->instructions_32[0xfe] = &taint_x86::r_decode_execute_fe;          // 
        this->instructions_32[0xff] = &taint_x86::r_decode_execute_ff;          // 

        //extended instructions

        this->instructions_32_extended[0x34] = &taint_x86::r_sysenter; 

        this->instructions_32_extended[0x80] = &taint_x86::r_noop_un; 
        this->instructions_32_extended[0x81] = &taint_x86::r_noop_un; 
        this->instructions_32_extended[0x82] = &taint_x86::r_jb_jc_jnae;
        this->instructions_32_extended[0x83] = &taint_x86::r_jae_jnb_jnc; 
        this->instructions_32_extended[0x84] = &taint_x86::r_je_jz;
        this->instructions_32_extended[0x85] = &taint_x86::r_jne_jnz;
        this->instructions_32_extended[0x86] = &taint_x86::r_jbe_jna;
        this->instructions_32_extended[0x87] = &taint_x86::r_ja_jnbe;
        this->instructions_32_extended[0x88] = &taint_x86::r_js;
        this->instructions_32_extended[0x89] = &taint_x86::r_jns;
        this->instructions_32_extended[0x8a] = &taint_x86::r_jp_jpe;
        this->instructions_32_extended[0x8b] = &taint_x86::r_jnp_jpo;
        this->instructions_32_extended[0x8c] = &taint_x86::r_jl_jnge;
        this->instructions_32_extended[0x8d] = &taint_x86::r_jge_jnl;
        this->instructions_32_extended[0x8e] = &taint_x86::r_jle_jng;
        this->instructions_32_extended[0x8f] = &taint_x86::r_jg_jnle; 

        this->instructions_32_extended[0xa4] = &taint_x86::r_shld_r_rm_16_32_imm_8; 
        this->instructions_32_extended[0xa5] = &taint_x86::r_shld_r_rm_16_32_cl; 
        this->instructions_32_extended[0xac] = &taint_x86::r_shrd_r_rm_16_32_imm_8; 
        this->instructions_32_extended[0xad] = &taint_x86::r_shrd_r_rm_16_32_cl; 
        this->instructions_32_extended[0xaf] = &taint_x86::r_imul_r_rm_16_32; 
        this->instructions_32_extended[0xb0] = &taint_x86::r_cmpxchg_rm_r_8;        // cf
        this->instructions_32_extended[0xb1] = &taint_x86::r_cmpxchg_rm_r_16_32;    // cf
        this->instructions_32_extended[0xb3] = &taint_x86::r_btr_rm_r_16_32;
        this->instructions_32_extended[0xb6] = &taint_x86::r_movzx_r_16_32_rm_8;    // cf
        this->instructions_32_extended[0xb7] = &taint_x86::r_movzx_r_32_rm_16;      // cf
        this->instructions_32_extended[0xba] = &taint_x86::r_decode_execute_0fba;
        this->instructions_32_extended[0xbc] = &taint_x86::r_bsf_r_rm_16_32;
        this->instructions_32_extended[0xbe] = &taint_x86::r_movsx_r_16_32_rm_8;
        this->instructions_32_extended[0xbf] = &taint_x86::r_movsx_r_32_rm_16;      
        this->instructions_32_extended[0xc0] = &taint_x86::r_xadd_rm_r_8;      
        this->instructions_32_extended[0xc1] = &taint_x86::r_xadd_rm_r_16_32;      

        this->cur_tid = 0x0;
        this->tid_count = 0x0;

        // fill the rest with noops
        for(unsigned int i = 0x0; i < 0x100; i++)
        {
            if(this->instructions_32[i] == 0x0)
                this->instructions_32[i] = &taint_x86::r_noop;
    
            if(this->instructions_32_extended[i] == 0x0)
                this->instructions_32_extended[i] = &taint_x86::r_noop;
        }

        for(unsigned int i = 0x0; i < MAX_THREAD_NUMBER; i++)
            this->tids[i] = -1;

        /* taint stuff */
        this->current_propagation_count = 0x1;

        this->started = 0x0;
        this->finished = 0x0;
        this->aborted = 0x0;

        /* taint stuff */
/*
        this->propagations = (PROPAGATION*)malloc(sizeof(PROPAGATION)*MAX_PRPAGATIONS_OBSERVED);
        if(this->propagations == 0x0)
        {
            printf("Not enough memory\n");
        }
*/

        /* taint stuff */
        this->taints = (REGION*)malloc(sizeof(REGION)*MAX_TAINTS_OBSERVED);
        if(this->taints == 0x0)
        {
            printf("Not enough memory\n");
        }

        this->ctx_info = (CONTEXT_INFO*)malloc(sizeof(CONTEXT_INFO)*MAX_THREADS);
        if(this->ctx_info == 0x0)
        {
            printf("Not enough memory\n");
        }

        
        /* Default options */
        fprintf(stdout, "Setting default option: OPTION_VERIFY_SEG_SEC\n");
        this->options |= OPTION_VERIFY_SEG_SEC;
        fprintf(stdout, "Setting default option: OPTION_HANDLE_BREAKPOINTS\n");
        this->options |= OPTION_HANDLE_BREAKPOINTS;
        fprintf(stdout, "Setting default option: OPTION_VERIFY_OOB\n");
        this->options |= OPTION_VERIFY_OOB;

        /* for graph_plugin */
        fprintf(stdout, "Setting default option: OPTION_ANALYZE_JUMPS\n");
        this->options |= OPTION_ANALYZE_JUMPS;
        fprintf(stdout, "Setting default option: OPTION_NOT_EMITTING_BLACKLISTED\n");
        this->options |= OPTION_NOT_EMITTING_BLACKLISTED;
        fprintf(stdout, "Setting default option: OPTION_VERIFY_SEG_SEC\n");
        this->options |= OPTION_VERIFY_SEG_SEC;
        fprintf(stdout, "Setting default option: OPTION_ANALYZE_WANTED_IN_SYMBOLS\n");
        this->options |= OPTION_ANALYZE_WANTED_IN_SYMBOLS;
        fprintf(stdout, "Setting default option: OPTION_UNMATCHED_RET_INVALIDATES_STACK\n");
        this->options |= OPTION_UNMATCHED_RET_INVALIDATES_STACK;

    }

    ~taint_x86() {
        free(this->memory);
//        free(this->propagations);
        free(this->taints);
        free(this->ctx_info);

    }

};

#endif
