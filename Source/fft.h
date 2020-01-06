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

class FFT
{
public:

	FFT(int size)
	{
		tableSize = size;
		fftTable.resize(tableSize);
	}

	FFT()
	{

	}

	void fft(float *table, int size)
	{
		tableSize = size;
		fftTable.resize(tableSize);
		for (int i = 0; i < tableSize; i++)
		{
			fftTable[i].real(table[i] + 1.0f);
			fftTable[i].imag(0.0);
		}
		fftAlgorithm(fftTable);
		FFTForm = 1;
	}

	void ifft()
	{
		if (FFTForm == 0)
		{
			throw std::runtime_error("Couldnt perform IFFT");
			return;
		}
		// conjugate the complex numbers
		fftTable = fftTable.apply(std::conj);
		// forward fft
		fftAlgorithm(fftTable);

		// conjugate the complex numbers again
		fftTable = fftTable.apply(std::conj);

		// scale the numbers
		fftTable /= fftTable.size();
		FFTForm = 0;
	}

	void removeHarmonic(int harm)
	{
		if (harm > tableSize)
		{
			throw std::runtime_error("FFT Error: Harmonic out of range");
		}
		fftTable[harm].real(0.);
		fftTable[harm].imag(0.);
	}

	void removeHarmonics(int harmStart, int harmEnd)
	{
		if ((harmEnd > tableSize) || (harmStart > harmEnd))
		{
			throw std::runtime_error("FFT Error: Harmonic range problem");
		}
		for (int i = harmStart; i < harmEnd; i++)
		{
			fftTable[i].real(0.);
			fftTable[i].imag(0.);
		}
	}

	float getReal(int harmonic)
	{
		return real(fftTable[harmonic]);
	}
	float getImag(int harmonic)
	{
		return imag(fftTable[harmonic]);
	}
	void setReal(int harmonic, float value)
	{
		fftTable[harmonic].real(value);
	}
	void setImag(int harmonic, float value)
	{
		fftTable[harmonic].imag(value);
	}

	void returnWave(float *table, int size)
	{
		if (size != tableSize)
		{
			throw std::runtime_error("FFT Error: Incorrect Size");
		}

		for (int i = 0; i < tableSize; i++)
		{
			table[i] = std::sqrt(real(fftTable[i])*real(fftTable[i]) + imag(fftTable[i])*imag(fftTable[i])) - 1.0f;
		}

	}

	bool isFFTForm()
	{
		return FFTForm;
	}

	void print()
	{
		std::cout << "PRINT" << std::endl;
		for (int i = 0; i < tableSize; i++)
		{
			std::cout << fftTable[i] << std::endl;
		}
	}

private:

	const double PI = 3.141592653589793238460;

	typedef std::complex<double> Complex;
	typedef std::valarray<Complex> CArray;

	CArray fftTable;
	int tableSize;
	bool FFTForm;

	void fftAlgorithm(CArray &x)
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
#pragma once
