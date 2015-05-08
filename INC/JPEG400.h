#ifndef _JPEG400_H_
#define _JPEG400_H_
    typedef unsigned int UINT32;
    typedef int    INT32;
    typedef unsigned short UINT16;
    typedef short   INT16;
    typedef unsigned char UINT8;
    typedef char   INT8;
    UINT32 encode_image(UINT8 * input_ptr, UINT8 * output_ptr, UINT32 quality_factor, UINT32 image_format, UINT32 image_width, UINT32 image_height);

#endif