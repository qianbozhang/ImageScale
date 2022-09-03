#include "nearest/nearest.h"
#include <stdio.h>
#include <cstring>
#include <math.h>


int convert_nearest(const ImgBuffer in, ImgBuffer* out)
{
    printf("InBuf = %p, InWidth = %d, InHeight = %d. \n", in.buf, in.w, in.h);
    printf("OutBuf = %p, OutWidth = %d, OutHeight = %d. \n", out->buf, out->w, out->h);
    Fracetion_t w_scale = { 0, 0 }, h_scale = { 0, 0 };
    double sw= 1.0 * in.w / out->w;
    double sh= 1.0 * in.h / out->h;

    printf("nearest: width scale(%f) , height scale(%f) \n", sw, sh);

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

    int srcX = 0, srcY = 0;
    for(int i = 0; i < out->h; i ++)
    {
        for (int j = 0; j < out->w; j ++)
        {
            srcX = round(i * 1.0 * in.h / out->h);
            srcY = round(j * 1.0 * in.w / out->w);
            out_Arr[i][j].r = in_Arr[srcX][srcY].r;
            out_Arr[i][j].g = in_Arr[srcX][srcY].g;
            out_Arr[i][j].b = in_Arr[srcX][srcY].b;
        }
    }

    memcpy(out->buf, out_Arr, out->w * out->h * 3);

    return 0;
}