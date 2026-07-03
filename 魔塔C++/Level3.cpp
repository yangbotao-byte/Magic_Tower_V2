#include "allinclude.h"

int level3() {
    init();
    beginLevelLoot();

    IMAGE rawBg, levelBg;
    loadimage(&rawBg, "./关卡3.png");
    scaleImage(&levelBg, &rawBg, _WINDOW_WIDTH__, _WINDOW_LONG__);

    int wave = 0;
    DWORD clearTime = 0;
    float jumpVelocity = 0.0f;
    bool jumping = false;

    empty1.reset(680, 0);
    empty1.loadAppearance("./各个关卡的敌人/第三关/b_01_03", 0.82f);
    empty1.y = 430 - empty1.height;
    empty1.configure(230, 10, 310);
    empty1.hurtBox = { empty1.x, empty1.y, empty1.x + empty1.width, empty1.y + empty1.height };

    while (true) {
        keydetect();
        UIdetect();
        BeginBatchDraw();
        putimage(0, 0, &levelBg);

        if (hero1.jumpkey && !jumping) {
            jumping = true;
            isJumping = true;
            jumpVelocity = -18.0f;
        }

        if (jumping) {
            hero1.y += (int)jumpVelocity;
            jumpVelocity += 1.2f;
            if (hero1.Lwalkkey) { hero1.x -= hero1.speed; hero1.attackL = true; hero1.attackR = false; }
            if (hero1.Rwalkkey) { hero1.x += hero1.speed; hero1.attackL = false; hero1.attackR = true; }
            hero1.x = max(0, min(_WINDOW_WIDTH__ - 100, hero1.x));

            bool airAttack = (GetAsyncKeyState('J') & 0x8000) != 0;
            if (airAttack && !hero1.atkAnimR.empty()) {
                int frame = (GetTickCount() / 55) % hero1.atkAnimR.size();
                vector<IMAGE>& frames = hero1.attackL ? hero1.atkAnimL : hero1.atkAnimR;
                putimagePNG2(hero1.x, hero1.y, &frames[frame % frames.size()]);
            }
            else if (!hero1.jumpAnimR.empty()) {
                int frame = (GetTickCount() / 80) % hero1.jumpAnimR.size();
                putimagePNG2(hero1.x, hero1.y, &hero1.jumpAnimR[frame % hero1.jumpAnimR.size()]);
            }
            else if (!hero1.idleAnimR.empty()) {
                putimagePNG2(hero1.x, hero1.y, &hero1.idleAnimR[0]);
            }

            if (hero1.y >= 180) {
                hero1.y = 180;
                jumping = false;
                isJumping = false;
                hero1.jumpkey = false;
                jumpVelocity = 0;
            }
        }
        else {
            isJumping = false;
            updatehero();
        }

        if (empty1.alive) {
            empty1.update(hero1);
            empty1.draw();
        }
        else if (wave == 2) {
            if (!isBossLootSpawned()) spawnLevelLoot(3);
            if (!hasUncollectedBossLoot()) { EndBatchDraw(); return 1; }
        }
        else if (clearTime == 0) {
            clearTime = GetTickCount();
        }
        else if (GetTickCount() - clearTime > 850) {
            ++wave;
            clearTime = 0;
            empty1.reset(wave == 1 ? 650 : 590, 0);
            if (wave == 1) {
                empty1.loadAppearance("./各个关卡的敌人/第三关/b_01_04", 0.78f);
                empty1.configure(320, 12, 285);
            }
            else {
                empty1.loadAppearance("./各个关卡的敌人/第三关/boss", 1.05f);
                empty1.configure(820, 18, 235);
            }
            empty1.y = 430 - empty1.height;
            empty1.hurtBox = { empty1.x, empty1.y, empty1.x + empty1.width, empty1.y + empty1.height };
        }

        if (hero1.hp <= 0) { EndBatchDraw(); return 2; }

        settextstyle(20, 0, "Microsoft YaHei");
        settextcolor(wave == 2 ? RGB(255, 90, 110) : RGB(232, 201, 125));
        char title[80];
        if (wave == 2) strcpy_s(title, "第三关 · 星魂终焉 · 最终魔王");
        else sprintf_s(title, "第三关 · 终焉之塔    守卫 %d / 2", wave + 1);
        outtextxy((_WINDOW_WIDTH__ - textwidth(title)) / 2, 22, title);

        updateAttackBox(hero1);
        updateAndDrawParticles();
        updateAndDrawLoot();
        updateUI();
        EndBatchDraw();
        Sleep(45);
    }
}
