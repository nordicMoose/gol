#ifndef HDEF_H
#define HDEF_H

#include "def.h"

#define INPUTKEYSC 46 //number of keys available to be used for inputfield input

//These variables and functions are not meant to be used by the user directly, but rather by the engine systems

extern w_Color *SCREENBUFFER;
extern w_Color *BACKGROUND;



extern int RNDITER;
extern char KEYSCHANGED;
extern char KEYUPFLAG;
extern char KEYDOWNFLAG;
extern char KEYREPEATFLAG;
extern char DISPLAYFLAG;
extern char EXITFLAG;
extern char SCRCHANGEDFLAG;
extern char USEBACKGROUND;

extern char keysPressed[MAX_KEYS];
extern int keysDown[MAX_KEYS];
extern int keysDownRepeat[MAX_KEYS];
extern int keysReleased[MAX_KEYS];

extern unsigned int keysDown_count;
extern unsigned int keysDownRepeat_count;
extern unsigned int keysReleased_count;

extern int SCROLLVALUE;
extern w_Image textbitmap;

extern const unsigned int INPUTFIELDMAXLEN;

extern w_Timer** program_timers;
extern unsigned int timer_count;

extern w_Inputfield** program_inputfields;
extern unsigned int inputfield_count;

extern const int INPUTFKEYS[INPUTKEYSC];
extern const char inputkeys_normal[INPUTKEYSC];
extern const char inputkeys_shift[INPUTKEYSC];
extern const char inputkeys_alt[INPUTKEYSC];


extern w_Font defaultfont;           //program default font
extern char* CWD;
extern char CAPSLOCK;
extern void** window_handle;
extern char** c_argv;                  
extern int c_argc;                     


//functions
void Init(int argc, char **argv);
void ResetKeyUpDown();
char* GetData(char* data, int startindex);
char* GetLData(char* data, int startindex, char letter);
int CropImage(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, w_Image image, w_Image* out); //MARKED FOR MOVING TO DEF
void IterateTimers();
int LoadDefaultFont(const char string[], w_Font* outfont);
void DoInputFields();
void RemoveCharInputField();
void AddCharInputField(int index);
void SplitArgs(char* cmdLine, char*** out_argv, int* out_argc);

//variable functions
void SendResizeMessage();
void SendRendermodeMessage(char mode);
void SetNewRenderMode(char mode);
void SetWindowTitleVar(const char* title);
void SetWindowTitleInternal(const char* title);
void InitWindowSize(int width, int height);
void SetWindowSizeOld();    //Called by resize handler in wndproc if the resizing fails
void SetDeltatime(double t);
void SetTotaltime(double t);
void ChangeVSyncState(VSYNC_STATE);

void SetWglSwapIntervalFunc(void*);

void SetOrientationPoints();

#endif