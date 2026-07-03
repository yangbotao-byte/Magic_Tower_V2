#include<graphics.h>
//#include"allinclude.h"

// Render PNG image with alpha transparency blending
// Uses the formula: Cp = ap*FP + (1-ap)*BP (Porter-Duff over operator)
void putimagePNG(int  picture_x, int picture_y, IMAGE* picture)
{
    DWORD* dst = GetImageBuffer();
    DWORD* draw = GetImageBuffer();
    DWORD* src = GetImageBuffer(picture);
    int picture_width = picture->getwidth();
    int picture_height = picture->getheight();
    int graphWidth = getwidth();
    int graphHeight = getheight();
    int dstX = 0;

    // Per-pixel alpha blending
    for (int iy = 0; iy < picture_height; iy++)
    {
        for (int ix = 0; ix < picture_width; ix++)
        {
            int srcX = ix + iy * picture_width;
            int sa = ((src[srcX] & 0xff000000) >> 24); // Alpha channel
            int sr = ((src[srcX] & 0xff0000) >> 16);   // Red
            int sg = ((src[srcX] & 0xff00) >> 8);       // Green
            int sb = src[srcX] & 0xff;                  // Blue
            if (ix >= 0 && ix <= graphWidth && iy >= 0 && iy <= graphHeight && dstX <= graphWidth * graphHeight)
            {
                dstX = (ix + picture_x) + (iy + picture_y) * graphWidth;
                int dr = ((dst[dstX] & 0xff0000) >> 16);
                int dg = ((dst[dstX] & 0xff00) >> 8);
                int db = dst[dstX] & 0xff;
                // Blend: result = src * alpha + dst * (1 - alpha)
                draw[dstX] = ((sr * sa / 255 + dr * (255 - sa) / 255) << 16)
                    | ((sg * sa / 255 + dg * (255 - sa) / 255) << 8)
                    | (sb * sa / 255 + db * (255 - sa) / 255);
            }
        }
    }
}

// putimagePNG2 with y<0 clipping support
void putimagePNG2(int x, int y, IMAGE* picture) {
    IMAGE imgTmp;
    if (y < 0) {
        SetWorkingImage(picture);
        getimage(&imgTmp, 0, -y,
            picture->getwidth(), picture->getheight() + y);
        SetWorkingImage();
        y = 0;
        picture = &imgTmp;
    }

    if (x < 0) {
        SetWorkingImage(picture);
        getimage(&imgTmp, -x, 0, picture->getwidth() + x, picture->getheight());
        SetWorkingImage();
        x = 0;
        picture = &imgTmp;
    }

    putimagePNG(x, y, picture);
}

// putimagePNG2 with window width clipping
void putimagePNG2(int x, int y, int winWidth, IMAGE* picture) {
    IMAGE imgTmp;
    if (y < 0) {
        SetWorkingImage(picture);
        getimage(&imgTmp, 0, -y,
            picture->getwidth(), picture->getheight() + y);
        SetWorkingImage();
        y = 0;
        picture = &imgTmp;
    }

    if (x < 0) {
        SetWorkingImage(picture);
        getimage(&imgTmp, -x, 0, picture->getwidth() + x, picture->getheight());
        SetWorkingImage();
        x = 0;
        picture = &imgTmp;
    }
    else if (x >= winWidth) {
        return;
    }
    else if (x > winWidth - picture->getwidth()) {
        SetWorkingImage(picture);
        getimage(&imgTmp, 0, 0, winWidth - x, picture->getheight());
        SetWorkingImage();
        picture = &imgTmp;
    }

    putimagePNG(x, y, picture);
}

// Scale image to new size (nearest-neighbor)
void scaleImage(IMAGE* dst, IMAGE* src, int newW, int newH) {
    int oldW = src->getwidth();
    int oldH = src->getheight();
    if (oldW <= 0 || oldH <= 0) return;
    Resize(dst, newW, newH);
    DWORD* srcBuf = GetImageBuffer(src);
    DWORD* dstBuf = GetImageBuffer(dst);
    for (int y = 0; y < newH; y++) {
        int srcY = y * oldH / newH;
        for (int x = 0; x < newW; x++) {
            int srcX = x * oldW / newW;
            dstBuf[y * newW + x] = srcBuf[srcY * oldW + srcX];
        }
    }
}
