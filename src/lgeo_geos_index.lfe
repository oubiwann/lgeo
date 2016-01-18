;;;; Copyright (C) 2011 Couchbase, Inc.
;;;; Copyright (C) 2016 Duncan McGreggor.
;;;;
;;;; Licensed under the Apache License, Version 2.0 (the "License"); you may
;;;; not use this file except in compliance with the License. You may obtain a
;;;; copy of the License at
;;;;
;;;;   http://www.apache.org/licenses/LICENSE-2.0
;;;;
;;;; Unless required by applicable law or agreed to in writing, software
;;;; distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
;;;; WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
;;;; License for the specific language governing permissions and limitations
;;;; nuder the License.

(defmodule lgeo_geos_index
  (on_load #(init 0))
  (export all))

;;; Loader

(defun init ()
  (let ((so-lib (lgeo-util:get-so-file (MODULE))))
    (catch (erlang:load_nif so-lib 0))))

;;; NIF functions

;; XXX setup alias in separate module
(defun strtree_create ()
  (erlang:nif_error 'nif_not_loaded))

;; XXX setup alias in separate module
(defun strtree_insert (_RTree _Geom _Eterm)
  (erlang:nif_error 'nif_not_loaded))

;; XXX setup alias in separate module
(defun strtree_iterate (_RTree)
  (erlang:nif_error 'nif_not_loaded))

;; XXX setup alias in separate module
(defun strtree_query (_Rtree _Geom)
  (erlang:nif_error 'nif_not_loaded))

;; XXX setup alias in separate module
;; (defun strtree_remove (_RTree _Geom _Eterm)
;;   (erlang:nif_error 'nif_not_loaded))
