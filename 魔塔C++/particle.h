#pragma once
#include <vector>
#include <graphics.h>

struct Particle {
    float x, y;
    float dx, dy;
    int life;
    COLORREF color;
};

struct DamageText {
    float x, y;
    float vy;
    int life;
    int maxLife;
    int damage;
    COLORREF color;
    bool isCrit;
};

extern std::vector<Particle> fireParticles;
extern std::vector<Particle> iceParticles;
extern std::vector<DamageText> damageTexts;

void addFireParticle(int x, int y);
void addIceParticle(int x, int y);
void addTreasureParticle(int x, int y);
void addDamageText(int x, int y, int damage, bool isCrit);
void updateAndDrawParticles();
