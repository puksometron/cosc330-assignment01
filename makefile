OBJS	= char_stats.o ring_process.o
SOURCE	= char_stats.c ring_process.c
HEADER	= ring_process.h
OUT	= char_stats.o
CC	 = gcc
FLAGS	 = -g -c -Wall -pedantic
LFLAGS	 = 

all: $(OBJS)
	$(CC) -g $(SOURCE) -o $(OUT) $(LFLAGS)

char_stats.o: char_stats.c
	$(CC) $(FLAGS) char_stats.c 

ring_process.o: ring_process.c
	$(CC) $(FLAGS) ring_process.c 


clean:
	rm -f $(OBJS) $(OUT)

run: $(OUT)
	./$(OUT)
