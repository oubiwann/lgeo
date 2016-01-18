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

(defmodule lgeo_geos_io
  (on_load #(init 0))
  (export all))

;;; Loader

(defun init ()
  (let ((so-lib (lgeo-util:get-so-file (MODULE))))
    (catch (erlang:load_nif so-lib 0))))

;;; NIF functions

;; XXX setup alias in separate module
(defun wkbreader_create ()
  (erlang:nif_error 'nif_not_loaded))

;; XXX setup alias in separate module
(defun wkbreader_read (_WKBReader _Wkb)
  (erlang:nif_error 'nif_not_loaded))

;; XXX setup alias in separate module
(defun wkbreader_readhex (_WKBReader _WkbHex)
  (erlang:nif_error 'nif_not_loaded))

;; XXX setup alias in separate module
(defun wkbwriter_create()
  (erlang:nif_error 'nif_not_loaded))

;; XXX setup alias in separate module
(defun wkbwriter_write (_WKBWriter _Geom)
  (erlang:nif_error 'nif_not_loaded))

;; XXX setup alias in separate module
(defun wkbwriter_writehex (_WKBWriter _Geom)
  (erlang:nif_error 'nif_not_loaded))

;; XXX setup alias in separate module
(defun wktreader_create ()
  (erlang:nif_error 'nif_not_loaded))

;; XXX setup alias in separate module
(defun wktreader_read (_WKTReader _Wkt)
  (erlang:nif_error 'nif_not_loaded))

;; XXX setup alias in separate module
(defun wktwriter_create ()
  (erlang:nif_error 'nif_not_loaded))

;; XXX setup alias in separate module
(defun wktwriter_write (_WKTWriter _Geom)
  (erlang:nif_error 'nif_not_loaded))
