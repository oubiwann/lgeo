(defmodule ogr.spatial-reference
  (export all))

(defun import-epsg (_Code)
  "Initialize SRS based on EPSG GCS or PCS code."
  (lgeo_osr:import_from_epsg _Code))

(defun export-wkt (_SRS)
  "Convert this SRS into WKT format."
  (lgeo_osr:export_to_wkt _SRS))
