/*
*   Tomasz Salacinski
*   CERT Orange Polska
*   tomasz.salacinski@orange.com
*/

#include <iostream>
#include <fstream>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <iomanip> 
#include <iniparser.h>
#include "taint_x86.h"
#include <math.h>

#define STRIP_WIDTH 0x40
#define STRIP_WIDTH_2 0x70
#define STRIP_HEIGHT 0x400
#define ROUNDING_MASK 0xffffff00
#define SCALE_FACTOR 0x5

#define COLOR_BLUE SDL_MapRGB(format, 0x00, 0x00, 0xff)
#define COLOR_BLACK SDL_MapRGB(format, 0x00, 0x00, 0x00)
#define COLOR_GREY SDL_MapRGB(format, 0xf0, 0xf0, 0xf0)
#define COLOR_WHITE SDL_MapRGB(format, 0xff, 0xff, 0xff)

LIB_INFO libs[0x100];
unsigned lib_count = 0x0;

enum textquality {solid, shaded, blended};
 
  SDL_Surface *drawtext(TTF_Font *fonttodraw, char fgR, char fgG, char fgB, char fgA, char bgR, char bgG, char bgB, char bgA, char text[], textquality quality)
  {
    SDL_Color tmpfontcolor = {fgR,fgG,fgB,fgA};
    SDL_Color tmpfontbgcolor = {bgR, bgG, bgB, bgA};
    SDL_Surface *resulting_text;
 
    if (quality == solid) resulting_text = TTF_RenderText_Solid(fonttodraw, text, tmpfontcolor);
    else if (quality == shaded) resulting_text = TTF_RenderText_Shaded(fonttodraw, text, tmpfontcolor, tmpfontbgcolor);
    else if (quality == blended) resulting_text = TTF_RenderText_Blended(fonttodraw, text, tmpfontcolor);
 
    return resulting_text;
  }

int load_lib_info(char* struct_file)
{
    FILE* f;
    char line[0x100];
    char* cmd;
    OFFSET off;
    char* name;


    printf("Opening: %s\n", struct_file);
    f = fopen(struct_file, "r");

    while(fgets(line, 0x100, f))
    {
        if(line[0] != 'R' || line[1] != 'L') continue;
        cmd = strtok(line, ",");
        off = strtol(strtok(0x0, ","), 0x0, 0x10);
        name = strtok(0x0, ",");
        name[strlen(name)] = 0x0;

        printf("Registering lib: %s at 0x%08x\n", name, off);
        
        strcpy(libs[lib_count].name, name);
        libs[lib_count].offset = off;
        libs[lib_count].loaded = 0x1;
        lib_count++;
    }
    printf("Loaded %d libraries\n", lib_count);
    fclose(f);
}

int main(int argc, char** argv)
{
    unsigned int img_from, img_to, scale, size;
    unsigned long i;
    SDL_Surface* img_taint;
    SDL_Surface* img_map;
    SDL_Surface* img_libs;
    std::ifstream myfile;
    char* buffer;
    SDL_PixelFormat* format;
    char* taint_file;
    char* struct_file;
    TTF_Font *font;
    unsigned report_step;
    unsigned int cur_byte;
    unsigned map_step;

    char out_taint[MAX_NAME];
    char out_map[MAX_NAME];
    char out_libs[MAX_NAME];

    SDL_Rect taint_marker;
    SDL_Rect map_marker;
    SDL_Rect mark3;
    SDL_Rect lib_marker;

    dictionary* ini, out_ini;

    unsigned mark_size;
    char sign[0x100];
    unsigned j;
    unsigned my_step = 0x10;
    int my_step_scale;
    OFFSET addr;

    /* parse arguments and ini */

    if(argc < 6)
        std::cout << "Usage: " << argv[0] << " <ini file> <from> <to> <scale> <png out>" << std::endl;

    ini = iniparser_load(argv[1]);
    taint_file = iniparser_getstring(ini, "general:taint_dump_path", 0x0);
    struct_file = iniparser_getstring(ini, "general:structured_info_path", 0x0);
    img_from = strtol(argv[2], 0x0, 0x10);
    img_to = strtol(argv[3], 0x0, 0x10);
    mark_size = strtol(argv[4], 0x0, 0x10);

    /* calculate scale and sizes */

    std::cout <<  std::setbase(0x10) << "Mark size: " << mark_size << std::endl;

    size = img_to - img_from;
    cur_byte = 0x0;
    report_step = size / 11;
    map_step = 0x1 << int(log10(size) / log10(16)) * 0x4; // calculate proper scale
    my_step_scale = int((log10(size / STRIP_WIDTH) /log10(16) - SCALE_FACTOR) * 0x4);
    if(my_step_scale < 0x0) my_step_scale = 0;
    my_step = 0x1 << my_step_scale;

    printf("Calculated map resolution: 0x%x\n", map_step);
    printf("Calculated image resolution: 0x%x\n", my_step);

    /* initialize SDL components */

	if (SDL_Init(SDL_INIT_VIDEO) != 0){
		std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
		return 1;
	}

    if (TTF_Init() != 0)
    {
		std::cout << "TTF_Init Error. " << std::endl;
        SDL_Quit();
        return 1;
    }

    img_taint = SDL_CreateRGBSurface(0, STRIP_WIDTH, STRIP_HEIGHT, 32, 0, 0, 0, 0);
    format = img_taint->format;
    img_map = SDL_CreateRGBSurface(0, STRIP_WIDTH, STRIP_HEIGHT, 32, 0, 0, 0, 0);
    img_libs = SDL_CreateRGBSurface(0, STRIP_WIDTH_2, STRIP_HEIGHT, 32, 0, 0, 0, 0);

    SDL_FillRect(img_taint, NULL, COLOR_GREY);
    SDL_FillRect(img_map, NULL, COLOR_WHITE);
    SDL_FillRect(img_libs, NULL, COLOR_WHITE);

    font = TTF_OpenFont("COURIER.TTF", 10);
    if(!font) printf("Pleace place COURIER.TTF in this folder.\n");

    /* load taint data into buffer */

    buffer = (char*)malloc(size);
    std::cout << std::setbase(0x10) << "Size: " << size << std::endl << "Starting reading file" << std::endl;

    myfile.open (taint_file);
    myfile.seekg(img_from, std::ios_base::beg);
    myfile.read(buffer, size);
    myfile.close(); 

    /* load library info */

    std::cout << "Loading library info" << std::endl;
    load_lib_info(struct_file);

    /* configure markers */

    taint_marker.x = 0;
    taint_marker.w = STRIP_WIDTH;
    taint_marker.h = mark_size;

    map_marker.x = 0;
    map_marker.w = STRIP_WIDTH;
    map_marker.h = 0x1;

    lib_marker.x = 0;
    lib_marker.w = STRIP_WIDTH_2;
    lib_marker.h = 0x1;

    /* generate visual data */

    std::cout << "Starting visualizing taint" << std::endl;

    for(i=0x0; i< size; i += my_step)
    {
        
        cur_byte = i;
        addr = cur_byte + img_from;

        /* report about progress */
        if(!(i % report_step))
            std::cout << "Done: " << std::setbase(10) << (i*100)/size << "%"  << std::endl;

        if( (addr % map_step) < my_step)
        {
            sprintf(sign, "0x%08x", addr & ROUNDING_MASK);
            SDL_Surface *text_surface = drawtext(font, 1, 1, 1, 1, 0, 0, 0, 1, sign, solid);
            map_marker.y = ((i * STRIP_HEIGHT) / size);
            SDL_Rect rect2 = { 0x0, 0x0, STRIP_WIDTH, 50 };
            rect2.y = ((i * STRIP_HEIGHT) / size) +2;
            SDL_BlitSurface(text_surface, NULL, img_map, &rect2);
            SDL_FillRect(img_map, &map_marker, COLOR_BLACK);
            SDL_FreeSurface(text_surface);
        }

        for(j = 0x0; j<lib_count; j++)
            if((libs[j].offset - addr) < my_step)
            {
                printf("0x%08x\n", libs[j].offset);
                SDL_Surface *text_surface = drawtext(font, 1, 1, 1, 1, 0, 0, 0, 1, libs[j].name, solid);
                lib_marker.y = ((i * STRIP_HEIGHT) / size);
                SDL_Rect rect3 = { 0x0, 0x0, STRIP_WIDTH_2, 50 };
                rect3.y = ((i * STRIP_HEIGHT) / size) +2;
                SDL_BlitSurface(text_surface, NULL, img_libs, &rect3);
                SDL_FillRect(img_libs, &lib_marker, COLOR_BLACK);
                SDL_FreeSurface(text_surface);
            }
                

        if(buffer[cur_byte] != 0x0)
        {
            taint_marker.y = (i * STRIP_HEIGHT) / size;
            SDL_FillRect(img_taint, &taint_marker, COLOR_BLUE);
            
        }
    }

    std::cout << "All done." << std::endl;
    std::cout << "Finished visualizing taint" << std::endl;
    
    /* Writing out results */

    sprintf(out_taint, "%s-taint.png", argv[5]);
    sprintf(out_map, "%s-map.png", argv[5]);
    sprintf(out_libs, "%s-libs.png", argv[5]);
    SDL_SaveBMP(img_taint, out_taint);
    SDL_SaveBMP(img_map, out_map);
    SDL_SaveBMP(img_libs, out_libs);

    /* Deinitialize */

    iniparser_freedict(ini);
    free(buffer);
	SDL_Quit();

    return 0x0;
}

