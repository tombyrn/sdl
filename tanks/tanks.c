#include "includes.h"

void process_input(struct tank* player) {
    SDL_Event e;

    while(SDL_PollEvent(&e)) {
        if(e.type == SDL_QUIT) is_running = false;
        if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_CLOSE) is_running = false;
    }

    const Uint8* keystates = SDL_GetKeyboardState(NULL);
    if (keystates[SDL_SCANCODE_A]) { player->dx = -10; player->is_moving = true; }
    if (keystates[SDL_SCANCODE_D]) { player->dx =  10; player->is_moving = true; }
}

void init_tank(struct tank* player){
    player->rect = calloc(1, sizeof(SDL_Rect));
    player->rect->h = 15;
    player->rect->w = 50;
    player->rect->x = (SCREEN_WIDTH / 2) - (player->rect->w / 2);
    player->rect->y = (SCREEN_HEIGHT / 2) - (player->rect->h / 2);

    player->rect2 = calloc(1, sizeof(SDL_Rect));
    player->rect2->w = 0;

    player->window = 0;
    player->dy = 10;
    player->dx = 0;
    player->is_moving = false;
}

void update_tank(struct tank* player) {

    // move player down
    if(player->dy) {
        if(player->rect->y + player->rect->h < SCREEN_HEIGHT - ROOM_MARGIN - LINE_WIDTH) 
            player->rect->y += player->dy;

        else {
            // snap exactly to the floor in case of overshoot
            player->rect->y = SCREEN_HEIGHT - ROOM_MARGIN - player->rect->h - LINE_WIDTH; 
            player->dy = 0;
        }

    }
        
    // move player right/left
    if(player->dx){
        if(player->dx < 0)
            player->rect->x += player->dx; 
        if(player->dx > 0)
            player->rect->x += player->dx; 

        player->dx = 0;
        player->is_moving = false;
    }

    // check collision

    // UNDOCKED
    if(!docked) {
        player->rect2->w = 0;
        // left boundary
        if(player->rect->x <= ROOM_MARGIN + LINE_WIDTH)
            player->rect->x = ROOM_MARGIN + LINE_WIDTH;
        // right boundary
        if(player->rect->x + player->rect->w >= SCREEN_WIDTH - ROOM_MARGIN - LINE_WIDTH)
            player->rect->x = SCREEN_WIDTH - player->rect->w - ROOM_MARGIN - LINE_WIDTH;
    }

    // DOCKED
    else {

        // player in window 0
        if(player->window == 0) {
            // [1][0]
            if(left_idx) {
                // left boundary
                // moving from window0 to window1 to the left
                if(player->rect->x < 0 ) {
                    player->rect2->w = player->rect->w;
                    player->rect2->h = player->rect->h;
                    player->rect2->x = SCREEN_WIDTH - abs(player->rect->x);
                    player->rect2->y = SCREEN_HEIGHT - ROOM_MARGIN - player->rect->h - LINE_WIDTH;
                    // printf("0: w: %d\th: %d\tx: %d\ty: %d\n", player.rect2->w, player.rect2->h, player.rect2->x, player.rect2->y);
                }
                
                // keep left boundary blcoked
                if(player->rect->x + player->rect->w >= SCREEN_WIDTH - ROOM_MARGIN - LINE_WIDTH)
                    player->rect->x = SCREEN_WIDTH - player->rect->w - ROOM_MARGIN - LINE_WIDTH; 

                // switch screens
                if(player->rect->x < -1 * player->rect->w) {
                    printf("hey\n");
                    player->window = 1;
                    SDL_Rect* rect_tmp = player->rect;
                    player->rect = player->rect2;
                    player->rect2 = rect_tmp;
                    player->rect2->w = 0;
    
                    SDL_Renderer* renderer_tmp = player->renderer;
                    player->renderer = player->other_renderer;
                    player->other_renderer = renderer_tmp;  
                }
            }
            
            // [0][1]
            if(right_idx) {

                // moving from window0 to window1 to the right
                if(player->rect->x + player->rect->w > SCREEN_WIDTH) {
                    player->rect2->w = player->rect->w;
                    player->rect2->h = player->rect->h;
                    player->rect2->x = -1 * (SCREEN_WIDTH - player->rect->x);
                    player->rect2->y = SCREEN_HEIGHT - ROOM_MARGIN - player->rect->h - LINE_WIDTH;
                    // printf("1: w: %d\th: %d\tx: %d\ty: %d\n", player->rect2->w, player->rect2->h, player->rect2->x, player->rect2->y);
                }

                // keep right boundary blocked
                if(player->rect->x <= ROOM_MARGIN + LINE_WIDTH)
                    player->rect->x = ROOM_MARGIN + LINE_WIDTH;

                // switch screens
                if(player->rect->x >= SCREEN_WIDTH) {
                    printf("2hey\n");
                    player->window = 1;
                    SDL_Rect* rect_tmp = player->rect;
                    player->rect = player->rect2;
                    player->rect2 = rect_tmp;
                    player->rect2->w = 0;
    
                    SDL_Renderer* renderer_tmp = player->renderer;
                    player->renderer = player->other_renderer;
                    player->other_renderer = renderer_tmp; 
                }
            }
        }

        // player in window 1
        else {

            // [1][0]
            if(left_idx) {
                if(player->rect->x > SCREEN_WIDTH - player->rect->w) {
                    player->rect2->w = player->rect->w;
                    player->rect2->h = player->rect->h;
                    player->rect2->x = -1 * (SCREEN_WIDTH - player->rect->x);
                    player->rect2->y = SCREEN_HEIGHT - ROOM_MARGIN - player->rect->h - LINE_WIDTH;
                }

                if(player->rect->x < ROOM_MARGIN + LINE_WIDTH)
                    player->rect->x = ROOM_MARGIN + LINE_WIDTH;

                // switch screens
                if(player->rect->x >= SCREEN_WIDTH) {
                    printf("g\n");
                    player->window = 0;
                    SDL_Rect* rect_tmp = player->rect;
                    player->rect = player->rect2;
                    player->rect2 = rect_tmp;
                    player->rect2->w = 0;
    
                    SDL_Renderer* renderer_tmp = player->renderer;
                    player->renderer = player->other_renderer;
                    player->other_renderer = renderer_tmp;  

                }
            }

            // [0][1]
            if(right_idx) {

                if(player->rect->x < 0) {
                    player->rect2->w = player->rect->w;
                    player->rect2->h = player->rect->h;
                    player->rect2->x = SCREEN_WIDTH - abs(player->rect->x);
                    player->rect2->y = SCREEN_HEIGHT - ROOM_MARGIN - player->rect->h - LINE_WIDTH;
                }

                if(player->rect->x + player->rect->w >= SCREEN_WIDTH - ROOM_MARGIN - LINE_WIDTH) {
                    player->rect->x = SCREEN_WIDTH - ROOM_MARGIN - LINE_WIDTH - player->rect->w;
                }


                // switch screens
                if(player->rect->x + player->rect->w < 0) {
                    player->window = 0;
                    SDL_Rect* rect_tmp = player->rect;
                    player->rect = player->rect2;
                    player->rect2 = rect_tmp;
                    player->rect2->w = 0;
    
                    SDL_Renderer* renderer_tmp = player->renderer;
                    player->renderer = player->other_renderer;
                    player->other_renderer = renderer_tmp;  

                }
            }

        }

    }
}

void render_tank(struct tank* player) {
    // draw tank
    SDL_SetRenderDrawColor(player->renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(player->renderer, player->rect);
    if(player->rect2->w) {
        SDL_SetRenderDrawColor(player->other_renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
        SDL_RenderFillRect(player->other_renderer, player->rect2);
    }
    
    
    // pivot point for gun
    int tank_center_x = player->rect->x + player->rect->w / 2;
    int tank_center_y = player->rect->y ;
    
    // calculate angle to mouse
    float angle = atan2(mouse_y - tank_center_y, mouse_x - tank_center_x);

    int gun_length = 30;
    
    // end point of gun barrel
    int gun_end_x = tank_center_x + (int)(cos(angle) * gun_length);
    int gun_end_y = tank_center_y + (int)(sin(angle) * gun_length);
    
    // draw gun
    SDL_SetRenderDrawColor(player->renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderDrawLine(player->renderer, tank_center_x, tank_center_y, gun_end_x, gun_end_y);
}