#ifndef __HERO_H__
#define __HERO_H__
#include<graphics.h>
#include<string>
#include<vector>
using namespace std;

enum HeroType {
    HERO_SWORDSMAN = 0,
    HERO_SWORD_SHIELD = 1,
    HERO_COUNT = 2
};

const char* getHeroName(int type);
const char* getHeroFolder(int type);

// Load all frame_N.png files from a directory
int loadAnimFrames(vector<IMAGE>& out, const char* folder);

class hero {
public:
    struct AttackBox {
        int x, y;
        int width, height;
        bool active;
    };

    int x, y;
    int hp, maxHp;
    int speed;
    int hurt;
    int magicAttack;
    int mana, maxMana;
    int defense;
    int critChance;    // Critical hit chance (%)
    int levelsCleared;

    bool Lwalkkey, Rwalkkey, attackkey, jumpkey;
    bool attackL, attackR;
    bool hasDealtDamage;

    bool isHit, isSkill, isQSkill, isDead;
    int hitCounter, skillCounter, qskillCounter, deathCounter;
    bool sleep;
    int sleeptime;

    AttackBox atkBox;
    HeroType heroType;

    // Right-facing animations
    vector<IMAGE> idleAnimR, walkAnimR, atkAnimR, jumpAnimR, deathAnimR, skillAnimR, qskillAnimR;
    // Left-facing (flipped)
    vector<IMAGE> idleAnimL, walkAnimL, atkAnimL, deathAnimL, skillAnimL, qskillAnimL;

    // Legacy path vectors (unused with new heroes)
    vector<string> walkPaths, standPaths, attackPaths, swalkPaths, sattackPaths;
    vector<IMAGE> stand1, walk1, walk2, attack1, attack2, jump, hitR, hitL, missR, missL, standLeft, stand2;

public:
    hero();
    hero(int x1, int y1, HeroType type);
    hero(int x1, int y1, int nStand, int nWalk, int nAttack); // Legacy

    void loadHeroSprites(HeroType type);
    void updatestand(int count);
    void updatewalk(int count);
    void updateattack(int count);
    void updateHit();
    void updateSkill();
    void updateQSkill();
    void updateDeath();
    void updatejump();
    void boolhero();
    void beHit(int attackPower);
    void freeze();
    ~hero() {}
};

struct HeroTypeStats {
    int hp, attack, magic, defense, speed, critChance;
};
extern const HeroTypeStats HERO_STATS[HERO_COUNT];

#endif
