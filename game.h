#ifndef GAME_H_
#define GAME_H_

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

// Game States
#define PLAYING_STATE 0
#define GAME_OVER_STATE 1
#define QUIT_STATE 2

typedef struct {
    int posx, posy, width, height;
} Player;

typedef struct {
    // directionx = 1 (right), -1 (left);
    // directiony = 1 (down), -1 (up);
    int posx, posy, directionx, directiony, ballSize;
    float speed;
} Ball;

typedef struct {
    int posx, posy, width, height;
} Brick;

typedef struct {
    Player player;
    SDL_Rect bricks[20];
    Ball ball;
    int lives, state, score;
    SDL_Texture *textures[10];
    TTF_Font *font;
} Game;

#endif
