#include "./header.h"

int map[MAP_HEIGHT][MAP_WIDTH] = {
    {1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,1,0,0,1},
    {1,0,0,0,0,1,0,0,1},
    {1,0,0,0,0,1,1,1,1},
    {1,0,0,0,0,0,0,0,1},
    {1,0,1,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1,1}
};

int last_frame_time = 0;
int game_is_running = 0;
int mouse_x, mouse_y;
int prev_mouse_x;

double distance, wall_height;

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

struct player {
    double x, y;
    double angle; // in degrees
    double fov;
} p;

void cast_ray(double rayAngle) {
    double x = p.x;
    double y = p.y;
    double dx = cos(rayAngle);
    double dy = sin(rayAngle);

    int i = 0;
    while (i < 1000) {
        int map_x = (int)floor(x);
        int map_y = (int)floor(y);
        if (map_x < 0 || map_x >= MAP_WIDTH || map_y < 0 || map_y >= MAP_HEIGHT)
            break;
        if (map[map_y][map_x] == 1)
            break;

        x += dx * 0.05;
        y += dy * 0.05;
        i++;
    }

    distance = sqrt((x - p.x)*(x - p.x) + (y - p.y)*(y - p.y));
    if (distance < 0.0001) distance = 0.0001; // avoid div by zero
    wall_height = 300 / distance;
}

void draw_wall_slice(int i, double wall_height, int slice_width) {
    for (int j = 0; j < wall_height; j++) {
        int y_pos = floor(300 - wall_height / 2 + j);
        SDL_SetRenderDrawColor(renderer, 5, 60, 50, SDL_ALPHA_OPAQUE);
        SDL_Rect r = {i * slice_width, y_pos, slice_width, 1};
        SDL_RenderFillRect(renderer, &r);
    }
}

void raycast() {
    int rays = 400;
    int slice_width = SCREEN_WIDTH / rays;
    double angle_step = p.fov / rays;

    for (int i = 0; i < rays; i++) {
        double ray_angle = (p.angle - (p.fov / 2) + i * angle_step) * DEG2RAD;
        cast_ray(ray_angle);
        draw_wall_slice(i, wall_height, slice_width);
    }
}

int initialize_window(void) {
    if(SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "Error initializing SDL: %s\n", SDL_GetError());
        return 0;
    }

    window = SDL_CreateWindow("Raycaster",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH, SCREEN_HEIGHT,
        SDL_WINDOW_SHOWN);

    if(!window) {
        fprintf(stderr, "Error creating SDL Window: %s\n", SDL_GetError());
        return 0;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if(!renderer) {
        fprintf(stderr, "Error creating SDL Renderer: %s\n", SDL_GetError());
        return 0;
    }
    
    // hide cursor & capture mouse motion
    SDL_SetRelativeMouseMode(SDL_TRUE); 
    SDL_GetMouseState(&prev_mouse_x, NULL);

    return 1;
}

void setup() {
    p.x = 1.5;
    p.y = 1.5;
    p.angle = 0;   // facing right
    p.fov = 90;
}

void process_input(float delta_time) {
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
        if(event.type == SDL_QUIT)
            game_is_running = 0;
        else if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)
            game_is_running = 0;
        else if(event.type == SDL_MOUSEMOTION) {
            // mouse look
            int dx = event.motion.xrel;
            p.angle += dx * 0.2; // sensitivity
            if (p.angle < 0) p.angle += 360;
            if (p.angle >= 360) p.angle -= 360;
        }
    }

    const Uint8* keystates = SDL_GetKeyboardState(NULL);

    // wasd controls
    double move_step = PLAYER_SPEED * delta_time;
    double move_x = 0;
    double move_y = 0;

    double rad_angle = p.angle * DEG2RAD;
    double cos_a = cos(rad_angle);
    double sin_a = sin(rad_angle);

    if (keystates[SDL_SCANCODE_W]) {
        move_x += cos_a * move_step;
        move_y += sin_a * move_step;
    }
    if (keystates[SDL_SCANCODE_S]) {
        move_x -= cos_a * move_step;
        move_y -= sin_a * move_step;
    }
    if (keystates[SDL_SCANCODE_A]) {
        move_x += sin_a * move_step;
        move_y -= cos_a * move_step;
    }
    if (keystates[SDL_SCANCODE_D]) {
        move_x -= sin_a * move_step;
        move_y += cos_a * move_step;
    }

    // collision check
    int next_x = (int)(p.x + move_x);
    int next_y = (int)(p.y + move_y);

    if (map[(int)p.y][next_x] == 0)
        p.x += move_x;
    if (map[next_y][(int)p.x] == 0)
        p.y += move_y;
}

void update() {
    int time_to_wait = MS_PER_FRAME - (SDL_GetTicks() - last_frame_time);
    if(time_to_wait > 0 && time_to_wait <= MS_PER_FRAME)
        SDL_Delay(time_to_wait);

    float delta_time = (SDL_GetTicks() - last_frame_time) / 1000.0f;

    process_input(delta_time);

    last_frame_time = SDL_GetTicks();
}

void render() {
    SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
    SDL_RenderClear(renderer);
    raycast();
    SDL_RenderPresent(renderer);
}

void destroy_window() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int main() {
    game_is_running = initialize_window();	
    setup();

    while(game_is_running) {
        update();
        render();
    }

    destroy_window();
    return 0;
}
