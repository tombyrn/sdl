#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define SCREEN_WIDTH 900
#define SCREEN_HEIGHT  700

#define CANVAS_WIDTH (canvas.pixel_size * canvas.cols)
#define CANVAS_HEIGHT (canvas.pixel_size * canvas.rows)

#define FPS 60
#define MS_PER_FRAME (1000 / FPS)