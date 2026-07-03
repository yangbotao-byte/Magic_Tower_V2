#include "allinclude.h"

int level2() {
    init(); beginLevelLoot();
    IMAGE rawBg, levelBg; loadimage(&rawBg, "./关卡2.jpg"); scaleImage(&levelBg, &rawBg, _WINDOW_WIDTH__, _WINDOW_LONG__);
    empty1.reset(500, 160);
    empty1.loadCustomAppearance("./各个关卡的敌人/第二关/02_demon_walk", "demon_walk_", 12,
        "./各个关卡的敌人/第二关/03_demon_cleave", "demon_cleave_", 15, 1.55f);
    empty1.x=560; empty1.y=430-empty1.height; empty1.hurtBox={empty1.x,empty1.y,empty1.x+empty1.width,empty1.y+empty1.height};
    empty1.configure(650,16,260);
    float js=0; bool jumping=false;
    bool victoryReady=false; bool enterArmed=false;
    while(true) {
        keydetect(); UIdetect(); BeginBatchDraw(); putimage(0,0,&levelBg);
        if(hero1.jumpkey&&!jumping){jumping=true;js=-17;}
        if(jumping){hero1.y+=(int)js;js+=1.2f;if(hero1.Lwalkkey)hero1.x-=hero1.speed;if(hero1.Rwalkkey)hero1.x+=hero1.speed;
            if(!hero1.jumpAnimR.empty()){int f=(GetTickCount()/80)%hero1.jumpAnimR.size();putimagePNG2(hero1.x,hero1.y,&hero1.jumpAnimR[f]);}
            if(hero1.y>=180){hero1.y=180;jumping=false;hero1.jumpkey=false;}}
        else updatehero();
        if(empty1.alive){empty1.update(hero1);empty1.draw();}
        else {
            if(!isBossLootSpawned()) spawnLevelLoot(2);
            if(!hasUncollectedBossLoot()) victoryReady=true;
        }
        if(hero1.hp<=0){EndBatchDraw();return 2;}
        settextstyle(22,0,"Microsoft YaHei");settextcolor(RGB(255,105,90));const char* t="第二关 · 深渊魔王";outtextxy((_WINDOW_WIDTH__-textwidth(t))/2,22,t);
        updateAttackBox(hero1);updateAndDrawParticles();updateAndDrawLoot();updateUI();
        if(victoryReady){
            int l=255,t2=395,r=705,b=470;
            setfillcolor(RGB(18,14,27));setlinecolor(RGB(228,177,78));fillroundrect(l,t2,r,b,12,12);rectangle(l+5,t2+5,r-5,b-5);
            setbkmode(TRANSPARENT);settextstyle(21,0,"Microsoft YaHei");settextcolor(RGB(246,216,148));
            const char* done="战利品已收纳，第二关完成";outtextxy((_WINDOW_WIDTH__-textwidth(done))/2,407,done);
            settextstyle(16,0,"Microsoft YaHei");settextcolor(RGB(184,147,216));const char* hint="请松开 R，按 Enter 进入胜利结算";outtextxy((_WINDOW_WIDTH__-textwidth(hint))/2,440,hint);
            bool enterDown=(GetAsyncKeyState(VK_RETURN)&0x8000)!=0;
            if(!enterDown) enterArmed=true;
            if(enterArmed&&enterDown){EndBatchDraw();return 1;}
        }
        EndBatchDraw();Sleep(45);
    }
}