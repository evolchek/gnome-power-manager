/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*-
 *
 * Copyright (C) 2007 Richard Hughes <richard@hughsie.com>
 *
 * Licensed under the GNU General Public License Version 2
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include "config.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <glib.h>

#include "gpm-debug.h"
#include "gpm-array.h"
#include "gpm-array-float.h"

/**
 * gpm_array_float_guassian_value:
 *
 * @x: input value
 * @sigma: sigma value
 * Return value: the gaussian, in floating point precision
 **/
gfloat
gpm_array_float_guassian_value (gfloat x, gfloat sigma)
{
	return (1.0 / (sqrtf(2.0*3.1415927) * sigma)) * (expf((-(powf(x,2.0)))/(2.0 * powf(sigma, 2.0))));
}

/**
 * gpm_array_float_new:
 *
 * @length: length of array
 * Return value: Allocate array
 *
 * Creates a new size array which is zeroed. Free with g_array_free();
 **/
GArray *
gpm_array_float_new (guint length)
{
	guint i;
	GArray *array;
	array = g_array_sized_new (TRUE, FALSE, sizeof(gfloat), length);
	array->len = length;

	/* clear to 0.0 */
	for (i=0; i<length; i++) {
		g_array_index (array, gfloat, i) = 0.0;
	}

	return array;
}

/**
 * gpm_array_float_get:
 *
 * @array: input array
 **/
inline gfloat
gpm_array_float_get (GArray *array, guint i)
{
	if (i >= array->len) {
		g_error ("above index! (%i)", i);
	}
	return g_array_index (array, gfloat, i);
}

/**
 * gpm_array_float_set:
 *
 * @array: input array
 **/
inline void
gpm_array_float_set (GArray *array, guint i, gfloat value)
{
	g_array_index (array, gfloat, i) = value;
}

/**
 * gpm_array_float_free:
 *
 * @array: input array
 *
 * Frees the array, deallocating data
 **/
void
gpm_array_float_free (GArray *array)
{
	g_array_free (array, FALSE);
}

/**
 * gpm_array_float_from_array_y:
 *
 * @arrayfloat: copy into
 * @array: copy from
 * Return value: success
 *
 * Converts a GpmArray->y to GpmArrayFloat
 **/
gboolean
gpm_array_float_from_array_y (GArray *arrayfloat, GpmArray *array)
{
	GpmArrayPoint *point;
	guint i;
	guint length;

	length = arrayfloat->len;
	/* copy from one structure to a quick 1D array */
	for (i=0; i<length; i++) {
		point = gpm_array_get (array, i);
		g_array_index (arrayfloat, gfloat, i) = point->y;
	}
	return TRUE;
}

/**
 * gpm_array_float_from_array_z:
 *
 * @arrayfloat: copy into
 * @array: copy from
 * Return value: success
 *
 * Converts a GpmArray->z to GpmArrayFloat
 **/
gboolean
gpm_array_float_from_array_z (GArray *arrayfloat, GpmArray *array)
{
	GpmArrayPoint *point;
	guint i;
	guint length;

	length = arrayfloat->len;
	/* copy from one structure to a quick 1D array */
	for (i=0; i<length; i++) {
		point = gpm_array_get (array, i);
		g_array_index (arrayfloat, gfloat, i) = point->data;
	}
	return TRUE;
}

/**
 * gpm_array_float_to_array_y:
 *
 * @arrayfloat: copy from
 * @array: copy into
 * Return value: success
 *
 * Converts a GpmArray->y to GpmArrayFloat
 **/
gboolean
gpm_array_float_to_array_y (GArray *arrayfloat, GpmArray *array)
{
	GpmArrayPoint *point;
	guint i;
	guint length;

	length = arrayfloat->len;
	/* copy from one structure to a slow 2D array */
	for (i=0; i<length; i++) {
		point = gpm_array_get (array, i);
		point->y = g_array_index (arrayfloat, gfloat, i);
	}
	return TRUE;
}

/**
 * gpm_array_float_to_array_z:
 *
 * @arrayfloat: copy from
 * @array: copy into
 * Return value: success
 *
 * Converts a GpmArray->z to GpmArrayFloat
 **/
gboolean
gpm_array_float_to_array_z (GArray *arrayfloat, GpmArray *array)
{
	GpmArrayPoint *point;
	guint i;
	guint length;

	length = arrayfloat->len;
	/* copy from one structure to a slow 2D array */
	for (i=0; i<length; i++) {
		point = gpm_array_get (array, i);
		point->data = g_array_index (arrayfloat, gfloat, i);
	}
	return TRUE;
}

/**
 * gpm_array_float_compute_gaussian:
 *
 * @length: length of output array
 * @sigma: sigma value
 * Return value: Gaussian array
 *
 * Create a set of Gaussian array of a specified size
 **/
GArray *
gpm_array_float_compute_gaussian (guint length, gfloat sigma)
{
	GArray *array;
	gint half_length;
	guint i;
	gfloat div;

	array = gpm_array_float_new (length);

	/* array positions 0..10, has to be an odd number */
	half_length = (length / 2) + 1;
	for (i=0; i<half_length; i++) {
		div = half_length - (i + 1);
		g_array_index (array, gfloat, i) = gpm_array_float_guassian_value (div, sigma);
	}

	/* no point working these out, we can just reflect the gaussian */
	for (i=half_length; i<length; i++) {
		div = g_array_index (array, gfloat, length-(i+1));
		g_array_index (array, gfloat, i) = div;
	}
	return array;
}

/**
 * gpm_array_float_sum:
 *
 * @array: input array
 *
 * Sum the elements of the array
 **/
gfloat
gpm_array_float_sum (GArray *array)
{
	guint length;
	guint i;
	gfloat total = 0;

	length = array->len;
	/* debug out */
	for (i=0; i<length; i++) {
		total += g_array_index (array, gfloat, i);
	}
	return total;
}

/**
 * gpm_array_float_print:
 *
 * @array: input array
 *
 * Print the array
 **/
gboolean
gpm_array_float_print (GArray *array)
{
	guint length;
	guint i;

	length = array->len;
	/* debug out */
	for (i=0; i<length; i++) {
		gpm_debug ("[%i]\tval=%f", i, g_array_index (array, gfloat, i));
	}
	return TRUE;
}

/**
 * gpm_array_float_convolve:
 *
 * @data: input array
 * @kernel: kernel array
 * Return value: Colvolved array, same length as data
 *
 * Convolves an array with a kernel, and returns an array the same size.
 * THIS FUNCTION IS REALLY SLOW...
 **/
GArray *
gpm_array_float_convolve (GArray *data, GArray *kernel)
{
	gint length_data;
	gint length_kernel;
	GArray *result;
	gfloat value;
	gint i;
	gint j;
	gint index;

	length_data = data->len;
	length_kernel = kernel->len;

	result = gpm_array_float_new (length_data);

	/* convolve */
	for (i=0;i<length_data;i++) {
		value = 0;
		for (j=0;j<length_kernel;j++) {
			index = i+j-(length_kernel/2);
			if (index < 0) {
				index = 0;
			} else if (index >= length_data) {
				index = length_data - 1;
			}
			value += g_array_index (data, gfloat, index) * g_array_index (kernel, gfloat, j);
		}
		g_array_index (result, gfloat, i) = value;
	}
	return result;
}

/**
 * gpm_array_float_compute_integral:
 * @array: This class instance
 *
 * Computes complete discrete integral of dataset.
 * Will only work with a step size of one.
 **/
gfloat
gpm_array_float_compute_integral (GArray *array, guint x1, guint x2)
{
	gfloat value;
	gint i;

	g_return_val_if_fail (x2 >= x1, 0.0);

	/* if the same point, then we have no area */
	if (x1 == x2) {
		return 0.0;
	}

	value = 0.0;
	for (i=x1; i <= x2; i++) {
		value += g_array_index (array, gfloat, i);
	}
	return value;
}
