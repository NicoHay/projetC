OBJS			= main.o projet.o
SOURCE			= main.c projet.c
HEADER			= projet.h
OUT			    = ProjetNico 
PARAM 			= 4444 5555
COMPILATEUR	 	= gcc
FLAGS			= -g -c -Wall
LIBEXTERNE		= -lpthread

all: $(OBJS)
	$(COMPILATEUR) -g $(OBJS) -o $(OUT) $(LIBEXTERNE)

main.o: main.c
	$(COMPILATEUR) $(FLAGS) main.c 

projet.o: projet.c
	$(COMPILATEUR) $(FLAGS) projet.c 


clean:
	rm -f *.o $(OUT) 

run: 
	./$(OUT) ${PORT1} ${PORT2} 
