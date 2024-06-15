/* SPDX-License-Identifier: MIT */
/* Copyright (c) Kirikiri SDL2 Developers */

#include <math.h>
#include <stdint.h>
#if defined(__clang__) && defined(__arm__)
#define roundevenf roundevenf_workaround
#endif
float roundevenf(float v)
{
	float rounded = roundf(v);
	float diff = rounded - v;
	if ((fabsf(diff) == 0.5f) && (((int32_t)rounded) & 1))
	{
		rounded = v - diff;
	}
	return rounded;
}
