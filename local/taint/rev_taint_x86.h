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

#define MAX_NAME 0x100
#define MAX_THREADS 0x10
//#define MAX_THREADS 0x1000000

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

// decoding modes

# define MODE_32 0x0
# define MODE_16 0x1
# define MODE_8  0x2

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
#define OFFSET unsigned int
#define WORD short
#define BYTE unsigned char

/* modrm target & sources */

// regions
#define MODRM_REG   0x1
#define MODRM_MEM   0x2

// sizes
#define MODRM_SIZE_8     0x1
#define MODRM_SIZE_16    0x2
#define MODRM_SIZE_32    0x3
#define MODRM_SIZE_64    0x4

#define REVERSE_ANALYSIS

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

class BYTE_t
{
    public:

    BYTE val;
    BYTE val_t;

    BYTE_t()
    {
        this->val = 0x0;
        this->val_t = 0x0;
    }

    BYTE_t(BYTE a)
    {
        this->val = a;
        this->val_t = 0x0;
    }
    
    BYTE_t operator+(BYTE a)
    {
        BYTE_t ret;
        ret.val = this->val + a;
        return ret;
    }

    BYTE_t operator-(BYTE a)
    {
        BYTE_t ret;
        ret.val = this->val - a;
        return ret;
    }

    BYTE_t operator+=(BYTE a)
    {
        this->val += a;
    }

    BYTE_t operator-=(BYTE a)
    {
        this->val -= a;
    }

    BYTE_t operator+(BYTE_t a)
    {
        BYTE_t ret;
        ret.val = this->val + a.val;
        ret.val_t = this->val_t + a.val_t;
        return ret;
    }

    BYTE_t operator-(BYTE_t a)
    {
        BYTE_t ret;
        ret.val = this->val - a.val;
        ret.val_t = this->val_t - a.val_t;
        return ret;
    }

    BYTE_t operator+=(BYTE_t a)
    {
        this->val += a.val;
        this->val_t += a.val_t;
    }

    BYTE_t operator-=(BYTE_t a)
    {
        this->val -= a.val;
        this->val_t -= a.val_t;
    }

    BYTE_t operator*(BYTE a)
    {
        BYTE_t ret;
        ret.val = this->val * a;
        return ret;
    }

    BYTE_t operator/(BYTE a)
    {
        BYTE_t ret;
        ret.val = (DWORD)(this->val / a);
        return ret;
    }

    BYTE_t operator*=(BYTE a)
    {
        this->val *= a;
    }

    BYTE_t operator/=(BYTE a)
    {
        this->val /= a;
    }

    BYTE_t operator*(BYTE_t a)
    {
        BYTE_t ret;
        ret.val = this->val * a.val;
        ret.val_t = this->val_t * a.val_t;
        return ret;
    }

    BYTE_t operator/(BYTE_t a)
    {
        BYTE_t ret;
        ret.val = (DWORD)(this->val / a.val);
        ret.val_t = (DWORD)(this->val_t / a.val_t);
        return ret;
    }

    BYTE_t operator*=(BYTE_t a)
    {
        this->val *= a.val;
        this->val_t *= a.val_t;
    }

    BYTE_t operator/=(BYTE_t a)
    {
        this->val /= a.val;
        this->val_t /= a.val_t;
    }

    BYTE_t operator=(BYTE a)
    {
        this->val = a;
        this->val_t = 0x0;
    }

    BYTE_t operator=(BYTE_t a)
    {
        this->val = a.val;
        this->val_t = a.val_t;
    }

    BYTE_t operator&(BYTE_t a)
    {
        BYTE_t ret;

        ret.val = this->val & a.val;
        ret.val_t = this->val_t | a.val_t; // verify
        
        return ret;
    }

    BYTE_t operator^(BYTE_t a)
    {
        BYTE_t ret;

        ret.val = this->val ^ a.val;
        ret.val_t = this->val_t | a.val_t; //verify
        
        return ret;
    }

    BYTE_t operator|(BYTE_t a)
    {
        BYTE_t ret;

        ret.val = this->val | a.val;
        ret.val_t = this->val_t | a.val_t; 
        
        return ret;
    }

    BYTE_t operator&=(BYTE_t a)
    {
        this->val = this->val & a.val;
        this->val_t = this->val_t | a.val_t; // verify
    }

    BYTE_t operator^=(BYTE_t a)
    {
        this->val = this->val ^ a.val;
        this->val_t = this->val_t | a.val_t; //verify
    }

    BYTE_t operator|=(BYTE_t a)
    {
        this->val = this->val | a.val;
        this->val_t = this->val_t | a.val_t; 
    }

    // shifts & rotates
    BYTE_t operator<<(BYTE_t a)
    {
        BYTE_t ret;

        ret.val << a.val;
        ret.val_t << a.val;

        return ret;
    }

    BYTE_t operator>>(BYTE_t a)
    {
        BYTE_t ret;

        ret.val >> a.val;
        ret.val_t >> a.val;

        return ret;
    }

    BYTE_t operator<<=(BYTE_t a)
    {
        this->val <<= a.val;
        this->val_t <<= a.val;
    }

    BYTE_t operator>>=(BYTE_t a)
    {
        this->val >>= a.val;
        this->val_t >>= a.val;
    }


    BYTE_t operator!()
    {
        this->val = !this->val;
        this->val_t = !this->val_t; //verify
    }

    BYTE_t operator~()
    {
        this->val = -this->val;
        this->val_t = !this->val_t; //verify
    }

    void from_mem(BYTE_t* source)
    {
        val    = source[0].val;
        val_t  = source[0].val_t;
    }

    void to_mem(BYTE_t* target)
    {
        target[0].val   = val;
        target[0].val_t = val_t;
    }
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

        tmp[1] = val[0].val;
        tmp[0] = val[1].val;

        return ret;
    }

    WORD get_t_WORD()
    {
        DWORD ret;

        char* tmp;
        tmp = (char*)&ret;

        tmp[1] = val[0].val_t;
        tmp[0] = val[1].val_t;

        return ret;
    }

    void set_WORD(WORD v)
    {
        char* tmp;
        tmp = (char*)&v;

        val[1].val = tmp[0];
        val[0].val = tmp[1];

        return;
    }

    void set_t_WORD(WORD v_t)
    {
        char* tmp;
        tmp = (char*)&v_t;

        val[1].val_t = tmp[0];
        val[0].val_t = tmp[1];

        return;
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
        this->set_t_WORD(a.get_t_WORD());
/*
        this->val[0] = a.val[0];
        this->val[1] = a.val[1];
*/
    }

    WORD_t operator+(WORD b)
    {
        WORD_t ret;
        WORD v;
        WORD t;

        v = this->get_WORD() + b;
        t = this->get_t_WORD();
        
        ret.set_WORD(v);
        ret.set_t_WORD(t);

        return ret;
    }

    WORD_t operator+(WORD_t b)
    {
        WORD_t ret;
        WORD v;
        WORD t;

        v = this->get_WORD() + b.get_WORD();
        t = this->get_t_WORD() || b.get_t_WORD();
        
        ret.set_WORD(v);
        ret.set_t_WORD(t);

        return ret;
    }

    WORD_t operator+=(WORD b)
    {   
        WORD_t ret;
        WORD v;
        WORD t;

        v = this->get_WORD() + b;
        t = this->get_t_WORD();
        
        ret.set_WORD(v);
        ret.set_t_WORD(t);
        this->set_WORD(v);
        this->set_t_WORD(t);

        return ret;
    }

    WORD_t operator+=(WORD_t b)
    {
        WORD_t ret;
        WORD v;
        WORD t;

        v = this->get_WORD() + b.get_WORD();
        t = this->get_t_WORD() || b.get_t_WORD();
        
        ret.set_WORD(v);
        ret.set_t_WORD(t);
        this->set_WORD(v);
        this->set_t_WORD(t);

        return ret;
    }

    WORD_t operator-(WORD b)
    {
        WORD_t ret;
        WORD v;
        WORD t;

        v = this->get_WORD() - b;
        t = this->get_t_WORD();
        
        ret.set_WORD(v);
        ret.set_t_WORD(t);

        return ret;
    }

    WORD_t operator-(WORD_t b)
    {
        WORD_t ret;
        WORD v;
        WORD t;

        v = this->get_WORD() - b.get_WORD();
        t = this->get_t_WORD() || b.get_t_WORD();
        
        ret.set_WORD(v);
        ret.set_t_WORD(t);

        return ret;
    }

    WORD_t operator-=(WORD b)
    {   
        WORD_t ret;
        WORD v;
        WORD t;

        v = this->get_WORD() - b;
        t = this->get_t_WORD();
        
        ret.set_WORD(v);
        ret.set_t_WORD(t);
        this->set_WORD(v);
        this->set_t_WORD(t);

        return ret;
    }

    WORD_t operator-=(WORD_t b)
    {
        WORD_t ret;
        WORD v;
        WORD t;

        v = this->get_WORD() - b.get_WORD();
        t = this->get_t_WORD() || b.get_t_WORD();
        
        ret.set_WORD(v);
        ret.set_t_WORD(t);
        this->set_WORD(v);
        this->set_t_WORD(t);

        return ret;
    }

    WORD_t operator*(WORD b)
    {
        WORD_t ret;
        WORD v;
        WORD t;

        v = this->get_WORD() * b;
        t = this->get_t_WORD();
        
        ret.set_WORD(v);
        ret.set_t_WORD(t);

        return ret;
    }

    WORD_t operator*(WORD_t b)
    {
        WORD_t ret;
        WORD v;
        WORD t;

        v = this->get_WORD() * b.get_WORD();
        t = this->get_t_WORD() || b.get_t_WORD();
        
        ret.set_WORD(v);
        ret.set_t_WORD(t);

        return ret;
    }

    WORD_t operator*=(WORD b)
    {   
        WORD_t ret;
        WORD v;
        WORD t;

        v = this->get_WORD() * b;
        t = this->get_t_WORD();
        
        ret.set_WORD(v);
        ret.set_t_WORD(t);
        this->set_WORD(v);
        this->set_t_WORD(t);

        return ret;
    }

    WORD_t operator*=(WORD_t b)
    {
        WORD_t ret;
        WORD v;
        WORD t;

        v = this->get_WORD() * b.get_WORD();
        t = this->get_t_WORD() || b.get_t_WORD();
        
        ret.set_WORD(v);
        ret.set_t_WORD(t);
        this->set_WORD(v);
        this->set_t_WORD(t);

        return ret;
    }

    WORD_t operator/(WORD b)
    {
        WORD_t ret;
        WORD v;
        WORD t;

        v = this->get_WORD() / b;
        t = this->get_t_WORD();
        
        ret.set_WORD(v);
        ret.set_t_WORD(t);

        return ret;
    }

    WORD_t operator/(WORD_t b)
    {
        WORD_t ret;
        WORD v;
        WORD t;

        v = this->get_WORD() / b.get_WORD();
        t = this->get_t_WORD() || b.get_t_WORD();
        
        ret.set_WORD(v);
        ret.set_t_WORD(t);

        return ret;
    }

    WORD_t operator/=(WORD b)
    {   
        WORD_t ret;
        WORD v;
        WORD t;

        v = this->get_WORD() / b;
        t = this->get_t_WORD();
        
        ret.set_WORD(v);
        ret.set_t_WORD(t);
        this->set_WORD(v);
        this->set_t_WORD(t);

        return ret;
    }

    WORD_t operator/=(WORD_t b)
    {
        WORD_t ret;
        WORD v;
        WORD t;

        v = this->get_WORD() / b.get_WORD();
        t = this->get_t_WORD() || b.get_t_WORD();
        
        ret.set_WORD(v);
        ret.set_t_WORD(t);
        this->set_WORD(v);
        this->set_t_WORD(t);

        return ret;
    }

    WORD_t operator&(WORD_t a)
    {
        WORD_t ret;
        ret.val[0].val = this->val[0].val & a[0].val;
        ret.val[1].val = this->val[1].val & a[1].val;

        ret.val[0].val_t = this->val[0].val_t | a[0].val_t; // verify
        ret.val[1].val_t = this->val[1].val_t | a[1].val_t; // verify
        return ret;
    }

    WORD_t operator^(WORD_t a)
    {
        WORD_t ret;
        ret.val[0].val = this->val[0].val ^ a[0].val;
        ret.val[1].val = this->val[1].val ^ a[1].val;

        ret.val[0].val_t = this->val[0].val_t | a[0].val_t; // verify
        ret.val[1].val_t = this->val[1].val_t | a[1].val_t; // verify
        return ret;
    }

    WORD_t operator|(WORD_t a)
    {
        WORD_t ret;
        ret.val[0].val = this->val[0].val | a[0].val;
        ret.val[1].val = this->val[1].val | a[1].val;

        ret.val[0].val_t = this->val[0].val_t | a[0].val_t; // verify
        ret.val[1].val_t = this->val[1].val_t | a[1].val_t; // verify
        return ret;
    }

    WORD_t operator&=(WORD_t a)
    {
        this->val[0].val &= a[0].val;
        this->val[1].val &= a[1].val;

        this->val[0].val_t |= a[0].val_t; // verify
        this->val[1].val_t |= a[1].val_t; // verify
    }

    WORD_t operator^=(WORD_t a)
    {
        this->val[0].val ^= a[0].val;
        this->val[1].val ^= a[1].val;


        this->val[0].val_t |= a[0].val_t; // verify
        this->val[1].val_t |= a[1].val_t; // verify
    }

    WORD_t operator|=(WORD_t a)
    {
        this->val[0].val |= a[0].val;
        this->val[1].val |= a[1].val;

        this->val[0].val_t |= a[0].val_t; // verify
        this->val[1].val_t |= a[1].val_t; // verify
    }

    // shifts & rotates
    WORD_t operator<<(BYTE_t a)
    {
        WORD_t ret;
        WORD temp_16;

        temp_16 = this->get_WORD();
        temp_16 << a.val;
        ret.set_WORD(temp_16);

        temp_16 = this->get_t_WORD();
        temp_16 << a.val;
        ret.set_t_WORD(temp_16);

        return ret;
    }

    WORD_t operator>>(BYTE_t a)
    {
        WORD_t ret;
        WORD temp_16;

        temp_16 = this->get_WORD();
        temp_16 >> a.val;
        ret.set_WORD(temp_16);

        temp_16 = this->get_t_WORD();
        temp_16 >> a.val;
        ret.set_t_WORD(temp_16);

        return ret;
    }

    WORD_t operator<<=(BYTE_t a)
    {
        WORD temp_16;

        temp_16 = this->get_WORD();
        temp_16 <<= a.val;
        this->set_WORD(temp_16);

        temp_16 = this->get_t_WORD();
        temp_16 <<= a.val;
        this->set_t_WORD(temp_16);
    }

    WORD_t operator>>=(BYTE_t a)
    {
        WORD temp_16;

        temp_16 = this->get_WORD();
        temp_16 >>= a.val;
        this->set_WORD(temp_16);

        temp_16 = this->get_t_WORD();
        temp_16 >>= a.val;
        this->set_t_WORD(temp_16);
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
        ret.set_WORD(-this->get_WORD());
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
        val[1] = a;
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
            val[0].val      = source[0].val;
            val[0].val_t    = source[0].val_t;
            val[1].val      = source[1].val;
            val[1].val_t    = source[1].val_t;

        }
        else
        {
            val[1].val      = source[0].val;
            val[1].val_t    = source[0].val_t;
            val[0].val      = source[1].val;
            val[0].val_t    = source[1].val_t;

        }
    }

    void to_mem(BYTE_t* target, int le=1)
    {
        if(!le)
        {
            target[0].val   = val[0].val;
            target[0].val_t = val[0].val_t;
            target[1].val   = val[1].val;
            target[1].val_t = val[1].val_t;
        }
        else
        {
            target[1].val   = val[0].val;
            target[1].val_t = val[0].val_t;
            target[0].val   = val[1].val;
            target[0].val_t = val[1].val_t;
        }
    }

};

class DWORD_t
{
    public:

    BYTE_t val[4];
    
    /* relations to WORD_t */
    
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

        tmp[0] = val[3].val;
        tmp[1] = val[2].val;
        tmp[2] = val[1].val;
        tmp[3] = val[0].val;

        return ret;
    }

    DWORD get_t_DWORD()
    {
        DWORD ret;

        char* tmp;
        tmp = (char*)&ret;

        tmp[0] = val[3].val_t;
        tmp[1] = val[2].val_t;
        tmp[2] = val[1].val_t;
        tmp[3] = val[0].val_t;

        return ret;
    }

    void set_DWORD(DWORD v)
    {
        char* tmp;
        tmp = (char*)&v;

        val[0].val = tmp[3];
        val[1].val = tmp[2];
        val[2].val = tmp[1];
        val[3].val = tmp[0];

        return;
    }

    void set_t_DWORD(DWORD v_t)
    {
        char* tmp;
        tmp = (char*)&v_t;

        val[0].val_t = tmp[3];
        val[1].val_t = tmp[2];
        val[2].val_t = tmp[1];
        val[3].val_t = tmp[0];

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
        t = this->get_t_DWORD();
        
        ret.set_DWORD(v);
        ret.set_t_DWORD(t);

        return ret;
    }

    DWORD_t operator+(DWORD_t b)
    {
        DWORD_t ret;
        DWORD v;
        DWORD t;

        v = this->get_DWORD() + b.get_DWORD();
        t = this->get_t_DWORD() || b.get_t_DWORD();
        
        ret.set_DWORD(v);
        ret.set_t_DWORD(t);

        return ret;
    }

    DWORD_t operator+=(DWORD b)
    {   
        DWORD_t ret;
        DWORD v;
        DWORD t;

        v = this->get_DWORD() + b;
        t = this->get_t_DWORD();
        
        ret.set_DWORD(v);
        ret.set_t_DWORD(t);
        this->set_DWORD(v);
        this->set_t_DWORD(t);

        return ret;
    }

    DWORD_t operator+=(DWORD_t b)
    {
        DWORD_t ret;
        DWORD v;
        DWORD t;

        v = this->get_DWORD() + b.get_DWORD();
        t = this->get_t_DWORD() || b.get_t_DWORD();
        
        ret.set_DWORD(v);
        ret.set_t_DWORD(t);
        this->set_DWORD(v);
        this->set_t_DWORD(t);

        return ret;
    }

    DWORD_t operator-(DWORD b)
    {
        DWORD_t ret;
        DWORD v;
        DWORD t;

        v = this->get_DWORD() - b;
        t = this->get_t_DWORD();
        
        ret.set_DWORD(v);
        ret.set_t_DWORD(t);

        return ret;
    }

    DWORD_t operator-(DWORD_t b)
    {
        DWORD_t ret;
        DWORD v;
        DWORD t;

        v = this->get_DWORD() - b.get_DWORD();
        t = this->get_t_DWORD() || b.get_t_DWORD();
        
        ret.set_DWORD(v);
        ret.set_t_DWORD(t);

        return ret;
    }

    DWORD_t operator-=(BYTE b)
    {   
        DWORD_t ret;
        DWORD v;
        DWORD t;

        v = this->get_DWORD() - b;
        t = this->get_t_DWORD();
        
        ret.set_DWORD(v);
        ret.set_t_DWORD(t);
        this->set_DWORD(v);
        this->set_t_DWORD(t);

        return ret;
    }

    DWORD_t operator-=(BYTE_t b)
    {
        DWORD_t ret;
        DWORD v;
        DWORD t;

        v = this->get_DWORD() - b.val;
        t = this->get_t_DWORD() || b.val_t;
        
        ret.set_DWORD(v);
        ret.set_t_DWORD(t);
        this->set_DWORD(v);
        this->set_t_DWORD(t);

        return ret;
    }

    DWORD_t operator-=(DWORD b)
    {   
        DWORD_t ret;
        DWORD v;
        DWORD t;

        v = this->get_DWORD() - b;
        t = this->get_t_DWORD();
        
        ret.set_DWORD(v);
        ret.set_t_DWORD(t);
        this->set_DWORD(v);
        this->set_t_DWORD(t);

        return ret;
    }

    DWORD_t operator-=(DWORD_t b)
    {
        DWORD_t ret;
        DWORD v;
        DWORD t;

        v = this->get_DWORD() - b.get_DWORD();
        t = this->get_t_DWORD() || b.get_t_DWORD();
        
        ret.set_DWORD(v);
        ret.set_t_DWORD(t);
        this->set_DWORD(v);
        this->set_t_DWORD(t);

        return ret;
    }

    DWORD_t operator*(DWORD b)
    {
        DWORD_t ret;
        DWORD v;
        DWORD t;

        v = this->get_DWORD() * b;
        t = this->get_t_DWORD();
        
        ret.set_DWORD(v);
        ret.set_t_DWORD(t);

        return ret;
    }

    DWORD_t operator*(DWORD_t b)
    {
        DWORD_t ret;
        DWORD v;
        DWORD t;

        v = this->get_DWORD() * b.get_DWORD();
        t = this->get_t_DWORD() || b.get_t_DWORD();
        
        ret.set_DWORD(v);
        ret.set_t_DWORD(t);

        return ret;
    }

    DWORD_t operator*=(DWORD b)
    {   
        DWORD_t ret;
        DWORD v;
        DWORD t;

        v = this->get_DWORD() * b;
        t = this->get_t_DWORD();
        
        ret.set_DWORD(v);
        ret.set_t_DWORD(t);
        this->set_DWORD(v);
        this->set_t_DWORD(t);

        return ret;
    }

    DWORD_t operator*=(DWORD_t b)
    {
        DWORD_t ret;
        DWORD v;
        DWORD t;

        v = this->get_DWORD() * b.get_DWORD();
        t = this->get_t_DWORD() || b.get_t_DWORD();
        
        ret.set_DWORD(v);
        ret.set_t_DWORD(t);
        this->set_DWORD(v);
        this->set_t_DWORD(t);

        return ret;
    }

    DWORD_t operator/(DWORD b)
    {
        DWORD_t ret;
        DWORD v;
        DWORD t;

        v = (DWORD)(this->get_DWORD() / b);
        t = this->get_t_DWORD();
        
        ret.set_DWORD(v);
        ret.set_t_DWORD(t);

        return ret;
    }

    DWORD_t operator/(DWORD_t b)
    {
        DWORD_t ret;
        DWORD v;
        DWORD t;

        v = (DWORD)(this->get_DWORD() / b.get_DWORD());
        t = this->get_t_DWORD() || b.get_t_DWORD();
        
        ret.set_DWORD(v);
        ret.set_t_DWORD(t);

        return ret;
    }

    DWORD_t operator/=(BYTE b)
    {   
        DWORD_t ret;
        DWORD v;
        DWORD t;

        v = (DWORD)(this->get_DWORD() / b);
        t = this->get_t_DWORD();
        
        ret.set_DWORD(v);
        ret.set_t_DWORD(t);
        this->set_DWORD(v);
        this->set_t_DWORD(t);

        return ret;
    }

    DWORD_t operator/=(BYTE_t b)
    {
        DWORD_t ret;
        DWORD v;
        DWORD t;

        v = (DWORD)(this->get_DWORD() / b.val);
        t = this->get_t_DWORD() || b.val_t;
        
        ret.set_DWORD(v);
        ret.set_t_DWORD(t);
        this->set_DWORD(v);
        this->set_t_DWORD(t);

        return ret;
    }

    DWORD_t operator/=(DWORD b)
    {   
        DWORD_t ret;
        DWORD v;
        DWORD t;

        v = (DWORD)(this->get_DWORD() / b);
        t = this->get_t_DWORD();
        
        ret.set_DWORD(v);
        ret.set_t_DWORD(t);
        this->set_DWORD(v);
        this->set_t_DWORD(t);

        return ret;
    }

    DWORD_t operator/=(DWORD_t b)
    {
        DWORD_t ret;
        DWORD v;
        DWORD t;

        v = (DWORD)(this->get_DWORD() / b.get_DWORD());
        t = this->get_t_DWORD() || b.get_t_DWORD();
        
        ret.set_DWORD(v);
        ret.set_t_DWORD(t);
        this->set_DWORD(v);
        this->set_t_DWORD(t);

        return ret;
    }

    DWORD_t operator&(DWORD_t a)
    {
        DWORD_t ret;
        ret.val[0].val = this->val[0].val & a[0].val;
        ret.val[1].val = this->val[1].val & a[1].val;
        ret.val[2].val = this->val[2].val & a[2].val;
        ret.val[3].val = this->val[3].val & a[3].val;

        ret.val[0].val_t = this->val[0].val_t | a[0].val_t; // verify
        ret.val[1].val_t = this->val[1].val_t | a[1].val_t; // verify
        ret.val[2].val_t = this->val[2].val_t | a[2].val_t; // verify
        ret.val[3].val_t = this->val[3].val_t | a[3].val_t; // verify
        return ret;
    }

    DWORD_t operator^(DWORD_t a)
    {
        DWORD_t ret;
        ret.val[0].val = this->val[0].val ^ a[0].val;
        ret.val[1].val = this->val[1].val ^ a[1].val;
        ret.val[2].val = this->val[2].val ^ a[2].val;
        ret.val[3].val = this->val[3].val ^ a[3].val;

        ret.val[0].val_t = this->val[0].val_t | a[0].val_t; // verify
        ret.val[1].val_t = this->val[1].val_t | a[1].val_t; // verify
        ret.val[2].val_t = this->val[2].val_t | a[2].val_t; // verify
        ret.val[3].val_t = this->val[3].val_t | a[3].val_t; // verify
        return ret;
    }

    DWORD_t operator|(DWORD_t a)
    {
        DWORD_t ret;
        ret.val[0].val = this->val[0].val | a[0].val;
        ret.val[1].val = this->val[1].val | a[1].val;
        ret.val[2].val = this->val[2].val | a[2].val;
        ret.val[3].val = this->val[3].val | a[3].val;

        ret.val[0].val_t = this->val[0].val_t | a[0].val_t; // verify
        ret.val[1].val_t = this->val[1].val_t | a[1].val_t; // verify
        ret.val[2].val_t = this->val[2].val_t | a[2].val_t; // verify
        ret.val[3].val_t = this->val[3].val_t | a[3].val_t; // verify
        return ret;
    }

    DWORD_t operator&=(DWORD_t a)
    {
        this->val[0].val &= a[0].val;
        this->val[1].val &= a[1].val;
        this->val[2].val &= a[2].val;
        this->val[3].val &= a[3].val;

        this->val[0].val_t |= a[0].val_t; // verify
        this->val[1].val_t |= a[1].val_t; // verify
        this->val[2].val_t |= a[2].val_t; // verify
        this->val[3].val_t |= a[3].val_t; // verify
    }

    DWORD_t operator^=(DWORD_t a)
    {
        this->val[0].val ^= a[0].val;
        this->val[1].val ^= a[1].val;
        this->val[2].val ^= a[2].val;
        this->val[3].val ^= a[3].val;


        this->val[0].val_t |= a[0].val_t; // verify
        this->val[1].val_t |= a[1].val_t; // verify
        this->val[2].val_t |= a[2].val_t; // verify
        this->val[3].val_t |= a[3].val_t; // verify
    }

    DWORD_t operator|=(DWORD_t a)
    {
        this->val[0].val |= a[0].val;
        this->val[1].val |= a[1].val;
        this->val[2].val |= a[2].val;
        this->val[3].val |= a[3].val;

        this->val[0].val_t |= a[0].val_t; // verify
        this->val[1].val_t |= a[1].val_t; // verify
        this->val[2].val_t |= a[2].val_t; // verify
        this->val[3].val_t |= a[3].val_t; // verify
    }

    // shifts & rotates
    DWORD_t operator<<(BYTE_t a)
    {
        DWORD_t ret;
        DWORD temp_32;

        temp_32 = this->get_DWORD();
        temp_32 << a.val;
        ret.set_DWORD(temp_32);

        temp_32 = this->get_t_DWORD();
        temp_32 << a.val;
        ret.set_t_DWORD(temp_32);

        return ret;
    }

    DWORD_t operator>>(BYTE_t a)
    {
        DWORD_t ret;
        DWORD temp_32;

        temp_32 = this->get_DWORD();
        temp_32 >> a.val;
        ret.set_DWORD(temp_32);

        temp_32 = this->get_t_DWORD();
        temp_32 >> a.val;
        ret.set_t_DWORD(temp_32);

        return ret;
    }

    DWORD_t operator<<=(BYTE_t a)
    {
        DWORD temp_32;

        temp_32 = this->get_DWORD();
        temp_32 <<= a.val;
        this->set_DWORD(temp_32);

        temp_32 = this->get_t_DWORD();
        temp_32 <<= a.val;
        this->set_t_DWORD(temp_32);
    }

    DWORD_t operator>>=(BYTE_t a)
    {
        DWORD temp_32;

        temp_32 = this->get_DWORD();
        temp_32 >>= a.val;
        this->set_DWORD(temp_32);

        temp_32 = this->get_t_DWORD();
        temp_32 >>= a.val;
        this->set_t_DWORD(temp_32);
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
        ret.set_DWORD(-this->get_DWORD());
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
        val[3] = a;
    }

    //from WORD, proxy to WORD constructor
    DWORD_t(WORD a)
    {
        this->set_DWORD(DWORD(a));
    }

    //from WORD_t, proxy to WORD_t constructor
    DWORD_t(WORD_t a)
    {
        val[2] = a.val[0];
        val[3] = a.val[1];
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
            val[0].val      = source[0].val;
            val[0].val_t    = source[0].val_t;
            val[1].val      = source[1].val;
            val[1].val_t    = source[1].val_t;
            val[2].val      = source[2].val;
            val[2].val_t    = source[2].val_t;
            val[3].val      = source[3].val;
            val[3].val_t    = source[3].val_t;
        }
        else
        {
            val[3].val      = source[0].val;
            val[3].val_t    = source[0].val_t;
            val[2].val      = source[1].val;
            val[2].val_t    = source[1].val_t;
            val[1].val      = source[2].val;
            val[1].val_t    = source[2].val_t;
            val[0].val      = source[3].val;
            val[0].val_t    = source[3].val_t;
        }
    }

    void to_mem(BYTE_t* target, int le=1)
    {
        if(!le)
        {
            target[0].val   = val[0].val;
            target[0].val_t = val[0].val_t;
            target[1].val   = val[1].val;
            target[1].val_t = val[1].val_t;
            target[2].val   = val[2].val;
            target[2].val_t = val[2].val_t;
            target[3].val   = val[3].val;
            target[3].val_t = val[3].val_t;
        }
        else
        {
            target[3].val   = val[0].val;
            target[3].val_t = val[0].val_t;
            target[2].val   = val[1].val;
            target[2].val_t = val[1].val_t;
            target[1].val   = val[2].val;
            target[1].val_t = val[2].val_t;
            target[0].val   = val[3].val;
            target[0].val_t = val[3].val_t;
        }
    }

};

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

typedef struct _LIB_INFO
{
    OFFSET offset;
    char name[MAX_NAME];
    DWORD loaded;
} LIB_INFO;

typedef struct _EXCEPTION_INFO
{
    EXCEPTION_RECORD er;
    DWORD tid;
} EXCEPTION_INFO;

typedef struct _CONTEXT_INFO
{
    DWORD tid;
    BYTE_t registers[REG_SIZE];
    OFFSET seg_map[0x6];
} CONTEXT_INFO;

//deprecated
/*
typedef struct modrm_op_ready_32_
{
    DWORD_t* src;
    DWORD_t* tgt;
} modrm_op_ready_32;
*/

class taint_x86
{
    public:
    FILE* dump_file;
    FILE* instr_file;
    FILE* lost_file;
    BYTE_t* memory;
    DWORD mem_length;
    int current_prefixes;
    BYTE_t* current_instr_byte;
    BYTE current_instr_length;
    DWORD my_bps[0x10];
    CONTEXT_INFO ctx_info[MAX_THREADS];
    CONTEXT_INFO* cur_info;

    DWORD tids[0x1000];
    DWORD tid_count;
    DWORD cur_tid;
    DWORD bp_hit;
    DWORD extended;

    LIB_INFO libs[0x100];
    DWORD libs_count;

    EXCEPTION_INFO exceptions[0x100];
    DWORD exceptions_count;

    int modrm_table_32[0x100];

    typedef int (taint_x86::*instruction_routine)(BYTE_t* args);
    instruction_routine instructions_32[0x100];
    instruction_routine instructions_32_extended[0x100];
    instruction_routine escaped_instructions_32[0x10000];

    // instruction routines
    int r_noop(BYTE_t*);
    int r_push_eax_32(BYTE_t*);
    int r_push_ecx_32(BYTE_t*);
    int r_push_edx_32(BYTE_t*);
    int r_push_ebx_32(BYTE_t*);
    int r_push_esp_32(BYTE_t*);
    int r_push_ebp_32(BYTE_t*);
    int r_push_esi_32(BYTE_t*);
    int r_push_edi_32(BYTE_t*);
    int r_pop_eax_32(BYTE_t*);
    int r_pop_ecx_32(BYTE_t*);
    int r_pop_edx_32(BYTE_t*);
    int r_pop_ebx_32(BYTE_t*);
    int r_pop_esp_32(BYTE_t*);
    int r_pop_ebp_32(BYTE_t*);
    int r_pop_esi_32(BYTE_t*);
    int r_pop_edi_32(BYTE_t*);
    int r_push_imm_8(BYTE_t*);
    int r_push_imm_16_32(BYTE_t*);
    int r_push_rm_16_32(BYTE_t*);

    int r_lea(BYTE_t*);

    int r_mov_r_rm_32(BYTE_t*);
    int r_mov_r_rm_8(BYTE_t*);
    int r_mov_rm_r_32(BYTE_t*);
    int r_mov_rm_r_8(BYTE_t*);
    int r_mov_rm_imm_16_32(BYTE_t*);
    int r_mov_imm_rm_32(BYTE_t*);

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

    int r_mov_ax_eax_moffset_16_32(BYTE_t*);

    int r_inc_eax_32(BYTE_t*);
    int r_inc_ecx_32(BYTE_t*);
    int r_inc_edx_32(BYTE_t*);
    int r_inc_ebx_32(BYTE_t*);
    int r_inc_esp_32(BYTE_t*);
    int r_inc_ebp_32(BYTE_t*);
    int r_inc_esi_32(BYTE_t*);
    int r_inc_edi_32(BYTE_t*);

    int r_dec_eax_32(BYTE_t*);
    int r_dec_ecx_32(BYTE_t*);
    int r_dec_edx_32(BYTE_t*);
    int r_dec_ebx_32(BYTE_t*);
    int r_dec_esp_32(BYTE_t*);
    int r_dec_ebp_32(BYTE_t*);
    int r_dec_esi_32(BYTE_t*);
    int r_dec_edi_32(BYTE_t*);

    // add, sub, mul & div
    // add    

    int r_add_rm_imm_16_32(BYTE_t*);
    int r_add_rm_16_32_imm_8(BYTE_t*);
    int r_add_al_imm_8(BYTE_t*);
    int r_add_ax_eax_imm_16_32(BYTE_t*);
    int r_add_rm_imm8(BYTE_t*);
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

    // div
    int r_div_rm_8(BYTE_t*);
    int r_div_rm_16_32(BYTE_t*);

    // xor ops
    int r_xor_r_rm(BYTE_t*);
    int r_xor_rm_r(BYTE_t*);
    int r_xor_al_imm_8(BYTE_t*);
    int r_xor_ax_imm_16(BYTE_t*);
    int r_xor_eax_imm_32(BYTE_t*);

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
    int r_sar_shr_rm_16_32_1(BYTE_t*);
    int r_sal_shl_rm_8_cl(BYTE_t*);
    int r_shr_rm_8_cl(BYTE_t*);
    int r_sar_rm_8_cl(BYTE_t*);
    int r_sal_shl_rm_16_32_cl(BYTE_t*);
    int r_shr_rm_16_32_cl(BYTE_t*);
    int r_sar_rm_16_32_cl(BYTE_t*);

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

    // neg & not
    int r_neg_rm_8(BYTE_t*);
    int r_neg_rm_16_32(BYTE_t*);
    int r_not_rm_8(BYTE_t*);
    int r_not_rm_16_32(BYTE_t*);

    // ret
    int r_retn(BYTE_t*);
    int r_ret(BYTE_t*);

    // call
    int r_call_rel(BYTE_t*);
    int r_call_abs_near(BYTE_t*);
    int r_call_abs_far(BYTE_t*);

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
    int r_decode_execute_c7(BYTE_t*);
    int r_decode_execute_d0(BYTE_t*);
    int r_decode_execute_d1(BYTE_t*);
    int r_decode_execute_d2(BYTE_t*);
    int r_decode_execute_d3(BYTE_t*);
    int r_decode_execute_c0(BYTE_t*);
    int r_decode_execute_c1(BYTE_t*);
    int r_decode_execute_f6(BYTE_t*);
    int r_decode_execute_f7(BYTE_t*);
    int r_decode_execute_ff(BYTE_t*);
   
    // extended
    int r_movzx_r_32_rm_16(BYTE_t*);
    int r_movzx_r_16_32_rm_8(BYTE_t*);
 
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
    int a_calculate_instr_length(BYTE_t*);
    int a_is_prefix(BYTE_t*);
    int a_decode_modrm(BYTE_t*, modrm_ptr*, modrm_ptr*, int, int);
    OFFSET a_decode_sib(BYTE_t*);
    OFFSET a_decode_sib_mod(BYTE_t*, BYTE);
    OFFSET a_offset_recalculate_16(OFFSET);
    OFFSET a_offset_recalculate_8(OFFSET);
    int a_push_lost_32(DWORD);
    int a_push_lost_16(WORD);
    int a_push_lost_8(BYTE);
    DWORD a_pop_lost();

    // other
    DWORD le2dword(char*);
    CONTEXT_INFO* get_tid(DWORD);
    int already_added(DWORD);
    int add_thread(CONTEXT_info);
    int del_thread(DWORD);
    int add_taint(OFFSET start, DWORD length);
    int add_lib(OFFSET, char*);
    int del_lib(OFFSET);
    int add_exception(EXCEPTION_INFO);
    int load_mem_from_file(char*);
    int load_instr_from_file(char*);
    int open_lost_file(char*);
    int close_files();

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
    int post_execute_instruction(DWORD);
    int execute_instruction_at_eip(DWORD);
    int execute_instruction_at_eip(DWORD, DWORD);
    int print_context(int);
    int print_t_context(int);
    int print_context();
    int print_t_context();
    int print_all_contexts();
    int print_all_t_contexts();
    int print_stack(DWORD);
    int print_t_stack(DWORD);
    int print_mem(OFFSET, DWORD);
    int print_t_mem(OFFSET, DWORD);
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
        this->instructions_32[0x00] = &taint_x86::r_add_rm_r_8;
        this->instructions_32[0x01] = &taint_x86::r_add_rm_r_16_32;
        this->instructions_32[0x02] = &taint_x86::r_add_r_rm_8;
        this->instructions_32[0x03] = &taint_x86::r_add_r_rm_16_32;

        this->instructions_32[0x04] = &taint_x86::r_add_al_imm_8;
        this->instructions_32[0x05] = &taint_x86::r_add_ax_eax_imm_16_32;

        this->instructions_32[0x08] = &taint_x86::r_or_rm_r_8;
        this->instructions_32[0x09] = &taint_x86::r_or_rm_r_16_32;
        this->instructions_32[0x0a] = &taint_x86::r_or_r_rm_8;
        this->instructions_32[0x0b] = &taint_x86::r_or_r_rm_16_32;
        this->instructions_32[0x0c] = &taint_x86::r_or_al_imm_8;
        this->instructions_32[0x0d] = &taint_x86::r_or_ax_eax_imm_16_32;
        this->instructions_32[0x0f] = &taint_x86::r_extend;

        this->instructions_32[0x18] = &taint_x86::r_sbb_rm_r_8;
        this->instructions_32[0x19] = &taint_x86::r_sbb_rm_r_16_32;
        this->instructions_32[0x1a] = &taint_x86::r_sbb_r_rm_8;
        this->instructions_32[0x1b] = &taint_x86::r_sbb_r_rm_16_32;
        this->instructions_32[0x1c] = &taint_x86::r_sbb_al_imm_8;
        this->instructions_32[0x1d] = &taint_x86::r_sbb_ax_eax_imm_16_32;


        this->instructions_32[0x20] = &taint_x86::r_and_rm_r_8;
        this->instructions_32[0x21] = &taint_x86::r_and_rm_r_16_32;
        this->instructions_32[0x22] = &taint_x86::r_and_r_rm_8;
        this->instructions_32[0x23] = &taint_x86::r_and_r_rm_16_32;

        this->instructions_32[0x24] = &taint_x86::r_and_al_imm_8;
        this->instructions_32[0x25] = &taint_x86::r_and_ax_eax_imm_16_32;

        this->instructions_32[0x26] = &taint_x86::r_es_override;

        this->instructions_32[0x28] = &taint_x86::r_sub_rm_r_8;
        this->instructions_32[0x29] = &taint_x86::r_sub_rm_r_16_32;
        this->instructions_32[0x2a] = &taint_x86::r_sub_r_rm_8;
        this->instructions_32[0x2b] = &taint_x86::r_sub_r_rm_16_32;
        this->instructions_32[0x2c] = &taint_x86::r_sub_al_imm_8;
        this->instructions_32[0x2d] = &taint_x86::r_sub_ax_eax_imm_16_32;


        this->instructions_32[0x31] = &taint_x86::r_xor_rm_r;

        this->instructions_32[0x33] = &taint_x86::r_xor_r_rm;
        this->instructions_32[0x34] = &taint_x86::r_xor_al_imm_8;
        this->instructions_32[0x35] = &taint_x86::r_xor_ax_imm_16;
        this->instructions_32[0x36] = &taint_x86::r_xor_eax_imm_32;


        this->instructions_32[0x36] = &taint_x86::r_ss_override;
        this->instructions_32[0x2e] = &taint_x86::r_cs_override;
        this->instructions_32[0x3e] = &taint_x86::r_ds_override;
        
        this->instructions_32[0x40] = &taint_x86::r_inc_eax_32;
        this->instructions_32[0x41] = &taint_x86::r_inc_ecx_32;
        this->instructions_32[0x42] = &taint_x86::r_inc_edx_32;
        this->instructions_32[0x43] = &taint_x86::r_inc_ebx_32;
        this->instructions_32[0x44] = &taint_x86::r_inc_esp_32;
        this->instructions_32[0x45] = &taint_x86::r_inc_ebp_32;
        this->instructions_32[0x46] = &taint_x86::r_inc_esi_32;
        this->instructions_32[0x47] = &taint_x86::r_inc_edi_32;

        this->instructions_32[0x48] = &taint_x86::r_dec_eax_32;
        this->instructions_32[0x49] = &taint_x86::r_dec_ecx_32;
        this->instructions_32[0x4a] = &taint_x86::r_dec_edx_32;
        this->instructions_32[0x4b] = &taint_x86::r_dec_ebx_32;
        this->instructions_32[0x4c] = &taint_x86::r_dec_esp_32;
        this->instructions_32[0x4d] = &taint_x86::r_dec_ebp_32;
        this->instructions_32[0x4e] = &taint_x86::r_dec_esi_32;
        this->instructions_32[0x4f] = &taint_x86::r_dec_edi_32;

        this->instructions_32[0x50] = &taint_x86::r_push_eax_32;
        this->instructions_32[0x51] = &taint_x86::r_push_ecx_32;
        this->instructions_32[0x52] = &taint_x86::r_push_edx_32;
        this->instructions_32[0x53] = &taint_x86::r_push_ebx_32;
        this->instructions_32[0x54] = &taint_x86::r_push_esp_32;
        this->instructions_32[0x55] = &taint_x86::r_push_ebp_32;
        this->instructions_32[0x56] = &taint_x86::r_push_esi_32;
        this->instructions_32[0x57] = &taint_x86::r_push_edi_32;
        this->instructions_32[0x58] = &taint_x86::r_pop_eax_32;
        this->instructions_32[0x59] = &taint_x86::r_pop_ecx_32;
        this->instructions_32[0x5a] = &taint_x86::r_pop_edx_32;
        this->instructions_32[0x5b] = &taint_x86::r_pop_ebx_32;
        this->instructions_32[0x5c] = &taint_x86::r_pop_esp_32;
        this->instructions_32[0x5d] = &taint_x86::r_pop_ebp_32;
        this->instructions_32[0x5e] = &taint_x86::r_pop_esi_32;
        this->instructions_32[0x5f] = &taint_x86::r_pop_edi_32;

        this->instructions_32[0x64] = &taint_x86::r_fs_override;
        this->instructions_32[0x65] = &taint_x86::r_gs_override;
        this->instructions_32[0x66] = &taint_x86::r_oper_size_override;
        this->instructions_32[0x67] = &taint_x86::r_addr_size_override;

        this->instructions_32[0x68] = &taint_x86::r_push_imm_16_32;
        this->instructions_32[0x6a] = &taint_x86::r_push_imm_8;

        this->instructions_32[0x80] = &taint_x86::r_decode_execute_80;
        this->instructions_32[0x81] = &taint_x86::r_decode_execute_81;
        this->instructions_32[0x83] = &taint_x86::r_decode_execute_83;

        this->instructions_32[0x88] = &taint_x86::r_mov_rm_r_8;
        this->instructions_32[0x89] = &taint_x86::r_mov_rm_r_32;
        this->instructions_32[0x8a] = &taint_x86::r_mov_r_rm_8;
        this->instructions_32[0x8b] = &taint_x86::r_mov_r_rm_32;
        this->instructions_32[0x8d] = &taint_x86::r_lea;

        
        this->instructions_32[0xa1] = &taint_x86::r_mov_ax_eax_moffset_16_32;

        this->instructions_32[0xb0] = &taint_x86::r_mov_al_imm8;
        this->instructions_32[0xb1] = &taint_x86::r_mov_cl_imm8;
        this->instructions_32[0xb2] = &taint_x86::r_mov_dl_imm8;
        this->instructions_32[0xb3] = &taint_x86::r_mov_bl_imm8;
        this->instructions_32[0xb4] = &taint_x86::r_mov_ah_imm8;
        this->instructions_32[0xb5] = &taint_x86::r_mov_ch_imm8;
        this->instructions_32[0xb6] = &taint_x86::r_mov_dh_imm8;
        this->instructions_32[0xb7] = &taint_x86::r_mov_bh_imm8;

        this->instructions_32[0xb8] = &taint_x86::r_mov_eax_imm_16_32;
        this->instructions_32[0xb9] = &taint_x86::r_mov_ecx_imm_16_32;
        this->instructions_32[0xba] = &taint_x86::r_mov_edx_imm_16_32;
        this->instructions_32[0xbb] = &taint_x86::r_mov_ebx_imm_16_32;
        this->instructions_32[0xbc] = &taint_x86::r_mov_esp_imm_16_32;
        this->instructions_32[0xbd] = &taint_x86::r_mov_ebp_imm_16_32;
        this->instructions_32[0xbe] = &taint_x86::r_mov_esi_imm_16_32;
        this->instructions_32[0xbf] = &taint_x86::r_mov_edi_imm_16_32;

        this->instructions_32[0xc0] = &taint_x86::r_decode_execute_c0;
        this->instructions_32[0xc1] = &taint_x86::r_decode_execute_c1;
        this->instructions_32[0xc2] = &taint_x86::r_retn;
        this->instructions_32[0xc3] = &taint_x86::r_ret;

        this->instructions_32[0xc7] = &taint_x86::r_decode_execute_c7;
        this->instructions_32[0xd0] = &taint_x86::r_decode_execute_d0;
        this->instructions_32[0xd1] = &taint_x86::r_decode_execute_d1;
        this->instructions_32[0xd2] = &taint_x86::r_decode_execute_d2;
        this->instructions_32[0xd3] = &taint_x86::r_decode_execute_d3;

        this->instructions_32[0xe8] = &taint_x86::r_call_rel;

        this->instructions_32[0xf0] = &taint_x86::r_lock;
        this->instructions_32[0xf2] = &taint_x86::r_repne;
        this->instructions_32[0xf3] = &taint_x86::r_rep;

        this->instructions_32[0xf6] = &taint_x86::r_decode_execute_f6;
        this->instructions_32[0xf7] = &taint_x86::r_decode_execute_f7;
        this->instructions_32[0xff] = &taint_x86::r_decode_execute_ff;

        //extended instructions
        // ...

        this->instructions_32_extended[0xb6] = &taint_x86::r_movzx_r_16_32_rm_8;
        this->instructions_32_extended[0xb7] = &taint_x86::r_movzx_r_32_rm_16;

        this->cur_tid = 0x0;
        this->tid_count = 0x0;
/*
verify:

0x98 cwde


*/

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
            this->my_bps[i] = 0x0;

    }

    ~taint_x86() {
        free(this->memory);
    }

};


