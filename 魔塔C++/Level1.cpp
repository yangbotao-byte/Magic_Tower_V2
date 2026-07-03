#include"allinclude.h"
IMAGE bg;

// Jump physics
static float jumpSpeed = 0.0f;
const float gravity = 1.2f;
const float initialJumpSpeed = -18.0f;
static Animation jumpAnim = { 0, 6, 0.5f, 0.0f, true }; // 6 frames, 0.5s each

Enemy empty1(700,280);
vector<IMAGE> bin;

int level1(){
    init();
    IMAGE bgRaw;
    loadimage(&bgRaw, "./bg\\Background_8K.png");
    scaleImage(&bg, &bgRaw, _WINDOW_WIDTH__, _WINDOW_LONG__);

    // Load ice effect frames
    bin.resize(11);
    for (int i = 1; i <= 10; i++) {
        char path[50];
        sprintf(path, "./ice/%d.PNG", i);
        loadimage(&bin[i], path);
    }

    srand((unsigned int)time(0));

    // Design document: Bao1 drops at the beginning of level 1.
    int enemyWave = 0;
    DWORD waveClearTime = 0;
    empty1.reset(720, 280);
    empty1.configure(100, 6, 360);

    DWORD lastTime = GetTickCount();
    while (1) {
        // Delta time
        DWORD currentTime = GetTickCount();
        float deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;

        // Input
        keydetect();
        UIdetect();

        BeginBatchDraw();
        putimage(0, 0, &bg);  // Pre-scaled bg

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

            // Render: attack/skill during jump, or jump animation
            bool jumpAtk = (GetAsyncKeyState('J') & 0x8000) != 0;
            bool jumpSkill = (GetAsyncKeyState('L') & 0x8000) != 0;
            bool jumpQ = (GetAsyncKeyState('Q') & 0x8000) != 0;

            if (jumpAtk && !hero1.atkAnimR.empty()) {
                static int jaCnt = 0; jaCnt++;
                putimagePNG2(hero1.x, hero1.y, &hero1.atkAnimR[jaCnt % hero1.atkAnimR.size()]);
            } else if (jumpSkill && !hero1.skillAnimR.empty()) {
                static int jsCnt = 0; jsCnt++;
                putimagePNG2(hero1.x, hero1.y, &hero1.skillAnimR[jsCnt % hero1.skillAnimR.size()]);
            } else if (jumpQ && !hero1.qskillAnimR.empty()) {
                static int jqCnt = 0; jqCnt++;
                putimagePNG2(hero1.x, hero1.y, &hero1.qskillAnimR[jqCnt % hero1.qskillAnimR.size()]);
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
            // Normal ground state
            updatehero();
        }

        // Three sequential enemies. The next one appears only after the current enemy dies.
        if (empty1.alive) {
            empty1.update(hero1);
            empty1.draw();
        }
        else if (enemyWave == 2) {
            if (!isBossLootSpawned()) spawnBossLoot();
            if (!hasUncollectedBossLoot()) return 1;
        }
        else if (waveClearTime == 0) {
            waveClearTime = GetTickCount();
        }
        else if (GetTickCount() - waveClearTime >= 850) {
            ++enemyWave;
            waveClearTime = 0;
            if (enemyWave >= 3) return 1;
            if (enemyWave == 1) {
                empty1.reset(610, 0);
                empty1.loadAppearance("./各个关卡的敌人/第一关/e_m_002", 0.62f);
                empty1.y = 430 - empty1.height;
                empty1.configure(150, 8, 330);
            }
            else {
                empty1.reset(550, 0);
                empty1.loadAppearance("./各个关卡的敌人/第一关/BOSS", 0.55f);
                empty1.y = 430 - empty1.height;
                empty1.configure(260, 12, 280); // final boss
            }
            empty1.hurtBox = { empty1.x, empty1.y, empty1.x + empty1.width, empty1.y + empty1.height };
        }
        if (hero1.hp <= 0) return 2;

        // Wave indicator
        settextstyle(18, 0, "Microsoft YaHei");
        settextcolor(enemyWave == 2 ? RGB(255, 105, 90) : RGB(245, 210, 135));
        char waveText[80];
        if (enemyWave == 2) strcpy_s(waveText, "最终首领");
        else sprintf_s(waveText, "敌人  %d / 3", enemyWave + 1);
        outtextxy((_WINDOW_WIDTH__ - textwidth(waveText)) / 2, 22, waveText);
        updateAttackBox(hero1);
        updateAndDrawParticles();
        updateAndDrawLoot();
        updateUI();
        EndBatchDraw();
        Sleep(70);
    }
}
