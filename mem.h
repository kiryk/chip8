#define TRUE  1
#define FALSE 0

#define MEMB 0x1000 /* memory bytes */
#define SCRB 0x100  /* screen bytes */
#define SCRX 64     /* screen width */
#define SCRY 32     /* screen height */

#define getword(s, n) (s >> n * 4 & 0xF)
#define getbyte(s, n) (s >> n * 4 & 0xFF)
#define getaddr(s, n) (s >> n * 4 & 0xFFF)

struct oper_t { /* opcode data */
	unsigned short mask;
	unsigned short code;
	void (*oper)(unsigned short);
};

/* mem.c */
extern const struct oper_t ops[]; /* opcodes' data */

extern unsigned char reg[]; /* registers */
extern unsigned char *vf;   /* address/flag register */
extern unsigned short regi, regs, regd; /* I reg, sound reg, delay reg */
extern int sp, pc; /* stack pointer, process counter */

extern unsigned short stack[];

extern unsigned char mem[];
extern unsigned char *scr[]; /* addresses to the screen's part of memory */
