#ifndef __ALLINCLUDE_H__
#define __ALLINCLUDE_H__
#define _CRT_SECURE_NO_WARNINGS 1

#define _WINDOW_WIDTH__ 960
#define _WINDOW_LONG__  540

#include<graphics.h>
#include<iostream>
#include<conio.h>
#include<vector>
#include<string>
#include <vector>
#include <queue>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <algorithm>

// Music
#include<Windows.h>
#include<mmsystem.h>
#pragma comment(lib,"winmm.lib")

#include"hero.h"
#include"particle.h"
#include"Enemy.h"

using namespace std;

struct Animation {
    int currentFrame;     // Current frame being played
    int frameCount;       // Total frames
    float frameTime;      // Time per frame (seconds)
    float elapsedTime;    // Accumulated time for current frame
    bool loop;            // Whether animation loops
};

struct AttackBox {
    int x, y;             // Top-left position
    int width, height;    // Dimensions
    bool active;          // Whether attack hitbox is active
};

// Game scene functions
extern void showLoveScreen();
extern int welcome();
extern int loading();
extern void mainwelcome();
extern void updatehero();
extern void keydetect();
extern void updateUI();
extern void init();
extern int UIdetect();
extern void resetTreasureSystem();
extern void updateTreasureSystem();
extern void grantTreasure(int id);
extern int getEquippedTreasure();
extern void spawnBossLoot();
extern void spawnLevelLoot(int level);
extern void beginLevelLoot();
extern void updateAndDrawLoot();
extern bool hasUncollectedBossLoot();
extern bool isBossLootSpawned();
extern int mymap();
extern int level1();
extern int level2();
extern int level3();
extern void showCompletionHome();
extern int showGameOver(bool victory);
extern int selectHero();

// Level transition animations
extern void updatemen();
extern void updatemen1();

// Utility functions
extern void putimagePNG(int  picture_x, int picture_y, IMAGE* picture);
extern void putimagePNG2(int x, int y, IMAGE* picture);
extern void putimagePNG2(int x, int y, int winWidth, IMAGE* picture);
extern void scaleImage(IMAGE* dst, IMAGE* src, int newW, int newH);
extern void UpdateAnimation(Animation& anim, float deltaTime);
extern void updateAttackBox(hero& h);

// Global objects
extern hero hero1;
extern bool isJumping;
extern bool showMST;
extern vector<IMAGE> bin;
extern Enemy empty1;

#endif
