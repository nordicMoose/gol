#ifndef TYPES_H
#define TYPES_H

typedef unsigned long w_Color;    //Holds 4 values (0-255), alpha-red-green-blue

typedef char* (*NamingFunc)(void*); // A function that take a void* as argument and returns a cstring

typedef struct       //Raw image format
{
    w_Color* data;
    unsigned int width;
    unsigned int height;
}w_Image;

typedef struct
{
    unsigned short character;      //wchar_t
    unsigned int x;
    unsigned int y;
    w_Image bitmap;
}w_Chardata;

typedef struct
{
    unsigned int c_width;
    unsigned int c_height;
    unsigned int count;
    char alphamode;
    w_Chardata* chardata;
}w_Font;

typedef struct
{
    double time;
    char enabled;
    const unsigned int id;
}w_Timer;

typedef struct
{
    //char text[1024];
    char* text;
    unsigned int charcount;
    unsigned int len;
    char active;
    const unsigned int id;
}w_Inputfield;

typedef enum
{
    VSYNC_DISABLED,
    VSYNC_ENABLED
}VSYNC_STATE;

//Windows types

typedef union{
    struct {
        unsigned long LowPart;
        long HighPart;
    } DUMMYSTRUCTNAME;
    struct {
        unsigned long LowPart;
        long HighPart;
    } u;
    long long  QuadPart;
}w_LARGE_INTEGER;

typedef struct
{
    long  x;
    long  y;
}w_LPoint;

#endif