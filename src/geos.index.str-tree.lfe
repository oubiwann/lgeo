;;;; A query-only R-tree created using the Sort-Tile-Recursive (STR) algorithm.
;;;; For two-dimensional spatial data.
;;;;
;;;; The STR packed R-tree is simple to implement and maximizes space
;;;; utilization; that is, as many leaves as possible are filled to capacity.
;;;; Overlap between nodes is far less than in a basic R-tree. However, once
;;;; the tree has been built (explicitly or on the first call to query), items
;;;; may not be added or removed.
;;;;
;;;; Described in: P. Rigaux, Michel Scholl and Agnes Voisard. Spatial
;;;; Databases With Application To GIS. Morgan Kaufmann, San Francisco, 2002.
(defmodule geos.index.str-tree
  (export all))

(defun create ()
  "Constructs an STRtree with the a default maximum number (10) of child
  nodes that a node may have."
  (lgeo_geos_index:strtree_create))

(defun insert (rtree geom term)
  "Adds a spatial item with an extent specified by the given Envelope to
  the index."
  (lgeo_geos_index:strtree_insert rtree geom term))

(defun iterate (rtree)
  "Iterate over all items added thus far."
  (lgeo_geos_index:strtree_iterate rtree))

(defun query (rtree geom)
  "Queries the index for all items whose extents intersect the given search
  Envelope.

  Note that some kinds of indexes may also return objects which do not in fact
  intersect the query envelope."
  (lgeo_geos_index:strtree_query rtree geom))

;; (defun remove (rtree geom term)
;;   (lgeo_geos_index:strtree_remove rtree geom term))
