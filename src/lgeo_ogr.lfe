;;;; Copyright (C) 2012 Carlos Roman. All rights reserved.
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
(defmodule lgeo_ogr
  (on_load #(init 0))
  (export all))

;;; Loader

(defun init ()
  (let ((so-lib (lgeo-util:get-so-file (MODULE))))
    (catch (erlang:load_nif so-lib 0))))

;;; NIF functions

;; XXX setup alias in separate module
(defun ds_get_layer (_Datasource _Index)
  (erlang:nif_error 'nif_not_loaded))

;; XXX setup alias in separate module
(defun ds_get_layer_count (_Datasource)
  (erlang:nif_error 'nif_not_loaded))

;; XXX setup alias in separate module
(defun dr_get_name (_Driver)
  (erlang:nif_error 'nif_not_loaded))

;; XXX setup alias in separate module
(defun g_export_to_wkb (_Geometry)
  (erlang:nif_error 'nif_not_loaded))

;; XXX setup alias in separate module
(defun g_export_to_wkt (_Geometry)
  (erlang:nif_error 'nif_not_loaded))

;; XXX setup alias in separate module
(defun f_get_fields (_Feature)
  (erlang:nif_error 'nif_not_loaded))

;; XXX setup alias in separate module
(defun f_get_geometry (_Feature)
  (erlang:nif_error 'nif_not_loaded))

;; XXX setup alias in separate module
(defun f_get_geometry_ref (_Feature)
  (erlang:nif_error 'nif_not_loaded))

;; XXX setup alias in separate module
(defun fd_get_field_count (_FeatureDefn)
  (erlang:nif_error 'nif_not_loaded))

;; XXX setup alias in separate module
(defun fd_get_field_defn (_FeatureDefn _Index)
  (erlang:nif_error 'nif_not_loaded))

;; XXX setup alias in separate module
(defun fd_get_fields_name (_FeatureDefn)
  (erlang:nif_error 'nif_not_loaded))

;; XXX setup alias in separate module
(defun fd_get_fields_type (_FeatureDefn)
  (erlang:nif_error 'nif_not_loaded))

;; XXX setup alias in separate module
(defun fd_get_geom_type (_FeatureDefn)
  (erlang:nif_error 'nif_not_loaded))

;; XXX setup alias in separate module
(defun fld_get_name_ref (_FieldDefn)
  (erlang:nif_error 'nif_not_loaded))

;; XXX setup alias in separate module
(defun fld_get_type (_FieldDefn)
  (erlang:nif_error 'nif_not_loaded))

;; XXX setup alias in separate module
(defun l_get_feature (_Layer _Index)
  (erlang:nif_error 'nif_not_loaded))

;; XXX setup alias in separate module
(defun l_get_feature_count (_Layer)
  (erlang:nif_error 'nif_not_loaded))

;; XXX setup alias in separate module
(defun l_get_next_feature (_Layer)
  (erlang:nif_error 'nif_not_loaded))

;; XXX setup alias in separate module
(defun l_get_layer_defn (_Layer)
  (erlang:nif_error 'nif_not_loaded))

;; XXX setup alias in separate module
(defun l_reset_reading (_Layer)
  (erlang:nif_error 'nif_not_loaded))

;; XXX setup alias in separate module
(defun get_driver (_DriverIdx)
  (erlang:nif_error 'nif_not_loaded))

;; XXX setup alias in separate module
(defun get_driver_by_name (_DriverName)
  (erlang:nif_error 'nif_not_loaded))

(defun open (_FileName)
  (erlang:nif_error 'nif_not_loaded))

(defun open (_FileName _Update)
  (erlang:nif_error 'nif_not_loaded))

