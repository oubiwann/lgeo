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

static ErlNifResourceType* GEOSWKTREADER_RESOURCE;
static ErlNifResourceType* GEOSWKTWRITER_RESOURCE;
static ErlNifResourceType* GEOSWKBREADER_RESOURCE;
static ErlNifResourceType* GEOSWKBWRITER_RESOURCE;

static void
wktreader_destroy(ErlNifEnv *env, void *obj)
{
    GEOSWKTReader **wkt_reader = (GEOSWKTReader**)obj;
    GEOSWKTReader_destroy(*wkt_reader);
}

static void
wktwriter_destroy(ErlNifEnv *env, void *obj)
{
    GEOSWKTWriter **wkt_writer = (GEOSWKTWriter**)obj;
    GEOSWKTWriter_destroy(*wkt_writer);
}

static void
wkbreader_destroy(ErlNifEnv *env, void *obj)
{
    GEOSWKBReader **wkb_reader = (GEOSWKBReader**)obj;
    GEOSWKBReader_destroy(*wkb_reader);
}

static void
wkbwriter_destroy(ErlNifEnv *env, void *obj)
{
    GEOSWKBWriter **wkb_writer = (GEOSWKBWriter**)obj;
    GEOSWKBWriter_destroy(*wkb_writer);
}

/* From https://github.com/iamaleksey/iconverl/blob/master/c_src/iconverl.c */
static int
load(ErlNifEnv *env, void **priv, ERL_NIF_TERM load_info)
{
    initGEOS(notice_handler, error_handler);

    GEOSWKTREADER_RESOURCE = enif_open_resource_type(
        env, NULL, "geoswktreader_resource", &wktreader_destroy,
        ERL_NIF_RT_CREATE | ERL_NIF_RT_TAKEOVER, NULL);

    GEOSWKTWRITER_RESOURCE = enif_open_resource_type(
        env, NULL, "geoswktwriter_resource", &wktwriter_destroy,
        ERL_NIF_RT_CREATE | ERL_NIF_RT_TAKEOVER, NULL);

    GEOSWKBREADER_RESOURCE = enif_open_resource_type(
        env, NULL, "geoswkbreader_resource", &wkbreader_destroy,
        ERL_NIF_RT_CREATE | ERL_NIF_RT_TAKEOVER, NULL);

    GEOSWKBWRITER_RESOURCE = enif_open_resource_type(
        env, NULL, "geoswkbwriter_resource", &wkbwriter_destroy,
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
 * Reader and Writer APIs
 *
 ***********************************************************************/

/*
WktReader = lgeo_geos_io:wktreader_create().
<<>>
*/
static ERL_NIF_TERM
wktreader_create(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    ERL_NIF_TERM eterm;
    GEOSWKTReader **wkt_reader = \
        enif_alloc_resource(GEOSWKTREADER_RESOURCE, sizeof(GEOSWKTReader*));

    *wkt_reader = GEOSWKTReader_create();
    eterm = enif_make_resource(env, wkt_reader);
    enif_release_resource(wkt_reader);
    return eterm;
}

/*
WktReader = lgeo_geos_io:wktreader_create(),
Geom = lgeo_geos_io:wktreader_read(WktReader, "POINT(10 10)"),
lgeo_geos_io:from_geom(Geom).
{'Point',[10.0,10.0]}
*/
static ERL_NIF_TERM
wktreader_read(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    GEOSWKTReader **wkt_reader;
    ERL_NIF_TERM eterm;

    if (argc != 2) {
        return enif_make_badarg(env);
    }

    if(!enif_get_resource(env, argv[0], GEOSWKTREADER_RESOURCE,
        (void**)&wkt_reader)) {
        return enif_make_badarg(env);
    }

    unsigned len;
    if (!enif_get_list_length(env, argv[1], &len)){
        return enif_make_badarg(env);
    }
    char *wkt = enif_alloc(sizeof(char)*(len+1));

    if(!enif_get_string(env, argv[1], wkt, len+1, ERL_NIF_LATIN1)) {
        return enif_make_badarg(env);
    }

    GEOSGeometry **geom = \
        enif_alloc_resource(GEOSGEOM_RESOURCE, sizeof(GEOSGeometry*));

    *geom = GEOSWKTReader_read(*wkt_reader, wkt);
    eterm = enif_make_resource(env, geom);
    enif_release_resource(geom);
    enif_free(wkt);
    return eterm;
}

/*
WkbReader = lgeo_geos_io:wkbreader_create().
<<>>
*/
static ERL_NIF_TERM
wkbreader_create(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    ERL_NIF_TERM eterm;

    GEOSWKBReader **wkb_reader = \
        enif_alloc_resource(GEOSWKBREADER_RESOURCE, sizeof(GEOSWKBReader*));
    *wkb_reader = GEOSWKBReader_create();


    eterm = enif_make_resource(env, wkb_reader);
    enif_release_resource(wkb_reader);
    return eterm;
}

/*
WktReader = lgeo_geos_io:wktreader_create(),
Geom = lgeo_geos_io:wktreader_read(WktReader, "POINT(10.0 10.0)"),
WkbWriter = lgeo_geos_io:wkbwriter_create(),
Bin = lgeo_geos_io:wkbwriter_write(WkbWriter, Geom),
WkbReader = lgeo_geos_io:wkbreader_create(),
Geom2 = lgeo_geos_io:wkbreader_read(WkbReader, Bin),
lgeo_geos_io:from_geom(Geom2).
{'Point',[10.0,10.0]}
*/
static ERL_NIF_TERM
wkbreader_read(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    GEOSWKBReader **wkb_reader;
    ErlNifBinary bin;
    ERL_NIF_TERM eterm;

    if (argc != 2) {
        return enif_make_badarg(env);
    }

    if(!enif_get_resource(env, argv[0], GEOSWKBREADER_RESOURCE,
        (void**)&wkb_reader)) {
        return enif_make_badarg(env);
    }

    if (!enif_inspect_binary(env, argv[1], &bin)){
            return enif_make_badarg(env);
    }

    GEOSGeometry **geom = \
        enif_alloc_resource(GEOSGEOM_RESOURCE, sizeof(GEOSGeometry*));

    *geom = GEOSWKBReader_read(*wkb_reader, bin.data, bin.size);
    eterm = enif_make_resource(env, geom);
    enif_release_resource(geom);
    return eterm;
}


/*
WkbReader = lgeo_geos_io:wkbreader_create(),
Geom = lgeo_geos_io:wkbreader_readhex(WkbReader,
    "010100000000000000000024400000000000002440"),
lgeo_geos_io:from_geom(Geom).
{'Point',[10.0,10.0]}
*/
static ERL_NIF_TERM
wkbreader_readhex(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    GEOSWKBReader **wkb_reader;
    ERL_NIF_TERM eterm;

    if (argc != 2) {
        return enif_make_badarg(env);
    }

    if(!enif_get_resource(env, argv[0], GEOSWKBREADER_RESOURCE,
        (void**)&wkb_reader)) {
        return enif_make_badarg(env);
    }

    unsigned len;
    if (!enif_get_list_length(env, argv[1], &len)){
        return enif_make_badarg(env);
    }
    char *wkb_hex = enif_alloc(sizeof(char)*(len+1));

    // TODO: Specific message in cases < 0, == 0
    if(enif_get_string(env, argv[1], wkb_hex, len+1, ERL_NIF_LATIN1) <= 0) {
        return enif_make_badarg(env);
    }

    GEOSGeometry **geom = \
        enif_alloc_resource(GEOSGEOM_RESOURCE, sizeof(GEOSGeometry*));

    size_t size = strlen(wkb_hex);
    *geom = GEOSWKBReader_readHEX(*wkb_reader, (unsigned char *)wkb_hex, size);
    eterm = enif_make_resource(env, geom);
    enif_release_resource(geom);
    enif_free(wkb_hex);
    return eterm;
}

/*
WktWriter = lgeo_geos_io:wktwriter_create().
<<>>
*/
static ERL_NIF_TERM
wktwriter_create(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    ERL_NIF_TERM eterm;
    GEOSWKTWriter **wkt_writer = \
        enif_alloc_resource(GEOSWKTWRITER_RESOURCE, sizeof(GEOSWKTWriter*));

    *wkt_writer = GEOSWKTWriter_create();
    eterm = enif_make_resource(env, wkt_writer);
    enif_release_resource(wkt_writer);
    return eterm;
}

/*
WktReader = lgeo_geos_io:wktreader_create(),
Geom = lgeo_geos_io:wktreader_read(WktReader, "POINT(10 10)"),
WktWriter = lgeo_geos_io:wktwriter_create(),
lgeo_geos_io:wktwriter_write(WktWriter, Geom).
"Point(10.0000000000000000 10.0000000000000000)"
*/
static ERL_NIF_TERM
wktwriter_write(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    GEOSWKTWriter **wkt_writer;
    GEOSGeometry **geom;
    ERL_NIF_TERM eterm;

    if (argc != 2) {
        return enif_make_badarg(env);
    }

    if(!enif_get_resource(env, argv[0], GEOSWKTWRITER_RESOURCE,
        (void**)&wkt_writer)) {
        return enif_make_badarg(env);
    }

    if(!enif_get_resource(env, argv[1], GEOSGEOM_RESOURCE, (void**)&geom)) {
        return enif_make_badarg(env);
    }

    char *wkt = GEOSWKTWriter_write(*wkt_writer, *geom);
    eterm = enif_make_string(env, wkt, ERL_NIF_LATIN1);
    GEOSFree(wkt);
    return eterm;
}


/*
WkbWriter = lgeo_geos_io:wkbwriter_create().
<<>>
*/
static ERL_NIF_TERM
wkbwriter_create(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    ERL_NIF_TERM eterm;
    GEOSWKBWriter **wkb_writer = \
        enif_alloc_resource(GEOSWKBWRITER_RESOURCE, sizeof(GEOSWKBWriter*));

    *wkb_writer = GEOSWKBWriter_create();
    eterm = enif_make_resource(env, wkb_writer);
    enif_release_resource(wkb_writer);
    return eterm;
}

/*
WktReader = lgeo_geos_io:wktreader_create(),
Geom = lgeo_geos_io:wktreader_read(WktReader, "POINT(10.0 10.0)"),
WkbWriter = lgeo_geos_io:wkbwriter_create(),
lgeo_geos_io:wkbwriter_write(WkbWriter, Geom).
<<1,1,0,0,0,0,0,0,0,0,0,36,64,0,0,0,0,0,0,36,64>>
*/
static ERL_NIF_TERM
wkbwriter_write(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    GEOSWKBWriter **wkb_writer;
    GEOSGeometry **geom;
    ERL_NIF_TERM eterm;

    if (argc != 2) {
        return enif_make_badarg(env);
    }

    if(!enif_get_resource(env, argv[0], GEOSWKBWRITER_RESOURCE,
        (void**)&wkb_writer)) {
        return enif_make_badarg(env);
    }

    if(!enif_get_resource(env, argv[1], GEOSGEOM_RESOURCE, (void**)&geom)) {
        return enif_make_badarg(env);
    }

    size_t size;
    unsigned char *wkb = GEOSWKBWriter_write(*wkb_writer, *geom, &size);
    wkb[size] = '\0'; /* ensure it is null terminated */

    ErlNifBinary bin = {.size = size, .data = wkb};
    eterm = enif_make_binary(env, &bin);
    enif_release_binary(&bin);
    GEOSFree(wkb);
    return eterm;
}


/*
WktReader = lgeo_geos_io:wktreader_create(),
Geom = lgeo_geos_io:wktreader_read(WktReader, "POINT(10.0 10.0)"),
WkbWriter = lgeo_geos_io:wkbwriter_create(),
lgeo_geos_io:wkbwriter_writehex(WkbWriter, Geom).
"010100000000000000000024400000000000002440"
*/
static ERL_NIF_TERM
wkbwriter_writehex(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    GEOSWKBWriter **wkb_writer;
    GEOSGeometry **geom;
    ERL_NIF_TERM eterm;

    if (argc != 2) {
        return enif_make_badarg(env);
    }

    if(!enif_get_resource(env, argv[0], GEOSWKBWRITER_RESOURCE,
        (void**)&wkb_writer)) {
        return enif_make_badarg(env);
    }

    if(!enif_get_resource(env, argv[1], GEOSGEOM_RESOURCE, (void**)&geom)) {
        return enif_make_badarg(env);
    }

    size_t size;
    unsigned char *wkb_hex = GEOSWKBWriter_writeHEX(*wkb_writer, *geom, &size);
    wkb_hex[size] = '\0'; /* ensure it is null terminated */
    eterm = enif_make_string(env, (char *)wkb_hex, ERL_NIF_LATIN1);
    GEOSFree(wkb_hex);
    return eterm;
}


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
    {"wkbreader_create", 0, wkbreader_create},
    {"wkbreader_read", 2, wkbreader_read},
    {"wkbreader_readhex", 2, wkbreader_readhex},
    {"wkbwriter_create", 0, wkbwriter_create},
    {"wkbwriter_write", 2, wkbwriter_write},
    {"wkbwriter_writehex", 2, wkbwriter_writehex},
    {"wktreader_create", 0, wktreader_create},
    {"wktreader_read", 2, wktreader_read},
    {"wktwriter_create", 0, wktwriter_create},
    {"wktwriter_write", 2, wktwriter_write}
};

ERL_NIF_INIT(lgeo_geos_io, nif_funcs, &load, NULL, NULL, unload);
