#include <string.h>
#include <taint_emul_x86.h>

int write_region(FILE* f, BYTE_t* src, unsigned int len)
{
    unsigned int i;

    for(i = 0x0; i<len; i++)
        fwrite(&src[i].val, 1, 1, f);

    return i;

}

int write_t_region(FILE* f, BYTE_t* src, unsigned int len)
{
    unsigned int i;

    for(i = 0x0; i<len; i++)
        fwrite(&src[i].val+1, 1, 1, f);

    return i;

}

int filter_str(char* str)
{
    unsigned i, len;

    len = strlen(str);

    for(i = 0x0; i<len; i++)
    {
        if(str[i] == '\r')
            str[i] = 0x0;
    }
    return 0x0;
}

int filter_str_2(char* str, char unwanted)
{
    unsigned i, len;

    len = strlen(str);

    for(i = 0x0; i<len; i++)
    {
        if(str[i] == unwanted)
            str[i] = '_';
    }
    return 0x0;
}

