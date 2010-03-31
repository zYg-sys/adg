/* ADG - Automatic Drawing Generation
 * Copyright (C) 2010  Nicola Fontana <ntd at entidi.it>
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


#include "test-internal.h"


static void
_adg_test_org1(void)
{
    AdgADim *adim;
    AdgModel *model;
    AdgPoint *origin, *explicit_point, *model_point;
    AdgPoint *org1;

    adim = adg_adim_new();
    model = ADG_MODEL(adg_path_new());
    origin = adg_point_new();
    explicit_point = adg_point_new();
    model_point = adg_point_new();

    adg_point_set_pair_explicit(origin, 0, 0);
    adg_point_set_pair_explicit(explicit_point, 123, 321);
    adg_model_set_named_pair(model, "named-pair",
                             adg_point_get_pair(explicit_point));
    adg_point_set_pair_from_model(model_point, model, "named-pair");

    /* Ensure ADG does not consider an explicit point equals to
     * a point bound to a named pair with the same coordinates */
    g_assert(!adg_point_equal(explicit_point, model_point));

    org1 = adg_adim_get_org1(adim);
    g_assert(org1 == NULL);

    /* Using the public APIs */
    adg_adim_set_org1_explicit(adim, 0, 0);
    org1 = adg_adim_get_org1(adim);
    g_assert(adg_point_equal(org1, origin));

    adg_adim_set_org1(adim, NULL);
    org1 = adg_adim_get_org1(adim);
    g_assert(org1 == NULL);

    adg_adim_set_org1(adim, explicit_point);
    org1 = adg_adim_get_org1(adim);
    g_assert(adg_point_equal(org1, explicit_point));

    /* Checking the lazy assignment feature */
    adg_adim_set_org1_from_model(adim, model, "dummy");
    org1 = adg_adim_get_org1(adim);
    g_assert(org1 != NULL);
    g_assert(adg_point_get_pair(org1) == NULL);

    adg_adim_set_org1_from_model(adim, model, "named-pair");
    org1 = adg_adim_get_org1(adim);
    g_assert(adg_point_equal(org1, model_point));

    /* Using GObject property methods */
    g_object_set(adim, "org1", origin, NULL);
    g_object_get(adim, "org1", &org1, NULL);
    g_assert(adg_point_equal(org1, origin));
    adg_point_destroy(org1);

    g_object_set(adim, "org1", NULL, NULL);
    g_object_get(adim, "org1", &org1, NULL);
    g_assert(org1 == NULL);

    g_object_set(adim, "org1", explicit_point, NULL);
    g_object_get(adim, "org1", &org1, NULL);
    g_assert(adg_point_equal(org1, explicit_point));
    adg_point_destroy(org1);

    /* Checking the lazy assignment feature */
    adg_adim_set_org1_from_model(adim, model, "dummy");
    g_object_get(adim, "org1", &org1, NULL);
    g_assert(org1 != NULL);
    g_assert(adg_point_get_pair(org1) == NULL);
    adg_point_destroy(org1);

    g_object_set(adim, "org1", model_point, NULL);
    g_object_get(adim, "org1", &org1, NULL);
    g_assert(adg_point_equal(org1, model_point));
    adg_point_destroy(org1);

    adg_point_destroy(origin);
    adg_point_destroy(explicit_point);
    adg_point_destroy(model_point);
    g_object_unref(adim);
    g_object_unref(model);
}

static void
_adg_test_org2(void)
{
    AdgADim *adim;
    AdgModel *model;
    AdgPoint *origin, *explicit_point, *model_point;
    AdgPoint *org2;

    adim = adg_adim_new();
    model = ADG_MODEL(adg_path_new());
    origin = adg_point_new();
    explicit_point = adg_point_new();
    model_point = adg_point_new();

    adg_point_set_pair_explicit(origin, 0, 0);
    adg_point_set_pair_explicit(explicit_point, 123, 321);
    adg_model_set_named_pair(model, "named-pair",
                             adg_point_get_pair(explicit_point));
    adg_point_set_pair_from_model(model_point, model, "named-pair");

    /* Ensure ADG does not consider an explicit point equals to
     * a point bound to a named pair with the same coordinates */
    g_assert(!adg_point_equal(explicit_point, model_point));

    org2 = adg_adim_get_org2(adim);
    g_assert(org2 == NULL);

    /* Using the public APIs */
    adg_adim_set_org2_explicit(adim, 0, 0);
    org2 = adg_adim_get_org2(adim);
    g_assert(adg_point_equal(org2, origin));

    adg_adim_set_org2(adim, NULL);
    org2 = adg_adim_get_org2(adim);
    g_assert(org2 == NULL);

    adg_adim_set_org2(adim, explicit_point);
    org2 = adg_adim_get_org2(adim);
    g_assert(adg_point_equal(org2, explicit_point));

    /* Checking the lazy assignment feature */
    adg_adim_set_org2_from_model(adim, model, "dummy");
    org2 = adg_adim_get_org2(adim);
    g_assert(org2 != NULL);
    g_assert(adg_point_get_pair(org2) == NULL);

    adg_adim_set_org2_from_model(adim, model, "named-pair");
    org2 = adg_adim_get_org2(adim);
    g_assert(adg_point_equal(org2, model_point));

    /* Using GObject property methods */
    g_object_set(adim, "org2", origin, NULL);
    g_object_get(adim, "org2", &org2, NULL);
    g_assert(adg_point_equal(org2, origin));
    adg_point_destroy(org2);

    g_object_set(adim, "org2", NULL, NULL);
    g_object_get(adim, "org2", &org2, NULL);
    g_assert(org2 == NULL);

    g_object_set(adim, "org2", explicit_point, NULL);
    g_object_get(adim, "org2", &org2, NULL);
    g_assert(adg_point_equal(org2, explicit_point));
    adg_point_destroy(org2);

    /* Checking the lazy assignment feature */
    adg_adim_set_org2_from_model(adim, model, "dummy");
    g_object_get(adim, "org2", &org2, NULL);
    g_assert(org2 != NULL);
    g_assert(adg_point_get_pair(org2) == NULL);
    adg_point_destroy(org2);

    g_object_set(adim, "org2", model_point, NULL);
    g_object_get(adim, "org2", &org2, NULL);
    g_assert(adg_point_equal(org2, model_point));
    adg_point_destroy(org2);

    adg_point_destroy(origin);
    adg_point_destroy(explicit_point);
    adg_point_destroy(model_point);
    g_object_unref(adim);
    g_object_unref(model);
}


int
main(int argc, char *argv[])
{
    adg_test_init(&argc, &argv);

    adg_test_add_func("/adg/adim/org1", _adg_test_org1);
    adg_test_add_func("/adg/adim/org2", _adg_test_org2);

    return g_test_run();
}