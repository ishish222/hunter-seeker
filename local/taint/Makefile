CC =g++
CFLAGS = -rdynamic -I. -ggdb -w -std=c++11 -D_GLIBCXX_USE_CXX11_ABI=0 -DHANDLE_BREAKPOINTS -DUSE_DISTORM -fpermissive $(DISTORM_LIB) 
NAMES = symbol_export call_analyze taint_analyze coverage_analyze coverage_diff coverage_sum coverage_count

CEXTRACT_OBJS = debug.o cextract_taint_x86.o cextract_main.c 
SYMB_OBJS = symbol_export.o
TAINT_ANALYZE_OBJS = debug.o emul_x86.o out_utils.o utils.o breakpoint.o taint_plugin.o taint_analyze.c plugin.o
CALL_ANALYZE_OBJS = debug.o emul_x86.o out_utils.o utils.o breakpoint.o graph_plugin.o call_analyze.c plugin.o
COVERAGE_ANALYZE_OBJS = debug.o emul_x86.o out_utils.o utils.o breakpoint.o coverage_plugin.o coverage_analyze.c plugin.o
COVERAGE_DIFF_OBJS = coverage_diff.c 
COVERAGE_SUM_OBJS = coverage_sum.c 
COVERAGE_COUNT_OBJS = coverage_count.c 

INILIBS = -l iniparser
BLISS_LIB = -lpebliss -Llib 
LTRACE_LIB = -lpebliss -Llib 
SDL_LIB = -L/usr/local/lib -lSDL -lSDL_ttf -Wl,-rpath=/usr/local/lib
SDL_INCLUDE = -I/usr/local/include

READLINE_LIB = -L/usr/lib -lhistory -lreadline

DISTORM_LIB= -L/usr/lib -l distorm3 -Idistorm_inc

#CXXFLAGS = -Wall -c -std=c++11 -fpermissive $(SDL_INCLUDE)
CXXFLAGS = -Wall -c -std=c++11 -fpermissive -DUSE_DISTORM $(DISTORM_LIB)
#LDFLAGS = $(SDL_LIB)
CXX = g++

%.o: %.cpp
	$(CC) -c -o $@ $< $(CFLAGS)

cextract: $(CEXTRACT_OBJS)
	$(CC) -I. $(CEXTRACT_OBJS) -o $@ $(BLISS_LIB) $(CFLAGS)

call_analyze: $(CALL_ANALYZE_OBJS)
	$(CC) -I. $(CALL_ANALYZE_OBJS) -o $@ $(BLISS_LIB) $(CFLAGS)

taint_analyze: $(TAINT_ANALYZE_OBJS)
	$(CC) -I. $(TAINT_ANALYZE_OBJS) -o $@ $(BLISS_LIB) $(READLINE_LIB) $(DISTORM_LIB) $(DISTORM_INC) $(CFLAGS)

coverage_analyze: $(COVERAGE_ANALYZE_OBJS)
	$(CC) -I. $(COVERAGE_ANALYZE_OBJS) -o $@ $(BLISS_LIB) $(CFLAGS)

coverage_diff: $(COVERAGE_DIFF_OBJS)
	$(CC) -I. $(COVERAGE_DIFF_OBJS) -o $@

coverage_sum: $(COVERAGE_SUM_OBJS)
	$(CC) -I. $(COVERAGE_SUM_OBJS) -o $@

coverage_count: $(COVERAGE_COUNT_OBJS)
	$(CC) -I. $(COVERAGE_COUNT_OBJS) -o $@

symbol_export: $(SYMB_OBJS)
	$(CC) -I. $(SYMB_OBJS) -o $@ $(BLISS_LIB) $(CFLAGS)

.PHONY: clean

all: $(NAMES)

clean:
	rm -f *.o $(NAMES)
