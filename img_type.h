#ifndef _IMG_TYPE_H_
#define _IMG_TYPE_H_

#include <string>
typedef unsigned char  UInt8;

typedef struct _buffer
{
    int w;
    int h;
    UInt8 *buf;
}ImgBuffer;

typedef struct _fraction
{
    int num;
    int den;
}Fracetion_t;



typedef struct _imgContext
{
    ImgBuffer in;
    ImgBuffer out;

    // funcs
    int (*open_input)       (std::string path, ImgBuffer* in);
    int (*convert)          (const ImgBuffer in, ImgBuffer* out);
    int (*save_output)      (std::string path, const ImgBuffer out);

}ImgContext_t;

typedef enum
{
    SCALE_UNKNOWN = 0,
    SCALE_NEAREST,
    SCALE_BILINEAR,
    SCALE_BICUBIC
} ScaleMethod;


/**
 * @brief: find greatest common divisor
 * 
 * @param [in] a: the first value
 * @param [in] b: the second value
 *
 * @return:
 *   scale result
 */
int greatest_common_divisor(int a, int b);


#endif