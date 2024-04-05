#include <ctype.h>  // для iscntrl
#include <getopt.h>
#include <locale.h>
#include <regex.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ENABLED_SHORT_FLAGS "+e:ivclnhsf:o"
#define PROMPT_END -1
#define size 10000

typedef struct {
  bool e;
  bool i;
  bool v;
  bool c;
  bool l;
  bool n;
  bool h;
  bool s;
  bool f;
  bool o;
  bool multiple_files;
  char filename[size];
  char reg_str[size];
  char cur_str[size];
} Flags;

static void printstderr();
static int setflag(Flags *flags, char flag);
static int parser(Flags *flags, int argc, char *argv[]);
static void grep(Flags *flags, char *argv[]);
static void add_reg_from_file(Flags *flags, char *filename);
static void flag_o(Flags *flags, regmatch_t match);
static void not_flag_o(Flags *flags);
static void flag_n(Flags *flags, int line);
static void flag_multiple_files(Flags *flags, char *mystr);

int main(int argc, char *argv[]) {
  Flags flags = {0};
  // парсим команду
  int error = parser(&flags, argc, argv);
  if (!error) {
    if (!flags.multiple_files) {
      grep(&flags, argv);
    } else {
      while (optind < argc) {
        grep(&flags, argv);
        optind++;
      }
    }
  }
  return 0;
}
static void flag_o(Flags *flags, regmatch_t match) {
  if (flags->o) {
    char matched_string[10000];
    strncpy(matched_string, flags->cur_str + match.rm_so,
            match.rm_eo - match.rm_so);
    matched_string[match.rm_eo - match.rm_so] = '\0';
    fprintf(stdout, "%s", matched_string);
    if (flags->cur_str[strlen(flags->cur_str) - 1] == '\n' &&
        matched_string[strlen(flags->cur_str) - 1] != '\n') {
      fprintf(stdout, "\n");
    }
  }
}

static void not_flag_o(Flags *flags) {
  if (!flags->o) fprintf(stdout, "%s", flags->cur_str);
}

static void flag_n(Flags *flags, int line) {
  if (flags->n) fprintf(stdout, "%d:", line);
}

static void flag_multiple_files(Flags *flags, char *mystr) {
  if (flags->multiple_files && !flags->h) fprintf(stdout, "%s:", mystr);
}

static void grep(Flags *flags, char *argv[]) {
  FILE *file = fopen(argv[optind], "r");
  regex_t reg;
  regmatch_t match;
  int regcompflag = flags->i ? REG_EXTENDED | REG_ICASE : REG_EXTENDED;
  int contains = 0;
  int count = 0;
  int line = 0;
  regcomp(&reg, flags->reg_str, regcompflag);
  if (file != NULL) {
    while (fgets(flags->cur_str, size, file) != NULL) {
      line++;
      contains = !regexec(&reg, flags->cur_str, 1, &match, REG_EXTENDED);
      if (flags->v) contains = !contains;
      if (contains) count++;
      if (contains && !flags->l && !flags->c) {
        flag_multiple_files(flags, argv[optind]);
        flag_n(flags, line);
        not_flag_o(flags);
        flag_o(flags, match);
      }
    }
    if (flags->c) {
      if (flags->l && count > 1) count = 1;
      if (flags->multiple_files && !flags->h)
        fprintf(stdout, "%s:", argv[optind]);
      fprintf(stdout, "%d\n", count);
    }
    if (flags->l && count > 0) fprintf(stdout, "%s\n", argv[optind]);
    if (!flags->l && !flags->c && contains &&
        flags->cur_str[strlen(flags->cur_str) - 1] != '\n') {
      fprintf(stdout, "\n");
    }
    fclose(file);
  } else {
    if (!flags->s) fprintf(stdout, "s21_grep: %s: No such file", argv[optind]);
  }
  regfree(&reg);
}

static void add_reg_from_file(Flags *flags, char *filename) {
  FILE *file = fopen(filename, "r");
  if (file != NULL) {
    while (fgets(flags->cur_str, size, file) != NULL) {
      if (flags->cur_str[strlen(flags->cur_str) - 1] == '\n') {
        flags->cur_str[strlen(flags->cur_str) - 1] = '\0';
      }
      strcat(flags->reg_str, flags->cur_str);
      strcat(flags->reg_str, "|");
    }
    fclose(file);
  } else {
    if (!flags->s) {
      fprintf(stdout, "s21_grep: %s: No such file", filename);
    }
  }
}

// парсинг команды и установка флагов
static int parser(Flags *flags, int argc, char *argv[]) {
  int error = 0;
  char current_flag = getopt(argc, argv, ENABLED_SHORT_FLAGS);
  if (argc < 3) {
    error = 1;
    printf(
        "usage: grep [-ivclnhsf] [-e pattern] [-f file] [pattern] [file ...]");
  }
  while (current_flag != PROMPT_END && !error) {
    error = setflag(flags, current_flag);
    current_flag = getopt(argc, argv, ENABLED_SHORT_FLAGS);
  }
  if (flags->v == true) {
    flags->o = false;
  }
  if (!flags->e && !flags->f) {
    if (argc > optind) {
      strcat(flags->reg_str, argv[optind]);
    }
    optind++;
  }
  if (flags->e || flags->f) {
    flags->reg_str[strlen(flags->reg_str) - 1] = '\0';
  }
  if (argc > optind + 1) {
    flags->multiple_files = true;
  }
  return error;
}

// установка флага
static int setflag(Flags *flags, char flag) {
  int error = 0;
  switch (flag) {
    case 'e':
      flags->e = true;
      strcat(flags->reg_str, optarg);
      strcat(flags->reg_str, "|");
      break;
    case 'i':
      flags->i = true;
      break;
    case 'v':
      flags->v = true;
      break;
    case 'c':
      flags->c = true;
      break;
    case 'l':
      flags->l = true;
      break;
    case 'n':
      flags->n = true;
      break;
    case 'h':
      flags->h = true;
      break;
    case 's':
      flags->s = true;
      break;
    case 'f':
      flags->f = true;
      strcpy(flags->filename, optarg);
      add_reg_from_file(flags, flags->filename);
      break;
    case 'o':
      flags->o = true;
      break;
    case '?':
    default:
      printstderr();
      error = 1;
      break;
  }
  return error;
}

static void printstderr() {
  fprintf(stderr, "Usage: grep [OPTION]... PATTERNS [FILE]...");
}