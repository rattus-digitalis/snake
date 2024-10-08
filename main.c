#include "header.h"

typedef struct {
    int x;
    int y;
} Point;

typedef struct {
    Point position[100];
    int length;
    int dx;
    int dy;
} Snake;

typedef struct {
    Point position;
} Food;

void initSnake(Snake *snake) {
    snake->length = 5;
    snake->dx = TILE_SIZE;
    snake->dy = 0;
    for (int i = 0; i < snake->length; i++) {
        snake->position[i].x = WINDOW_WIDTH / 2 - i * TILE_SIZE;
        snake->position[i].y = WINDOW_HEIGHT / 2;
    }
}

void spawnFood(Food *food) {
    food->position.x = (rand() % (WINDOW_WIDTH / TILE_SIZE)) * TILE_SIZE;
    food->position.y = (rand() % (WINDOW_HEIGHT / TILE_SIZE)) * TILE_SIZE;
}

void drawSnake(SDL_Renderer *renderer, Snake *snake) {
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    for (int i = 0; i < snake->length; i++) {
        SDL_Rect rect = {snake->position[i].x, snake->position[i].y, TILE_SIZE, TILE_SIZE};
        SDL_RenderFillRect(renderer, &rect);
    }
}

void drawFood(SDL_Renderer *renderer, Food *food) {
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_Rect rect = {food->position.x, food->position.y, TILE_SIZE, TILE_SIZE};
    SDL_RenderFillRect(renderer, &rect);
}

void updateSnake(Snake *snake) {
    for (int i = snake->length - 1; i > 0; i--) {
        snake->position[i] = snake->position[i - 1];
    }
    snake->position[0].x += snake->dx;
    snake->position[0].y += snake->dy;
}

int checkCollision(Snake *snake) {
    // Collision avec les bords
    if (snake->position[0].x < 0 || snake->position[0].x >= WINDOW_WIDTH ||
        snake->position[0].y < 0 || snake->position[0].y >= WINDOW_HEIGHT) {
        return 1;
    }

    // Collision avec soi-même
    for (int i = 1; i < snake->length; i++) {
        if (snake->position[0].x == snake->position[i].x &&
            snake->position[0].y == snake->position[i].y) {
            return 1;
        }
    }
    return 0;
}

int checkFoodCollision(Snake *snake, Food *food) {
    return (snake->position[0].x == food->position.x && snake->position[0].y == food->position.y);
}

void growSnake(Snake *snake) {
    snake->length++;
}

int main(int argc, char *argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("Erreur lors de l'initialisation de la SDL: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("Snake", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        printf("Erreur lors de la création de la fenêtre: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("Erreur lors de la création du renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    srand(time(NULL));

    Snake snake;
    Food food;
    initSnake(&snake);
    spawnFood(&food);

    int running = 1;
    SDL_Event event;
    int delay = 100;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            } else if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_UP:
                        if (snake.dy == 0) {
                            snake.dx = 0;
                            snake.dy = -TILE_SIZE;
                        }
                        break;
                    case SDLK_DOWN:
                        if (snake.dy == 0) {
                            snake.dx = 0;
                            snake.dy = TILE_SIZE;
                        }
                        break;
                    case SDLK_LEFT:
                        if (snake.dx == 0) {
                            snake.dx = -TILE_SIZE;
                            snake.dy = 0;
                        }
                        break;
                    case SDLK_RIGHT:
                        if (snake.dx == 0) {
                            snake.dx = TILE_SIZE;
                            snake.dy = 0;
                        }
                        break;
                }
            }
        }

        updateSnake(&snake);

        if (checkCollision(&snake)) {
            printf("Game Over!\n");
            running = 0;
        }

        if (checkFoodCollision(&snake, &food)) {
            growSnake(&snake);
            spawnFood(&food);
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        drawSnake(renderer, &snake);
        drawFood(renderer, &food);

        SDL_RenderPresent(renderer);
        SDL_Delay(delay);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
