#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "limits.h"
#include "types.h"

typedef struct _LIBRARY
{
    OFFSET offset;
    char name[MAX_NAME];
    char path[MAX_NAME];
    unsigned size;
    DWORD length;
    DWORD loaded;
    char* content;
    unsigned instructions_touched;
    char blacklisted;
} LIBRARY;

int main(int argc, char** argv)
{
    if(argc < 0x2)
    {
        printf("Usage: %s <cov1>\n", argv[0]);
        return -1;
    }

    char path1[MAX_NAME];
    
    strcpy(path1, argv[1]);    

    FILE* f1;

    LIBRARY lib1;

    f1 = fopen(path1, "rb");

    /* load coverage 1 */
    fseek(f1, 0x0, SEEK_END);
    lib1.size = ftell(f1);
    fseek(f1, 0x0, SEEK_SET);

    lib1.content = (char*)malloc(lib1.size);
    fread(lib1.content, lib1.size, 0x1, f1);
    fclose(f1);
    

    unsigned i;
    unsigned touched= 0x0;

    /* lib3 = lib1 - lib2 */

    for(i = 0x0; i< lib1.size; i++)
    {
        if(lib1.content[i])
        {
                touched ++;
        }
    }

    printf("%d instructions more %s touched\n", touched, path1);

    return 0x0;
}
