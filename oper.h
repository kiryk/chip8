void oper_clr(unsigned short code);   /* clear the screen */
void oper_ret(unsigned short code);   /* return from subroutine */
void oper_jmp(unsigned short code);   /* jump to address */
void oper_call(unsigned short code);  /* run subroutine */
void oper_equ(unsigned short code);   /* skip next if reg equals number */
void oper_nequ(unsigned short code);  /* skip next if reg doesn't equal number */
void oper_requ(unsigned short code);  /* skip next if two regs contain same value */
void oper_set(unsigned short code);   /* save number in reg */
void oper_add(unsigned short code);   /* increment reg's value by number */
void oper_setr(unsigned short code);  /* save reg's value in another reg */
void oper_or(unsigned short code);    /* bitwise or operation */
void oper_and(unsigned short code);   /* bitwise and operation */
void oper_xor(unsigned short code);   /* bitwise xor operation */
void oper_radd(unsigned short code);  /* increment reg's value by value of another reg */
void oper_rsub(unsigned short code);  /* decrement reg's value by value of another reg */
void oper_lsr(unsigned short code);   /* shift register by one bit right */
void oper_rrsub(unsigned short code); /* substract value of two regs, and save it to the first one */
void oper_lsl(unsigned short code);   /* shift register by one bit left */
void oper_rnequ(unsigned short code); /* skip next if two regs don't contain the same value */
void oper_iset(unsigned short code);  /* save address in register I */
void oper_rjmp(unsigned short code);  /* jump to the given address incremented by reg v0 */
void oper_arnd(unsigned short code);  /* save random (0-255) value in reg */
void oper_draw(unsigned short code);  /* draw sprite at given coordinates */
void oper_kequ(unsigned short code);  /* skip next if reg's value equals pressed key */
void oper_knequ(unsigned short code); /* skip next if reg's value doesn't equal pressed key */
void oper_getd(unsigned short code);  /* save value of delay timer in reg */
void oper_getk(unsigned short code);  /* wait for key pressed, and save its keycode to reg */
void oper_setd(unsigned short code);  /* save reg's value in delay timer */
void oper_sets(unsigned short code);  /* save reg's value in sound timer */
void oper_iadd(unsigned short code);  /* increment reg I by given reg's value */
void oper_font(unsigned short code);  /* set I to address of sprite of character in reg */
void oper_bcd(unsigned short code);   /* save binary-coded decimal value of reg in I */
void oper_save(unsigned short code);  /* save all general purpose regs in memory at address in I */
void oper_load(unsigned short code);  /* load all general puprose regs from memory address in I */
void oper_crca(unsigned short code);  /* -- "call RCA 1802 program", not supported -- */
