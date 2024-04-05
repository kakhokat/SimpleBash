#include <ctype.h>  // для iscntrl
#include <getopt.h>
#include <locale.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define ENABLED_SHORT_FLAGS "+beEnstTv"
#define PROMPT_END -1
static const struct option ENABLED_LONG_FLAGS[] = {
    {"number-nonblank", 0, NULL, 'b'},
    {"number", 0, NULL, 'n'},
    {"squeeze-blank", 0, NULL, 's'},
    {NULL, 0, NULL, 0}};

typedef struct {
  bool b;
  bool e;
  bool n;
  bool v;
  bool s;
  bool t;
} Flags;

static void printerr();
static void setflag(Flags *flags, char flag);
static void parser(Flags *flags, int argc, char *argv[]);
static void print_files(Flags *flags, int file_count, char *file_paths[]);
static void print_file(Flags *flags, FILE *file);
static void print_char(Flags *flags, FILE *file, char cur_char, char prev_char,
                       int *line_number);
static void count_new_line(Flags *flags, char cur_char, char prev_char,
                           int *line_number);

int main(int argc, char *argv[]) {
  Flags flags = {0};
  // парсим команду
  parser(&flags, argc, argv);
  print_files(&flags, argc - optind, argv + optind);

  return 0;
}

// парсинг команды и установка флагов
static void parser(Flags *flags, int argc, char *argv[]) {
  int long_optind = 0;
  char current_flag = getopt_long(argc, argv, ENABLED_SHORT_FLAGS,
                                  ENABLED_LONG_FLAGS, &long_optind);
  while (current_flag != PROMPT_END) {
    setflag(flags, current_flag);
    current_flag = getopt_long(argc, argv, ENABLED_SHORT_FLAGS,
                               ENABLED_LONG_FLAGS, &long_optind);
  }
  if (flags->b == true) {
    flags->n = false;
  }
}

// установка флага
static void setflag(Flags *flags, char flag) {
  switch (flag) {
    case 'b':
      flags->b = true;
      break;
    case 'e':
      flags->e = true;
      flags->v = true;
      break;
    case 'E':
      flags->e = true;
      break;
    case 'v':
      flags->v = true;
      break;
    case 'n':
      flags->n = true;
      break;
    case 's':
      flags->s = true;
      break;
    case 't':
      flags->t = true;
      flags->v = true;
      break;
    case 'T':
      flags->t = true;
      break;
    case '?':
    default:
      printerr();
  }
}

static void print_files(Flags *flags, int file_count, char *file_paths[]) {
  int file_id = 0;
  FILE *file = NULL;
  while (file_id < file_count) {
    file = fopen(file_paths[file_id], "r");
    if (file) {
      print_file(flags, file);  // печать содержимого
      fclose(file);
    } else {
      printerr();
    }
    file_id++;
  }
}

static void print_file(Flags *flags, FILE *file) {
  int line_number = 0;
  char prev_char = '\n';
  char cur_char;
  cur_char = getc(file);
  while (!feof(file)) {
    count_new_line(flags, cur_char, prev_char, &line_number);
    print_char(flags, file, cur_char, prev_char, &line_number);
    prev_char = cur_char;
    cur_char = getc(file);
  }
}

static void count_new_line(Flags *flags, char cur_char, char prev_char,
                           int *line_number) {
  if (flags->b && prev_char == '\n' && cur_char != '\n') {
    fprintf(stdout, "%6d\t", (++*line_number));
  } else if (flags->n && prev_char == '\n') {
    if (!flags->s || cur_char != '\n') {
      fprintf(stdout, "%6d\t", (++*line_number));
    } else {
      ++*line_number;
    }
  }
}

static void print_char(Flags *flags, FILE *file, char cur_char, char prev_char,
                       int *line_number) {
  // вывод символов
  if (flags->s && cur_char == prev_char && cur_char == '\n') {
    if (flags->n) {
      fprintf(stdout, "%6d\t", *line_number);
    }
    if (flags->e == true) {
      putc('$', stdout);
    }
    putc('\n', stdout);
    while (cur_char == '\n') {
      cur_char = getc(file);
    }
    ungetc(cur_char, file);
  } else if (cur_char == '\n') {
    if (flags->e == true) {
      putc('$', stdout);
    }
    putc('\n', stdout);
  } else if (cur_char == '\t') {
    if (flags->t == true) {
      putc('^', stdout);
      putc('I', stdout);
    } else {
      putc('\t', stdout);
    }
  } else if (flags->v &&
             iscntrl(cur_char)) {  // cur_char < 32 || cur_char > 126
    putc('^', stdout);
    if (cur_char == 127) {
      putc(cur_char - 64, stdout);
    } else {
      putc(cur_char + 64, stdout);
    }
  } else {
    putc(cur_char, stdout);
  }
}

static void printerr() { fprintf(stderr, "usage: cat [-beEnstT] [file ...]"); }