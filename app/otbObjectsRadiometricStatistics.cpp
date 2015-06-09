#include "otbObjectsRadiometricStatistics.h"

void otb::Wrapper::ObjectsRadiometricStatistics::DoInit()
{
	SetName("ObjectsRadiometricStatistics");
	SetDescription("Compute features attributes of a vector dataset over an image.");

	SetDocName("ObjectsRadiometricStatistics");
	SetDocLongDescription("This application computes radiometric and shapes attributes on a vector dataset, using an image. The results are stored in the attribute table. Shape attributes are : number of pixels, flatness, roundness, elongation, perimeter. Radiometric attributes are for each band of the input image : mean, standard-deviation, median, variance, kurtosis, skewness.");
	SetDocLimitations("None");
	SetDocAuthors("Arnaud Durand");
	SetDocSeeAlso("For now, support of input dataset with multiple layers having different projection reference system is limited.");

	AddParameter(ParameterType_InputFilename,  "in",   "Input vector dataset");
	SetParameterDescription( "in", "Input vector dataset providing segmentation.");

	AddParameter(ParameterType_InputImage,  "im",   "Input reference image");
	SetParameterDescription( "im", "Input image used to compute radiometric attributes.");

	AddParameter(ParameterType_String,"field","ID field");
	SetParameterDescription("field","Name of the field containing object IDs.");
	SetParameterString("field","label");

	AddParameter(ParameterType_Int,"background", "Background value");
	SetParameterDescription("background","Background value. Needs to be different of any object ID.");
	SetDefaultParameterInt("background",0);

	AddParameter(ParameterType_InputFilename, "out", "Output vector dataset");
	SetParameterDescription("out", "Output vector dataset containing features attributes.");
}

void otb::Wrapper::ObjectsRadiometricStatistics::DoUpdateParameters()
{
}

void otb::Wrapper::ObjectsRadiometricStatistics::DoExecute()
{
	VectorImageType::Pointer referenceImage = GetParameterImage("im");

	otb::ogr::DataSource::Pointer ogrDS;
	ogrDS = otb::ogr::DataSource::New(GetParameterString("in"), otb::ogr::DataSource::Modes::Update_LayerUpdate);

	m_OGRDataSourceRendering = OGRDataSourceToMapFilterType::New();
	m_OGRDataSourceRendering->AddOGRDataSource(ogrDS);

	m_OGRDataSourceRendering->SetOutputSize(referenceImage->GetLargestPossibleRegion().GetSize());
	m_OGRDataSourceRendering->SetOutputOrigin(referenceImage->GetOrigin());

	m_OGRDataSourceRendering->SetOutputSpacing(referenceImage->GetSpacing());

	m_OGRDataSourceRendering->SetOutputProjectionRef(referenceImage->GetProjectionRef());

	//	m_OGRDataSourceRendering->SetOutputParametersFromImage(referenceImage); // A tester

	m_OGRDataSourceRendering->SetBackgroundValue(GetParameterInt("background"));
	m_OGRDataSourceRendering->SetBurnAttributeMode(true);
	m_OGRDataSourceRendering->SetBurnAttribute(GetParameterString("field"));

	// Write label image from OGR
	/*
	WriterType::Pointer writer = WriterType::New(); 
	writer->SetInput(m_OGRDataSourceRendering->GetOutput());	
	writer->SetFileName("label_image.tif");
	writer->Update();
	*/

	// Label image from OGR to statistics label map
	ConverterStatisticsType::Pointer converterStats = ConverterStatisticsType::New(); 
	converterStats->SetInput(m_OGRDataSourceRendering->GetOutput()); 
	converterStats->SetBackgroundValue(GetParameterInt("background")); 
	converterStats->Update();

	// Prepare channel extraction

	ExtractROIFilterType::Pointer m_ExtractROIFilter = ExtractROIFilterType::New();
	m_ExtractROIFilter->SetInput(referenceImage);

	// Update dataset with new fields

	otb::ogr::Layer layer = ogrDS->GetLayerChecked(0);

	OGRFieldDefn fieldNbPixels("NbPixels", OFTInteger);
	layer.CreateField(fieldNbPixels, true);

	OGRFieldDefn fieldFlatness("Flat", OFTReal);
	layer.CreateField(fieldFlatness, true);

	OGRFieldDefn fieldRoundness("Round", OFTReal);
	layer.CreateField(fieldRoundness, true);

	OGRFieldDefn fieldElongation("Elong", OFTReal);
	layer.CreateField(fieldElongation, true);

	OGRFieldDefn fieldPerimeter("Perim", OFTReal);
	layer.CreateField(fieldPerimeter, true);

	for(unsigned int i = 0; i < referenceImage->GetNumberOfComponentsPerPixel(); i++)
	{
		std::ostringstream fieldoss;
		fieldoss<<"meanB"<<i+1;
		OGRFieldDefn field(fieldoss.str().c_str(), OFTReal);
		layer.CreateField(field, true);
	}

	for(unsigned int i = 0; i < referenceImage->GetNumberOfComponentsPerPixel(); i++)
	{
		std::ostringstream fieldoss;
		fieldoss<<"stdB"<<i+1;
		OGRFieldDefn field(fieldoss.str().c_str(), OFTReal);
		layer.CreateField(field, true);
	}

	for(unsigned int i = 0; i < referenceImage->GetNumberOfComponentsPerPixel(); i++)
	{
		std::ostringstream fieldoss;
		fieldoss<<"MedB"<<i+1;
		OGRFieldDefn field(fieldoss.str().c_str(), OFTReal);
		layer.CreateField(field, true);
	}

	for(unsigned int i = 0; i < referenceImage->GetNumberOfComponentsPerPixel(); i++)
	{
		std::ostringstream fieldoss;
		fieldoss<<"VarB"<<i+1;
		OGRFieldDefn field(fieldoss.str().c_str(), OFTReal);
		layer.CreateField(field, true);
	}

	for(unsigned int i = 0; i < referenceImage->GetNumberOfComponentsPerPixel(); i++)
	{
		std::ostringstream fieldoss;
		fieldoss<<"KurtB"<<i+1;
		OGRFieldDefn field(fieldoss.str().c_str(), OFTReal);
		layer.CreateField(field, true);
	}

	for(unsigned int i = 0; i < referenceImage->GetNumberOfComponentsPerPixel(); i++)
	{
		std::ostringstream fieldoss;
		fieldoss<<"SkewB"<<i+1;
		OGRFieldDefn field(fieldoss.str().c_str(), OFTReal);
		layer.CreateField(field, true);
	}

	for(unsigned int i = 0; i < referenceImage->GetNumberOfComponentsPerPixel(); i++)
	{
		// Channel selection

		m_ExtractROIFilter->SetChannel(i + 1);

		// Statistics computation
		StatisticsFilterType::Pointer statistics = StatisticsFilterType::New(); 
		statistics->SetInput(converterStats->GetOutput());
		statistics->SetFeatureImage(m_ExtractROIFilter->GetOutput());
		statistics->SetComputePerimeter(true);
		statistics->Update();

		// Write shape attributes only one time
		if(i==0)
		{
			for(otb::ogr::Layer::iterator featIt = layer.begin(); featIt!=layer.end(); ++featIt)
			{
				otb::ogr::Feature m_Feature = *featIt;

				unsigned int label = m_Feature.ogr().GetFieldAsInteger(layer.GetLayerDefn().GetFieldIndex(GetParameterString("field").c_str()));

				if(statistics->GetOutput()->HasLabel(label))
				{
					const StatisticsLabelObjectType *labelObjectStats = statistics->GetOutput()->GetLabelObject(label);

					m_Feature.ogr().SetField("NbPixels", (int)labelObjectStats->GetNumberOfPixels());
					m_Feature.ogr().SetField("Flat", labelObjectStats->GetFlatness());
					m_Feature.ogr().SetField("Round", labelObjectStats->GetRoundness());
					m_Feature.ogr().SetField("Elong", labelObjectStats->GetElongation());
					m_Feature.ogr().SetField("Perim", labelObjectStats->GetPerimeter());

					layer.SetFeature(m_Feature);
				}
			}	
		}

		// Features iteration
		for(otb::ogr::Layer::iterator featIt = layer.begin(); featIt!=layer.end(); ++featIt)
		{
			otb::ogr::Feature m_Feature = *featIt;

			unsigned int label = m_Feature.ogr().GetFieldAsInteger(layer.GetLayerDefn().GetFieldIndex(GetParameterString("field").c_str()));

			if(statistics->GetOutput()->HasLabel(label))
			{
				const StatisticsLabelObjectType *labelObjectStats = statistics->GetOutput()->GetLabelObject(label);
				std::ostringstream fieldoss;
			
				fieldoss<<"meanB"<<i+1;
				m_Feature.ogr().SetField(fieldoss.str().c_str(),labelObjectStats->GetMean());
				fieldoss.str("");	

				fieldoss<<"stdB"<<i+1;
				m_Feature.ogr().SetField(fieldoss.str().c_str(),labelObjectStats->GetStandardDeviation());
				fieldoss.str("");

				fieldoss<<"medB"<<i+1;
				m_Feature.ogr().SetField(fieldoss.str().c_str(),labelObjectStats->GetMedian());
				fieldoss.str("");	

				fieldoss<<"varB"<<i+1;
				m_Feature.ogr().SetField(fieldoss.str().c_str(),labelObjectStats->GetVariance());
				fieldoss.str("");	
		
				fieldoss<<"kurtB"<<i+1;
				m_Feature.ogr().SetField(fieldoss.str().c_str(),labelObjectStats->GetKurtosis());
				fieldoss.str("");	

				fieldoss<<"skewB"<<i+1;
				m_Feature.ogr().SetField(fieldoss.str().c_str(),labelObjectStats->GetSkewness());
				fieldoss.str("");	
			}
			else
			{
				otbAppLogINFO( << "Object number " << label << " is skipped. This could happen during the rasterisation process when an object is smaller than 1 pixel.");
			}
		}
	}
}
