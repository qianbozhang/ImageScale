#include "bicubic/bicubic.h"
#include <stdio.h>
#include <cstring>
#include <math.h>

float W_x(float x)
{
    float a = -0.5;
    float x_abs = abs(x);

    if(x_abs <= 1) {
        return (a + 2) * pow(x_abs, 3) - (a + 3) * pow(x_abs, 2) + 1;
    } else if(x_abs > 1 && x_abs < 2) {
        return a * pow(x_abs, 3) - 5 * a * pow(x_abs, 2) + 8 * a * x_abs - 4 * a;
    }else {
        return 0;
    }  
}


ImgRGB pointX(ImgRGB pixs[4][4], float w_x[4], float w_y[4])
{
    float f_r = 0.0, f_g = 0.0, f_b = 0.0;
    for(int k = 0; k < 3; k++){
        for(int i = 0; i < 4; i ++)
            for(int j = 0; j < 4; j++ ) {
                if(k == 0)
                    f_r += pixs[i][j].r * w_x[i] * w_y[j];
                else if (k == 1)
                    f_g += pixs[i][j].g * w_x[i] * w_y[j];
                else if (k == 2)
                    f_b += pixs[i][j].b * w_x[i] * w_y[j];
            }
    }
    ImgRGB result = {.r = (int)f_r, .g = (int)f_g, .b = (int)f_b};
    
    return result;
}

int convert_bicubic(const ImgBuffer in, ImgBuffer* out)
{
    printf("InBuf = %p, InWidth = %d, InHeight = %d. \n", in.buf, in.w, in.h);
    printf("OutBuf = %p, OutWidth = %d, OutHeight = %d. \n", out->buf, out->w, out->h);
    Fracetion_t w_scale = { 0, 0 }, h_scale = { 0, 0 };
    int _w = greatest_common_divisor(in.w, out->w);
    int _h = greatest_common_divisor(in.h, out->h);
    w_scale.num = in.w / _w;
    w_scale.den = out->w / _w;

    h_scale.num = in.h / _h;
    h_scale.den = out->h / _h;

    printf("bicubic: width scale(%d / %d) , height scale(%d / %d) \n", w_scale.num, w_scale.den, h_scale.num, h_scale.den);

    //malloc
    int size = out->w * out->h * 3;
    if(out->buf) {
        free(out->buf);
        out->buf = NULL;
    }

    out->buf = (UInt8*) malloc ( sizeof(UInt8) * size);
    memset(out->buf, 0, size);
    //input array
    ImgRGB in_Arr[in.h][in.w];
    memcpy(in_Arr, in.buf, in.w * in.h * 3);
    //output array
    ImgRGB out_Arr[out->h][out->w];

    double srcX = 0.0, srcY = 0.0;
    typedef struct _q
    {
        int x;
        int y;
    }Q;
    Q q[4][4];
    int count = 0;

    float w_x[4] = {0};
    float w_y[4] = {0};
    ImgRGB p[4][4] = {0};
    float u = 0, v = 0;
    int x0, x1, x2, x3, y0, y1, y2, y3;

    for(int oi = 0; oi < out->h; oi ++)
    {
        for (int oj = 0; oj < out->w; oj ++)
        {
            srcX = oi * 1.0 * in.h / out->h;//行
            srcY = oj * 1.0 * in.w / out->w;//列

            if(srcX <= 1 || srcX >= in.w - 2 || srcY <= 1 || srcY >= in.h -2) {
                out_Arr[oi][oj] = in_Arr[(int)srcX][(int)srcY];
            } else {
                u = srcX - (int)srcX;
                v = srcY - (int)srcY;
                x0 = (int)srcX - 1;   x1 = (int)srcX;   x2 = ceil(srcX);   x3 = ceil(srcX) + 1;
                y0 = (int)srcY - 1;   y1 = (int)srcY;   y2 = ceil(srcY);   y3 = ceil(srcY) + 1;
                w_x[0] = W_x(1 + u); w_x[1] = W_x(u);  w_x[2] = W_x(1 - u);  w_x[3] = W_x(2 - u);
                w_y[0] = W_x(1 + v); w_y[1] = W_x(v);  w_y[2] = W_x(1 - v);  w_y[3] = W_x(2 - v);

                //pixs
                p[0][0] = in_Arr[x0][y0];    p[0][1] = in_Arr[x0][y1];  p[0][2] = in_Arr[x0][y2];  p[0][3] = in_Arr[x0][y3];
                p[1][0] = in_Arr[x1][y0];    p[1][1] = in_Arr[x1][y1];  p[1][2] = in_Arr[x1][y2];  p[1][3] = in_Arr[x1][y3];
                p[2][0] = in_Arr[x2][y0];    p[2][1] = in_Arr[x2][y1];  p[2][2] = in_Arr[x2][y2];  p[2][3] = in_Arr[x2][y3];
                p[3][0] = in_Arr[x3][y0];    p[3][1] = in_Arr[x3][y1];  p[3][2] = in_Arr[x3][y2];  p[3][3] = in_Arr[x3][y3];

                 out_Arr[oi][oj] = pointX(p, w_x, w_y);
            } //else
        } //for
    }

    memcpy(out->buf, out_Arr, out->w * out->h * 3);


    return 0;
}