OBJS	= main.o func.o
SOURCE	= main.cpp func.cpp
HEADER	= main.h
OUT	= kals.out
CC	 = g++
FLAGS	 = -g -c
LFLAGS	 = 

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS)

main.o: main.cpp
	$(CC) $(FLAGS) main.cpp -std=c++11

func.o: func.cpp
	$(CC) $(FLAGS) func.cpp -std=c++11


clean:
	rm -f $(OBJS) $(OUT)
