#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include <geos_c.h>

#include "erl_nif.h"

/* From comp.lang.c FAQ Question 17.3 */
#define Streq(s1, s2) (strcmp((s1), (s2)) == 0)

#define DIMENSION 2

static ErlNifResourceType* GEOSGEOM_RESOURCE;
static ErlNifResourceType* GEOSSTRTREE_RESOURCE;

/* Currently support for 2 dimensions only */
int
set_GEOSCoordSeq_from_eterm_list(GEOSCoordSequence *seq, int pos,
        ErlNifEnv *env, const ERL_NIF_TERM *coords)
{
    double dbl_coord;
    int int_coord;
    ERL_NIF_TERM head, tail;

    if (enif_get_list_cell(env, *coords, &head, &tail)) {
        if (enif_get_int(env, head, &int_coord)) {
            dbl_coord = int_coord;
        }
        else if (!enif_get_double(env, head, &dbl_coord)) {
            return enif_make_badarg(env);
        }
        GEOSCoordSeq_setX(seq, pos, dbl_coord);

        enif_get_list_cell(env, tail, &head, &tail);
        if (enif_get_int(env, head, &int_coord)) {
            dbl_coord = int_coord;
        }
        else if (!enif_get_double(env, head, &dbl_coord)) {
            return enif_make_badarg(env);
        }
        GEOSCoordSeq_setY(seq, pos, dbl_coord);
        return 1;
    }
    return enif_make_badarg(env);
}

GEOSGeometry*
eterm_to_geom_point(ErlNifEnv *env, const ERL_NIF_TERM *coords_list)
{
    GEOSCoordSequence *coords_seq;

    coords_seq = GEOSCoordSeq_create(1, DIMENSION);
    set_GEOSCoordSeq_from_eterm_list(coords_seq, 0, env, coords_list);
    return GEOSGeom_createPoint(coords_seq);
}

GEOSCoordSeq
eterm_to_geom_linestring_coords(ErlNifEnv *env,
    const ERL_NIF_TERM *coords_list)
{
    unsigned int i=0, len;
    GEOSCoordSequence *coords_seq;
    ERL_NIF_TERM head, tail;

    enif_get_list_length(env, *coords_list, &len);
    coords_seq = GEOSCoordSeq_create(len, DIMENSION);
    while (enif_get_list_cell(env, *coords_list, &head, &tail)) {
        if (!set_GEOSCoordSeq_from_eterm_list(coords_seq, i, env, &head)) {
            return NULL;
        }
        i++;
        coords_list = &tail;
    }
    return coords_seq;
}

GEOSGeometry*
eterm_to_geom_linestring(ErlNifEnv *env, const ERL_NIF_TERM *eterm)
{
    GEOSCoordSequence *coords_seq = eterm_to_geom_linestring_coords(env,
        eterm);
    return GEOSGeom_createLineString(coords_seq);
}

GEOSGeometry*
eterm_to_geom_polygon(ErlNifEnv *env, const ERL_NIF_TERM *eterm)
{
    ERL_NIF_TERM outer_eterm, inner_eterm, tail;
    unsigned int rings_num, i;
    GEOSCoordSequence *outer_seq, *inner_seq;
    GEOSGeometry *outer_geom, *geom;
    GEOSGeometry **geoms;

    enif_get_list_length(env, *eterm, &rings_num);
    enif_get_list_cell(env, *eterm, &outer_eterm, &inner_eterm);
    outer_seq = eterm_to_geom_linestring_coords(env, &outer_eterm);
    outer_geom = GEOSGeom_createLinearRing(outer_seq);

    // if there are holes
    geoms = enif_alloc(sizeof(GEOSGeometry*)*rings_num-1);
    for (i=0; enif_get_list_cell(env, inner_eterm, &inner_eterm, &tail); i++) {
        inner_seq = eterm_to_geom_linestring_coords(env, &inner_eterm);
        geoms[i] = GEOSGeom_createLinearRing(inner_seq);
        inner_eterm = tail;
    }
    geom = GEOSGeom_createPolygon(outer_geom, geoms, rings_num-1);
    enif_free(geoms);
    return geom;
}

GEOSGeometry*
eterm_to_geom_multi(ErlNifEnv *env, ERL_NIF_TERM eterm, int type,
    GEOSGeometry*(*eterm_to_geom)(ErlNifEnv *env, const ERL_NIF_TERM *eterm))
{
    unsigned int i, geoms_num;
    GEOSGeometry *geom;
    GEOSGeometry **geoms;
    ERL_NIF_TERM tail;

    enif_get_list_length(env, eterm, &geoms_num);
    geoms = enif_alloc(sizeof(GEOSGeometry*)*geoms_num);
    for (i=0; enif_get_list_cell(env, eterm , &eterm, &tail); i++) {
        geoms[i] = (*eterm_to_geom)(env, &eterm);
        eterm = tail;
    }
    geom = GEOSGeom_createCollection(type, geoms, geoms_num);
    enif_free(geoms);
    return geom;
}

GEOSGeometry*
eterm_to_geom(ErlNifEnv *env, const ERL_NIF_TERM *eterm)
{
    // coords_num is the number coordinates for Points and LineStrings,
    // in a case of Polygons it's the number of rings (inner and outer),
    // in case of all Multi* types it's the number of geometries
    unsigned int coords_num;
    int tmp_size;
    GEOSGeometry *geom;
    // longest geometry type is "GeometryCollection"
    char type[19];
    const ERL_NIF_TERM *geom_tuple;

    // Split into geometry type and coordinates
    if (!enif_get_tuple(env, *eterm, &tmp_size, &geom_tuple)) {
        return NULL;
    }
    // Get geometry
    if (!enif_get_atom(env, geom_tuple[0], type, 19, ERL_NIF_LATIN1)) {
        return NULL;
    }

    if (!enif_get_list_length(env, geom_tuple[1], &coords_num)) {
        return NULL;
    }

    if(Streq(type, "Point")) {
        // Needs GEOS 3.3.0
        //if (coords_num==0) {
        //    geom = GEOSGeom_createEmptyPoint();
        //}
        //else {
            geom = eterm_to_geom_point(env, &geom_tuple[1]);
        //}
    }
    else if(Streq(type, "LineString")) {
        // Needs GEOS 3.3.0
        //if (coords_num==0) {
        //    geom = GEOSGeom_createEmptyLineString();
        //}
        //else {
            geom = eterm_to_geom_linestring(env, &geom_tuple[1]);
       //}
    }
    // The polygon follows the GeoJSON specification. First element
    // in the list is the polygon, subsequent ones are holes.
    else if(Streq(type, "Polygon")) {
        // Needs GEOS 3.3.0
        //if (coords_num==0) {
        //    geom = GEOSGeom_createEmptyPolygon();
        //}
        //else {
            geom = eterm_to_geom_polygon(env, &geom_tuple[1]);
        //}
    }
    else if(Streq(type, "MultiPoint")) {
        // Needs GEOS 3.3.0
        //if (coords_num==0) {
        //    geom = GEOSGeom_createEmptyCollection(GEOS_MULTIPOINT);
        //}
        //else {
            geom = eterm_to_geom_multi(env, geom_tuple[1], GEOS_MULTIPOINT,
                eterm_to_geom_point);
        //}
    }
    else if(Streq(type, "MultiLineString")) {
        // Needs GEOS 3.3.0
        //if (coords_num==0) {
        //    geom = GEOSGeom_createEmptyCollection(GEOS_MULTILINESTRING);
        //}
        //else {
            geom = eterm_to_geom_multi(env, geom_tuple[1],
                GEOS_MULTILINESTRING, eterm_to_geom_linestring);
        //}
    }
    else if(Streq(type, "MultiPolygon")) {
        // Needs GEOS 3.3.0
        //if (coords_num==0) {
        //    geom = GEOSGeom_createEmptyCollection(GEOS_MULTIPOLYGON);
        //}
        //else {
            geom = eterm_to_geom_multi(env, geom_tuple[1], GEOS_MULTIPOLYGON,
                eterm_to_geom_polygon);
        //}
    }
    else if(Streq(type, "GeometryCollection")) {
        // Needs GEOS 3.3.0
        //if (coords_num==0) {
        //    geom = GEOSGeom_createEmptyCollection(GEOS_GEOMETRYCOLLECTION);
        //}
        //else {
            geom = eterm_to_geom_multi(env, geom_tuple[1],
                GEOS_GEOMETRYCOLLECTION, eterm_to_geom);
        //}
    }
    else {
        return NULL;
    }
    //printf("geom: %s\r\n", GEOSGeomToWKT(geom));
    return geom;
}



/* Currently support for 2 dimensions only */
ERL_NIF_TERM
GEOSCoordSequence_to_eterm_list(ErlNifEnv *env,
    const GEOSCoordSequence *coords_seq, unsigned int len)
{
    int i = 0;
    double coordx, coordy;
    ERL_NIF_TERM *coords_list;
    ERL_NIF_TERM coords, coords_list_eterm;

    coords_list = enif_alloc(sizeof(ERL_NIF_TERM)*len);
    for(i=0; i<len; i++) {
        GEOSCoordSeq_getX(coords_seq, i, &coordx);
        GEOSCoordSeq_getY(coords_seq, i, &coordy);
        coords = enif_make_list2(env, enif_make_double(env, coordx),
            enif_make_double(env, coordy));
        coords_list[i] = coords;
    }
    coords_list_eterm = enif_make_list_from_array(env, coords_list, len);
    enif_free(coords_list);
    return coords_list_eterm;
}

ERL_NIF_TERM
geom_to_eterm_point_coords(ErlNifEnv *env, const GEOSGeometry *geom)
{
    const GEOSCoordSequence *coords_seq;
    double coordx, coordy;

    coords_seq = GEOSGeom_getCoordSeq(geom);
    GEOSCoordSeq_getX(coords_seq, 0, &coordx);
    GEOSCoordSeq_getY(coords_seq, 0, &coordy);
    return enif_make_list2(env, enif_make_double(env, coordx),
        enif_make_double(env, coordy));
}

ERL_NIF_TERM
geom_to_eterm_linestring_coords(ErlNifEnv *env, const GEOSGeometry *geom)
{
    const GEOSCoordSequence *coords_seq;

    coords_seq = GEOSGeom_getCoordSeq(geom);
    return GEOSCoordSequence_to_eterm_list(env, coords_seq,
        GEOSGetNumCoordinates(geom));
}

ERL_NIF_TERM
geom_to_eterm_polygon_coords(ErlNifEnv *env, const GEOSGeometry *geom)
{
    unsigned int inner_num, i;
    const GEOSGeometry *outer, *inner;
    const GEOSCoordSequence *coords_seq;
    ERL_NIF_TERM coords;
    ERL_NIF_TERM *rings;

    inner_num = GEOSGetNumInteriorRings(geom);
    // all rings, outer + inner
    rings = enif_alloc(sizeof(ERL_NIF_TERM)*inner_num+1);

    outer = GEOSGetExteriorRing(geom);
    coords_seq = GEOSGeom_getCoordSeq(outer);
    rings[0] = GEOSCoordSequence_to_eterm_list(env, coords_seq,
        GEOSGetNumCoordinates(outer));

    for (i=0; i<inner_num; i++) {
        inner = GEOSGetInteriorRingN(geom, i);
        coords_seq = GEOSGeom_getCoordSeq(inner);
        rings[i+1] = GEOSCoordSequence_to_eterm_list(env,
            coords_seq, GEOSGetNumCoordinates(inner));
    }
    coords = enif_make_list_from_array(env, rings, inner_num+1);
    enif_free(rings);
    return coords;
}

// Creates the coordinates for a multi-geometry.
static ERL_NIF_TERM
geom_to_eterm_multi_coords(ErlNifEnv *env, const GEOSGeometry *multi_geom,
    ERL_NIF_TERM(*geom_to_eterm_coords)(ErlNifEnv *env, const GEOSGeometry *geom))
{
    int geom_num, i;
    const GEOSGeometry *geom;
    ERL_NIF_TERM coords;
    ERL_NIF_TERM *coords_multi;

    geom_num = GEOSGetNumGeometries(multi_geom);
    coords_multi = enif_alloc(sizeof(ERL_NIF_TERM)*geom_num);
    for (i=0; i<geom_num; i++) {
        geom = GEOSGetGeometryN(multi_geom, i);
        coords_multi[i] = (*geom_to_eterm_coords)(env, geom);
    }
    coords = enif_make_list_from_array(env, coords_multi, geom_num);
    enif_free(coords_multi);
    return coords;
}

static ERL_NIF_TERM
geom_to_eterm(ErlNifEnv *env, const GEOSGeometry *geom)
{
    ERL_NIF_TERM coords;
    int type = GEOSGeomTypeId(geom);

    switch(type) {
    case GEOS_POINT:
        if (GEOSisEmpty(geom)) {
            coords = enif_make_list(env, 0);
        }
        else {
            coords = geom_to_eterm_point_coords(env, geom);
        }
        return enif_make_tuple2(env, enif_make_atom(env, "Point"), coords);
    case GEOS_LINESTRING:
        if (GEOSisEmpty(geom)) {
            coords = enif_make_list(env, 0);
        }
        else {
            coords = geom_to_eterm_linestring_coords(env, geom);
        }
        return enif_make_tuple2(env, enif_make_atom(env, "LineString"),
            coords);
    case GEOS_POLYGON:
        if (GEOSisEmpty(geom)) {
            coords = enif_make_list(env, 0);
        }
        else {
            coords = geom_to_eterm_polygon_coords(env, geom);
        }
        return enif_make_tuple2(env, enif_make_atom(env, "Polygon"), coords);
    case GEOS_MULTIPOINT:
        if (GEOSisEmpty(geom)) {
            coords = enif_make_list(env, 0);
        }
        else {
            coords = geom_to_eterm_multi_coords(env, geom,
                geom_to_eterm_point_coords);
        }
        return enif_make_tuple2(env, enif_make_atom(env, "MultiPoint"),
            coords);
    case GEOS_MULTILINESTRING:
        if (GEOSisEmpty(geom)) {
            coords = enif_make_list(env, 0);
        }
        else {
            coords = geom_to_eterm_multi_coords(env, geom,
                geom_to_eterm_linestring_coords);
        }
        return enif_make_tuple2(env, enif_make_atom(env, "MultiLineString"),
            coords);
    case GEOS_MULTIPOLYGON:
        if (GEOSisEmpty(geom)) {
            coords = enif_make_list(env, 0);
        }
        else {
            coords = geom_to_eterm_multi_coords(env, geom,
                geom_to_eterm_polygon_coords);
        }
        return enif_make_tuple2(env, enif_make_atom(env, "MultiPolygon"),
            coords);
    case GEOS_GEOMETRYCOLLECTION:
        if (GEOSisEmpty(geom)) {
            coords = enif_make_list(env, 0);
        }
        else {
            coords = geom_to_eterm_multi_coords(env, geom, geom_to_eterm);
        }
        return enif_make_tuple2(env, enif_make_atom(env, "GeometryCollection"),
            coords);
    }
    return -1;
}

typedef struct {
    ErlNifEnv *env;
    GEOSSTRtree *tree;
} GeosSTRtree_t;

typedef struct {
    int count;
    int size;
    ERL_NIF_TERM *elements;
} GeosSTRtree_acc_t;

void
geosstrtree_cb(void *item, void *acc) {
    GeosSTRtree_acc_t *acc_ptr  = (GeosSTRtree_acc_t *) acc;
    ++(acc_ptr->count);
    if (acc_ptr->count == acc_ptr->size) {
        acc_ptr->size *=2;
        acc_ptr->elements = enif_realloc(acc_ptr->elements, acc_ptr->size);
    }
    acc_ptr->elements[acc_ptr->count-1] = (ERL_NIF_TERM) item;
}

/* From http://trac.gispython.org/lab/browser/PCL/trunk/PCL-Core/cartography/
    geometry/_geommodule.c */
static void
notice_handler(const char *fmt, ...) {
    va_list ap;
    fprintf(stderr, "NOTICE: ");
    va_start (ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    fprintf(stderr, "\n" );
}

/* From http://trac.gispython.org/lab/browser/PCL/trunk/PCL-Core/cartography/
    geometry/_geommodule.c */
static void
error_handler(const char *fmt, ...)
{
    va_list ap;
    va_start (ap, fmt);
    va_end(ap);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n" );
}

static void
geom_destroy(ErlNifEnv *env, void *obj)
{
    GEOSGeometry **geom = (GEOSGeometry**)obj;
    GEOSGeom_destroy(*geom);
}

static void
strtree_destroy(ErlNifEnv *env, void *obj)
{
    GeosSTRtree_t **tree = (GeosSTRtree_t**)obj;
    GEOSSTRtree_destroy((**tree).tree);
    enif_free_env((**tree).env);
    enif_free(*tree);
}
