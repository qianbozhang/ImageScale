#include "bilinear/bilinear.h"
#include <stdio.h>
#include <cstring>
#include <math.h>



static double calculate_point(int x1, int x2, double x, UInt8 pix1, UInt8 pix2)
{
    return pix1 * (x2 - x) / (x2 - x1) + pix2 * (x - x1) / (x2 - x1);
}

int convert_bilinear(const ImgBuffer in, ImgBuffer* out)
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

    printf("bilinear: width scale(%d / %d) , height scale(%d / %d) \n", w_scale.num, w_scale.den, h_scale.num, h_scale.den);

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

    //重复线性插值
    typedef struct _q
    {
        int x;
        int y;
    }Q;

    /*
     * |q11(x1,y1)------R1(x,y1)--------q12(x2,y1)|
     * |-------------------|----------------------|
     * |----------------p(x,y)--------------------|
     * |-------------------|----------------------|
     * |q21(x1,y2)------R2(x,y2)--------q22(x2,y2)|
    */
    
    double srcX = 0.0, srcY = 0.0;
    Q q11, q12, q21, q22;
    double R1r, R1g, R1b, R2r, R2g, R2b;
    for(int i = 0; i < out->h; i ++)
    {
        for (int j = 0; j < out->w; j ++)
        {
            srcX = i * 1.0 * in.h / out->h;
            srcY = j * 1.0 * in.w / out->w;

            if((int)srcX == ceil(srcX) && (int)srcY == ceil(srcY))
            {
                 out_Arr[i][j] = in_Arr[(int)srcX][(int)srcY];
            } else if((int)srcX == ceil(srcX)) {
                //setup 1: find q11,q21
                q11.x = (int) srcX;  q11.y = (int) srcY;
                q21.x = (int) srcX;  q21.y = ceil(srcY);
                //setup 2: calculate p
                out_Arr[i][j].r = calculate_point(q11.y, q21.y, srcY, in_Arr[q11.x][q11.y].r, in_Arr[q21.x][q21.y].r);
                out_Arr[i][j].g = calculate_point(q11.y, q21.y, srcY, in_Arr[q11.x][q11.y].g, in_Arr[q21.x][q21.y].g);
                out_Arr[i][j].b = calculate_point(q11.y, q21.y, srcY, in_Arr[q11.x][q11.y].b, in_Arr[q21.x][q21.y].b);
            } else if((int)srcY == ceil(srcY)) {
                //setup 1: find q11,q12
                q11.x = (int) srcX;  q11.y = (int) srcY;
                q12.x = ceil (srcX); q12.y = (int) srcY;
                //setup 2: calculate p
                out_Arr[i][j].r = calculate_point(q11.x, q12.x, srcX, in_Arr[q11.x][q11.y].r, in_Arr[q12.x][q12.y].r);
                out_Arr[i][j].g = calculate_point(q11.x, q12.x, srcX, in_Arr[q11.x][q11.y].g, in_Arr[q12.x][q12.y].g);
                out_Arr[i][j].b = calculate_point(q11.x, q12.x, srcX, in_Arr[q11.x][q11.y].b, in_Arr[q12.x][q12.y].b);
            } else {
                 //setup 1: find q11,q12,q21,q22
                q11.x = (int) srcX;  q11.y = (int) srcY;
                q12.x = ceil (srcX); q12.y = (int) srcY;
                q21.x = (int) srcX;  q21.y = ceil(srcY);
                q22.x = ceil (srcX); q22.y = ceil(srcY);
                // printf("q11(%d, %d), q12(%d, %d), q21(%d, %d), q22(%d, %d), P(%f, %f) \n", q11.x, q11.y, q12.x, q12.y, q21.x, q21.y, q22.x, q22.y, srcX, srcY);

                //setup 2:calculate R1, R2
                R1r = calculate_point(q11.x, q12.x, srcX, in_Arr[q11.x][q11.y].r, in_Arr[q12.x][q12.y].r);
                R1g = calculate_point(q11.x, q12.x, srcX, in_Arr[q11.x][q11.y].g, in_Arr[q12.x][q12.y].g);
                R1b = calculate_point(q11.x, q12.x, srcX, in_Arr[q11.x][q11.y].b, in_Arr[q12.x][q12.y].b);
                R2r = calculate_point(q21.x, q22.x, srcX, in_Arr[q21.x][q21.y].r, in_Arr[q22.x][q22.y].r);
                R2g = calculate_point(q21.x, q22.x, srcX, in_Arr[q21.x][q21.y].g, in_Arr[q22.x][q22.y].g);
                R2b = calculate_point(q21.x, q22.x, srcX, in_Arr[q21.x][q21.y].b, in_Arr[q22.x][q22.y].b);

                //setup 3: calculate p
                out_Arr[i][j].r = calculate_point(q12.y, q22.y, srcY, R1r, R2r);
                out_Arr[i][j].g = calculate_point(q12.y, q22.y, srcY, R1g, R2g);
                out_Arr[i][j].b = calculate_point(q12.y, q22.y, srcY, R1b, R2b);
            }
        }
    }

    memcpy(out->buf, out_Arr, out->w * out->h * 3);
    return 0;
}