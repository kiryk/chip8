#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "mem.h"
#include "oper.h"
#include "io.h"

int main(int argc, char *argv[])
{
  int i, d = 2;
  unsigned short code = 0;
  char *pname = NULL, *rname = NULL;
  FILE *fd = NULL;

  if (argc < 2) {
    printf("Usage: %s [game file]\n", argv[0]);
		printf("Or with options:\n"
			"\t-p [game file]\n"
			"\t-r [rom file]\n"
			"\t-d [delay time]\n");
    exit(1);
  } else if (argc == 2) {
    pname = argv[1];
  } else for (i = 0; argv[i]; ++i) {
    if (argv[i][0] == '-' && argv[i + 1]) {
      switch (argv[i][1]) {
      case 'p':
        pname = argv[i + 1];
        break;
      case 'r':
        rname = argv[i + 1];
        break;
      case 'd':
        d = atoi(argv[i + 1]);
        break;
      default:
        printf("Error: unknown option %s\n", argv[i]);
        exit(1);
        break;
      }
      ++i;
    }
  }

  /* load the program */
  if (!pname) {
    printf("Error: program's filename not specified, use -p\n");
    exit(1);
  }
  if ((fd = fopen(pname, "r")) == NULL) {
    printf("Error: could not open %s\n", pname);
    exit(1);
  }
  for (i = pc; fread(&mem[i], 1, 1, fd) == 1; ++i)
    ;
  fclose(fd);

  /* load ROM file */
  if (rname) {
    if ((fd = fopen(rname, "r")) == NULL) {
      printf("Error: could not open %s\n", rname);
      exit(1);
    }
    for (i = 0; fread(&mem[i], 1, 1, fd) == 1; ++i)
      ;
    fclose(fd);
  }

  srand(time(NULL));
  atexit(io_deinit);
  io_init();
  for (;;) {
    io_check_quit();
    io_timers();
    io_screen();
    code = mem[pc] << 8 | mem[pc + 1];
    pc += 2;
    for (i = 0; ops[i].mask; ++i)
      if ((code & ops[i].mask) == ops[i].code) {
          ops[i].oper(code);
          break;
      }
    io_delay(d);
  }

  exit(0);
}
