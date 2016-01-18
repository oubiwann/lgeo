/*
 *     Copyright 2011 Couchbase, Inc.
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 */
#include "lgeo_geos.h"


/* From https://github.com/iamaleksey/iconverl/blob/master/c_src/iconverl.c */
static int
load(ErlNifEnv *env, void **priv, ERL_NIF_TERM load_info)
{
    initGEOS(notice_handler, error_handler);

    GEOSGEOM_RESOURCE = enif_open_resource_type(
        env, NULL, "geosgeom_resource", &geom_destroy,
        ERL_NIF_RT_CREATE | ERL_NIF_RT_TAKEOVER, NULL);

    return 0;
}

void
unload(ErlNifEnv* env, void* priv_data)
{
    finishGEOS();
}


/************************************************************************
 *
 *  Binary predicates - return 2 on exception, 1 on true, 0 on false
 *
 ***********************************************************************/

/*
Geom1 = lgeo_geos_geom:to_geom({'Point',[5,5]}),
Geom2 = lgeo_geos_geom:to_geom({'LineString', [[1,1],[14,14]]}),
lgeo_geos_geom:disjoint(Geom1, Geom2).
false
*/
static ERL_NIF_TERM
disjoint(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    GEOSGeometry **geom1;
    GEOSGeometry **geom2;

    if (argc != 2) {
        return enif_make_badarg(env);
    }

    if(!enif_get_resource(env, argv[0], GEOSGEOM_RESOURCE, (void**)&geom1)) {
        return enif_make_badarg(env);
    }

    if(!enif_get_resource(env, argv[1], GEOSGEOM_RESOURCE, (void**)&geom2)) {
        return enif_make_badarg(env);
    }

    int result;
    if ((result = GEOSDisjoint(*geom1, *geom2)) == 1) {
        return enif_make_atom(env, "true");
    } else if (result == 0) {
        return enif_make_atom(env, "false");
    } else {
        return enif_make_atom(env, "error");
    }
}

/*
Geom1 = lgeo_geos_geom:to_geom({'LineString', [[3,3],[10,10]]}),
Geom2 = lgeo_geos_geom:to_geom({'LineString', [[1,1],[7,7]]}),
lgeo_geos_geom:intersects(Geom1, Geom2).
true
*/
static ERL_NIF_TERM
intersects(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    GEOSGeometry **geom1;
    GEOSGeometry **geom2;

    if (argc != 2) {
        return enif_make_badarg(env);
    }

    if(!enif_get_resource(env, argv[0], GEOSGEOM_RESOURCE, (void**)&geom1)) {
        return enif_make_badarg(env);
    }
    if(!enif_get_resource(env, argv[1], GEOSGEOM_RESOURCE, (void**)&geom2)) {
        return enif_make_badarg(env);
    }

    int result;
    if ((result = GEOSIntersects(*geom1, *geom2)) == 1 ) {
        return enif_make_atom(env, "true");
    } else if (result == 0) {
        return enif_make_atom(env, "false");
    } else {
        return enif_make_atom(env, "error");
    }
}


/************************************************************************
 *
 * Topology operations - return NULL on exception.
 *
 ***********************************************************************/

/*
Geom1 = lgeo_geos_geom:to_geom({'LineString', [[3,3],[10,10]]}),
Geom2 = lgeo_geos_geom:to_geom({'LineString', [[1,1],[7,7]]}),
Geom3 = lgeo_geos_geom:intersection(Geom1, Geom2),
lgeo_geos_geom:from_geom(Geom3).
{'LineString', [[3,3],[7,7]]}
*/
static ERL_NIF_TERM
intersection(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    GEOSGeometry **geom1;
    GEOSGeometry **geom2;
    ERL_NIF_TERM eterm;

    if (argc != 2) {
        return enif_make_badarg(env);
    }

    if(!enif_get_resource(env, argv[0], GEOSGEOM_RESOURCE, (void**)&geom1)) {
        return enif_make_badarg(env);
    }

    if(!enif_get_resource(env, argv[1], GEOSGEOM_RESOURCE, (void**)&geom2)) {
        return enif_make_badarg(env);
    }

    GEOSGeometry **result_geom = \
        enif_alloc_resource(GEOSGEOM_RESOURCE, sizeof(GEOSGeometry*));
    *result_geom = GEOSIntersection(*geom1, *geom2);

    if (*result_geom == NULL) {
        eterm = enif_make_atom(env, "undefined");
    } else {
        eterm = enif_make_tuple2(env,
            enif_make_atom(env, "ok"),
            enif_make_resource(env, result_geom));
        enif_release_resource(result_geom);
    }
    return eterm;
}

/*
Geom1 = lgeo_geos_geom:to_geom({'LineString', [[4,4],[10,10]]}),
Geom2 = lgeo_geos_geom:get_centroid_geom(Geom1),
lgeo_geos_geom:from_geom(Geom2).
{'Point',[7.0,7.0]}
*/
static ERL_NIF_TERM
get_centroid(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    GEOSGeometry **geom;
    ERL_NIF_TERM eterm;

    if (argc != 1) {
        return enif_make_badarg(env);
    }

    if(!enif_get_resource(env, argv[0], GEOSGEOM_RESOURCE, (void**)&geom)) {
        return enif_make_badarg(env);
    }

    GEOSGeometry **result_geom = \
        enif_alloc_resource(GEOSGEOM_RESOURCE, sizeof(GEOSGeometry*));
    *result_geom = GEOSGetCentroid(*geom);

    if (*result_geom == NULL) {
        eterm = enif_make_atom(env, "undefined");
    } else {
        eterm = enif_make_tuple2(env,
            enif_make_atom(env, "ok"),
            enif_make_resource(env, result_geom));
        enif_release_resource(result_geom);
    }
    return eterm;
}

/*
Geom1 = lgeo_geos_geom:to_geom({'LineString', [[4,4], [4.5, 4.5], [10,10]]}),
lgeo_geos_geom:topology_preserve_simplify(Geom1, 1).
{'LineString',[[4.0,4.0],[10.0,10.0]]}
*/
static ERL_NIF_TERM
topology_preserve_simplify(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    GEOSGeometry **geom;
    GEOSGeometry *simpler_geom;
    double dbl_tol;
    int int_tol;
    ERL_NIF_TERM eterm;

    if (argc != 2) {
        return enif_make_badarg(env);
    }

    if(!enif_get_resource(env, argv[0], GEOSGEOM_RESOURCE, (void**)&geom)) {
        return enif_make_badarg(env);
    }
    if (enif_get_int(env, argv[1], &int_tol)) {
        dbl_tol = int_tol;
    }
    else if (!enif_get_double(env, argv[1], &dbl_tol)) {
        return enif_make_badarg(env);
    }

    simpler_geom = GEOSTopologyPreserveSimplify(*geom, dbl_tol);
    eterm = geom_to_eterm(env, simpler_geom);
    GEOSGeom_destroy(simpler_geom);
    return eterm;
}


/************************************************************************
 *
 *  Validity checking
 *
 ***********************************************************************/
/*
Geom1 = lgeo_geos_geom:to_geom({'LineString', [[4,4], [4.5, 4.5], [10,10]]}),
lgeo_geos_geom:is_valid(Geom1).
true
Geom2 = lgeo_geos_geom:wktreader_read(WktReader,"POLYGON((0 0, 1 1, 1 2, 1 1, 0 0))"),
lgeo_geos_geom:is_valid(Geom2).
false
*/
static ERL_NIF_TERM
is_valid(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    GEOSGeometry **geom1;

    if (argc != 1) {
        return enif_make_badarg(env);
    }

    if(!enif_get_resource(env, argv[0], GEOSGEOM_RESOURCE, (void**)&geom1)) {
        return enif_make_badarg(env);
    }

    int isvalid;
    if ((isvalid = GEOSisValid(*geom1)) == 1 ) {
        return enif_make_atom(env, "true");
    }
    else if (isvalid == 0) {
        return enif_make_atom(env, "false");
    }
    else {
        return enif_make_atom(env, "error");
    }
}

static
ERL_NIF_TERM to_geom(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]) {
    ERL_NIF_TERM eterm;
    GEOSGeometry **geom = \
        enif_alloc_resource(GEOSGEOM_RESOURCE, sizeof(GEOSGeometry*));

    *geom = eterm_to_geom(env, argv);
    eterm = enif_make_resource(env, geom);
    enif_release_resource(geom);
    return eterm;
}

static
ERL_NIF_TERM from_geom(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]) {
    GEOSGeometry **geom;
    ERL_NIF_TERM eterm;

    if (argc != 1) {
        return enif_make_badarg(env);
    }

    if(!enif_get_resource(env, argv[0], GEOSGEOM_RESOURCE, (void**)&geom)) {
        return enif_make_badarg(env);
    }

    eterm = geom_to_eterm(env, *geom);
    return eterm;
}


static ErlNifFunc nif_funcs[] =
{
    {"disjoint", 2, disjoint},
    {"get_centroid", 1, get_centroid},
    {"intersection", 2, intersection},
    {"intersects", 2, intersects},
    {"is_valid", 1, is_valid},
    {"to_geom", 1, to_geom},
    {"from_geom", 1, from_geom},
    {"topology_preserve_simplify", 2, topology_preserve_simplify}
};

ERL_NIF_INIT(lgeo_geos_geom, nif_funcs, &load, NULL, NULL, unload);
