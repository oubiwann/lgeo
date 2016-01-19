# Introduction

*User Guide: LFE/Erlang GIS Bindings for GDAL, OGR, & GEOS*

[![][lgeo-logo]][lgeo-logo-large]

[lgeo-logo]: images/lgeo-250x.png
[lgeo-logo-large]: images/lgeo-1000x.png


## About lgeo

This project is an experiment for solving GIS problems on the Erlang VM, particularly, doing so in LFE.

It is based on the work originally done by [@caroman](https://github.com/caroman) in his [erlogr](https://github.com/caroman/erlogr) project. Unfortunately, that code base was written for Erlang R13 and hasn't been updated for 18.x (neither, it seems, does it work on 15.x). The erlogr project focused entirely upon OGR, the vector part of GDAL. I'm still trying to sort out the Erlang C ports for OGR, but I have a greater interest in the raster side of things (e.g., satellite data), so I'm going to be putting most of my efforts into getting a few bits of GDAL-proper up and runnnig. [@caroman](https://github.com/caroman) also worked on two other Erlang GIS projects -- both of which are being brought into lgeo (more details below in the [Erlang Sources](#erlang-sources-) section).

The only functions from the erlogr project that were written in Erlang were NIF wrappers. In lgeo, these have been rewritten in LFE (essentially one line of code, each). Additionally, though, lgeo is providing NIF wrappers organized along the same lines as the related [C++ libraries](#c-sources-), and to a lesser extent, the [Python bindings](http://gdal.org/python/). LFE/Erlang doesn't have namespaces, though, so this is "faked" though a dotted file/module naming convention, such as ``gdal.dataset`` and functions like ``gdal.dataset:open``.


## Erlang Sources

The Erlang projects whose code was converted to LFE, and in that form provided the basis
for the lgeo project, as as follows:

* [erlogr](https://github.com/caroman/erlogr)
* [erlosr](https://github.com/caroman/erlosr)
* [erlgeom](https://github.com/caroman/erlgeom) (forked from [Couchbase's repo](https://github.com/couchbaselabs/erlgeom))


## C Sources

The C source code from the GDAL and GEOS bits used so far in lgeo are documented here:

* [GDAL API](http://gdal.org/1.11/gdal_8h.html)
* [GDAL Tutorial](http://gdal.org/1.11/gdal_tutorial.html)
* [OGR API](http://gdal.org/1.11/ogr/ogr__api_8h.html)
* [OGR Tutorial](http://gdal.org/1.11/ogr/ogr_apitut.html)
* [OSR API](http://gdal.org/1.11/ogr/classOGRSpatialReference.html)
* [OSR Tutorial](http://gdal.org/1.11/ogr/osr_tutorial.html)
* GEOS API
  * [geom](http://geos.osgeo.org/doxygen/namespacegeos_1_1geom.html)
  * [index](http://geos.osgeo.org/doxygen/namespacegeos_1_1index.html)
  * [io](http://geos.osgeo.org/doxygen/namespacegeos_1_1io.html)


## Conventions

We use styled call-outs to provide immediate visual cues about the nature of
the information being shared.

These are as follows:

<aside class="info">
This style indicates useful information, background, or other insights.
</aside>

<aside class="success">
This style indicates a best practice, good usage, and other tips for success.
</aside>

<aside class="caution">
This style indicates one should use caution or that behaviour may not be as
expected
</aside>

<aside class="danger">
This style indicates information that could cause errors if ignored.
</aside>


## API Organization

There are four main "namespaces" for the lgeo project:

* ``lgeo``/``lgeo.*`` - top-level modules and function that relate specifically to
  the lgeo project
* ``gdal``/``gdal.*`` - modules for functions from GDAL (specifically, ``gdal.h``)
* ``ogr``/``ogr.*`` - modules for functions from OGR (specifically, ``ogr_api.h``
  and ``ogr_srs_api.h``)
* ``geos``/``geos.*`` - modules for functions from GEOS (specifically, ``geos_c.h``)

Note that it may become necessary in the future to include additional pseudo-namespaces (e.g., for
the [PROJ4 library](https://trac.osgeo.org/proj/)).

The LFE NIF wrappers of the form ``lgeo_*`` are not intended to be used directly (though one certianly may do so). It is preferable to use the dotted modules.


