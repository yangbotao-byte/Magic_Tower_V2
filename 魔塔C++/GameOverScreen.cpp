#include"allinclude.h"

struct Btn { int x,y,w,ht; const char* t; COLORREF b,hvC; bool hv; };
struct Confetti { float x,y,vy,vx; COLORREF c; int life,max; };

void drawBtn(Btn& b) {
    setfillcolor(RGB(0,0,0)); fillrectangle(b.x+2,b.y+2,b.x+b.w+2,b.y+b.ht+2);
    COLORREF c=b.hv?b.hvC:b.b; setfillcolor(c); fillrectangle(b.x,b.y,b.x+b.w,b.y+b.ht);
    setfillcolor(RGB(min(255,GetRValue(c)+40),min(255,GetGValue(c)+40),min(255,GetBValue(c)+40)));
    fillrectangle(b.x+2,b.y+2,b.x+b.w-2,b.y+b.ht/2);
    setlinecolor(RGB(200,200,200)); setlinestyle(PS_SOLID,2);
    rectangle(b.x,b.y,b.x+b.w,b.y+b.ht); setlinestyle(PS_SOLID,1);
    settextcolor(b.hv?RGB(255,255,200):WHITE); settextstyle(18,0,_T("Arial"));
    outtextxy(b.x+(b.w-textwidth(b.t))/2,b.y+(b.ht-textheight(b.t))/2,b.t);
}

// Draw a star shape for victory
void drawStar(int cx, int cy, int r, COLORREF c) {
    int pts[10][2];
    for (int i=0;i<5;i++) {
        double a1=(i*72-90)*3.14159/180, a2=(i*72+36-90)*3.14159/180;
        pts[i*2][0]=cx+(int)(r*cos(a1));     pts[i*2][1]=cy+(int)(r*sin(a1));
        pts[i*2+1][0]=cx+(int)(r*0.4*cos(a2)); pts[i*2+1][1]=cy+(int)(r*0.4*sin(a2));
    }
    POINT p[10]; for(int i=0;i<10;i++){p[i].x=pts[i][0];p[i].y=pts[i][1];}
    setfillcolor(c); solidpolygon(p,10);
}

// Draw an X for defeat
void drawX(int cx, int cy, int s, COLORREF c) {
    setlinecolor(c); setlinestyle(PS_SOLID,5);
    line(cx-s,cy-s,cx+s,cy+s); line(cx+s,cy-s,cx-s,cy+s);
    setlinestyle(PS_SOLID,1);
}

// Draw sparkle particles
void sparkle(int x, int y) {
    static float ph=0; ph+=0.1f;
    for (int i=0;i<8;i++) {
        float a=i*0.785f+ph; int r=8+rand()%6;
        setfillcolor(RGB(255,255,200+rand()%55));
        solidcircle(x+(int)(r*cos(a)),y+(int)(r*sin(a)),1+rand()%2);
    }
}

int showGameOver(bool victory) {
    const int cx=_WINDOW_WIDTH__/2, cy=180;

    Btn btns[]={
        {170,440,180,45,"Play Again",RGB(40,140,40),RGB(60,200,60),false},
        {390,440,180,45,"Level Map",RGB(40,80,160),RGB(60,120,210),false},
        {610,440,180,45,"Main Menu",RGB(150,40,40),RGB(210,60,60),false},
    };

    vector<Confetti> cf;
    if (victory) for(int i=0;i<80;i++){Confetti c;c.x=(float)(rand()%_WINDOW_WIDTH__);c.y=-(float)(rand()%400);c.vy=1.5f+rand()%30/10.f;c.vx=(rand()%50-25)/10.f;c.c=RGB(100+rand()%155,100+rand()%155,100+rand()%155);c.life=120+rand()%80;c.max=c.life;cf.push_back(c);}

    float anim=0, glow=0;

    // Initial burst
    for(int i=0;i<40;i++) addFireParticle(cx+rand()%60-30, cy+rand()%20-10);

    while(1){
        int mx=-1,my=-1; bool clk=false;
        while(MouseHit()){MOUSEMSG m=GetMouseMsg();mx=m.x;my=m.y;if(m.uMsg==WM_LBUTTONDOWN)clk=true;}
        for(int i=0;i<3;i++) btns[i].hv=(mx>=btns[i].x&&mx<=btns[i].x+btns[i].w&&my>=btns[i].y&&my<=btns[i].y+btns[i].ht);
        if(clk){if(btns[0].hv)return 1;if(btns[1].hv)return 2;if(btns[2].hv)return 3;}
        if(_kbhit()){char c=_getch();if(c==13||c==' ')return 1;if(c==27)return 3;}

        anim+=0.03f; glow+=0.06f;
        if(anim>1)anim=1;

        cleardevice(); BeginBatchDraw();
        setbkcolor(RGB(12,10,25));

        // Background particles
        for(int i=0;i<60;i++){setfillcolor(RGB(25,20,40));solidcircle((i*137+50)%_WINDOW_WIDTH__,(i*89+30)%_WINDOW_LONG__,1+i%2);}

        // === Icon panel ===
        int panW=500, panH=260, panX=(_WINDOW_WIDTH__-panW)/2, panY=55;
        setfillcolor(RGB(18,16,38));
        fillrectangle(panX,panY,panX+panW,panY+panH);
        // Glowing border
        int ga=(int)(80+40*sin(glow));
        setlinecolor(RGB(victory?ga:ga/2, victory?ga/2:ga, 60));
        setlinestyle(PS_SOLID,3); rectangle(panX,panY,panX+panW,panY+panH); setlinestyle(PS_SOLID,1);

        // Victory icon
        if(victory){
            // Animated star
            int sr=(int)(40+10*sin(anim*6.28f));
            drawStar(cx, cy-10, sr*anim, RGB(255,215,0));
            // Glow rings
            for(int r=sr+20;r>sr;r-=8){
                int a=(int)(80*(float)(r-sr)/20);
                setlinecolor(RGB(255,215,a)); circle(cx,cy-10,r);
            }
            // Sparkles
            if(rand()%100<40) sparkle(cx,cy-10);
        } else {
            // X icon
            int xs=(int)(35*anim);
            drawX(cx, cy-10, xs, RGB(220,50,50));
            // Pulsing circle
            setlinecolor(RGB(220,50,(int)(80+40*sin(glow)))); setlinestyle(PS_SOLID,2);
            circle(cx, cy-10, xs+15); setlinestyle(PS_SOLID,1);
        }

        // Title
        int ts=(int)(40*anim); if(ts<4)ts=4;
        const char* tt=victory?"VICTORY!":"DEFEATED";
        COLORREF tc=victory?RGB(255,215,0):RGB(230,50,50);
        settextcolor(RGB(GetRValue(tc)*ga/255,GetGValue(tc)*ga/255,GetBValue(tc)*ga/255));
        settextstyle(ts+8,0,_T("Arial")); outtextxy((_WINDOW_WIDTH__-textwidth(tt))/2,panY+20,tt);
        settextcolor(tc); settextstyle(ts,0,_T("Arial"));
        outtextxy((_WINDOW_WIDTH__-textwidth(tt))/2,panY+24,tt);

        // Subtitle
        settextcolor(RGB(180,180,200)); settextstyle(18,0,_T("Arial"));
        const char* st=victory?"Enemy vanquished!":"Fallen in battle...";
        outtextxy((_WINDOW_WIDTH__-textwidth(st))/2,panY+75,st);

        // Stats
        settextstyle(15,0,_T("Arial"));
        int sy2=panY+105;
        char t[64];
        if(victory){
            settextcolor(RGB(100,255,130));
            sprintf(t,"HP Remaining: %d / %d",hero1.hp,hero1.maxHp); outtextxy((_WINDOW_WIDTH__-textwidth(t))/2,sy2,t);
            settextcolor(RGB(180,200,255));
            sprintf(t,"Attack: %d  |  Crit: %d%%  |  Cleared: %d",hero1.hurt,hero1.critChance,hero1.levelsCleared+1);
            outtextxy((_WINDOW_WIDTH__-textwidth(t))/2,sy2+25,t);
            settextcolor(RGB(255,200,100));
            outtextxy((_WINDOW_WIDTH__-textwidth("Outstanding!"))/2,sy2+50,"Outstanding!");
        } else {
            settextcolor(RGB(255,120,120));
            sprintf(t,"Enemy HP: %d / %d",empty1.hp,empty1.maxHp); outtextxy((_WINDOW_WIDTH__-textwidth(t))/2,sy2,t);
            settextcolor(RGB(200,180,140));
            sprintf(t,"Your ATK: %d  |  Enemy ATK: %d",hero1.hurt,empty1.attackPower);
            outtextxy((_WINDOW_WIDTH__-textwidth(t))/2,sy2+25,t);
            settextcolor(RGB(255,200,100));
            outtextxy((_WINDOW_WIDTH__-textwidth("Rise again, warrior!"))/2,sy2+50,"Rise again, warrior!");
        }

        // Confetti
        if(victory) for(int i=0;i<(int)cf.size();i++){Confetti& c=cf[i];c.y+=c.vy;c.x+=c.vx;c.vx*=0.998f;c.life--;float p=1.f-(float)c.life/c.max;int a=(int)(255*(1-p*p));if(a>0&&c.y>-50&&c.y<_WINDOW_LONG__+50){setfillcolor(RGB(min(255,GetRValue(c.c)*a/255),min(255,GetGValue(c.c)*a/255),min(255,GetBValue(c.c)*a/255)));solidcircle((int)c.x,(int)c.y,2+(int)(3*(1-p)));}}
        if(victory){for(int i=(int)cf.size()-1;i>=0;i--)if(cf[i].life<=0||cf[i].y>_WINDOW_LONG__+50)cf.erase(cf.begin()+i);if(cf.size()<35&&rand()%100<35){Confetti c;c.x=(float)(rand()%_WINDOW_WIDTH__);c.y=-20;c.vy=2.5f+rand()%25/10.f;c.vx=(rand()%40-20)/10.f;c.c=RGB(100+rand()%155,100+rand()%155,100+rand()%155);c.life=100+rand()%60;c.max=c.life;cf.push_back(c);}}

        updateAndDrawParticles();

        // Buttons
        for(int i=0;i<3;i++) drawBtn(btns[i]);

        settextcolor(RGB(140,140,140)); settextstyle(12,0,_T("Arial"));
        const char* h="Enter/Space: Play Again  |  ESC: Menu";
        outtextxy((_WINDOW_WIDTH__-textwidth(h))/2,505,h);

        EndBatchDraw(); Sleep(16);
    }
    return 0;
}
