#ifndef APPLICATION_H
#define APPLICATION_H

#include "def.h"

typedef void(*w_VoidFunction)(void);


int w_ApplicationInit(int argc, char **argv, int windowWidth, int windowHeight, const char* windowTitle);
int w_ApplicationStart();
void w_RegisterStart(w_VoidFunction startfunction);
void w_RegisterUpdate(w_VoidFunction updatefunction);
void w_RegisterRender(w_VoidFunction renderfunction);
void w_RegisterOnExit(w_VoidFunction onexitfunction);
void w_SetFPSLimit(int fps);
void w_SetRenderOrientation(int mode);

#endif