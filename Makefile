CC = $(CXX)
CXXFLAGS = -std=c++11 -pedantic -Wall -Wextra -ggdb -Og
LDFLAGS = -pthread

OBJ = main.o hw1.o

main: $(OBJ)

main.o: hw1.h stb_image.h stb_image_write.h

hw1.o: hw1.h stb_image.h stb_image_write.h

clean:
	$(RM) main $(OBJ) blurred_*.jpg

.PHONY: all clean
