#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_mixer.h>

#include "./game.h"

const SDL_Color TEXT_COLOR = { .r = 255, .g = 255, .b = 255 };

void reset_ball(Game *game)
{
    game->ball.posx = 250;
    game->ball.posy = SCREEN_HEIGHT-30;
    game->ball.directionx = 1;
    game->ball.directiony = -1;
    game->ball.ballSize = 7;
    game->ball.speed = 2.0;
}

void check_game_over_condition(Game *game)
{
    if (game->lives > 0) {
        game->lives--;
        reset_ball(game);
    } else {
        game->state = GAME_OVER_STATE;
    }
}

void check_collision_with_bricks(Game *game)
{
    for (int i = 0; i < 20; ++i) {
        if ((game->ball.posx >= game->bricks[i].x &&
             game->ball.posx <= game->bricks[i].x + game->bricks[i].w) &&
            (game->ball.posy >= game->bricks[i].y &&
             game->ball.posy <= game->bricks[i].y + game->bricks[i].h)) {
            // hide brick
            game->bricks[i].y = -120;
            game->bricks[i].w = 0;
            game->bricks[i].h = 0;

            // redirect the ball
            game->ball.directionx *= -1;
            game->ball.directiony *= -1;
            game->ball.speed += 0.10;

            // score +
            game->score++;
        }
    }
}

void do_step(Game *game)
{
    if (game->state == PLAYING_STATE) {
        // Moving the ball
        game->ball.posx += game->ball.directionx * game->ball.speed;
        game->ball.posy += game->ball.directiony * game->ball.speed;

        // Collision with bricks
        check_collision_with_bricks(game);

        // Collision with walls
        if (game->ball.posx >= SCREEN_WIDTH || game->ball.posx <= 0)
            game->ball.directionx *= -1;
        if (game->ball.posy <= 0)
            game->ball.directiony *= -1;

        // Collision with player
        if (game->ball.posy >= game->player.posy &&
            (game->ball.posx >= game->player.posx &&
             game->ball.posx <= game->player.posx + game->player.width)) {
            game->ball.directiony = -1;
            game->ball.speed += 0.10;
        }

        if (game->ball.posy > SCREEN_HEIGHT) {
            check_game_over_condition(game);
        }
    }
}

void render_game(SDL_Renderer *renderer, Game *game)
{
    // Render Player
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_Rect playerRect = { game->player.posx,
                            game->player.posy,
                            game->player.width,
                            game->player.height };
    SDL_RenderFillRect(renderer,
                       &playerRect);

    // Render Ball
    filledCircleRGBA(renderer,
                     game->ball.posx, game->ball.posy, game->ball.ballSize,
                     255, 255, 255, 255);

    // Render bricks
    for (int i = 0; i < 20; ++i) {
        SDL_Rect brick_inside = { game->bricks[i].x + 2,
                                  game->bricks[i].y + 2,
                                  game->bricks[i].w - 4,
                                  game->bricks[i].h - 4 };

        SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
        SDL_RenderFillRect(renderer,
                           &game->bricks[i]);
        SDL_SetRenderDrawColor(renderer, 255, 165, 20, 255);
        SDL_RenderFillRect(renderer,
                           &brick_inside);
    }
}

void render_ui(SDL_Renderer *renderer, Game *game)
{
    if (game->state != GAME_OVER_STATE) {
        // Draw lives
        char lives_str[12];
        sprintf(lives_str, "Hearts: %d", game->lives);

        SDL_Surface *life_surface = TTF_RenderText_Solid(game->font, lives_str, TEXT_COLOR);
        SDL_Texture *life_texture = SDL_CreateTextureFromSurface(renderer, life_surface);
        SDL_FreeSurface(life_surface);

        SDL_Rect uiRect = { 0, 0, 110, 24 };
        SDL_RenderCopy(renderer, life_texture, NULL, &uiRect);

        // Draw Score
        char score_str[12];
        sprintf(score_str, "Score: %d", game->score);

        SDL_Surface *score_surface = TTF_RenderText_Solid(game->font, score_str, TEXT_COLOR);
        SDL_Texture *score_texture = SDL_CreateTextureFromSurface(renderer, score_surface);
        SDL_FreeSurface(score_surface);

        uiRect.x = SCREEN_WIDTH - 110;
        SDL_RenderCopy(renderer, score_texture, NULL, &uiRect);
    } else {
        // Draw Game Over
        SDL_Surface *game_over_surface = TTF_RenderText_Solid(game->font, "GAME OVER", TEXT_COLOR);
        SDL_Texture *game_over_texture = SDL_CreateTextureFromSurface(renderer, game_over_surface);
        SDL_FreeSurface(game_over_surface);

        SDL_Rect gameOverRect = { SCREEN_WIDTH / 2 - 45, SCREEN_HEIGHT / 2 - 20, 90, 40 };
        SDL_RenderCopy(renderer, game_over_texture, NULL, &gameOverRect);
    }
}

void event_handling(Game *game)
{
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type)
        case SDL_QUIT:
            game->state = QUIT_STATE;
        break;
    }

    const Uint8 *state = SDL_GetKeyboardState(NULL);

    if (state[SDL_SCANCODE_ESCAPE])
        game->state = QUIT_STATE;

    if (state[SDL_SCANCODE_A] || state[SDL_SCANCODE_LEFT])
        if (game->player.posx >= 0)
            game->player.posx -= 10;
    if (state[SDL_SCANCODE_D] || state[SDL_SCANCODE_RIGHT])
        if (game->player.posx + game->player.width <= SCREEN_WIDTH)
            game->player.posx += 10;
}

int main()
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        printf("ERROR: Couldn't initialize SDL.\n Message: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("Arkanoid by elnawe",
                                          100,
                                          100,
                                          SCREEN_WIDTH,
                                          SCREEN_HEIGHT,
                                          0);

    if (window == NULL) {
        printf("ERROR: Couldn't create window.\n Message: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1,
                                                SDL_RENDERER_ACCELERATED |
                                                SDL_RENDERER_PRESENTVSYNC);

    if (TTF_Init() != 0) {
        printf("ERROR: Couldn't init TTF.\n Message: %s\n", TTF_GetError());
        SDL_Quit();
        return 1;
    }

    TTF_Font *font = TTF_OpenFont("Roboto-Regular.ttf", 14);

    if (!font) {
        printf("ERROR: Couldn't load font.\n Message: %s\n", TTF_GetError());
        SDL_Quit();
        return 1;
    }

    if (Mix_Init(MIX_INIT_OGG) != 0) {
        printf("ERROR: Couldn't load mix library.\n Message: %s\n", Mix_GetError());
        Mix_Quit();
        SDL_Quit();
        return 1;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) != 0) {
        printf("ERROR: Couldn't open audio.\n Message: %s\n", Mix_GetError());
        return 1;
    }

    Mix_Music *background_music = Mix_LoadMUS("tetris.ogg");

    if (!background_music) {
        printf("ERROR: Couldn't load file.\n Message: %s\n", Mix_GetError());
        return 1;
    }

    Mix_PlayMusic(background_music, -1);

    Player player = {
        .posx = 250,
        .posy = SCREEN_HEIGHT - 20,
        .width = 75,
        .height = 10
    };
    Ball ball = {
        .posx = 250,
        .posy = SCREEN_HEIGHT-30,
        .directionx = 1,
        .directiony = -1,
        .ballSize = 7,
        .speed = 2.0
    };

    Game game = {
        .player = player,
        .ball = ball,
        .lives = 2,
        .state = PLAYING_STATE,
        .font = font,
        .score = 0
    };

    for (int i = 0; i < 20; ++i) {
        SDL_Rect brickRect = { (64 * i) % 640,
                               30,
                               64,
                               20 };

        if (i > 9) {
            brickRect.y = 50;
        }

        game.bricks[i] = brickRect;
    }

    while (game.state != QUIT_STATE) {
        event_handling(&game);

        do_step(&game);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        render_game(renderer, &game);
        render_ui(renderer, &game);
        SDL_RenderPresent(renderer);
    }

    // TODO move this cleanup to each function that takes care of rendering things.
    TTF_CloseFont(font);
    TTF_Quit();
    Mix_FreeMusic(background_music);
    Mix_CloseAudio();
    Mix_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    font = NULL;
    window = NULL;
    renderer = NULL;

    return 0;
}
