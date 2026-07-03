#define _CRT_SECURE_NO_WARNINGS 1
#include"hero.h"
#include"allinclude.h"
#include<algorithm>
#include<windows.h>
#pragma comment(lib, "Msimg32.lib")

const HeroTypeStats HERO_STATS[HERO_COUNT] = {
    { 100, 25, 12, 10,  8, 20 },  // Swordsman: 20% crit
    { 120, 18, 15, 20,  7, 10 },  // Sword-Shield: 10% crit
};

const char* getHeroName(int type) {
    if (type == HERO_SWORDSMAN)     return "剑士";
    if (type == HERO_SWORD_SHIELD)  return "守护骑士";
    return "未知";
}

const char* getHeroFolder(int type) {
    if (type == HERO_SWORDSMAN)     return "swordsman";
    if (type == HERO_SWORD_SHIELD)  return "sword_shield";
    return "swordsman";
}

// Load all frame_*.png files in a directory, sorted by frame number
int loadAnimFrames(vector<IMAGE>& out, const char* folder) {
    out.clear();
    // Scan for frame_N.png files, collect all frame numbers
    vector<int> nums;
    for (int i = 0; i < 300; i++) {
        char path[256];
        sprintf(path, "%s/frame_%d.png", folder, i);
        FILE* f = fopen(path, "rb");
        if (f) { fclose(f); nums.push_back(i); }
    }
    if (nums.empty()) return 0;
    sort(nums.begin(), nums.end());
    out.resize(nums.size());
    for (int i = 0; i < (int)nums.size(); i++) {
        char path[256];
        sprintf(path, "%s/frame_%d.png", folder, nums[i]);
        loadimage(&out[i], path);
    }
    return nums.size();
}


hero::hero() {
    x = 0; y = 0;
    hp = 100; maxHp = 100;
    speed = 5; hurt = 20; magicAttack = 9; mana = maxMana = 100;
    defense = 10; critChance = 15; levelsCleared = 0;
    attackL = false; attackR = true;
    hasDealtDamage = false;
    isHit = false; isSkill = false; isQSkill = false; isDead = false;
    hitCounter = 0; skillCounter = 0; qskillCounter = 0; deathCounter = 0;
    sleep = false; sleeptime = 0;
    heroType = HERO_SWORDSMAN;
}

hero::hero(int x1, int y1, HeroType type) {
    x = x1; y = y1;
    heroType = type;
    const HeroTypeStats& s = HERO_STATS[type];
    hp = s.hp; maxHp = s.hp;
    hurt = s.attack; magicAttack = s.magic; mana = maxMana = 100;
    defense = s.defense; speed = s.speed;
    critChance = s.critChance; levelsCleared = 0;

    attackL = false; attackR = true;
    hasDealtDamage = false;
    isHit = false; isSkill = false; isQSkill = false; isDead = false;
    hitCounter = 0; skillCounter = 0; qskillCounter = 0; deathCounter = 0;
    sleep = false; sleeptime = 0;
    loadHeroSprites(type);
}

hero::hero(int x1, int y1, int nStand, int nWalk, int nAttack) {
    // Legacy constructor
    x = x1; y = y1; heroType = HERO_SWORDSMAN;
    const HeroTypeStats& s = HERO_STATS[HERO_SWORDSMAN];
    hp = s.hp; maxHp = s.hp;
    hurt = s.attack; magicAttack = s.magic; mana = maxMana = 100;
    defense = s.defense; speed = s.speed;
    critChance = s.critChance; levelsCleared = 0;
    attackL = false; attackR = true;
    hasDealtDamage = false;
    isHit = isSkill = isQSkill = isDead = false;
    hitCounter = skillCounter = qskillCounter = deathCounter = 0;
    sleep = false; sleeptime = 0;

    stand1.resize(nStand); attack1.resize(nAttack);
    walk1.resize(nWalk); walk2.resize(nWalk); attack2.resize(nAttack);
    jump.resize(4);
    for (int i = 0; i < nStand; i++) {
        char p[50]; sprintf(p, "./hero/stand/frame_%d.png", i); standPaths.push_back(p);
    }
    for (int i = 0; i < nWalk; i++) {
        char p[50]; sprintf(p, "./hero/walk/frame_%d.png", i); walkPaths.push_back(p);
    }
    for (int i = 0; i < nAttack; i++) {
        char p[50]; sprintf(p, "./hero/attack/frame_%d.png", i); attackPaths.push_back(p);
    }
    for (int i = 0; i < nWalk; i++) {
        char p[50]; sprintf(p, "./hero/Rwalk/frame_%d.png", i); swalkPaths.push_back(p);
    }
    for (int i = 0; i < nAttack; i++) {
        char p[50]; sprintf(p, "./hero/Lattack/frame_%d.png", i); sattackPaths.push_back(p);
    }
    for (int i = 0; i < 4; i++) {
        char p[50]; sprintf(p, "./hero/jump/frame_0%d.png", i); loadimage(&jump[i], p);
    }
}

void hero::loadHeroSprites(HeroType type) {
    const char* f = getHeroFolder(type);
    char dir[256];

    // Load all animations
    sprintf(dir, "./hero_select/%s/idle", f);    loadAnimFrames(idleAnimR, dir);
    sprintf(dir, "./hero_select/%s/walk", f);    loadAnimFrames(walkAnimR, dir);
    sprintf(dir, "./hero_select/%s/attack", f);  loadAnimFrames(atkAnimR, dir);
    sprintf(dir, "./hero_select/%s/jump", f);    loadAnimFrames(jumpAnimR, dir);
    sprintf(dir, "./hero_select/%s/death", f);   loadAnimFrames(deathAnimR, dir);
    sprintf(dir, "./hero_select/%s/skill", f);   loadAnimFrames(skillAnimR, dir);
    sprintf(dir, "./hero_select/%s/qskill", f);  loadAnimFrames(qskillAnimR, dir);

    // Create left-facing (flipped) versions
    auto flipVec = [](vector<IMAGE>& dst, vector<IMAGE>& src) {
        dst.resize(src.size());
        for (int i = 0; i < (int)src.size(); i++) flipImageHorizontal(&dst[i], &src[i]);
    };
    flipVec(idleAnimL, idleAnimR);
    flipVec(walkAnimL, walkAnimR);
    flipVec(atkAnimL, atkAnimR);
    flipVec(deathAnimL, deathAnimR);
    flipVec(skillAnimL, skillAnimR);
    flipVec(qskillAnimL, qskillAnimR);

    // Scale all sprites to 65%
    auto scaleVec = [](vector<IMAGE>& v, float s) {
        for (int i = 0; i < (int)v.size(); i++) {
            IMAGE tmp = v[i];
            scaleImage(&v[i], &tmp, (int)(tmp.getwidth() * s), (int)(tmp.getheight() * s));
        }
    };
    float sc = 0.70f;
    scaleVec(idleAnimR, sc); scaleVec(idleAnimL, sc);
    scaleVec(walkAnimR, sc); scaleVec(walkAnimL, sc);
    scaleVec(atkAnimR, sc);  scaleVec(atkAnimL, sc);
    scaleVec(jumpAnimR, sc);
    scaleVec(deathAnimR, sc); scaleVec(deathAnimL, sc);
    scaleVec(skillAnimR, sc); scaleVec(skillAnimL, sc);
    scaleVec(qskillAnimR, sc); scaleVec(qskillAnimL, sc);

    // Sync legacy vectors for backward compat
    stand1 = idleAnimR;
    standLeft = idleAnimL;
    walk1 = walkAnimR;
    walk2 = walkAnimL;
    attack1 = atkAnimR;
    attack2 = atkAnimL;
    hitR = deathAnimR;
    hitL = deathAnimL;
    missR = skillAnimR;
    missL = skillAnimL;
    jump = jumpAnimR;
}

// ---- Render Methods ----

void hero::updatestand(int count) {
    if (idleAnimR.empty()) return;
    int idx = count % idleAnimR.size();
    if (attackR && !attackL)      putimagePNG2(x, y, &idleAnimR[idx]);
    else if (attackL && !attackR) putimagePNG2(x, y, &idleAnimL[idx]);
    else                          putimagePNG2(x, y, &idleAnimR[idx]);
}

void hero::updatewalk(int count) {
    if (walkAnimR.empty()) { updatestand(count); return; }
    if (Rwalkkey) {
        x += speed;
        putimagePNG2(x, y, &walkAnimR[count % walkAnimR.size()]);
    }
    else if (Lwalkkey) {
        x -= speed;
        putimagePNG2(x, y, &walkAnimL[count % walkAnimL.size()]);
    }
    // Screen bounds
    if (x < 0) x = 0;
    if (x > _WINDOW_WIDTH__ - 100) x = _WINDOW_WIDTH__ - 100;
}

void hero::updateattack(int count) {
    if (atkAnimR.empty()) return;
    int idx = count % atkAnimR.size();
    if (attackkey && attackR)        putimagePNG2(x, y, &atkAnimR[idx]);
    else if (attackkey && attackL)   putimagePNG2(x, y, &atkAnimL[idx]);
    else if (attackkey)              putimagePNG2(x, y, &atkAnimR[idx]);
}

void hero::updateHit() {
    if (deathAnimR.empty()) return;
    hitCounter++;
    int idx = hitCounter % deathAnimR.size();
    if (attackR || (!attackL && !attackR)) putimagePNG2(x, y, &deathAnimR[idx]);
    else                                    putimagePNG2(x, y, &deathAnimL[idx]);
}

void hero::updateSkill() {
    if (skillAnimR.empty()) return;
    skillCounter++;
    int idx = skillCounter % skillAnimR.size();
    if (attackR || (!attackL && !attackR)) putimagePNG2(x, y, &skillAnimR[idx]);
    else                                    putimagePNG2(x, y, &skillAnimL[idx]);
}

void hero::updateQSkill() {
    if (qskillAnimR.empty()) return;
    qskillCounter++;
    int idx = qskillCounter % qskillAnimR.size();
    if (attackR || (!attackL && !attackR)) putimagePNG2(x, y, &qskillAnimR[idx]);
    else                                    putimagePNG2(x, y, &qskillAnimL[idx]);
}

void hero::updateDeath() {
    if (deathAnimR.empty()) return;
    deathCounter++;
    int idx = deathCounter % deathAnimR.size();
    putimagePNG2(x, y, &deathAnimR[idx]);
}

void hero::beHit(int attackPower) {
    hp -= attackPower;
    isHit = true;
    hitCounter = 0;
    if (hp < 0) hp = 0;
}

void hero::boolhero() {
    int barWidth = 240, barHeight = 16;
    int barX = 120, barY = 45;
    int healthWidth = hp * barWidth / maxHp;

    setfillcolor(RGB(20, 20, 30));
    fillrectangle(barX + 2, barY + 2, barX + barWidth + 2, barY + barHeight + 2);
    setfillcolor(RGB(30, 30, 40));
    fillrectangle(barX, barY, barX + barWidth, barY + barHeight);

    COLORREF hpColor;
    float hpRatio = (float)hp / maxHp;
    if (hpRatio > 0.6f)      hpColor = RGB(40, 200, 60);
    else if (hpRatio > 0.3f) hpColor = RGB(255, 180, 0);
    else                     hpColor = RGB(230, 30, 30);

    setfillcolor(hpColor);
    fillrectangle(barX, barY, barX + healthWidth, barY + barHeight);
    if (healthWidth > 10) {
        setfillcolor(RGB(min(255, GetRValue(hpColor) + 60),
            min(255, GetGValue(hpColor) + 60), min(255, GetBValue(hpColor) + 60)));
        fillrectangle(barX + 2, barY + 2, barX + healthWidth - 2, barY + 6);
    }
    setlinecolor(RGB(180, 180, 200));
    setlinestyle(PS_SOLID, 2);
    rectangle(barX, barY, barX + barWidth, barY + barHeight);
    setlinestyle(PS_SOLID, 1);

    char hpText[48];
    sprintf(hpText, "%s  HP: %d / %d", getHeroName(heroType), hp, maxHp);
    settextcolor(WHITE);
    settextstyle(11, 0, _T("Arial"));
    int tw = textwidth(hpText);
    outtextxy(barX + (barWidth - tw) / 2, barY + 2, hpText);
}

void hero::freeze() { sleep = true; sleeptime = 30; }
