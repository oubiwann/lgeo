(set geom-1 (geos.geom.geometry:term->geom #(LineString ((3 3) (10 10)))))
(set geom-2 (geos.geom.geometry:term->geom #(LineString ((1 1) (7 7)))))
(geos.geom.geometry:intersect? geom-1 geom-2)
(geos.geom.geometry:get-intersection geom-1 geom-2)
