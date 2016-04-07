#define _USE_MATH_DEFINES
#include <math.h>
#include "MatlabVector.h"
#include <complex>

#include <mkl_types.h>
#include <mkl.h>
#include <mkl_dfti.h>

#include "Preferences.h"
#include <specialfunctions.h>

// ������� ���������� �������������� ����� �������� a � b ��� �������� �������� �������
// ������ a � ������ b ������ ���� ����������� �������
MatlabVector conv_fourier_lib(MatlabVector a, MatlabVector b)
{
	static uint size_a = 0, size_b = 0;
	static MKL_Complex16 *nA, *nB, *fA, *fB;
	static DFTI_DESCRIPTOR_HANDLE fpa, fpb, bpa;

	if (size_a != a.size() || size_b != b.size())
	{
		size_a = a.size(); size_b = b.size();
		DftiFreeDescriptor(&fpa); DftiFreeDescriptor(&bpa); DftiFreeDescriptor(&fpb);
		if (nA) mkl_free(nA);
		if (nB) mkl_free(nB);
		if (fA) mkl_free(fA);
		if (fB) mkl_free(fB);

		nA = (MKL_Complex16*)mkl_malloc(sizeof(MKL_Complex16) * size_a, 0);
		nB = (MKL_Complex16*)mkl_malloc(sizeof(MKL_Complex16) * size_b, 0);
		fA = (MKL_Complex16*)mkl_malloc(sizeof(MKL_Complex16) * size_a, 0);
		fB = (MKL_Complex16*)mkl_malloc(sizeof(MKL_Complex16) * size_b, 0);

		DftiCreateDescriptor(&fpa, DFTI_DOUBLE, DFTI_COMPLEX, 1, size_a);
		DftiCreateDescriptor(&fpb, DFTI_DOUBLE, DFTI_COMPLEX, 1, size_b);
		DftiCreateDescriptor(&bpa, DFTI_DOUBLE, DFTI_COMPLEX, 1, size_a);
		DftiCommitDescriptor(fpa);
		DftiCommitDescriptor(fpb);
		DftiCommitDescriptor(bpa);
	}

	for (uint i = 0; i < a.size(); i++)
	{
		nA[i].real = a[i];
		nA[i].imag = 0;
	}
	DftiComputeForward(fpa, nA, fA);

	for (uint i = 0; i < b.size(); i++)
	{
		nB[i].real = b[i];
		nB[i].imag = 0;
	}
	DftiComputeForward(fpb, nB, fB);

	if (a.size() != b.size()) throw new char*("a.size != b.size");
	for (uint i = 0; i < a.size(); i++) // ����� �� �������, ���� ������� ������� �������. �� ����� �� ������ ���������.
	{
		auto tmp_compl = std::complex<num>(fA[i].real, fA[i].imag) * std::complex<num>(fB[i].real, fB[i].imag);
		fA[i].real = tmp_compl.real();
		fA[i].imag = tmp_compl.imag();
	}

	DftiComputeBackward(bpa, fA, nA);

	MatlabVector res = MatlabVector(a.size());
	for (uint i = 0; i < a.size(); i++)
	{
		a[i] = nA[i].real;
	}

	return res;
}

// ������ �������������� �������
MatlabVector hankel_2D_direct(MatlabVector f)
{
	MatlabVector res(f.size());
	for (uint i = 0; i < res.size(); i++)
	{
		res[i] = 0;
		MatlabVector tmp = g_rh * f;
		for (uint j = 0; j < tmp.size(); j++)
		{
			res[i] += tmp[j] * alglib::besselj0((g_rh)[j] * (g_rh)[i]);
		}
	}
	return res;
}

// ��������� �������������� ����� ���������-������������ �������
// �������� �������� ������� ��������� ������ ����
MatlabVector conv_radial_2D(MatlabVector a, MatlabVector b)
{
	MatlabVector res;
	// res = \frac{ 1 }{2\pi} H[(2\pi) ^ 2 H[f] \cdot H[g]]
	// so, we need H
	res = hankel_2D_direct(hankel_2D_direct(a) * hankel_2D_direct(b) * 4 * M_PI * M_PI) * (1 / (2 * M_PI));
	return res;
}

// ������� "���������"
// �������� ��������� �������� ��� �������
MatlabVector conv(MatlabVector a, MatlabVector b, int) // int � ����� ��������������
{
	return conv_radial_2D(a, b);
}