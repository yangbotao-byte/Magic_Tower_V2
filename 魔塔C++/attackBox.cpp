#include"allinclude.h"
#include"hero.h"

void updateAttackBox(hero& h) {
    bool showBox = h.attackkey || hero1.isSkill || hero1.isQSkill;
    if (showBox) {
        int sw = h.idleAnimR.empty() ? 100 : h.idleAnimR[0].getwidth();
        int sh = h.idleAnimR.empty() ? 100 : h.idleAnimR[0].getheight();

        h.atkBox.y = h.y + sw / 2;      // Y = hero.y + half width
        h.atkBox.width = sw / 4;         // width = quarter sprite
        h.atkBox.height = sh / 4;        // height = quarter sprite
        h.atkBox.active = true;

        if (h.attackR)      h.atkBox.x = h.x + sw / 2;
        else if (h.attackL) h.atkBox.x = h.x + sw / 4;
        else                h.atkBox.x = h.x + sw / 2;
    }

    if (hero1.atkBox.active && showBox) {
        setlinecolor(RED);
        rectangle(hero1.atkBox.x, hero1.atkBox.y,
            hero1.atkBox.x + hero1.atkBox.width,
            hero1.atkBox.y + hero1.atkBox.height);
    } else {
        h.atkBox.active = false;
    }
}
