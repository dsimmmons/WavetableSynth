/*
  ==============================================================================

	fft.h
	Created: 9 Nov 2018 11:51:33pm
	Author:  Denver

	https://rosettacode.org/wiki/Fast_Fourier_transform#C.2B.2B

  ==============================================================================
*/

#pragma once

#include <complex>
#include <iostream>
#include <valarray>
#include "newFloatArray.h"

//INCOMPLETE
static class FFT
{
public:

	void ifft(float *table, int size)
	{
		CArray x;
		x.resize(size);
		for (int i = 0; i < size; i++)
		{
			x[i].real(table[i]);
			x[i].imag(0.0);
		}

		// conjugate the complex numbers
		x = x.apply(std::conj);
		// forward fft
		fftHelper(x);

		// conjugate the complex numbers again
		x = x.apply(std::conj);

		// scale the numbers
		x /= x.size();
		for (int i = 0; i < size; i++)
		{
			table[i] = sqrt(real(x[i])*real(x[i]) + imag(x[i])*imag(x[i]));
		}
	}

	void fft(float *table, int size)
	{
		CArray x;
		x.resize(size);
		for (int i = 0; i < size; i++)
		{
			x[i].real(table[i]);
			x[i].imag(0.0);
		}
		fftHelper(x);
		for (int i = 0; i < (size); i++)
		{
			table[i] = sqrt(real(x[i])*real(x[i]) + imag(x[i])*imag(x[i]));
		}
	}
private:

	const double PI = 3.141592653589793238460;

	typedef std::complex<double> Complex;
	typedef std::valarray<Complex> CArray;

	void fftHelper(CArray &x)
	{
		// DFT
		unsigned int N = x.size(), k = N, n;
		double thetaT = 3.14159265358979323846264338328L / N;
		Complex phiT = Complex(cos(thetaT), -sin(thetaT)), T;
		while (k > 1)
		{
			n = k;
			k >>= 1;
			phiT = phiT * phiT;
			T = 1.0L;
			for (unsigned int l = 0; l < k; l++)
			{
				for (unsigned int a = l; a < N; a += n)
				{
					unsigned int b = a + k;
					Complex t = x[a] - x[b];
					x[a] += x[b];
					x[b] = t * T;
				}
				T *= phiT;
			}
		}
		// Decimate
		unsigned int m = (unsigned int)log2(N);
		for (unsigned int a = 0; a < N; a++)
		{
			unsigned int b = a;
			// Reverse bits
			b = (((b & 0xaaaaaaaa) >> 1) | ((b & 0x55555555) << 1));
			b = (((b & 0xcccccccc) >> 2) | ((b & 0x33333333) << 2));
			b = (((b & 0xf0f0f0f0) >> 4) | ((b & 0x0f0f0f0f) << 4));
			b = (((b & 0xff00ff00) >> 8) | ((b & 0x00ff00ff) << 8));
			b = ((b >> 16) | (b << 16)) >> (32 - m);
			if (b > a)
			{
				Complex t = x[a];
				x[a] = x[b];
				x[b] = t;
			}
		}
	}

};
