;;;; Basic implementation of Geometry.
;;;;
;;;; The spatial analysis methods will return the most specific class possible
;;;; to represent the result. If the result is homogeneous, a Point,
;;;; LineString, or Polygon will be returned if the result contains a single
;;;; element; otherwise, a MultiPoint, MultiLineString, or MultiPolygon will
;;;; be returned. If the result is heterogeneous a GeometryCollection will be
;;;; returned.
;;;; The results computed by the set-theoretic methods may contain
;;;; constructed points which are not present in the input Geometry. These
;;;; new points arise from intersections between line segments in the edges
;;;; of the input Geometry. In the general case it is not possible to
;;;; represent constructed points exactly. This is due to the fact that the
;;;; coordinates of an intersection point may contain twice as many bits of
;;;; precision as the coordinates of the input line segments. In order to
;;;; represent these constructed points explicitly, JTS must truncate them
;;;; to fit the PrecisionModel.
;;;;
;;;; Unfortunately, truncating coordinates moves them slightly. Line
;;;; segments which would not be coincident in the exact result may become
;;;; coincident in the truncated representation. This in turn leads to
;;;; "topology collapses" – situations where a computed element has a lower
;;;; dimension than it would in the exact result.
(defmodule geos.geom.geometry
  (export all))

(defun disjoint (geom-1 geom-2)
  "Tests whether two geometries are disjoint from each other.

  The disjoint predicate has the following equivalent definitions:

  * The two geometries have no point in common
  * The DE-9IM Intersection Matrix for the two geometries matches [FF*FF****]
  * ! g.intersects(this) (disjoint is the inverse of intersects)

  Returns a boolean."
  (lgeo_geos_geom:disjoint geom-1 geom-2))

(defun get-centroid (geom)
  "Computes the centroid of this Geometry.

  The centroid is equal to the centroid of the set of component Geometries of
  highest dimension (since the lower-dimension geometries contribute zero
  'weight' to the centroid).

  Returns a Point."
  (lgeo_geos_geom:get_gentroid geom))

(defun get-intersection (geom-1 geom-2)
  "Returns a Geometry representing the points shared by the two provided
  Geometry objects."
  (lgeo_geos_geom:intersection geom-1 geom-2))

(defun intersect? (geom-1 geom-2)
  ""
  (lgeo_geos_geom:intersects geom-1 geom-2))

(defun valid? (geom)
  "Tests the validity of the given Geometry.

  Returns boolean."
  (lgeo_geos_geom:is_valid geom))

(defun term->geom (term)
  "Convert a GeoCouch Erlang geometry representation to the GEOS Erlang geometry
  representation."
  (lgeo_geos_geom:to_geom term))

(defun geom->term (term)
  "Convert a GEOS Erlang geometry representation to the GeoCouch Erlang geometry
  representation."
  (lgeo_geos_geom:from_geom term))

(defun topology-preserve-simplify (geom tolerance)
  ""
  (lgeo_geos_geom:topology_preserve_simplify geom tolerance))
