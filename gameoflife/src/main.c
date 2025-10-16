#include "./header.h"

int last_frame_time = 0;
int game_is_running = 0;
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
int window_width = SCREEN_WIDTH;
int window_height = SCREEN_HEIGHT;

int num_cols = NUM_COLS;
int num_rows = NUM_ROWS;
int mouse_x, mouse_y;
bool mouse_clicked = false; 



struct cell ** colony;

int initialize_window(void) {

	if(SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		fprintf(stderr, "Error initializing SDL\n");
		return 0;
	}

	if(IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) {
		fprintf(stderr, "Error initializing SDL Image\n");
		return 0;
	}

	window = SDL_CreateWindow(
		NULL, 
		SDL_WINDOWPOS_CENTERED, 
		SDL_WINDOWPOS_CENTERED, 
		SCREEN_WIDTH, 
		SCREEN_HEIGHT,
		SDL_WINDOW_SHOWN |
		SDL_WINDOW_INPUT_FOCUS |
		SDL_WINDOW_RESIZABLE
	);

	if(window == NULL) {
		fprintf(stderr, "Error creating SDL Window.\n");
		return 0;
	}
	
	renderer = SDL_CreateRenderer(window, -1, 0);
	if(!renderer) {
		fprintf(stderr, "Error creating SDL Renderer.\n");
		return 0;
	}

	return 1;	
}

void setup() {
	srand(time(NULL)); 
	game_is_running = initialize_window();	

	colony = calloc(NUM_ROWS, sizeof(struct cell*));
	for(int i = 0; i < NUM_ROWS; i++) {
		colony[i] = calloc(NUM_COLS, sizeof(struct cell));
	}

	for(int i = 0; i < NUM_ROWS; i++) {
		for(int j = 0; j < NUM_COLS; j++) {
			colony[i][j].alive = rand() % 10 > 8 ? true : false;
			colony[i][j].changed = false;
			colony[i][j].r.x = j * CELL_WIDTH;
			colony[i][j].r.y = i * CELL_HEIGHT;
			colony[i][j].r.w = CELL_WIDTH;
			colony[i][j].r.h = CELL_HEIGHT;

		}
	}

	SDL_AddTimer(TIMER_DELAY, next_generation, NULL);
	SDL_AddEventWatch(resize_event, NULL);

}

int resize_event(void *userdata, SDL_Event* event) {
  if (event->type == SDL_WINDOWEVENT) {
    if (event->window.event == SDL_WINDOWEVENT_RESIZED) {
		int new_width = event->window.data1;
		int new_height = event->window.data2;

		window_width = new_width;
		window_height = new_height;

		int new_cols = window_width / CELL_WIDTH;
		int new_rows = window_height / CELL_HEIGHT;

		// allocate new colony
		struct cell **new_colony = calloc(new_rows, sizeof(struct cell *));
		for (int i = 0; i < new_rows; i++) {
			new_colony[i] = calloc(new_cols, sizeof(struct cell));
		}

		// copy overlapping data from old colony
		int min_rows = (new_rows < num_rows) ? new_rows : num_rows;
		int min_cols = (new_cols < num_cols) ? new_cols : num_cols;

		for (int i = 0; i < min_rows; i++) {
			for (int j = 0; j < min_cols; j++) {
				new_colony[i][j].alive = colony[i][j].alive;
			}
		}

		// free old colony
		for (int i = 0; i < num_rows; i++) {
			free(colony[i]);
		}
		free(colony);

		colony = new_colony;
		num_cols = new_cols;
		num_rows = new_rows;

		// recalculate rectangle positions and sizes
		for (int i = 0; i < num_rows; i++) {
			for (int j = 0; j < num_cols; j++) {
				colony[i][j].r.x = j * CELL_WIDTH;
				colony[i][j].r.y = i * CELL_HEIGHT;
				colony[i][j].r.w = CELL_WIDTH;
				colony[i][j].r.h = CELL_HEIGHT;
			}
		}

		// printf("resized colony to %d rows x %d cols\n", num_rows, num_cols);
		update();
    	render();
    }
  }
  return 1;
}

void process_input() {
	SDL_Event event;
	while(SDL_PollEvent(&event)) {
		switch(event.type) {
			case SDL_QUIT:
				game_is_running = 0;
				break;
			case SDL_KEYDOWN:
				if(event.key.keysym.sym == SDLK_ESCAPE)
					game_is_running = 0;
				break;
			case SDL_WINDOWEVENT:
				if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
					
				}
				break;

			case SDL_MOUSEBUTTONDOWN:
				mouse_clicked = true;
				break;
			case SDL_MOUSEBUTTONUP:
				mouse_clicked = false;
				break;
		}
	}
}

int get_live_neighbors(int i, int j) {
	int total = 0;
	for (int y = -1; y <= 1; y++) {
		for (int x = -1; x <= 1; x++) {
			if (y == 0 && x == 0) continue;
			int ny = i + y;
			int nx = j + x;
			if (ny >= 0 && ny < num_rows && nx >= 0 && nx < num_cols) {
				if (colony[ny][nx].alive) total++;
			}
		}
	}
	return total;
}

unsigned int next_generation(unsigned int a, void* p) {
	struct cell** next_colony = calloc(num_rows, sizeof(struct cell*));
	for(int i = 0; i < num_rows; i++) {
		next_colony[i] = calloc(num_cols, sizeof(struct cell));
		for(int j = 0; j < num_cols; j++) {
			next_colony[i][j].alive = colony[i][j].alive;
			next_colony[i][j].r = colony[i][j].r;
		}
	}

	//  calculate next generation
	for(int i = 0; i < num_rows; i++) {
		for(int j = 0; j < num_cols; j++) {
			int live_neighbors = get_live_neighbors(i, j);

			if(colony[i][j].alive){
                // Rules 1-3
                if(live_neighbors < 2 || live_neighbors > 3)
                    next_colony[i][j].alive = false;
            }
            // Rule 4   
            else if(live_neighbors == 3){
                    next_colony[i][j].alive = true;
            }
		}
	}

	for(int i = 0; i < num_rows; i++) {
		for(int j = 0; j < num_cols; j++) {
			colony[i][j] = next_colony[i][j];
		}
		free(next_colony[i]);
	}

	free(next_colony);
	
	return a;
}


void update() {
	int time_to_wait = MS_PER_FRAME - (SDL_GetTicks() - last_frame_time);
	if( time_to_wait > 0 && time_to_wait <= MS_PER_FRAME)
		SDL_Delay(time_to_wait);
	
	// get a delta time factor converted to seconds to be used to update objects displayed
	// float delta_time = (SDL_GetTicks64() - last_frame_time) / 1000.0f;
	// next_generation();

	if(mouse_clicked){
		SDL_GetMouseState(&mouse_x, &mouse_y);
		int x = (mouse_x/CELL_WIDTH);
		int y = (mouse_y/CELL_HEIGHT);
		// printf("mx: %d, my: %d\n", mouse_x, mouse_y);
		// printf("x: %d, y: %d\n", x, y);
		
		if (x >= 0 && x < num_cols && y >= 0 && y < num_rows && colony[y][x].changed == false){
			// printf("alive: %d -> %d\n\n", (int)colony[y][x].alive, (int)!colony[y][x].alive);
			colony[y][x].alive = !colony[y][x].alive;
			colony[y][x].changed = true;
		}
	}

	// printf("ttw: %d\t dt: %f\n", time_to_wait, delta_time);

	last_frame_time = SDL_GetTicks();
}

void render() {
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(renderer);

	// render grid of rectangles
	for(int i = 0; i < num_rows; i++) {
		for(int j = 0; j < num_cols; j++) {
			SDL_SetRenderDrawColor(renderer, 0, colony[i][j].alive ? 222 : 0, 0, SDL_ALPHA_OPAQUE);
			SDL_RenderFillRect(renderer, &colony[i][j].r); 
		}
	}

	SDL_RenderPresent(renderer);
}

void destroy_window() {
	for(int i = 0; i < num_rows; i++) {
		if(colony[i]) free(colony[i]);
	}
	if(colony) free(colony);

	printf("destroction\n");
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

int main() {

	setup();

	while(game_is_running) {

		process_input();

		update();

		render();

	}

	
	destroy_window();
	return 0;
}
