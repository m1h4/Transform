// Code taken from:
// http://local.wasp.uwa.edu.au/~pbourke/miscellaneous/imagefilter/
// Image Filtering in the Frequency Domain
//   Written by Paul Bourke

#include "Globals.h"
#include "Transform.h"

int power(int number)
{
	int pwr = 0;

	while(number >>= 1)
		++pwr;

	return pwr;
}

/*-------------------------------------------------------------------------
   Perform a 2D FFT inplace given a complex 2D array
   The direction dir, 1 for forward, -1 for reverse
   The size of the array (nx,ny)
   Return false if there are memory problems or
      the dimensions are not powers of 2
*/
int FFT2D(COMPLEX_NUMBER **c,int nx,int ny,int dir)
{
	/* Transform the rows */
	COMPLEX_WORD* real = (COMPLEX_WORD*)malloc(nx*sizeof(COMPLEX_WORD));
	COMPLEX_WORD* imag = (COMPLEX_WORD*)malloc(nx*sizeof(COMPLEX_WORD));

	if(!real || !imag)
		return FALSE;

	for(long j = 0; j < ny; ++j)
	{
		for(long i = 0; i < nx; ++i)
		{
			real[i] = c[i][j].real;
			imag[i] = c[i][j].imag;
		}

		FFT(dir, nx, real, imag);

		for(long i = 0; i < nx; ++i)
		{
			c[i][j].real = real[i];
			c[i][j].imag = imag[i];
		}
	}

	free(real);
	free(imag);

	/* Transform the columns */
	real = (COMPLEX_WORD*)malloc(ny*sizeof(COMPLEX_WORD));
	imag = (COMPLEX_WORD*)malloc(ny*sizeof(COMPLEX_WORD));

	if(!real || !imag)
		return FALSE;

	for(long i = 0; i < nx; ++i)
	{
		for(long j = 0; j < ny; ++j)
		{
			real[j] = c[i][j].real;
			imag[j] = c[i][j].imag;
		}

		FFT(dir, ny, real, imag);
		
		for(long j = 0; j < ny; ++j)
		{
			c[i][j].real = real[j];
			c[i][j].imag = imag[j];
		}
	}

	free(real);
	free(imag);

	return TRUE;
}

/*-------------------------------------------------------------------------
   This computes an in-place complex-to-complex FFT
   x and y are the real and imaginary arrays of 2^m points.
   dir =  1 gives forward transform
   dir = -1 gives reverse transform

     Formula: forward
                  N-1
                  ---
              1   \          - j k 2 pi n / N
      X(n) = ---   >   x(k) e                    = forward transform
              N   /                                n=0..N-1
                  ---
                  k=0

      Formula: reverse
                  N-1
                  ---
                  \          j k 2 pi n / N
      X(n) =       >   x(k) e                    = forward transform
                  /                                n=0..N-1
                  ---
                  k=0
*/
int FFT(int dir, int nn, COMPLEX_WORD* x,COMPLEX_WORD* y)
{
	long i1,j,k,i2,l,l1,l2;
	COMPLEX_WORD c1,c2,tx,ty,t1,t2,u1,u2,z;

	/* Do the bit reversal */
	i2 = nn >> 1;
	j = 0;
	for(long i = 0; i < nn-1; ++i)
	{
		if(i < j)
		{
			tx = x[i];
			ty = y[i];
			x[i] = x[j];
			y[i] = y[j];
			x[j] = tx;
			y[j] = ty;
		}

		k = i2;
		while(k <= j)
		{
			j -= k;
			k >>= 1;
		}

		j += k;
	}

	/* Compute the FFT */
	c1 = -1.0;
	c2 = 0.0;
	l2 = 1;
	for(l = 0; l < power(nn); ++l)
	{
		l1 = l2;
		l2 <<= 1;
		u1 = 1.0;
		u2 = 0.0;

		for(j = 0; j < l1; ++j)
		{
		for(long i = j; i < nn; i += l2)
		{
			i1 = i + l1;
			t1 = u1 * x[i1] - u2 * y[i1];
			t2 = u1 * y[i1] + u2 * x[i1];
			x[i1] = x[i] - t1;
			y[i1] = y[i] - t2;
			x[i] += t1;
			y[i] += t2;
		}

		z =  u1 * c1 - u2 * c2;
		u2 = u1 * c2 + u2 * c1;
		u1 = z;
	}
	c2 = sqrtf((1.0f - c1) / 2.0f);
	if(dir == 1)
		c2 = -c2;
	c1 = sqrtf((1.0f + c1) / 2.0f);
	}

	/* Scaling for forward transform */
	if(dir == 1)
	{
		for(long i = 0; i < nn; ++i)
		{
			x[i] /= (COMPLEX_WORD)nn;
			y[i] /= (COMPLEX_WORD)nn;
		}
	}

	return TRUE;
}
