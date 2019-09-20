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
    if(argc < 0x3)
    {
        printf("Usage: %s <cov1> <cov2>\n", argv[0]);
        return -1;
    }

    char path1[MAX_NAME];
    char path2[MAX_NAME];
//    char path3[MAX_NAME];
    
    strcpy(path1, argv[1]);    
    strcpy(path2, argv[2]);    
//    strcpy(path3, argv[3]);    

    FILE* f1;
    FILE* f2;
//    FILE* f3;

    LIBRARY lib1, lib2, lib3;

    f1 = fopen(path1, "rb");
    f2 = fopen(path2, "rb");
//    f3 = fopen(path3, "wb");

    /* load coverage 1 */
    fseek(f1, 0x0, SEEK_END);
    lib1.size = ftell(f1);
    fseek(f1, 0x0, SEEK_SET);

    lib1.content = (char*)malloc(lib1.size);
    fread(lib1.content, lib1.size, 0x1, f1);
    fclose(f1);
    
    /* load coverage 2 */
    fseek(f2, 0x0, SEEK_END);
    lib2.size = ftell(f2);
    fseek(f2, 0x0, SEEK_SET);

    lib2.content = (char*)malloc(lib2.size);
    fread(lib2.content, lib2.size, 0x1, f2);
    fclose(f2);
    
    /* create difference */
    if(lib2.size != lib1.size)
    {
        printf("The sizes are different\n");
        return -1;
    }

//    lib3.size = lib1.size;
//    lib3.content = (char*)malloc(lib3.size);

    unsigned i;
    unsigned touched_more = 0x0;

    /* lib3 = lib1 - lib2 */

    for(i = 0x0; i< lib1.size; i++)
    {
        if(lib1.content[i])
        {
            if(lib2.content[i])
            {
//                lib3.content[i] = 0x0;
            }
            else
            {
//                lib3.content[i] = 0xff;
                touched_more ++;
            }
        }
    }

    if(touched_more)
    printf("%d instructions more %s touched than %s\n", touched_more, path1, path2);
    //fwrite(lib3.content, lib3.size, 0x1, f3);
    //fclose(f3);

    return 0x0;
}
