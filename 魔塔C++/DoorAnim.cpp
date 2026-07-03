#include"allinclude.h"
IMAGE men[48];
IMAGE menScaled[48];
IMAGE men1[113];
IMAGE men1Scaled[113];

// First door opening animation (48 frames)
void updatemen() {
    int newW = _WINDOW_WIDTH__;
    int newH = _WINDOW_LONG__;

    for (int i = 0; i < 48; ++i) {
        char path[64];
        sprintf_s(path, "./set/kai/frame_%02d_delay-0.08s.png", i + 1);
        loadimage(&men[i], path);
        scaleImage(&menScaled[i], &men[i], newW, newH);
    }

    mciSendString("stop doorMain", 0, 0, 0);
    mciSendString("play doorMain from 0", 0, 0, 0);

    for (int i = 0; i < 48; i++) {
        putimage(0, 0, &menScaled[i]);
        Sleep(100);
        cleardevice();
    }
}

// Second door opening animation (113 frames)
void updatemen1() {
    int newW = _WINDOW_WIDTH__;
    int newH = _WINDOW_LONG__;

    for (int i = 0; i < 113; ++i) {
        char path[64];
        sprintf_s(path, "./set/kai_1/frame_%03d_delay-0.1s.png", i + 1);
        loadimage(&men1[i], path);
        scaleImage(&men1Scaled[i], &men1[i], newW, newH);
    }

    mciSendString("stop doorLevel", 0, 0, 0);
    mciSendString("play doorLevel from 0", 0, 0, 0);

    for (int i = 0; i < 113; i++) {
        putimage(0, 0, &men1Scaled[i]);
        Sleep(100);
        cleardevice();
    }
}
