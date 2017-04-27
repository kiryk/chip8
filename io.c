#include <SDL2/SDL.h>
#include "chip8.h"
#include "io.h"

static SDL_Keycode keys[] = {
  SDLK_LALT, SDLK_q, SDLK_w, SDLK_e,
  SDLK_a, SDLK_s, SDLK_d, SDLK_z,
  SDLK_x, SDLK_c, SDLK_r, SDLK_t,
  SDLK_f, SDLK_g, SDLK_v, SDLK_b
};

static SDL_Scancode scans[] = {
  SDL_SCANCODE_LALT, SDL_SCANCODE_Q, SDL_SCANCODE_W, SDL_SCANCODE_E,
  SDL_SCANCODE_A, SDL_SCANCODE_S, SDL_SCANCODE_D, SDL_SCANCODE_Q,
  SDL_SCANCODE_X, SDL_SCANCODE_C, SDL_SCANCODE_R, SDL_SCANCODE_T,
  SDL_SCANCODE_F, SDL_SCANCODE_G, SDL_SCANCODE_V, SDL_SCANCODE_B
};

static SDL_Window *window = NULL;
static SDL_Surface *screen = NULL, *color[2] = {NULL, NULL};

int refresh = FALSE;

void io_init(void)
{
  if (SDL_Init(SDL_INIT_EVERYTHING))
    exit(1);
  atexit(SDL_Quit);

  window = SDL_CreateWindow("CHIP-8 Emulation",
    SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
    SCRX * PIXX, SCRY * PIXY,
    SDL_WINDOW_SHOWN);
  #ifdef FULLSCREEN
    SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
  #endif
  if (!window)
    exit(1);
  screen = SDL_GetWindowSurface(window);

  if (!(color[0] = SDL_CreateRGBSurface(0, PIXX, PIXY, 32, 0, 0, 0, 0)))
    exit(1);
  SDL_FillRect(color[0], NULL, SDL_MapRGB(color[0]->format, 0, 0, 0));

  if (!(color[1] = SDL_CreateRGBSurface(0, PIXX, PIXY, 32, 0, 0, 0, 0)))
    exit(1);
  SDL_FillRect(color[1], NULL, SDL_MapRGB(color[0]->format, 255, 255, 255));
}

void io_deinit(void)
{
  if (color[0])
    SDL_FreeSurface(color[0]);
  if (color[1])
    SDL_FreeSurface(color[1]);
  if (window)
    SDL_DestroyWindow(window);
  SDL_Quit();
}

void io_screen(void)
{
  int x, y;
  SDL_Rect rect;

  if (refresh)
    refresh = FALSE;
  else
    return;

  for (x = 0; x < SCRX; ++x) {
    for (y = 0; y < SCRY; ++y) {
      rect.x = x * PIXX;
      rect.y = y * PIXY;
      rect.w = PIXX;
      rect.h = PIXY;
      SDL_BlitSurface(color[(scr[y][x/8] >> (7 - x%8)) & 1],
        NULL, screen, &rect);
    }
  }
  SDL_UpdateWindowSurface(window);
}

void io_check_quit(void)
{
  static unsigned char *keystate;
  static SDL_Event event;

  keystate = SDL_GetKeyboardState(NULL);

  SDL_PollEvent(&event);
  if (event.type == SDL_QUIT)
    exit(0);
  if (keystate[SDL_SCANCODE_ESCAPE])
    exit(0);
}

unsigned short io_key_wait(void)
{
  static unsigned short i;
  static SDL_Event event;

  for (;;) {
    if (SDL_WaitEvent(&event) &&
        (event.type == SDL_KEYDOWN || event.type == SDL_QUIT)) {
      if (event.type == SDL_QUIT)
	exit(0);
      if (event.key.keysym.sym == SDLK_ESCAPE)
        exit(0);
      for (i = 0; i < KEYN; ++i)
        if (keys[i] == event.key.keysym.sym)
          return i;
    }
  }

  return 0;
}

unsigned char io_key_check(int k)
{
  static unsigned char *keystate;
  static SDL_Event event;

  keystate = SDL_GetKeyboardState(NULL);
  SDL_PollEvent(&event);

  return keystate[scans[k]];
}

void io_timers(void)
{
  static unsigned long int t = 0;

  if (t + 16 < SDL_GetTicks()) {
    if (regs > 0)
      regs--;
    if (regd > 0)
      regd--;
    t = SDL_GetTicks();
  }
}

void io_delay(int i)
{
  SDL_Delay(i);
}
