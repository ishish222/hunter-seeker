CC =g++
CFLAGS = -I. -ggdb
NAME = trace
OBJS = taint_x86.o main.c

%.o: %.cpp
	$(CC) -c -o $@ $< $(CFLAGS)

trace: $(OBJS)
	$(CC) -I. $(OBJS) -o $@

.PHONY: clean

clean:
	rm -f *.o $(NAME)
