OBJS	= main.o projet.o
SOURCE	= main.c projet.c
HEADER	= projet.h
OUT		= ProjetNL 
PARAM	=  5555 4444
CC	 	= gcc
FLAGS	= -g -c -Wall
LIBEXTERNE	= -lpthread

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LIBEXTERNE)

main.o: main.c
	$(CC) $(FLAGS) main.c 

projet.o: projet.c
	$(CC) $(FLAGS) projet.c 


clean:
	rm -f $(OBJS) $(OUT) 

run: $(OUT) 
	./$(OUT) $(PARAM)