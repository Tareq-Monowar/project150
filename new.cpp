#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <SDL_image.h>
#include <bits/stdc++.h>
#include <fstream>
#undef main

const int SCREEN_WIDTH = 1000;
const int SCREEN_HEIGHT = 600;
const int TILE_SIZE = 20;
int score=0;
int timer=0;
int bonusFoodTimer=0;
bool pause;
bool bonusFoodActive=false;
bool gameOver=false;
bool quit=false;

SDL_Window *window = nullptr;
SDL_Renderer *renderer = nullptr;
SDL_Texture* BGTexture = nullptr;
//SDL_Texture* BGTexture = nullptr;
Mix_Music* bgm;
Mix_Chunk* eatSound;
Mix_Chunk* tick = nullptr;


class Snake {
public:
    Snake();
    void handleInput(SDL_Event &e);
    void move();
    void render(SDL_Renderer *renderer);
    bool checkCollision();
    void spawnFood();

private:
    SDL_Rect food;
    std::vector<SDL_Rect> body;
    int direction; // 0: up, 1: down, 2: left, 3: right
    SDL_Rect bonusFood;
};


Snake::Snake() {
    SDL_Rect head = {0, 60, TILE_SIZE, TILE_SIZE};
    body.push_back(head);
    SDL_Rect body1 = {head.x,head.y,TILE_SIZE,TILE_SIZE};
    body.push_back(body1);
    SDL_Rect body2 = {head.x,head.y,TILE_SIZE,TILE_SIZE};
    body.push_back(body2);
    direction = 3; // Start moving to the right
    spawnFood();
    bonusFoodActive=false;
}

void Snake::handleInput(SDL_Event &e) {
    if (e.type == SDL_KEYDOWN) {
        switch (e.key.keysym.sym) {
            case SDLK_UP:
                if (direction != 1){
                    direction = 0;
                    if (pause) pause= !pause;
                }
                break;
            case SDLK_DOWN:
                if (direction != 0){ 
                    direction = 1;
                    if (pause) pause= !pause;
                }
                break;
            case SDLK_LEFT:
                if (direction != 3){ 
                    direction = 2;
                    if (pause) pause= !pause;
                }
                break;
            case SDLK_RIGHT:
                if (direction != 2){
                    direction = 3;
                    if (pause) pause= !pause;
                }

                break;
            case SDLK_SPACE:
            pause=!pause;
            break;

        }
    }
}

void Snake::move() {
    SDL_Rect newHead = body.front();

    switch (direction) {
        case 0: // Up
            newHead.y -= TILE_SIZE;
            break;
        case 1: // Down
            newHead.y += TILE_SIZE;
            break;
        case 2: // Left
            newHead.x -= TILE_SIZE;
            break;
        case 3: // Right
            newHead.x += TILE_SIZE;
            break;
    }
 
    // if (newHead.x < 0) {
    //     newHead.x = SCREEN_WIDTH - TILE_SIZE;
    // } else if (newHead.x >= SCREEN_WIDTH) {
    //     newHead.x = 0;
    // }

    body.insert(body.begin(), newHead);

    if (newHead.x == food.x && newHead.y == food.y) {
        score+=1;
        Mix_PlayChannel(-1,eatSound,0);
        spawnFood();
    }
    else if (bonusFoodActive && newHead.x < bonusFood.x + bonusFood.w && newHead.x + newHead.w > bonusFood.x &&
        newHead.y < bonusFood.y + bonusFood.h && newHead.y + newHead.h > bonusFood.y) {
        score+=10;
        Mix_PlayChannel(-1,eatSound,0);
        Mix_HaltChannel(1);
        //body.push_back(SDL_Rect{0, 0, TILE_SIZE, TILE_SIZE});
        bonusFoodActive = false;
    } else {
        body.pop_back();
    }


    if (bonusFoodActive && SDL_GetTicks() > bonusFoodTimer) {
        Mix_HaltChannel(1);
        bonusFoodActive = false;
    }

    if (checkCollision()) {
        std::cout << "Your Score is " << score << std::endl;
        std::cout << "Game Over!" << std::endl;
        gameOver =true;
    }
}


void Snake::render(SDL_Renderer *renderer) {
    
    SDL_SetRenderDrawColor(renderer, 255, 251, 206, 177); // snake body color
    for (int i = 1; i < body.size(); ++i) {
        SDL_RenderFillRect(renderer, &body[i]);   
    }

    SDL_SetRenderDrawColor(renderer, 255,0,255, 127); // head color
    SDL_RenderFillRect(renderer, &body[0]);

    SDL_SetRenderDrawColor(renderer, 0,0 ,0, 0); // food color
    SDL_RenderFillRect(renderer, &food);


    if (bonusFoodActive) {
        SDL_SetRenderDrawColor(renderer, 0, 127, 0, 255);  // bonusfood color
        SDL_RenderFillRect(renderer, &bonusFood);
    }

    // draw walls
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 0);
    SDL_Rect wall1 = {(SCREEN_WIDTH)/2-(TILE_SIZE*4),SCREEN_HEIGHT/2-(TILE_SIZE*8) , TILE_SIZE*2, TILE_SIZE*16};
    SDL_Rect wall2 = {(SCREEN_WIDTH*8)/25-(TILE_SIZE*4),SCREEN_HEIGHT/2-(TILE_SIZE*8) , TILE_SIZE*20, TILE_SIZE*2};

    SDL_RenderFillRect(renderer, &wall1);
    SDL_RenderFillRect(renderer, &wall2);

    // SDL_SetRenderDrawColor(renderer,255,0,0,255);
    // SDL_Rect wall6 = {0,57,SCREEN_WIDTH,TILE_SIZE/6};
    // SDL_RenderFillRect(renderer, &wall6);
    
}


bool Snake::checkCollision() {
    SDL_Rect head = body.front();

    for (auto it = body.begin() + 1; it != body.end(); ++it) {
        if (head.x == it->x && head.y == it->y) {
            return true;
        }
    }

    if (head.y < 0 || head.y >= SCREEN_HEIGHT) {
        return true;
    }

    if (head.x < 0 || head.x >= SCREEN_WIDTH) {
        return true;
    }

    SDL_Rect wall1 = {SCREEN_WIDTH/2-(TILE_SIZE*4),SCREEN_HEIGHT/2-(TILE_SIZE*8) , TILE_SIZE*2, TILE_SIZE*16};
    SDL_Rect wall2 = {(SCREEN_WIDTH*8)/25-(TILE_SIZE*4),SCREEN_HEIGHT/2-(TILE_SIZE*8) , TILE_SIZE*20, TILE_SIZE*2};
    //SDL_Rect wall6 = {0,58,SCREEN_WIDTH,TILE_SIZE/10};

    if ((head.x < wall1.x + wall1.w && head.x + head.w > wall1.x && head.y < wall1.y + wall1.h && head.y + head.h > wall1.y) ||
        (head.x < wall2.x + wall2.w && head.x + head.w > wall2.x && head.y < wall2.y + wall2.h && head.y + head.h > wall2.y)) {
        return true;
    }

    return false;
}

void Snake::spawnFood() {
    SDL_Rect wall1 = {SCREEN_WIDTH/2-(TILE_SIZE*4),SCREEN_HEIGHT/2-(TILE_SIZE*8) , TILE_SIZE*2, TILE_SIZE*16};
    SDL_Rect wall2 = {(SCREEN_WIDTH*8)/25-(TILE_SIZE*4),SCREEN_HEIGHT/2-(TILE_SIZE*8) , TILE_SIZE*20, TILE_SIZE*2};
   // SDL_Rect wall6 = {0,58,SCREEN_WIDTH,TILE_SIZE/10};
    do {
        food.x = rand() % (SCREEN_WIDTH/ TILE_SIZE) * TILE_SIZE;
        food.y = rand() % ((SCREEN_HEIGHT-TILE_SIZE*3)/TILE_SIZE) * TILE_SIZE;
    } while ((food.x >= wall1.x && food.x < wall1.x + wall1.w && food.y >= wall1.y && food.y < wall1.y + wall1.h) ||
             (food.x >= wall2.x && food.x < wall2.x + wall2.w && food.y >= wall2.y && food.y < wall2.y + wall2.h) ||
             (food.x >= 0 && food.x < SCREEN_WIDTH && food.y >= 0 && food.y < 60) ||
             std::any_of(body.begin(), body.end(), [this](const SDL_Rect &segment) {
                return food.x == segment.x && food.y == segment.y;
            }));

    food.w = TILE_SIZE;
    food.h = TILE_SIZE;

    if (score % 7 == 0 && score!=0) {
        bonusFoodActive = true;
        bonusFoodTimer = SDL_GetTicks() + 7000;
        Mix_PlayChannel(1, tick, 0);

    do {
    bonusFood.x = rand() % (SCREEN_WIDTH / TILE_SIZE) * TILE_SIZE;
    bonusFood.y = rand() % ((SCREEN_HEIGHT-TILE_SIZE*4) / TILE_SIZE) * TILE_SIZE;
    } while ((bonusFood.x >= wall1.x && bonusFood.x < wall1.x + wall1.w && bonusFood.y >= wall1.y && bonusFood.y < wall1.y + wall1.h) ||
    (bonusFood.x >= wall2.x && bonusFood.x < wall2.x + wall2.w && bonusFood.y >= wall2.y && bonusFood.y < wall2.y + wall2.h) ||
     (bonusFood.x >= 0 && bonusFood.x < SCREEN_WIDTH && bonusFood.y >= 0 && bonusFood.y < 60) ||
     std::any_of(body.begin(), body.end(), [this](const SDL_Rect &segment) {
         return food.x == segment.x && food.y == segment.y;
         }));

    bonusFood.w=TILE_SIZE*2;
    bonusFood.h=TILE_SIZE*2;
    }

}

void renderScore(SDL_Renderer* renderer, TTF_Font* font, int score) {
    SDL_Color fontColor = {255, 255, 102, 255};
    SDL_Surface* surface1 = TTF_RenderText_Solid(font, ("Score: " + std::to_string(score)).c_str(), fontColor);
    if (surface1) {
        SDL_Texture* text1 = SDL_CreateTextureFromSurface(renderer, surface1);

        if (text1) {
            SDL_Rect textRect={5,5,200,50};
            SDL_RenderCopy(renderer, text1, nullptr, &textRect);
            SDL_DestroyTexture(text1);
        }
        
        SDL_FreeSurface(surface1);
    }

    int timer = (int)(bonusFoodTimer-SDL_GetTicks())/1000;
    std::string t = "Timer: " + std::to_string(timer);
    SDL_Surface* surface2 = TTF_RenderText_Solid(font, t.c_str() , fontColor);
    if (surface2) {
        SDL_Texture* text2 = SDL_CreateTextureFromSurface(renderer, surface2);

        if (text2) {
            SDL_Rect textRect2={675,5,200,50};
            if (bonusFoodActive)
                SDL_RenderCopy(renderer, text2, nullptr, &textRect2);
            SDL_DestroyTexture(text2);
        }
        
        SDL_FreeSurface(surface2);
    }
}

void displayGameOver(SDL_Renderer* renderer, TTF_Font* font, int score) {

    SDL_RenderClear(renderer);

    SDL_Surface* BGSurface = IMG_Load("jungle.jpeg");
    BGTexture = SDL_CreateTextureFromSurface(renderer, BGSurface);
    SDL_FreeSurface(BGSurface);
    
    SDL_Color Color = {255, 55, 50, 255};
    SDL_Color Color2 = {55, 50, 255, 255};
    std::string msg = "GAME OVER!";
    std::string scoreMsg = "Final Score: " + std::to_string(score);

    SDL_Surface* surface = TTF_RenderText_Solid(font, msg.c_str(), Color);
    SDL_Surface* scoreSurface = TTF_RenderText_Solid(font, scoreMsg.c_str(), Color2);
    SDL_RenderCopy(renderer, BGTexture, nullptr, nullptr);

    if (surface && scoreSurface) {
        SDL_Texture* text = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_Texture* scoreText = SDL_CreateTextureFromSurface(renderer, scoreSurface);

        if (text && scoreText) {
            SDL_Rect textRect = {SCREEN_WIDTH/2-300, 200, 600,160 };
            SDL_Rect scoreRect = {SCREEN_WIDTH/2-300, 400,600,160};

            SDL_RenderCopy(renderer, text, nullptr, &textRect);
            SDL_RenderCopy(renderer, scoreText, nullptr, &scoreRect);

            SDL_DestroyTexture(text);
            SDL_DestroyTexture(scoreText);
        }

        SDL_FreeSurface(surface);
        SDL_FreeSurface(scoreSurface);
    }
    SDL_RenderPresent(renderer);
    SDL_Delay(5000);

}


int main(int argc, char *argv[]) {
    
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();
    Mix_Init(MIX_INIT_MP3);
    IMG_Init(IMG_INIT_JPG);
    
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
    bgm = Mix_LoadMUS("itachi_main_theme.mp3");
    Mix_VolumeMusic(20);
    Mix_PlayMusic(bgm, -1);
    eatSound = Mix_LoadWAV("food.mp3");
    tick = Mix_LoadWAV("the-mechanical-wall-clock-ticks-one-minute-253101.mp3");

    window = SDL_CreateWindow("Snake Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    
    TTF_Font* font = TTF_OpenFont("Arial.ttf", 50);

    SDL_Surface* BGSurface = IMG_Load("jungle2.jpeg");
    BGTexture = SDL_CreateTextureFromSurface(renderer, BGSurface);
    SDL_FreeSurface(BGSurface);
    
    Snake snake;

    SDL_Event e;
    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
            snake.handleInput(e);
        }
        if (!pause) 
        snake.move();

        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
        SDL_RenderClear(renderer);

        SDL_RenderCopy(renderer, BGTexture, nullptr, nullptr);
        
        snake.render(renderer);
        renderScore(renderer, font, score);
        if (gameOver) {
            displayGameOver(renderer, font, score);
            break;
        }

        SDL_RenderPresent(renderer);
        if(bonusFoodActive)
            SDL_Delay(80);
        else
            SDL_Delay(120);
    } 

   

    IMG_Quit();
    Mix_FreeMusic(bgm);
    Mix_FreeChunk(eatSound);
    Mix_CloseAudio();
    SDL_DestroyTexture(BGTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    TTF_Quit();
    Mix_Quit();
    return 0;
}

