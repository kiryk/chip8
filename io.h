#define NO_KEY 17
#define KEYN 16

#define PIXX 12
#define PIXY 12

extern int refresh;

void           io_init(void);
void           io_deinit(void);
void           io_screen(void);
void           io_check_quit(void);
unsigned char  io_key_check(int);
unsigned short io_key_wait(void);
void           io_timers(void);
void           io_delay(int i);
