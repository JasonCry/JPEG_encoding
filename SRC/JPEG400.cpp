    // JPEG400.cpp : 定义控制台应用程序的入口点。
    //
#include "../INC/JPEG400.h"    
    #define  BLOCK_SIZE 64  
    #define CLIP(color) (unsigned char)(((color)>0xFF)?0xff:(((color)<0)?0:(color)))
    //编码中所要用到的与图像相关的参数的结构体
    typedef struct JPEG_ENCODER_STRUCTURE
    {
        UINT16 mcu_width;//最小编码单元的宽度
        UINT16 mcu_height;//最小编码单元的高度
        UINT16 horizontal_mcus;//输入图像所划分的最小编码单元在列方向(水平)上的个数
        UINT16 vertical_mcus;//输入图像所划分的最小编码单元在行方向上的个数
        UINT16 rows_in_bottom_mcus;
        UINT16 cols_in_right_mcus;
        UINT16 length_minus_mcu_width;//输入图像宽度-最小编码单元的宽度
        UINT16 length_minus_width;//输入图像的高度-最小编码单元的高度
        UINT16 mcu_width_size;
        UINT16 offset;
        INT16 ldc1;
        INT16 ldc2;
        INT16 ldc3;
        UINT16 rows;
        UINT16 cols;
        UINT16 incr;
    } JPEG_ENCODER_STRUCTURE;


    static void(*read_format) (JPEG_ENCODER_STRUCTURE * jpeg_encoder_structure, UINT8 * input_ptr);
    //////////////////////////////////////////////////////////////////////////
    UINT8  Lqt[BLOCK_SIZE];
    UINT8  Cqt[BLOCK_SIZE];
    UINT16  ILqt[BLOCK_SIZE];
    UINT16  ICqt[BLOCK_SIZE];
    INT16  Y1[BLOCK_SIZE];
    INT16  Y2[BLOCK_SIZE];
    INT16  Y3[BLOCK_SIZE];
    INT16  Y4[BLOCK_SIZE];
    INT16  CB[BLOCK_SIZE];
    INT16  CR[BLOCK_SIZE];
    INT16  Temp[BLOCK_SIZE];
    INT32  lcode;
    UINT16  bitindex;
    static UINT8 zigzag_table[] = {
    0, 1, 5, 6, 14, 15, 27, 28, 2, 4, 7, 13, 16, 26, 29, 42, 3, 8, 12, 17,
    25, 30, 41, 43, 9, 11, 18, 24, 31, 40, 44, 53, 10, 19, 23, 32, 39, 45, 52,
    54, 20, 22, 33, 38, 46, 51, 55, 60, 21, 34, 37, 47, 50, 56, 59, 61, 35, 36,
    48, 49, 57, 58, 62, 63
    };
    /*   //原始版本
    // Header for JPEG Encoder
    static UINT16 markerdata[] = {
    // dht
    0xFFC4, 0x1A2, 0x00,
    // luminance dc (2 - 16) + 1
    0x0105, 0x0101, 0x00101, 0x0101, 0x0000, 0x00000, 00000, 00000,
    // luminance dc (2 - 12) + 1
    0x0102, 0x0304, 0x0506, 0x0708, 0x090A, 0x0B01,
    // chrominance dc (1 - 16)
    0x0003, 0x0101, 0x0101, 0x0101, 0x0101, 0x0100, 0x0000, 0x0000,
    // chrominance dc (1 - 12)
    0x0001, 0x00203, 0x0405, 0x0607, 0x0809, 0x00A0B,
    // luminance ac 1 + (1 - 15)
    0x1000, 0x0201, 0x0303, 0x0204, 0x0305, 0x0504, 0x0400, 0x0001,
    // luminance ac 1 + (1 - 162) + 1
    0x7D01, 0x0203, 0x0004, 0x1105, 0x1221, 0x3141, 0x0613, 0x5161, 0x0722,
    0x7114, 0x3281, 0x91A1, 0x0823, 0x42B1, 0xC115, 0x52D1, 0xF024, 0x3362,
    0x7282, 0x090A, 0x1617, 0x1819, 0x1A25, 0x2627, 0x2829, 0x2A34, 0x3536,
    0x3738, 0x393A, 0x4344, 0x4546, 0x4748, 0x494A, 0x5354, 0x5556, 0x5758,
    0x595A, 0x6364, 0x6566, 0x6768, 0x696A, 0x7374, 0x7576, 0x7778, 0x797A,
    0x8384, 0x8586, 0x8788, 0x898A, 0x9293, 0x9495, 0x9697, 0x9899, 0x9AA2,
    0xA3A4, 0xA5A6, 0xA7A8, 0xA9AA, 0xB2B3, 0xB4B5, 0xB6B7, 0xB8B9, 0xBAC2,
    0xC3C4, 0xC5C6, 0xC7C8, 0xC9CA, 0xD2D3, 0xD4D5, 0xD6D7, 0xD8D9, 0xDAE1,
    0xE2E3, 0xE4E5, 0xE6E7, 0xE8E9, 0xEAF1, 0xF2F3, 0xF4F5, 0xF6F7, 0xF8F9,
    0xFA11,
    // chrominance ac (1 - 16)
    0x0002, 0x0102, 0x0404, 0x0304, 0x0705, 0x0404, 0x0001, 0x0277,
    // chrominance ac (1 - 162)
    0x0001, 0x0203, 0x1104, 0x0521, 0x3106, 0x1241, 0x5107, 0x6171, 0x1322,
    0x3281, 0x0814, 0x4291, 0xA1B1, 0xC109, 0x2333, 0x52F0, 0x1562, 0x72D1,
    0x0A16, 0x2434, 0xE125, 0xF117, 0x1819, 0x1A26, 0x2728, 0x292A, 0x3536,
    0x3738, 0x393A, 0x4344, 0x4546, 0x4748, 0x494A, 0x5354, 0x5556, 0x5758,
    0x595A, 0x6364, 0x6566, 0x6768, 0x696A, 0x7374, 0x7576, 0x7778, 0x797A,
    0x8283, 0x8485, 0x8687, 0x8889, 0x8A92, 0x9394, 0x9596, 0x9798, 0x999A,
    0xA2A3, 0xA4A5, 0xA6A7, 0xA8A9, 0xAAB2, 0xB3B4, 0xB5B6, 0xB7B8, 0xB9BA,
    0xC2C3, 0xC4C5, 0xC6C7, 0xC8C9, 0xCAD2, 0xD3D4, 0xD5D6, 0xD7D8, 0xD9DA,
    0xE2E3, 0xE4E5, 0xE6E7, 0xE8E9, 0xEAF2, 0xF3F4, 0xF5F6, 0xF7F8, 0xF9FA
    };
    */
    // Header for JPEG Encoder
    static UINT16 markerdata[] = {
    // dht
    0xFFC4, 0x1A2, 0x00,
    // luminance dc (2 - 16) + 1
    0x0105, 0x0101, 0x00101, 0x0101, 0x0000, 0x00000, 00000, 00000,
    // luminance dc (2 - 12) + 1
    0x0102, 0x0304, 0x0506, 0x0708, 0x090A, 0x0B01,
    // chrominance dc (1 - 16)
    0x0003, 0x0101, 0x0101, 0x0101, 0x0101, 0x0100, 0x0000, 0x0000,
    // chrominance dc (1 - 12)
    0x0001, 0x00203, 0x0405, 0x0607, 0x0809, 0x00A0B,
    // luminance ac 1 + (1 - 15)
    0x1000, 0x0201, 0x0303, 0x0204, 0x0305, 0x0504, 0x0400, 0x0001,
    // luminance ac 1 + (1 - 162) + 1
    0x7D01, 0x0203, 0x0004, 0x1105, 0x1221, 0x3141, 0x0613, 0x5161, 0x0722,
    0x7114, 0x3281, 0x91A1, 0x0823, 0x42B1, 0xC115, 0x52D1, 0xF024, 0x3362,
    0x7282, 0x090A, 0x1617, 0x1819, 0x1A25, 0x2627, 0x2829, 0x2A34, 0x3536,
    0x3738, 0x393A, 0x4344, 0x4546, 0x4748, 0x494A, 0x5354, 0x5556, 0x5758,
    0x595A, 0x6364, 0x6566, 0x6768, 0x696A, 0x7374, 0x7576, 0x7778, 0x797A,
    0x8384, 0x8586, 0x8788, 0x898A, 0x9293, 0x9495, 0x9697, 0x9899, 0x9AA2,
    0xA3A4, 0xA5A6, 0xA7A8, 0xA9AA, 0xB2B3, 0xB4B5, 0xB6B7, 0xB8B9, 0xBAC2,
    0xC3C4, 0xC5C6, 0xC7C8, 0xC9CA, 0xD2D3, 0xD4D5, 0xD6D7, 0xD8D9, 0xDAE1,
    0xE2E3, 0xE4E5, 0xE6E7, 0xE8E9, 0xEAF1, 0xF2F3, 0xF4F5, 0xF6F7, 0xF8F9,
    0xFA11,
    // chrominance ac (1 - 16)
    0x0002, 0x0102, 0x0404, 0x0304, 0x0705, 0x0404, 0x0001, 0x0277,
    // chrominance ac (1 - 162)
    0x0001, 0x0203, 0x1104, 0x0521, 0x3106, 0x1241, 0x5107, 0x6171, 0x1322,
    0x3281, 0x0814, 0x4291, 0xA1B1, 0xC109, 0x2333, 0x52F0, 0x1562, 0x72D1,
    0x0A16, 0x2434, 0xE125, 0xF117, 0x1819, 0x1A26, 0x2728, 0x292A, 0x3536,
    0x3738, 0x393A, 0x4344, 0x4546, 0x4748, 0x494A, 0x5354, 0x5556, 0x5758,
    0x595A, 0x6364, 0x6566, 0x6768, 0x696A, 0x7374, 0x7576, 0x7778, 0x797A,
    0x8283, 0x8485, 0x8687, 0x8889, 0x8A92, 0x9394, 0x9596, 0x9798, 0x999A,
    0xA2A3, 0xA4A5, 0xA6A7, 0xA8A9, 0xAAB2, 0xB3B4, 0xB5B6, 0xB7B8, 0xB9BA,
    0xC2C3, 0xC4C5, 0xC6C7, 0xC8C9, 0xCAD2, 0xD3D4, 0xD5D6, 0xD7D8, 0xD9DA,
    0xE2E3, 0xE4E5, 0xE6E7, 0xE8E9, 0xEAF2, 0xF3F4, 0xF5F6, 0xF7F8, 0xF9FA
    };

    UINT8 bitsize[] = {
    0, 1, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5,
    5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
    6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
    8, 8
    };

    UINT16 luminance_dc_code_table[] = {
    0x0000, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x000E, 0x001E, 0x003E,
    0x007E, 0x00FE, 0x01FE
    };

    UINT16 luminance_dc_size_table[] = {
    0x0002, 0x0003, 0x0003, 0x0003, 0x0003, 0x0003, 0x0004, 0x0005, 0x0006,
    0x0007, 0x0008, 0x0009
    };

    UINT16 chrominance_dc_code_table[] = {
    0x0000, 0x0001, 0x0002, 0x0006, 0x000E, 0x001E, 0x003E, 0x007E, 0x00FE,
    0x01FE, 0x03FE, 0x07FE
    };

    UINT16 chrominance_dc_size_table[] = {
    0x0002, 0x0002, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007, 0x0008,
    0x0009, 0x000A, 0x000B
    };

    UINT16 luminance_ac_code_table[] = {
    0x000A, 0x0000, 0x0001, 0x0004, 0x000B, 0x001A, 0x0078, 0x00F8, 0x03F6,
    0xFF82, 0xFF83, 0x000C, 0x001B, 0x0079, 0x01F6, 0x07F6, 0xFF84, 0xFF85,
    0xFF86, 0xFF87, 0xFF88, 0x001C, 0x00F9, 0x03F7, 0x0FF4, 0xFF89, 0xFF8A,
    0xFF8b, 0xFF8C, 0xFF8D, 0xFF8E, 0x003A, 0x01F7, 0x0FF5, 0xFF8F, 0xFF90,
    0xFF91, 0xFF92, 0xFF93, 0xFF94, 0xFF95, 0x003B, 0x03F8, 0xFF96, 0xFF97,
    0xFF98, 0xFF99, 0xFF9A, 0xFF9B, 0xFF9C, 0xFF9D, 0x007A, 0x07F7, 0xFF9E,
    0xFF9F, 0xFFA0, 0xFFA1, 0xFFA2, 0xFFA3, 0xFFA4, 0xFFA5, 0x007B, 0x0FF6,
    0xFFA6, 0xFFA7, 0xFFA8, 0xFFA9, 0xFFAA, 0xFFAB, 0xFFAC, 0xFFAD, 0x00FA,
    0x0FF7, 0xFFAE, 0xFFAF, 0xFFB0, 0xFFB1, 0xFFB2, 0xFFB3, 0xFFB4, 0xFFB5,
    0x01F8, 0x7FC0, 0xFFB6, 0xFFB7, 0xFFB8, 0xFFB9, 0xFFBA, 0xFFBB, 0xFFBC,
    0xFFBD, 0x01F9, 0xFFBE, 0xFFBF, 0xFFC0, 0xFFC1, 0xFFC2, 0xFFC3, 0xFFC4,
    0xFFC5, 0xFFC6, 0x01FA, 0xFFC7, 0xFFC8, 0xFFC9, 0xFFCA, 0xFFCB, 0xFFCC,
    0xFFCD, 0xFFCE, 0xFFCF, 0x03F9, 0xFFD0, 0xFFD1, 0xFFD2, 0xFFD3, 0xFFD4,
    0xFFD5, 0xFFD6, 0xFFD7, 0xFFD8, 0x03FA, 0xFFD9, 0xFFDA, 0xFFDB, 0xFFDC,
    0xFFDD, 0xFFDE, 0xFFDF, 0xFFE0, 0xFFE1, 0x07F8, 0xFFE2, 0xFFE3, 0xFFE4,
    0xFFE5, 0xFFE6, 0xFFE7, 0xFFE8, 0xFFE9, 0xFFEA, 0xFFEB, 0xFFEC, 0xFFED,
    0xFFEE, 0xFFEF, 0xFFF0, 0xFFF1, 0xFFF2, 0xFFF3, 0xFFF4, 0xFFF5, 0xFFF6,
    0xFFF7, 0xFFF8, 0xFFF9, 0xFFFA, 0xFFFB, 0xFFFC, 0xFFFD, 0xFFFE,
    0x07F9
    };

    UINT16 luminance_ac_size_table[] = {
    0x0004, 0x0002, 0x0002, 0x0003, 0x0004, 0x0005, 0x0007, 0x0008, 0x000A,
    0x0010, 0x0010, 0x0004, 0x0005, 0x0007, 0x0009, 0x000B, 0x0010, 0x0010,
    0x0010, 0x0010, 0x0010, 0x0005, 0x0008, 0x000A, 0x000C, 0x0010, 0x0010,
    0x0010, 0x0010, 0x0010, 0x0010, 0x0006, 0x0009, 0x000C, 0x0010, 0x0010,
    0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0006, 0x000A, 0x0010, 0x0010,
    0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0007, 0x000B, 0x0010,
    0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0007, 0x000C,
    0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0008,
    0x000C, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010,
    0x0009, 0x000F, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010,
    0x0010, 0x0009, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010,
    0x0010, 0x0010, 0x0009, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010,
    0x0010, 0x0010, 0x0010, 0x000A, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010,
    0x0010, 0x0010, 0x0010, 0x0010, 0x000A, 0x0010, 0x0010, 0x0010, 0x0010,
    0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x000B, 0x0010, 0x0010, 0x0010,
    0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010,
    0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010,
    0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010,
    0x000B
    };

    UINT16 chrominance_ac_code_table[] = {
    0x0000, 0x0001, 0x0004, 0x000A, 0x0018, 0x0019, 0x0038, 0x0078, 0x01F4,
    0x03F6, 0x0FF4, 0x000B, 0x0039, 0x00F6, 0x01F5, 0x07F6, 0x0FF5, 0xFF88,
    0xFF89, 0xFF8A, 0xFF8B, 0x001A, 0x00F7, 0x03F7, 0x0FF6, 0x7FC2, 0xFF8C,
    0xFF8D, 0xFF8E, 0xFF8F, 0xFF90, 0x001B, 0x00F8, 0x03F8, 0x0FF7, 0xFF91,
    0xFF92, 0xFF93, 0xFF94, 0xFF95, 0xFF96, 0x003A, 0x01F6, 0xFF97, 0xFF98,
    0xFF99, 0xFF9A, 0xFF9B, 0xFF9C, 0xFF9D, 0xFF9E, 0x003B, 0x03F9, 0xFF9F,
    0xFFA0, 0xFFA1, 0xFFA2, 0xFFA3, 0xFFA4, 0xFFA5, 0xFFA6, 0x0079, 0x07F7,
    0xFFA7, 0xFFA8, 0xFFA9, 0xFFAA, 0xFFAB, 0xFFAC, 0xFFAD, 0xFFAE, 0x007A,
    0x07F8, 0xFFAF, 0xFFB0, 0xFFB1, 0xFFB2, 0xFFB3, 0xFFB4, 0xFFB5, 0xFFB6,
    0x00F9, 0xFFB7, 0xFFB8, 0xFFB9, 0xFFBA, 0xFFBB, 0xFFBC, 0xFFBD, 0xFFBE,
    0xFFBF, 0x01F7, 0xFFC0, 0xFFC1, 0xFFC2, 0xFFC3, 0xFFC4, 0xFFC5, 0xFFC6,
    0xFFC7, 0xFFC8, 0x01F8, 0xFFC9, 0xFFCA, 0xFFCB, 0xFFCC, 0xFFCD, 0xFFCE,
    0xFFCF, 0xFFD0, 0xFFD1, 0x01F9, 0xFFD2, 0xFFD3, 0xFFD4, 0xFFD5, 0xFFD6,
    0xFFD7, 0xFFD8, 0xFFD9, 0xFFDA, 0x01FA, 0xFFDB, 0xFFDC, 0xFFDD, 0xFFDE,
    0xFFDF, 0xFFE0, 0xFFE1, 0xFFE2, 0xFFE3, 0x07F9, 0xFFE4, 0xFFE5, 0xFFE6,
    0xFFE7, 0xFFE8, 0xFFE9, 0xFFEA, 0xFFEb, 0xFFEC, 0x3FE0, 0xFFED, 0xFFEE,
    0xFFEF, 0xFFF0, 0xFFF1, 0xFFF2, 0xFFF3, 0xFFF4, 0xFFF5, 0x7FC3, 0xFFF6,
    0xFFF7, 0xFFF8, 0xFFF9, 0xFFFA, 0xFFFB, 0xFFFC, 0xFFFD, 0xFFFE,
    0x03FA
    };

    UINT16 chrominance_ac_size_table[] = {
    0x0002, 0x0002, 0x0003, 0x0004, 0x0005, 0x0005, 0x0006, 0x0007, 0x0009,
    0x000A, 0x000C, 0x0004, 0x0006, 0x0008, 0x0009, 0x000B, 0x000C, 0x0010,
    0x0010, 0x0010, 0x0010, 0x0005, 0x0008, 0x000A, 0x000C, 0x000F, 0x0010,
    0x0010, 0x0010, 0x0010, 0x0010, 0x0005, 0x0008, 0x000A, 0x000C, 0x0010,
    0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0006, 0x0009, 0x0010, 0x0010,
    0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0006, 0x000A, 0x0010,
    0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0007, 0x000B,
    0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0007,
    0x000B, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010,
    0x0008, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010,
    0x0010, 0x0009, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010,
    0x0010, 0x0010, 0x0009, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010,
    0x0010, 0x0010, 0x0010, 0x0009, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010,
    0x0010, 0x0010, 0x0010, 0x0010, 0x0009, 0x0010, 0x0010, 0x0010, 0x0010,
    0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x000B, 0x0010, 0x0010, 0x0010,
    0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x000E, 0x0010, 0x0010,
    0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x000F, 0x0010,
    0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010,
    0x000A
    };


    UINT32 FDCT_factor[] = {0};
    /*=====================================================================================
    name: RGB_2_400
    func: 将RGB格式的图像数据转换成YUV_400格式
    input: input_ptr:输入图像的指针； output_ptr:输出jpeg图像的指针； image_width:图像的宽度； image_height:图像的高度
    return: 无
    description:
    ======================================================================================*/
    static void RGB_2_400(UINT8 * input_ptr, UINT8 * output_ptr, UINT32 image_width, UINT32 image_height)
    {
        UINT32 i, size;
        UINT8 R, G, B;
        INT32 Y;
        UINT8 * inbuf = input_ptr;
        size = image_width * image_height;
        for (i = size; i > 0; i--)

        {
            B = inbuf[0];
            G = inbuf[1];
            R = inbuf[2];
            inbuf += 3;
            Y = CLIP((77 * R + 150 * G + 29 * B) >> 8);
            *input_ptr++ = (UINT8)Y;
        }
    }

    /*=====================================================================================
    name:read_400_format 
    func: 从输入的图像当中，读取一个8x8块
    input: jpeg_encoder_structure:编码用到的相关参数结构体的指针； input_ptr:输入图像数据
    return: 无
    description:
    ======================================================================================*/
    static void read_400_format(JPEG_ENCODER_STRUCTURE * jpeg_encoder_structure, UINT8 * input_ptr)
    {
        INT32 i, j;
        INT16 * Y1_Ptr = Y1;
        UINT16 rows = jpeg_encoder_structure->rows;
        UINT16 cols = jpeg_encoder_structure->cols;
        UINT16 incr = jpeg_encoder_structure->incr;
        for (i = rows; i > 0; i--)
        {
            for (j = cols; j > 0; j--)
                *Y1_Ptr++ = *input_ptr++ - 128;
            for (j = 8 - cols; j > 0; j--)
                *Y1_Ptr++ = *(Y1_Ptr - 1);
            input_ptr += incr;
        }
        for (i = 8 - rows; i > 0; i--)
        {
            for (j = 8; j > 0; j--)
                *Y1_Ptr++ = *(Y1_Ptr - 8);
        }
    }

    /*=====================================================================================
    name:DSP_Division
    func: 适用于DSP的除法快速计算
    input: numer:被除数；denom:除数
    return: 商
    description:
    ======================================================================================*/
    UINT16 DSP_Division(UINT32 numer, UINT32 denom)
    {
        UINT16 i;
        denom <<= 15;
        for (i = 16; i > 0; i--)
        {
            if (numer > denom)
            {
                numer -= denom;
                numer <<= 1;
                numer++;
            }
            else
                numer <<= 1;
        }
        return (UINT16)numer;
    }

    /*=====================================================================================
    name:write_markers 
    func: 将jpeg文件的相关文件头写入到文件当中,实际是先放入文件的缓存区当中，等所有的文件数据都准备好后，统一写入文件当中
    input: output_ptr:输出的jpeg图像文件的指针； image_format:图像的类型； image_width:图像的宽度； image_height:图像的高度
    return:输出的jpeg图像文件的指针
    description:
    ======================================================================================*/
    UINT8 * write_markers(UINT8 * output_ptr, UINT32 image_format, UINT32 image_width, UINT32 image_height)
    {
        UINT16 i, header_length;
        UINT8 number_of_components;

        // Start of image marker
        *output_ptr++ = 0xFF;
        *output_ptr++ = 0xD8;

        // Quantization table marker
        *output_ptr++ = 0xFF;
        *output_ptr++ = 0xDB;

        // Quantization table length
        *output_ptr++ = 0x00;
        //*output_ptr++ = 0x84;
        *output_ptr++ = 0x43;

        // Pq, Tq
        *output_ptr++ = 0x00;

        // Lqt table
        for (i = 0; i < 64; i++)
            *output_ptr++ = Lqt[i];

        // Pq, Tq
/*       *output_ptr++ = 0x01;

        // Cqt table
        for (i = 0; i < 64; i++)
            *output_ptr++ = Cqt[i];
*/
        // huffman table(DHT)
        for (i = 0; i < 210; i++)
        {
            *output_ptr++ = (UINT8)(markerdata[i] >> 8);
            *output_ptr++ = (UINT8)markerdata[i];
        }
        //FOUR_ZERO_ZERO
        number_of_components = 1;

        // Frame header(SOF)

        // Start of frame marker
        *output_ptr++ = 0xFF;
        *output_ptr++ = 0xC0;
        header_length = (UINT16)(8 + 3 * number_of_components);

        // Frame header length  
        *output_ptr++ = (UINT8)(header_length >> 8);
        *output_ptr++ = (UINT8)header_length;

        // Precision (P)
        *output_ptr++ = 0x08;

        // image height
        *output_ptr++ = (UINT8)(image_height >> 8);
        *output_ptr++ = (UINT8)image_height;

        // image width
        *output_ptr++ = (UINT8)(image_width >> 8);
        *output_ptr++ = (UINT8)image_width;

        // Nf
        *output_ptr++ = number_of_components;
        // FOUR_ZERO_ZERO
        *output_ptr++ = 0x01;
        *output_ptr++ = 0x11;
        *output_ptr++ = 0x00;

        // Scan header(SOF)

        // Start of scan marker
        *output_ptr++ = 0xFF;
        *output_ptr++ = 0xDA;
        header_length = (UINT16)(6 + (number_of_components << 1));

        // Scan header length
        *output_ptr++ = (UINT8)(header_length >> 8);
        *output_ptr++ = (UINT8)header_length;

        // Ns
        *output_ptr++ = number_of_components;
        //FOUR_ZERO_ZERO
        *output_ptr++ = 0x01;
        *output_ptr++ = 0x00;

        *output_ptr++ = 0x00;
        *output_ptr++ = 0x3F;
        *output_ptr++ = 0x00;
        return output_ptr;
    }

    /*=====================================================================================
    name:initialize_quantization_tables 
    func: 初始化编码中所用到的相关参数, 如基本块的大小，图像所划分出的块的个数等
    input: jpeg:编码参数结构体指针； image_format:图像的类型； image_width:图像的宽度； image_height:图像的高度；
    return:无
    description:
    ======================================================================================*/
    static void initialization(JPEG_ENCODER_STRUCTURE * jpeg, UINT32 image_format, UINT32 image_width, UINT32 image_height)
    {
        UINT16 mcu_width, mcu_height, bytes_per_pixel;
        lcode = 0;
        bitindex = 0;

        jpeg->mcu_width = mcu_width = 8;
        jpeg->mcu_height = mcu_height = 8;
        jpeg->horizontal_mcus = (UINT16)((image_width + mcu_width - 1) >> 3);
        jpeg->vertical_mcus = (UINT16)((image_height + mcu_height - 1) >> 3);

        bytes_per_pixel = 1;
        read_format = read_400_format;

        jpeg->rows_in_bottom_mcus = (UINT16)(image_height - (jpeg->vertical_mcus - 1) * mcu_height);
        jpeg->cols_in_right_mcus = (UINT16)(image_width - (jpeg->horizontal_mcus - 1) * mcu_width);
        jpeg->length_minus_mcu_width = (UINT16)((image_width - mcu_width) * bytes_per_pixel);
        jpeg->length_minus_width = (UINT16)((image_width - jpeg->cols_in_right_mcus) * bytes_per_pixel);
        jpeg->mcu_width_size = (UINT16)(mcu_width * bytes_per_pixel);

        jpeg->offset = (UINT16)((image_width * (mcu_height - 1) - (mcu_width - jpeg->cols_in_right_mcus)) * bytes_per_pixel);
        jpeg->ldc1 = 0;
        jpeg->ldc2 = 0;
        jpeg->ldc3 = 0;
    }

    /*=====================================================================================
    name:initialize_quantization_tables 
    func: 初始化编码中所用到的量化表
    input:quality_factor:;
    return:无
    description:
    ======================================================================================*/
    void initialize_quantization_tables(UINT32 quality_factor)
    {
        UINT16 i, index;
        UINT32 value;
/* 
        UINT8 luminance_quant_table[] = {//原如量化表
            16, 11, 10, 16, 24, 40, 51, 61, 12, 12, 14, 19, 26, 58, 60, 55, 14, 13,
            16, 24, 40, 57, 69, 56, 14, 17, 22, 29, 51, 87, 80, 62, 18, 22, 37,
            56, 68, 109, 103, 77, 24, 35, 55, 64, 81, 104, 113, 92, 49, 64, 78,
            87, 103, 121, 120, 101, 72, 92, 95, 98, 112, 100, 103, 99
        };
*/

/*       UINT8 luminance_quant_table[] = {//高质量压缩
            0x00, 0x02, 0x01, 0x01, 0x02, 0x01, 0x01, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x03, 0x05, 0x03, 0x03, 0x03, 0x03, 0x03, 0x06, 0x04, 0x04, 0x03, 0x05, 0x07, 0x06, 0x07, 0x07, 0x07, 0x06, 0x07, 0x07, 0x08, 0x09, 0x0b, 0x09, 0x08, 0x08, 0x0a, 0x08, 0x07, 0x07, 0x0a, 0x0d, 0x0a, 0x0a, 0x0b, 0x0c, 0x0c, 0x0c, 0x0c, 0x07, 0x09, 0x0e, 0x0f, 0x0d, 0x0c, 0x0e, 0x0b, 0x0c, 0x0c, 0x0c };
 */

/*
 UINT8 luminance_quant_table[] = {
     0x0b, 0x07, 0x08, 0x0a, 0x08, 0x07, 0x0b, 0x0a, 0x09, 0x0a, 0x0c, 0x0c, 0x0b, 0x0d, 0x11, 0x1c, 0x12, 0x11, 0x0f, 0x0f, 0x11, 0x23, 0x19, 0x1a, 0x14, 0x1c, 0x29, 0x24, 0x2b, 0x2b, 0x28, 0x24, 0x28, 0x27, 0x2e, 0x33, 0x42, 0x38, 0x2e, 0x30, 0x3e, 0x31, 0x27, 0x28, 0x39, 0x4e, 0x3a, 0x3e, 0x44, 0x46, 0x4a, 0x4a, 0x4a, 0x2c, 0x37, 0x51, 0x56, 0x50, 0x47, 0x56, 0x42, 0x48, 0x4a, 0x47 };
*/

/*
 UINT8 luminance_quant_table[] = {//最大压缩率,matlab中图像质量为15时保存的jpg文件中提取的量化表
     0x35, 0x25, 0x21, 0x35, 0x50, 0x85, 0xaa, 0xcb
     , 0x28, 0x28, 0x2f, 0x3f, 0x57, 0xc1, 0xc8, 0xb7
     , 0x2f, 0x2b, 0x35, 0x50, 0x85, 0xbe, 0xe6, 0xba
     , 0x2f, 0x39, 0x49, 0x61, 0xaa, 0xff, 0xff, 0xce
     , 0x3c, 0x49, 0x7b, 0xba, 0xe2, 0xff, 0xff, 0xff
     , 0x50, 0x75, 0xb7, 0xd5, 0xff, 0xff, 0xff, 0xff
     , 0xa3, 0xd5, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
     , 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
 };
 */
        UINT8 luminance_quant_table[] = { //matlab中图像质量为22时保存的jpg文件中提取的量化表
              0x24, 0x19, 0x17, 0x24, 0x36, 0x5b, 0x74, 0x8a
            , 0x1b, 0x1b, 0x20, 0x2b, 0x3b, 0x84, 0x88, 0x7d
            , 0x20, 0x1e, 0x24, 0x36, 0x5b, 0x81, 0x9d, 0x7f
            , 0x20, 0x27, 0x32, 0x42, 0x74, 0xc5, 0xb6, 0x8d
            , 0x29, 0x32, 0x54, 0x7f, 0x9a, 0xf7, 0xea, 0xaf
            , 0x36, 0x4f, 0x7d, 0x91, 0xb8, 0xec, 0xff, 0xd1
            , 0x6f, 0x91, 0xb1, 0xc5, 0xea, 0xff, 0xff, 0xe5
            , 0xa3, 0xd1, 0xd8, 0xde, 0xfe, 0xe3, 0xea, 0xe1
        };
        for (i = 0; i < 64; i++)
        {
            index = zigzag_table[i];

            //// luminance quantization table * quality factor  
            value = luminance_quant_table[i];// *quality_factor;
            if (value == 0)
                value = 1;

            else if (value > 255)
                value = 255;
            Lqt[index] = (UINT8)value;
            ILqt[i] = DSP_Division(0x8000, value);

        }
    }
/*     //原始版本
    void initialize_quantization_tables(UINT32 quality_factor)
    {
        UINT16 i, index;
        UINT32 value;
         UINT8 luminance_quant_table[] = {
        16, 11, 10, 16, 24, 40, 51, 61, 12, 12, 14, 19, 26, 58, 60, 55, 14, 13,
        16, 24, 40, 57, 69, 56, 14, 17, 22, 29, 51, 87, 80, 62, 18, 22, 37,
        56, 68, 109, 103, 77, 24, 35, 55, 64, 81, 104, 113, 92, 49, 64, 78,
        87, 103, 121, 120, 101, 72, 92, 95, 98, 112, 100, 103, 99
        };
        

        UINT8 chrominance_quant_table[] = {
            17, 18, 24, 47, 99, 99, 99, 99, 18, 21, 26, 66, 99, 99, 99, 99, 24, 26,
            56, 99, 99, 99, 99, 99, 47, 66, 99, 99, 99, 99, 99, 99, 99, 99, 99,
            99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
            99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99
        };
        for (i = 0; i < 64; i++)
        {
            index = zigzag_table[i];

            //// luminance quantization table * quality factor  
            value = luminance_quant_table[i];// *quality_factor;
            value = (value + 0x200) >> 10;
            if (value == 0)
                value = 1;

            else if (value > 255)
                value = 255;
            Lqt[index] = (UINT8)value;
            ILqt[i] = DSP_Division(0x8000, value);

            ///* chrominance quantization table * quality factor 
            value = chrominance_quant_table[i] * quality_factor;
            value = (value + 0x200) >> 10;
            if (value == 0)
            value = 1;

            else if (value > 255)
            value = 255;
            Cqt[index] = (UINT8)value;
            ICqt[i] = DSP_Division(0x8000, value);
            
        }
    }
*/
    /*=====================================================================================
    name:close_bitstream
    func: 完成.jpg文件的写入
    input:输出的jpg文件的指针
    return:输出的jpg文件的指针
    description:
    ======================================================================================*/
    UINT8 * close_bitstream(UINT8 * output_ptr)
    {
        UINT16 i, count;
        UINT8 * ptr;
        if (bitindex > 0)
        {
            lcode <<= (32 - bitindex);
            count = (bitindex + 7) >> 3;
            ptr = (UINT8 *)& lcode + 3;
            for (i = count; i > 0; i--)
            {
                if ((*output_ptr++ = *ptr--) == 0xff)
                    *output_ptr++ = 0;
            }
        }
        // End of image marker
        *output_ptr++ = 0xFF;
        *output_ptr++ = 0xD9;
        return output_ptr;
    }
    /*=====================================================================================
    name:DCT
    func: 实现对8x8块的DCT变换
    input:8x8块的指针
    return: 无
    description:
    ======================================================================================*/
    ///* DCT for One block(8x8)  
    static void DCT(INT16 * data)
    {
        UINT16 i;
        INT32 x0, x1, x2, x3, x4, x5, x6, x7, x8;

        //All values are shifted left by 10
        //and rounded off to nearest integer 
        static const UINT16 c1 = 1420; // cos PI/16 * root(2)  
        static const UINT16 c2 = 1338; // cos PI/8 * root(2)   
        static const UINT16 c3 = 1204; // cos 3PI/16 * root(2) 
        static const UINT16 c5 = 805; // cos 5PI/16 * root(2) 
        static const UINT16 c6 = 554; // cos 3PI/8 * root(2)  
        static const UINT16 c7 = 283; // cos 7PI/16 * root(2) 
        static const UINT16 s1 = 3;
        static const UINT16 s2 = 10;
        static const UINT16 s3 = 13;
        for (i = 8; i > 0; i--)
        {
            x8 = data[0] + data[7];
            x0 = data[0] - data[7];
            x7 = data[1] + data[6];
            x1 = data[1] - data[6];
            x6 = data[2] + data[5];
            x2 = data[2] - data[5];
            x5 = data[3] + data[4];
            x3 = data[3] - data[4];
            x4 = x8 + x5;
            x8 -= x5;
            x5 = x7 + x6;
            x7 -= x6;
            data[0] = (INT16)(x4 + x5);
            data[4] = (INT16)(x4 - x5);
            data[2] = (INT16)((x8 * c2 + x7 * c6) >> s2);
            data[6] = (INT16)((x8 * c6 - x7 * c2) >> s2);
            data[7] = (INT16)((x0 * c7 - x1 * c5 + x2 * c3 - x3 * c1) >> s2);
            data[5] = (INT16)((x0 * c5 - x1 * c1 + x2 * c7 + x3 * c3) >> s2);
            data[3] = (INT16)((x0 * c3 - x1 * c7 - x2 * c1 - x3 * c5) >> s2);
            data[1] = (INT16)((x0 * c1 + x1 * c3 + x2 * c5 + x3 * c7) >> s2);
            data += 8;
        }
        data -= 64;
        for (i = 8; i > 0; i--)
        {
            x8 = data[0] + data[56];
            x0 = data[0] - data[56];
            x7 = data[8] + data[48];
            x1 = data[8] - data[48];
            x6 = data[16] + data[40];
            x2 = data[16] - data[40];
            x5 = data[24] + data[32];
            x3 = data[24] - data[32];
            x4 = x8 + x5;
            x8 -= x5;
            x5 = x7 + x6;
            x7 -= x6;
            data[0] = (INT16)((x4 + x5) >> s1);
            data[32] = (INT16)((x4 - x5) >> s1);
            data[16] = (INT16)((x8 * c2 + x7 * c6) >> s3);
            data[48] = (INT16)((x8 * c6 - x7 * c2) >> s3);
            data[56] = (INT16)((x0 * c7 - x1 * c5 + x2 * c3 - x3 * c1) >> s3);
            data[40] = (INT16)((x0 * c5 - x1 * c1 + x2 * c7 + x3 * c3) >> s3);
            data[24] = (INT16)((x0 * c3 - x1 * c7 - x2 * c1 - x3 * c5) >> s3);
            data[8] = (INT16)((x0 * c1 + x1 * c3 + x2 * c5 + x3 * c7) >> s3);
            data++;
        }
    }

    /*=====================================================================================
    name:quantization
    func: 实现对8x8的量化
    input: data:8x8块指针；quant_table_prt：量化表指针
    return: 无
    description:
    ======================================================================================*/
    // multiply DCT Coefficients with Quantization table and store in ZigZag location 
    void quantization(INT16 * const data, UINT16 * const quant_table_ptr)
    {
        INT16 i;
        INT32 value;
        for (i = 63; i >= 0; i--)

        {
            value = data[i] * quant_table_ptr[i];
            value = (value + 0x4000) >> 15;
            Temp[zigzag_table[i]] = (INT16)value;
        }
    }

    /*=====================================================================================
    name:huffman
    func: 实现对8x8x块的huffman编码
    input:jpeg_encoder_structure:jpeg编码所用到的相关参数结构体的指针；component:组件数量；output_ptr:输出数据指针
    return: 输出数据的指针
    description:
    ======================================================================================*/
    UINT8 * huffman(JPEG_ENCODER_STRUCTURE * jpeg_encoder_structure, UINT16 component, UINT8 * output_ptr)
    {
        UINT16 i;
        UINT16 * DcCodeTable, *DcSizeTable, *AcCodeTable, *AcSizeTable;
        INT16 * Temp_Ptr, Coeff, LastDc;
        UINT16 AbsCoeff, HuffCode, HuffSize, RunLength = 0, DataSize = 0, index;
        INT16 bits_in_next_word;
        UINT16 numbits;
        UINT32 data;
        Temp_Ptr = Temp;
        Coeff = *Temp_Ptr++;
        if (component == 1)
        {
            DcCodeTable = luminance_dc_code_table;
            DcSizeTable = luminance_dc_size_table;
            AcCodeTable = luminance_ac_code_table;
            AcSizeTable = luminance_ac_size_table;
            LastDc = jpeg_encoder_structure->ldc1;
            jpeg_encoder_structure->ldc1 = Coeff;
        }
        else
        {
            DcCodeTable = chrominance_dc_code_table;
            DcSizeTable = chrominance_dc_size_table;
            AcCodeTable = chrominance_ac_code_table;
            AcSizeTable = chrominance_ac_size_table;
            if (component == 2)
            {
                LastDc = jpeg_encoder_structure->ldc2;
                jpeg_encoder_structure->ldc2 = Coeff;
            }
            else
            {
                LastDc = jpeg_encoder_structure->ldc3;
                jpeg_encoder_structure->ldc3 = Coeff;
            }
        }

        Coeff -= LastDc;
        AbsCoeff = (Coeff < 0) ? -Coeff-- : Coeff;

        while (AbsCoeff != 0)
        {
            AbsCoeff >>= 1;
            DataSize++;
        }

        HuffCode = DcCodeTable[DataSize];
        HuffSize = DcSizeTable[DataSize];
        Coeff &= (1 << DataSize) - 1;
        data = (HuffCode << DataSize) | Coeff;
        numbits = HuffSize + DataSize;
        //PUTBITS
        {
            bits_in_next_word = (INT16)(bitindex + numbits - 32);
            if (bits_in_next_word < 0)
            {
                lcode = (lcode << numbits) | data;
                bitindex += numbits;
            }
            else
            {
                lcode = (lcode << (32 - bitindex)) | (data >> bits_in_next_word);
                if ((*output_ptr++ = (UINT8)(lcode >> 24)) == 0xff)
                    *output_ptr++ = 0;
                if ((*output_ptr++ = (UINT8)(lcode >> 16)) == 0xff)
                    *output_ptr++ = 0;
                if ((*output_ptr++ = (UINT8)(lcode >> 8)) == 0xff)
                    *output_ptr++ = 0;
                if ((*output_ptr++ = (UINT8)lcode) == 0xff)
                    *output_ptr++ = 0;
                lcode = data;
                bitindex = bits_in_next_word;
            }
        }
        for (i = 63; i > 0; i--)
        {
            if ((Coeff = *Temp_Ptr++) != 0)
            {
                while (RunLength > 15)
                {
                    RunLength -= 16;
                    data = AcCodeTable[161];
                    numbits = AcSizeTable[161];
                    //PUTBITS
                    {
                        bits_in_next_word = (INT16)(bitindex + numbits - 32);
                        if (bits_in_next_word < 0)
                        {
                            lcode = (lcode << numbits) | data;
                            bitindex += numbits;
                        }
                        else
                        {
                            lcode = (lcode << (32 - bitindex)) | (data >> bits_in_next_word);
                            if ((*output_ptr++ = (UINT8)(lcode >> 24)) == 0xff)
                                *output_ptr++ = 0;
                            if ((*output_ptr++ = (UINT8)(lcode >> 16)) == 0xff)
                                *output_ptr++ = 0;
                            if ((*output_ptr++ = (UINT8)(lcode >> 8)) == 0xff)
                                *output_ptr++ = 0;
                            if ((*output_ptr++ = (UINT8)lcode) == 0xff)
                                *output_ptr++ = 0;
                            lcode = data;
                            bitindex = bits_in_next_word;
                        }
                    }
                }
                
                AbsCoeff = (Coeff < 0) ? -Coeff-- : Coeff;

                if (AbsCoeff >> 8 == 0)
                    DataSize = bitsize[AbsCoeff];
                else
                    DataSize = bitsize[AbsCoeff >> 8] + 8;

                index = RunLength * 10 + DataSize;
                HuffCode = AcCodeTable[index];
                HuffSize = AcSizeTable[index];
                Coeff &= (1 << DataSize) - 1;
                data = (HuffCode << DataSize) | Coeff;
                numbits = HuffSize + DataSize;
                // PUTBITS
                {
                    bits_in_next_word = (INT16)(bitindex + numbits - 32);
                    if (bits_in_next_word < 0)
                    {
                        lcode = (lcode << numbits) | data;
                        bitindex += numbits;
                    }
                    else
                    {
                        lcode = (lcode << (32 - bitindex)) | (data >> bits_in_next_word);
                        if ((*output_ptr++ = (UINT8)(lcode >> 24)) == 0xff)
                            *output_ptr++ = 0;
                        if ((*output_ptr++ = (UINT8)(lcode >> 16)) == 0xff)
                            *output_ptr++ = 0;
                        if ((*output_ptr++ = (UINT8)(lcode >> 8)) == 0xff)
                            *output_ptr++ = 0;
                        if ((*output_ptr++ = (UINT8)lcode) == 0xff)
                            *output_ptr++ = 0;
                        lcode = data;
                        bitindex = bits_in_next_word;
                    }
                }
                RunLength = 0;
            }
            else
                RunLength++;
        }
        if (RunLength != 0)
        {
            data = AcCodeTable[0];
            numbits = AcSizeTable[0];
            // PUTBITS 
            {
                bits_in_next_word = (INT16)(bitindex + numbits - 32);
                if (bits_in_next_word < 0)
                {
                    lcode = (lcode << numbits) | data;
                    bitindex += numbits;
                }
                else
                {
                    lcode = (lcode << (32 - bitindex)) | (data >> bits_in_next_word);
                    if ((*output_ptr++ = (UINT8)(lcode >> 24)) == 0xff)
                        *output_ptr++ = 0;
                    if ((*output_ptr++ = (UINT8)(lcode >> 16)) == 0xff)
                        *output_ptr++ = 0;
                    if ((*output_ptr++ = (UINT8)(lcode >> 8)) == 0xff)
                        *output_ptr++ = 0;
                    if ((*output_ptr++ = (UINT8)lcode) == 0xff)
                        *output_ptr++ = 0;
                    lcode = data;
                    bitindex = bits_in_next_word;
                }
            }
        }
        return output_ptr;
    }

    static UINT8 *encodeMCU(JPEG_ENCODER_STRUCTURE * jpeg_encoder_structure, UINT32 image_format, UINT8 * output_ptr)
    {
        DCT(Y1);
        quantization(Y1, ILqt);
        output_ptr = huffman(jpeg_encoder_structure, 1, output_ptr);
        return output_ptr;
    }

    #define  FOUR_ZERO_ZERO 0 // Grey scale Y00 ...

    /*=================================================================================================================================
    name: encode_image
    func: 实现对整幅图片的JPEG编码
    input:input_ptr:输入图像的指针; output_ptr:输出的JPEG编码图你的指针; quality_factor:; image_format:输入图像的格式,即灰度还是彩色;
          image_width:输入图像的宽度; image_height:输入图像的高度;
    return: .jpg文件的长度
    description:
    =====================================================================================================================================*/
    UINT32 encode_image(UINT8 * input_ptr, UINT8 * output_ptr, UINT32 quality_factor, UINT32 image_format, UINT32 image_width, UINT32 image_height)
    {
        UINT16 i, j;
        UINT8 * output;
        JPEG_ENCODER_STRUCTURE JpegStruct;
        JPEG_ENCODER_STRUCTURE * jpeg_encoder_structure = &JpegStruct;
        output = output_ptr;
        image_format = FOUR_ZERO_ZERO;
        //RGB_2_400(input_ptr, output_ptr, image_width, image_height);
        // Initialization of JPEG control structure  
        initialization(jpeg_encoder_structure, image_format, image_width, image_height);//设定输入图像的相关参数，如图像的宽度与高度，所分成的8x8块的个数等

        // Quantization Table Initialization 
        initialize_quantization_tables(quality_factor);//设置量化变

        // Writing Marker Data  
        output_ptr = write_markers(output_ptr, image_format, image_width, image_height);//写入jpeg文件头相关的信息
        for (i = 1; i <= jpeg_encoder_structure->vertical_mcus; i++)
        {
                jpeg_encoder_structure->rows = jpeg_encoder_structure->mcu_height;
            for (j = 1; j <= jpeg_encoder_structure->horizontal_mcus; j++)
            {
                jpeg_encoder_structure->cols = jpeg_encoder_structure->mcu_width;
                jpeg_encoder_structure->incr = jpeg_encoder_structure->length_minus_mcu_width;

                read_format(jpeg_encoder_structure, input_ptr);//读入一个8x8块
                // Encode the data in MCU 
                output_ptr = encodeMCU(jpeg_encoder_structure, image_format, output_ptr);//进行JPEG编码
                input_ptr += jpeg_encoder_structure->mcu_width_size;
            }
            input_ptr += jpeg_encoder_structure->offset;
        }
        // Close Routine 
        output_ptr = close_bitstream(output_ptr);//没有用到库函数，可以正常移植
        return (UINT32)(output_ptr - output);
    }

  

   

