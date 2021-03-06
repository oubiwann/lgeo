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

    GEOSSTRTREE_RESOURCE = enif_open_resource_type(
        env, NULL, "geosstrtree_resource", &strtree_destroy,
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
 *  STRtree functions
 *
 ***********************************************************************/

/*
extern GEOSSTRtree GEOS_DLL *GEOSSTRtree_create(size_t nodeCapacity);

GeosSTRtree = lgeo_geos_index:strtree_create().
<<>>
*/
static ERL_NIF_TERM
strtree_create(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    ERL_NIF_TERM eterm;

    GeosSTRtree_t **tree = \
        enif_alloc_resource(GEOSSTRTREE_RESOURCE, sizeof(GeosSTRtree_t*));

    size_t nodeCapacity = 10;
    GEOSSTRtree *rtree = GEOSSTRtree_create(nodeCapacity);
    ErlNifEnv *tree_env = enif_alloc_env();

    *tree = (GeosSTRtree_t*) enif_alloc(sizeof(GeosSTRtree_t));
    (*tree)->env = tree_env;
    (*tree)->tree = rtree;

    eterm = enif_make_resource(env, tree);
    enif_release_resource(tree);
    return eterm;
}

/*
extern void GEOS_DLL GEOSSTRtree_insert(GEOSSTRtree *tree,
                                        const GEOSGeometry *g,
                                        void *item);

GeosSTRtree = lgeo_geos_index:strtree_create(),
Element = {element, 1},
Geom = lgeo_geos_index:to_geom({'LineString', [[4,4], [4.5, 4.5], [10,10]]}),
lgeo_geos_index:strtree_insert(GeosSTRtree, Geom, Element).
ok
*/
static ERL_NIF_TERM
strtree_insert(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    GeosSTRtree_t **tree;
    GEOSGeometry **geom;

    if (argc != 3) {
        return enif_make_badarg(env);
    }

    if(!enif_get_resource(env, argv[0], GEOSSTRTREE_RESOURCE, (void**)&tree)) {
        return enif_make_badarg(env);
    }

    if(!enif_get_resource(env, argv[1], GEOSGEOM_RESOURCE, (void**)&geom)) {
        return enif_make_badarg(env);
    }
    ERL_NIF_TERM copy = enif_make_copy((**tree).env, argv[2]);
    GEOSSTRtree_insert((**tree).tree, GEOSEnvelope(*geom), (void *)copy);
    return enif_make_atom(env, "ok");
}

/*
extern void GEOS_DLL GEOSSTRtree_query(GEOSSTRtree *tree,
                                       const GEOSGeometry *g,
                                       GEOSQueryCallback callback,
                                       void *userdata);

GeosSTRtree = lgeo_geos_index:strtree_create(),
Element1 = {'LineString', [[4.0,4.0], [4.5, 4.5], [10.0,10.0]]},
Element2 = 17.0,
Element3 = ["hola"],
Geom = lgeo_geos_index:to_geom(Element1),
lgeo_geos_index:strtree_insert(GeosSTRtree, Geom, Element1),
lgeo_geos_index:strtree_insert(GeosSTRtree, Geom, Element2),
lgeo_geos_index:strtree_insert(GeosSTRtree, Geom, Element3),
lgeo_geos_index:strtree_query(GeosSTRtree, Geom).
[Element1, Element2, Element3]
*/
static ERL_NIF_TERM
strtree_query(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    GeosSTRtree_t **tree;
    GEOSGeometry **geom;
    ERL_NIF_TERM eterm;

    if (argc != 2) {
        return enif_make_badarg(env);
    }

    if(!enif_get_resource(env, argv[0], GEOSSTRTREE_RESOURCE, (void**)&tree)) {
        return enif_make_badarg(env);
    }

    if(!enif_get_resource(env, argv[1], GEOSGEOM_RESOURCE, (void**)&geom)) {
        return enif_make_badarg(env);
    }

    int size = 128;
    ERL_NIF_TERM *arr = (ERL_NIF_TERM *) enif_alloc(sizeof(ERL_NIF_TERM)*size);
    GeosSTRtree_acc_t acc = {.count=0, .size=size, .elements=arr};
    GEOSSTRtree_query((**tree).tree, *geom, geosstrtree_cb, &acc);

    eterm = enif_make_list_from_array(env, acc.elements, acc.count);
    enif_free(arr);
    return eterm;
}



/* extern void GEOS_DLL GEOSSTRtree_iterate(GEOSSTRtree *tree,
                                       GEOSQueryCallback callback,
                                       void *userdata);

*/
static ERL_NIF_TERM
strtree_iterate(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    GeosSTRtree_t **tree;
    ERL_NIF_TERM eterm;

    if (argc != 1) {
        return enif_make_badarg(env);
    }

    if(!enif_get_resource(env, argv[0], GEOSSTRTREE_RESOURCE, (void**)&tree)) {
        return enif_make_badarg(env);
    }

    int size = 128;

    ERL_NIF_TERM *arr = (ERL_NIF_TERM *) enif_alloc(sizeof(ERL_NIF_TERM)*size);
    GeosSTRtree_acc_t acc = {.count=0, .size=size, .elements=arr};
    GEOSSTRtree_iterate((**tree).tree, geosstrtree_cb, &acc);

    eterm = enif_make_list_from_array(env, acc.elements, acc.count);
    enif_free(arr);
    return eterm;
}

/*
//Removed until know how to compare two terms from diferent environments if possible
//insert saves a copy and remove needs that copy to work

extern char GEOS_DLL GEOSSTRtree_remove(GEOSSTRtree *tree,
                                        const GEOSGeometry *g,
                                        void *item);

//GeosSTRtree = lgeo_geos_index:strtree_create(),
//Ls1 = {'LineString', [[3.0,3.0],[6.0,6.0]]},
//Geom1 = lgeo_geos_index:to_geom(Ls1),
//lgeo_geos_index:strtree_insert(GeosSTRtree, Geom1, Ls1),
//lgeo_geos_index:strtree_remove(GeosSTRtree, Geom1, Ls1),
//Geoms = lgeo_geos_index:strtree_query(GeosSTRtree, Geom1).
//[]

static ERL_NIF_TERM
strtree_remove(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    GeosSTRtree_t **tree;
    GEOSGeometry **geom;

    if (argc != 3) {
        return enif_make_badarg(env);
    }

    if(!enif_get_resource(env, argv[0], GEOSSTRTREE_RESOURCE, (void**)&tree)) {
        return enif_make_badarg(env);
    }

    if(!enif_get_resource(env, argv[1], GEOSGEOM_RESOURCE, (void**)&geom)) {
        return enif_make_badarg(env);
    }

    char remove = \
        GEOSSTRtree_remove((**tree).tree, GEOSEnvelope(*geom), (void*)argv[2]);
        //printf("Rtree remove: %d.\n", remove);

    if (remove == 0) {
        return enif_make_tuple2(env,
            enif_make_atom(env, "ok"),
            enif_make_atom(env, "false"));
    } else if (remove == 1) {
        return enif_make_tuple2(env,
            enif_make_atom(env, "ok"),
            enif_make_atom(env, "true"));
    } else {
        return enif_make_tuple2(env,
            enif_make_atom(env, "error"),
            enif_make_atom(env, "undefined"));
    }
}
*/

/************************************************************************
 *
 *  Erlang-GEOS Translation
 *
 ***********************************************************************/

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
    {"strtree_create", 0, strtree_create},
    {"strtree_insert", 3, strtree_insert},
    {"strtree_iterate", 1, strtree_iterate},
    {"strtree_query", 2, strtree_query}
    //{"strtree_remove", 3, strtree_remove}
};

ERL_NIF_INIT(lgeo_geos_index, nif_funcs, &load, NULL, NULL, unload);
