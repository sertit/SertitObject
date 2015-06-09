set(DOCUMENTATION "Applications dedicated to object-based image classification.")

# define the dependencies of the include module and the tests
otb_module(SertitObject
  DEPENDS
    OTBCommon
	OTBApplicationEngine
	OTBStatistics
	OTBConversion
  TEST_DEPENDS
    OTBTestKernel
    OTBCommandLine
  DESCRIPTION
    "${DOCUMENTATION}"
)
