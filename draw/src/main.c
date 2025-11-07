#include "./header.h"

int last_frame_time = 0;
int game_is_running = 0;
int mouse_x, mouse_y;

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

struct color {
	int r, g, b, a;
};

struct pixel {
	struct color c;
	SDL_Rect rect;
};

struct canvas {
	int rows, cols;
	int pixel_size;
	bool is_drawing;
	struct pixel** grid;

	SDL_Rect rect;
	SDL_Texture* texture;
} canvas;

struct button {
	SDL_Rect rect;
	bool clicked;
};

struct color chosen_color;

struct button increase_rows;
struct button decrease_rows;
struct button increase_cols;
struct button decrease_cols;

int initialize_window(void) {
	if(SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		fprintf(stderr, "Error initializing SDL\n");
		return 0;
	}

	if(IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) {
		fprintf(stderr, "Error initializing SDL_Image\n");
		return 0;
	}

	window = SDL_CreateWindow(
		"draw",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		SCREEN_WIDTH,
		SCREEN_HEIGHT,
		0
	);

	if(window == NULL) {
		fprintf(stderr, "Error creating SDL Window.\n");
		return 0;
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
	if(!renderer) {
		fprintf(stderr, "Error creating SDL Renderer.\n");
		return 0;
	}

	return 1;
}

void setup() {
	canvas.rows = 12;
	canvas.cols = 12;
	canvas.pixel_size = 50;
	canvas.is_drawing = false;

	canvas.grid = calloc(canvas.rows, sizeof(struct pixel*));
	for(int i = 0; i < canvas.rows; i++) {
		canvas.grid[i] = calloc(canvas.cols, sizeof(struct pixel));
		for (int j = 0; j < canvas.cols; j++) {
			canvas.grid[i][j].c.r = 255;
			canvas.grid[i][j].c.g = 255;
			canvas.grid[i][j].c.b = 255;
			canvas.grid[i][j].c.a = SDL_ALPHA_OPAQUE;
			canvas.grid[i][j].rect.w = canvas.pixel_size;
			canvas.grid[i][j].rect.h = canvas.pixel_size;
			canvas.grid[i][j].rect.x = j * canvas.pixel_size;
			canvas.grid[i][j].rect.y = i * canvas.pixel_size;
		}
	}

	chosen_color.r = 0;
	chosen_color.g = 212;
	chosen_color.b = 24;
	chosen_color.a = SDL_ALPHA_OPAQUE;

	canvas.texture = SDL_CreateTexture(
		renderer,
		SDL_PIXELFORMAT_RGBA8888,
		SDL_TEXTUREACCESS_TARGET,
		canvas.cols * canvas.pixel_size,
		canvas.rows * canvas.pixel_size
	);

	if(!canvas.texture) {
		fprintf(stderr, "Failed to create canvas texture: %s\n", SDL_GetError());
		exit(1);
	}

	canvas.rect.x = (SCREEN_WIDTH - CANVAS_WIDTH)/2;
	canvas.rect.y = (SCREEN_HEIGHT - CANVAS_HEIGHT)/2;
	canvas.rect.w = CANVAS_WIDTH;
	canvas.rect.h = CANVAS_HEIGHT;

	// setup buttons
	increase_rows.rect.x = 10;
	increase_rows.rect.y = 10;
	increase_rows.rect.w = 25;
	increase_rows.rect.h = 15;
	increase_rows.clicked = false;
	
	decrease_rows.rect.x = 40;
	decrease_rows.rect.y = 10;
	decrease_rows.rect.w = 25;
	decrease_rows.rect.h = 15;
	decrease_rows.clicked = false;
	
	increase_cols.rect.x = 10;
	increase_cols.rect.y = 30;
	increase_cols.rect.w = 25;
	increase_cols.rect.h = 15;
	increase_cols.clicked = false;
	
	decrease_cols.rect.x = 40;
	decrease_cols.rect.y = 30;
	decrease_cols.rect.w = 25;
	decrease_cols.rect.h = 15;
	decrease_cols.clicked = false;
	


	// fill the texture with white pixels
	SDL_SetRenderTarget(renderer, canvas.texture);
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderClear(renderer);
	SDL_SetRenderTarget(renderer, NULL);
}

void check_button_click(struct button* b) {
	if(mouse_x > b->rect.x && mouse_x < b->rect.x + b->rect.w &&
	   mouse_y > b->rect.y && mouse_y < b->rect.y + b->rect.h)
		b->clicked = true;
}

void process_input() {
	SDL_Event event;
	SDL_GetMouseState(&mouse_x, &mouse_y);

	while(SDL_PollEvent(&event)) {
		switch (event.type) {
			case SDL_QUIT:
				game_is_running = 0;
				break;
			case SDL_KEYDOWN:
				if (event.key.keysym.sym == SDLK_ESCAPE)
					game_is_running = 0;
				break;
			case SDL_MOUSEBUTTONDOWN:
				// start drawing; actual bounds-checking is done in update (after mapping)
				canvas.is_drawing = true;
				check_button_click(&increase_rows);
				check_button_click(&decrease_rows);
				check_button_click(&increase_cols);
				check_button_click(&decrease_cols);

				break;
			case SDL_MOUSEBUTTONUP:
				canvas.is_drawing = false;
				break;
		}
	}
}

void update() {
	int time_to_wait = MS_PER_FRAME - (SDL_GetTicks() - last_frame_time);
	if(time_to_wait > 0 && time_to_wait <= MS_PER_FRAME)
		SDL_Delay(time_to_wait);

	if(canvas.is_drawing) {
		// map mouse position to canvas texture coordinates
		int rel_x = mouse_x - canvas.rect.x;
		int rel_y = mouse_y - canvas.rect.y;

		// only draw if inside the canvas rect
		if(rel_x >= 0 && rel_x < canvas.rect.w && rel_y >= 0 && rel_y < canvas.rect.h) {
			// scale mouse coords to texture pixel coords
			float scale_x = (float)(canvas.cols * canvas.pixel_size) / (float)canvas.rect.w;
			float scale_y = (float)(canvas.rows * canvas.pixel_size) / (float)canvas.rect.h;

			int tex_x = (int)(rel_x * scale_x);
			int tex_y = (int)(rel_y * scale_y);

			int row = tex_y / canvas.pixel_size;
			int col = tex_x / canvas.pixel_size;

			if(row >= 0 && row < canvas.rows && col >= 0 && col < canvas.cols) {
				struct pixel* clicked_pixel = &canvas.grid[row][col];
				clicked_pixel->c = chosen_color;
			}
			
		}
	}
		

	if(increase_rows.clicked) {

		canvas.rows++;

		// make new grid
		struct pixel** new_grid = calloc(canvas.rows, sizeof(struct pixel*));

		for(int i = 0; i < canvas.rows; i++) {
			new_grid[i] = calloc(canvas.cols, sizeof(struct pixel));
			for(int j = 0; j < canvas.cols; j++) {
				// copy old grid over
				if(i != canvas.rows-1) {
					new_grid[i][j].c.r = canvas.grid[i][j].c.r;
					new_grid[i][j].c.g = canvas.grid[i][j].c.g;
					new_grid[i][j].c.b = canvas.grid[i][j].c.b;
					new_grid[i][j].c.a = canvas.grid[i][j].c.a;
					new_grid[i][j].rect.w = canvas.grid[i][j].rect.w;
					new_grid[i][j].rect.h = canvas.grid[i][j].rect.h;
					new_grid[i][j].rect.x = canvas.grid[i][j].rect.x;
					new_grid[i][j].rect.y = canvas.grid[i][j].rect.y;

				}
				else {
					new_grid[i][j].c.r = 255;
					new_grid[i][j].c.g = 255;
					new_grid[i][j].c.b = 255;
					new_grid[i][j].c.a = SDL_ALPHA_OPAQUE;
					new_grid[i][j].rect.w = canvas.pixel_size;
					new_grid[i][j].rect.h = canvas.pixel_size;
					new_grid[i][j].rect.x = j * canvas.pixel_size;
					new_grid[i][j].rect.y = i * canvas.pixel_size;

				}

			}
		}

		// free old grid
		for(int i = 0; i < canvas.rows-1; i++) {
			free(canvas.grid[i]);
		}
		free(canvas.grid);
		
		canvas.grid = new_grid;
		for(int i = 0; i < canvas.rows-1; i++) {
			canvas.grid[i] = new_grid[i];
		}

		SDL_DestroyTexture(canvas.texture);
		canvas.texture = SDL_CreateTexture(renderer,SDL_PIXELFORMAT_RGBA8888,SDL_TEXTUREACCESS_TARGET,canvas.cols * canvas.pixel_size,canvas.rows * canvas.pixel_size);

		SDL_SetRenderTarget(renderer, canvas.texture);
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_RenderClear(renderer);
		SDL_SetRenderTarget(renderer, NULL);

		increase_rows.clicked = false;
	}
	if(canvas.rows > 1 && decrease_rows.clicked) {

		canvas.rows--;

		// make new grid
		struct pixel** new_grid = calloc(canvas.rows, sizeof(struct pixel*));

		for(int i = 0; i < canvas.rows; i++) {
			new_grid[i] = calloc(canvas.cols, sizeof(struct pixel));
			for(int j = 0; j < canvas.cols; j++) {
				// copy old grid over
				new_grid[i][j].c.r = canvas.grid[i][j].c.r;
				new_grid[i][j].c.g = canvas.grid[i][j].c.g;
				new_grid[i][j].c.b = canvas.grid[i][j].c.b;
				new_grid[i][j].c.a = canvas.grid[i][j].c.a;
				new_grid[i][j].rect.w = canvas.grid[i][j].rect.w;
				new_grid[i][j].rect.h = canvas.grid[i][j].rect.h;
				new_grid[i][j].rect.x = canvas.grid[i][j].rect.x;
				new_grid[i][j].rect.y = canvas.grid[i][j].rect.y;

			}
		}

		// free old grid
		for(int i = 0; i < canvas.rows+1; i++) {
			free(canvas.grid[i]);
		}
		free(canvas.grid);
		
		canvas.grid = new_grid;
		for(int i = 0; i < canvas.rows; i++) {
			canvas.grid[i] = new_grid[i];
		}

		SDL_DestroyTexture(canvas.texture);
		canvas.texture = SDL_CreateTexture(renderer,SDL_PIXELFORMAT_RGBA8888,SDL_TEXTUREACCESS_TARGET,canvas.cols * canvas.pixel_size,canvas.rows * canvas.pixel_size);

		SDL_SetRenderTarget(renderer, canvas.texture);
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_RenderClear(renderer);
		SDL_SetRenderTarget(renderer, NULL);

		decrease_rows.clicked = false;
	}
	if(increase_cols.clicked) {

			canvas.cols++;

			// make new grid
			struct pixel** new_grid = calloc(canvas.rows, sizeof(struct pixel*));

			for(int i = 0; i < canvas.rows; i++) {
				new_grid[i] = calloc(canvas.cols, sizeof(struct pixel));
				for(int j = 0; j < canvas.cols; j++) {
					// copy old grid over
					if(j != canvas.cols-1) {
						new_grid[i][j].c.r = canvas.grid[i][j].c.r;
						new_grid[i][j].c.g = canvas.grid[i][j].c.g;
						new_grid[i][j].c.b = canvas.grid[i][j].c.b;
						new_grid[i][j].c.a = canvas.grid[i][j].c.a;
						new_grid[i][j].rect.w = canvas.grid[i][j].rect.w;
						new_grid[i][j].rect.h = canvas.grid[i][j].rect.h;
						new_grid[i][j].rect.x = canvas.grid[i][j].rect.x;
						new_grid[i][j].rect.y = canvas.grid[i][j].rect.y;

					}
					else {
						new_grid[i][j].c.r = 255;
						new_grid[i][j].c.g = 255;
						new_grid[i][j].c.b = 255;
						new_grid[i][j].c.a = SDL_ALPHA_OPAQUE;
						new_grid[i][j].rect.w = canvas.pixel_size;
						new_grid[i][j].rect.h = canvas.pixel_size;
						new_grid[i][j].rect.x = j * canvas.pixel_size;
						new_grid[i][j].rect.y = i * canvas.pixel_size;

					}

				}
			}

			// free old grid
			for(int i = 0; i < canvas.rows; i++) {
				free(canvas.grid[i]);
			}
			free(canvas.grid);
			
			canvas.grid = new_grid;
			for(int i = 0; i < canvas.rows; i++) {
				canvas.grid[i] = new_grid[i];
			}

			SDL_DestroyTexture(canvas.texture);
			canvas.texture = SDL_CreateTexture(renderer,SDL_PIXELFORMAT_RGBA8888,SDL_TEXTUREACCESS_TARGET,canvas.cols * canvas.pixel_size,canvas.rows * canvas.pixel_size);

			SDL_SetRenderTarget(renderer, canvas.texture);
			SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
			SDL_RenderClear(renderer);
			SDL_SetRenderTarget(renderer, NULL);

			increase_cols.clicked = false;
		}
		if(canvas.cols > 1 && decrease_cols.clicked) {

			canvas.cols--;

			// make new grid
			struct pixel** new_grid = calloc(canvas.rows, sizeof(struct pixel*));

			for(int i = 0; i < canvas.rows; i++) {
				new_grid[i] = calloc(canvas.cols, sizeof(struct pixel));
				for(int j = 0; j < canvas.cols; j++) {
					// copy old grid over
					new_grid[i][j].c.r = canvas.grid[i][j].c.r;
					new_grid[i][j].c.g = canvas.grid[i][j].c.g;
					new_grid[i][j].c.b = canvas.grid[i][j].c.b;
					new_grid[i][j].c.a = canvas.grid[i][j].c.a;
					new_grid[i][j].rect.w = canvas.grid[i][j].rect.w;
					new_grid[i][j].rect.h = canvas.grid[i][j].rect.h;
					new_grid[i][j].rect.x = canvas.grid[i][j].rect.x;
					new_grid[i][j].rect.y = canvas.grid[i][j].rect.y;

				}
			}

			// free old grid
			for(int i = 0; i < canvas.rows; i++) {
				free(canvas.grid[i]);
			}
			free(canvas.grid);
			
			canvas.grid = new_grid;
			for(int i = 0; i < canvas.rows; i++) {
				canvas.grid[i] = new_grid[i];
			}

			SDL_DestroyTexture(canvas.texture);
			canvas.texture = SDL_CreateTexture(renderer,SDL_PIXELFORMAT_RGBA8888,SDL_TEXTUREACCESS_TARGET,canvas.cols * canvas.pixel_size,canvas.rows * canvas.pixel_size);

			SDL_SetRenderTarget(renderer, canvas.texture);
			SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
			SDL_RenderClear(renderer);
			SDL_SetRenderTarget(renderer, NULL);

			decrease_cols.clicked = false;
		}

	last_frame_time = SDL_GetTicks();
}


void render() {

	// render pixels to texture
	SDL_SetRenderTarget(renderer, canvas.texture);
	for(int i = 0; i < canvas.rows; i++) {
		for(int j = 0; j < canvas.cols; j++) {
			struct pixel* p = &canvas.grid[i][j];
			
			SDL_SetRenderDrawColor(renderer, p->c.r, p->c.g, p->c.b, p->c.a);
			SDL_RenderFillRect(renderer, &p->rect);

		}
	}
	SDL_SetRenderTarget(renderer, NULL);

	// render the canvas texture to the screen (stretches/scales to window)
	SDL_RenderCopy(renderer, canvas.texture, NULL, &canvas.rect);


	// render buttons
	SDL_SetRenderDrawColor(renderer, 211, 211, 211, 255);
	SDL_RenderFillRect(renderer, &increase_rows.rect);
	SDL_RenderFillRect(renderer, &decrease_rows.rect);
	SDL_RenderFillRect(renderer, &increase_cols.rect);
	SDL_RenderFillRect(renderer, &decrease_cols.rect);

	SDL_RenderPresent(renderer);
}

void destroy_window() {
	for (int i = 0; i < canvas.rows; i++)
		free(canvas.grid[i]);
	free(canvas.grid);

	if (canvas.texture) SDL_DestroyTexture(canvas.texture);
	if (renderer) SDL_DestroyRenderer(renderer);
	if (window) SDL_DestroyWindow(window);
	SDL_Quit();
}

int main() {
	game_is_running = initialize_window();
	setup();

	while (game_is_running) {
		process_input();
		update();
		render();
	}

	destroy_window();
	return 0;
}
