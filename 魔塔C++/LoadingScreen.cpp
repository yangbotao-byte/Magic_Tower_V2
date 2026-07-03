#define _CRT_SECURE_NO_WARNINGS 1
#include"allinclude.h"

int loading() {
    IMAGE loading001;
    IMAGE ProgressBar;

    loadimage(&loading001, "./loading\\loading_001.png");
    loadimage(&ProgressBar, "./loading\\programbar.png");

    // Scale images to fit window
    IMAGE loadScaled, barScaled;
    int loadW = loading001.getwidth();
    int loadH = loading001.getheight();
    int barW = ProgressBar.getwidth();
    int barH = ProgressBar.getheight();

    int newLoadW = loadW * _WINDOW_WIDTH__ / 1300;
    int newLoadH = loadH * _WINDOW_LONG__ / 650;
    int newBarW = barW * _WINDOW_WIDTH__ / 1300;
    int newBarH = barH * _WINDOW_LONG__ / 650;

    scaleImage(&loadScaled, &loading001, newLoadW, newLoadH);
    scaleImage(&barScaled, &ProgressBar, newBarW, newBarH);

    // Progress bar dimensions (scaled)
    int pbWidth = 760;
    int pbHeight = 10;
    int pbX = (_WINDOW_WIDTH__ - pbWidth) / 2;
    int pbY = 440;

    // If hero has legacy path vectors, load them
    bool hasLegacyPaths = !hero1.standPaths.empty();
    int totalFrames = hasLegacyPaths ?
        (hero1.standPaths.size() + hero1.walkPaths.size()
       + hero1.attackPaths.size() + hero1.swalkPaths.size()) : 50;

    for (int i = 0; i < totalFrames; i++) {
        if (hasLegacyPaths) {
            if (i < (int)hero1.standPaths.size()) {
                loadimage(&hero1.stand1[i], hero1.standPaths[i].c_str());
            }
            else if (i < (int)(hero1.walkPaths.size() + hero1.standPaths.size())) {
                int walkIndex = i - hero1.standPaths.size();
                loadimage(&hero1.walk1[walkIndex], hero1.walkPaths[walkIndex].c_str());
                loadimage(&hero1.walk2[walkIndex], hero1.swalkPaths[walkIndex].c_str());
            }
            else if (i < (int)(hero1.attackPaths.size() + hero1.walkPaths.size() + hero1.standPaths.size())) {
                int attackIndex = i - hero1.standPaths.size() - hero1.walkPaths.size();
                loadimage(&hero1.attack1[attackIndex], hero1.attackPaths[attackIndex].c_str());
                loadimage(&hero1.attack2[attackIndex], hero1.sattackPaths[attackIndex].c_str());
            }
        }

        float progress = (float)(i + 1) / totalFrames;

        cleardevice();
        BeginBatchDraw();

        setbkcolor(RGB(30, 30, 50));

        // Loading splash image (scaled, centered)
        putimagePNG2((_WINDOW_WIDTH__ - newLoadW) / 2, 40, &loadScaled);

        // Progress bar background
        setfillcolor(RGB(70, 70, 90));
        fillrectangle(pbX, pbY, pbX + pbWidth, pbY + pbHeight);

        // Progress bar fill
        setfillcolor(RGB(100, 200, 255));
        fillrectangle(pbX, pbY, pbX + (int)(pbWidth * progress), pbY + pbHeight);

        // Progress bar border
        setlinecolor(WHITE);
        rectangle(pbX, pbY, pbX + pbWidth, pbY + pbHeight);

        // Progress bar overlay image (scaled)
        putimagePNG2((_WINDOW_WIDTH__ - newBarW) / 2, pbY - 50, &barScaled);

        // Loading percentage
        char text[64];
        sprintf(text, _T("Loading... %d%%"), (int)(progress * 100));
        settextcolor(WHITE);
        settextstyle(20, 0, _T("Arial"));
        int tw = textwidth(text);
        outtextxy((_WINDOW_WIDTH__ - tw) / 2, pbY - 50, text);

        // Hero name
        char heroText[64];
        sprintf(heroText, "Hero: %s", getHeroName(hero1.heroType));
        settextcolor(RGB(255, 215, 0));
        settextstyle(18, 0, _T("Arial"));
        int hw = textwidth(heroText);
        outtextxy((_WINDOW_WIDTH__ - hw) / 2, pbY + 30, heroText);

        settextstyle(20, 0, _T("SimHei"));
        const char* hint = "Preparing for adventure...";
        int hiw = textwidth(hint);
        outtextxy((_WINDOW_WIDTH__ - hiw) / 2, pbY + 55, hint);

        EndBatchDraw();
        FlushBatchDraw();
        Sleep(40);
    }

    const char* done = "Loading complete!";
    int dw = textwidth(done);
    outtextxy((_WINDOW_WIDTH__ - dw) / 2, pbY + 55, done);
    FlushBatchDraw();
    EndBatchDraw();
    cleardevice();

    return 1;
}
