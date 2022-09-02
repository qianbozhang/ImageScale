#include "bilinear/bilinear.h"
#include <stdio.h>
#include <cstring>

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

    printf("nearest: width scale(%d / %d) , height scale(%d / %d) \n", w_scale.num, w_scale.den, h_scale.num, h_scale.den);

    //malloc
    int size = out->w * out->h * 3;
    if(out->buf) {
        free(out->buf);
        out->buf = NULL;
    }

    out->buf = (UInt8*) malloc ( sizeof(UInt8) * size);
    memset(out->buf, 0, size);
    //input array
    UInt8 in_Arr[in.h][in.w * 3] = {0};
    memcpy(in_Arr, in.buf, in.w * in.h * 3);
    //output array
    UInt8 out_Arr[out->h][out->w * 3] = {0};

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
    for(int i = 0; i < out->h; i ++)
    {
        for (int j = 0; j < out->w * 3; j ++)
        {
            srcX = (double)(i * w_scale.num / w_scale.den);
            srcY = (double)(j * h_scale.num / h_scale.den);

            //setup 1: find q11,q12,q21,q22
            q11.x = (int) srcX; q11.y = (int) srcY;
            q12.x = ceil(srcX); 
            //setup 2:calculate R1, R2

            //setup 3: calculate p

            // out_Arr[i][j] = in_Arr[srcX][srcY];
        }
    }

    memcpy(out->buf, out_Arr, out->w * out->h * 3);
    return 0;
}