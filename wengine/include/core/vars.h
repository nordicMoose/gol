#ifndef VARS_H
#define VARS_H

#include "types.h"

char GetCapslock();                         //Returns the current capslock state
void** GetWindowHandle();                   //Returns a handle to the window
char** GetArgs();
int GetArgCount();              
char* GetWorkingDirectory();                //Returns the current working directory
int GetDrawHeight();                        //Returns the drawing resolution height
int GetDrawWidth();                         //Returns the drawing resolution width
void SetDrawSize(int width, int height);    //Sets the drawing resolution for the software renderer
void SetRenderMode(char mode);              //Sets the rendering mode
char GetRenderMode();                       //Returns the current rendering mode
double GetDeltatime();                      //Returns time taken by last frame, in seconds
double GetTotaltime();                      //Returns the time the application has been running. Timer is started when w_StartApplication is called
int GetWindowHeight();                      //Returns the window height
int GetWindowWidth();                       //Returns the window width
const char* GetWindowTitle();               //Returns the window title
int SetWindowTitle(const char* title);      //Set window title text
void SetWindowSize(int width, int height);  //Sets the window size
const char* GetEngineVersion();             //Returns the version of wengine.lib
const w_Font* GetDefaultFont();             //Get a pointer to the default font
void SetVSync(VSYNC_STATE);                 //Enable / Disable vsync. Only works when renderer is opengl
VSYNC_STATE GetVSync();                     //Get current vsync state
void SetUserBlitRectState(char s);          //Set the state for userBlitRect, which dictates whether sw renderer uses user provided rect for blit
void SetUserBlitDestRect(int x, int y, int w, int h);   //Set the blit dest rect
void SetUserBlitSrcRect(int x, int y, int w, int h);   //Set the blit src rect

#endif