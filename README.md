Summary
=======

This module provides 2 applications dedicated to object-oriented image analysis.

Aggregate
---------

The aim of this application is to merge the result of a segmentation with a pixel-based image classification in order to produce an object-oriented image classification. The input segmentation is a labeled image, typically the result provided by the OTB application LSMSSegmentation. The output is a vector dataset containing objects and the corresponding class in the attribute table. Connected regions belonging to the same class are merged. This application could be used at the last step of the LSMS pipeline in replacement of the application LSMSVectorization.

ObjectsRadiometricStatistics
----------------------------
This application computes radiometric and shape attributes on a vector dataset, using an image. The results are stored in the attribute table. Shape attributes are : number of pixels, flatness, roundness, elongation, perimeter. Radiometric attributes are for each band of the input image : mean, standard-deviation, median, variance, kurtosis, skewness. The result could be use to perform futher object-oriented image analysis.

Licence
=======

This code is provided under the CeCILL-B free software license agreement.

Contact
=======

For any issues regarding this module please contact us:
sertit@sertit.u-strasbg.fr

SERTIT - University of Strasbourg
http://sertit.u-strasbg.fr
