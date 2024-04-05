CC = gcc
FLAGS = -Wall -Werror -Wextra
TARGET = s21_cat

all:clean build

build:
	$(CC) $(FLAGS) -o $(TARGET) s21_cat.c

clean:
	if [ -s ./s21_cat ]; then rm s21_cat; fi

run_tests:
	chmod 777 ./tests_cat.sh
	./tests_cat.sh