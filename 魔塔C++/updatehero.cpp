#include"allinclude.h"
static int counter;

// Get attack hitbox rect (J/L/Q all use this, in front of hero at body level)
RECT getAtkRect() {
    RECT r;
    int sw = hero1.idleAnimR.empty() ? 100 : hero1.idleAnimR[0].getwidth();
    int sh = hero1.idleAnimR.empty() ? 100 : hero1.idleAnimR[0].getheight();
    r.top = hero1.y + sw / 2;
    r.bottom = r.top + sh / 4;
    if (hero1.attackR) {
        r.left = hero1.x + sw / 2;   r.right = hero1.x + sw * 3 / 4;
    } else {
        r.left = hero1.x + sw / 4;   r.right = hero1.x + sw / 2;
    }
    return r;
}

void updatehero() {
    if (hero1.isHit) {
        hero1.updateHit(); counter++;
        if (counter >= (int)hero1.hitR.size()) { counter = 0; hero1.isHit = false; }
    }
    else if (hero1.isQSkill) {
        hero1.updateQSkill(); counter++;
        if (counter == 4 && !hero1.hasDealtDamage) {
            RECT r = getAtkRect();
            if (empty1.alive && empty1.isHitBy(r)) {
                empty1.beHit(hero1.magicAttack + 5);
                addDamageText(empty1.x + empty1.width / 2, empty1.y, hero1.magicAttack + 5, true);
                hero1.hasDealtDamage = true;
            }
        }
        if (counter >= (int)hero1.missR.size()) { counter = 0; hero1.isQSkill = false; hero1.hasDealtDamage = false; }
    }
    else if (hero1.isSkill) {
        hero1.updateSkill(); counter++;
        if (counter == 3 && !hero1.hasDealtDamage) {
            RECT r = getAtkRect();
            if (empty1.alive && empty1.isHitBy(r)) {
                empty1.beHit(hero1.magicAttack);
                addDamageText(empty1.x + empty1.width / 2, empty1.y, hero1.magicAttack, true);
                hero1.hasDealtDamage = true;
            }
        }
        if (counter >= (int)hero1.missR.size()) { counter = 0; hero1.isSkill = false; hero1.hasDealtDamage = false; }
    }
    else if (hero1.sleep) {
        hero1.sleeptime--;
        int pw = hero1.idleAnimR.empty() ? 100 : hero1.idleAnimR[0].getwidth();
        int ph = hero1.idleAnimR.empty() ? 100 : hero1.idleAnimR[0].getheight();
        putimagePNG2(hero1.x + pw/4, hero1.y + ph/4, &bin[1]);
        if (hero1.sleeptime == 0) hero1.sleep = false;
    }
    else if (hero1.attackkey && !isJumping) {
        if (hero1.Rwalkkey) { hero1.attackL = false; hero1.attackR = true; }
        else if (hero1.Lwalkkey) { hero1.attackL = true; hero1.attackR = false; }
        hero1.updateattack(counter); counter++;
        if (counter == 5 && !hero1.hasDealtDamage) {
            RECT r = getAtkRect();
            if (hero1.atkBox.active && empty1.isHitBy(r)) {
                bool crit = (rand() % 100) < hero1.critChance;
                int dmg = crit ? hero1.hurt * 2 : hero1.hurt;
                empty1.beHit(dmg);
                addDamageText(empty1.x + empty1.width / 2, empty1.y, dmg, crit);
                hero1.hasDealtDamage = true;
            }
        }
        if (counter >= (int)hero1.attack1.size()) { counter = 0; hero1.attackkey = false; hero1.hasDealtDamage = false; }
        if ((GetAsyncKeyState('J') & 0x8000) == 0) { hero1.attackkey = false; counter = 0; hero1.hasDealtDamage = false; }
    }
    else if (hero1.Lwalkkey && !isJumping) {
        hero1.attackL = true; hero1.attackR = false;
        hero1.updatewalk(counter); counter++;
        if ((GetAsyncKeyState('A') & 0x8000) == 0) { hero1.Lwalkkey = false; counter = 0; }
    }
    else if (hero1.Rwalkkey && !isJumping) {
        hero1.attackL = false; hero1.attackR = true;
        hero1.updatewalk(counter); counter++;
        if ((GetAsyncKeyState('D') & 0x8000) == 0) { hero1.Rwalkkey = false; counter = 0; }
    }
    else if (!hero1.jumpkey && !isJumping) {
        counter %= hero1.stand1.size();
        hero1.updatestand(counter);
        counter++; counter %= hero1.stand1.size();
    }
}

void keydetect() {
    hero1.Lwalkkey = (GetAsyncKeyState('A') & 0x8000) != 0;
    hero1.Rwalkkey = (GetAsyncKeyState('D') & 0x8000) != 0;
    hero1.attackkey = (GetAsyncKeyState('J') & 0x8000) != 0;
    hero1.jumpkey = (GetAsyncKeyState('K') & 0x8000) != 0;
    if ((GetAsyncKeyState('L') & 0x8000) && !hero1.isSkill && !hero1.isQSkill && !hero1.isHit) {
        hero1.isSkill = true; hero1.skillCounter = 0;
    }
    if ((GetAsyncKeyState('Q') & 0x8000) && !hero1.isSkill && !hero1.isQSkill && !hero1.isHit) {
        hero1.isQSkill = true; hero1.qskillCounter = 0;
    }
}
