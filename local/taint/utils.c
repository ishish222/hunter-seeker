#include <string.h>
#include <taint_emul_x86.h>

void print_location(modrm_ptr a)
{
    switch(a.region)
    {
        case MODRM_REG:
            printf("Region: MODRM_REG\n");
            break;
        case MODRM_MEM:
            printf("Region: MODRM_MEM\n");
            break;
        default:
            printf("Undeclared\n");
            break;
    }

    printf("Offset: 0x%x\n", a.offset);
}

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

void print_usage()
{
    printf("Usage: taint -i <instr> -d <dump> -o <dump2> -t <taint> -T <taint_dump> -l <lost> -p <prefix> -n <count>\n");
    printf("-i input  - forward instruction file\n");
    printf("-d input  - memory dump in t1\n");
    printf("-o input  - start addr\n");
    printf("-t input  - end addr\n");
    printf("-t input  - taint dump\n");
    printf("-l input  - analysis depth\n");
    printf("-L input  - loadable dir path\n");
    return;
}

void _pause()
{
    fprintf(stdout, "Press any key\n");
    getchar();
    return;
}

