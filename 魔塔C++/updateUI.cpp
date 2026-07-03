#include "allinclude.h"

namespace {
const int TREASURE_COUNT = 8;
const DWORD TREASURE_COOLDOWN = 20000;
IMAGE treasureIcons[TREASURE_COUNT];
vector<IMAGE> treasureAnimations[TREASURE_COUNT];
IMAGE portalFrames[12];
bool portalLoaded = false;
bool treasureOwned[TREASURE_COUNT] = { false };
bool treasureLoaded = false;
int equippedTreasure = -1;
int selectedTreasure = 0;
int appliedAttackBonus = 0;
DWORD lastTreasureUse = 0;
bool eWasDown = false;
bool bagWasDown = false;
const int LOOT_COUNT = 8;
IMAGE lootIcons[LOOT_COUNT];
bool lootOwned[LOOT_COUNT] = { false };
bool lootActive[LOOT_COUNT] = { false };
bool bossLootWasSpawned = false;
int equippedLoot[8] = { -1,-1,-1,-1,-1,-1,-1,-1 };
const int lootTargetSlot[LOOT_COUNT] = { 5, 1, 4, 2, 6, 7, 0, 3 };
const char* lootNames[LOOT_COUNT] = { "秘银手饰", "魔纹护肩", "生命戒指", "暗影盔甲", "疾风战靴", "深渊魔剑", "星魂护符", "守护饰品" };
const char* lootFiles[LOOT_COUNT] = { "第一关/手饰1.png", "第一关/护肩1.png", "第一关/戒指1.png", "第一关/盔甲1.png", "第一关/靴子1.png", "第二关/剑.png", "第三关/护符.png", "第三关/饰品.png" };
const int lootX[LOOT_COUNT] = { 330, 395, 460, 525, 590, 460, 430, 535 };
const int lootY[LOOT_COUNT] = { 385, 375, 390, 378, 386, 380, 382, 382 };

void applyLootBonus(int id, int sign) {
    if (id == 0) { hero1.magicAttack += 5 * sign; hero1.critChance += 5 * sign; }
    else if (id == 1) hero1.defense += 6 * sign;
    else if (id == 2) { hero1.maxHp += 30 * sign; if (sign > 0) hero1.hp += 30; else hero1.hp = min(hero1.hp, hero1.maxHp); }
    else if (id == 3) { hero1.defense += 12 * sign; hero1.maxHp += 20 * sign; if (sign > 0) hero1.hp += 20; else hero1.hp = min(hero1.hp, hero1.maxHp); }
    else if (id == 4) { hero1.speed += 1 * sign; hero1.defense += 3 * sign; }
    else if (id == 5) { hero1.hurt += 18 * sign; hero1.critChance += 8 * sign; }
    else if (id == 6) { hero1.magicAttack += 15 * sign; hero1.maxMana += 20 * sign; hero1.mana = min(hero1.mana, hero1.maxMana); }
    else if (id == 7) { hero1.defense += 10 * sign; hero1.maxHp += 25 * sign; if (sign > 0) hero1.hp += 25; else hero1.hp = min(hero1.hp, hero1.maxHp); }
}

bool isLootEquipped(int id) {
    if (id < 0) return false;
    for (int i=0;i<8;++i) if (equippedLoot[i]==id) return true;
    return false;
}
void unequipLootItem(int id) {
    if (id<0 || id>=LOOT_COUNT) return;
    int slot=lootTargetSlot[id];
    if (equippedLoot[slot]==id) { applyLootBonus(id,-1); equippedLoot[slot]=-1; }
}

void equipLootItem(int id) {
    if (id < 0 || id >= LOOT_COUNT || !lootOwned[id]) return;
    int slot = lootTargetSlot[id];
    if (equippedLoot[slot] >= 0) applyLootBonus(equippedLoot[slot], -1);
    equippedLoot[slot] = id;
    applyLootBonus(id, 1);
}
bool treasureDropActive = true;
float treasureDropY = -80.0f;
float treasureDropSpeed = 3.0f;
const int treasureDropX = 470;
const int treasureGroundY = 365;

const char* treasureNames[TREASURE_COUNT] = {
    "紫藤葫芦", "钢铁鸿鹄", "紫晶偃月珠", "日月万象仪",
    "焰灯符文法轮", "太极双焰环", "佛魔双面莲火", "三鼓金焰宝幢"
};
const char* treasureEffects[TREASURE_COUNT] = {
    "使用后恢复 20 点生命，冷却 20 秒。", "装备后攻击力增加 40。",
    "神秘宝珠，效果将在后续任务解锁。", "古老法仪，效果将在后续任务解锁。",
    "燃烧的符文法轮，效果待解锁。", "阴阳相生的双焰环，效果待解锁。",
    "蕴含佛魔双焰的莲火。", "与金焰共鸣的神秘宝幢。"
};

void panel(int x1, int y1, int x2, int y2, COLORREF fill, COLORREF lineColor) {
    setfillcolor(fill); solidrectangle(x1, y1, x2, y2);
    setlinecolor(lineColor); setlinestyle(PS_SOLID, 2); rectangle(x1, y1, x2, y2);
    setlinestyle(PS_SOLID, 1);
}

void loadTreasureIcons() {
    if (treasureLoaded) return;
    for (int i = 0; i < TREASURE_COUNT; ++i) {
        for (int frame = 0; frame < 32; ++frame) {
            char path[MAX_PATH];
            sprintf_s(path, "./法宝/bao%d/rotate/action/%04d.png", i + 1, frame);
            FILE* file = fopen(path, "rb");
            if (!file) break;
            fclose(file);
            IMAGE raw, follower;
            loadimage(&raw, path);
            if (raw.getwidth() <= 0 || raw.getheight() <= 0) continue;
            scaleImage(&follower, &raw, 52, 52);
            treasureAnimations[i].push_back(follower);
            if (frame == 0) scaleImage(&treasureIcons[i], &raw, 72, 72);
        }
    }
    treasureLoaded = true;
}

void loadPortalFrames() {
    if (portalLoaded) return;
    for (int i = 0; i < 12; ++i) {
        char path[MAX_PATH]; sprintf_s(path, "./UI/chuan/frame%d.png", i);
        IMAGE raw; loadimage(&raw, path);
        if (raw.getwidth() > 0 && raw.getheight() > 0)
            scaleImage(&portalFrames[i], &raw, raw.getwidth() * _WINDOW_WIDTH__ / 1300,
                raw.getheight() * _WINDOW_LONG__ / 650);
    }
    portalLoaded = true;
}
void loadLootIcons() {
    for (int i = 0; i < LOOT_COUNT; ++i) {
        char path[MAX_PATH]; sprintf_s(path, "./各个关卡的敌人/%s", lootFiles[i]);
        IMAGE raw; loadimage(&raw, path);
        if (raw.getwidth() > 0) scaleImage(&lootIcons[i], &raw, 44, 44);
    }
}

void spawnLevelLootImpl(int level) {
    if (bossLootWasSpawned) return;
    bossLootWasSpawned = true;
    int first = level == 1 ? 0 : (level == 2 ? 5 : 6);
    int last = level == 1 ? 5 : (level == 2 ? 6 : 8);
    for (int i=0;i<LOOT_COUNT;++i) lootActive[i]=(i>=first && i<last);
}
void spawnBossLootImpl() { spawnLevelLootImpl(1); }
bool isBossLootSpawnedImpl() { return bossLootWasSpawned; }
bool hasUncollectedBossLootImpl() {
    for (int i = 0; i < LOOT_COUNT; ++i) if (lootActive[i]) return true;
    return false;
}
void updateAndDrawLootImpl() {
    for (int i = 0; i < LOOT_COUNT; ++i) if (lootActive[i]) {
        setfillcolor(RGB(25, 18, 30)); solidellipse(lootX[i]-4, lootY[i]+38, lootX[i]+48, lootY[i]+49);
        setlinecolor(RGB(215, 165, 70)); circle(lootX[i]+22, lootY[i]+22, 27);
        if (lootIcons[i].getwidth() > 0) putimagePNG2(lootX[i], lootY[i], &lootIcons[i]);
        int heroCenter = hero1.x + (hero1.idleAnimR.empty()?40:hero1.idleAnimR[0].getwidth()/2);
        if (abs(heroCenter-(lootX[i]+22)) < 75 && (GetAsyncKeyState('R') & 0x8000)) {
            lootActive[i] = false; lootOwned[i] = true;
            for(int p=0;p<18;++p) addTreasureParticle(lootX[i]+22,lootY[i]+22);
        }
    }
    if (hasUncollectedBossLootImpl()) {
        settextstyle(15,0,"Microsoft YaHei"); settextcolor(RGB(255,220,145));
        outtextxy(370,345,"靠近战利品，按住 R 拾取");
    }
}
void equipTreasure(int id) {
    if (id < 0 || id >= TREASURE_COUNT || !treasureOwned[id]) return;
    hero1.hurt -= appliedAttackBonus;
    appliedAttackBonus = (id == 1) ? 40 : 0;
    hero1.hurt += appliedAttackBonus;
    equippedTreasure = id;
}

void useTreasure() {
    if (equippedTreasure < 0) return;
    DWORD now = GetTickCount();
    if (lastTreasureUse != 0 && now - lastTreasureUse < TREASURE_COOLDOWN) return;
    if (equippedTreasure == 0) {
        hero1.hp = min(hero1.maxHp, hero1.hp + 20);
        addDamageText(hero1.x + 35, hero1.y - 8, -20, false);
    }
    lastTreasureUse = now;
    for (int i = 0; i < 32; ++i)
        addTreasureParticle(hero1.x + 35 + rand() % 50 - 25, hero1.y + 40 + rand() % 50 - 25);
}

void drawBar(int x, int y, int w, int h, int value, int maximum, COLORREF color, const char* label) {
    int safeMax = maximum > 0 ? maximum : 1;
    int fill = max(0, min(w, value * w / safeMax));
    setfillcolor(RGB(13, 15, 25)); solidrectangle(x, y, x + w, y + h);
    setfillcolor(color); solidrectangle(x + 2, y + 2, x + fill - 2, y + h - 2);
    setlinecolor(RGB(170, 150, 105)); rectangle(x, y, x + w, y + h);
    char text[80]; sprintf_s(text, "%s  %d / %d", label, value, maximum);
    settextstyle(15, 0, "Microsoft YaHei"); settextcolor(WHITE);
    outtextxy(x + 8, y + (h - textheight(text)) / 2, text);
}
}

void spawnBossLoot() { spawnBossLootImpl(); }
void spawnLevelLoot(int level) { spawnLevelLootImpl(level); }
void beginLevelLoot() { bossLootWasSpawned = false; for (int i=0;i<LOOT_COUNT;++i) lootActive[i]=false; }
bool isBossLootSpawned() { return isBossLootSpawnedImpl(); }
bool hasUncollectedBossLoot() { return hasUncollectedBossLootImpl(); }
void updateAndDrawLoot() { updateAndDrawLootImpl(); }
void resetTreasureSystem() {
    hero1.hurt -= appliedAttackBonus;
    appliedAttackBonus = 0;
    for (int i = 0; i < TREASURE_COUNT; ++i) treasureOwned[i] = false;
    equippedTreasure = -1; selectedTreasure = 0; lastTreasureUse = 0;
    bossLootWasSpawned = false;
    for (int i=0;i<LOOT_COUNT;++i) { lootOwned[i]=false; lootActive[i]=false; }
    for (int i=0;i<8;++i) equippedLoot[i]=-1;
    treasureDropActive = true; treasureDropY = -80.0f; treasureDropSpeed = 3.0f;
}

void grantTreasure(int id) {
    if (id < 0 || id >= TREASURE_COUNT) return;
    loadTreasureIcons(); treasureOwned[id] = true;
    if (equippedTreasure < 0) equipTreasure(id);
}

int getEquippedTreasure() { return equippedTreasure; }

void init() { loadTreasureIcons(); loadPortalFrames(); loadLootIcons(); }

void updateTreasureSystem() {
    bool eDown = (GetAsyncKeyState('E') & 0x8000) != 0;
    if (eDown && !eWasDown) useTreasure();
    eWasDown = eDown;

    if (treasureDropActive) {
        if (treasureDropY < treasureGroundY) {
            treasureDropY += treasureDropSpeed;
            treasureDropSpeed = min(13.0f, treasureDropSpeed + 0.28f);
            if (treasureDropY > treasureGroundY) treasureDropY = (float)treasureGroundY;
        }
        int heroCenterX = hero1.x + (hero1.idleAnimR.empty() ? 50 : hero1.idleAnimR[0].getwidth() / 2);
        bool nearDrop = abs(heroCenterX - (treasureDropX + 26)) < 105 && treasureDropY >= treasureGroundY;
        if (nearDrop && (GetAsyncKeyState('R') & 0x8000)) {
            treasureDropActive = false;
            grantTreasure(0);
            for (int i = 0; i < 55; ++i)
                addFireParticle(treasureDropX + 26 + rand() % 50 - 25, treasureGroundY + 25 + rand() % 35 - 18);
        }
    }
}

void updateUI() {
    updateTreasureSystem();
    loadPortalFrames();

    // Bao1 falls from the sky, lands, then waits for the player to hold R nearby.
    if (treasureDropActive && !treasureAnimations[0].empty()) {
        int frame = (GetTickCount() / 90) % (int)treasureAnimations[0].size();
        int dy = (int)treasureDropY;
        if (dy >= treasureGroundY) {
            setfillcolor(RGB(25, 20, 30)); fillellipse(treasureDropX + 6, treasureGroundY + 47, treasureDropX + 48, treasureGroundY + 57);
            setlinecolor(RGB(240, 190, 75)); circle(treasureDropX + 26, treasureGroundY + 25, 34 + (GetTickCount() / 180) % 4);
        } else {
            setlinecolor(RGB(150, 100, 220));
            line(treasureDropX + 26, max(0, dy - 90), treasureDropX + 26, dy + 18);
        }
        putimagePNG2(treasureDropX, dy, &treasureAnimations[0][frame]);
        int heroCenterX = hero1.x + (hero1.idleAnimR.empty() ? 50 : hero1.idleAnimR[0].getwidth() / 2);
        if (dy >= treasureGroundY && abs(heroCenterX - (treasureDropX + 26)) < 105) {
            setfillcolor(RGB(12, 10, 18)); solidrectangle(treasureDropX - 35, treasureGroundY - 38, treasureDropX + 88, treasureGroundY - 8);
            setlinecolor(RGB(210, 165, 75)); rectangle(treasureDropX - 35, treasureGroundY - 38, treasureDropX + 88, treasureGroundY - 8);
            settextstyle(15, 0, "Microsoft YaHei"); settextcolor(RGB(255, 224, 150));
            outtextxy(treasureDropX - 21, treasureGroundY - 32, "按住 R 拾取法宝");
        }
    }
    // Restored animated teleport array.
    int portalFrame = (GetTickCount() / 100) % 12;
    if (portalFrames[portalFrame].getwidth() > 0)
        putimagePNG2(760, 380, &portalFrames[portalFrame]);

    // Equipped treasure floats and rotates beside the hero.
    if (equippedTreasure >= 0 && !treasureAnimations[equippedTreasure].empty()) {
        vector<IMAGE>& anim = treasureAnimations[equippedTreasure];
        int frame = (GetTickCount() / 90) % (int)anim.size();
        int bob = (int)(5 * sin(GetTickCount() / 220.0));
        int fx = hero1.attackR ? hero1.x - 10 : hero1.x + 64;
        int fy = hero1.y + 20 + bob;
        fx = max(5, min(_WINDOW_WIDTH__ - 58, fx));
        setlinecolor(RGB(245, 196, 85));
        circle(fx + 26, fy + 26, 27 + (GetTickCount() / 180) % 2);
        putimagePNG2(fx, fy, &anim[frame]);
    }

    // Compact layered HUD: no oversized box, stronger hierarchy and cleaner bars.
    setfillcolor(RGB(7, 9, 16)); solidrectangle(18, 16, 370, 103);
    setfillcolor(RGB(185, 137, 55)); solidrectangle(18, 16, 23, 103);
    setlinecolor(RGB(82, 73, 65)); rectangle(23, 16, 370, 103);
    settextstyle(23, 0, "Microsoft YaHei"); settextcolor(RGB(241, 214, 156));
    outtextxy(36, 20, getHeroName(hero1.heroType));
    char combat[96]; sprintf_s(combat, "攻击 %d   防御 %d   暴击 %d%%", hero1.hurt, hero1.defense, hero1.critChance);
    settextstyle(15, 0, "Microsoft YaHei"); settextcolor(RGB(155, 153, 160)); outtextxy(190, 28, combat);
    drawBar(36, 52, 310, 15, hero1.hp, hero1.maxHp, RGB(196, 48, 55), "生命");
    drawBar(36, 76, 310, 12, hero1.mana, hero1.maxMana, RGB(49, 105, 210), "法力");

    setfillcolor(RGB(7, 9, 16)); solidrectangle(804, 16, 942, 94);
    setfillcolor(RGB(185, 137, 55)); solidrectangle(937, 16, 942, 94);
    setlinecolor(RGB(82, 73, 65)); rectangle(804, 16, 937, 94);
    settextstyle(16, 0, "Microsoft YaHei"); settextcolor(RGB(205, 190, 158)); outtextxy(815, 23, "法宝  [E]");
    if (equippedTreasure >= 0) {
        putimagePNG2(816, 42, &treasureIcons[equippedTreasure]);
        DWORD elapsed = lastTreasureUse == 0 ? TREASURE_COOLDOWN : GetTickCount() - lastTreasureUse;
        int remain = elapsed >= TREASURE_COOLDOWN ? 0 : (int)((TREASURE_COOLDOWN - elapsed + 999) / 1000);
        char cd[20]; if (remain) sprintf_s(cd, "%ds", remain); else strcpy_s(cd, "就绪");
        settextstyle(15, 0, "Arial"); settextcolor(remain ? RGB(170, 170, 180) : RGB(104, 225, 145));
        outtextxy(886, 59, cd);
    }

    setfillcolor(RGB(7, 9, 16)); solidrectangle(0, 514, 960, 540);
    setlinecolor(RGB(73, 64, 54)); line(0, 513, 960, 513);
    settextstyle(16, 0, "Microsoft YaHei"); settextcolor(RGB(170, 168, 166));
    outtextxy(24, 520, "A/D 移动    J 攻击    K 跳跃    L/Q 技能    E 法宝    1 背包    ESC 返回");
}
static bool inBag = false;

void drawBagSlot(int x, int y, int size, bool active) {
    setfillcolor(active ? RGB(48, 37, 30) : RGB(24, 20, 21));
    solidrectangle(x, y, x + size, y + size);
    setlinecolor(active ? RGB(188, 145, 78) : RGB(83, 72, 67));
    rectangle(x, y, x + size, y + size);
    setlinecolor(RGB(39, 33, 34)); rectangle(x + 3, y + 3, x + size - 3, y + size - 3);
}

void UI_BAG() {
    loadTreasureIcons();
    bool mouseDown = false;
    int mx = 0, my = 0, draggingItem = -1;
    int tab = 0; // 0 equipment, 1 weapon, 2 treasure
    while (true) {
        bool oneDown = (GetAsyncKeyState('1') & 0x8000) != 0;
        if ((GetAsyncKeyState(VK_ESCAPE) & 0x8000) || (oneDown && !bagWasDown)) break;
        bagWasDown = oneDown;

        bool click = false, released = false;
        while (MouseHit()) {
            MOUSEMSG m = GetMouseMsg(); mx = m.x; my = m.y;
            if (m.uMsg == WM_LBUTTONDOWN && !mouseDown) { click = true; mouseDown = true; }
            if (m.uMsg == WM_LBUTTONUP) { released = true; mouseDown = false; }
        }
        if (click && my >= 82 && my <= 124) {
            if (mx >= 80 && mx < 250) tab = 0;
            else if (mx >= 250 && mx < 420) tab = 1;
            else if (mx >= 420 && mx < 590) tab = 2;
        }

        BeginBatchDraw();
        setbkcolor(RGB(18, 18, 20)); cleardevice();
        for (int y = 30; y < 510; y += 24) {
            int shade = 18 + (y / 24) % 2 * 3;
            setfillcolor(RGB(shade, shade - 2, shade - 1));
            solidrectangle(0, y, 960, y + 24);
        }
        setlinecolor(RGB(48, 40, 36));
        for (int x = 18; x < 960; x += 74) line(x, 30, x - 28, 510);
        // Ornamental top and outer black-iron frame.
        setfillcolor(RGB(5, 6, 7)); solidrectangle(0, 0, 960, 30);
        setfillcolor(RGB(8, 8, 10)); solidrectangle(0, 510, 960, 540);
        setlinecolor(RGB(65, 58, 54)); line(0, 29, 960, 29); line(0, 510, 960, 510);
        panel(8, 36, 952, 494, RGB(20, 17, 18), RGB(112, 91, 69));
        panel(18, 48, 942, 482, RGB(31, 25, 24), RGB(55, 49, 47));
        int corners[4][2] = { {19,49}, {941,49}, {19,481}, {941,481} };
        for (int i = 0; i < 4; ++i) {
            setfillcolor(RGB(142, 103, 53)); solidcircle(corners[i][0], corners[i][1], 5);
            setlinecolor(RGB(230, 184, 98)); circle(corners[i][0], corners[i][1], 7);
        }
        setlinecolor(RGB(133, 96, 55));
        line(245, 68, 705, 68); line(265, 72, 685, 72);
        setfillcolor(RGB(105, 72, 40)); solidcircle(475, 70, 10);
        setlinecolor(RGB(218, 164, 82)); circle(475, 70, 13);

        settextstyle(34, 0, "Microsoft YaHei"); settextcolor(RGB(225, 198, 145));
        outtextxy(88, 48, "角色背包");
        settextstyle(17, 0, "Microsoft YaHei"); settextcolor(RGB(139, 126, 115));
        outtextxy(700, 57, "按 1 或 ESC 关闭");

        const char* tabs[3] = { "装备栏", "武器栏", "法宝栏" };
        for (int i = 0; i < 3; ++i) {
            int x = 80 + i * 170;
            setfillcolor(i == tab ? RGB(73, 43, 31) : RGB(36, 28, 27));
            solidrectangle(x, 82, x + 160, 124);
            setlinecolor(i == tab ? RGB(196, 145, 72) : RGB(80, 68, 61));
            rectangle(x, 82, x + 160, 124);
            settextstyle(22, 0, "Microsoft YaHei");
            settextcolor(i == tab ? RGB(255, 219, 151) : RGB(157, 143, 132));
            outtextxy(x + (160 - textwidth(tabs[i])) / 2, 92, tabs[i]);
        }

        // Shared details panel on the right.
        panel(580, 82, 695, 454, RGB(38, 29, 25), RGB(105, 85, 65));
        settextstyle(24, 0, "Microsoft YaHei"); settextcolor(RGB(222, 190, 135));
        outtextxy(588, 102, "角色详情");
        setlinecolor(RGB(109, 84, 58)); line(588, 138, 687, 138);
        char info[128];
        settextstyle(14, 0, "Microsoft YaHei"); settextcolor(RGB(203, 188, 164));
        sprintf_s(info, "职业：%s", getHeroName(hero1.heroType)); outtextxy(588, 160, info);
        sprintf_s(info, "生命：%d / %d", hero1.hp, hero1.maxHp); outtextxy(588, 194, info);
        sprintf_s(info, "法力：%d / %d", hero1.mana, hero1.maxMana); outtextxy(588, 225, info);
        sprintf_s(info, "攻击：%d", hero1.hurt); outtextxy(588, 270, info);
        sprintf_s(info, "防御：%d", hero1.defense); outtextxy(588, 301, info);
        sprintf_s(info, "魔法：%d", hero1.magicAttack); outtextxy(588, 332, info);
        sprintf_s(info, "暴击：%d%%", hero1.critChance); outtextxy(588, 363, info);
        sprintf_s(info, "通关数：%d", hero1.levelsCleared); outtextxy(588, 394, info);

        // Loot inventory in the right-side panel. Drag an icon to its matching equipment slot.
        panel(705, 82, 930, 454, RGB(25, 20, 21), RGB(125, 94, 60));
        settextstyle(20, 0, "Microsoft YaHei"); settextcolor(RGB(228, 198, 139));
        outtextxy(726, 102, "拾取物品栏");
        settextstyle(12, 0, "Microsoft YaHei"); settextcolor(RGB(140, 128, 118));
        outtextxy(720, 132, "拖到对应部位 · 拖回来卸装");
        for (int i = 0; i < LOOT_COUNT; ++i) {
            int ix = 720 + (i % 3) * 68;
            int iy = 165 + (i / 3) * 92;
            drawBagSlot(ix, iy, 56, lootOwned[i] && !isLootEquipped(i));
            if (lootOwned[i] && !isLootEquipped(i) && lootIcons[i].getwidth() > 0) {
                putimagePNG2(ix + 4, iy + 4, &lootIcons[i]);
                settextstyle(10, 0, "Microsoft YaHei"); settextcolor(RGB(190, 174, 148));
                outtextxy(ix, iy + 55, lootNames[i]);
                if (click && mx >= ix && mx <= ix + 56 && my >= iy && my <= iy + 56)
                    draggingItem = i;
            }
        }
        if (tab == 0) {
            panel(80, 136, 575, 454, RGB(39, 30, 27), RGB(125, 94, 60));
            setfillcolor(RGB(25, 20, 21)); solidrectangle(94, 150, 561, 440);
            setlinecolor(RGB(76, 57, 42)); rectangle(94, 150, 561, 440);
            for (int r = 60; r <= 122; r += 20) circle(326, 277, r);

            // Symmetric equipment slots with clear labels.
            int slots[8][2] = { {300,158},{205,196},{395,196},{180,278},{420,278},{205,350},{395,350},{300,378} };
            const char* slotNames[8] = { "头盔","护肩","盔甲","饰品","戒指","护腕","战靴","武器" };
            for (int i = 0; i < 8; ++i) {
                bool targetHover = draggingItem >= 0 && lootTargetSlot[draggingItem] == i &&
                    mx >= slots[i][0] && mx <= slots[i][0] + 54 && my >= slots[i][1] && my <= slots[i][1] + 54;
                drawBagSlot(slots[i][0], slots[i][1], 54, equippedLoot[i] >= 0 || targetHover);
                if (equippedLoot[i] >= 0 && lootIcons[equippedLoot[i]].getwidth() > 0) {
                    putimagePNG2(slots[i][0] + 5, slots[i][1] + 5, &lootIcons[equippedLoot[i]]);
                    if (click && mx >= slots[i][0] && mx <= slots[i][0]+54 && my >= slots[i][1] && my <= slots[i][1]+54)
                        draggingItem = equippedLoot[i];
                }
                settextstyle(12, 0, "Microsoft YaHei"); settextcolor(RGB(125, 112, 103));
                outtextxy(slots[i][0] + (54 - textwidth(slotNames[i])) / 2, slots[i][1] + 58, slotNames[i]);
            }
            if (released && draggingItem >= 0) {
                int target = lootTargetSlot[draggingItem];
                bool wasEquipped = equippedLoot[target] == draggingItem;
                bool onTarget = mx >= slots[target][0] && mx <= slots[target][0] + 54 && my >= slots[target][1] && my <= slots[target][1] + 54;
                bool onInventory = mx >= 705 && mx <= 930 && my >= 82 && my <= 454;
                if (onTarget && !wasEquipped) equipLootItem(draggingItem);
                else if (onInventory && wasEquipped) unequipLootItem(draggingItem);
                draggingItem = -1;
            }

            // Animated character preview in the center instead of a large empty circle.
            setfillcolor(RGB(64, 48, 38)); solidcircle(327, 284, 80);
            setlinecolor(RGB(201, 150, 72)); circle(327, 284, 83);
            if (!hero1.idleAnimR.empty()) {
                int frame = (GetTickCount() / 110) % (int)hero1.idleAnimR.size();
                IMAGE& raw = hero1.idleAnimR[frame];
                // Scale down to fit inside the circle (diameter ~124px)
                float previewScale = 190.0f / max(raw.getwidth(), raw.getheight());
                int pw = (int)(raw.getwidth() * previewScale);
                int ph = (int)(raw.getheight() * previewScale);
                IMAGE preview;
                scaleImage(&preview, &raw, pw, ph);
                // Center the preview exactly in the circle
                int px = 327 - pw / 2;
                int py = 284 - ph / 2;
                putimagePNG2(px, py, &preview);
            }
            setfillcolor(RGB(54, 38, 29)); solidrectangle(263, 347, 391, 376);
            setlinecolor(RGB(152, 108, 60)); rectangle(263, 347, 391, 376);
            settextstyle(15, 0, "Microsoft YaHei"); settextcolor(RGB(234, 201, 139));
            const char* equippedText = equippedTreasure >= 0 ? "当前已装备" : "尚未装备法宝";
            outtextxy(263 + (128 - textwidth(equippedText)) / 2, 353, equippedText);
        }
        else if (tab == 1) {
            panel(80, 136, 575, 454, RGB(30, 24, 24), RGB(102, 82, 64));
            settextstyle(24, 0, "Microsoft YaHei"); settextcolor(RGB(220, 188, 132)); outtextxy(105, 157, "当前武器");
            drawBagSlot(105, 202, 112, true);
            settextstyle(58, 0, "Microsoft YaHei"); settextcolor(RGB(208, 174, 110)); outtextxy(134, 224, "剑");
            settextstyle(22, 0, "Microsoft YaHei"); settextcolor(RGB(235, 211, 166));
            outtextxy(250, 205, hero1.heroType == HERO_SWORDSMAN ? "星辉长剑" : "守护者巨剑");
            settextstyle(19, 0, "Microsoft YaHei"); settextcolor(RGB(182, 169, 151));
            sprintf_s(info, "基础攻击：%d", HERO_STATS[hero1.heroType].attack); outtextxy(250, 248, info);
            sprintf_s(info, "当前攻击：%d", hero1.hurt); outtextxy(250, 280, info);
            sprintf_s(info, "暴击概率：%d%%", hero1.critChance); outtextxy(250, 312, info);
            outtextxy(105, 380, "更多武器将在后续冒险中解锁");
        }
        else {
            panel(80, 136, 575, 454, RGB(27, 22, 23), RGB(102, 82, 64));
            settextstyle(22, 0, "Microsoft YaHei"); settextcolor(RGB(220, 188, 132)); outtextxy(102, 151, "法宝仓库");
            for (int i = 0; i < TREASURE_COUNT; ++i) {
                int col = i % 4, row = i / 4;
                int x = 102 + col * 112, y = 190 + row * 112;
                bool hover = mx >= x && mx <= x + 88 && my >= y && my <= y + 88;
                drawBagSlot(x, y, 88, i == equippedTreasure || hover);
                if (treasureOwned[i] && treasureIcons[i].getwidth() > 0) putimagePNG2(x + 8, y + 8, &treasureIcons[i]);
                else { settextstyle(28, 0, "Arial"); settextcolor(RGB(73, 66, 66)); outtextxy(x + 35, y + 26, "?"); }
                if (hover && click) {
                    selectedTreasure = i;
                    if (treasureOwned[i] && i == equippedTreasure) useTreasure();
                }
            }
            settextstyle(18, 0, "Microsoft YaHei");
            if (treasureOwned[selectedTreasure]) {
                settextcolor(RGB(235, 201, 139)); outtextxy(102, 418, treasureNames[selectedTreasure]);
                int bx = 438, by = 407;
                bool hover = mx >= bx && mx <= 555 && my >= by && my <= 443;
                setfillcolor(hover ? RGB(105, 58, 34) : RGB(74, 43, 30)); solidrectangle(bx, by, 555, 443);
                setlinecolor(RGB(192, 137, 66)); rectangle(bx, by, 555, 443);
                const char* label = selectedTreasure == equippedTreasure ? "按 E 使用" : "点击装备";
                settextcolor(RGB(250, 220, 160)); outtextxy(bx + (117 - textwidth(label)) / 2, by + 9, label);
                if (hover && click) {
                    if (selectedTreasure == equippedTreasure) useTreasure();
                    else equipTreasure(selectedTreasure);
                }
            } else {
                settextcolor(RGB(126, 118, 115)); outtextxy(102, 418, "尚未获得其他法宝");
            }
        }

        if (draggingItem >= 0 && lootIcons[draggingItem].getwidth() > 0)
            putimagePNG2(mx - 22, my - 22, &lootIcons[draggingItem]);
        EndBatchDraw(); Sleep(16);
    }
}
int UIdetect() {
    bool oneDown = (GetAsyncKeyState('1') & 0x8000) != 0;
    if (oneDown && !bagWasDown && !inBag) {
        bagWasDown = true; inBag = true; UI_BAG(); inBag = false;
    }
    if (!oneDown) bagWasDown = false;
    if ((GetAsyncKeyState(VK_ESCAPE) & 0x8000) != 0) return 10;
    return 0;
}
