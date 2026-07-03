#define _CRT_SECURE_NO_WARNINGS 1
#include "particle.h"
#include <cstdlib>
#include <cmath>
#include <algorithm>

std::vector<Particle> fireParticles;
std::vector<Particle> iceParticles;
std::vector<DamageText> damageTexts;

void addFireParticle(int x, int y) {
    Particle p;
    p.x = (float)x + rand() % 20 - 10;
    p.y = (float)y + rand() % 10 - 5;
    p.dx = (float)(rand() % 120 - 60) / 100.0f;
    p.dy = -(float)(rand() % 50 + 20) / 100.0f;
    p.life = 30 + rand() % 20;
    // Fire colors: yellow → orange → red
    int r = 255;
    int g = 100 + rand() % 155;
    int b = rand() % 30;
    p.color = RGB(r, g, b);
    fireParticles.push_back(p);
}

void addIceParticle(int x, int y) {
    Particle p;
    p.x = (float)x + rand() % 30 - 15;
    p.y = (float)y + rand() % 20 - 10;
    p.dx = (float)(rand() % 80 - 40) / 100.0f;
    p.dy = -(float)(rand() % 20 + 5) / 100.0f;
    p.life = 40 + rand() % 30;
    int b = 200 + rand() % 55;
    p.color = RGB(100 + rand() % 50, 180 + rand() % 50, b);
    iceParticles.push_back(p);
}

void addTreasureParticle(int x, int y) {
    Particle p;
    double angle = (rand() % 628) / 100.0;
    float speed = 0.8f + (rand() % 100) / 80.0f;
    p.x = (float)x + rand() % 24 - 12;
    p.y = (float)y + rand() % 24 - 12;
    p.dx = (float)cos(angle) * speed;
    p.dy = (float)sin(angle) * speed - 0.35f;
    p.life = 45 + rand() % 30;
    if (rand() % 2 == 0)
        p.color = RGB(155 + rand() % 80, 65 + rand() % 65, 220 + rand() % 35);
    else
        p.color = RGB(60 + rand() % 70, 205 + rand() % 50, 105 + rand() % 90);
    fireParticles.push_back(p);
}
void addDamageText(int x, int y, int damage, bool isCrit) {
    DamageText dt;
    dt.x = (float)x + (rand() % 30 - 15);
    dt.y = (float)y;
    dt.vy = -3.5f - (rand() % 20) / 10.0f;
    dt.life = 40;
    dt.maxLife = 40;
    dt.damage = damage;
    dt.isCrit = isCrit;
    dt.color = damage < 0 ? RGB(90, 245, 135) : (isCrit ? RGB(255, 210, 55) : RGB(245, 75, 70));
    damageTexts.push_back(dt);
}

void updateAndDrawParticles() {
    // Fire particles
    for (int i = 0; i < (int)fireParticles.size(); ) {
        Particle& p = fireParticles[i];
        p.x += p.dx;
        p.y += p.dy;
        p.life--;
        float fade = (float)p.life / 50.0f;
        int r = min(255, (int)(GetRValue(p.color) * fade));
        int g = min(255, (int)(GetGValue(p.color) * fade));
        int b = min(255, (int)(GetBValue(p.color) * fade));
        int size = 2 + (int)(3 * fade);
        setfillcolor(RGB(r, g, b));
        solidcircle((int)p.x, (int)p.y, size);
        // Inner bright core
        setfillcolor(RGB(min(255,r+80), min(255,g+80), min(255,b+30)));
        solidcircle((int)p.x, (int)p.y, size/2);
        if (p.life <= 0) fireParticles.erase(fireParticles.begin() + i);
        else i++;
    }

    // Ice particles
    for (int i = 0; i < (int)iceParticles.size(); ) {
        Particle& p = iceParticles[i];
        p.x += p.dx;
        p.y += p.dy;
        p.life--;
        float fade = (float)p.life / 70.0f;
        int r = min(255, (int)(GetRValue(p.color) * fade));
        int g = min(255, (int)(GetGValue(p.color) * fade));
        int b = min(255, (int)(GetBValue(p.color) * fade));
        int size = 2 + (int)(2 * fade);
        setfillcolor(RGB(r, g, b));
        solidcircle((int)p.x, (int)p.y, size);
        if (size > 1) {
            setfillcolor(RGB(min(255,r+60), min(255,g+60), 255));
            solidcircle((int)p.x, (int)p.y, size/2);
        }
        if (p.life <= 0) iceParticles.erase(iceParticles.begin() + i);
        else i++;
    }

    // Styled floating combat text.
    for (int i=0;i<(int)damageTexts.size();) {
        DamageText& dt=damageTexts[i]; dt.y+=dt.vy;dt.vy*=0.94f;dt.life--;
        float progress=1.0f-(float)dt.life/dt.maxLife;int alpha=(int)(255*(1.0f-progress*progress));
        bool healing=dt.damage<0;char numStr[24];
        if(healing)sprintf_s(numStr,"+%d",-dt.damage);else sprintf_s(numStr,"-%d",dt.damage);
        int fs=dt.isCrit?34:25;settextstyle(fs,0,"Microsoft YaHei");int tx=(int)dt.x,ty=(int)dt.y;
        settextcolor(RGB(18,8,12));outtextxy(tx-2,ty,numStr);outtextxy(tx+2,ty,numStr);outtextxy(tx,ty-2,numStr);outtextxy(tx,ty+2,numStr);
        int cr=min(255,GetRValue(dt.color)*alpha/255),cg=min(255,GetGValue(dt.color)*alpha/255),cb=min(255,GetBValue(dt.color)*alpha/255);
        settextcolor(RGB(cr,cg,cb));outtextxy(tx,ty,numStr);
        if(dt.isCrit&&!healing){settextstyle(14,0,"Microsoft YaHei");settextcolor(RGB(cr,cg,cb));outtextxy(tx+4,ty-18,"暴击!");}
        if(dt.life<=0)damageTexts.erase(damageTexts.begin()+i);else ++i;
    }
}
