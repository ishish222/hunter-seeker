#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

//add 16 bit, 8bit offsets

#define MEM_SIZE 0x100000000
#define REG_SIZE 0x750

#define DWORD int
#define OFFSET int
#define WORD short
#define BYTE char

class BYTE_t
{
    public:

    BYTE val;
    BYTE val_t;
    
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
};

class DWORD_t
{
    public:

    BYTE_t val[4];
    
    DWORD get_DWORD()
    {
        DWORD ret;

        char* tmp;
        tmp = (char*)&ret;

        tmp[0] = val[0].val;
        tmp[1] = val[1].val;
        tmp[2] = val[2].val;
        tmp[3] = val[3].val;

        return ret;
    }

    DWORD get_t_DWORD()
    {
        DWORD ret;

        char* tmp;
        tmp = (char*)&ret;

        tmp[0] = val[0].val_t;
        tmp[1] = val[1].val_t;
        tmp[2] = val[2].val_t;
        tmp[3] = val[3].val_t;

        return ret;
    }

    void set_DWORD(DWORD v)
    {
        char* tmp;
        tmp = (char*)&v;

        val[0].val = tmp[0];
        val[1].val = tmp[1];
        val[2].val = tmp[2];
        val[3].val = tmp[3];

        return;
    }

    void set_t_DWORD(DWORD v_t)
    {
        char* tmp;
        tmp = (char*)&v_t;

        val[0].val = tmp[0];
        val[1].val = tmp[1];
        val[2].val = tmp[2];
        val[3].val = tmp[3];

        return;
    }

    BYTE_t operator[](int index)
    {
        if(index < 0x4)
            return val[index];
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
        val[0] = a;
        val[1] = 0x0;
        val[2] = 0x0;
        val[3] = 0x0;
    }

    //from BYTE_t, proxy to BYTE constructor
    DWORD_t(BYTE_t a)
    {
        val[0] = a;
        val[1] = 0x0;
        val[2] = 0x0;
        val[3] = 0x0;
    }

    //from DWORD, cast to char and consider individual bytes
    DWORD_t(DWORD a)
    {
        char* tmp;
        tmp = (char*)&a;

        val[0] = tmp[0];
        val[1] = tmp[1];
        val[2] = tmp[2];
        val[3] = tmp[3];
    }
/*
    //from DWORD_t
    DWORD_t(DWORD_t& a)
    {
        /*
        val[0].val = a.val[0];
        val[1].val = a.val[1];
        val[2].val = a.val[2];
        val[3].val = a.val[3];

        val[0].val_t = a.val_t[0];
        val[1].val_t = a.val_t[1];
        val[2].val_t = a.val_t[2];
        val[3].val_t = a.val_t[3];
        *\/
        val[0] = a[0];
        val[1] = a[1];
        val[2] = a[2];
        val[3] = a[3];
    }
*/
    //from BYTE_t pointer, with endianess
    DWORD_t(BYTE_t* a, int le=0)
    {
        if(!le)
        {
            val[0] = *a;
            a += sizeof(BYTE_t);
            val[1] = *a;
            a += sizeof(BYTE_t);
            val[2] = *a;
            a += sizeof(BYTE_t);
            val[3] = *a;
            a += sizeof(BYTE_t);
        }
        else
        {
            val[3] = *a;
            a += sizeof(BYTE_t);
            val[2] = *a;
            a += sizeof(BYTE_t);
            val[1] = *a;
            a += sizeof(BYTE_t);
            val[0] = *a;
            a += sizeof(BYTE_t);
        }
    }
};
/*
typedef struct BYTE_t_
{
    BYTE val;
    BYTE val_t;
} BYTE_t;

typedef struct DWORD_t_
{
    BYTE_t val[4];
    BYTE_t val_t[4];
} DWORD_t;
*/
class taint_x86
{
    public:
    BYTE_t* memory;
    BYTE_t registers[REG_SIZE];

    typedef int (taint_x86::*instruction_routine)(char* args);
    instruction_routine instructions_32[0x100];

    // instruction routines
    int r_pop_eax_32(char*);
    int r_pop_ebx_32(char*);
    int r_push_eax_32(char*);

    // auxilliary
    int a_push_32(DWORD_t);
    DWORD_t a_pop_32();

    DWORD le2dword(char*);

    void store_32(OFFSET, DWORD_t);
    DWORD_t restore_32(OFFSET);

    void store_32(DWORD_t, DWORD_t);
    DWORD_t restore_32(DWORD_t);

    void reg_store_32(OFFSET, DWORD_t);
    DWORD_t reg_restore_32(OFFSET);

    // engine methods
    int execute_instruction(char*);
    int print_context();
    int print_bt_buffer(BYTE_t*, DWORD);
    int print_all_regs();

    taint_x86()
    {
        printf("Initializing taint emulator\n");
    
        this->instructions_32[0x50] = &taint_x86::r_push_eax_32;
        this->instructions_32[0x58] = &taint_x86::r_pop_eax_32;
        this->instructions_32[0x5a] = &taint_x86::r_pop_ebx_32;
//        this->instructions_32[0x50] = taint_x86::r_push_eax_32;
//        this->instructions_32[0x51] = r_push_ecx_32;
//        this->instructions_32[0x52] = r_push_edx_32;
//        this->instructions_32[0x53] = r_push_ebx_32;

        //this->reg_store_32(EAX, "\x11\x22\x33\x44");
//        this->reg_store_32(ESP, "\x00\x00\x11\x00");
        this->memory = (BYTE_t*)malloc(sizeof(BYTE_t)*MEM_SIZE);

        DWORD_t tmp(0x11223344);

        this->reg_store_32(EAX, tmp);
        this->reg_store_32(EBX, 0x55332211);
        this->reg_store_32(ESP, 0x00001100);

    }

    ~taint_x86() {
        free(this->memory);
    }

};


