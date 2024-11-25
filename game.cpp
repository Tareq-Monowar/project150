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
int score = 0;
int bonusFoodTimer = 0;
int bonusFoodeTimer = 0;
bool pause = false;
bool bonusFoodActive = false;
bool bonusFoodeActive = false;
bool gameOver = false;
bool quit = false;
int value=0;

SDL_Window *window = nullptr;
SDL_Renderer *renderer = nullptr;
SDL_Texture *BGTexture = nullptr;
Mix_Music *bgm;
Mix_Chunk *eatSound;
Mix_Chunk *tick = nullptr;

enum GameState {
    MENU,
    PLAYING,
    EXIT
};

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
    SDL_Rect body1 = {head.x, head.y, TILE_SIZE, TILE_SIZE};
    body.push_back(body1);
    SDL_Rect body2 = {head.x, head.y, TILE_SIZE, TILE_SIZE};
    body.push_back(body2);
    direction = 3;
    spawnFood();
    bonusFoodActive = false;
    bonusFoodeActive = false;
   
}

void Snake::handleInput(SDL_Event &e) {
    if (e.type == SDL_KEYDOWN) {
        switch (e.key.keysym.sym) {
            case SDLK_UP:
                if (direction != 1) {
                    direction = 0;
                    if(pause)  pause = !pause;
                }
                break;
            case SDLK_DOWN:
                if (direction != 0) {
                    direction = 1;
                    if(pause)  pause = !pause;
                }
                break;
            case SDLK_LEFT:
                if (direction != 3) {
                    direction = 2;
                    if(pause)  pause = !pause;
                }
                break;
            case SDLK_RIGHT:
                if (direction != 2) {
                    direction = 3;
                    if(pause)  pause = !pause;
                }
                break;
            case SDLK_SPACE:
                pause = !pause;
                break;
        }
    }
}

void Snake::move() {
    SDL_Rect newHead = body.front();

    switch (direction)
    {
        case 0: 
            newHead.y -= TILE_SIZE; 
            break; 
        case 1: 
            newHead.y += TILE_SIZE; 
            break; 
        case 2: 
            newHead.x -= TILE_SIZE; 
            break; 
        case 3: 
            newHead.x += TILE_SIZE; 
            break; 
    }

    
    if (newHead.x < 0) {
        newHead.x = SCREEN_WIDTH - TILE_SIZE;
    }
    else if(newHead.x >= SCREEN_WIDTH){
         newHead.x = 0;
    }

    if (newHead.y < 0) {
        newHead.y = SCREEN_HEIGHT - TILE_SIZE;
    }
    else if(newHead.y >= SCREEN_HEIGHT){
         newHead.y = 0;
    }

    body.insert(body.begin(), newHead);

    // Food collision
    if(newHead.x == food.x && newHead.y == food.y) 
    {
        score++;
        Mix_PlayChannel(-1, eatSound, 0);
        spawnFood();
    } 
    else if(bonusFoodActive && newHead.x < bonusFood.x + bonusFood.w && newHead.x + newHead.w > bonusFood.x &&
               newHead.y < bonusFood.y + bonusFood.h && newHead.y + newHead.h > bonusFood.y) 
    {
        score += 10;
        Mix_PlayChannel(-1, eatSound, 0);
        Mix_HaltChannel(1);
        bonusFoodActive = false;
    }
    else if(bonusFoodeActive && newHead.x < bonusFood.x + bonusFood.w && newHead.x + newHead.w > bonusFood.x &&
               newHead.y < bonusFood.y + bonusFood.h && newHead.y + newHead.h > bonusFood.y) 
    {
        score -= 10;
        //if(score<0) return displayGameOver(renderer, font, score);
        Mix_PlayChannel(-1, eatSound, 0);
        Mix_HaltChannel(1);
        bonusFoodeActive = false;
    }
    else
    {
        body.pop_back();
    }

    if (bonusFoodActive && SDL_GetTicks() > bonusFoodTimer)
    {
        Mix_HaltChannel(1);
        bonusFoodActive = false;
    }

    if (bonusFoodeActive && SDL_GetTicks() > bonusFoodeTimer)
    {
        Mix_HaltChannel(1);
        bonusFoodeActive = false;
    }


    if (checkCollision())
    {
        std::cout << "Your Score is " << score << std::endl;
        std::cout << "Game Over!" << std::endl;
        gameOver = true;
    }
}

void Snake::render(SDL_Renderer *renderer) {
    SDL_SetRenderDrawColor(renderer, 255, 251, 206, 177); 
    for (int i = 1; i < body.size(); ++i) {
        SDL_RenderFillRect(renderer, &body[i]);
    }

    SDL_SetRenderDrawColor(renderer, 255, 0, 255, 127); 
    SDL_RenderFillRect(renderer, &body[0]);

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); 
    SDL_RenderFillRect(renderer, &food);

    if (bonusFoodActive) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); 
        SDL_RenderFillRect(renderer, &bonusFood);
    }

    if (bonusFoodeActive) {
        SDL_SetRenderDrawColor(renderer, 0, 125, 0, 255); 
        SDL_RenderFillRect(renderer, &bonusFood);
    }

    SDL_SetRenderDrawColor(renderer, 0, 0 , 0, 255);
    SDL_Rect wall1 = {(SCREEN_WIDTH)/4-(TILE_SIZE*2),SCREEN_HEIGHT/2-(TILE_SIZE*8) , TILE_SIZE, TILE_SIZE*16};
    SDL_Rect wall2 = {(SCREEN_HEIGHT)*31/32-(TILE_SIZE*2),SCREEN_HEIGHT/2-(TILE_SIZE*8) , TILE_SIZE*9, TILE_SIZE};
    SDL_Rect wall3 = {(SCREEN_WIDTH)*3/4-(TILE_SIZE*2),SCREEN_HEIGHT/2-(TILE_SIZE*8) , TILE_SIZE, TILE_SIZE*16};
    SDL_Rect wall4 = {SCREEN_WIDTH/4-(TILE_SIZE*2),(SCREEN_HEIGHT)/2+(TILE_SIZE*8),TILE_SIZE*8,TILE_SIZE };


    SDL_RenderFillRect(renderer, &wall1);
     SDL_RenderFillRect(renderer, &wall2);
     SDL_RenderFillRect(renderer, &wall3);
    SDL_RenderFillRect(renderer, &wall4);


}

bool Snake::checkCollision() {
    SDL_Rect head = body.front();

    for (auto it = body.begin() + 1; it != body.end(); ++it) {
        if (head.x == it->x && head.y == it->y) {
            return true;
        }
    }

    SDL_Rect wall1 = {(SCREEN_WIDTH)/4-(TILE_SIZE*2),SCREEN_HEIGHT/2-(TILE_SIZE*8) , TILE_SIZE, TILE_SIZE*16};
    SDL_Rect wall2 = {(SCREEN_HEIGHT)*31/32-(TILE_SIZE*2),SCREEN_HEIGHT/2-(TILE_SIZE*8) , TILE_SIZE*9, TILE_SIZE};
    SDL_Rect wall3 = {(SCREEN_WIDTH)*3/4-(TILE_SIZE*2),SCREEN_HEIGHT/2-(TILE_SIZE*8) , TILE_SIZE, TILE_SIZE*16};
    SDL_Rect wall4 = {SCREEN_WIDTH/4-(TILE_SIZE*2),(SCREEN_HEIGHT)/2+(TILE_SIZE*8),TILE_SIZE*8,TILE_SIZE };

    if((head.x < wall1.x + wall1.w && head.x + head.w > wall1.x && head.y < wall1.y + wall1.h && head.y + head.h > wall1.y) ||
        (head.x < wall2.x + wall2.w && head.x + head.w > wall2.x && head.y < wall2.y + wall2.h && head.y + head.h > wall2.y) ||
        (head.x < wall3.x + wall3.w && head.x + head.w > wall3.x && head.y < wall3.y + wall3.h && head.y + head.h > wall3.y) ||
        (head.x < wall4.x + wall4.w && head.x + head.w > wall4.x && head.y < wall4.y + wall4.h && head.y + head.h > wall4.y))
        {
            return true;  
            //yesorno(renderer, font);
        }


    return false;
}

void Snake::spawnFood() {

    SDL_Rect wall1 = {(SCREEN_WIDTH)/4-(TILE_SIZE*2),SCREEN_HEIGHT/2-(TILE_SIZE*8) , TILE_SIZE, TILE_SIZE*16};
    SDL_Rect wall2 = {(SCREEN_HEIGHT)*31/32-(TILE_SIZE*2),SCREEN_HEIGHT/2-(TILE_SIZE*8) , TILE_SIZE*9, TILE_SIZE};
    SDL_Rect wall3 = {(SCREEN_WIDTH)*3/4-(TILE_SIZE*2),SCREEN_HEIGHT/2-(TILE_SIZE*8) , TILE_SIZE, TILE_SIZE*16};
    SDL_Rect wall4 = {SCREEN_WIDTH/4-(TILE_SIZE*2),(SCREEN_HEIGHT)/2+(TILE_SIZE*8),TILE_SIZE*8,TILE_SIZE };

    do {
        food.x = rand() % (SCREEN_WIDTH / TILE_SIZE) * TILE_SIZE;
        food.y = rand() % ((SCREEN_HEIGHT - TILE_SIZE * 3) / TILE_SIZE) * TILE_SIZE;
    } while ((food.x < wall1.x + wall1.w && food.x + food.w > wall1.x && food.y < wall1.y + wall1.h && food.y + food.h > wall1.y) ||
        (food.x < wall2.x + wall2.w && food.x + food.w > wall2.x && food.y < wall2.y + wall2.h && food.y + food.h > wall2.y) ||
        (food.x < wall3.x + wall3.w && food.x + food.w > wall3.x && food.y < wall3.y + wall3.h && food.y + food.h > wall3.y) ||
        (food.x < wall4.x + wall4.w && food.x + food.w > wall4.x && food.y < wall4.y + wall4.h && food.y + food.h > wall4.y) ||
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
    } while ((bonusFood.x < wall1.x + wall1.w && bonusFood.x + bonusFood.w > wall1.x && bonusFood.y < wall1.y + wall1.h && bonusFood.y + bonusFood.h > wall1.y) ||
        (bonusFood.x < wall2.x + wall2.w && bonusFood.x + bonusFood.w > wall2.x && food.y < wall2.y + wall2.h && bonusFood.y + bonusFood.h > wall2.y) ||
        (bonusFood.x < wall3.x + wall3.w && bonusFood.x + bonusFood.w > wall3.x && bonusFood.y < wall3.y + wall3.h && bonusFood.y + bonusFood.h > wall3.y) ||
        (bonusFood.x < wall4.x + wall4.w && bonusFood.x + bonusFood.w > wall4.x && bonusFood.y < wall4.y + wall4.h && bonusFood.y + bonusFood.h > wall4.y) ||
        (bonusFood.x >= 0 && bonusFood.x < SCREEN_WIDTH && bonusFood.y >= 0 && bonusFood.y < 60) ||
     std::any_of(body.begin(), body.end(), [this](const SDL_Rect &segment) {
         return food.x == segment.x && food.y == segment.y;
         }));

    bonusFood.w=TILE_SIZE*2;
    bonusFood.h=TILE_SIZE*2;
    }

    if (score % 4 == 0 && score!=0){
        bonusFoodeActive = true;
        bonusFoodeTimer = SDL_GetTicks() + 6000;
        Mix_PlayChannel(1, tick, 0);
    

    do {
    bonusFood.x = rand() % (SCREEN_WIDTH / TILE_SIZE) * TILE_SIZE;
    bonusFood.y = rand() % ((SCREEN_HEIGHT-TILE_SIZE*5) / TILE_SIZE) * TILE_SIZE;
    } while ((bonusFood.x < wall1.x + wall1.w && bonusFood.x + bonusFood.w > wall1.x && bonusFood.y < wall1.y + wall1.h && bonusFood.y + bonusFood.h > wall1.y) ||
        (bonusFood.x < wall2.x + wall2.w && bonusFood.x + bonusFood.w > wall2.x && food.y < wall2.y + wall2.h && bonusFood.y + bonusFood.h > wall2.y) ||
        (bonusFood.x < wall3.x + wall3.w && bonusFood.x + bonusFood.w > wall3.x && bonusFood.y < wall3.y + wall3.h && bonusFood.y + bonusFood.h > wall3.y) ||
        (bonusFood.x < wall4.x + wall4.w && bonusFood.x + bonusFood.w > wall4.x && bonusFood.y < wall4.y + wall4.h && bonusFood.y + bonusFood.h > wall4.y) ||
        (bonusFood.x >= 0 && bonusFood.x < SCREEN_WIDTH && bonusFood.y >= 0 && bonusFood.y < 60) ||
     std::any_of(body.begin(), body.end(), [this](const SDL_Rect &segment) {
         return food.x == segment.x && food.y == segment.y;
         }));

    bonusFood.w=TILE_SIZE*2;
    bonusFood.h=TILE_SIZE*2;
    }

}



void renderScore(SDL_Renderer *renderer, TTF_Font *font, int score) {
    SDL_Color fontColor = {255, 255, 102, 255};
    SDL_Surface *surface1 = TTF_RenderText_Solid(font, ("Score: " + std::to_string(score)).c_str(), fontColor);
    if (surface1) {
        SDL_Texture *text1 = SDL_CreateTextureFromSurface(renderer, surface1);

        if (text1) {
            SDL_Rect textRect = {5, 5, 200, 50};
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
            SDL_Rect textRect2={750,5,200,50};
            if (bonusFoodActive)
                SDL_RenderCopy(renderer, text2, nullptr, &textRect2);
            SDL_DestroyTexture(text2);
        }
        
        SDL_FreeSurface(surface2);
    }

    int timer2 = (int)(bonusFoodeTimer-SDL_GetTicks())/1000;
    std::string t2 = "Timer2: " + std::to_string(timer2);
    SDL_Surface* surface3 = TTF_RenderText_Solid(font, t2.c_str() , fontColor);
    if (surface3) {
        SDL_Texture* text3 = SDL_CreateTextureFromSurface(renderer, surface3);

        if (text3) {
            SDL_Rect textRect3={550,5,200,50};
            if (bonusFoodActive)
                SDL_RenderCopy(renderer, text3, nullptr, &textRect3);
            SDL_DestroyTexture(text3);
        }
        
        SDL_FreeSurface(surface3);
    }

}

void displayMenu(SDL_Renderer *renderer, TTF_Font *font) {
    SDL_Color color = {0, 0, 0, 255};

    SDL_RenderClear(renderer);

    SDL_Surface *BGSurface = IMG_Load("snakeview.jpeg");
    SDL_Texture *BGTexture = SDL_CreateTextureFromSurface(renderer, BGSurface);
    SDL_FreeSurface(BGSurface);

    SDL_RenderCopy(renderer, BGTexture, nullptr, nullptr);
    SDL_DestroyTexture(BGTexture);

    SDL_Surface *surfaceStart = TTF_RenderText_Solid(font, "START", color);
    SDL_Surface *surfaceExit = TTF_RenderText_Solid(font, "EXIT", color);

    if (surfaceStart && surfaceExit) {
        SDL_Texture *textStart = SDL_CreateTextureFromSurface(renderer, surfaceStart);
        SDL_Texture *textExit = SDL_CreateTextureFromSurface(renderer, surfaceExit);

        if (textStart && textExit) {
            SDL_Rect startRect = {SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 - 80, 200, 60};
            SDL_Rect exitRect = {SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 + 20, 200, 60};

            SDL_RenderCopy(renderer, textStart, nullptr, &startRect);
            SDL_RenderCopy(renderer, textExit, nullptr, &exitRect);

            SDL_DestroyTexture(textStart);
            SDL_DestroyTexture(textExit);
        }

        SDL_FreeSurface(surfaceStart);
        SDL_FreeSurface(surfaceExit);
    }

    SDL_RenderPresent(renderer);
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
    bgm = Mix_LoadMUS("naruto_theme_bgm.mp3");

    Mix_VolumeMusic(20);
    Mix_PlayMusic(bgm, -1);
    eatSound = Mix_LoadWAV("mixkit-wild-lion-animal-roar-6.wav");
    tick = Mix_LoadWAV("the-mechanical-wall-clock-ticks-one-minute-253101.mp3");

    window = SDL_CreateWindow("Snake Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    TTF_Font *font = TTF_OpenFont("Arial.ttf", 50);

    SDL_Surface* BGSurface = IMG_Load("jungle2.jpeg");
    BGTexture = SDL_CreateTextureFromSurface(renderer, BGSurface);
    SDL_FreeSurface(BGSurface);

    Snake snake;

    SDL_Event e;
    GameState gameState = MENU;

    while (gameState != EXIT) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                gameState = EXIT;
            }

            if (gameState == MENU) {
                if (e.type == SDL_KEYDOWN) {
                    if (e.key.keysym.sym == SDLK_RETURN) {
                        gameState = PLAYING;
                        Mix_PlayMusic(bgm, -1);
                    } else if (e.key.keysym.sym == SDLK_ESCAPE) {
                        gameState = EXIT;
                    }
                }
            } else if (gameState == PLAYING) {
                snake.handleInput(e);
            }
        }

        if (gameState == MENU) {
            displayMenu(renderer, font);
        } else if (gameState == PLAYING) {
            if (!pause) {
                snake.move();
            }

            SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
            SDL_RenderClear(renderer);

            SDL_Surface *BGSurface = IMG_Load("jj.png");
            BGTexture = SDL_CreateTextureFromSurface(renderer, BGSurface);
            SDL_FreeSurface(BGSurface);

            SDL_RenderCopy(renderer, BGTexture, nullptr, nullptr);
            SDL_DestroyTexture(BGTexture);

            snake.render(renderer);
            renderScore(renderer, font, score);

            if (gameOver) {
                displayGameOver(renderer, font, score);
                break;
                gameState = EXIT;
            }

            SDL_RenderPresent(renderer);
        if(bonusFoodActive)
            SDL_Delay(80);
        else
            SDL_Delay(120);
        }
    }

    Mix_FreeMusic(bgm);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_CloseFont(font);
    Mix_CloseAudio();
    TTF_Quit();
    Mix_Quit();
    IMG_Quit();
    SDL_Quit();

    return 0;
}
