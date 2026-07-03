#include"allinclude.h"

struct Heart {
    float x, y, vy, vx, size;
    COLORREF color;
    int life, maxLife;
};

// Draw a heart shape at (cx, cy) with given size
void drawHeart(int cx, int cy, int sz, COLORREF c) {
    // Smooth mathematical heart curve; avoids the diamond seam from circles + triangle.
    const int count = 40;
    POINT pts[count];
    for (int i=0;i<count;++i) {
        double t=2.0*3.1415926*i/count;
        double sx=16*pow(sin(t),3);
        double sy=13*cos(t)-5*cos(2*t)-2*cos(3*t)-cos(4*t);
        pts[i].x=cx+(int)(sx*sz/32.0);
        pts[i].y=cy-(int)(sy*sz/32.0);
    }
    setfillcolor(c); setlinecolor(RGB(min(255,GetRValue(c)+25),GetGValue(c),GetBValue(c)));
    fillpolygon(pts,count);
}
void showLoveScreen() {
    vector<Heart> hearts;

    // Initial heart burst
    for (int i = 0; i < 50; i++) {
        Heart h;
        h.x = (float)(rand() % _WINDOW_WIDTH__);
        h.y = (float)(rand() % _WINDOW_LONG__);
        h.vy = -(1.0f + rand() % 20 / 10.0f);
        h.vx = (rand() % 30 - 15) / 10.0f;
        h.size = 8.0f + rand() % 20;
        h.color = RGB(200 + rand() % 55, 20 + rand() % 40, 80 + rand() % 60);
        h.life = 200 + rand() % 200;
        h.maxLife = h.life;
        hearts.push_back(h);
    }

    float alpha = 0;
    float pulse = 0;
    DWORD startTime = GetTickCount();

    while (1) {
        DWORD now = GetTickCount();
        float dt = (now - startTime) / 1000.0f;
        startTime = now;

        // Fade in
        if (alpha < 255) alpha += 1.5f;
        pulse += 0.04f;

        // Spawn new hearts from bottom
        if (rand() % 100 < 30) {
            Heart h;
            h.x = (float)(rand() % _WINDOW_WIDTH__);
            h.y = (float)_WINDOW_LONG__ + 20;
            h.vy = -(1.5f + rand() % 25 / 10.0f);
            h.vx = (rand() % 40 - 20) / 10.0f;
            h.size = 6.0f + rand() % 16;
            h.color = RGB(200 + rand() % 55, 30 + rand() % 50, 80 + rand() % 70);
            h.life = 150 + rand() % 150;
            h.maxLife = h.life;
            hearts.push_back(h);
        }

        // Update hearts
        for (int i = 0; i < (int)hearts.size(); i++) {
            Heart& h = hearts[i];
            h.y += h.vy;
            h.x += h.vx;
            h.life--;
        }
        // Remove dead hearts
        for (int i = (int)hearts.size() - 1; i >= 0; i--) {
            if (hearts[i].life <= 0 || hearts[i].y < -60)
                hearts.erase(hearts.begin() + i);
        }

        // Check for skip (any key or click)
        if (_kbhit()) { _getch(); break; }
        if (MouseHit()) { MOUSEMSG m = GetMouseMsg(); if (m.uMsg == WM_LBUTTONDOWN) break; }

        // Auto-skip after ~8 seconds
        if (alpha >= 255 && hearts.size() < 5) break;

        cleardevice();
        BeginBatchDraw();

        // Deep pink background
        setbkcolor(RGB(30, 5, 20));

        // Draw hearts
        for (int i = 0; i < (int)hearts.size(); i++) {
            Heart& h = hearts[i];
            float prog = 1.0f - (float)h.life / h.maxLife;
            int a = (int)(min(255, (int)alpha) * (1.0f - prog * prog));
            if (a < 0) a = 0;
            int r = min(255, GetRValue(h.color) * a / 255);
            int g = min(255, GetGValue(h.color) * a / 255);
            int b = min(255, GetBValue(h.color) * a / 255);
            drawHeart((int)h.x, (int)h.y, (int)h.size, RGB(r, g, b));
        }

        // A private dedication presented as a letter, not a generic splash screen.
        int cardX = 150, cardY = 62, cardW = 660, cardH = 410;
        setfillcolor(RGB(38, 14, 28)); solidrectangle(cardX, cardY, cardX + cardW, cardY + cardH);
        setlinecolor(RGB(172, 104, 116)); setlinestyle(PS_SOLID, 2);
        rectangle(cardX, cardY, cardX + cardW, cardY + cardH);
        setlinestyle(PS_SOLID, 1);
        setlinecolor(RGB(100, 55, 70)); rectangle(cardX + 8, cardY + 8, cardX + cardW - 8, cardY + cardH - 8);
        setfillcolor(RGB(218, 146, 154));
        solidcircle(cardX + 13, cardY + 13, 4); solidcircle(cardX + cardW - 13, cardY + 13, 4);
        solidcircle(cardX + 13, cardY + cardH - 13, 4); solidcircle(cardX + cardW - 13, cardY + cardH - 13, 4);

        settextstyle(22, 0, "Microsoft YaHei"); settextcolor(RGB(232, 188, 192));
        const char* eyebrow = "写给我最珍贵的伴侣";
        outtextxy((_WINDOW_WIDTH__ - textwidth(eyebrow)) / 2, 88, eyebrow);
        setlinecolor(RGB(143, 82, 98)); line(265, 124, 695, 124);

        int glow = (int)(205 + 40 * sin(pulse));
        settextstyle(48, 0, "Microsoft YaHei"); settextcolor(RGB(255, glow, 210));
        const char* name = "小菁菁";
        outtextxy((_WINDOW_WIDTH__ - textwidth(name)) / 2, 143, name);

        settextstyle(19, 0, "Microsoft YaHei"); settextcolor(RGB(239, 207, 211));
        const char* msg1 = "这段旅程里的每一束光、每一次相遇，";
        const char* msg2 = "都藏着我想与你分享的认真与温柔。";
        const char* msg3 = "谢谢你陪在我身边，也谢谢你成为我的勇气。";
        outtextxy((_WINDOW_WIDTH__ - textwidth(msg1)) / 2, 235, msg1);
        outtextxy((_WINDOW_WIDTH__ - textwidth(msg2)) / 2, 270, msg2);
        outtextxy((_WINDOW_WIDTH__ - textwidth(msg3)) / 2, 315, msg3);

        settextstyle(17, 0, "Microsoft YaHei"); settextcolor(RGB(203, 151, 165));
        const char* sign = "—— 永远爱你的人  YBT";
        outtextxy((_WINDOW_WIDTH__ - textwidth(sign)) / 2, 365, sign);
        int hsz = 15 + (int)(4 * sin(pulse * 1.5));
        drawHeart(_WINDOW_WIDTH__ / 2, 414, hsz, RGB(245, 91, 132));

        settextstyle(14, 0, "Microsoft YaHei"); settextcolor(RGB(154, 117, 132));
        const char* hint = "按任意键或点击鼠标，开启我们的冒险";
        outtextxy((_WINDOW_WIDTH__ - textwidth(hint)) / 2, 488, hint);
        EndBatchDraw();
        Sleep(16);
    }
}
