set(SertitObject_LINK_LIBRARIES
${OTBCommon_LIBRARIES}
${0TBApplicationEngine_LIBRARIES}
${OTBStatistics_LIBRARIES}
${OTBConversion_LIBRARIES}
)

OTB_CREATE_APPLICATION(NAME Aggregate
                       SOURCES otbAggregate.cpp
                       LINK_LIBRARIES ${${otb-module}_LIBRARIES}
)

OTB_CREATE_APPLICATION(NAME ObjectsRadiometricStatistics
                       SOURCES otbObjectsRadiometricStatistics.cpp
                       LINK_LIBRARIES ${${otb-module}_LIBRARIES}
)






