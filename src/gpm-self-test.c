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
#include <stdlib.h>
#include <glib.h>
#include <string.h>
#include <glib/gi18n.h>
#include <libgnomeui/gnome-ui-init.h>
#include <dbus/dbus-glib.h>

#include "gpm-common.h"
#include "gpm-debug.h"
#include "gpm-powermanager.h"
#include "gpm-proxy.h"

guint test_total = 0;
guint test_suc = 0;
gchar *test_type = NULL;

void
test_title (const gchar *format, ...)
{
	va_list args;
	gchar va_args_buffer [1025];
	va_start (args, format);
	g_vsnprintf (va_args_buffer, 1024, format, args);
	va_end (args);
	g_print ("> check #%u\t%s: \t%s...", test_total+1, test_type, va_args_buffer);
	test_total++;
}

void
test_success (const gchar *format, ...)
{
	va_list args;
	gchar va_args_buffer [1025];
	test_suc++;
	if (format == NULL) {
		g_print ("...OK\n");
		return;
	}
	va_start (args, format);
	g_vsnprintf (va_args_buffer, 1024, format, args);
	va_end (args);
	g_print ("...OK [%s]\n", va_args_buffer);
}

void
test_failed (const gchar *format, ...)
{
	va_list args;
	gchar va_args_buffer [1025];
	va_start (args, format);
	g_vsnprintf (va_args_buffer, 1024, format, args);
	va_end (args);
	g_print ("...FAILED [%s]\n", va_args_buffer);
}

void
test_proxy (GpmPowermanager *powermanager)
{
	GpmProxy *gproxy = NULL;
	DBusGProxy *proxy = NULL;

	test_type = "proxy  ";

	/************************************************************/
	test_title ("make sure we can get a new gproxy");
	gproxy = gpm_proxy_new ();
	if (gproxy != NULL) {
		test_success ("got gproxy");
	} else {
		test_failed ("could not get gproxy");
	}

	/************************************************************/
	test_title ("make sure proxy if NULL when no assign");
	proxy = gpm_proxy_get_proxy (gproxy);
	if (proxy == NULL) {
		test_success ("got NULL proxy");
	} else {
		test_failed ("did not get NULL proxy");
	}

	/************************************************************/
	test_title ("make sure we can assign and connect");
	proxy = gpm_proxy_assign (gproxy,
				  GPM_PROXY_SESSION,
				  GPM_DBUS_SERVICE,
				  GPM_DBUS_PATH_INHIBIT,
				  GPM_DBUS_INTERFACE_INHIBIT);
	if (proxy != NULL) {
		test_success ("got proxy (init)");
	} else {
		test_failed ("could not get proxy (init)");
	}

	/************************************************************/
	test_title ("make sure proxy non NULL when assigned");
	proxy = gpm_proxy_get_proxy (gproxy);
	if (proxy != NULL) {
		test_success ("got valid proxy");
	} else {
		test_failed ("did not get valid proxy");
	}

	g_object_unref (gproxy);	
}

void
test_inhibit (GpmPowermanager *powermanager)
{
	gboolean ret;
	gboolean valid;
	guint cookie1 = 0;
	guint cookie2 = 0;
	test_type = "inhibit";

	/************************************************************/
	test_title ("make sure we are not inhibited");
	ret = gpm_powermanager_is_valid (powermanager, FALSE, &valid);
	if (ret == FALSE) {
		test_failed ("Unable to test validity");
	} else if (valid == FALSE) {
		test_failed ("Already inhibited");
	} else {
		test_success (NULL);
	}

	/************************************************************/
	test_title ("clear an invalid cookie");
	ret = gpm_powermanager_uninhibit (powermanager, 123456);
	if (ret == FALSE) {
		test_success ("invalid cookie failed as expected");
	} else {
		test_failed ("should have rejected invalid cookie");
	}

	/************************************************************/
	test_title ("get auto cookie 1");
	ret = gpm_powermanager_inhibit_auto (powermanager,
				  "gnome-power-self-test",
				  "test inhibit",
				  &cookie1);
	if (ret == FALSE) {
		test_failed ("Unable to inhibit");
	} else if (cookie1 == 0) {
		test_failed ("Cookie invalid (cookie: %u)", cookie1);
	} else {
		test_success ("cookie: %u", cookie1);
	}

	/************************************************************/
	test_title ("make sure we are auto inhibited");
	ret = gpm_powermanager_is_valid (powermanager, FALSE, &valid);
	if (ret == FALSE) {
		test_failed ("Unable to test validity");
	} else if (valid == FALSE) {
		test_success ("inhibited");
	} else {
		test_failed ("inhibit failed");
	}

	/************************************************************/
	test_title ("make sure we are not manual inhibited");
	ret = gpm_powermanager_is_valid (powermanager, TRUE, &valid);
	if (ret == FALSE) {
		test_failed ("Unable to test validity");
	} else if (valid == TRUE) {
		test_success ("not inhibited for manual action");
	} else {
		test_failed ("inhibited auto but manual not valid");
	}

	/************************************************************/
	test_title ("get cookie 2");
	ret = gpm_powermanager_inhibit_auto (powermanager,
				  "gnome-power-self-test",
				  "test inhibit",
				  &cookie2);
	if (ret == FALSE) {
		test_failed ("Unable to inhibit");
	} else if (cookie2 == 0) {
		test_failed ("Cookie invalid (cookie: %u)", cookie2);
	} else {
		test_success ("cookie: %u", cookie2);
	}

	/************************************************************/
	test_title ("clear cookie 1");
	ret = gpm_powermanager_uninhibit (powermanager, cookie1);
	if (ret == FALSE) {
		test_failed ("cookie failed to clear");
	} else {
		test_success (NULL);
	}

	/************************************************************/
	test_title ("make sure we are still inhibited");
	ret = gpm_powermanager_is_valid (powermanager, FALSE, &valid);
	if (ret == FALSE) {
		test_failed ("Unable to test validity");
	} else if (valid == FALSE) {
		test_success ("inhibited");
	} else {
		test_failed ("inhibit failed");
	}

	/************************************************************/
	test_title ("clear cookie 2");
	ret = gpm_powermanager_uninhibit (powermanager, cookie2);
	if (ret == FALSE) {
		test_failed ("cookie failed to clear");
	} else {
		test_success (NULL);
	}

	/************************************************************/
	test_title ("make sure we are not auto inhibited");
	ret = gpm_powermanager_is_valid (powermanager, FALSE, &valid);
	if (ret == FALSE) {
		test_failed ("Unable to test validity");
	} else if (valid == FALSE) {
		test_failed ("Auto inhibited when we shouldn't be");
	} else {
		test_success (NULL);
	}

	/************************************************************/
	test_title ("make sure we are not manual inhibited");
	ret = gpm_powermanager_is_valid (powermanager, TRUE, &valid);
	if (ret == FALSE) {
		test_failed ("Unable to test validity");
	} else if (valid == FALSE) {
		test_failed ("Manual inhibited when we shouldn't be");
	} else {
		test_success (NULL);
	}

	/************************************************************/
	test_title ("get manual cookie 1");
	ret = gpm_powermanager_inhibit_manual (powermanager,
				  "gnome-power-self-test",
				  "test inhibit",
				  &cookie1);
	if (ret == FALSE) {
		test_failed ("Unable to manual inhibit");
	} else if (cookie1 == 0) {
		test_failed ("Cookie invalid (cookie: %u)", cookie1);
	} else {
		test_success ("cookie: %u", cookie1);
	}

	/************************************************************/
	test_title ("make sure we are auto inhibited");
	ret = gpm_powermanager_is_valid (powermanager, FALSE, &valid);
	if (ret == FALSE) {
		test_failed ("Unable to test validity");
	} else if (valid == FALSE) {
		test_success ("inhibited");
	} else {
		test_failed ("inhibit failed");
	}

	/************************************************************/
	test_title ("make sure we are manual inhibited");
	ret = gpm_powermanager_is_valid (powermanager, TRUE, &valid);
	if (ret == FALSE) {
		test_failed ("Unable to test validity");
	} else if (valid == FALSE) {
		test_success ("inhibited");
	} else {
		test_failed ("inhibit failed");
	}

	/************************************************************/
	test_title ("clear cookie 1");
	ret = gpm_powermanager_uninhibit (powermanager, cookie1);
	if (ret == FALSE) {
		test_failed ("cookie failed to clear");
	} else {
		test_success (NULL);
	}

}

int
main (int argc, char **argv)
{
	GOptionContext  *context;
 	GnomeProgram    *program;
	GpmPowermanager *powermanager;
	int retval;

	context = g_option_context_new ("GNOME Power Manager Self Test");
	program = gnome_program_init (argv[0], VERSION, LIBGNOMEUI_MODULE,
			    argc, argv,
			    GNOME_PROGRAM_STANDARD_PROPERTIES,
			    GNOME_PARAM_GOPTION_CONTEXT, context,
			    GNOME_PARAM_HUMAN_READABLE_NAME,
			    "Power Inhibit Test",
			    NULL);
	gpm_debug_init (FALSE);

	powermanager = gpm_powermanager_new ();
	if (powermanager == NULL) {
		g_warning ("Unable to get connection to power manager");
		return 1;
	}

	test_inhibit (powermanager);
	test_proxy (powermanager);

	g_print ("test passes (%u/%u) : ", test_suc, test_total);
	if (test_suc == test_total) {
		g_print ("ALL OKAY\n");
		retval = 0;
	} else {
		g_print ("%u FAILURE(S)\n", test_total - test_suc);
		retval = 1;
	}

	g_object_unref (powermanager);

//	g_object_unref (program);
	g_option_context_free (context);
	return retval;
}