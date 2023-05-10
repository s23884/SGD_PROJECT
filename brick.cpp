#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_ttf.h>
#include <iostream>
#include <string>

#define WIDTH 620
#define HEIGHT 720
#define FONT_SIZE 32
#define BALL_SPEED 12
#define SPEED 9
#define SIZE 16
#define COL 13
#define ROW 8
#define PI 3.14159265358979323846
#define SPACING 16

SDL_Renderer* renderer;
SDL_Window* window;
TTF_Font* font;
SDL_Color color, bc;
bool running;
int frameCount, timerFPS, lastFrame, fps;

SDL_Rect paddle, ball, lives, brick;
float velY, velX;
int livesCount;
bool bricks[ROW*COL];

void resetBricks() {
    for(int i=0; i<COL*ROW; i++) {
        bricks[i]=1;
    }
    livesCount=3;
    paddle.x=(WIDTH/2)-(paddle.w/2);
    ball.y=paddle.y-(paddle.h*4);
    velY=BALL_SPEED/2;
    velX=0;
    ball.x=WIDTH/2-(SIZE/2);
}

void setBrick(int i) {
    brick.x=(((i%COL)+1)*SPACING)+((i%COL)*brick.w)-(SPACING/2);
    brick.y=brick.h*3+(((i%ROW)+1)*SPACING)+((i%ROW)*brick.h)-(SPACING/2);
}

void write(std::string text, int x, int y) {
    SDL_Surface *surface;
    SDL_Texture *texture;
    const char* t = text.c_str();
    surface = TTF_RenderText_Solid(font, t, color);
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    lives.w=surface->w;
    lives.h=surface->h;
    lives.x=x-lives.w;
    lives.y=y-lives.h;
    SDL_FreeSurface(surface);
    SDL_RenderCopy(renderer, texture, NULL, &lives);
    SDL_DestroyTexture(texture);
}

void update() {
    if(SDL_HasIntersection(&ball, &paddle)) {
        double rel = (paddle.x+(paddle.w/2))-(ball.x+(SIZE/2));
        double norm = rel/(paddle.w/2);
        double bounce = norm * (5*PI/12);
        velY=-BALL_SPEED*cos(bounce);
        velX=BALL_SPEED*-sin(bounce);
    }
    if(ball.y<=0) {velY=-velY;}//serve();}
    if(ball.y+SIZE>=HEIGHT) {velY=-velY;livesCount--;}//serve();}
    if(ball.x<=0 || ball.x+SIZE>=WIDTH) velX=-velX;
    ball.y+=velY;
    ball.x+=velX;
    if(paddle.x<0)paddle.x=0;
    if(paddle.x+paddle.w>WIDTH)paddle.x=WIDTH-paddle.w;
    if(livesCount<=0)resetBricks();

    bool reset=1;
    for(int i=0; i<COL*ROW; i++) {
        setBrick(i);
        if(SDL_HasIntersection(&ball, &brick) && bricks[i]) {
            bricks[i]=0;
            //velX=-velX;
            //velY=-velY;
            if(ball.x >= brick.x) {velX=velX * -1;ball.x=ball.x - 20;}
            if(ball.x <= brick.x) {velX=velX * -1;ball.x=ball.x + 20;}
            if(ball.y <= brick.y) {velY=velY * -1;ball.y=ball.y - 20;}
            if(ball.y >= brick.y) {velY=velY * -1;ball.y=ball.y + 20;}
        }
        if(bricks[i])reset=0;
    }
    if(reset)resetBricks();

}

void input() {
    SDL_Event e;
    const Uint8 *keystates = SDL_GetKeyboardState(NULL);
    while(SDL_PollEvent(&e)) if(e.type==SDL_QUIT) running=false;
    if(keystates[SDL_SCANCODE_ESCAPE]) running=false;
    if(keystates[SDL_SCANCODE_LEFT]) paddle.x-=SPEED;
    if(keystates[SDL_SCANCODE_RIGHT]) paddle.x+=SPEED;
}

void render() {
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 255);
    SDL_RenderClear(renderer);

//    frameCount++;
//    timerFPS = SDL_GetTicks()-lastFrame;
//    if(timerFPS<(1000/60)) {
//        SDL_Delay((1000/60)-timerFPS);
//    }

    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);
    SDL_RenderFillRect(renderer, &paddle);
    SDL_RenderFillRect(renderer, &ball);
    write(std::to_string(livesCount), WIDTH/2+FONT_SIZE/2, FONT_SIZE*1.5);

    for(int i=0; i<COL*ROW; i++) {
        SDL_SetRenderDrawColor(renderer, bc.r, bc.g, bc.b, 255);
        if(i%2==0)SDL_SetRenderDrawColor(renderer, bc.g, bc.r, bc.b, 255);
        if(bricks[i]) {
            setBrick(i);
            SDL_RenderFillRect(renderer, &brick);
        }
    }

    SDL_RenderPresent(renderer);
}

int main() {
    if(SDL_Init(SDL_INIT_EVERYTHING) < 0) std::cout << "Failed at SDL_Init()" << std::endl;
    if(SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, 0, &window, &renderer) < 0) std::cout << "Failed at SDL_CreateWindowAndRenderer())" << std::endl;
    SDL_SetWindowTitle(window, "Brick Breaker");
    TTF_Init();
    font = TTF_OpenFont("font.ttf", FONT_SIZE);
    running=1;
    static int lastTime = 0;

    color.r=color.g=color.b=255;
    bc.r=255;bc.g=bc.b=0;
    paddle.h=22; paddle.w=HEIGHT/4;
    paddle.h=12;
    ball.w=ball.h=SIZE;
    paddle.y=HEIGHT-paddle.h-32;

    brick.w=(WIDTH-(SPACING*COL))/COL;
    brick.h=22;
    resetBricks();

    auto prev_tick = SDL_GetTicks();//
    int frame_dropped = 0;//
    const int TICKS = 16;//
    while(running) {
//        lastFrame=SDL_GetTicks();
//        if(lastFrame>=(lastTime+1000)) {
//            lastTime=lastFrame;
//            fps=frameCount;
//            frameCount=0;
//        }

        if (!frame_dropped) {//
            update();
            input();
            render();
        }//

        auto curr_tick = SDL_GetTicks();//

        if (curr_tick - prev_tick < TICKS) {//
            SDL_Delay(TICKS - (curr_tick - prev_tick));//
            frame_dropped = 0;//
        } else frame_dropped = 1;//

        prev_tick += TICKS;//

    }
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}