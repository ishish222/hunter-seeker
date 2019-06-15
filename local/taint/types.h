#include <stdio.h>

#ifndef TYPES_H
#define TYPES_H

#define DWORD int
#define UDWORD unsigned int
#define OFFSET unsigned int
#define WORD short
#define UWORD unsigned short
#define BYTE unsigned char
#define QWORD long long int
#define UQWORD unsigned long long int

#define CAUSE_ID_NONE 0xffffffff

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

typedef struct _EXCEPTION_INFO
{
    EXCEPTION_RECORD er;
    DWORD tid;
} EXCEPTION_INFO;

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

typedef struct _CONTEXT_OUT
{
    CONTEXT ctx;
    LDT_ENTRY ldt[0x6];
    int thread_id;
} CONTEXT_OUT;

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
        this->id = CAUSE_ID_NONE;
    }

    BYTE_t(BYTE a)
    {
        this->val = 0x0;
        this->id = CAUSE_ID_NONE;
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
    }

    WORD_t operator=(WORD_t a)
    {
        this->set_WORD(a.get_WORD());
        this->set_WORD_t(a.get_WORD_t());
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

#endif
