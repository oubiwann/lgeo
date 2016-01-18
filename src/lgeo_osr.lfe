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
(defmodule lgeo_osr
  (on_load #(init 0))
  (export all))

;;; Loader

(defun init ()
  (let ((so-lib (lgeo-util:get-so-file (MODULE))))
    (catch (erlang:load_nif so-lib 0))))

;;; NIF functions

(defun import_from_epsg (_EPSG)
  (erlang:nif_error 'nif_not_loaded))

(defun export_to_wkt (_EPSG)
  (erlang:nif_error 'nif_not_loaded))



