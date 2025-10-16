#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT  800

#define CELL_WIDTH 25
#define CELL_HEIGHT 25

#define NUM_ROWS (SCREEN_HEIGHT / CELL_HEIGHT)
#define NUM_COLS (SCREEN_WIDTH / CELL_WIDTH)

#define TOTAL_CELLS (CELLS_PER_ROW * CELLS_PER_COL)

#define FPS 60
#define MS_PER_FRAME (1000 / FPS)

#define TIMER_DELAY 2000

struct cell {
	bool alive;
	bool changed;
	SDL_Rect r;
};

unsigned int next_generation(unsigned int a, void* p);
int resize_event(void *userdata, SDL_Event* event);
void update();
void render();