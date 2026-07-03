#include"allinclude.h"
#include<algorithm>

#define MAXN 100
#define SCREEN_WIDTH 960
#define SCREEN_HEIGHT 540

struct Node {
    int id, x, y;
};

struct Edge {
    int u, v, w;
    bool operator<(const Edge& e) const {
        return w < e.w;
    }
};

struct Sparkle {
    int x, y;
    float speedY;
    int life;
    int maxLife;
};

struct TrailParticle {
    float x, y;
    int life;
    int maxLife;
};

// Graph data
vector<Node> nodes;
vector<pair<int, int> > graph[MAXN];
vector<Edge> allEdges;
vector<Edge> mstEdges;
float pulse = 0;
bool increasing = true;
bool showMST = false;

struct Flame {
    int x, y, life;
};
vector<Flame> flames;
vector<Sparkle> sparkles;
vector<TrailParticle> trailParticles;

int fa[MAXN];
int find(int x) {
    if (fa[x] == x) return x;
    else { fa[x] = find(fa[x]); return fa[x]; }
}

void addEdge(int u, int v, int w) {
    graph[u].push_back(make_pair(v, w));
    graph[v].push_back(make_pair(u, w));
    allEdges.push_back({ u, v, w });
}

void kruskal(int start, int end) {
    for (int i = 0; i < MAXN; ++i) fa[i] = i;
    sort(allEdges.begin(), allEdges.end());
    mstEdges.clear();
    for (size_t i = 0; i < allEdges.size(); ++i) {
        Edge& e = allEdges[i];
        int fu = find(e.u), fv = find(e.v);
        if (fu != fv) {
            fa[fu] = fv;
            mstEdges.push_back(e);
        }
    }
    showMST = true;
}

vector<int> dijkstra(int start, int end) {
    const int INF = 1e9;
    vector<int> dist(nodes.size(), INF);
    vector<int> prev(nodes.size(), -1);
    priority_queue<pair<int, int>, vector<pair<int, int> >, greater<pair<int, int> > > pq;
    dist[start] = 0;
    pq.push(make_pair(0, start));
    while (!pq.empty()) {
        pair<int, int> top = pq.top(); pq.pop();
        int d = top.first, u = top.second;
        if (d > dist[u]) continue;
        for (size_t i = 0; i < graph[u].size(); ++i) {
            int v = graph[u][i].first;
            int w = graph[u][i].second;
            if (dist[u] + w < dist[v]) {
                dist[v] = dist[u] + w;
                prev[v] = u;
                pq.push(make_pair(dist[v], v));
            }
        }
    }
    vector<int> path;
    for (int v = end; v != -1; v = prev[v])
        path.push_back(v);
    reverse(path.begin(), path.end());
    return path;
}

int getClickedNode(int mx, int my) {
    for (int i = 0; i < (int)nodes.size(); ++i) {
        int dx = mx - nodes[i].x;
        int dy = my - nodes[i].y;
        if (sqrt((double)(dx * dx + dy * dy)) < 35)
            return nodes[i].id;
    }
    return -1;
}

// ==================== Drawing Functions ====================

void drawTitle() {
    static float titleGlow = 0;
    titleGlow += 0.03f;
    int alpha = 180 + (int)(75 * sin(titleGlow));

    // Shadow
    settextcolor(RGB(0, 0, 0));
    settextstyle(36, 0, _T("Arial"));
    outtextxy(53, 23, _T("魔塔 · 三章之路"));

    // Glowing golden title
    settextcolor(RGB(255, 215, alpha));
    settextstyle(36, 0, _T("Arial"));
    outtextxy(50, 20, _T("魔塔 · 三章之路"));

    // Subtitle
    settextcolor(RGB(200, 200, 200));
    settextstyle(14, 0, _T("Arial"));
    outtextxy(80, 60, "左键前往关卡节点  ·  Enter 进入关卡  ·  全篇共三关");
}

void drawNode(int x, int y, int id, bool isHighlighted, bool isStart) {
    // Outer glow
    int glowSize = isStart ? 26 : (isHighlighted ? 22 : 18);
    int glowAlpha = isStart ? 100 : (isHighlighted ? 70 : 40);

    for (int r = glowSize; r > 14; r -= 2) {
        int a = glowAlpha * (glowSize - r) / glowSize;
        setfillcolor(RGB(80 + a, 120 + a, 255));
        solidcircle(x, y, r);
    }

    // Main body with pulse
    int baseColor = isStart ? 120 : (60 + (int)(pulse * 80));
    setfillcolor(RGB(baseColor + 20, baseColor + 70, 220));
    solidellipse(x - 14, y - 16, x + 14, y + 16);

    // Highlight/shine
    setfillcolor(RGB(180, 200, 255));
    solidellipse(x - 6, y - 12, x + 5, y - 4);

    // Base/platform
    setfillcolor(RGB(40, 40, 80));
    solidrectangle(x - 12, y + 12, x + 12, y + 16);
    setfillcolor(RGB(60, 60, 110));
    solidrectangle(x - 10, y + 10, x + 10, y + 12);

    // Node ID text
    TCHAR idStr[10];
    _stprintf(idStr, _T("%d"), id);
    settextcolor(WHITE);
    settextstyle(14, 0, _T("Consolas"));
    int tw = textwidth(idStr);
    outtextxy(x - tw / 2, y - 6, idStr);

    // Golden ring for start node
    if (isStart) {
        setlinecolor(RGB(255, 215, 0));
        setlinestyle(PS_SOLID, 3);
        circle(x, y, 20);
        setlinestyle(PS_SOLID, 1);
    }
}

void drawMST() {
    int glow = 100 + (int)(80 * pulse);
    for (size_t i = 0; i < mstEdges.size(); ++i) {
        Edge& e = mstEdges[i];
        Node a = nodes[e.u], b = nodes[e.v];

        // Glow outer line
        setlinecolor(RGB(glow / 3, glow / 3, glow / 2));
        setlinestyle(PS_SOLID, 6);
        line(a.x, a.y, b.x, b.y);

        // Bright inner line
        setlinecolor(RGB(glow, glow, 255));
        setlinestyle(PS_SOLID, 3);
        line(a.x, a.y, b.x, b.y);
    }
    setlinestyle(PS_SOLID, 1);
    setlinecolor(WHITE);
}

void drawMapBackground() {
    setbkcolor(RGB(12, 12, 30));
    cleardevice();

    // Subtle grid
    for (int x = 0; x < SCREEN_WIDTH; x += 50) {
        setlinecolor(RGB(18, 18, 40));
        line(x, 0, x, SCREEN_HEIGHT);
    }
    for (int y = 0; y < SCREEN_HEIGHT; y += 50) {
        setlinecolor(RGB(18, 18, 40));
        line(0, y, SCREEN_WIDTH, y);
    }

    // Decorative borders
    setlinecolor(RGB(60, 60, 140));
    rectangle(5, 5, SCREEN_WIDTH - 5, SCREEN_HEIGHT - 5);
    setlinecolor(RGB(25, 25, 70));
    rectangle(12, 12, SCREEN_WIDTH - 12, SCREEN_HEIGHT - 12);
}

void drawMap() {
    drawMapBackground();
    drawTitle();

    if (showMST) drawMST();

    // Draw edges between nodes
    for (int u = 0; u < (int)nodes.size(); ++u) {
        for (size_t i = 0; i < graph[u].size(); ++i) {
            int v = graph[u][i].first;
            if (u < v) {
                setlinecolor(RGB(40, 40, 70));
                setlinestyle(PS_DOT, 1);
                line(nodes[u].x, nodes[u].y, nodes[v].x, nodes[v].y);
            }
        }
    }
    setlinestyle(PS_SOLID, 1);

    // Draw nodes
    for (int i = 0; i < (int)nodes.size(); ++i) {
        drawNode(nodes[i].x, nodes[i].y, nodes[i].id, false, false);
    }
}

void drawOrb(int x, int y, float progress) {
    (void)progress;

    // Outer glow
    int glowR = 10 + (int)(5 * sin(progress * 6.28f));
    for (int r = glowR + 6; r > glowR; r -= 2) {
        float ratio = (float)(r - glowR) / 6.0f;
        setfillcolor(RGB((int)(255 * ratio), (int)(180 * ratio), (int)(30 * ratio)));
        solidcircle(x, y, r);
    }

    // Main orb body
    setfillcolor(RGB(255, 200, 50));
    solidcircle(x, y, glowR + 2);

    // Inner bright core
    setfillcolor(RGB(255, 255, 220));
    solidcircle(x, y, glowR);

    // Brightest center
    setfillcolor(WHITE);
    solidcircle(x, y, 4);

    // Add trail particles
    for (int i = 0; i < 2; i++) {
        TrailParticle tp;
        tp.x = (float)x + rand() % 8 - 4;
        tp.y = (float)y + rand() % 8 - 4;
        tp.life = 12;
        tp.maxLife = 12;
        trailParticles.push_back(tp);
    }
}

// Draw trail particles
void drawTrails() {
    for (int i = 0; i < (int)trailParticles.size(); ) {
        TrailParticle& tp = trailParticles[i];
        float progress = 1.0f - (float)tp.life / tp.maxLife;
        int alpha = (int)(200 * (1.0f - progress));
        int size = (int)(3 * (1.0f - progress));
        if (size < 1) size = 1;

        setfillcolor(RGB(alpha, alpha / 2, 0));
        solidcircle((int)tp.x, (int)tp.y, size);

        tp.life--;
        tp.y -= 0.5f;

        if (tp.life <= 0)
            trailParticles.erase(trailParticles.begin() + i);
        else
            i++;
    }
}

// Arrival burst effect
void drawArrivalBurst(int x, int y) {
    static int burstLife = 0;
    static int burstX = 0, burstY = 0;

    if (x != -1) {
        burstLife = 25;
        burstX = x;
        burstY = y;
        return;
    }

    if (burstLife <= 0) return;

    float progress = 1.0f - (float)burstLife / 25.0f;
    int ringR = (int)(progress * 60);

    // Expanding rings
    for (int r = ringR; r > ringR - 15; r -= 5) {
        if (r <= 0) continue;
        int alpha = (int)(255 * (1.0f - progress));
        setlinecolor(RGB(alpha, alpha / 2, 0));
        circle(burstX, burstY, r);
    }

    // Burst particles
    for (int i = 0; i < 5; i++) {
        float angle = (float)(rand() % 360) * 3.14159f / 180.0f;
        float dist = progress * 50 + rand() % 10;
        int px = burstX + (int)(cos(angle) * dist);
        int py = burstY + (int)(sin(angle) * dist);
        setfillcolor(RGB(255, 200 - (int)(progress * 150), 0));
        solidcircle(px, py, 2);
    }

    burstLife--;
    if (burstLife < 0) burstLife = 0;
}

// Highlight active path edge
void drawActivePathEdge(int x1, int y1, int x2, int y2, float highlight) {
    // Glowing path edge behind the orb
    int alpha = (int)(200 * highlight);
    setlinecolor(RGB(alpha, alpha / 2, 0));
    setlinestyle(PS_SOLID, 4);
    line(x1, y1, x2, y2);

    // Brighter inner line
    setlinecolor(RGB(alpha, alpha * 3 / 4, alpha / 4));
    setlinestyle(PS_SOLID, 2);
    line(x1, y1, x2, y2);

    setlinestyle(PS_SOLID, 1);
}

// Forward declarations
void drawFlames();
void drawSparkles();

void moveAlongPath(const vector<int>& path, int startNode) {
    for (int i = 0; i + 1 < (int)path.size(); ++i) {
        int u = path[i], v = path[i + 1];
        int x1 = nodes[u].x, y1 = nodes[u].y;
        int x2 = nodes[v].x, y2 = nodes[v].y;

        float dx = (x2 - x1) / 50.0f;
        float dy = (y2 - y1) / 50.0f;
        float x = (float)x1, y = (float)y1;

        for (int step = 0; step <= 50; ++step) {
            drawMap();

            // Highlight the current edge being traversed
            float edgeProgress = (float)step / 50.0f;
            float highlight = 1.0f - abs(edgeProgress - 0.5f) * 2.0f; // Peak at middle
            drawActivePathEdge(x1, y1, x2, y2, highlight);

            // Draw all nodes
            for (int j = 0; j < (int)nodes.size(); ++j) {
                bool isSpecial = (nodes[j].id == startNode || nodes[j].id == v);
                drawNode(nodes[j].x, nodes[j].y, nodes[j].id, isSpecial, nodes[j].id == startNode);
            }

            // Draw the traveling orb with trail
            drawOrb((int)x, (int)y, edgeProgress);
            drawTrails();
            drawFlames();
            drawSparkles();
            FlushBatchDraw();

            x += dx;
            y += dy;
            Sleep(12);
        }

        // Arrival effect at intermediate node
        if (i + 2 < (int)path.size()) {
            drawArrivalBurst(x2, y2);
        }
    }

    // Final arrival burst
    if (!path.empty()) {
        int last = path.back();
        drawArrivalBurst(nodes[last].x, nodes[last].y);
    }
}

void updatePulse() {
    if (increasing) {
        pulse += 0.02f;
        if (pulse >= 1.0f) increasing = false;
    }
    else {
        pulse -= 0.02f;
        if (pulse <= 0.1f) increasing = true;
    }
}

void initGraph() {
    nodes.clear(); allEdges.clear();
    for (int i=0;i<MAXN;++i) graph[i].clear();
    nodes.push_back({0,170,300});
    nodes.push_back({1,370,250});
    nodes.push_back({2,585,315});
    nodes.push_back({3,790,230});
    addEdge(0,1,10); addEdge(1,2,10); addEdge(2,3,10);
}

// ==================== Particle Effects ====================

void drawFlames() {
    // Update and draw existing flames
    for (int i = 0; i < (int)flames.size(); ++i) {
        int r = flames[i].life % 20;
        setfillcolor(RGB(255, 80 + r * 8, 0));
        solidcircle(flames[i].x, flames[i].y, 3 + r / 5);
        flames[i].y -= 1 + rand() % 3;
        flames[i].life--;
    }
    // Remove dead flames
    for (int i = (int)flames.size() - 1; i >= 0; --i) {
        if (flames[i].life <= 0)
            flames.erase(flames.begin() + i);
    }
    // Spawn new flames at bottom
    for (int i = 0; i < 6; ++i) {
        Flame f = { rand() % SCREEN_WIDTH, SCREEN_HEIGHT - 5, 15 + rand() % 25 };
        flames.push_back(f);
    }
}

void drawSparkles() {
    // Update and draw existing sparkles
    for (int i = 0; i < (int)sparkles.size(); ++i) {
        Sparkle& s = sparkles[i];
        float progress = 1.0f - (float)s.life / s.maxLife;
        int alpha = (int)(255 * (1.0f - progress));
        int size = progress < 0.5f ? 2 : 1;
        setfillcolor(RGB(alpha, alpha, 255));
        solidcircle(s.x, s.y, size);
        s.y -= (int)s.speedY;
        s.life--;
    }
    // Remove dead sparkles
    for (int i = (int)sparkles.size() - 1; i >= 0; --i) {
        if (sparkles[i].life <= 0)
            sparkles.erase(sparkles.begin() + i);
    }
    // Spawn new sparkles near nodes
    for (int i = 0; i < (int)nodes.size(); ++i) {
        if (rand() % 100 < 12) {
            Sparkle s = {
                nodes[i].x + rand() % 40 - 20,
                nodes[i].y + rand() % 10 - 15,
                0.3f + (rand() % 10) / 10.0f,
                20 + rand() % 20,
                20 + rand() % 20
            };
            sparkles.push_back(s);
        }
    }
}

// ==================== Main Map Function ====================

int mymap() {

    srand((unsigned)time(0));
    BeginBatchDraw();

    initGraph();

    int start = 0;
    while (1) {
        drawMap();
        for (int i = 0; i < (int)nodes.size(); ++i) {
            if (nodes[i].id == start) {
                drawNode(nodes[i].x, nodes[i].y, nodes[i].id, true, true);
            }
        }
        drawFlames();
        drawSparkles();
        drawTrails();
        drawArrivalBurst(-1, -1);  // Draw lingering burst effect
        updatePulse();

        if (MouseHit()) {
            MOUSEMSG msg = GetMouseMsg();
            if (msg.uMsg == WM_LBUTTONDOWN) {
                int end = getClickedNode(msg.x, msg.y);
                if (end != -1 && end != start) {
                    vector<int> path = dijkstra(start, end);
                    moveAlongPath(path, start);
                    start = end;
                    showMST = false;
                }
            }
            else if (msg.uMsg == WM_RBUTTONDOWN) {
                int target = getClickedNode(msg.x, msg.y);
                if (target != -1 && target != start) {
                    kruskal(start, target);
                }
            }
        }

        if (_kbhit()) {
            int ch = _getch();
            if (ch == 13) {
                if (start != 0) {
                    EndBatchDraw();
                    return start;
                }
            }
        }

        FlushBatchDraw();
        Sleep(20);
    }
    EndBatchDraw();
    return 0;
}
