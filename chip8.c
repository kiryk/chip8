#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "chip8.h"
#include "oper.h"
#include "io.h"

/* stack declaration */
unsigned short stack[0x10];

/* registers */
unsigned char reg[0x10] = {0};
unsigned char *vf = &reg[0xF];
unsigned short regi = 0, regs = 0, regd = 0;
int sp = -1, pc = 0x200;

/* memory with preloaded fonts' definition */
unsigned char mem[MEMB] = {
  0xF0, 0x90, 0x90, 0x90, 0xF0,
  0x10, 0x10, 0x10, 0x10, 0x10,
  0xF0, 0x10, 0xF0, 0x80, 0xF0,
  0xF0, 0x10, 0xF0, 0x10, 0xF0,
  0x90, 0x90, 0xF0, 0x10, 0x10,
  0xF0, 0x80, 0xF0, 0x10, 0xF0,
  0xF0, 0x80, 0xF0, 0x90, 0xF0,
  0xF0, 0x10, 0x10, 0x10, 0x10,
  0xF0, 0x90, 0xF0, 0x90, 0xF0,
  0xF0, 0x90, 0xF0, 0x10, 0xF0,
  0xF0, 0x90, 0xF0, 0x90, 0x90,
  0xE0, 0x90, 0xE0, 0x90, 0xE0,
  0xF0, 0x80, 0x80, 0x80, 0xF0,
  0xE0, 0x90, 0x90, 0x90, 0xE0,
  0xF0, 0x80, 0xF0, 0x80, 0xF0,
  0xF0, 0x80, 0xF0, 0x80, 0x80,
  0x60, 0x60, 0x60, 0x00, 0x60
};

/* pointers to the screen's part of the memory, so it can be accessed easily */
unsigned char *scr[] = {
  mem + 0xF00, mem + 0xF08, mem + 0xF10, mem + 0xF18,
  mem + 0xF20, mem + 0xF28, mem + 0xF30, mem + 0xF38,
  mem + 0xF40, mem + 0xF48, mem + 0xF50, mem + 0xF58,
  mem + 0xF60, mem + 0xF68, mem + 0xF70, mem + 0xF78,
  mem + 0xF80, mem + 0xF88, mem + 0xF90, mem + 0xF98,
  mem + 0xFA0, mem + 0xFA8, mem + 0xFB0, mem + 0xFB8,
  mem + 0xFC0, mem + 0xFC8, mem + 0xFD0, mem + 0xFD8,
  mem + 0xFE0, mem + 0xFE8, mem + 0xFF0, mem + 0xFF8
};

/* processor operations' descriptions and the functions simulating them */
struct oper_t ops[] = {
  {0xFFFF, 0x00E0, oper_clr},
  {0xFFFF, 0x00EE, oper_ret},
  {0xF000, 0x1000, oper_jmp},
  {0xF000, 0x2000, oper_call},
  {0xF000, 0x3000, oper_equ},
  {0xF000, 0x4000, oper_nequ},
  {0xF00F, 0x5000, oper_requ},
  {0xF000, 0x6000, oper_set},
  {0xF000, 0x7000, oper_add},
  {0xF00F, 0x8000, oper_setr},
  {0xF00F, 0x8001, oper_or},
  {0xF00F, 0x8002, oper_and},
  {0xF00F, 0x8003, oper_xor},
  {0xF00F, 0x8004, oper_radd},
  {0xF00F, 0x8005, oper_rsub},
  {0xF00F, 0x8006, oper_lsr},
  {0xF00F, 0x8007, oper_rrsub},
  {0xF00F, 0x800E, oper_lsl},
  {0xF00F, 0x9000, oper_rnequ},
  {0xF000, 0xA000, oper_iset},
  {0xF000, 0xB000, oper_rjmp},
  {0xF000, 0xC000, oper_arnd},
  {0xF000, 0xD000, oper_draw},
  {0xF0FF, 0xE09E, oper_kequ},
  {0xF0FF, 0xE0A1, oper_knequ},
  {0xF0FF, 0xF007, oper_getd},
  {0xF0FF, 0xF00A, oper_getk},
  {0xF0FF, 0xF015, oper_setd},
  {0xF0FF, 0xF018, oper_sets},
  {0xF0FF, 0xF01E, oper_iadd},
  {0xF0FF, 0xF029, oper_font},
  {0xF0FF, 0xF033, oper_bcd},
  {0xF0FF, 0xF055, oper_save},
  {0xF0FF, 0xF065, oper_load},
  {0xF000, 0x0000, oper_crca},
  {0, 0, NULL}
};

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
