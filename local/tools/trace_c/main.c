#include <taint_x86.h>
#include <stdio.h>

int main()
{
    printf("Here we go\n");
    taint_x86 taint_eng;

    char instruction[0x10];
    instruction[0x0] = '\x50';
    instruction[0x1] = '\x50';
    instruction[0x2] = '\x50';

    printf("Here we go\n");


    taint_eng.execute_instruction(instruction);

    return 0x0;
}
