CC = gcc
FLAGS = -Wall -Werror -Wextra
TARGET = s21_grep

all:clean build

build:
	$(CC) $(FLAGS) -o $(TARGET) s21_grep.c

clean:
	if [ -s ./s21_grep ]; then rm s21_grep; fi

run_tests:
	chmod 777 ./tests_grep.sh
	./tests_grep.sh