CC =g++
CFLAGS = -rdynamic -I. -ggdb -w -std=c++11 -D_GLIBCXX_USE_CXX11_ABI=0 -DHANDLE_BREAKPOINTS
NAMES = taint show_taint caller_taint symbol_export ltrace strace cextract call_analyze

OBJS = debug.o taint_x86.o main.c
REVOBJS = debug.o rev_taint_x86.o rev_main.c
CALL_OBJS = debug.o caller_taint_x86.o caller_main.c 
CEXTRACT_OBJS = debug.o cextract_taint_x86.o cextract_main.c 
SYMB_OBJS = symbol_export.o
LTRACE_OBJS = debug.o ltrace_taint_x86.o ltrace_main.c
STRACE_OBJS = debug.o caller_taint_x86.o strace_main.c 
CALL_ANALYZE_OBJS = debug.o taint_emul_x86.o out_utils.o utils.o breakpoint.o call_analyze.c

INILIBS = -l iniparser
BLISS_LIB = -lpebliss -Llib 
LTRACE_LIB = -lpebliss -Llib 
SDL_LIB = -L/usr/local/lib -lSDL -lSDL_ttf -Wl,-rpath=/usr/local/lib
SDL_INCLUDE = -I/usr/local/include

DISTORM_FLAGS = -Llib -l distorm3 -Iinc

CXXFLAGS = -Wall -c -std=c++11 $(SDL_INCLUDE)
LDFLAGS = $(SDL_LIB)
CXX = g++

%.o: %.cpp
	$(CC) -c -o $@ $< $(CFLAGS)

taint: $(OBJS)
	$(CC) -I. $(OBJS) -o $@ $(BLISS_LIB) $(DISTORM_FLAGS) $(CFLAGS)

cextract: $(CEXTRACT_OBJS)
	$(CC) -I. $(CEXTRACT_OBJS) -o $@ $(BLISS_LIB) $(CFLAGS)

caller_taint: $(CALL_OBJS)
	$(CC) -I. $(CALL_OBJS) -o $@ $(BLISS_LIB) $(CFLAGS)

call_analyze: $(CALL_ANALYZE_OBJS)
	$(CC) -I. $(CALL_ANALYZE_OBJS) -o $@ $(BLISS_LIB) $(CFLAGS)

symbol_export: $(SYMB_OBJS)
	$(CC) -I. $(SYMB_OBJS) -o $@ $(BLISS_LIB) $(CFLAGS)

ltrace: $(LTRACE_OBJS)
	$(CC) -I. $(LTRACE_OBJS) -o $@ $(LTRACE_LIB) $(CFLAGS)

strace: $(STRACE_OBJS)
	$(CC) -I. $(STRACE_OBJS) -o $@ $(LTRACE_LIB) $(CFLAGS)


.PHONY: clean

show_taint: show_taint.o
	$(CXX) $< $(LDFLAGS) -o $@ $(INILIBS) 

main.o: show_taint.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

all: $(NAMES)

clean:
	rm -f *.o $(NAMES)
