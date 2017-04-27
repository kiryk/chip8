#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "chip8.h"
#include "io.h"

void oper_clr(unsigned short code)
{
  int x, y;

  for (x = SCRX / 8 - 1; x >= 0; x--)
    for (y = SCRY - 1; y >= 0; y--)
      scr[y][x] = 0;
}

void oper_ret(unsigned short code)
{
  if (sp >= 0)
    pc = stack[sp--];
}

void oper_jmp(unsigned short code)
{
  pc = getaddr(code, 0);
}

void oper_call(unsigned short code)
{
  if (sp < 0x10 - 1) {
    stack[++sp] = pc;
    pc = getaddr(code, 0);
  }
}

void oper_equ(unsigned short code)
{
  if (getbyte(code, 0) == reg[getword(code, 2)])
    pc += 2;
}

void oper_nequ(unsigned short code)
{
  if (getbyte(code, 0) != reg[getword(code, 2)])
    pc += 2;
}

void oper_requ(unsigned short code)
{
  if (reg[getword(code, 1)] == reg[getword(code, 2)])
    pc += 2;
}

void oper_set(unsigned short code)
{
  reg[getword(code, 2)] = getbyte(code, 0);
}

void oper_add(unsigned short code)
{
  reg[getword(code, 2)] += getbyte(code, 0);
}

void oper_setr(unsigned short code)
{
  reg[getword(code, 2)] = reg[getword(code, 1)];
}

void oper_or(unsigned short code)
{
  reg[getword(code, 2)] |= reg[getword(code, 1)];
}

void oper_and(unsigned short code)
{
  reg[getword(code, 2)] &= reg[getword(code, 1)];
}

void oper_xor(unsigned short code)
{
  reg[getword(code, 2)] ^= reg[getword(code, 1)];
}

void oper_radd(unsigned short code)
{
  if (reg[getword(code, 2)] + reg[getword(code, 1)] > 0xFF)
    *vf = 1;
  else
    *vf = 0;
  reg[getword(code, 2)] += reg[getword(code, 1)];
}

void oper_rsub(unsigned short code)
{
  if (reg[getword(code, 2)] - reg[getword(code, 1)] < 0x00)
    *vf = 0;
  else
    *vf = 1;
  reg[getword(code, 2)] -= reg[getword(code, 1)];
}

void oper_lsr(unsigned short code)
{
  *vf = reg[getword(code, 2)] & 0x01;
  reg[getword(code, 2)] >>= 1;
}

void oper_rrsub(unsigned short code)
{
  if (reg[getword(code, 1)] - reg[getword(code, 2)] < 0x00)
    *vf = 0;
  else
    *vf = 1;
  reg[getword(code, 2)] = reg[getword(code, 1)] - reg[getword(code, 2)];
}

void oper_lsl(unsigned short code)
{
  *vf = reg[getword(code, 2)] & 0x80;
  reg[getword(code, 2)] <<= 1;
}

void oper_rnequ(unsigned short code)
{
  if (reg[getword(code, 1)] != reg[getword(code, 2)])
    pc += 2;;
}

void oper_iset(unsigned short code)
{
  regi = getaddr(code, 0);
}

void oper_rjmp(unsigned short code)
{
  pc = reg[0] + getaddr(code, 0);
}

void oper_arnd(unsigned short code)
{
  reg[getword(code, 2)] = rand() & getbyte(code, 0);
}

void oper_draw(unsigned short code)
{
  int x = reg[getword(code, 2)];
  int y = reg[getword(code, 1)];
  int n = getword(code, 0), i;

  *vf = 0;
  for (i = 0; i < n; ++i, ++y) {
    if (y >= SCRY)
			break;
    if (*vf == 0 && (scr[y][x/8] & mem[regi + i] >> x % 8))
      *vf = 1;
    scr[y][x/8] ^= (mem[regi + i] >> x % 8) & 0xFF;

    if (x + 8 >= SCRX || x % 8 == 0)
      continue;
    if (*vf == 0 && (scr[y][x/8 + 1] & mem[regi + i] << (8 - x % 8)))
      *vf = 1;
    scr[y][x/8 + 1] ^= (mem[regi + i] << (8 - x % 8)) & 0xFF;
  }

  refresh = TRUE;
}

void oper_kequ(unsigned short code)
{
  if (io_key_check(reg[getword(code, 2)]))
    pc += 2;
}

void oper_knequ(unsigned short code)
{
  if (!io_key_check(reg[getword(code, 2)]))
    pc += 2;
}

void oper_getd(unsigned short code)
{
  reg[getword(code, 2)] = regd;
}

void oper_getk(unsigned short code)
{
  reg[getword(code, 2)] = io_key_wait();
}

void oper_setd(unsigned short code)
{
  regd = reg[getword(code, 2)];
}

void oper_sets(unsigned short code)
{
  regs = reg[getword(code, 2)];
}

void oper_iadd(unsigned short code)
{
  if (regi + reg[getword(code, 2)] > 0xFFF)
    *vf = 1;
  else
    *vf = 0;
  regi += reg[getword(code, 2)];
}

void oper_font(unsigned short code)
{
  if (reg[getword(code, 2)] <= 0xF)
    regi = reg[getword(code, 2)] * 5;
  else
    regi = 16 * 5;
}

void oper_bcd(unsigned short code)
{
  unsigned short tmp = reg[getword(code, 2)];

  mem[regi + 2] = tmp % 10, tmp /= 10;
  mem[regi + 1] = tmp % 10, tmp /= 10;
  mem[regi]     = tmp % 10;
}

void oper_save(unsigned short code)
{
  int i, n = getword(code, 2);

  for (i = 0; i <= n; ++i)
    mem[regi + i] = reg[i];
}

void oper_load(unsigned short code)
{
  int i, n = getword(code, 2);

  for (i = 0; i <= n; ++i)
    reg[i] = mem[regi + i];
}

void oper_crca(unsigned short code)
{
  return;
}
