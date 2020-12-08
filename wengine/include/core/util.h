#ifndef UTIL_H
#define UTIL_H

#include <assert.h>
#include <direct.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>

void swapp(void** a, void** b);
double clampf(double value, double min, double max);
int clampi(int value, int min, int max);
unsigned int clampui(unsigned int value, unsigned int min, unsigned int max);
char* GetWorkingDir();
void flip(unsigned int* a, unsigned int* b);
void flipi(int* a, int* b);
void flipf(float * a, float* b);
char* StrToLower(char * str, unsigned int len);
unsigned int Random(unsigned int min, unsigned int max);
int StringToLong(const char* str, long* out_value);
int StringToFloat(const char* str, float* out_value);
char* GetAvailableFileName(const char* format, ...);

#endif