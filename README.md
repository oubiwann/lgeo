# lgeo


[![][lgeo-logo]][lgeo-logo-large]

[lgeo-logo]: resources/images/lgeo-250x.png
[lgeo-logo-large]: resources/images/lgeo-1000x.png

*LFE/Erlang GIS Bindings for GDAL, OGR, & GEOS*


#### Table of Contents

* [Introduction](#introduction-)
* [Documentation](#documentation-)
* [Example Usage](#example-usage-)
* [License](#license-)


**NOTICE**: Work in progress; partial OGR, OSR, and GEOS support; no GDAL support


## Introduction [&#x219F;](#table-of-contents)

This project is an experiment for solving GIS problems on the Erlang VM, particularly, doing so in LFE.

It is based on the work originally done by [@caroman](https://github.com/caroman) in his [erlogr](https://github.com/caroman/erlogr) project. For more details on:

* the various projects that have been woven together in lgeo,
* the organization of the modules and namespaces,
* the C/C++ libraries used to create the LFE/Erlang NIFs,
* dependency, installation, and setup information
* detailed usage examples (user guide), and
* API documentation,

be sure to read the documentation (links below).


## Documentation [&#x219F;](#table-of-contents)

* [API docs](http://oubiwann.github.com/lgeo/current/api)
* [User Guide](http://oubiwann.github.com/lgeo/current/user-guide)


## Example Usage [&#x219F;](#table-of-contents)

The code selections below are just teasers. For more comprehensive examples, visit the [User Guide](http://oubiwann.github.com/lgeo/user-guide).

Here's an example session in the LFE REPL:

```bash
$ make repl

Erlang/OTP 17 [erts-6.2] [source] [64-bit] [smp:4:4] [async-threads:10] ...

LFE Shell V6.2 (abort with ^G)
>
```

Project versions:

```cl
> (lgeo:version)
(#(erlang "18")
 #(emulator "7.1")
 #(driver-version "3.2")
 #(lfe "0.10.1")
 #(lutil "0.7.0")
 #(gdal/ogr "1.11.2")
 #(lgeo "0.2.0-dev"))
```

GDAL metadata:

```cl
> (gdal:version)
"1.11.2"
> (gdal:release-date)
"20150210"
> (gdal:version-str)
"GDAL 1.11.2, released 2015/02/10"
> (gdal:format-build-info)
PAM_ENABLED=YES
OGR_ENABLED=YES
ok
> (gdal:format-license)

GDAL/OGR
--------
GDAL/OGR is released under the MIT/X license.
The LICENSE.TXT distributed with GDAL/OGR should
contain additional details.


lgeo
----
Apache License, Version 2.0

ok
```

OGR operations:

```cl
> (set driver (ogr.driver:get 0))

> (ogr.driver:get-name driver)

```

Old Erlang example (going away soon):

```erlang
    1> Driver = erlogr:get_driver(0),
    1> erlogr:dr_get_name(Driver).
    "ESRI Shapefile"

    2> Driver = erlogr:get_driver_by_name("ESRI Shapefile"),
    2> erlogr:dr_get_name(Driver).
    "ESRI Shapefile"

    3> DataSource = erlogr:open("test/polygon.shp"),
    3> Layer = erlogr:ds_get_layer(DataSource, 0),
    3> Feature = erlogr:l_get_feature(Layer, 0),
    3> Geometry = erlogr:f_get_geometry_ref(Feature),
    3> Wkt = erlogr:g_export_to_wkt(Geometry).
    "POLYGON ((0.351988636363636 -0.969460227272728,2.058238636363636 0.086505681818182,2.690625 -1.524289772727273,0.0 -2.0015625,-0.304261363636364 -1.828551136363636,0.351988636363636 -0.969460227272728))"

    4> Wkb = erlogr:g_export_to_wkb(Geometry).
    <<1,3,0,0,0,1,0,0,0,6,0,0,0,136,181,111,88,251,134,214,63,
      72,23,93,116,209,5,239,191,...>>
```


## License [&#x219F;](#table-of-contents)

```
Copyright © 2011 Couchbase, Inc.
Copyright © 2012 Carlos Roman
Copyright © 2016 Duncan McGreggor

Distributed under the Apache License Version 2.0.
```
