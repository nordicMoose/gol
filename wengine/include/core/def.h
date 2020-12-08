#ifndef DEF_H
#define DEF_H

//Move everything out of here except for value defs and includes
//functions should be defined in their own headers

//create init function for everything, and then a "startprogram" which will have the windows message loop
//init should take window size and title as params. Somehow include the fps limit too, without doing ifs every frame
//startprogram should take start, update and exit function pointers as parameters
//if they are 0, they aren't executed, and if update is 0 also, call exit at program start
//This way engine can be compiled into a library

//#include <windows.h>
#include <limits.h>
#include <math.h>
#include <time.h>
#include "util.h"
#include "color.h"
#include "input.h"
#include "vars.h"
#include "glad/glad.h"

//program icon resource id
#define ID_ICON 1

//value defs
#define MAX_KEYS 0xFE
#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795028841971693993751058209749445923078164062
#endif

//turn into variables and move into vars.c
//#define W_WIDTH 600
//#define W_HEIGHT 600
//#define W_TITLE "Application"
#define DFONT_SMALL

#define W_RENDERER_SW 0
#define W_RENDERER_GL 1

#define W_ORIENTATION_UP 0
#define W_ORIENTATION_RIGHT 1
#define W_ORIENTATION_DOWN 2
#define W_ORIENTATION_LEFT 3

//figure out how to do this without macros
#ifndef FPS_NOLIMIT
#define MIN_FRAMETIME -1000LL   //x100 nanoseconds, e.g. 1000 = 0.1ms, must have the '-' and 'LL', as it is LARGE_INTEGER
#endif


//Misc functions
void ExitApplication();                  //Exit the application
w_Color * GetScreenBufferCopy();         //Get a copy of the current screenbuffer
w_Color* Dev_GetScreenBuffer();          //Get pointer to screenbuffer. Use with caution
void Dev_SetScreenBuffer(w_Color* buffer);  //Set screen buffer pointer. Use with caution
char FileExists(const char* path);       //Check if a file exists. Returns 1 if exists, 0 otherwise. define USE_WINACCESS to use windows functions instead of stdio   

//Timer functions
w_Timer* CreateTimer();             //Timer constructor
int DeleteTimer(w_Timer** timer);   //Timer destructor

//Inputfield functions
w_Inputfield* CreateInputField();           //Inputfield constructor
int DeleteInputField(w_Inputfield** field); //Inputfield destructor

//Graphic functions
int DrawPixel(unsigned int x, unsigned int y, w_Color color);       //Draw single pixel
int DrawLine(int x1, int y1, int x2, int y2, w_Color color);        //Draw line from point to point
int DrawLineClamped(int x1, int y1, int x2, int y2, int xmin, int ymin, int xmax, int ymax, w_Color color); //Draw line from point to point, but don't draw outside of xmin...xmax ymin...ymax
void DrawHLine(int x1, int x2, int y, w_Color color);               //Draw line from x1 to x2 on y  
int DrawRect(int x1, int y1, int x2, int y2, w_Color color);        //Draw rectangle outline (1 pixel wide)
int FillDrawRect(int x1, int y1, int x2, int y2, w_Color color);    //Fill a rectangle
int SetBuffer(w_Color* buffer);                                     //Set screenbuffer to another buffer
int FillScreen(w_Color color);                                      //Set screenbuffer all to one color
void Display();                                                     //Render screenbuffer to window if something has been drawn
void DisplayDirect();                                               //Render screenbuffer to window even if nothing changed

//image and image drawing
w_Image LoadTImage(const char * path);                                                                           //Load image from file to a w_Image
w_Image * LoadTImageP(const char * path);                                                                        //Load image from a file to a new w_Image pointer
w_Image LoadTImageS(unsigned char data[], unsigned int width, unsigned int height);                              //Load image from a string to a w_Image
int DrawImage(unsigned int x, unsigned int y, const w_Image* image);                                             //Draw a w_Image so that image top left is at xy
int DrawImageS(unsigned int x, unsigned int y, unsigned int width, unsigned int height, const w_Image* image);   //Draw a w_Image so that image top left is at xy, with specified width and height
int SaveImage(const w_Image* image, const char* path);                                                           //Saves a w_Image to a specified png file
int SaveImageB(const w_Image* image, NamingFunc namefunc, void* nameargs);                                       //Saves a w_image to a file, name of which is obtained from namefunc with nameargs as arguments
void DestroyImageP(w_Image* image);                                                                              //Free memory of image pointer 
void DestroyImage(w_Image image);                                                                                //Free memory of image

//text and text drawing
int LoadFont(const char* datapath, w_Font* outfont); //load a font from fdata file and a bitmap
int LoadFontB(const char* filename, const char* fontname, int height, w_Font** out_font); //load a font from one of the mainstream font files, such as ttf
int DrawStringS(unsigned int x, unsigned int y, unsigned int scale, const char* str, const w_Color color[], unsigned int colorLen, const w_Font* font);                                                 //Draw a char* string
int DrawStringBS(unsigned int x, unsigned int y, unsigned int scale, const char* str, const w_Color color[], unsigned int colorLen, const w_Color bgcolor[], unsigned int bgcolorLen, const w_Font* font);    //Draw a wchar_t string
int DrawString(unsigned int x, unsigned int y, unsigned int scale, const wchar_t* str, const w_Color color[], unsigned int colorLen, const w_Font* font);                                               //Draw a char* string with a background
int DrawStringB(unsigned int x, unsigned int y, unsigned int scale, const wchar_t* str, const w_Color color[], unsigned int colorLen, const w_Color bgcolor[], unsigned int bgcolorLen, const w_Font* font);  //Draw a wchar_t string with a background

//FINISH THESE
//background stuff
void UseBackground(unsigned char value);
int SetBackgroundColor(w_Color color);
int ClearScreen();
int ClearArea(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2);
int ClearPixel(unsigned int x, unsigned int y);

#endif