CC = $(CXX)
CXXFLAGS = -Wall -Wextra -ggdb -pedantic -std=c++11

all: main

main: main.o hw1.o

main.o: main.cpp  hw1.h stb_image.h stb_image_write.h

hw1.o: hw1.cpp hw1.h

clean:
	$(RM) main main.o hw1.o blurred_*.jpg

.PHONY: all clean
