#define _CRT_SECURE_NO_WARNINGS 1

#include <graphics.h>
#include <conio.h>
#include <thread>
#include <chrono>
#include <iostream>
#include <vector>
#include <string>
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

#include "allinclude.h"
using namespace std;

// Global hero object (initialized after hero selection)
hero hero1;

const int SCROLL_SPEED = 1;
static int volume = 100;

// Ending credits text
std::vector<std::string> storyText = {
    "终章 · 星魂归途", "",
    "千年前，神魔之战撕裂大陆，守护天使的星魂散落在三座魔塔。",
    "魔王用黑暗侵蚀生灵，而古代法师以生命换来了漫长的封印。", "",
    "在雨落古堡的那一夜，一只紫藤葫芦从天而降。",
    "你拾起它，也拾起了最后一枚尚未熄灭的星光。",
    "穿过古堡、深渊与终焉之塔，你击败了守卫和最后的魔王。", "",
    "当终焉的钟声停下，散落的星魂重新汇聚。",
    "守护者没有回到遥远的神座——她化作光，留在每一个被守护的人心中。",
    "而那只葫芦回到了故事开始的地方，安静地陪你看完最后一场雨。", "",
    "冒险至此落幕，但爱让所有走过的道路都有了意义。", "",
    "制作与程序：杨博涛", "故事与世界：魔塔 · 星魂纪元",
    "特别致谢：小菁菁的爱", "",
    "谢谢你陪我走到这里。", "愿每一次归来，都有人在灯火下等你。"
};

void DrawOptionPanel();
void setvol(int vol);
void DrawScrollingText(int offsetY);
void resetHero();

// Reset hero state for replay
void resetHero() {
    hero1.x = 100;
    hero1.y = 180;
    hero1.hp = hero1.maxHp;
    hero1.mana = hero1.maxMana;
    hero1.attackkey = false;
    hero1.Lwalkkey = false;
    hero1.Rwalkkey = false;
    hero1.jumpkey = false;
    hero1.attackL = false;
    hero1.attackR = true;  // Face right by default
    hero1.hasDealtDamage = false;
    hero1.isHit = false;
    hero1.isSkill = false;
    hero1.hitCounter = 0;
    hero1.skillCounter = 0;
    hero1.sleep = false;
    hero1.sleeptime = 0;
}

// Draw scrolling ending credits
void DrawScrollingText(int offsetY) {
    settextcolor(WHITE);
    settextstyle(24, 0, "Microsoft YaHei");
    int lineHeight = 40;
    int y = _WINDOW_LONG__ - offsetY;
    for (const auto& line : storyText) {
        outtextxy((_WINDOW_WIDTH__ - textwidth(line.c_str())) / 2, y, line.c_str());
        y += lineHeight;
    }
}

// Set system volume
void setvol(int vol) {
    int vol1 = max(0, min(100, vol));
    DWORD vol2 = vol1 * 65535 / 100;
    waveOutSetVolume(0, vol2 | (vol2 << 16));
}

// Volume settings panel
void DrawOptionPanel() {
    for(int y=0;y<_WINDOW_LONG__;++y){setlinecolor(RGB(11+y/80,8,22+y/34));line(0,y,_WINDOW_WIDTH__,y);}
    int l=125,t=42,r=835,b=498;
    setfillcolor(RGB(24,17,28));setlinecolor(RGB(151,105,49));fillroundrect(l,t,r,b,18,18);rectangle(l+8,t+8,r-8,b-8);
    setbkmode(TRANSPARENT);settextstyle(36,0,"Microsoft YaHei");settextcolor(RGB(240,205,132));
    const char* title="游戏设置";outtextxy((_WINDOW_WIDTH__-textwidth(title))/2,65,title);
    settextstyle(16,0,"Microsoft YaHei");settextcolor(RGB(144,128,153));const char* sub="调整旅途体验 · 设置即时保存";outtextxy((_WINDOW_WIDTH__-textwidth(sub))/2,108,sub);
    setlinecolor(RGB(105,72,40));line(l+38,140,r-38,140);
    settextstyle(22,0,"Microsoft YaHei");settextcolor(RGB(217,173,94));outtextxy(l+48,165,"音频");
    settextstyle(17,0,"Microsoft YaHei");settextcolor(RGB(210,205,213));outtextxy(l+48,207,"主音量");
    RECT slider={l+155,210,r-70,226};setfillcolor(RGB(49,42,54));solidrectangle(slider.left,slider.top,slider.right,slider.bottom);
    int pos=slider.left+(slider.right-slider.left)*volume/100;setfillcolor(RGB(166,108,45));solidrectangle(slider.left,slider.top,pos,slider.bottom);
    setfillcolor(RGB(241,194,102));setlinecolor(RGB(255,225,158));fillcircle(pos,(slider.top+slider.bottom)/2,10);
    char vt[32];sprintf_s(vt,"%d%%",volume);settextcolor(RGB(240,213,159));outtextxy(r-57,204,vt);
    settextstyle(13,0,"Microsoft YaHei");settextcolor(RGB(125,116,135));outtextxy(slider.left,234,"0");outtextxy((slider.left+slider.right)/2-8,234,"50");outtextxy(slider.right-22,234,"100");
    settextstyle(22,0,"Microsoft YaHei");settextcolor(RGB(217,173,94));outtextxy(l+48,280,"操作说明");
    settextstyle(16,0,"Microsoft YaHei");settextcolor(RGB(205,199,210));
    outtextxy(l+48,322,"A / D   左右移动       J   普通攻击       K   跳跃");
    outtextxy(l+48,354,"L / Q   角色技能       E   使用法宝       R   拾取物品");
    outtextxy(l+48,386,"1       打开背包       鼠标拖拽装备      ESC 返回");
    setfillcolor(RGB(35,26,39));setlinecolor(RGB(83,62,91));fillroundrect(l+45,426,r-45,464,8,8);
    settextstyle(14,0,"Microsoft YaHei");settextcolor(RGB(155,140,166));outtextxy(l+65,436,"提示：法宝技能会消耗冷却时间，装备可在背包中拖入或卸下。按 ESC 返回主菜单。");
    while(MouseHit()){MOUSEMSG msg=GetMouseMsg();if((msg.uMsg==WM_LBUTTONDOWN||msg.uMsg==WM_MOUSEMOVE)&&msg.mkLButton&&msg.x>=slider.left-8&&msg.x<=slider.right+8&&msg.y>=slider.top-14&&msg.y<=slider.bottom+14){volume=max(0,min(100,(msg.x-slider.left)*100/(slider.right-slider.left)));setvol(volume);}}
}

// Play ending credits sequence
void playCredits() {
    cleardevice();
    int offsetY = 0;
    int totalHeight = storyText.size() * 40 + _WINDOW_LONG__;

    BeginBatchDraw();
    while (offsetY < totalHeight) {
        cleardevice();
        DrawScrollingText(offsetY);
        offsetY += 2;
        FlushBatchDraw();
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
    EndBatchDraw();
}

void ensureAssetWorkingDirectory() {
    if (GetFileAttributesA("./bg/Background_8K.png") != INVALID_FILE_ATTRIBUTES) return;
    const char* candidates[] = { "../../MagicTowerGame", "../MagicTowerGame", "MagicTowerGame" };
    for (const char* dir : candidates) {
        char test[MAX_PATH];
        sprintf_s(test, "%s/bg/Background_8K.png", dir);
        if (GetFileAttributesA(test) != INVALID_FILE_ATTRIBUTES) {
            SetCurrentDirectoryA(dir);
            return;
        }
    }
}

int main() {
    ensureAssetWorkingDirectory();
    initgraph(_WINDOW_WIDTH__, _WINDOW_LONG__);
    // Open every audio device once to prevent MP3 decoder stalls during gameplay.
    mciSendString("open \"./music/begin.mp3\" type mpegvideo alias bgm", 0, 0, 0);
    mciSendString("open \"./music/111.mp3\" type mpegvideo alias thunder", 0, 0, 0);
    mciSendString("open \"./music/nu.mp3\" type mpegvideo alias doorMain", 0, 0, 0);
    mciSendString("open \"./music/long.mp3\" type mpegvideo alias doorLevel", 0, 0, 0);
    mciSendString("play bgm from 0 repeat", 0, 0, 0);

    int key = 0;
    bool volumset = false;

    showLoveScreen();  // Love dedication screen

    while (1) {
        int result = 0;
        key = welcome(); // Show welcome menu

        if (key == 1) {
            // ---- Start New Game ----
            cleardevice();

            // Hero selection
            int heroChoice = selectHero();
            hero1.loadHeroSprites((HeroType)heroChoice);
            hero1.heroType = (HeroType)heroChoice;
            const HeroTypeStats& st = HERO_STATS[heroChoice];
            hero1.hp = st.hp;      hero1.maxHp = st.hp;
            hero1.hurt = st.attack; hero1.magicAttack = st.magic;
            hero1.defense = st.defense; hero1.speed = st.speed;
            hero1.critChance = st.critChance;
            hero1.mana = hero1.maxMana = 100;
            resetTreasureSystem();
            hero1.x = 100; hero1.y = 180;  // Initial position

            loading();       // Loading screen
            mainwelcome();   // Main world
            updatemen();     // Door animation 1

            // Level loop: allows replaying levels or going back to map
            bool inLevelLoop = true;
            while (inLevelLoop) {
                int levelChoice = mymap(); // Level select map

                if (levelChoice == 1) {
                    updatemen1();     // Door animation 2

                    // Reset enemy and hero for a fresh attempt
                    resetHero();
                    empty1.reset(700, 280);

                    result = level1(); // Play level 1

                    // Show game over screen
                    int goChoice = 0;
                    if (result == 1) {
                        // Victory
                        hero1.levelsCleared++;
                        goChoice = showGameOver(true);
                    }
                    else if (result == 2) {
                        // Defeat
                        goChoice = showGameOver(false);
                    }

                    // Handle game over choice
                    if (goChoice == 1) {
                        // Play Again - loop back to level 1
                        continue;
                    }
                    else if (goChoice == 2) {
                        // Level Map - go back to map
                        showMST = false;
                        continue;
                    }
                    else if (goChoice == 3) {
                        // Main Menu - exit level loop
                        inLevelLoop = false;
                    }
                }
                else if (levelChoice == 2) {
                    bool replayLevel2 = true;
                    while (replayLevel2 && inLevelLoop) {
                        resetHero();
                        beginLevelLoot();
                        result = level2();

                        if (result == 1) {
                            // Level 2 already asked for Enter confirmation inside the scene.
                            // Do not open another result screen: the same Enter would leak into it.
                            hero1.levelsCleared++;
                            result = 0;
                            replayLevel2 = false;
                            showMST = false;
                            break;
                        }

                        int goChoice = showGameOver(false);
                        if (goChoice == 1) {
                            replayLevel2 = true;
                        }
                        else if (goChoice == 2) {
                            replayLevel2 = false;
                            showMST = false;
                        }
                        else {
                            replayLevel2 = false;
                            inLevelLoop = false;
                        }
                    }
                    if (inLevelLoop) continue;
                }
                else if (levelChoice == 3) {
                    resetHero();
                    result = level3();
                    if (result == 1) {
                        hero1.levelsCleared++;
                        showCompletionHome();
                        playCredits();
                        result = 3;
                        inLevelLoop = false;
                    } else {
                        int goChoice = showGameOver(false);
                        if (goChoice == 1 || goChoice == 2) { showMST = false; continue; }
                        inLevelLoop = false;
                    }
                }
                else {
                    // Player didn't select a valid level
                    inLevelLoop = false;
                }
            }

            // After level loop ends, check if we should break the main loop
            if (result == 3) {
                break; // Three chapters completed
            }
        }
        else if (key == 2) {
            // ---- Options / Volume ----
            cleardevice();
            volumset = true;
            while (volumset) {
                BeginBatchDraw();
                cleardevice();
                DrawOptionPanel();
                if (_kbhit()) {
                    char ch = _getch();
                    if (ch == 27) { // ESC to exit
                        volumset = false;
                    }
                }
                EndBatchDraw();
                Sleep(10);
            }
        }
        else if (key == 3) {
            // ---- Exit Game ----
            break;
        }
    }

    // Exit: close music and graphics
    mciSendString("close doorLevel", 0, 0, 0);
    mciSendString("close doorMain", 0, 0, 0);
    mciSendString("close thunder", 0, 0, 0);
    mciSendString("close bgm", 0, 0, 0);
    closegraph();
    TerminateProcess(GetCurrentProcess(), 0);
    return 0;
}
