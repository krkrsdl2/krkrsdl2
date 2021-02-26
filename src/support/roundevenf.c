
#include <math.h>
#include <stdint.h>
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
