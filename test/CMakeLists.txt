otb_module_test()

otb_test_application(NAME apSertitObjectAggregate
  APP Aggregate
  OPTIONS -in ${INPUTDATA}/QB_Toulouse_Ortho_XS_6class_knn.tif
  -inseg ${INPUTDATA}/QB_Toulouse_ortho_labelImage.tif
  -out ${TEMP}/apSertitObjectAggregate.shp
  -outim ${TEMP}/apSertitObjectAggregateRaster.tif int16
  )

otb_test_application(NAME apSertitObjectRadiometricStatistics
  APP ObjectsRadiometricStatistics
  OPTIONS -in ${TEMP}/apSertitObjectAggregate.shp
  -im ${INPUTDATA}/QB_Toulouse_Ortho_XS.tif
  -field "label"
  -background 0
  )

set_tests_properties ( apSertitObjectRadiometricStatistics
  PROPERTIES
  DEPENDS apSertitObjectAggregate)
