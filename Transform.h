#pragma once

typedef float COMPLEX_WORD;

typedef struct
{
	COMPLEX_WORD real;
	COMPLEX_WORD imag;
} COMPLEX_NUMBER;

int FFT2D(COMPLEX_NUMBER **c,int nx,int ny,int dir);
int FFT(int dir,int m,COMPLEX_WORD *x,COMPLEX_WORD *y);