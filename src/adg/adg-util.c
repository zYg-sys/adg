/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007,2008,2009,2010  Nicola Fontana <ntd at entidi.it>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301, USA.
 */


/**
 * SECTION:adg-util
 * @Section_Id:utilities
 * @title: Utilities
 * @short_description: Assorted macros and functions
 *
 * Collection of macros and functions that do not fit inside any other topic.
 **/

/**
 * ADG_DIR_RIGHT:
 *
 * Symbolic constant for the right direction (in radians).
 **/

/**
 * ADG_DIR_DOWN:
 *
 * Symbolic constant for the down direction (in radians).
 **/

/**
 * ADG_DIR_LEFT:
 *
 * Symbolic constant for the left direction (in radians).
 **/

/**
 * ADG_DIR_UP:
 *
 * Symbolic constant for the up direction (in radians).
 **/

/**
 * ADG_FORWARD_DECL:
 * @id: The name of a struct
 *
 * Forward declaration of struct @id. It is equivalent to a typical
 * struct forward declaration, for example:
 *
 * |[
 * ADG_FORWARD_DECL(test)
 * ]|
 *
 * will expand to:
 *
 * |[
 * typedef struct _test test
 * ]|
 *
 * This macro is needed to fake <command>gtk-doc</command>, because
 * up to now (v.1.12) it generates two conflicting links when using
 * forward declarations: the first in the source with the declaration
 * and the second where the type is defined. Using ADG_FORWARD_DECL()
 * instead of the usual typedef avoids the parsing of the declaration
 * in the first file (<command>gtk-doc</command> is not able to do C
 * preprocessing).
 *
 * The same principle can be applied in the definition file. Following
 * the previous example, you can use something like this where struct
 * _type is defined:
 *
 * |[
 * #if 0
 * // This is declared in another file
 * typedef struct _type type;
 * #endif
 * struct _type {
 * ...
 * };
 * ]|
 **/


#include "adg-internal.h"
#include "adg-util.h"
#include <string.h>
#include <limits.h>


#if GLIB_CHECK_VERSION(2, 16, 0)
#else
/**
 * adg_strcmp:
 * @s1: first string to compare
 * @s2: second string to compare
 *
 * A strcmp() function guarded against %NULL values. If glib-2.16.0 or
 * greather is used, adg_strcmp() will be an alias for g_strcmp0.
 * Otherwise the function will behave in the same way as strcmp() with
 * the following exceptions:
 *
 * <itemizedlist>
 * <listitem><code>s1 == NULL && s2 == NULL</code>: returns %0;</listitem>
 * <listitem><code>s1 == NULL</code>: returns %INT_MIN;</listitem>
 * <listitem><code>s2 == NULL</code>: returns %INT_MAX.</listitem>
 * </itemizedlist>
 *
 * Returns: %0 if @s1 matches @s2, a value less than %0 if @s1 is less
 *          than @s2 or greather than %0 if @s1 is greather than @s2
 **/
gint
adg_strcmp(const gchar *s1, const gchar *s2)
{
    /* This will also catch the NULL == NULL case */
    if (s1 == s2)
        return 0;

    if (s1 == NULL)
        return INT_MIN;

    if (s2 == NULL)
        return INT_MAX;

    return strcmp(s1, s2);
}
#endif

/**
 * adg_is_string_empty:
 * @str: the subject string
 *
 * Checks if @str is an empty string, that is if is %NULL or if
 * its first character is %'\0'.
 *
 * Returns: %TRUE if @str is an empty string, %FALSE otherwise
 **/
gboolean
adg_is_string_empty(const gchar *str)
{
    return str == NULL || str[0] == '\0';
}

/**
 * adg_is_enum_value:
 * @value: the enum value to check
 * @enum_type: a #GEnum based type
 *
 * Checks if @value is a valid @enum_type value.
 *
 * Returns: %TRUE if @value is a valid @enum_type, %FALSE otherwise
 **/
gboolean
adg_is_enum_value(int value, GType enum_type)
{
    GEnumClass *enum_class;
    gboolean found;

    enum_class = g_type_class_ref(enum_type);
    g_return_val_if_fail(enum_class != NULL, FALSE);

    found = FALSE;

    if (value >= enum_class->minimum && value <= enum_class->maximum) {
        GEnumValue *enum_value;
        guint n;

        for (n = 0; !found && n < enum_class->n_values; ++n) {
            enum_value = enum_class->values + n;
            found = value == enum_value->value;
        }
    }

    g_type_class_unref(enum_class);

    return found;
}

/**
 * adg_is_boolean_value:
 * @value: the gboolean value to check
 *
 * Checks if @value is a valid #gboolean value, that is if it is %TRUE
 * or %FALSE. No other values are accepted.
 *
 * Returns: %TRUE if @value is a valid #gboolean, %FALSE otherwise
 **/
gboolean
adg_is_boolean_value(gboolean value)
{
    return value == TRUE || value == FALSE;
}