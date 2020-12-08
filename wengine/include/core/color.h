#ifndef COLOR_H
#define COLOR_H
#include "types.h"

//#define w_Rgb(r,g,b)            ((w_Color)(((unsigned char)(b)|((unsigned short)((unsigned char)(g))<<8))|(((unsigned long)(unsigned char)(r))<<16)))
//#define w_Rgba(r,g,b,a)         ((w_Color)((((unsigned char)(b)|((unsigned short)((unsigned char)(g))<<8))|(((unsigned long)(unsigned char)(r))<<16))|(((unsigned long)(unsigned char)(a))<<24)))
//#define w_Bgr(b,g,r)            ((w_Color)(((unsigned char)(r)|((unsigned short)((unsigned char)(g))<<8))|(((unsigned long)(unsigned char)(b))<<16)))
//#define GetColor(tcolor)        ((w_Color)(((w_Color)tcolor) & 0x00FFFFFF)) //Returns color with alpha channel removed
//#define GetBValueT(tcolor)      ((unsigned char)tcolor)
//#define GetGValueT(tcolor)      ((unsigned char)(((w_Color)tcolor) >> 8))
//#define GetRValueT(tcolor)      ((unsigned char)(((w_Color)tcolor) >> 16))
//#define GetAValueT(tcolor)      ((unsigned char)(((w_Color)tcolor) >> 24))

w_Color w_Rgb(unsigned char r, unsigned char g, unsigned char b);
w_Color w_Rgba(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
unsigned char GetBValueT(w_Color color);
unsigned char GetGValueT(w_Color color);
unsigned char GetRValueT(w_Color color);
unsigned char GetAValueT(w_Color color);

//Remove this, it's not useful. I think it's only used in 1 place
w_Color GetColor(w_Color color);

#endif