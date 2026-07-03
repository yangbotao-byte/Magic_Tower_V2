#include "allinclude.h"
#include <vector>
#include <random>
#include <cmath>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

using namespace std;

struct Raindrop {
    float x, y;
    float speedY;
    float speedX;
};

struct Splash {
    float x, y;
    int life;
};

// Lightning state
int lightningTimer = 0;
bool isLightning = false;
int lightningDuration = 0;

const int RAIN_NUM = 200;
vector<Raindrop> raindrops;
vector<Splash> splashes;

// Jump physics
static Animation jumpAnim = { 0, 6, 0.5f, 0.0f, true };

void updateBG(IMAGE world[]);
void updateRain();
void updateSplash();
void updateLightning();

bool isJumping = false;
float jumpSpeed = 0.0f;
const float gravity = 1.2f;
const float initialJumpSpeed = -18.0f;

void mainwelcome() {
    init();

    // Load and scale parallax background layers
    IMAGE world1[3];
    char image[200];
    for (int i = 0; i < 3; i++) {
        sprintf(image, "./bg\\Background_layer0%d.png", i);
        IMAGE temp;
        loadimage(&temp, image);
        scaleImage(&world1[i], &temp, _WINDOW_WIDTH__, _WINDOW_LONG__);
    }

    // Initialize raindrops
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<float> xDist(0, 1300);
    uniform_real_distribution<float> yDist(-600, 0);
    uniform_real_distribution<float> speedDistY(6.0f, 12.0f);
    uniform_real_distribution<float> speedDistX(-2.0f, 2.0f);

    for (int i = 0; i < RAIN_NUM; ++i) {
        raindrops.push_back({ xDist(gen), yDist(gen), speedDistY(gen), speedDistX(gen) });
    }

    // Main world loop
    DWORD lastTime = GetTickCount();
    while (1) {
        // Frame delta time
        DWORD currentTime = GetTickCount();
        float deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;

        keydetect();
        UIdetect();

        BeginBatchDraw();
        updateBG(world1);
        updateRain();
        updateSplash();
        updateLightning();

        // Jump initiation
        if (hero1.jumpkey && !isJumping) {
            isJumping = true;
            jumpSpeed = initialJumpSpeed;
            jumpAnim.currentFrame = 0;
            jumpAnim.elapsedTime = 0;
        }

        // Jump physics
        if (isJumping) {
            hero1.y += jumpSpeed;
            jumpSpeed += gravity;

            // Allow left/right movement during jump
            if (hero1.Lwalkkey) { hero1.x -= hero1.speed; hero1.attackL = true; hero1.attackR = false; }
            if (hero1.Rwalkkey) { hero1.x += hero1.speed; hero1.attackL = false; hero1.attackR = true; }

            // Screen bounds
            if (hero1.x < 0) hero1.x = 0;
            if (hero1.x > _WINDOW_WIDTH__ - 100) hero1.x = _WINDOW_WIDTH__ - 100;
            if (hero1.y < 10) { hero1.y = 10; jumpSpeed = 0; }

            // Attack/skill during jump or jump animation
            bool jAtk = (GetAsyncKeyState('J') & 0x8000) != 0;
            bool jSk = (GetAsyncKeyState('L') & 0x8000) != 0;
            bool jQ = (GetAsyncKeyState('Q') & 0x8000) != 0;
            if (jAtk && !hero1.atkAnimR.empty()) {
                static int c = 0; c++; putimagePNG2(hero1.x, hero1.y, &hero1.atkAnimR[c % hero1.atkAnimR.size()]);
            } else if (jSk && !hero1.skillAnimR.empty()) {
                static int c = 0; c++; putimagePNG2(hero1.x, hero1.y, &hero1.skillAnimR[c % hero1.skillAnimR.size()]);
            } else if (jQ && !hero1.qskillAnimR.empty()) {
                static int c = 0; c++; putimagePNG2(hero1.x, hero1.y, &hero1.qskillAnimR[c % hero1.qskillAnimR.size()]);
            } else if (!hero1.jumpAnimR.empty()) {
                UpdateAnimation(jumpAnim, deltaTime);
                int jf = jumpAnim.currentFrame % hero1.jumpAnimR.size();
                putimagePNG2(hero1.x, hero1.y, &hero1.jumpAnimR[jf]);
            } else if (!hero1.idleAnimR.empty()) {
                int jf = jumpAnim.currentFrame % hero1.idleAnimR.size();
                putimagePNG2(hero1.x, hero1.y, &hero1.idleAnimR[jf]);
            }

            // Landing detection
            if (hero1.y >= 180) {
                hero1.y = 180;
                isJumping = false;
                hero1.jumpkey = false;
                jumpSpeed = 0;
            }
        }
        else {
            // Ground movement
            updatehero();
        }

        updateUI();
        EndBatchDraw();
        Sleep(35);

        // W key to advance (anywhere near chuan or right side)
        bool key = (GetAsyncKeyState('W') & 0x8000) != 0;
        if (key == true && hero1.x > 400) {
            break;
        }
        cleardevice();
    }
}

void updateBG(IMAGE world[]) {
    putimagePNG2(0, 0, &world[2]);
    putimagePNG2(0, 100, &world[1]);
    putimagePNG2(0, 0, &world[0]);
}

void updateRain() {
    for (auto& drop : raindrops) {
        drop.x += drop.speedX;
        drop.y += drop.speedY;

        setlinecolor(RGB(180, 180, 255));
        line(drop.x, drop.y, drop.x - drop.speedX * 1.5f, drop.y - drop.speedY * 1.5f);

        // Splash when hitting ground
        if (drop.y > 580) {
            splashes.push_back({ drop.x, 580, 10 });
            drop.y = -10;
            drop.x = rand() % 1300;
            drop.speedY = 6.0f + rand() % 6;
            drop.speedX = -2.0f + rand() % 5;
        }
    }
}

void updateSplash() {
    for (int i = 0; i < (int)splashes.size(); i++) {
        setlinecolor(RGB(150, 150, 255));
        circle(splashes[i].x, splashes[i].y, 3 + (10 - splashes[i].life));

        splashes[i].life--;
        if (splashes[i].life <= 0) {
            splashes.erase(splashes.begin() + i);
            i--;
        }
    }
}

void updateLightning() {
    if (isLightning) {
        // Draw zigzag lightning bolt
        setlinecolor(WHITE);
        int startX = rand() % 1200 + 50;
        int y = 0;
        while (y < 600) {
            int nextX = startX + (rand() % 41 - 20);
            int nextY = y + rand() % 40 + 20;
            line(startX, y, nextX, nextY);
            startX = nextX;
            y = nextY;
        }

        lightningDuration--;
        if (lightningDuration <= 0) {
            isLightning = false;
            lightningTimer = rand() % 200 + 10;
        }
    }
    else {
        lightningTimer--;
        if (lightningTimer <= 0) {
            isLightning = true;
            lightningDuration = 3;
            // Play thunder sound
            mciSendString("stop thunder", 0, 0, 0);
            mciSendString("play thunder from 0", 0, 0, 0);
        }
    }
}

void showCompletionHome() {
    init();
    IMAGE world[3];
    for(int i=0;i<3;++i){char path[100];sprintf_s(path,"./bg/Background_layer0%d.png",i);IMAGE raw;loadimage(&raw,path);scaleImage(&world[i],&raw,_WINDOW_WIDTH__,_WINDOW_LONG__);}
    hero1.x=430; hero1.y=180;
    bool uWasDown=true;
    while(true){
        BeginBatchDraw(); updateBG(world); updateRain(); updateSplash(); updateLightning();
        if(!hero1.idleAnimR.empty()){int f=(GetTickCount()/90)%hero1.idleAnimR.size();putimagePNG2(hero1.x,hero1.y,&hero1.idleAnimR[f]);}
        updateUI();
        int l=155,t=62,r=805,b=170;
        setfillcolor(RGB(16,13,25));setlinecolor(RGB(220,171,78));fillroundrect(l,t,r,b,14,14);rectangle(l+7,t+7,r-7,b-7);
        setbkmode(TRANSPARENT);settextstyle(27,0,"Microsoft YaHei");settextcolor(RGB(245,215,145));
        const char* msg1="三座魔塔已经沉寂，你已顺利通关";outtextxy((_WINDOW_WIDTH__-textwidth(msg1))/2,82,msg1);
        settextstyle(17,0,"Microsoft YaHei");settextcolor(RGB(188,151,218));const char* msg2="葫芦回到了最初降落之地，星魂也终于找到了归途。";outtextxy((_WINDOW_WIDTH__-textwidth(msg2))/2,120,msg2);
        settextcolor(RGB(238,190,91));const char* msg3="按 U 键进入终章与致谢";outtextxy((_WINDOW_WIDTH__-textwidth(msg3))/2,145,msg3);
        FlushBatchDraw();
        bool down=(GetAsyncKeyState('U')&0x8000)!=0;if(down&&!uWasDown){EndBatchDraw();return;}uWasDown=down;Sleep(30);
    }
}