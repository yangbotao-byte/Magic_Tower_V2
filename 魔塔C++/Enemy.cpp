#define _CRT_SECURE_NO_WARNINGS 1
#include "Enemy.h"
#include"allinclude.h"

static vector<IMAGE> gIceEffectFrames;
static int gIceEffectTimer = 0;
static int gIceEffectX = 0;
static int gIceEffectY = 0;

static void ensureIceEffectLoaded() {
    if (!gIceEffectFrames.empty()) return;
    gIceEffectFrames.resize(10);
    for (int i = 0; i < 10; ++i) {
        char path[MAX_PATH];
        sprintf_s(path, "./各个关卡的敌人/第一关/e_m_001/ice/%d.PNG", i + 1);
        IMAGE raw;
        loadimage(&raw, path);
        if (raw.getwidth() > 0 && raw.getheight() > 0) {
            int h = 150;
            int w = max(80, raw.getwidth() * h / max(1, raw.getheight()));
            scaleImage(&gIceEffectFrames[i], &raw, w, h);
        }
    }
}

// Helper function: flip IMAGE horizontally by swapping pixels
void flipImageHorizontal(IMAGE* dst, IMAGE* src) {
    int w = src->getwidth();
    int h = src->getheight();
    if (w <= 0 || h <= 0) return;

    // Resize destination to match source
    Resize(dst, w, h);

    DWORD* srcBuf = GetImageBuffer(src);
    DWORD* dstBuf = GetImageBuffer(dst);

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            dstBuf[y * w + x] = srcBuf[y * w + (w - 1 - x)];
        }
    }
}

Enemy::Enemy(int _x, int _y) {
    int frameCount = 8;
    walkFrames.resize(frameCount);
    attackFrames.resize(frameCount);
    walkFramesL.resize(frameCount);
    attackFramesL.resize(frameCount);

    // Load right-facing sprites
    for (int i = 0; i < frameCount; i++) {
        char path[MAX_PATH];
        sprintf_s(path, "./各个关卡的敌人/第一关/e_m_001/run_2/action/%04d.png", i);
        loadimage(&walkFrames[i], path);

        sprintf_s(path, "./各个关卡的敌人/第一关/e_m_001/attack_2/action/%04d.png", i);
        loadimage(&attackFrames[i], path);
    }

    // Create left-facing sprites by flipping
    for (int i = 0; i < frameCount; i++) {
        flipImageHorizontal(&walkFramesL[i], &walkFrames[i]);
        flipImageHorizontal(&attackFramesL[i], &attackFrames[i]);
    }

    // Scale all enemy frames to 70% to match hero
    float esc = 0.70f;
    for (int i = 0; i < frameCount; i++) {
        IMAGE t;
        t = walkFrames[i]; scaleImage(&walkFrames[i], &t, (int)(t.getwidth()*esc), (int)(t.getheight()*esc));
        t = walkFramesL[i]; scaleImage(&walkFramesL[i], &t, (int)(t.getwidth()*esc), (int)(t.getheight()*esc));
        t = attackFrames[i]; scaleImage(&attackFrames[i], &t, (int)(t.getwidth()*esc), (int)(t.getheight()*esc));
        t = attackFramesL[i]; scaleImage(&attackFramesL[i], &t, (int)(t.getwidth()*esc), (int)(t.getheight()*esc));
    }

    x = _x;
    y = _y;
    width = walkFrames[1].getwidth();
    height = walkFrames[1].getheight();
    hp = 100;
    maxHp = 100;
    alive = true;
    currentFrame = 0;
    facingLeft = false;

    attackPower = 10;
    attackCooldown = 60;
    attackTimer = 0;
    isAttacking = false;

    skillCooldown = 300;
    skillTimer = 0;

    freezeDuration = 0;
    iceSkillActive = false;

    hurtBox = { x, y, x + width, y + height };
}

void Enemy::update(hero& player) {
    if (!alive) return;

    // Hero center based on actual sprite size
    int ph = player.idleAnimR.empty() ? 100 : player.idleAnimR[0].getheight();
    int pw = player.idleAnimR.empty() ? 100 : player.idleAnimR[0].getwidth();
    int playerX = player.x + pw / 2;
    int playerY = player.y + ph / 2;

    int centerX = x + width / 2;
    int dx = playerX - centerX;
    float dist = (float)sqrt((double)(dx * dx));

    facingLeft = (dx < 0);

    if (dist > 5 && freezeDuration == 0) {
        x += (int)(dx / dist * 2.5);
    }

    if (dist < 60) {
        if (attackTimer == 0) {
            isAttacking = true;
            attackTimer = attackCooldown;
            player.beHit(attackPower);
            addDamageText(playerX, playerY - 20, attackPower, false);
        }
    }
    else {
        isAttacking = false;
    }

    if (attackTimer > 0) attackTimer--;

    if (skillTimer <= 0) {
        if (rand() % 2 == 0)
            useFireSkill(player);
        else
            useIceSkill(player);

        skillTimer = skillCooldown;
    }
    else {
        skillTimer--;
    }

    if (freezeDuration > 0) freezeDuration--;

    hurtBox = { x, y, x + width, y + height };
}

void Enemy::draw() {
    if (!alive || walkFrames.empty() || attackFrames.empty()) return;
    vector<IMAGE>& frames = isAttacking
        ? (facingLeft ? attackFramesL : attackFrames)
        : (facingLeft ? walkFramesL : walkFrames);
    if (!frames.empty()) {
        currentFrame %= (int)frames.size();
        putimagePNG2(x, y, _WINDOW_WIDTH__, &frames[currentFrame]);
        currentFrame = (currentFrame + 1) % (int)frames.size();
    }

if (gIceEffectTimer > 0 && !gIceEffectFrames.empty()) {
        int elapsed = 36 - gIceEffectTimer;
        int frame = min(9, elapsed * 10 / 36);
        IMAGE& ice = gIceEffectFrames[frame];
        if (ice.getwidth() > 0) putimagePNG2(gIceEffectX - ice.getwidth() / 2, gIceEffectY - ice.getheight() + 35, &ice);
        --gIceEffectTimer;
    }

    int barWidth = max(90, width);
    int healthWidth = maxHp > 0 ? hp * barWidth / maxHp : 0;
    int barX = x, barY = y - 18;
    setfillcolor(RGB(22, 22, 30)); solidrectangle(barX, barY, barX + barWidth, barY + 10);
    COLORREF hpColor = hp > maxHp * 0.6f ? RGB(60, 205, 85)
        : (hp > maxHp * 0.3f ? RGB(245, 176, 45) : RGB(225, 55, 58));
    setfillcolor(hpColor); solidrectangle(barX + 2, barY + 2, barX + max(2, healthWidth - 2), barY + 8);
    setlinecolor(RGB(220, 195, 135)); rectangle(barX, barY, barX + barWidth, barY + 10);
    char hpText[24]; sprintf_s(hpText, "%d / %d", hp, maxHp);
    settextstyle(11, 0, "Arial"); settextcolor(WHITE);
    outtextxy(barX + barWidth + 7, barY - 2, hpText);
}
void Enemy::beHit(int damage) {
    hp -= damage;
    if (hp <= 0) {
        hp = 0;
        alive = false;
    }
}

bool Enemy::isHitBy(const RECT& atkBox) {
    if (!alive) return false;
    RECT intersect;
    return IntersectRect(&intersect, &hurtBox, &atkBox);
}

void Enemy::reset(int _x, int _y) {
    x = _x;
    y = _y;
    hp = maxHp;
    alive = true;
    currentFrame = 0;
    isAttacking = false;
    facingLeft = false;
    attackTimer = 0;
    skillTimer = 0;
    freezeDuration = 0;
    iceSkillActive = false;
    hurtBox = { x, y, x + width, y + height };
}

void Enemy::loadAppearance(const char* folder, float scale) {
    const int frameCount = 8;
    walkFrames.clear(); attackFrames.clear();
    walkFramesL.clear(); attackFramesL.clear();
    walkFrames.resize(frameCount); attackFrames.resize(frameCount);
    walkFramesL.resize(frameCount); attackFramesL.resize(frameCount);

    for (int i = 0; i < frameCount; ++i) {
        char walkPath[MAX_PATH], attackPath[MAX_PATH];
        sprintf_s(walkPath, "%s/run_2/action/%04d.png", folder, i);
        sprintf_s(attackPath, "%s/attack_2/action/%04d.png", folder, i);

        // Load into independent source images. Never resize an IMAGE shallow copy:
        // EasyX may share its pixel buffer and corrupt most of the sprite.
        IMAGE rawWalk, rawAttack;
        loadimage(&rawWalk, walkPath);
        loadimage(&rawAttack, attackPath);
        if (rawWalk.getwidth() <= 0 || rawWalk.getheight() <= 0 ||
            rawAttack.getwidth() <= 0 || rawAttack.getheight() <= 0) {
            char message[MAX_PATH * 2];
            sprintf_s(message, "怪物图片加载失败：\n%s\n%s", walkPath, attackPath);
            MessageBoxA(GetHWnd(), message, "资源错误", MB_OK | MB_ICONERROR);
            alive = false;
            return;
        }

        int walkW = max(1, (int)(rawWalk.getwidth() * scale));
        int walkH = max(1, (int)(rawWalk.getheight() * scale));
        int attackW = max(1, (int)(rawAttack.getwidth() * scale));
        int attackH = max(1, (int)(rawAttack.getheight() * scale));
        scaleImage(&walkFrames[i], &rawWalk, walkW, walkH);
        scaleImage(&attackFrames[i], &rawAttack, attackW, attackH);
        flipImageHorizontal(&walkFramesL[i], &walkFrames[i]);
        flipImageHorizontal(&attackFramesL[i], &attackFrames[i]);
    }

    width = walkFrames[0].getwidth();
    height = walkFrames[0].getheight();
    currentFrame = 0;
    isAttacking = false;
    alive = true;
    hurtBox = { x, y, x + width, y + height };
}
void Enemy::loadCustomAppearance(const char* walkFolder, const char* walkPrefix, int walkCount,
    const char* attackFolder, const char* attackPrefix, int attackCount, float scale) {
    walkFrames.clear(); walkFramesL.clear(); attackFrames.clear(); attackFramesL.clear();
    walkFrames.resize(walkCount); walkFramesL.resize(walkCount);
    attackFrames.resize(attackCount); attackFramesL.resize(attackCount);
    for (int i = 0; i < walkCount; ++i) {
        char path[MAX_PATH]; sprintf_s(path, "%s/%s%d.png", walkFolder, walkPrefix, i + 1);
        IMAGE raw; loadimage(&raw, path);
        scaleImage(&walkFramesL[i], &raw, (int)(raw.getwidth()*scale), (int)(raw.getheight()*scale));
        flipImageHorizontal(&walkFrames[i], &walkFramesL[i]);
    }
    for (int i = 0; i < attackCount; ++i) {
        char path[MAX_PATH]; sprintf_s(path, "%s/%s%d.png", attackFolder, attackPrefix, i + 1);
        IMAGE raw; loadimage(&raw, path);
        scaleImage(&attackFramesL[i], &raw, (int)(raw.getwidth()*scale), (int)(raw.getheight()*scale));
        flipImageHorizontal(&attackFrames[i], &attackFramesL[i]);
    }
    width=walkFrames[0].getwidth(); height=walkFrames[0].getheight(); currentFrame=0; alive=true;
    hurtBox={x,y,x+width,y+height};
}
void Enemy::configure(int health, int damage, int cooldown) {
    maxHp = health; hp = health;
    attackPower = damage;
    skillCooldown = cooldown;
    skillTimer = cooldown;
}

void Enemy::useFireSkill(hero& player) {
    int ph = player.idleAnimR.empty() ? 100 : player.idleAnimR[0].getheight();
    int pw = player.idleAnimR.empty() ? 100 : player.idleAnimR[0].getwidth();
    int cx = player.x + pw / 2;
    int cy = player.y + ph / 2;
    for (int i = 0; i < 40; i++) {
        addFireParticle(cx + rand() % 40 - 20, cy + rand() % 30 - 15);
    }
    player.beHit(15);
    addDamageText(cx, cy - 20, 15, true);
}

void Enemy::useIceSkill(hero& player) {
    int ph = player.idleAnimR.empty() ? 100 : player.idleAnimR[0].getheight();
    int pw = player.idleAnimR.empty() ? 100 : player.idleAnimR[0].getwidth();
    int cx = player.x + pw / 2;
    int cy = player.y + ph / 2;
    for (int i = 0; i < 30; i++) {
        addIceParticle(cx + rand() % 50 - 25, cy + rand() % 40 - 20);
    }
    ensureIceEffectLoaded();
    gIceEffectX = cx;
    gIceEffectY = player.y + ph;
    gIceEffectTimer = 36;
    player.freeze();
    player.sleeptime = 36;
}
