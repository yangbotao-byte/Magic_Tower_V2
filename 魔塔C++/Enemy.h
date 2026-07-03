#ifndef __ENEMY_H__
#define __ENEMY_H__
#include<vector>
#include<graphics.h>
#include"hero.h"

// Helper: flip an image horizontally
void flipImageHorizontal(IMAGE* dst, IMAGE* src);

class Enemy {
public:
    int x, y;
    int width, height;
    int hp;
    int maxHp;
    bool alive;

    std::vector<IMAGE> walkFrames;
    std::vector<IMAGE> attackFrames;
    std::vector<IMAGE> walkFramesL;   // Left-facing walk
    std::vector<IMAGE> attackFramesL; // Left-facing attack

    int currentFrame;
    bool isAttacking;
    bool facingLeft;  // Current facing direction

    RECT hurtBox;

    int attackPower;
    int attackCooldown;
    int attackTimer;

    int skillCooldown;
    int skillTimer;

    bool iceSkillActive;
    int freezeDuration;

    Enemy(int _x, int _y);

    void update(hero& player);
    void draw();
    void beHit(int damage);
    bool isHitBy(const RECT& atkBox);
    void reset(int _x, int _y);  // Reset for replay
    void configure(int health, int damage, int cooldown);
    void loadAppearance(const char* folder, float scale = 0.70f);
    void loadCustomAppearance(const char* walkFolder, const char* walkPrefix, int walkCount,
        const char* attackFolder, const char* attackPrefix, int attackCount, float scale);

private:
    void useFireSkill(hero& player);
    void useIceSkill(hero& player);
};

#endif
