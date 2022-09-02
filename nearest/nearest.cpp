#include "nearest/nearest.h"
#include <stdio.h>
#include <cstring>



int convert_nearest(const ImgBuffer in, ImgBuffer* out)
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

    int srcX = 0, srcY = 0;
    for(int i = 0; i < out->h; i ++)
    {
        for (int j = 0; j < out->w * 3; j ++)
        {
            srcX = i * w_scale.num / w_scale.den;
            srcY = j * h_scale.num / h_scale.den;
            out_Arr[i][j] = in_Arr[srcX][srcY];
        }
    }

    memcpy(out->buf, out_Arr, out->w * out->h * 3);

    return 0;
}