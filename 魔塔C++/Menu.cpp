#include <graphics.h>
#include <conio.h>
#include <vector>
#include <string>
#include <random>
#include <cmath>
#include <algorithm>
#include"allinclude.h"
using namespace std;

// Star particle structure
struct Star {
    float x, y;
    float speed;
    float brightness;
};

// HSV to RGB converter
// H: 0-360, S: 0-1, V: 0-1
COLORREF HSVtoRGB(int h, float s, float v) {
    float r, g, b;
    int i = h / 60;
    float f = h / 60.0f - i;
    float p = v * (1 - s);
    float q = v * (1 - f * s);
    float t = v * (1 - (1 - f) * s);
    switch (i % 6) {
    case 0: r = v; g = t; b = p; break;
    case 1: r = q; g = v; b = p; break;
    case 2: r = p; g = v; b = t; break;
    case 3: r = p; g = q; b = v; break;
    case 4: r = t; g = p; b = v; break;
    case 5: r = v; g = p; b = q; break;
    }
    return RGB((int)(r * 255), (int)(g * 255), (int)(b * 255));
}

// Initialize starfield particles
vector<Star> initStars(int count) {
    vector<Star> stars;
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<float> xDist(0, (float)_WINDOW_WIDTH__);
    uniform_real_distribution<float> yDist(0, (float)_WINDOW_LONG__);
    uniform_real_distribution<float> speedDist(0.5, 2.0);
    uniform_real_distribution<float> brightDist(120, 235);

    for (int i = 0; i < count; ++i) {
        stars.push_back({ xDist(gen), yDist(gen), speedDist(gen), brightDist(gen) });
    }
    return stars;
}

// Update and draw starfield
void updateStars(vector<Star>& stars) {
    for (auto& star : stars) {
        star.y += star.speed;
        if (star.y > _WINDOW_LONG__) {
            star.y = 0;
            star.x = rand() % _WINDOW_WIDTH__;
        }
        setfillcolor(RGB((int)star.brightness, (int)star.brightness, (int)star.brightness));
        fillcircle((int)star.x, (int)star.y, 2 + (int)(star.speed * 0.9));
    }
}

// Get text width with specified font
int GetTextWidth(const char* text, int fontSize, const char* fontName = "SimHei") {
    LOGFONT f;
    gettextstyle(&f);
    int oldSize = f.lfHeight;
    f.lfHeight = fontSize;
    strcpy_s(f.lfFaceName, fontName);
    settextstyle(&f);
    int width = textwidth(text);
    f.lfHeight = oldSize;
    settextstyle(&f);
    return width;
}

static void showStoryScreen() {
    const char* story[] = {
        "千年前，神魔之战撕裂大陆，守护天使陨落，星魂散落于魔塔。",
        "魔王以星魂污染生灵，古代法师耗尽力量，将灾厄暂时封印。",
        "如今封印衰弱，雨夜古堡再次传来龙吟，深渊王座也已苏醒。",
        "你携最后一枚星屑踏入魔塔。它能唤醒法宝，也会引来黑暗。",
        "寻找失落装备，与小菁菁并肩前行，让守护者的光重返人间。"
    };
    while (true) {
        BeginBatchDraw();
        for (int y=0;y<_WINDOW_LONG__;++y) { setlinecolor(RGB(12+y/70,8,24+y/35)); line(0,y,_WINDOW_WIDTH__,y); }
        int l=80,t=42,r=_WINDOW_WIDTH__-80,b=_WINDOW_LONG__-42;
        setfillcolor(RGB(23,16,28)); setlinecolor(RGB(159,113,52)); fillroundrect(l,t,r,b,18,18); rectangle(l+8,t+8,r-8,b-8);
        setbkmode(TRANSPARENT); settextstyle(38,0,"Microsoft YaHei"); settextcolor(RGB(240,205,132));
        const char* title="世界故事"; outtextxy((_WINDOW_WIDTH__-textwidth(title))/2,67,title);
        settextstyle(20,0,"Microsoft YaHei"); settextcolor(RGB(178,132,207)); const char* chapter="序章 · 星魂破碎之夜"; outtextxy((_WINDOW_WIDTH__-textwidth(chapter))/2,116,chapter);
        setlinecolor(RGB(109,75,42)); line(l+48,151,r-48,151); settextstyle(17,0,"Microsoft YaHei"); settextcolor(RGB(220,213,199));
        for(int i=0;i<5;++i) outtextxy(l+45,178+i*48,story[i]);
        settextcolor(RGB(207,157,78)); outtextxy(l+45,425,"第一幕  雨夜古堡     第二幕  深渊王座     终章  守护者归来");
        settextstyle(15,0,"Microsoft YaHei"); settextcolor(RGB(143,135,154)); const char* hint="按 Enter 或 ESC 返回主菜单"; outtextxy((_WINDOW_WIDTH__-textwidth(hint))/2,b-32,hint);
        FlushBatchDraw(); if(_kbhit()){int c=_getch();if(c==13||c==27){EndBatchDraw();return;}} Sleep(16);
    }
}
int welcome() {

    vector<Star> stars = initStars(150);

    vector<string> menuItems = {
        "开始冒险", "游戏设置", "世界故事", "离开游戏"
    };

    int selectedIndex = 0;

    const int menuItemHeight = 60;
    const int menuTotalHeight = menuItems.size() * menuItemHeight;
    const int menuStartY = (_WINDOW_LONG__ - menuTotalHeight) / 2;

    // Fire particle for menu ambiance
    struct FireParticle {
        float x, y;
        float speedY;
        int lifetime;
        int age;
    };

    vector<FireParticle> fireParticles;

    // Random generators
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<float> fireXDist(0, (float)_WINDOW_WIDTH__);
    uniform_real_distribution<float> fireSpeedDist(1.0f, 3.0f);
    uniform_int_distribution<int> fireLifeDist(30, 60);

    // Main menu loop
    while (true) {
        BeginBatchDraw();
        cleardevice();
        updateStars(stars);

        // Spawn new fire particles from bottom
        for (int i = 0; i < 8; ++i) {
            fireParticles.push_back({
                fireXDist(gen),
                (float)_WINDOW_LONG__,
                fireSpeedDist(gen),
                fireLifeDist(gen),
                0
                });
        }

        // Update and draw fire particles
        for (auto it = fireParticles.begin(); it != fireParticles.end();) {
            it->y -= it->speedY;
            it->age++;

            float progress = (float)it->age / it->lifetime;
            int hue = (int)(40 - 40 * progress);
            if (hue < 0) hue = 0;
            COLORREF color = HSVtoRGB(hue, 1.0f, 1.0f - progress * 0.5f);

            setfillcolor(color);
            fillcircle((int)it->x, (int)it->y, 3);

            if (it->age >= it->lifetime)
                it = fireParticles.erase(it);
            else
                ++it;
        }

        // Title with floating animation
        static float titleOffset = 0;
        titleOffset += 0.04f;
        int titleY = 50 + (int)(6 * sin(titleOffset));

        settextcolor(WHITE);
        settextstyle(48, 0, _T("Microsoft YaHei"));
        const char* title = "魔塔 · 星魂纪元";
        int titleWidth = GetTextWidth(title, 48, "Microsoft YaHei");
        outtextxy((_WINDOW_WIDTH__ -titleWidth) / 2, titleY, _T(title));

        // Draw menu items
        for (int i = 0; i < (int)menuItems.size(); i++) {
            int itemY = menuStartY + i * menuItemHeight;

            if (i == selectedIndex) {
                // Rainbow color cycling for selected item
                static int hue = 0;
                hue = (hue + 2) % 360;
                COLORREF color = HSVtoRGB(hue, 1, 1);
                settextcolor(color);
                settextstyle(36, 0, _T("Microsoft YaHei"));

                // Pulsing indicator dot
                int pulseSize = 6 + (int)(2 * sin(GetTickCount() / 200.0));
                setfillcolor(HSVtoRGB((hue + 180) % 360, 1, 1));
                fillcircle(_WINDOW_WIDTH__ / 2 - 100, itemY + 20, pulseSize);
            }
            else {
                settextcolor(RGB(180, 180, 180));
                settextstyle(32, 0, _T("Microsoft YaHei"));
            }

            int textWidth = GetTextWidth(menuItems[i].c_str(), 32, "Microsoft YaHei");
            int centerX = (_WINDOW_WIDTH__ -textWidth) / 2;
            outtextxy(centerX, itemY, menuItems[i].c_str());
        }

        // Input handling
        if (_kbhit()) {
            char ch = _getch();
            if (ch == 0 || ch == -32) {
                ch = _getch();
                switch (ch) {
                case 72: // Up arrow
                    selectedIndex = (selectedIndex - 1 + menuItems.size()) % menuItems.size();
                    break;
                case 80: // Down arrow
                    selectedIndex = (selectedIndex + 1) % menuItems.size();
                    break;
                }
            }
            else if (ch == 13) { // Enter key
                if (selectedIndex == 0) {
                    return 1;
                }
                else if (selectedIndex == 1) {
                    return 2;
                }
                else if (selectedIndex == 2) {
                    showStoryScreen();
                }
                else if (selectedIndex == 3) {
                    EndBatchDraw();
                    return 3;
                }
            }
        }

        FlushBatchDraw();
        Sleep(16);
    }

    return 0;
}
