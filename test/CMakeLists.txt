otb_module_test()

	otb_test_application(NAME apSertitObjectAggregate
			APP Aggregate
			OPTIONS -in  ${INPUTDATA}/QB_Toulouse_Ortho_XS.tif
			-inseg ${INPUTDATA}/TO_BE_FILLED.tif
			-out ${TEMP}/apSertitObjectAggregate.shp
			-outim ${TEMP}/apSertitObjectAggregateRaster.tif int16
			VALID  --compare-ogr ${NOTOL}
			${BASELINE}/apSertitObjectAggregateRef.shp
			${TEMP}/apSertitObjectAggregate.shp)

	otb_test_application(NAME apSertitObjectRadiometricStatistics
			APP ObjectsRadiometricStatistics
			OPTIONS -in ${INPUTDATA}/TOBEINVESTIGATED.shp
			-im ${INPUTDATA}/QB_Toulouse_Ortho_XS.tif
			-field "FID" TOBEINVESTIGATED
			-background 0
			-out ${TEMP}/apSertitObjectRadiometricStatistics.shp
			VALID --compare-ogr ${NOTOL}
			${BASELINE}/apSertitObjectRadiometricStatisticsRef.shp
			${TEMP}/apSertitObjectRadiometricStatistics.shp
			)

