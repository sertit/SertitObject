#include "otbWrapperApplication.h"
#include "otbWrapperApplicationFactory.h"

#include "otbOGRDataSourceWrapper.h"
#include "otbOGRDataSourceToLabelImageFilter.h"
#include "otbMultiToMonoChannelExtractROI.h"

#include "itkLabelMap.h" 
#include "itkLabelImageToLabelMapFilter.h" 
#include "itkStatisticsLabelMapFilter.h"
#include "itkStatisticsLabelObject.h"

#include "otbImageFileWriter.h"

#include "otbVectorData.h"

namespace otb
{
	namespace Wrapper
	{

		class ObjectsRadiometricStatistics : public Application
		{
			public:
				// Standard class
				typedef ObjectsRadiometricStatistics  Self;
				typedef Application                   Superclass;
				typedef itk::SmartPointer<Self>       Pointer;
				typedef itk::SmartPointer<const Self> ConstPointer;

				// Standard macro
				itkNewMacro(Self);
				itkTypeMacro(Self, otb::Application);

			private:
				void DoInit();
				void DoUpdateParameters();
				void DoExecute();

				// Base types
				typedef float		   			    PixelType; 
				typedef unsigned int				LabelType;
				typedef otb::Image<PixelType, 2>	ImageType;

				typedef otb::VectorImage<float, 2>	VectorImageType;	
				typedef otb::Image<unsigned int, 2> LabelImageType;

				// OGR
				typedef otb::OGRDataSourceToLabelImageFilter<LabelImageType>	OGRDataSourceToMapFilterType;
				OGRDataSourceToMapFilterType::Pointer 							m_OGRDataSourceRendering;

				// LabelMap
				typedef otb::MultiToMonoChannelExtractROI<PixelType, PixelType> 					ExtractROIFilterType;

				typedef itk::StatisticsLabelObject<LabelType, 2> 									StatisticsLabelObjectType;
				typedef itk::LabelMap<StatisticsLabelObjectType> 									StatisticsLabelMapType;
				typedef itk::StatisticsLabelMapFilter<StatisticsLabelMapType, ImageType >			StatisticsFilterType;
				typedef itk::LabelImageToLabelMapFilter <LabelImageType, StatisticsLabelMapType> 	ConverterStatisticsType;

				typedef otb::ImageFileWriter<LabelImageType> WriterType;
		};
	}
}

OTB_APPLICATION_EXPORT(otb::Wrapper::ObjectsRadiometricStatistics)
