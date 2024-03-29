TARGET = ./game_of_life.out
SRC = src/game_of_life.c

.PHONY: all clean build rebuild memcheck 1 2 3 4 5 6

all: rebuild check memcheck
#	$(TARGET)

clean:
	rm -rf $(TARGET)

rebuild: clean build

build:
	gcc -o $(TARGET) $(SRC) -lncurses 


testing:
	cat src/test1.txt /dev/stdin | $(TARGET)

1:
	cat src/1.txt /dev/stdin | $(TARGET)

2:
	cat src/2.txt /dev/stdin | $(TARGET)

3:
	cat src/3.txt /dev/stdin | $(TARGET)

4:
	cat src/4.txt /dev/stdin | $(TARGET)

5:
	cat src/5.txt /dev/stdin | $(TARGET)

6:
	cat src/6.txt /dev/stdin | $(TARGET)
