/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*-
 *
 * Copyright (C) 2007-2008 Richard Hughes <richard@hughsie.com>
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

#include <glib.h>

/**
 * egg_color_from_rgb:
 * @red: The red value
 * @green: The green value
 * @blue: The blue value
 **/
guint32
egg_color_from_rgb (guint8 red, guint8 green, guint8 blue)
{
	guint32 colour = 0;
	colour += (guint32) red * 0x10000;
	colour += (guint32) green * 0x100;
	colour += (guint32) blue;
	return colour;
}

/**
 * egg_color_to_rgb:
 * @red: The red value
 * @green: The green value
 * @blue: The blue value
 **/
void
egg_color_to_rgb (guint32 colour, guint8 *red, guint8 *green, guint8 *blue)
{
	*red = (colour & 0xff0000) / 0x10000;
	*green = (colour & 0x00ff00) / 0x100;
	*blue = colour & 0x0000ff;
}

/***************************************************************************
 ***                          MAKE CHECK TESTS                           ***
 ***************************************************************************/
#ifdef EGG_BUILD_TESTS
#include "egg-self-test.h"

void
egg_st_color (EggSelfTest *test)
{
	guint8 r, g, b;
	guint32 colour;

	if (egg_st_start (test, "EggColor") == FALSE) {
		return;
	}

	/************************************************************/
	egg_st_title (test, "get red");
	egg_color_to_rgb (0xff0000, &r, &g, &b);
	if (r == 255 && g == 0 && b == 0) {
		egg_st_success (test, "got red");
	} else {
		egg_st_failed (test, "could not get red (%i, %i, %i)", r, g, b);
	}

	/************************************************************/
	egg_st_title (test, "get green");
	egg_color_to_rgb (0x00ff00, &r, &g, &b);
	if (r == 0 && g == 255 && b == 0) {
		egg_st_success (test, "got green");
	} else {
		egg_st_failed (test, "could not get green (%i, %i, %i)", r, g, b);
	}

	/************************************************************/
	egg_st_title (test, "get blue");
	egg_color_to_rgb (0x0000ff, &r, &g, &b);
	if (r == 0 && g == 0 && b == 255) {
		egg_st_success (test, "got blue");
	} else {
		egg_st_failed (test, "could not get blue (%i, %i, %i)", r, g, b);
	}

	/************************************************************/
	egg_st_title (test, "get black");
	egg_color_to_rgb (0x000000, &r, &g, &b);
	if (r == 0 && g == 0 && b == 0) {
		egg_st_success (test, "got black");
	} else {
		egg_st_failed (test, "could not get black (%i, %i, %i)", r, g, b);
	}

	/************************************************************/
	egg_st_title (test, "get white");
	egg_color_to_rgb (0xffffff, &r, &g, &b);
	if (r == 255 && g == 255 && b == 255) {
		egg_st_success (test, "got white");
	} else {
		egg_st_failed (test, "could not get white (%i, %i, %i)", r, g, b);
	}

	/************************************************************/
	egg_st_title (test, "set red");
	colour = egg_color_from_rgb (0xff, 0x00, 0x00);
	if (colour == 0xff0000) {
		egg_st_success (test, "set red");
	} else {
		egg_st_failed (test, "could not set red (%i)", colour);
	}

	/************************************************************/
	egg_st_title (test, "set green");
	colour = egg_color_from_rgb (0x00, 0xff, 0x00);
	if (colour == 0x00ff00) {
		egg_st_success (test, "set green");
	} else {
		egg_st_failed (test, "could not set green (%i)", colour);
	}

	/************************************************************/
	egg_st_title (test, "set blue");
	colour = egg_color_from_rgb (0x00, 0x00, 0xff);
	if (colour == 0x0000ff) {
		egg_st_success (test, "set blue");
	} else {
		egg_st_failed (test, "could not set blue (%i)", colour);
	}

	/************************************************************/
	egg_st_title (test, "set white");
	colour = egg_color_from_rgb (0xff, 0xff, 0xff);
	if (colour == 0xffffff) {
		egg_st_success (test, "set white");
	} else {
		egg_st_failed (test, "could not set white (%i)", colour);
	}
}

#endif
