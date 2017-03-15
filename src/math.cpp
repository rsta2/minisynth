//
// math.cpp
//
// MiniSynth Pi - A virtual analogue synthesizer for Raspberry Pi
// Copyright (C) 2017  R. Stange <rsta2@o2online.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
#include "math.h"
#include <assert.h>

float fabsf (float x)
{
#ifndef __circle__
	return x >= 0.0 ? x : -x;
#else
	float y;
	asm volatile ("fabss %0, %1" : "=w" (y) : "w" (x));

	return y;
#endif
}

float sqrtf (float x)
{
	assert (x >= 0);

#ifndef __circle__
	if (x == 0)
	{
		return 0;
	}

	float y = 1;
	while (fabsf (x - y*y) > 1e-4)
	{
		y = (y + x/y) / 2;
	}
#else
	float y;
	asm volatile ("fsqrts %0, %1" : "=w" (y) : "w" (x));
#endif

	return y;
}

float expf (float x)
{
	int s = 0;
	if (x < 0)
	{
		s = 1;
		x = -x;
	}

	unsigned n = 0;
	while (x > 0.01f)
	{
		x /= 2;
		n++;
	}

	float t = x+1;
	float y = (t*t + 1) / 2;

	while (n--)
	{
		y *= y;
	}

	if (s)
	{
		y = 1 / y;
	}

	return y;
}

float logf (float x)
{
	assert (x > 0);

	int s = 0;
	if (x < 1)
	{
		x = 1 / x;
		s = 1;
	}

	unsigned n = 0;
	while (x > 1.005f)
	{
		x  = sqrtf (x);
		n++;
	}

	float y = sqrtf (x+x - 1) - 1;

	while (n--)
	{
		y += y;
	}

	if (s)
	{
		y = -y;
	}

	return y;
}

float powf (float x, float e)
{
	if (x == 0.0)
	{
		return 0.0;
	}

	return expf (e * logf (x));
}

float cosf (float x)
{
	x = fabsf (x);

	unsigned n = 0;
	while (x > 0.3f)
	{
		x /= 2;
		n++;
	}

	float t = (x*x/2 - 3);
	float y = (t*t - 3) / 6;

	while (n--)
	{
		y = 2*y*y - 1;
	}

	return y;
}

float sinf (float x)
{
	return cosf (PI/2 - x);
}
