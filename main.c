#include <stdio.h>
#include <SDL2/SDL.h>
#include <stdbool.h>
#include <time.h>

#define WIDTH 900
#define HEIGHT 600
#define CELL 20
#define CELLCOUNT ((WIDTH * HEIGHT) / (CELL * CELL))
#define ROWS (HEIGHT / CELL)
#define COLS (WIDTH / CELL)
#define LINE_THICKNESS 1

#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3


#define BACKGROUND_COLOR 0x000000
#define SNAKE_COLOR 0xfffff
#define APPLE_COLOR 0xff0000

/* This is an array used to store all the cells in the grid such that each array element is equal to some
 * cell and the snake cells are stored at the end of the array. This is useful as it allows efficient apple
 * generation by choosing a random array element that is not a snake cell (i.e. choosing an index < CELLCOUNT - snake_size).
 */
int cells[CELLCOUNT];

/* This is an array used to store the indices of all cells in the "cells" array. This is important because it
 * allows fast editing of the "cells" array when the snake moves.
 */
int cell_index[CELLCOUNT];

int snake_size = 1;

struct snake_node{
    struct snake_node* next;
    struct snake_node* prev;
    int cell;
};


int apple = 0;

// A simple method to swap two integers
void swap(int * x, int * y) {
    int temp = *x;
    *x = *y;
    *y = temp;
}

// A simple method to perform modular division
int mod(int cell, int d) {
    return (cell + d) % d;
}

// A simple method to check whether a cell is a snake cell or not
bool cell_on_snake(int cell) {
    return CELLCOUNT - cell_index[cell] <= snake_size;
}

// A simple method to generate a grid shape
void grid(SDL_Window * window, SDL_Surface* surface) {
    SDL_Rect line = {0, 0, WIDTH, LINE_THICKNESS};
    for (int i = 0; i < HEIGHT / CELL; i++) {
        line.y = i * CELL;
        SDL_FillRect(surface, &line, 0x1f1f1f1f);
    }
    line.w = LINE_THICKNESS;
    line.h = HEIGHT;
    line.y = 0;
    for (int i = 0; i < WIDTH / CELL; i++) {
        line.x = i * CELL;
        SDL_FillRect(surface, &line, 0x1f1f1f1f);
    }
    SDL_UpdateWindowSurface(window);
}

/* This method initializes the snake. In this implementation, a snake is just a linked list of cells.*/
struct snake_node* initialize_snake(SDL_Surface * surface) {
    struct snake_node* head = malloc(sizeof(struct snake_node));
    head->next = NULL;
    head->prev = NULL;

    int cord = rand() % CELLCOUNT;
    head->cell = cord;

    //swapping the snake head with the last element in the "cells" array, and updating the "cell_index" array accordingly
    swap(&cells[cell_index[cord]], &cells[CELLCOUNT - snake_size]);
    swap(&cell_index[cord], &cell_index[cells[cell_index[cord]]]);

    SDL_Rect cell = {(cord % COLS) * CELL, (cord / COLS) * CELL, CELL, CELL};
    SDL_FillRect(surface, &cell, 0xfffff);
    return head;
}
void generate_apple(int* apple, SDL_Surface* surface) {
    //choosing a random cell that is not a snake cell
    *apple = cells[rand() % (CELLCOUNT - snake_size)];
    //displaying the apple
    SDL_Rect rect = {(*apple % COLS) * CELL, (*apple / COLS) * CELL, CELL, CELL};
    SDL_FillRect(surface, &rect, APPLE_COLOR);
}
void move_snake(struct snake_node** head, struct snake_node** tail, int direction, SDL_Window* window, SDL_Surface* surface) {

    // Allocating a new node to move the snake ahead
    struct snake_node * new_node = malloc(sizeof(struct snake_node));
    new_node->prev = (*head);
    new_node->next = NULL;
    (*head)->next = new_node;
    (*head) = (*head)->next;


    if (direction == UP) {
        (*head)->cell = mod((*head)->prev->cell - COLS, CELLCOUNT);
    }
    else if (direction == DOWN) {
        (*head)->cell = mod((*head)->prev->cell + COLS, CELLCOUNT);
    }
    else if (direction == LEFT) {
        (*head)->cell = (*head)->prev->cell - (*head)->prev->cell % COLS + mod((*head)->prev->cell % COLS - 1, COLS);
    }
    else {
        (*head)->cell = (*head)->prev->cell - (*head)->prev->cell % COLS + mod((*head)->prev->cell % COLS + 1, COLS);
    }

    SDL_Rect cell = {((*head)->cell % COLS) * CELL, ((*head)->cell / COLS) * CELL, CELL, CELL};
    SDL_FillRect(surface, &cell, SNAKE_COLOR);

    /* If an apple is eaten, we don't delete the tail of the snake as the snake gets longer */
    if ((*head)->cell == apple) {
        // Incrementing snake size and adding the new head cell to the end of the "cells" array
        snake_size++;
        swap(&cells[cell_index[(*head)->cell]], &cells[CELLCOUNT - snake_size]);
        swap(&cell_index[(*head)->cell], &cell_index[cells[cell_index[(*head)->cell]]]);

        generate_apple(&apple, surface);
        return;
    }

    // swapping the tail cell with the new head cell in the "cells" array
    // This makes the new head cell a snake cell, while the tail cell will become a non-snake cell
    swap(&cells[cell_index[(*head)->cell]], &cells[cell_index[(*tail)->cell]]);
    swap(&cell_index[(*head)->cell], &cell_index[(*tail)->cell]);

    // Deleting the tail cell to prevent snake growth when no apple is eaten
    new_node = (*tail);
    cell.x = ((*tail)->cell % COLS) * CELL;
    cell.y = ((*tail)->cell / COLS) * CELL;
    SDL_FillRect(surface, &cell, BACKGROUND_COLOR);
    (*tail) = (*tail)->next;
    free(new_node);
    (*tail)->prev = NULL;
}


int main(void) {
    srand(time(NULL));
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window* window = SDL_CreateWindow("Snake",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,WIDTH,HEIGHT, 0);
    SDL_Surface* surface = SDL_GetWindowSurface(window);

    // Initializing the "cells" array
    for (int i = 0; i < CELLCOUNT; i++) {
        cells[i] = i;
        cell_index[i] = i;
    }

    struct snake_node* head = initialize_snake(surface);
    struct snake_node* tail = head;
    generate_apple(&apple, surface);

    // The start of the Event loop
    SDL_Event ev;
    bool running = true;
    int direction = RIGHT;
    while (running) {
        int curdir = direction;
        while (SDL_PollEvent(&ev) != 0) {
            if (ev.type == SDL_QUIT) {
                running = false;
            }
            else if (ev.type == SDL_KEYDOWN) {
                switch (ev.key.keysym.sym) {
                    case SDLK_UP:
                        if (curdir != DOWN)
                            direction = UP;
                    break;
                    case SDLK_DOWN:
                        if (curdir != UP)
                            direction = DOWN;
                    break;
                    case SDLK_LEFT:
                        if (curdir != RIGHT)
                            direction = LEFT;
                    break;
                    case SDLK_RIGHT:
                        if (curdir != LEFT)
                            direction = RIGHT;
                    break;
                }

            }
        }
        move_snake(&head, &tail, direction, window, surface);
        grid(window, surface);
        SDL_Delay(100);
    }
    return 0;
}