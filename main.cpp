#include <iostream>
#include <stdio.h>
#include <cstring>
#include <stdlib.h>

#include "img_type.h"
#include "bilinear/bilinear.h"
#include "nearest/nearest.h"
#include "bicubic/bicubic.h"
using namespace std;


int greatest_common_divisor(int a, int b)
{
    int temp = 0;
    while(b != 0)
    {
        temp = a;

        a = b;
        b = temp % b;
    }
    return abs(a);
}

void convert_to_Arr(ImgRGB** out_Arr, const ImgBuffer in)
{
    for(int i = 0; i < in.h; i ++)
    {
        for(int j = 0; j < in.w; j++)
        {
            out_Arr[i][j].r = in.buf[i * 3 + j];
            out_Arr[i][j].g = in.buf[i * 3 + j + 1];
            out_Arr[i][j].b = in.buf[i * 3 + j + 2];
        }
    }
}

int open_input_default(std::string path, ImgBuffer* in)
{
    if( in == NULL) {
        return -1;
    }
    
    in->w = 1920;
    in->h = 1080;
    //for RGB
    int size = in->w * in->h * 3;
    in->buf = (UInt8*) malloc (sizeof(UInt8) * size);
    FILE *fp = fopen(path.c_str(), "rb");
    if(fp)
    {
        fread(in->buf, 1, size, fp);
        fclose(fp);
    }

    printf("readed RGB %p(%d x %d) from %s. \n", in->buf, in->w, in->h, path.c_str());

    return 0;
}

int save_output_default(std::string path, const ImgBuffer out, ScaleMethod sm)
{
    if(out.w <= 0 || out.h <= 0 || out.buf == NULL) {
        printf("invalid buffer \n");
        return -1;
    }

    printf("saveBuf = %p, Width = %d, Height = %d. \n", out.buf, out.w, out.h);
    char p[128];
    snprintf(p, 128, "%d_%dx%d_%s", sm, out.w, out.h, path.c_str());

    FILE *fp = fopen(p, "wb");
    if(fp) {
        fwrite(out.buf, 1, out.w * out.h * 3, fp);
        fclose(fp);
    } else {
        printf("open file fail. \n");
    }

    return 0;
}

int convert_default(const ImgBuffer in, ImgBuffer* out)
{
    printf("InBuf = %p, InWidth = %d, InHeight = %d. \n", in.buf, in.w, in.h);
    printf("converting OutBuf = %p, OutWidth = %d, OutHeight = %d. \n", out->buf, out->w, out->h);

    int size = out->w * out->h * 3;
    if(out->buf) {
        free(out->buf);
        out->buf = NULL;
    }

    out->buf = (UInt8*) malloc ( sizeof(UInt8) * size);
    if(out->buf) {
        printf("memcpy buffer \n");
        memcpy(out->buf, in.buf, size);
    }

    printf("converting OutBuf = %p, OutWidth = %d, OutHeight = %d. \n", out->buf, out->w, out->h);

    return 0;
}





int main(int argc, char const *argv[])
{
    /* code */
    ImgContext_t imgc;
    ScaleMethod sm = SCALE_UNKNOWN;

    memset(&imgc, 0, sizeof(ImgContext_t));
    imgc.open_input = open_input_default;
    imgc.save_output = save_output_default;

    //default

    if(argc >= 4){
        sm = (ScaleMethod)atoi(argv[1]);
        imgc.out.w = atoi(argv[2]);
        imgc.out.h = atoi(argv[3]);
    }else{
        printf("example:./output/ImgScale method[1:nearest 2:bilinear 3:bicubic] w h \n");
        return -1;
    }

    //setup 1: read file
    printf(">>>>>>>>>>>>> read file >>>>>>>>>>> \n");
    imgc.open_input("input.rgb", &imgc.in);

    // setup 2: convert
    printf(">>>>>>>>>>>>> convert >>>>>>>>>>>>> \n");
    switch (sm)
    {
        case SCALE_NEAREST:
        {
            imgc.convert = convert_nearest;
            break;
        }
        case SCALE_BILINEAR:
        {
            imgc.convert = convert_bilinear;
            break;
        }
        case SCALE_BICUBIC:
        {
            imgc.convert = convert_bicubic;
            break;
        }
        default:
        {
            imgc.convert = convert_default;
            imgc.out.w = imgc.in.w;
            imgc.out.h = imgc.in.h;
            break;
        }
    }

    imgc.convert(imgc.in, &imgc.out);

    //sutep 3: save buf
    printf(">>>>>>>>>>>>> save file >>>>>>>>>>> \n");
    imgc.save_output("outBuf.rgb", imgc.out, sm);



    //free
    if(imgc.in.buf){
        free(imgc.in.buf);
        imgc.in.buf = NULL;
    }

    if(imgc.out.buf){
        free(imgc.out.buf);
        imgc.out.buf = NULL;
    }

    return 0;
}
