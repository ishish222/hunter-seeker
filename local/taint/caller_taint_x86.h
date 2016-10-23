/*
*   Tomasz Salacinski
*   CERT Orange Polska
*   tomasz.salacinski@orange.com
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/queue.h>
#include <errno.h>

// compile-time options and parameters
#define ANALYZE_JUMPS 
#define ANALYZE_LOOPS 
//#define UNMATCHED_RET_INVALIDATES_STACK
//#define UNMATCHED_RET_CREATES_CALL
#define NOT_EMITTING_BLACKLISTED
#define NO_LOOP 0xffffffff

#define MAX_NAME                0x200
#define MAX_SYMBOL_NAME         0x50
#define MAX_SYMBOL_COUNT        0x1000000
#define MAX_LIB_COUNT           0x100
#define MAX_THREADS             0x100
#define MAX_THREAD_NUMBER       0x1000
#define MAX_CALL_LEVELS         0x200
#define GRAPH_START             100
#define MAX_LOOP_ADDRS          0x10
//#define MAX_THREADS 0x1000000
#define MAX_PRPAGATIONS_OBSERVED 0x4000000
#define MAX_TAINTS_OBSERVED     0x400
#define MAX_EXCEPTIONS_COUNT    0x100
#define MAX_BREAKPOINTS         0x10
#define MAX_                    0x10
#define MAX_BLACKLIST           0x50
#define MAX_WANTED              0x100
#define MAX_LOOP_FENCES         0x10
#define MAX_LOOP_ADDR           0x50

// deprecated
#define MEM     0xff
#define RM_R    0x0
#define RM_M    0x1

// new modrm implementation
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

#define DWORD int
#define UDWORD unsigned int
#define OFFSET unsigned int
#define WORD short
#define UWORD unsigned short
#define BYTE unsigned char
#define QWORD long long int
#define UQWORD unsigned long long int

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


/*

EXCEPTION_RECORD definitions
taken from WinNT.h from MS Visual

*/

#define EXCEPTION_MAXIMUM_PARAMETERS 15
#define EXCEPTION_RECORD EXCEPTION_RECORD32

typedef struct _EXCEPTION_RECORD32 {
    DWORD ExceptionCode;
    DWORD ExceptionFlags;
    DWORD ExceptionRecord;
    DWORD ExceptionAddress;
    DWORD NumberParameters;
    DWORD ExceptionInformation[EXCEPTION_MAXIMUM_PARAMETERS];
} EXCEPTION_RECORD32, *PEXCEPTION_RECORD32;

/*

CONTEXT definitions
taken from WinNT.h from MS Visual

*/

#define MAXIMUM_SUPPORTED_EXTENSION     512
#define SIZE_OF_80387_REGISTERS      80

typedef struct _FLOATING_SAVE_AREA {
    DWORD   ControlWord;
    DWORD   StatusWord;
    DWORD   TagWord;
    DWORD   ErrorOffset;
    DWORD   ErrorSelector;
    DWORD   DataOffset;
    DWORD   DataSelector;
    BYTE    RegisterArea[SIZE_OF_80387_REGISTERS];
    DWORD   Cr0NpxState;
} FLOATING_SAVE_AREA;

typedef FLOATING_SAVE_AREA *PFLOATING_SAVE_AREA;

typedef struct _CONTEXT {

    //
    // The flags values within this flag control the contents of
    // a CONTEXT record.
    //
    // If the context record is used as an input parameter, then
    // for each portion of the context record controlled by a flag
    // whose value is set, it is assumed that that portion of the
    // context record contains valid context. If the context record
    // is being used to modify a threads context, then only that
    // portion of the threads context will be modified.
    //
    // If the context record is used as an IN OUT parameter to capture
    // the context of a thread, then only those portions of the thread's
    // context corresponding to set flags will be returned.
    //
    // The context record is never used as an OUT only parameter.
    //

    DWORD ContextFlags;

    //
    // This section is specified/returned if CONTEXT_DEBUG_REGISTERS is
    // set in ContextFlags.  Note that CONTEXT_DEBUG_REGISTERS is NOT
    // included in CONTEXT_FULL.
    //

    DWORD   Dr0;
    DWORD   Dr1;
    DWORD   Dr2;
    DWORD   Dr3;
    DWORD   Dr6;
    DWORD   Dr7;

    //
    // This section is specified/returned if the
    // ContextFlags word contians the flag CONTEXT_FLOATING_POINT.
    //

    FLOATING_SAVE_AREA FloatSave;

    //
    // This section is specified/returned if the
    // ContextFlags word contians the flag CONTEXT_SEGMENTS.
    //

    DWORD   SegGs;
    DWORD   SegFs;
    DWORD   SegEs;
    DWORD   SegDs;

    //
    // This section is specified/returned if the
    // ContextFlags word contians the flag CONTEXT_INTEGER.
    //

    DWORD   Edi;
    DWORD   Esi;
    DWORD   Ebx;
    DWORD   Edx;
    DWORD   Ecx;
    DWORD   Eax;

    //
    // This section is specified/returned if the
    // ContextFlags word contians the flag CONTEXT_CONTROL.
    //

    DWORD   Ebp;
    DWORD   Eip;
    DWORD   SegCs;              // MUST BE SANITIZED
    DWORD   EFlags;             // MUST BE SANITIZED
    DWORD   Esp;
    DWORD   SegSs;

    //
    // This section is specified/returned if the ContextFlags word
    // contains the flag CONTEXT_EXTENDED_REGISTERS.
    // The format and contexts are processor specific
    //

    BYTE    ExtendedRegisters[MAXIMUM_SUPPORTED_EXTENSION];

} CONTEXT;

typedef CONTEXT *PCONTEXT;

typedef struct _LDT_ENTRY {
  WORD  LimitLow;
  WORD  BaseLow;
  union {
    struct {
      BYTE BaseMid;
      BYTE Flags1;
      BYTE Flags2;
      BYTE BaseHi;
    } Bytes;
    struct {
      DWORD BaseMid  :8;
      DWORD Type  :5;
      DWORD Dpl  :2;
      DWORD Pres  :1;
      DWORD LimitHi  :4;
      DWORD Sys  :1;
      DWORD Reserved_0  :1;
      DWORD Default_Big  :1;
      DWORD Granularity  :1;
      DWORD BaseHi  :8;
    } Bits;
  } HighWord;
} LDT_ENTRY, *PLDT_ENTRY;

// own wrappers

typedef struct _CONTEXT_info
{
    CONTEXT ctx;
    LDT_ENTRY ldt[0x6];
    int thread_id;
} CONTEXT_info;

typedef struct _CONTEXT_set
{
    CONTEXT_info info[0x50];
    int count;
} CONTEXT_set;

struct PROPAGATION_;

#define BYTE_VAL_MASK      0x000000ff
#define BYTE_VAL_NMASK     0xffffff00
#define BYTE_VAL_OFFSET    0x00000000
#define BYTE_VAL_T_MASK    0x0000ff00
#define BYTE_VAL_T_NMASK   0xffff00ff
#define BYTE_VAL_T_OFFSET  0x00000008

#define BYTE_VAL_I_MASK    0xff000000
#define BYTE_VAL_I_NMASK   0x00ffffff
#define BYTE_VAL_I_OFFSET  0x00000018


class BYTE_t
{
    public:
    DWORD val;
    unsigned id;
//    BYTE val;
//    BYTE val_t;
    //struct PROPAGATION_* last_propagation;

    BYTE get_BYTE()
    {
        return (BYTE)((this->val & BYTE_VAL_MASK) >> BYTE_VAL_OFFSET);
    }

    BYTE get_BYTE_t()
    {
        return (BYTE)((this->val & BYTE_VAL_T_MASK) >> BYTE_VAL_T_OFFSET);
    }

    unsigned get_BYTE_t_id()
    {
//        return (this->val & BYTE_VAL_I_MASK) >> BYTE_VAL_I_OFFSET;
        return this->id;
    }

    void print()
    {
        printf("val: 0x%08x\n", val);
        printf("id:  0x%08x\n", id);
        printf("\n");
    }

    void set_BYTE(BYTE a)
    {
        this->val &= BYTE_VAL_NMASK;
        this->val |= (((UDWORD)a << BYTE_VAL_OFFSET) & BYTE_VAL_MASK);
    }

    void set_BYTE_t(BYTE a)
    {
        this->val &= BYTE_VAL_T_NMASK;
        this->val |= (((UDWORD)a << BYTE_VAL_T_OFFSET) & BYTE_VAL_T_MASK);
    }

    void set_BYTE_t_id(unsigned a)
    {
//        this->val &= BYTE_VAL_I_NMASK;
//        this->val |= (((DWORD)a << BYTE_VAL_I_OFFSET) & BYTE_VAL_I_MASK);
        this->id = a;
    }

    BYTE_t()
    {
        this->val = 0x0;
        this->id = 0x0;
    }

    BYTE_t(BYTE a)
    {
        this->val = 0x0;
        this->id = 0x0;
        this->set_BYTE(a);
    }
    
    BYTE_t operator+(BYTE a)
    {
        BYTE_t ret;
        ret.set_BYTE(this->get_BYTE() + a);
        return ret;
    }

    BYTE_t operator-(BYTE a)
    {
        BYTE_t ret;
        ret.set_BYTE(this->get_BYTE() - a);
        return ret;
    }

    BYTE_t operator+=(BYTE a)
    {
        this->set_BYTE(this->get_BYTE() + a);
    }

    BYTE_t operator-=(BYTE a)
    {
        this->set_BYTE(this->get_BYTE() - a);
    }

    BYTE_t operator+(BYTE_t a)
    {
        BYTE_t ret;
        ret.set_BYTE(  this->get_BYTE()   + a.get_BYTE());
        ret.set_BYTE_t(this->get_BYTE_t() + a.get_BYTE_t());
        return ret;
    }

    BYTE_t operator-(BYTE_t a)
    {
        BYTE_t ret;
        ret.set_BYTE(  this->get_BYTE()   - a.get_BYTE());
        ret.set_BYTE_t(this->get_BYTE_t() - a.get_BYTE_t());
        return ret;
    }

    BYTE_t operator+=(BYTE_t a)
    {
        this->set_BYTE(  this->get_BYTE()   + a.get_BYTE());
        this->set_BYTE_t(this->get_BYTE_t() + a.get_BYTE_t());
    }

    BYTE_t operator-=(BYTE_t a)
    {
        this->set_BYTE(  this->get_BYTE()   - a.get_BYTE());
        this->set_BYTE_t(this->get_BYTE_t() - a.get_BYTE_t());
    }

    BYTE_t operator*(BYTE a)
    {
        BYTE_t ret;
        ret.set_BYTE(this->get_BYTE() * a);
        return ret;
    }

    BYTE_t operator/(BYTE a)
    {
        BYTE_t ret;
        ret.set_BYTE((DWORD)(this->get_BYTE() / a));
        return ret;
    }

    BYTE_t operator%(BYTE a)
    {
        BYTE_t ret;
        ret.set_BYTE((DWORD)(this->get_BYTE() % a));
        return ret;
    }

    BYTE_t operator*=(BYTE a)
    {
        this->set_BYTE(this->get_BYTE() * a);
    }

    BYTE_t operator/=(BYTE a)
    {
        this->set_BYTE(this->get_BYTE() / a);
    }

    BYTE_t operator%=(BYTE a)
    {
        this->set_BYTE(this->get_BYTE() % a);
    }

    BYTE_t operator*(BYTE_t a)
    {
        BYTE_t ret;
        ret.set_BYTE(  this->get_BYTE()   * a.get_BYTE());
        ret.set_BYTE_t(this->get_BYTE_t() * a.get_BYTE_t());
        return ret;
    }

    BYTE_t operator/(BYTE_t a)
    {
        BYTE_t ret;
        ret.set_BYTE(  (DWORD)(this->get_BYTE()   / a.get_BYTE()));
        ret.set_BYTE_t((DWORD)(this->get_BYTE_t() / a.get_BYTE_t()));
        return ret;
    }

    BYTE_t operator%(BYTE_t a)
    {
        BYTE_t ret;
        ret.set_BYTE(  (DWORD)(this->get_BYTE()   % a.get_BYTE()));
        ret.set_BYTE_t((DWORD)(this->get_BYTE_t() % a.get_BYTE_t()));
        return ret;
    }

    BYTE_t operator*=(BYTE_t a)
    {
        this->set_BYTE(  this->get_BYTE()   * a.get_BYTE());
        this->set_BYTE_t(this->get_BYTE_t() * a.get_BYTE_t());
    }

    BYTE_t operator/=(BYTE_t a)
    {
        this->set_BYTE(  this->get_BYTE()   / a.get_BYTE());
        this->set_BYTE_t(this->get_BYTE_t() / a.get_BYTE_t());
    }

    BYTE_t operator%=(BYTE_t a)
    {
        this->set_BYTE(  this->get_BYTE()   % a.get_BYTE());
        this->set_BYTE_t(this->get_BYTE_t() % a.get_BYTE_t());
    }

    BYTE_t operator=(BYTE a)
    {
        this->set_BYTE(a);
        this->set_BYTE_t(0x0);
    }

    BYTE_t operator=(BYTE_t a)
    {
        this->set_BYTE(  a.get_BYTE());
        this->set_BYTE_t(a.get_BYTE_t());
    }

    BYTE_t operator&(BYTE_t a)
    {
        BYTE_t ret;

        ret.set_BYTE(  this->get_BYTE()   & a.get_BYTE());
        ret.set_BYTE_t(this->get_BYTE_t() | a.get_BYTE_t()); // verify
        
        return ret;
    }

    BYTE_t operator^(BYTE_t a)
    {
        BYTE_t ret;

        ret.set_BYTE(  this->get_BYTE()   ^ a.get_BYTE());
        ret.set_BYTE_t(this->get_BYTE_t() | a.get_BYTE_t()); //verify
        
        return ret;
    }

    BYTE_t operator|(BYTE_t a)
    {
        BYTE_t ret;

        ret.set_BYTE(  this->get_BYTE()   | a.get_BYTE());
        ret.set_BYTE_t(this->get_BYTE_t() | a.get_BYTE_t()); 
        
        return ret;
    }

    BYTE_t operator&=(BYTE_t a)
    {
        this->set_BYTE(  this->get_BYTE()   & a.get_BYTE());
        this->set_BYTE_t(this->get_BYTE_t() | a.get_BYTE_t()); // verify
    }

    BYTE_t operator^=(BYTE_t a)
    {
        this->set_BYTE(  this->get_BYTE()   ^ a.get_BYTE());
        this->set_BYTE_t(this->get_BYTE_t() | a.get_BYTE_t()); //verify
    }

    BYTE_t operator|=(BYTE_t a)
    {
        this->set_BYTE(  this->get_BYTE()   | a.get_BYTE());
        this->set_BYTE_t(this->get_BYTE_t() | a.get_BYTE_t()); 
    }

    // shifts & rotates
    BYTE_t operator<<(BYTE_t a)
    {
        BYTE_t ret;

        ret.set_BYTE(  ret.get_BYTE()   << a.get_BYTE());
        ret.set_BYTE_t(ret.get_BYTE_t() << a.get_BYTE());

        return ret;
    }

    BYTE_t operator>>(BYTE_t a)
    {
        BYTE_t ret;

        ret.set_BYTE(  ret.get_BYTE()   >> a.get_BYTE());
        ret.set_BYTE_t(ret.get_BYTE_t() >> a.get_BYTE());

        return ret;
    }

    BYTE_t operator<<=(BYTE_t a)
    {
        this->set_BYTE(  this->get_BYTE()   << a.get_BYTE());
        this->set_BYTE_t(this->get_BYTE_t() << a.get_BYTE());
    }

    BYTE_t operator>>=(BYTE_t a)
    {
        this->set_BYTE(  this->get_BYTE()   >> a.get_BYTE());
        this->set_BYTE_t(this->get_BYTE_t() >> a.get_BYTE());
    }


    BYTE_t operator!()
    {
        BYTE_t ret;
        ret.set_BYTE(  !this->get_BYTE());
        ret.set_BYTE_t(!this->get_BYTE_t()); //verify
        return ret;
    }

    BYTE_t operator~()
    {
        BYTE_t ret;
        ret.set_BYTE(  ~this->get_BYTE());
        ret.set_BYTE_t(~this->get_BYTE_t()); //verify
        return ret;
    }

    void from_mem(BYTE_t* source)
    {
        this->set_BYTE(  source[0].get_BYTE());
        this->set_BYTE_t(source[0].get_BYTE_t());
    }

    void to_mem(BYTE_t* target)
    {
        target[0].set_BYTE(  this->get_BYTE());
        target[0].set_BYTE_t(this->get_BYTE_t());
    }

/*
    void from_mem_off(OFFSET off)
    {
        BYTE_t* source;
        source = &this->memory[off];
        this->set_BYTE(source[0].get_BYTE());
        this->set_BYTE_t(source[0].get_BYTE_t());
    }

    void to_mem_off(OFFSET off)
    {
        BYTE_t* target;
        target = &this->memory[off];
        target[0].get_BYTE()   = this->get_BYTE();
        target[0].set_BYTE_t(this->get_BYTE_t());
    }
*/
};

class WORD_t
{
    public:

    BYTE_t val[2];

    WORD get_WORD()
    {
        WORD ret;

        char* tmp;
        tmp = (char*)&ret;

        tmp[1] = val[0].get_BYTE();
        tmp[0] = val[1].get_BYTE();

        return ret;
    }

    WORD get_WORD_t()
    {
        DWORD ret;

        char* tmp;
        tmp = (char*)&ret;

        tmp[1] = val[0].get_BYTE_t();
        tmp[0] = val[1].get_BYTE_t();

        return ret;
    }

    void set_WORD(WORD v)
    {
        char* tmp;
        tmp = (char*)&v;

        val[1].set_BYTE(tmp[0]);
        val[0].set_BYTE(tmp[1]);

        return;
    }

    void set_WORD_t(WORD v_t)
    {
        char* tmp;
        tmp = (char*)&v_t;

        val[1].set_BYTE_t(tmp[0]);
        val[0].set_BYTE_t(tmp[1]);

        return;
    }

    void print()
    {
        printf("val[0]: 0x%08x\n", val[0].val);
        printf("id[0]:  0x%08x\n", val[0].id);
        printf("val[1]: 0x%08x\n", val[1].val);
        printf("id[1]:  0x%08x\n", val[1].id);
        printf("\n");
    }

    BYTE_t& operator[](int index)
    {
        if(index < 0x2)
            return val[index];
    }
    
    /* these need to be based on routines */

    WORD_t operator=(WORD a)
    {
        this->set_WORD(a);
/*
        char* tmp;
        tmp = (char*)&a;

        this->val[0] = tmp[0];
        this->val[1] = tmp[1];
*/
    }

    WORD_t operator=(WORD_t a)
    {
        this->set_WORD(a.get_WORD());
        this->set_WORD_t(a.get_WORD_t());
/*
        this->val[0] = a.get_BYTE()[0];
        this->val[1] = a.get_BYTE()[1];
*/
    }

    WORD_t operator+(WORD b)
    {
        WORD_t ret;
        WORD v;
        WORD t;

        v = this->get_WORD() + b;
        t = this->get_WORD_t();
        
        ret.set_WORD(v);
        ret.set_WORD_t(t);

        return ret;
    }

    WORD_t operator+(WORD_t b)
    {
        WORD_t ret;
        WORD v;
        WORD t;

        v = this->get_WORD() + b.get_WORD();
        t = this->get_WORD_t() || b.get_WORD_t();
        
        ret.set_WORD(v);
        ret.set_WORD_t(t);

        return ret;
    }

    WORD_t operator+=(WORD b)
    {   
        WORD_t ret;
        WORD v;
        WORD t;

        v = this->get_WORD() + b;
        t = this->get_WORD_t();
        
        ret.set_WORD(v);
        ret.set_WORD_t(t);
        this->set_WORD(v);
        this->set_WORD_t(t);

        return ret;
    }

    WORD_t operator+=(WORD_t b)
    {
        WORD_t ret;
        WORD v;
        WORD t;

        v = this->get_WORD() + b.get_WORD();
        t = this->get_WORD_t() || b.get_WORD_t();
        
        ret.set_WORD(v);
        ret.set_WORD_t(t);
        this->set_WORD(v);
        this->set_WORD_t(t);

        return ret;
    }

    WORD_t operator-(WORD b)
    {
        WORD_t ret;
        WORD v;
        WORD t;

        v = this->get_WORD() - b;
        t = this->get_WORD_t();
        
        ret.set_WORD(v);
        ret.set_WORD_t(t);

        return ret;
    }

    WORD_t operator-(WORD_t b)
    {
        WORD_t ret;
        WORD v;
        WORD t;

        v = this->get_WORD() - b.get_WORD();
        t = this->get_WORD_t() || b.get_WORD_t();
        
        ret.set_WORD(v);
        ret.set_WORD_t(t);

        return ret;
    }

    WORD_t operator-=(WORD b)
    {   
        WORD_t ret;
        WORD v;
        WORD t;

        v = this->get_WORD() - b;
        t = this->get_WORD_t();
        
        ret.set_WORD(v);
        ret.set_WORD_t(t);
        this->set_WORD(v);
        this->set_WORD_t(t);

        return ret;
    }

    WORD_t operator-=(WORD_t b)
    {
        WORD_t ret;
        WORD v;
        WORD t;

        v = this->get_WORD() - b.get_WORD();
        t = this->get_WORD_t() || b.get_WORD_t();
        
        ret.set_WORD(v);
        ret.set_WORD_t(t);
        this->set_WORD(v);
        this->set_WORD_t(t);

        return ret;
    }

    WORD_t operator*(WORD b)
    {
        WORD_t ret;
        WORD v;
        WORD t;

        v = this->get_WORD() * b;
        t = this->get_WORD_t();
        
        ret.set_WORD(v);
        ret.set_WORD_t(t);

        return ret;
    }

    WORD_t operator*(WORD_t b)
    {
        WORD_t ret;
        WORD v;
        WORD t;

        v = this->get_WORD() * b.get_WORD();
        t = this->get_WORD_t() || b.get_WORD_t();
        
        ret.set_WORD(v);
        ret.set_WORD_t(t);

        return ret;
    }

    WORD_t operator*=(WORD b)
    {   
        WORD_t ret;
        WORD v;
        WORD t;

        v = this->get_WORD() * b;
        t = this->get_WORD_t();
        
        ret.set_WORD(v);
        ret.set_WORD_t(t);
        this->set_WORD(v);
        this->set_WORD_t(t);

        return ret;
    }

    WORD_t operator*=(WORD_t b)
    {
        WORD_t ret;
        WORD v;
        WORD t;

        v = this->get_WORD() * b.get_WORD();
        t = this->get_WORD_t() || b.get_WORD_t();
        
        ret.set_WORD(v);
        ret.set_WORD_t(t);
        this->set_WORD(v);
        this->set_WORD_t(t);

        return ret;
    }

    WORD_t operator/(WORD b)
    {
        WORD_t ret;
        WORD v;
        WORD t;

        v = this->get_WORD() / b;
        t = this->get_WORD_t();
        
        ret.set_WORD(v);
        ret.set_WORD_t(t);

        return ret;
    }

    WORD_t operator/(WORD_t b)
    {
        WORD_t ret;
        WORD v;
        WORD t;

        v = this->get_WORD() / b.get_WORD();
        t = this->get_WORD_t() || b.get_WORD_t();
        
        ret.set_WORD(v);
        ret.set_WORD_t(t);

        return ret;
    }

    WORD_t operator/=(WORD b)
    {   
        WORD_t ret;
        WORD v;
        WORD t;

        v = this->get_WORD() / b;
        t = this->get_WORD_t();
        
        ret.set_WORD(v);
        ret.set_WORD_t(t);
        this->set_WORD(v);
        this->set_WORD_t(t);

        return ret;
    }

    WORD_t operator/=(WORD_t b)
    {
        WORD_t ret;
        WORD v;
        WORD t;

        v = this->get_WORD() / b.get_WORD();
        t = this->get_WORD_t() || b.get_WORD_t();
        
        ret.set_WORD(v);
        ret.set_WORD_t(t);
        this->set_WORD(v);
        this->set_WORD_t(t);

        return ret;
    }

    WORD_t operator%(WORD b)
    {
        WORD_t ret;
        WORD v;
        WORD t;

        v = this->get_WORD() % b;
        t = this->get_WORD_t();
        
        ret.set_WORD(v);
        ret.set_WORD_t(t);

        return ret;
    }

    WORD_t operator%(WORD_t b)
    {
        WORD_t ret;
        WORD v;
        WORD t;

        v = this->get_WORD() % b.get_WORD();
        t = this->get_WORD_t() || b.get_WORD_t();
        
        ret.set_WORD(v);
        ret.set_WORD_t(t);

        return ret;
    }

    WORD_t operator%=(WORD b)
    {   
        WORD_t ret;
        WORD v;
        WORD t;

        v = this->get_WORD() % b;
        t = this->get_WORD_t();
        
        ret.set_WORD(v);
        ret.set_WORD_t(t);
        this->set_WORD(v);
        this->set_WORD_t(t);

        return ret;
    }

    WORD_t operator%=(WORD_t b)
    {
        WORD_t ret;
        WORD v;
        WORD t;

        v = this->get_WORD() % b.get_WORD();
        t = this->get_WORD_t() || b.get_WORD_t();
        
        ret.set_WORD(v);
        ret.set_WORD_t(t);
        this->set_WORD(v);
        this->set_WORD_t(t);

        return ret;
    }

    WORD_t operator&(WORD_t a)
    {
        WORD_t ret;
        ret.val[0].set_BYTE(  this->val[0].get_BYTE()   & a[0].get_BYTE());
        ret.val[1].set_BYTE(  this->val[1].get_BYTE()   & a[1].get_BYTE());

        ret.val[0].set_BYTE_t(this->val[0].get_BYTE_t() | a[0].get_BYTE_t()); // verify
        ret.val[1].set_BYTE_t(this->val[1].get_BYTE_t() | a[1].get_BYTE_t()); // verify
        return ret;
    }

    WORD_t operator^(WORD_t a)
    {
        WORD_t ret;
        ret.val[0].set_BYTE(  this->val[0].get_BYTE()   ^ a[0].get_BYTE());
        ret.val[1].set_BYTE(  this->val[1].get_BYTE()   ^ a[1].get_BYTE());

        ret.val[0].set_BYTE_t(this->val[0].get_BYTE_t() | a[0].get_BYTE_t()); // verify
        ret.val[1].set_BYTE_t(this->val[1].get_BYTE_t() | a[1].get_BYTE_t()); // verify
        return ret;
    }

    WORD_t operator|(WORD_t a)
    {
        WORD_t ret;
        ret.val[0].set_BYTE(  this->val[0].get_BYTE()   | a[0].get_BYTE());
        ret.val[1].set_BYTE(  this->val[1].get_BYTE()   | a[1].get_BYTE());

        ret.val[0].set_BYTE_t(this->val[0].get_BYTE_t() | a[0].get_BYTE_t()); // verify
        ret.val[1].set_BYTE_t(this->val[1].get_BYTE_t() | a[1].get_BYTE_t()); // verify
        return ret;
    }

    WORD_t operator&=(WORD_t a)
    {
        this->val[0].set_BYTE(  this->val[0].get_BYTE()   & a[0].get_BYTE());
        this->val[1].set_BYTE(  this->val[1].get_BYTE()   & a[1].get_BYTE());

        this->val[0].set_BYTE_t(this->val[0].get_BYTE_t() | a[0].get_BYTE_t()); // verify
        this->val[1].set_BYTE_t(this->val[1].get_BYTE_t() | a[1].get_BYTE_t()); // verify
    }

    WORD_t operator^=(WORD_t a)
    {
        this->val[0].set_BYTE(  this->val[0].get_BYTE()   ^ a[0].get_BYTE());
        this->val[1].set_BYTE(  this->val[1].get_BYTE()   ^ a[1].get_BYTE());


        this->val[0].set_BYTE_t(this->val[0].get_BYTE_t() | a[0].get_BYTE_t()); // verify
        this->val[1].set_BYTE_t(this->val[1].get_BYTE_t() | a[1].get_BYTE_t()); // verify
    }

    WORD_t operator|=(WORD_t a)
    {
        this->val[0].set_BYTE(  this->val[0].get_BYTE()   | a[0].get_BYTE());
        this->val[1].set_BYTE(  this->val[1].get_BYTE()   | a[1].get_BYTE());

        this->val[0].set_BYTE_t(this->val[0].get_BYTE_t() | a[0].get_BYTE_t()); // verify
        this->val[1].set_BYTE_t(this->val[1].get_BYTE_t() | a[1].get_BYTE_t()); // verify
    }

    // shifts & rotates
    WORD_t operator<<(BYTE_t a)
    {
        WORD_t ret;
        WORD temp_16;

        temp_16 = this->get_WORD();
        temp_16 << a.get_BYTE();
        ret.set_WORD(temp_16);

        temp_16 = this->get_WORD_t();
        temp_16 << a.get_BYTE();
        ret.set_WORD_t(temp_16);

        return ret;
    }

    WORD_t operator>>(BYTE_t a)
    {
        WORD_t ret;
        WORD temp_16;

        temp_16 = this->get_WORD();
        temp_16 >> a.get_BYTE();
        ret.set_WORD(temp_16);

        temp_16 = this->get_WORD_t();
        temp_16 >> a.get_BYTE();
        ret.set_WORD_t(temp_16);

        return ret;
    }

    WORD_t operator<<=(BYTE_t a)
    {
        WORD temp_16;

        temp_16 = this->get_WORD();
        temp_16 <<= a.get_BYTE();
        this->set_WORD(temp_16);

        temp_16 = this->get_WORD_t();
        temp_16 <<= a.get_BYTE();
        this->set_WORD_t(temp_16);
    }

    WORD_t operator>>=(BYTE_t a)
    {
        WORD temp_16;

        temp_16 = this->get_WORD();
        temp_16 >>= a.get_BYTE();
        this->set_WORD(temp_16);

        temp_16 = this->get_WORD_t();
        temp_16 >>= a.get_BYTE();
        this->set_WORD_t(temp_16);
    }

    // what about taint?
    WORD_t operator!()
    {
        WORD_t ret;
        ret.set_WORD(!this->get_WORD());
        return ret;
    }

    // what about taint?
    WORD_t operator~()
    {
        WORD_t ret;
        ret.set_WORD(~this->get_WORD());
        return ret;
    }

    //from null
    WORD_t()
    {
        val[0] = 0x0;
        val[1] = 0x0;
    }

    //from BYTE, proxy to BYTE constructor
    WORD_t(BYTE a)
    {
        this->set_WORD(WORD(a));
    }

    //from BYTE_t, proxy to BYTE constructor
    WORD_t(BYTE_t a)
    {
        this->set_WORD(  (WORD)a.get_BYTE());
        this->set_WORD_t((WORD)a.get_BYTE_t());
    }

    //from WORD, cast to char and consider individual bytes
    WORD_t(WORD a)
    {
        this->set_WORD(a);
    }

    //from BYTE_t pointer, with endianess
    void from_mem(BYTE_t* source, int le=1)
    {
        if(!le)
        {
            val[0].set_BYTE(  source[0].get_BYTE());
            val[0].set_BYTE_t(source[0].get_BYTE_t());
            val[1].set_BYTE(  source[1].get_BYTE());
            val[1].set_BYTE_t(source[1].get_BYTE_t());

        }
        else
        {
            val[1].set_BYTE(  source[0].get_BYTE());
            val[1].set_BYTE_t(source[0].get_BYTE_t());
            val[0].set_BYTE(  source[1].get_BYTE());
            val[0].set_BYTE_t(source[1].get_BYTE_t());

        }
    }

    void to_mem(BYTE_t* target, int le=1)
    {
        if(!le)
        {
            target[0].set_BYTE(  val[0].get_BYTE());
            target[0].set_BYTE_t(val[0].get_BYTE_t());
            target[1].set_BYTE(  val[1].get_BYTE());
            target[1].set_BYTE_t(val[1].get_BYTE_t());
        }
        else
        {
            target[1].set_BYTE(  val[0].get_BYTE());
            target[1].set_BYTE_t(val[0].get_BYTE_t());
            target[0].set_BYTE(  val[1].get_BYTE());
            target[0].set_BYTE_t(val[1].get_BYTE_t());
        }
    }

};

class DWORD_t
{
    public:

    BYTE_t val[4];
    
    /* relations to WORD_t */
    
    void print()
    {
        printf("val[0]: 0x%08x\n", val[0].val);
        printf("id[0]:  0x%08x\n", val[0].id);
        printf("val[1]: 0x%08x\n", val[1].val);
        printf("id[1]:  0x%08x\n", val[1].id);
        printf("val[2]: 0x%08x\n", val[2].val);
        printf("id[2]:  0x%08x\n", val[2].id);
        printf("val[3]: 0x%08x\n", val[3].val);
        printf("id[3]:  0x%08x\n", val[3].id);
        printf("\n");
    }

    void to_WORD_t_h(WORD_t& target)
    {
        target.val[0] = val[2];
        target.val[1] = val[3];
    }

    void to_WORD_t_l(WORD_t& target)
    {
        target.val[0] = val[0];
        target.val[1] = val[1];
    }

    DWORD get_DWORD()
    {
        DWORD ret;

        char* tmp;
        tmp = (char*)&ret;

        tmp[0] = val[3].get_BYTE();
        tmp[1] = val[2].get_BYTE();
        tmp[2] = val[1].get_BYTE();
        tmp[3] = val[0].get_BYTE();

        return ret;
    }

    DWORD get_DWORD_t()
    {
        DWORD ret;

        char* tmp;
        tmp = (char*)&ret;

        tmp[0] = val[3].get_BYTE_t();
        tmp[1] = val[2].get_BYTE_t();
        tmp[2] = val[1].get_BYTE_t();
        tmp[3] = val[0].get_BYTE_t();

        return ret;
    }

    void set_DWORD(DWORD v)
    {
        char* tmp;
        tmp = (char*)&v;

        val[0].set_BYTE(tmp[3]);
        val[1].set_BYTE(tmp[2]);
        val[2].set_BYTE(tmp[1]);
        val[3].set_BYTE(tmp[0]);

        return;
    }

    void set_DWORD_t(DWORD v_t)
    {
        char* tmp;
        tmp = (char*)&v_t;

        val[0].set_BYTE_t(tmp[3]);
        val[1].set_BYTE_t(tmp[2]);
        val[2].set_BYTE_t(tmp[1]);
        val[3].set_BYTE_t(tmp[0]);

        return;
    }

    BYTE_t& operator[](int index)
    {
        if(index < 0x4)
            return val[index];
    }
   
    DWORD_t operator=(int a)
    {
        this->set_DWORD(a);
    }

    DWORD_t operator=(unsigned int a)
    {
        this->set_DWORD(a);
    }

    DWORD_t operator=(DWORD_t a)
    {
        this->val[0] = a.val[0];
        this->val[1] = a.val[1];
        this->val[2] = a.val[2];
        this->val[3] = a.val[3];
    }

    DWORD_t operator+(DWORD b)
    {
        DWORD_t ret;
        DWORD v;
        DWORD t;

        v = this->get_DWORD() + b;
        t = this->get_DWORD_t();
        
        ret.set_DWORD(v);
        ret.set_DWORD_t(t);

        return ret;
    }

    DWORD_t operator+(DWORD_t b)
    {
        DWORD_t ret;
        DWORD v;
        DWORD t;

        v = this->get_DWORD() + b.get_DWORD();
        t = this->get_DWORD_t() || b.get_DWORD_t();
        
        ret.set_DWORD(v);
        ret.set_DWORD_t(t);

        return ret;
    }

    DWORD_t operator+=(DWORD b)
    {   
        DWORD_t ret;
        DWORD v;
        DWORD t;

        v = this->get_DWORD() + b;
        t = this->get_DWORD_t();
        
        ret.set_DWORD(v);
        ret.set_DWORD_t(t);
        this->set_DWORD(v);
        this->set_DWORD_t(t);

        return ret;
    }

    DWORD_t operator+=(DWORD_t b)
    {
        DWORD_t ret;
        DWORD v;
        DWORD t;

        v = this->get_DWORD() + b.get_DWORD();
        t = this->get_DWORD_t() || b.get_DWORD_t();
        
        ret.set_DWORD(v);
        ret.set_DWORD_t(t);
        this->set_DWORD(v);
        this->set_DWORD_t(t);

        return ret;
    }

    DWORD_t operator-(DWORD b)
    {
        DWORD_t ret;
        DWORD v;
        DWORD t;

        v = this->get_DWORD() - b;
        t = this->get_DWORD_t();
        
        ret.set_DWORD(v);
        ret.set_DWORD_t(t);

        return ret;
    }

    DWORD_t operator-(DWORD_t b)
    {
        DWORD_t ret;
        DWORD v;
        DWORD t;

        v = this->get_DWORD() - b.get_DWORD();
        t = this->get_DWORD_t() || b.get_DWORD_t();
        
        ret.set_DWORD(v);
        ret.set_DWORD_t(t);

        return ret;
    }

    DWORD_t operator-=(BYTE b)
    {   
        DWORD_t ret;
        DWORD v;
        DWORD t;

        v = this->get_DWORD() - b;
        t = this->get_DWORD_t();
        
        ret.set_DWORD(v);
        ret.set_DWORD_t(t);
        this->set_DWORD(v);
        this->set_DWORD_t(t);

        return ret;
    }

    DWORD_t operator-=(BYTE_t b)
    {
        DWORD_t ret;
        DWORD v;
        DWORD t;

        v = this->get_DWORD() - b.get_BYTE();
        t = this->get_DWORD_t() || b.get_BYTE_t();
        
        ret.set_DWORD(v);
        ret.set_DWORD_t(t);
        this->set_DWORD(v);
        this->set_DWORD_t(t);

        return ret;
    }

    DWORD_t operator-=(DWORD b)
    {   
        DWORD_t ret;
        DWORD v;
        DWORD t;

        v = this->get_DWORD() - b;
        t = this->get_DWORD_t();
        
        ret.set_DWORD(v);
        ret.set_DWORD_t(t);
        this->set_DWORD(v);
        this->set_DWORD_t(t);

        return ret;
    }

    DWORD_t operator-=(DWORD_t b)
    {
        DWORD_t ret;
        DWORD v;
        DWORD t;

        v = this->get_DWORD() - b.get_DWORD();
        t = this->get_DWORD_t() || b.get_DWORD_t();
        
        ret.set_DWORD(v);
        ret.set_DWORD_t(t);
        this->set_DWORD(v);
        this->set_DWORD_t(t);

        return ret;
    }

    DWORD_t operator*(DWORD b)
    {
        DWORD_t ret;
        DWORD v;
        DWORD t;

        v = this->get_DWORD() * b;
        t = this->get_DWORD_t();
        
        ret.set_DWORD(v);
        ret.set_DWORD_t(t);

        return ret;
    }

    DWORD_t operator*(DWORD_t b)
    {
        DWORD_t ret;
        DWORD v;
        DWORD t;

        v = this->get_DWORD() * b.get_DWORD();
        t = this->get_DWORD_t() || b.get_DWORD_t();
        
        ret.set_DWORD(v);
        ret.set_DWORD_t(t);

        return ret;
    }

    DWORD_t operator*=(DWORD b)
    {   
        DWORD_t ret;
        DWORD v;
        DWORD t;

        v = this->get_DWORD() * b;
        t = this->get_DWORD_t();
        
        ret.set_DWORD(v);
        ret.set_DWORD_t(t);
        this->set_DWORD(v);
        this->set_DWORD_t(t);

        return ret;
    }

    DWORD_t operator*=(DWORD_t b)
    {
        DWORD_t ret;
        DWORD v;
        DWORD t;

        v = this->get_DWORD() * b.get_DWORD();
        t = this->get_DWORD_t() || b.get_DWORD_t();
        
        ret.set_DWORD(v);
        ret.set_DWORD_t(t);
        this->set_DWORD(v);
        this->set_DWORD_t(t);

        return ret;
    }

    DWORD_t operator/(DWORD b)
    {
        DWORD_t ret;
        DWORD v;
        DWORD t;

        v = (DWORD)(this->get_DWORD() / b);
        t = this->get_DWORD_t();
        
        ret.set_DWORD(v);
        ret.set_DWORD_t(t);

        return ret;
    }

    DWORD_t operator/(DWORD_t b)
    {
        DWORD_t ret;
        DWORD v;
        DWORD t;

        v = (DWORD)(this->get_DWORD() / b.get_DWORD());
        t = this->get_DWORD_t() || b.get_DWORD_t();
        
        ret.set_DWORD(v);
        ret.set_DWORD_t(t);

        return ret;
    }

    DWORD_t operator/=(BYTE b)
    {   
        DWORD_t ret;
        DWORD v;
        DWORD t;

        v = (DWORD)(this->get_DWORD() / b);
        t = this->get_DWORD_t();
        
        ret.set_DWORD(v);
        ret.set_DWORD_t(t);
        this->set_DWORD(v);
        this->set_DWORD_t(t);

        return ret;
    }

    DWORD_t operator/=(BYTE_t b)
    {
        DWORD_t ret;
        DWORD v;
        DWORD t;

        v = (DWORD)(this->get_DWORD() / b.get_BYTE());
        t = this->get_DWORD_t() || b.get_BYTE_t();
        
        ret.set_DWORD(v);
        ret.set_DWORD_t(t);
        this->set_DWORD(v);
        this->set_DWORD_t(t);

        return ret;
    }

    DWORD_t operator/=(DWORD b)
    {   
        DWORD_t ret;
        DWORD v;
        DWORD t;

        v = (DWORD)(this->get_DWORD() / b);
        t = this->get_DWORD_t();
        
        ret.set_DWORD(v);
        ret.set_DWORD_t(t);
        this->set_DWORD(v);
        this->set_DWORD_t(t);

        return ret;
    }

    DWORD_t operator/=(DWORD_t b)
    {
        DWORD_t ret;
        DWORD v;
        DWORD t;

        v = ((DWORD)(this->get_DWORD()) / (b.get_DWORD()));
        t = this->get_DWORD_t() || b.get_DWORD_t();
        
        ret.set_DWORD(v);
        ret.set_DWORD_t(t);
        this->set_DWORD(v);
        this->set_DWORD_t(t);

        return ret;
    }

    DWORD_t operator%(DWORD b)
    {
        DWORD_t ret;
        DWORD v;
        DWORD t;

        v = (DWORD)(this->get_DWORD() % b);
        t = this->get_DWORD_t();
        
        ret.set_DWORD(v);
        ret.set_DWORD_t(t);

        return ret;
    }

    DWORD_t operator%(DWORD_t b)
    {
        DWORD_t ret;
        DWORD v;
        DWORD t;

        v = (DWORD)(this->get_DWORD() % b.get_DWORD());
        t = this->get_DWORD_t() || b.get_DWORD_t();
        
        ret.set_DWORD(v);
        ret.set_DWORD_t(t);

        return ret;
    }

    DWORD_t operator%=(BYTE b)
    {   
        DWORD_t ret;
        DWORD v;
        DWORD t;

        v = (DWORD)(this->get_DWORD() % b);
        t = this->get_DWORD_t();
        
        ret.set_DWORD(v);
        ret.set_DWORD_t(t);
        this->set_DWORD(v);
        this->set_DWORD_t(t);

        return ret;
    }

    DWORD_t operator%=(BYTE_t b)
    {
        DWORD_t ret;
        DWORD v;
        DWORD t;

        v = (DWORD)(this->get_DWORD() % b.get_BYTE());
        t = this->get_DWORD_t() || b.get_BYTE_t();
        
        ret.set_DWORD(v);
        ret.set_DWORD_t(t);
        this->set_DWORD(v);
        this->set_DWORD_t(t);

        return ret;
    }

    DWORD_t operator%=(DWORD b)
    {   
        DWORD_t ret;
        DWORD v;
        DWORD t;

        v = (DWORD)(this->get_DWORD() % b);
        t = this->get_DWORD_t();
        
        ret.set_DWORD(v);
        ret.set_DWORD_t(t);
        this->set_DWORD(v);
        this->set_DWORD_t(t);

        return ret;
    }

    DWORD_t operator%=(DWORD_t b)
    {
        DWORD_t ret;
        DWORD v;
        DWORD t;

        v = (DWORD)(this->get_DWORD() % b.get_DWORD());
        t = this->get_DWORD_t() || b.get_DWORD_t();
        
        ret.set_DWORD(v);
        ret.set_DWORD_t(t);
        this->set_DWORD(v);
        this->set_DWORD_t(t);

        return ret;
    }

    DWORD_t operator&(DWORD_t a)
    {
        DWORD_t ret;
        ret.val[0].set_BYTE(this->val[0].get_BYTE() & a[0].get_BYTE());
        ret.val[1].set_BYTE(this->val[1].get_BYTE() & a[1].get_BYTE());
        ret.val[2].set_BYTE(this->val[2].get_BYTE() & a[2].get_BYTE());
        ret.val[3].set_BYTE(this->val[3].get_BYTE() & a[3].get_BYTE());

        ret.val[0].set_BYTE_t(this->val[0].get_BYTE_t() | a[0].get_BYTE_t()); // verify
        ret.val[1].set_BYTE_t(this->val[1].get_BYTE_t() | a[1].get_BYTE_t()); // verify
        ret.val[2].set_BYTE_t(this->val[2].get_BYTE_t() | a[2].get_BYTE_t()); // verify
        ret.val[3].set_BYTE_t(this->val[3].get_BYTE_t() | a[3].get_BYTE_t()); // verify
        return ret;
    }

    DWORD_t operator^(DWORD_t a)
    {
        DWORD_t ret;
        ret.val[0].set_BYTE(this->val[0].get_BYTE() ^ a[0].get_BYTE());
        ret.val[1].set_BYTE(this->val[1].get_BYTE() ^ a[1].get_BYTE());
        ret.val[2].set_BYTE(this->val[2].get_BYTE() ^ a[2].get_BYTE());
        ret.val[3].set_BYTE(this->val[3].get_BYTE() ^ a[3].get_BYTE());

        ret.val[0].set_BYTE_t(this->val[0].get_BYTE_t() | a[0].get_BYTE_t()); // verify
        ret.val[1].set_BYTE_t(this->val[1].get_BYTE_t() | a[1].get_BYTE_t()); // verify
        ret.val[2].set_BYTE_t(this->val[2].get_BYTE_t() | a[2].get_BYTE_t()); // verify
        ret.val[3].set_BYTE_t(this->val[3].get_BYTE_t() | a[3].get_BYTE_t()); // verify
        return ret;
    }

    DWORD_t operator|(DWORD_t a)
    {
        DWORD_t ret;
        ret.val[0].set_BYTE(this->val[0].get_BYTE() | a[0].get_BYTE());
        ret.val[1].set_BYTE(this->val[1].get_BYTE() | a[1].get_BYTE());
        ret.val[2].set_BYTE(this->val[2].get_BYTE() | a[2].get_BYTE());
        ret.val[3].set_BYTE(this->val[3].get_BYTE() | a[3].get_BYTE());

        ret.val[0].set_BYTE_t(this->val[0].get_BYTE_t() | a[0].get_BYTE_t()); // verify
        ret.val[1].set_BYTE_t(this->val[1].get_BYTE_t() | a[1].get_BYTE_t()); // verify
        ret.val[2].set_BYTE_t(this->val[2].get_BYTE_t() | a[2].get_BYTE_t()); // verify
        ret.val[3].set_BYTE_t(this->val[3].get_BYTE_t() | a[3].get_BYTE_t()); // verify
        return ret;
    }

    DWORD_t operator&=(DWORD_t a)
    {
        this->val[0].set_BYTE(this->val[0].get_BYTE() & a[0].get_BYTE());
        this->val[1].set_BYTE(this->val[1].get_BYTE() & a[1].get_BYTE());
        this->val[2].set_BYTE(this->val[2].get_BYTE() & a[2].get_BYTE());
        this->val[3].set_BYTE(this->val[3].get_BYTE() & a[3].get_BYTE());

        this->val[0].set_BYTE_t(this->val[0].get_BYTE_t() | a[0].get_BYTE_t()); // verify
        this->val[1].set_BYTE_t(this->val[1].get_BYTE_t() | a[1].get_BYTE_t()); // verify
        this->val[2].set_BYTE_t(this->val[2].get_BYTE_t() | a[2].get_BYTE_t()); // verify
        this->val[3].set_BYTE_t(this->val[3].get_BYTE_t() | a[3].get_BYTE_t()); // verify
    }

    DWORD_t operator^=(DWORD_t a)
    {
        this->val[0].set_BYTE(this->val[0].get_BYTE() ^ a[0].get_BYTE());
        this->val[1].set_BYTE(this->val[1].get_BYTE() ^ a[1].get_BYTE());
        this->val[2].set_BYTE(this->val[2].get_BYTE() ^ a[2].get_BYTE());
        this->val[3].set_BYTE(this->val[3].get_BYTE() ^ a[3].get_BYTE());


        this->val[0].set_BYTE_t(this->val[0].get_BYTE_t() | a[0].get_BYTE_t()); // verify
        this->val[1].set_BYTE_t(this->val[1].get_BYTE_t() | a[1].get_BYTE_t()); // verify
        this->val[2].set_BYTE_t(this->val[2].get_BYTE_t() | a[2].get_BYTE_t()); // verify
        this->val[3].set_BYTE_t(this->val[3].get_BYTE_t() | a[3].get_BYTE_t()); // verify
    }

    DWORD_t operator|=(DWORD_t a)
    {
        this->val[0].set_BYTE(this->val[0].get_BYTE() | a[0].get_BYTE());
        this->val[1].set_BYTE(this->val[1].get_BYTE() | a[1].get_BYTE());
        this->val[2].set_BYTE(this->val[2].get_BYTE() | a[2].get_BYTE());
        this->val[3].set_BYTE(this->val[3].get_BYTE() | a[3].get_BYTE());

        this->val[0].set_BYTE_t(this->val[0].get_BYTE_t() | a[0].get_BYTE_t()); // verify
        this->val[1].set_BYTE_t(this->val[1].get_BYTE_t() | a[1].get_BYTE_t()); // verify
        this->val[2].set_BYTE_t(this->val[2].get_BYTE_t() | a[2].get_BYTE_t()); // verify
        this->val[3].set_BYTE_t(this->val[3].get_BYTE_t() | a[3].get_BYTE_t()); // verify
    }

    // shifts & rotates
    DWORD_t operator<<(BYTE_t a)
    {
        DWORD_t ret;
        DWORD temp_32;

        temp_32 = this->get_DWORD();
        temp_32 << a.get_BYTE();
        ret.set_DWORD(temp_32);

        temp_32 = this->get_DWORD_t();
        temp_32 << a.get_BYTE();
        ret.set_DWORD_t(temp_32);

        return ret;
    }

    DWORD_t operator>>(BYTE_t a)
    {
        DWORD_t ret;
        DWORD temp_32;

        temp_32 = this->get_DWORD();
        temp_32 >> a.get_BYTE();
        ret.set_DWORD(temp_32);

        temp_32 = this->get_DWORD_t();
        temp_32 >> a.get_BYTE();
        ret.set_DWORD_t(temp_32);

        return ret;
    }

    DWORD_t operator<<=(BYTE_t a)
    {
        DWORD temp_32;

        temp_32 = this->get_DWORD();
        temp_32 <<= a.get_BYTE();
        this->set_DWORD(temp_32);

        temp_32 = this->get_DWORD_t();
        temp_32 <<= a.get_BYTE();
        this->set_DWORD_t(temp_32);
    }

    DWORD_t operator>>=(BYTE_t a)
    {
        DWORD temp_32;

        temp_32 = this->get_DWORD();
        temp_32 >>= a.get_BYTE();
        this->set_DWORD(temp_32);

        temp_32 = this->get_DWORD_t();
        temp_32 >>= a.get_BYTE();
        this->set_DWORD_t(temp_32);
    }

    // what about taint?
    DWORD_t operator!()
    {
        DWORD_t ret;
        ret.set_DWORD(!this->get_DWORD());
        return ret;
    }

    // complement with 2
    // what about taint?
    DWORD_t operator~()
    {
        DWORD_t ret;
        ret.set_DWORD(~this->get_DWORD());
        return ret;
    }

    //from null
    DWORD_t()
    {
        val[0] = 0x0;
        val[1] = 0x0;
        val[2] = 0x0;
        val[3] = 0x0;
    }

    //from BYTE, proxy to BYTE constructor
    DWORD_t(BYTE a)
    {
        this->set_DWORD(DWORD(a));
    }

    //from BYTE_t, proxy to BYTE constructor
    DWORD_t(BYTE_t a)
    {
        this->set_DWORD(  DWORD(a.get_BYTE()));
        this->set_DWORD_t(DWORD(a.get_BYTE_t()));
    }

    //from WORD, proxy to WORD constructor
    DWORD_t(WORD a)
    {
        this->set_DWORD(DWORD(a));
    }

    //from WORD_t, proxy to WORD_t constructor
    DWORD_t(WORD_t a)
    {
        this->set_DWORD(  DWORD(a.get_WORD()));
        this->set_DWORD_t(DWORD(a.get_WORD_t()));
    }

    //from OFFSET, cast to char and consider individual bytes
    DWORD_t(OFFSET a)
    {
        this->set_DWORD((int)a);
    }
 
    //from DWORD, cast to char and consider individual bytes
    DWORD_t(DWORD a)
    {
        this->set_DWORD(a);
    }
 
   //from BYTE_t pointer, with endianess
    void from_mem(BYTE_t* source, int le=1)
    {
        if(!le)
        {
            val[0].set_BYTE(  source[0].get_BYTE());
            val[0].set_BYTE_t(source[0].get_BYTE_t());
            val[1].set_BYTE(  source[1].get_BYTE());
            val[1].set_BYTE_t(source[1].get_BYTE_t());
            val[2].set_BYTE(  source[2].get_BYTE());
            val[2].set_BYTE_t(source[2].get_BYTE_t());
            val[3].set_BYTE(  source[3].get_BYTE());
            val[3].set_BYTE_t(source[3].get_BYTE_t());
        }
        else
        {
            val[3].set_BYTE(  source[0].get_BYTE());
            val[3].set_BYTE_t(source[0].get_BYTE_t());
            val[2].set_BYTE(  source[1].get_BYTE());
            val[2].set_BYTE_t(source[1].get_BYTE_t());
            val[1].set_BYTE(  source[2].get_BYTE());
            val[1].set_BYTE_t(source[2].get_BYTE_t());
            val[0].set_BYTE(  source[3].get_BYTE());
            val[0].set_BYTE_t(source[3].get_BYTE_t());
        }
    }

    void to_mem(BYTE_t* target, int le=1)
    {
        if(!le)
        {
            target[0].set_BYTE(  val[0].get_BYTE());
            target[0].set_BYTE_t(val[0].get_BYTE_t());
            target[1].set_BYTE(  val[1].get_BYTE());
            target[1].set_BYTE_t(val[1].get_BYTE_t());
            target[2].set_BYTE(  val[2].get_BYTE());
            target[2].set_BYTE_t(val[2].get_BYTE_t());
            target[3].set_BYTE(  val[3].get_BYTE());
            target[3].set_BYTE_t(val[3].get_BYTE_t());
        }
        else
        {
            target[3].set_BYTE(  val[0].get_BYTE());
            target[3].set_BYTE_t(val[0].get_BYTE_t());
            target[2].set_BYTE(  val[1].get_BYTE());
            target[2].set_BYTE_t(val[1].get_BYTE_t());
            target[1].set_BYTE(  val[2].get_BYTE());
            target[1].set_BYTE_t(val[2].get_BYTE_t());
            target[0].set_BYTE(  val[3].get_BYTE());
            target[0].set_BYTE_t(val[3].get_BYTE_t());
        }
    }

};

#define FENCE_INACTIVE          0x0
#define FENCE_ACTIVE            0x1
#define FENCE_COLLECTING        0x2
#define FENCE_NOT_COLLECTING    0x3
#define FENCE_FINISHED          0x4

typedef struct LOOP_FENCE_
{
    OFFSET entry;
    OFFSET start;
    OFFSET limit;
    OFFSET struct_size;
    OFFSET struct_count;

    char collecting;
    char status;

} LOOP_FENCE;

typedef struct SYMBOL_
{
    OFFSET addr;
    char* lib_name;
    char* func_name;
    struct SYMBOL_* next;
    DWORD resolved;
    DWORD wanted;
    DWORD included;
} SYMBOL;

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


typedef struct _LIB_INFO
{
    OFFSET offset;
    char name[MAX_NAME];
    char path[MAX_NAME];
    DWORD length;
    DWORD loaded;
    char blacklisted;
    char* content;
    SYMBOL* symbols;
} LIB_INFO;

typedef struct _EXCEPTION_INFO
{
    EXCEPTION_RECORD er;
    DWORD tid;
} EXCEPTION_INFO;

typedef struct _CALL_LEVEL
{
    DWORD ret;
    OFFSET entry;
    /* loops handling */

    unsigned call_src_register_idx;
    unsigned loop_start;

    /* new loop handling */
    unsigned loop_pos;
    LOOP_FENCE* cur_fence;

    /* new new loop handling */
    unsigned loop_addr_idx;
    unsigned loop_limit;
    unsigned loop_struct_size;
    unsigned loop_struct_count;
    OFFSET   loop_addr[MAX_LOOP_ADDR];
    char     loop_status;

} CALL_LEVEL;

typedef struct _CONTEXT_INFO
{
    DWORD tid;
    BYTE_t registers[REG_SIZE];
    OFFSET seg_map[0x6];

    CALL_LEVEL* levels;

    /* graph handling */
    DWORD call_level;
    DWORD call_level_smallest;
    DWORD call_level_largest;
    DWORD call_level_ignored;
    char graph_filename[MAX_NAME];
    FILE* graph_file;
    OFFSET waiting;

    /* call level handling */ 
    unsigned ret_idx;
    char returning;
    char before_returning;
    char before_waiting;
    char calling;
    OFFSET target;
    OFFSET next;

} CONTEXT_INFO;

#define BP_MODE_READ    0x1
#define BP_MODE_WRITE   0x2
#define BP_MODE_EXECUTE 0x4



typedef struct BREAKPOINT_
{
    OFFSET offset;
    OFFSET mem_offset;
    BYTE mode;    
} BREAKPOINT;

typedef struct TRACE_WATCHPOINT_
{
    OFFSET offset;
    OFFSET mem_offset;
    BYTE_t* watched;
    char name[MAX_NAME];
    DWORD tid;
    char interactive;
} TRACE_WATCHPOINT;

typedef struct TAINT_WATCHPOINT_8_
{
    BYTE_t* watched;
    char name[MAX_NAME];
    unsigned long long count;
} TAINT_WATCHPOINT_8;

typedef struct TAINT_WATCHPOINT_16_
{
    WORD_t* watched;
    char name[MAX_NAME];
    unsigned long long count;
} TAINT_WATCHPOINT_16;

typedef struct TAINT_WATCHPOINT_32_
{
    DWORD_t* watched;
    char name[MAX_NAME];
    unsigned long long count;
} TAINT_WATCHPOINT_32;

typedef struct REGION_
{
    OFFSET off;
    OFFSET size;
} REGION;

class taint_x86
{
    public:
    FILE* dump_file;
    FILE* instr_file;
    FILE* lost_file;
    FILE* mod_file;

    /* graph parameters */
    DWORD start_addr;
    DWORD end_addr;
    OFFSET start_instr;
    OFFSET instr_limit;
    unsigned max_call_levels;
    unsigned call_level_start;
    unsigned call_level_offset;

    unsigned counter;

    DWORD depth;

    DWORD started;
    DWORD finished;
    DWORD aborted;

    /* memory parameters */
    BYTE_t* memory;
    DWORD mem_length;

    /* analysis options */
    DWORD options;

    BYTE_t invalid_byte;
    WORD_t invalid_word;
    DWORD_t invalid_dword;

    int current_prefixes;
    BYTE_t* last_instr_byte;
    BYTE_t* current_instr_byte;
    DWORD current_instr_is_jump;
    OFFSET current_eip;
    OFFSET last_eip;
    char last_inconsistent;
    unsigned long long  last_instr_count;
    unsigned long long  current_instr_count;
    BYTE current_instr_length;
    BYTE current_prefix_length;
    
    BREAKPOINT  my_bps[0x10];
    TAINT_WATCHPOINT_8 t8[0x10];
    TAINT_WATCHPOINT_16 t16[0x10];
    TAINT_WATCHPOINT_32 t32[0x10];
    OFFSET taint_checks[0x10];

    /* new breakpoints & watchpoints */

    BREAKPOINT new_bps[MAX_BREAKPOINTS];
    BREAKPOINT new_bps_t[MAX_BREAKPOINTS];
    TRACE_WATCHPOINT new_wps[MAX_BREAKPOINTS];   
    unsigned new_bpt_count;
    unsigned new_bpt_t_count;
    unsigned new_wpt_count;
 
    int add_breakpoint(BREAKPOINT);
    int add_taint_breakpoint(BREAKPOINT);
    int add_trace_watchpoint(TRACE_WATCHPOINT);
    int update_watchpoints(DWORD);

    CONTEXT_INFO* ctx_info;
    CONTEXT_INFO* cur_info;

    char enumerate;

    REGION* taints;
    unsigned taint_count;

    unsigned current_propagation_count;
    // moved to heap
//    PROPAGATION propagations[MAX_PRPAGATIONS_OBSERVED];
    PROPAGATION* propagations;

    DWORD tids[MAX_THREAD_NUMBER];
    DWORD tid_count;
    DWORD cur_tid;
    DWORD bp_hit;
    DWORD extended;
    DWORD rw_bp;

//    LIB_INFO libs[0x100];
    LIB_INFO* libs;
    unsigned libs_count;
    char lib_dir_path[MAX_NAME];
//    char lib_blacklist[MAX_NAME][MAX_BLACKLIST];
    char* lib_blacklist[MAX_NAME];
    unsigned blacklist_count;
    DWORD addr_blacklist[MAX_BLACKLIST];
    unsigned addr_blacklist_count;
//    char func_wanted[MAX_NAME][MAX_WANTED];
//    char func_included[MAX_NAME][MAX_WANTED];
    char* func_wanted[MAX_NAME];
    char* func_included[MAX_NAME];
    unsigned instr_wanted[MAX_WANTED];
    DWORD addr_wanted[MAX_WANTED];
    unsigned wanted_count;
    unsigned included_count;
    unsigned wanted_count_e;
    unsigned wanted_count_i;

    EXCEPTION_INFO exceptions[MAX_EXCEPTIONS_COUNT];
    DWORD exceptions_count;

    SYMBOL* symbols; 
    unsigned symbols_count;

    unsigned ret_idx;

    int add_symbol(SYMBOL**, OFFSET, char*, char*);
    int remove_symbol(SYMBOL*);
    SYMBOL* get_symbol(OFFSET);

    /* loop fences - new approach */
    LOOP_FENCE loop_fences[MAX_LOOP_FENCES]; 
    unsigned loop_fences_count;
    
    int enter_loop_demo(CONTEXT_INFO*);
    int exit_loop_demo(CONTEXT_INFO*);
    int enter_loop(CONTEXT_INFO*);
    int exit_loop(CONTEXT_INFO*);
    //int check_loop(CONTEXT_INFO*, OFFSET, OFFSET);
    int check_loop(CONTEXT_INFO*);
    int check_loop_2(CONTEXT_INFO*);
    int check_collecting(CONTEXT_INFO*);

    /* dumping taint transfer history */
    int write_history(FILE*);
    int dump_cmd(char*);
    int prompt_taint();
    TRACE_WATCHPOINT parse_trace_string(char* string);
    int query_history(TRACE_WATCHPOINT twp);

    int modrm_table_32[0x100];

    typedef int (taint_x86::*instruction_routine)(BYTE_t* args);
    instruction_routine instructions_32[0x100];
    instruction_routine instructions_32_extended[0x100];
//    instruction_routine escaped_instructions_32[0x10000];


    // instruction routines
    int r_noop(BYTE_t*);
    int r_noop_un(BYTE_t*);
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
    int a_is_prefix(BYTE_t*);
    int a_decode_modrm(BYTE_t*, modrm_ptr*, modrm_ptr*, int, int);
    OFFSET a_decode_sib(BYTE_t*);
    OFFSET a_decode_sib_mod(BYTE_t*, BYTE);
    OFFSET a_offset_recalculate_16(OFFSET);
    OFFSET a_offset_recalculate_8(OFFSET);
    int verify_oob_offset(OFFSET, OFFSET);

    int a_push_lost_32(DWORD);
    int a_push_lost_16(WORD);
    int a_push_lost_8(BYTE);
    DWORD a_pop_lost();
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

    // other
    DWORD le2dword(char*);
    CONTEXT_INFO* get_tid(DWORD);
    int already_added(DWORD);
    int add_thread(CONTEXT_info);
    int mod_thread(CONTEXT_info);
    int del_thread(DWORD);
    int del_thread_srsly(DWORD);
    int check_thread(CONTEXT_info);
    int add_taint(OFFSET start, UDWORD length);
    int add_lib(OFFSET, char*);
    int del_lib(OFFSET);
    int add_exception(EXCEPTION_INFO);
    int load_mem_from_file(char*);
    int load_instr_from_file(char*);
    int open_lost_file(char*);
    int open_mod_file(char*);
    int close_files();
    int set_lib_dir_path(char*);
    int add_blacklist(char*);
    int add_blacklist_addr(DWORD);
    int add_included(char*);
    int add_wanted(char*);
    int add_wanted_e(DWORD);
    int add_wanted_i(unsigned);
    int add_fence(OFFSET, OFFSET, OFFSET, OFFSET);
    int check_fence(CALL_LEVEL*);
    int check_lib_blacklist(LIB_INFO*);
    int check_addr_blacklist(OFFSET);
    int check_func_wanted(char*);
    int check_func_included(char*);
    int check_rets(OFFSET);
    LIB_INFO* get_lib(OFFSET);
    int handle_jmp(CONTEXT_INFO*);
    int handle_call(CONTEXT_INFO*);
    int handle_ret(CONTEXT_INFO*, OFFSET);
    int handle_exception(EXCEPTION_INFO);
    int apply_lib_layout(LIB_INFO*);
    int apply_lib_exports(LIB_INFO*);
    int apply_memory(DWORD, DWORD);
    int add_symbols(LIB_INFO*);
    int copy_symbol(SYMBOL**, SYMBOL*);
    int handle_sigsegv();

/*
    int from_mem(BYTE_t&, OFFSET, int);
    int from_mem(WORD_t&, OFFSET, int);
    int from_mem(DWORD_t&, OFFSET, int);
    int to_mem(BYTE_t&, OFFSET, int);
    int to_mem(WORD_t&, OFFSET, int);
    int to_mem(DWORD_t&, OFFSET, int);
*/

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

    // engine methods
    //int execute_instruction(char*);
    int pre_execute_instruction(DWORD);
    int execute_instruction(DWORD, DWORD);
    int post_execute_instruction(DWORD);
    int execute_instruction_at_eip(DWORD);
    int execute_instruction_at_eip(DWORD, DWORD);
    int finish();
    int verify_t_context(int);

    /* graph prints */

    void print_call(CONTEXT_INFO*, char*, const char*);
    void print_empty_call(CONTEXT_INFO*, char*, const char*);
    void print_ret(CONTEXT_INFO*);
    int dive(CONTEXT_INFO*, OFFSET, OFFSET);
    int surface(CONTEXT_INFO*);

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
    int print_bt_buffer(BYTE_t*, DWORD);
    int print_all_regs();
    int decode_modrm_byte(BYTE_t*, DWORD_t*, DWORD_t*);
    int decode_modrm_byte(BYTE_t*, OFFSET*, OFFSET*);

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

        this->instructions_32[0x72] = &taint_x86::r_noop_un;                    // cf
        this->instructions_32[0x73] = &taint_x86::r_noop_un;                    // cf
        this->instructions_32[0x74] = &taint_x86::r_noop_un;                    // cf
        this->instructions_32[0x75] = &taint_x86::r_noop_un;                    // cf
        this->instructions_32[0x76] = &taint_x86::r_noop_un;                    // cf
        this->instructions_32[0x77] = &taint_x86::r_noop_un;                    // cf
        this->instructions_32[0x78] = &taint_x86::r_noop_un;                    // cf
        this->instructions_32[0x79] = &taint_x86::r_noop_un;                    // cf
        this->instructions_32[0x7a] = &taint_x86::r_noop_un;                    // cf
        this->instructions_32[0x7b] = &taint_x86::r_noop_un;                    // cf
        this->instructions_32[0x7c] = &taint_x86::r_noop_un;                    // cf
        this->instructions_32[0x7d] = &taint_x86::r_noop_un;                    // cf
        this->instructions_32[0x7e] = &taint_x86::r_noop_un;                    // cf
        this->instructions_32[0x7f] = &taint_x86::r_noop_un;                    // cf
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
        this->instructions_32_extended[0x82] = &taint_x86::r_noop_un; 
        this->instructions_32_extended[0x83] = &taint_x86::r_noop_un; 
        this->instructions_32_extended[0x84] = &taint_x86::r_noop_un; 
        this->instructions_32_extended[0x85] = &taint_x86::r_noop_un; 
        this->instructions_32_extended[0x86] = &taint_x86::r_noop_un; 
        this->instructions_32_extended[0x87] = &taint_x86::r_noop_un; 
        this->instructions_32_extended[0x88] = &taint_x86::r_noop_un; 
        this->instructions_32_extended[0x89] = &taint_x86::r_noop_un; 
        this->instructions_32_extended[0x8a] = &taint_x86::r_noop_un; 
        this->instructions_32_extended[0x8b] = &taint_x86::r_noop_un; 
        this->instructions_32_extended[0x8c] = &taint_x86::r_noop_un; 
        this->instructions_32_extended[0x8d] = &taint_x86::r_noop_un; 
        this->instructions_32_extended[0x8e] = &taint_x86::r_noop_un; 
        this->instructions_32_extended[0x8f] = &taint_x86::r_noop_un; 

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

        // zero out bps
        for(unsigned int i = 0x0; i < 0x10; i++)
            this->my_bps[i].offset = 0x0;
        // zero out t8
        for(unsigned int i = 0x0; i < 0x10; i++)
            this->t8[i].count = 0x0;
        // zero out t16
        for(unsigned int i = 0x0; i < 0x10; i++)
            this->t16[i].count = 0x0;
        // zero out t32
        for(unsigned int i = 0x0; i < 0x10; i++)
            this->t32[i].count = 0x0;
        // zero out tids
        for(unsigned int i = 0x0; i < MAX_THREAD_NUMBER; i++)
            this->tids[i] = -1;

        this->current_propagation_count = 0x1;

        this->started = 0x0;
        this->finished = 0x0;
        this->aborted = 0x0;

        this->blacklist_count = 0x0;
        this->wanted_count = 0x0;
        this->wanted_count_i = 0x0;
        this->wanted_count_e = 0x0;

        this->max_call_levels = MAX_CALL_LEVELS;
        this->call_level_start = this->max_call_levels/3;


/*
        this->propagations = (PROPAGATION*)malloc(sizeof(PROPAGATION)*MAX_PRPAGATIONS_OBSERVED);
        if(this->propagations == 0x0)
        {
            printf("Not enough memory\n");
        }
*/
        unsigned i;
        for(i = 0x0; i< MAX_BLACKLIST; i++)
        {
            this->lib_blacklist[i] = (char*)malloc(MAX_NAME);
            if(this->lib_blacklist[0] == 0x0)
            {
                printf("Not enough memory\n");
            }
        }

        for(i = 0x0; i< MAX_WANTED; i++)
        {
            this->func_wanted[i] = (char*)malloc(MAX_NAME);
            if(this->func_wanted[i] == 0x0)
            {
                printf("Not enough memory\n");
            }
        }

        for(i = 0x0; i< MAX_WANTED; i++)
        {
            this->func_included[i] = (char*)malloc(MAX_NAME);
            if(this->func_included[i] == 0x0)
            {
                printf("Not enough memory\n");
            }
        }

        this->taints = (REGION*)malloc(sizeof(REGION)*MAX_TAINTS_OBSERVED);
        if(this->taints == 0x0)
        {
            printf("Not enough memory\n");
        }

        this->libs = (LIB_INFO*)malloc(sizeof(LIB_INFO)*MAX_LIB_COUNT);
        if(this->libs == 0x0)
        {
            printf("Not enough memory\n");
        }


        this->ctx_info = (CONTEXT_INFO*)malloc(sizeof(CONTEXT_INFO)*MAX_THREADS);
        if(this->ctx_info == 0x0)
        {
            printf("Not enough memory\n");
        }

    }

    ~taint_x86() {
        free(this->memory);
//        free(this->propagations);
        free(this->taints);
        free(this->libs);
        free(this->ctx_info);

        unsigned i;
        for(i = 0x0; i< MAX_BLACKLIST; i++)
        {
            free(this->lib_blacklist[i]);
        }

        for(i = 0x0; i< MAX_WANTED; i++)
        {
            free(this->func_wanted[i]);
        }

        for(i = 0x0; i< MAX_WANTED; i++)
        {
            free(this->func_included[i]);
        }

    }

};


