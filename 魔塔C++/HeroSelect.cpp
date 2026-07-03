#include"allinclude.h"

struct HeroCard { int x, y, w, h; HeroType type; bool hovered; };

const char* getStoryLine(int type, int ln) {
    if (type == HERO_SWORDSMAN) {
        if (ln == 0) return "A swift blade master.";
        if (ln == 1) return "Trained since childhood,";
        if (ln == 2) return "strikes with deadly precision.";
    }
    if (type == HERO_SWORD_SHIELD) {
        if (ln == 0) return "Sword and shield as one.";
        if (ln == 1) return "Defended allies through";
        if (ln == 2) return "countless battles.";
    }
    return NULL;
}

void drawHeroCard(HeroCard& card, IMAGE* p, int fw, int fh) {
    setfillcolor(RGB(22, 22, 40));
    fillrectangle(card.x, card.y, card.x + card.w, card.y + card.h);
    if (card.hovered) {
        for (int i = 3; i >= 1; i--) {
            setlinecolor(RGB(255, 215, 40 + i * 15)); setlinestyle(PS_SOLID, i * 2);
            rectangle(card.x - i, card.y - i, card.x + card.w + i, card.y + card.h + i);
        }
        setlinecolor(RGB(255, 215, 0)); setlinestyle(PS_SOLID, 2);
        rectangle(card.x - 1, card.y - 1, card.x + card.w + 1, card.y + card.h + 1);
        setlinestyle(PS_SOLID, 1);
    } else {
        setlinecolor(RGB(60, 60, 80));
        rectangle(card.x, card.y, card.x + card.w, card.y + card.h);
    }

    int ty = card.y + 14;

    // Name at top (bigger font)
    const char* name = getHeroName(card.type);
    settextcolor(card.hovered ? RGB(255, 255, 200) : RGB(255, 220, 100));
    settextstyle(26, 0, _T("Arial")); int nw = textwidth(name);
    outtextxy(card.x + (card.w - nw) / 2, ty, name); ty += 34;

    // Preview image
    if (p && fw > 0) { putimagePNG2(card.x + (card.w - fw) / 2, ty, p); }
    ty += 125;

    // Divider
    setlinecolor(RGB(80, 80, 100)); line(card.x + 20, ty, card.x + card.w - 20, ty); ty += 8;

    // Stats (bigger font)
    const HeroTypeStats& s = HERO_STATS[card.type];
    settextstyle(17, 0, _T("Arial"));
    int sx = card.x + 22, col2 = card.x + 155;
    char t[32];
    settextcolor(RGB(255, 140, 140));
    sprintf(t, "HP: %d", s.hp);      outtextxy(sx, ty, t);
    settextcolor(RGB(255, 200, 100));
    sprintf(t, "ATK: %d", s.attack); outtextxy(sx, ty + 30, t);
    settextcolor(RGB(100, 200, 255));
    sprintf(t, "DEF: %d", s.defense); outtextxy(col2, ty, t);
    settextcolor(RGB(140, 255, 140));
    sprintf(t, "SPD: %d", s.speed);   outtextxy(col2, ty + 30, t);
    ty += 66;

    // Divider 2
    setlinecolor(RGB(80, 80, 100)); line(card.x + 20, ty, card.x + card.w - 20, ty); ty += 8;

    // Story (bigger font)
    settextcolor(RGB(170, 170, 200)); settextstyle(15, 0, _T("Arial"));
    for (int ln = 0; ln < 3; ln++) {
        const char* sl = getStoryLine(card.type, ln);
        if (sl) { outtextxy(card.x + 14, ty, sl); ty += 22; }
    }
}

int selectHero() {
    IMAGE pre[2]; int fw[2] = {0}, fh[2] = {0};
    const char* fld[] = {"swordsman", "sword_shield"};
    for (int i = 0; i < 2; i++) {
        char p[128]; sprintf(p, "./hero_select/%s/idle/frame_0.png", fld[i]);
        loadimage(&pre[i], p); fw[i] = pre[i].getwidth(); fh[i] = pre[i].getheight();
    }
    const int cW = 300, cH = 370, gX = 50;
    int sX = (_WINDOW_WIDTH__ - (cW * 2 + gX)) / 2, sY = 55;
    HeroCard cards[2] = {
        {sX, sY, cW, cH, HERO_SWORDSMAN, false},
        {sX + cW + gX, sY, cW, cH, HERO_SWORD_SHIELD, false},
    };
    int sel = -1; float tg = 0.0f;
    while (1) {
        int mx = -1, my = -1; bool clk = false;
        while (MouseHit()) { MOUSEMSG m = GetMouseMsg(); mx = m.x; my = m.y; if (m.uMsg == WM_LBUTTONDOWN) clk = true; }
        for (int i = 0; i < 2; i++)
            cards[i].hovered = (mx >= cards[i].x && mx <= cards[i].x + cards[i].w &&
                                my >= cards[i].y && my <= cards[i].y + cards[i].h);
        const int bX = 15, bY = 15, bW = 130, bH = 35;
        bool bHov = (mx >= bX && mx <= bX + bW && my >= bY && my <= bY + bH);
        if (clk) { for (int i = 0; i < 2; i++) if (cards[i].hovered) sel = i; if (sel >= 0 && bHov) return sel; }
        if (_kbhit()) { char c = _getch(); if (c == 27) return 0; if (c == 13 && sel >= 0) return sel; if (c == '1') sel = 0; if (c == '2') sel = 1; }
        tg += 0.03f; cleardevice(); BeginBatchDraw();
        setbkcolor(RGB(12, 12, 28));
        for (int i = 0; i < 40; i++) { setfillcolor(RGB(35,35,55)); solidcircle((i*173+55)%_WINDOW_WIDTH__,(i*97+35)%_WINDOW_LONG__,1+i%2); }
        int al = 180 + (int)(75 * sin(tg));
        settextcolor(RGB(255,215,al)); settextstyle(28,0,_T("Arial"));
        const char* ti = "--- Choose Your Hero ---";
        outtextxy((_WINDOW_WIDTH__-textwidth(ti))/2, 8, ti);
        settextcolor(RGB(150,150,160)); settextstyle(12,0,_T("Arial"));
        const char* su = "Hover to preview | Click to select | 1-2 quick pick | Enter confirm";
        outtextxy((_WINDOW_WIDTH__-textwidth(su))/2, 40, su);
        for (int i = 0; i < 2; i++) {
            bool wh = cards[i].hovered; if (i == sel) cards[i].hovered = true;
            drawHeroCard(cards[i], &pre[i], fw[i], fh[i]); cards[i].hovered = wh;
            if (i == sel) {
                setlinecolor(RGB(0,255,100)); setlinestyle(PS_SOLID,3);
                rectangle(cards[i].x-2,cards[i].y-2,cards[i].x+cW+2,cards[i].y+cH+2); setlinestyle(PS_SOLID,1);
                setfillcolor(RGB(0,220,80)); solidcircle(cards[i].x+cW-12,cards[i].y+12,8);
                settextcolor(WHITE); settextstyle(11,0,_T("Arial")); outtextxy(cards[i].x+cW-18,cards[i].y+5,"V");
            }
        }
        if (sel >= 0) {
            setfillcolor(RGB(0,0,0)); fillrectangle(bX+2,bY+2,bX+bW+2,bY+bH+2);
            setfillcolor(bHov?RGB(60,190,60):RGB(35,130,35)); fillrectangle(bX,bY,bX+bW,bY+bH);
            setfillcolor(bHov?RGB(100,230,100):RGB(60,160,60)); fillrectangle(bX+1,bY+1,bX+bW-1,bY+bH/2);
            setlinecolor(RGB(180,255,180)); setlinestyle(PS_SOLID,2); rectangle(bX,bY,bX+bW,bY+bH); setlinestyle(PS_SOLID,1);
            settextcolor(WHITE); settextstyle(16,0,_T("Arial")); const char* bt = "Start Game!";
            outtextxy(bX+(bW-textwidth(bt))/2, bY+(bH-textheight(bt))/2, bt);
        }
        settextcolor(RGB(120,120,140)); settextstyle(11,0,_T("Arial"));
        const char* hi = "ESC = Back to Menu";
        outtextxy((_WINDOW_WIDTH__-textwidth(hi))/2, _WINDOW_LONG__-18, hi);
        EndBatchDraw(); Sleep(16);
    }
    return 0;
}
