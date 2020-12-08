#ifndef INPUT_H
#define INPUT_H
#include "types.h"

//Input functions
int GetKeyDown(int virtual_keycode);    //is true(1) for 1 frame after a key has been pressed
int GetKey(int virtual_keycode);        //is true(1) for as long as a key is pressed
int GetKeyUp(int virtual_keycode);      //is true(1) for 1 frame after a key has been released
int GetKeyRepeat(int virtual_keycode);  //is true(1) for 1 frame a key is repeating. Repeating is basically a new press from windows point of view, but we store pressed keys separately, so we can compare what is repeating
int GetScroll();                        //1 for scroll up, -1 for scroll down, 0 for no scroll
int GetMousePosition(w_LPoint* point);  //1 if successful, 0 if failed


#endif