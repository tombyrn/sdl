#include "includes.h"

bool is_running = true;
bool docked = false;
int left_idx, right_idx;
int mouse_x, mouse_y;

int main() {

    SDL_Init(SDL_INIT_VIDEO);
    struct window_properties windows[NUM_WINDOWS] = {0};
    init_windows(windows);


    struct tank player;
    init_tank(&player);
    player.renderer = windows[player.window].renderer;
    player.other_renderer = windows[1].renderer;


    SDL_Event e;

    int room_x = ROOM_MARGIN;
    int room_y = ROOM_MARGIN;
    int room_w = SCREEN_WIDTH - (2 * ROOM_MARGIN);
    int room_h = SCREEN_HEIGHT - (2 * ROOM_MARGIN);

    while(is_running) {

        process_input(&player);
        update_windows(windows);
        
        // draw room outlines
        for(int i = 0; i < NUM_WINDOWS; i++) {
            SDL_SetRenderDrawColor(windows[i].renderer, 142, 154, 143, 255);
            SDL_RenderClear(windows[i].renderer);
            
            
            if(!docked) {
                SDL_SetRenderDrawColor(windows[i].renderer, 80, 80, 80, 255);
                SDL_Rect room_outline = {room_x, room_y, room_w, room_h};
                SDL_RenderDrawRect(windows[i].renderer, &room_outline);
            }
            else {
                SDL_SetRenderDrawColor(windows[i].renderer, 0, 200, 0, 255);
                // draw ceiling
                SDL_RenderDrawLine(windows[i].renderer, ROOM_MARGIN, ROOM_MARGIN, SCREEN_WIDTH - ROOM_MARGIN, ROOM_MARGIN);
                // draw floor
                SDL_RenderDrawLine(windows[i].renderer, ROOM_MARGIN, SCREEN_HEIGHT - ROOM_MARGIN, SCREEN_WIDTH - ROOM_MARGIN, SCREEN_HEIGHT - ROOM_MARGIN);
                
                if(left_idx == i) // draw left border
                    SDL_RenderDrawLine(windows[i].renderer, ROOM_MARGIN, ROOM_MARGIN, ROOM_MARGIN, SCREEN_HEIGHT - ROOM_MARGIN);
                else // draw right border
                    SDL_RenderDrawLine(windows[i].renderer, SCREEN_WIDTH - ROOM_MARGIN, ROOM_MARGIN, SCREEN_WIDTH - ROOM_MARGIN, SCREEN_HEIGHT - ROOM_MARGIN);

            }

            
            SDL_SetRenderDrawColor(windows[i].renderer, 60, 60, 60, SDL_ALPHA_OPAQUE);
        }


        // update tank
        update_tank(&player);

        // printf("leftIDX: %d\trightIDX: %d\n", left_idx, right_idx);

        // get mouse position
        SDL_GetMouseState(&mouse_x, &mouse_y);

        render_tank(&player);
        
        render_windows(windows);

        SDL_Delay(50);
    }

    for(int i = 0; i < NUM_WINDOWS; i++) {
        SDL_DestroyRenderer(windows[i].renderer);
        SDL_DestroyWindow(windows[i].window);
    }

    SDL_Quit();
    return 0;
}