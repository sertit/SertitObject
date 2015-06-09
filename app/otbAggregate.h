#include "otbWrapperApplication.h"
#include "otbWrapperApplicationFactory.h"
#include "otbStreamingStatisticsImageFilter.h"
#include "itkChangeLabelImageFilter.h"
#include "itkLabelMap.h" 
#include "itkLabelImageToLabelMapFilter.h" 
#include "itkStatisticsLabelMapFilter.h"
#include "itkStatisticsLabelObject.h"
#include "itkHistogram.h"
#include "otbLabelImageToOGRDataSourceFilter.h"
#include "otbOGRDataSourceWrapper.h"
#include "otbOGRFeatureWrapper.h"

namespace otb
{
	namespace Wrapper
	{
		class Aggregate : public Application
		{
			public:
				typedef Aggregate 				Self;
				typedef Application 			Superclass;
				typedef itk::SmartPointer<Self> Pointer;
				typedef itk::SmartPointer<const Self> ConstPointer;

				typedef unsigned int   						 PixelType;
				typedef unsigned int						 LabelType;	
				typedef otb::Image<PixelType, 2>             ImageType;
				typedef otb::Image<PixelType, 2> 			 LabelImageType;

				typedef unsigned int LabelImagePixelType;

				typedef otb::StreamingStatisticsImageFilter<LabelImageType> StatisticsImageFilterType;

				typedef itk::ChangeLabelImageFilter<LabelImageType,LabelImageType> ChangeLabelImageFilterType;

				typedef itk::StatisticsLabelObject<LabelType, 2> 									StatisticsLabelObjectType;
				typedef itk::LabelMap<StatisticsLabelObjectType> 									StatisticsLabelMapType;
				typedef itk::StatisticsLabelMapFilter<StatisticsLabelMapType, ImageType >			StatisticsFilterType;
				typedef itk::LabelImageToLabelMapFilter <LabelImageType, StatisticsLabelMapType>	ConverterStatisticsType; 

				typedef otb::LabelImageToOGRDataSourceFilter<LabelImageType> LabelImageToOGRDataSourceFilterType;

				typedef itk::Statistics::Histogram<double> HistogramType;


				itkNewMacro(Self);
				itkTypeMacro(Aggregate, otb::Application);

			private:

				void DoInit();
				void DoUpdateParameters();
				void DoExecute();

				ChangeLabelImageFilterType::Pointer m_ChangeLabelFilter;

		};
	}
}

OTB_APPLICATION_EXPORT(otb::Wrapper::Aggregate)
