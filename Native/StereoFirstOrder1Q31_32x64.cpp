#include "stdafx.h"
#include <cmath>
#include <initializer_list>
#include "Constant.h"
#include "StereoFirstOrder1Q31_32x64.h"

namespace CrossTimeDsp::Dsp
{
	StereoFirstOrder1Q31_32x64::StereoFirstOrder1Q31_32x64(FirstOrderCoefficients coefficients)
	{
		// see remarks in BiquadQ31_32x64::BiquadQ31_32x64()
		this->postShift = Q31::GetQ31_32x64_PostShift(std::initializer_list<double>({ coefficients.A1, coefficients.B0, coefficients.B1 }));
		double coefficientScaling = std::pow(2.0, -this->postShift);
		this->a1 = new Q31(coefficientScaling * coefficients.A1, Q31::MaximumFractionalBits);
		this->b0 = new Q31(coefficientScaling * coefficients.B0, Q31::MaximumFractionalBits);
		this->b1 = new Q31(coefficientScaling * coefficients.B1, Q31::MaximumFractionalBits);

		__int32 channels = 2;
		this->x1 = new __int32[channels];
		this->y1 = new __int64[channels];
		for (__int32 channel = 0; channel < channels; ++channel)
		{
			this->x1[channel] = 0;
			this->y1[channel] = 0;
		}
	}

	StereoFirstOrder1Q31_32x64::~StereoFirstOrder1Q31_32x64()
	{
		delete this->a1;
		delete this->b0;
		delete this->b1;

		delete this->x1;
		delete this->y1;
	}

	void StereoFirstOrder1Q31_32x64::Filter(__int32* block, __int32 offset)
	{
		__int32 maxSample = offset + Constant::FilterBlockSizeInInt32s;
		for (__int32 sample = offset; sample < maxSample; sample += 2)
		{
			__int32 value0 = block[sample];
			__int64 accumulator0 = *(this->b0) * value0 + *(this->b1) * this->x1[0] - *(this->a1) * this->y1[0];
			this->x1[0] = value0;
			this->y1[0] = accumulator0 << (this->postShift + 1);
			block[sample] = (__int32)(accumulator0 >> (Q31::MaximumFractionalBits - this->postShift));

			__int32 value1 = block[sample + 1];
			__int64 accumulator1 = *(this->b0) * value1 + *(this->b1) * this->x1[1] - *(this->a1) * this->y1[1];
			this->x1[1] = value1;
			this->y1[1] = accumulator1 << (this->postShift + 1);
			block[sample + 1] = (__int32)(accumulator1 >> (Q31::MaximumFractionalBits - this->postShift));
		}
	}

	void StereoFirstOrder1Q31_32x64::FilterReverse(__int32* block, __int32 offset)
	{
		__int32 maxSample = offset + Constant::FilterBlockSizeInInt32s;
		for (__int32 sample = maxSample - 2; sample >= offset; sample -= 2)
		{
			__int32 value0 = block[sample];
			__int64 accumulator0 = *(this->b0) * value0 + *(this->b1) * this->x1[0] - *(this->a1) * this->y1[0];
			this->x1[0] = value0;
			this->y1[0] = accumulator0 << (this->postShift + 1);
			block[sample] = (__int32)(accumulator0 >> (Q31::MaximumFractionalBits - this->postShift));

			__int32 value1 = block[sample + 1];
			__int64 accumulator1 = *(this->b0) * value1 + *(this->b1) * this->x1[1] - *(this->a1) * this->y1[1];
			this->x1[1] = value1;
			this->y1[1] = accumulator1 << (this->postShift + 1);
			block[sample + 1] = (__int32)(accumulator1 >> (Q31::MaximumFractionalBits - this->postShift));
		}
	}
}