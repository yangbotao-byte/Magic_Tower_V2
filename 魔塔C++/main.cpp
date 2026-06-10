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

#include "allinclude.h" // 你的项目文件头
using namespace std;

// 示例：英雄对象
hero hero1(100, 180, 6, 8, 16);

const int SCROLL_SPEED = 1;
static int volume = 100;

std::vector<std::string> storyText = {
    "故事结束了……",
    "",
    "小骷髅成功完成了它的使命，",
    "在烈火与寒冰中穿梭，",
    "拯救了陷入困境的世界。",
    "",
    "但它知道，真正的和平仍需守护。",
    "于是，它踏上了新的旅途。",
    "",
    "感谢你陪它一起走过这段旅程。",
    "",
    "—— 制作人：杨博涛",
    "—— 特别鸣谢：杨博涛的大脑 + 小菁菁嘟爱",
    "",
    "感谢游玩！"
};

// 函数声明
void DrawOptionPanel();
void setvol(int vol);
void DrawScrollingText(int offsetY);

// 滚动剧情显示
void DrawScrollingText(int offsetY) {
    settextcolor(WHITE);
    settextstyle(24, 0, "微软雅黑");
    int lineHeight = 40;
    int y = _WINDOW_LONG__ - offsetY;
    for (const auto& line : storyText) {
        outtextxy((_WINDOW_WIDTH__ - textwidth(line.c_str())) / 2, y, line.c_str());
        y += lineHeight;
    }
}

int main() {
    initgraph(_WINDOW_WIDTH__, _WINDOW_LONG__);
    mciSendString("open ./music\\begin.mp3 alias bgm", 0, 0, 0);
    mciSendString("play bgm repeat", 0, 0, 0);

    int key = 0;
    bool volumset = false;

    while (1) {
        int result = 0;
        key = welcome(); // 你已有的欢迎函数
        if (key == 1) {
            cleardevice();
            loading();       // 你的加载动画
            mainwelcome();   // 主菜单
            updatemen();     // 菜单刷新
            int i = mymap(); // 地图选择
            cout << i << endl;

            if (i == 1) {
                updatemen1();     // 更新菜单
                result = level1(); // 启动关卡
            }

            // 游戏结束后滚动故事
            if (result == 1) {
                cleardevice();
                int offsetY = 0;
                int totalHeight = storyText.size() * 40 + _WINDOW_LONG__;

                BeginBatchDraw();
                while (offsetY < totalHeight) {
                    cleardevice();
                    DrawScrollingText(offsetY);
                    offsetY += 2; // 提高速度
                    FlushBatchDraw();
                    std::this_thread::sleep_for(std::chrono::milliseconds(16));
                }
                EndBatchDraw();
                break;
            }

        }
        else if (key == 2) {
            cleardevice();
            volumset = true;
            while (volumset) {
                BeginBatchDraw();
                cleardevice();
                DrawOptionPanel();
                if (_kbhit()) {
                    char ch = _getch();
                    if (ch == 27) { // ESC 退出
                        volumset = false;
                    }
                }
                EndBatchDraw();
                Sleep(10);
            }

        }
        else if (key == 3) {
            break;
        }
    }

    // 退出前关闭音乐与图形窗口
    mciSendString("close bgm", 0, 0, 0);
    closegraph();
    TerminateProcess(GetCurrentProcess(), 0);
    return 0;
}

// 音量设置函数
void setvol(int vol) {
    int vol1 = max(0, min(100, vol));
    DWORD vol2 = vol1 * 65535 / 100;
    waveOutSetVolume(0, vol2 | (vol2 << 16));
}

// 音量设置面板
void DrawOptionPanel() {
    int panelWidth = 300, panelHeight = 200;
    int panelX = (_WINDOW_WIDTH__ - panelWidth) / 2;
    int panelY = (_WINDOW_LONG__ - panelHeight) / 2;

    settextcolor(WHITE);
    settextstyle(24, 0, "Arial");
    outtextxy(panelX + 20, panelY + 20, "Volume Settings");

    RECT slider = { panelX + 50, panelY + 80, panelX + 250, panelY + 100 };
    setfillcolor(LIGHTGRAY);
    fillrectangle(slider.left, slider.top, slider.right, slider.bottom);

    int sliderPos = slider.left + (slider.right - slider.left) * volume / 100;
    setfillcolor(RED);
    fillrectangle(sliderPos - 5, slider.top - 5, sliderPos + 5, slider.bottom + 5);

    char volumeText[32];
    sprintf(volumeText, "Volume: %d%%", volume);
    outtextxy(slider.left, slider.bottom + 10, volumeText);

    if (MouseHit()) {
        MOUSEMSG msg = GetMouseMsg();
        if (msg.uMsg == WM_MOUSEMOVE && msg.mkLButton) {
            if (msg.x >= slider.left && msg.x <= slider.right) {
                volume = (msg.x - slider.left) * 100 / (slider.right - slider.left);
                setvol(volume);
            }
        }
    }
}
