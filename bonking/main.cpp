#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <time.h>
#include <fstream>

using namespace std;

const SDL_Color CYAN_COLOR                      = {  0, 255, 255};
const SDL_Color BLUE_COLOR                      = {  0,   0, 255};
const SDL_Color ORANGE_COLOR                    = {255, 165,   0};
const SDL_Color YELLOW_COLOR                    = {255, 255,   0};
const SDL_Color LIME_COLOR                      = {  0, 255,   0};
const SDL_Color PURPLE_COLOR                    = {128,   0, 128};
const SDL_Color RED_COLOR                       = {255,   0,   0};
const SDL_Color GREEN_COLOR                     = {  0, 128,   0};
const SDL_Color WHITE_COLOR                     = {255, 255, 255};
const SDL_Color BLACK_COLOR                     = {  0,   0,   0};

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const string WINDOW_TITLE = "bonking";
const string BKG_PATH = "background.jpg";
const string H1_PATH = "+25.jpg";
const string H2_PATH = "+50.jfif";
const string H3_PATH = "-75.png";
const string HSC_PATH = "high_score.txt";
const int FPS = 60;
const int DELAY = 1000/FPS;
const string FONT_PATH = "font-times-new-roman.ttf";
const string MUS_PATH = "nhacnen.mp3";

void logSDLError(std::ostream& os, const std::string &msg, bool fatal = false);
void initSDL(SDL_Window* &window, SDL_Renderer* &renderer);
void quitSDL(SDL_Window* window, SDL_Renderer* renderer);
void waitUntilKeyPressed();

void setRenderColor(SDL_Renderer* renderer,SDL_Color cl);
void showImg(SDL_Renderer* renderer,string path,SDL_Rect *dsc);
int random(int minN, int maxN);
bool checkInBlock(SDL_Point mouse,SDL_Rect block);
void delay(Uint32 frameStart);
void showText(SDL_Renderer* renderer,string msg,const int size,SDL_Color color,SDL_Rect dsc);
void play(SDL_Renderer* renderer,int& s,int& hs);
bool home(SDL_Renderer* renderer,int& s,int& hs,bool reset);

struct Block{
    SDL_Rect dsc;
    int tt;
};

int main(int agrc,char* agrv[])
{
    srand(time(NULL));

    SDL_Renderer* renderer;
    SDL_Window* window;
    initSDL(window,renderer);
    TTF_Init();

    Mix_OpenAudio(44100,MIX_DEFAULT_FORMAT,2,2048);

    Mix_Music* mus = Mix_LoadMUS(MUS_PATH.c_str());

    bool restart = false;
    int score = 0,highscore = 0;

    Mix_PlayMusic(mus,-1);

    while(home(renderer,score,highscore,restart)){
        play(renderer,score,highscore);
        restart = true;
    }

    Mix_PauseMusic();
    Mix_FreeMusic(mus);
    quitSDL(window,renderer);
    TTF_Quit();
    Mix_Quit();
    return 0;
}


void logSDLError(std::ostream& os, const std::string &msg, bool fatal)
{
    os << msg << " Error: " << SDL_GetError() << std::endl;
    if (fatal) {
        SDL_Quit();
        exit(1);
    }
}

void initSDL(SDL_Window* &window, SDL_Renderer* &renderer)
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
        logSDLError(std::cout, "SDL_Init", true);

    window = SDL_CreateWindow(WINDOW_TITLE.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    //window = SDL_CreateWindow(WINDOW_TITLE.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_FULLSCREEN_DESKTOP);

    if (window == nullptr)
        logSDLError(std::cout, "CreateWindow", true);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    //SDL_Renderer *renderer = SDL_CreateSoftwareRenderer(SDL_GetWindowSurface(window));
    if (renderer == nullptr)
        logSDLError(std::cout, "CreateRenderer", true);

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
}

void quitSDL (SDL_Window* window, SDL_Renderer* renderer)
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void waitUntilKeyPressed()
{
    SDL_Event e;
    while (true) {
        if ( SDL_WaitEvent(&e) != 0 &&
             (e.type == SDL_KEYDOWN || e.type == SDL_QUIT) )
            return;
        SDL_Delay(100);
    }
}


void setRenderColor(SDL_Renderer* renderer,SDL_Color cl)
{
    SDL_SetRenderDrawColor(renderer,cl.r,cl.g,cl.b,cl.a);
}


void showImg(SDL_Renderer* renderer,string path,SDL_Rect *dsc)
{
    SDL_Texture* res = nullptr;
    SDL_Surface* load = IMG_Load(path.c_str());

    if(load != nullptr){
        SDL_SetColorKey(load, SDL_TRUE,SDL_MapRGB(load->format,0xCB,0xC8,0x9D));
        res = SDL_CreateTextureFromSurface(renderer,load);
    }else{
        logSDLError(cout,"load Image",true);
    }
    SDL_RenderCopy(renderer,res,NULL,dsc);
    SDL_DestroyTexture(res);
    SDL_FreeSurface(load);
    load    = nullptr;
    res     = nullptr;
}


int random(int minN, int maxN)
{
    return minN + rand() % (maxN + 1 - minN);
}

bool checkInBlock(SDL_Point mouse,SDL_Rect block)
{
    if(mouse.x > block.x && mouse.x < block.x + block.w && mouse.y > block.y && mouse.y < block.y + block.h)
        return true;
    return false;
}


void delay(Uint32 frameStart)
{
    Uint32 frameTime;

	frameTime = SDL_GetTicks() - frameStart;
	//std::cout<<frameStart<<' '<<frameTime<<'\n';
    if (frameTime < DELAY)
    {
        SDL_Delay(DELAY - frameTime);
    }
}

void showText(SDL_Renderer* renderer,string msg,const int size,SDL_Color color,SDL_Rect dsc)
{
    TTF_Font *font = TTF_OpenFont(FONT_PATH.c_str(),size);

    SDL_Surface *textSurface = TTF_RenderText_Solid(font,msg.c_str(),color);
    SDL_Texture *text = SDL_CreateTextureFromSurface(renderer,textSurface);

    int w,h;
    SDL_QueryTexture(text,NULL,NULL,&w,&h);
    dsc.x += dsc.w/2 - w/2;
    dsc.y += (dsc.h-h)/2;
    dsc.w = w;
    dsc.h = h;
    SDL_RenderCopy(renderer,text,NULL,&dsc);

    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(text);
    TTF_CloseFont(font);

    text = nullptr;
    textSurface = nullptr;
    font = nullptr;
}

void play(SDL_Renderer* renderer,int& s,int& hs)
{
    SDL_RenderClear(renderer);
    SDL_Point mouse;
    SDL_Event e;
    bool isrunning = true;
    Uint32 time,oldTime,timeStart,timeEnd;
    int score = 0;
    int highscore = 0;
    int c,r;
    int timeCoolDown;
    string text1 = "Score";
    string text5 = "High Score";
    string text4 = "Time:";

    fstream f;
    f.open(HSC_PATH,ios_base::in);
    f>>highscore;
    f.close();

    Block block[3][3];

    SDL_Rect playScreen,text1dsc,text2dsc,text3dsc,text4dsc,text5dsc,text6dsc;

    playScreen.h = 318;
    playScreen.w = 318;
    playScreen.x = 32;
    playScreen.y = 141;

    text1dsc.h = 50;
    text1dsc.w = 200;
    text1dsc.x = 533;
    text1dsc.y = 141;

    text2dsc.h = 50;
    text2dsc.w = 200;
    text2dsc.x = 533;
    text2dsc.y = 190;

    text3dsc.h = 50;
    text3dsc.w = 50;
    text3dsc.x = 533 + 150 - 25;
    text3dsc.y = 43;

    text4dsc.h = 50;
    text4dsc.w = 100;
    text4dsc.x = 533 + 25;
    text4dsc.y = 43;

    text5dsc.h = 50;
    text5dsc.w = 200;
    text5dsc.x = 533;
    text5dsc.y = 298;

    text6dsc.h = 50;
    text6dsc.w = 200;
    text6dsc.x = 533;
    text6dsc.y = 340;

    for(int i = 0;i < 3;i++){
        for(int j = 0;j < 3;j++){
            block[i][j].dsc.h = 100;
            block[i][j].dsc.w = 100;
            block[i][j].dsc.x = playScreen.x + 9 * i + 100 * i;
            block[i][j].dsc.y = playScreen.y + 9 * j + 100 * j;
            block[i][j].tt = 0;
        }
    }

    c = random(0,2);
    r = random(0,2);

    block[c][r].tt = random(1,3);

    timeStart = timeEnd = time = oldTime = SDL_GetTicks();

    timeCoolDown = 60;

    while(isrunning){
        timeStart = time = SDL_GetTicks();

        if(time - oldTime > 1000){
            timeCoolDown --;
            oldTime = time;
        }

        showImg(renderer,BKG_PATH,nullptr);

        setRenderColor(renderer,WHITE_COLOR);
        //SDL_RenderDrawRects(renderer,block,9);

        for(int i = 0;i < 3;i++){
            for(int j = 0;j < 3;j++){
                SDL_RenderDrawRect(renderer,&block[i][j].dsc);
            }
        }

        if(timeStart - timeEnd > 750){
            block[c][r].tt = 0;

            c = random(0,2);
            r = random(0,2);

            block[c][r].tt = random(1,3);
            timeEnd = timeStart;
        }
        switch(block[c][r].tt){
        case 1:
            //setRenderColor(renderer,LIME_COLOR);
            showImg(renderer,H1_PATH,&block[c][r].dsc);
            break;
        case 2:
            //setRenderColor(renderer,ORANGE_COLOR);
            showImg(renderer,H2_PATH,&block[c][r].dsc);
            break;
        case 3:
            //setRenderColor(renderer,RED_COLOR);
            showImg(renderer,H3_PATH,&block[c][r].dsc);
            break;

        }

        while(SDL_PollEvent(&e)){
            switch(e.type){
            case SDL_QUIT:
                isrunning = false;
                break;
            case SDL_MOUSEBUTTONDOWN:
                for(int i = 0;i < 3;i++){
                    for(int j = 0;j < 3;j++){
                        if(checkInBlock(mouse,block[i][j].dsc) && block[i][j].tt ){
                            cout<<'('<<i+1<<','<<j+1<<")\n";
                            switch(block[i][j].tt){
                            case 1:
                                score += 25;
                                break;
                            case 2:
                                score += 50;
                                break;
                            case 3:
                                score -= 75;
                                break;

                            }
                            cout<<"score: "<<score<<'\n';
                            block[c][r].tt = 0;

                            c = random(0,2);
                            r = random(0,2);

                            block[c][r].tt = random(1,3);
                            timeEnd = timeStart;

                        }
                    }
                }
                break;
            case SDL_KEYDOWN:
                if(e.key.keysym.sym == SDLK_ESCAPE){
                    isrunning = false;
                }
                break;
            case SDL_MOUSEMOTION:
                mouse.x = e.motion.x;
                mouse.y = e.motion.y;
                break;
            }
        }

        highscore = max(highscore,score);

        showText(renderer,text1,30,BLACK_COLOR,text1dsc);
        showText(renderer,to_string(score),30,BLACK_COLOR,text2dsc);
        showText(renderer,to_string(timeCoolDown),30,BLACK_COLOR,text3dsc);
        showText(renderer,text4,30,BLACK_COLOR,text4dsc);
        showText(renderer,text5,30,BLACK_COLOR,text5dsc);
        showText(renderer,to_string(highscore),30,BLACK_COLOR,text6dsc);


        SDL_RenderPresent(renderer);
        delay(time);

        if(timeCoolDown == 0){
            isrunning = false;
            cout<<"time out\n";
        }

        //block[c][r].tt = 0;
    }
    //waitUntilKeyPressed();
    f.open(HSC_PATH,ios_base::out);
    f<<highscore;
    f.close();
    s = score;
    hs = highscore;
}

bool home(SDL_Renderer* renderer,int& s,int& hs,bool reset)
{
    SDL_RenderClear(renderer);
    SDL_Point mouse;
    SDL_Event e;
    if(reset == false){
        SDL_Rect playDsc,exitDsc,textDsc;

        playDsc.h = 110;
        playDsc.w = 290;
        playDsc.x = 255;
        playDsc.y = 300;

        exitDsc.w = 290;
        exitDsc.h = 110;
        exitDsc.x = 255;
        exitDsc.y = 449;

        textDsc.h = 180;
        textDsc.w = 620;
        textDsc.x = 90;
        textDsc.y = 40;
        showImg(renderer,BKG_PATH,nullptr);

        setRenderColor(renderer,ORANGE_COLOR);

        SDL_RenderFillRect(renderer,&playDsc);
        SDL_RenderFillRect(renderer,&exitDsc);

        showText(renderer,WINDOW_TITLE,100,BLACK_COLOR,textDsc);
        showText(renderer,"PLAY",50,BLACK_COLOR,playDsc);
        showText(renderer,"EXIT",50,BLACK_COLOR,exitDsc);
        SDL_RenderPresent(renderer);

        while(true){
                //cout<<1<<'\n';
            while(SDL_PollEvent(&e)){
                switch(e.type){
                case SDL_QUIT:
                   // cout<<"sdsd\n";
                    return false;
                    break;
                case SDL_KEYDOWN:
                    if(e.key.keysym.sym == SDLK_ESCAPE)
                        return false;
                    break;
                case SDL_MOUSEMOTION:
                    mouse.x = e.motion.x;
                    mouse.y = e.motion.y;
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    if(e.button.button == SDL_BUTTON_LEFT){
                        if(checkInBlock(mouse,playDsc))
                            return true;
                        if(checkInBlock(mouse,exitDsc))
                            return false;
                        break;
                    }
                    break;
                }
                SDL_Delay(10);
            }
        }
        return false;
    }else{
        SDL_Rect restartDsc,exitDsc,text1Dsc,text2Dsc,text3Dsc;

        text1Dsc.h = 180;
        text1Dsc.w = 620;
        text1Dsc.x = 90;
        text1Dsc.y = 40;

        text2Dsc.h = 55;
        text2Dsc.w = 290;
        text2Dsc.x = 255;
        text2Dsc.y = 273;

        text3Dsc.h = 55;
        text3Dsc.w = 290;
        text3Dsc.x = 255;
        text3Dsc.y = 381;

        restartDsc.h = 55;
        restartDsc.w = 200;
        restartDsc.x = 155;
        restartDsc.y = 489;

        exitDsc.w = 200;
        exitDsc.h = 55;
        exitDsc.x = 445;
        exitDsc.y = 489;

        showImg(renderer,BKG_PATH,nullptr);
        setRenderColor(renderer,ORANGE_COLOR);
        SDL_RenderFillRect(renderer,&restartDsc);
        SDL_RenderFillRect(renderer,&exitDsc);

        showText(renderer,(hs > s ? "Your Score":"High Score"),100,BLACK_COLOR,text1Dsc);
        showText(renderer,"Score: "+to_string(s),40,BLACK_COLOR,text2Dsc);
        showText(renderer,"highScore: "+to_string(hs),40,BLACK_COLOR,text3Dsc);
        showText(renderer,"Restart",40,BLACK_COLOR,restartDsc);
        showText(renderer,"Exit",40,BLACK_COLOR,exitDsc);

        SDL_RenderPresent(renderer);
        //waitUntilKeyPressed();

        while(true){
            while(SDL_PollEvent(&e)){
                switch(e.type){
                case SDL_QUIT:
                    return false;
                case SDL_KEYDOWN:
                    if(e.key.keysym.sym == SDLK_ESCAPE)
                        return false;
                    break;
                case SDL_MOUSEMOTION:
                    mouse.x = e.motion.x;
                    mouse.y = e.motion.y;
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    if(e.button.button = SDL_BUTTON_LEFT){
                        if(checkInBlock(mouse,restartDsc))
                            return true;
                        if(checkInBlock(mouse,exitDsc))
                            return false;

                    }
                    break;

                }
            }
            SDL_Delay(10);
        }

        return false;
    }
}
