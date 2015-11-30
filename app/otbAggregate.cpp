#include "otbAggregate.h"

void otb::Wrapper::Aggregate::DoInit()
{
	SetName("Aggregate");
	SetDescription("This application assign a class on each object of a segmentation by majority voting on a pixel-based classification.");

	SetDocName("Aggregate");
	SetDocLongDescription("The aim of this application is to merge the result of a segmentation with a pixel-based image classification in order to produce an object-oriented image classification. The input segmentation is a labeled image, typically the result provided by the OTB application LSMSSegmentation. The output is a vector dataset containing objects and the corresponding class in the attribute table. Connected regions belonging to the same class are merged. This application could be used at the last step of the LSMS pipeline in replacement of the application LSMSVectorization.");
	SetDocLimitations("Input classification and labeled image should have identical resolution and footprint");
	SetDocAuthors("Lucie Bouillot");
	SetDocSeeAlso("MeanShiftSmoothing, LSMSSegmentation, LSMSSmallRegionsMerging, TrainImagesClassifier, ImagesClassification");
	AddDocTag(Tags::Segmentation);
	AddDocTag(Tags::Learning);

	AddParameter(ParameterType_InputImage, "in", "Input classification");
	SetParameterDescription( "in", "Pixel-based image classification." );

	AddParameter(ParameterType_InputImage,  "inseg",    "Labeled image");
	SetParameterDescription( "inseg", "Labeled image representing a segmentation of an image.");

	AddParameter(ParameterType_OutputImage, "outim", "Output raster");
	SetParameterDescription( "outim", "Output raster, object-oriented image classification.");

	AddParameter(ParameterType_OutputFilename, "out", "Output vector dataset");
	SetParameterDescription( "out", "Output vector dataset, object-oriented image classification. The class 0 is considered as background.");

	SetDocExampleParameterValue("in","classification.tif");
	SetDocExampleParameterValue("inseg","labeled_image.tif");
	SetDocExampleParameterValue("outim","image_classification.tif");
	SetDocExampleParameterValue("out","vector_classification.shp");
}

void otb::Wrapper::Aggregate::DoUpdateParameters()
{
}

void otb::Wrapper::Aggregate::DoExecute()
{
	// Récupération de la labelmap
	LabelImageType::Pointer labelIn = GetParameterUInt32Image("inseg");
	labelIn->SetRequestedRegionToLargestPossibleRegion();
	labelIn->Update();

	// Filtre statistique pour récupérer le nombre de label dans la labelmap
	StatisticsImageFilterType::Pointer stats = StatisticsImageFilterType::New();
	stats->SetInput(labelIn);
	stats->Update();
	unsigned int regionCount=stats->GetMaximum();

	otbAppLogINFO(<<"Number of objects: "<<regionCount);

	//Récupération de la classification et statistique pour connaître le nombre de classes
	ImageType::Pointer imageIn = GetParameterUInt32Image("in");
	stats->SetInput(imageIn);
	stats->Update();
	unsigned int nbclass=stats->GetMaximum()-stats->GetMinimum()+1;
	otbAppLogINFO(<<"Number of classes: "<<nbclass);
	unsigned int minimum =stats->GetMinimum();
	otbAppLogINFO(<<"Minimum: "<<minimum);

	// Filtre LabelImage vers LabelMap(StatisticsLabelObject)
	ConverterStatisticsType::Pointer converterStats = ConverterStatisticsType::New();
	converterStats->SetInput(labelIn);
	converterStats->SetBackgroundValue(0);
	converterStats->Update();

	// Calcul des statistiques par objet de la LabelMap
	StatisticsFilterType::Pointer statistics = StatisticsFilterType::New();
	statistics->SetInput(converterStats->GetOutput());
	statistics->SetFeatureImage(imageIn);
	statistics->SetNumberOfBins(nbclass);
	statistics->Update();

	// Définition du filtre ChangeLabel
	m_ChangeLabelFilter = ChangeLabelImageFilterType::New();
	m_ChangeLabelFilter->SetInput(labelIn);

	// Iteration sur les objets, récupération de l'histogramme, extraction de la valeur mojoritaire
	// Changement de la valeur du label par la valeur majoritaire dans la label map => obtention d'une classification corrigée

	for(unsigned int i=0; i<regionCount+1; i++)
	{
		if(statistics->GetOutput()->HasLabel(i))
		{
			const StatisticsLabelObjectType *labelObjectStats = statistics->GetOutput()->GetLabelObject(i);
			const HistogramType *histogram = labelObjectStats->GetHistogram();

			unsigned int var = 0;
			unsigned int classe = minimum;
			for(unsigned int j=0; j< nbclass; j++)
			{
				if(histogram->GetFrequency(j)>var)
				{
					var = histogram->GetFrequency(j);
					classe = j+minimum;
				}
			}
			m_ChangeLabelFilter->SetChange(i,classe);
		}
	}

	SetParameterOutputImage("outim", m_ChangeLabelFilter->GetOutput());

	//Vectorisation
	otbAppLogINFO(<<"Vectorization...");

	//Définition du shapefile
  const std::string shapefile = GetParameterString("out");

	otb::ogr::DataSource::Pointer ogrDS;
	otb::ogr::Layer layer(NULL, false);

	std::string projRef = imageIn->GetProjectionRef();

	OGRSpatialReference oSRS(projRef.c_str());

	otbAppLogINFO(<< projRef);

	std::vector<std::string> options;

	ogrDS = otb::ogr::DataSource::New(shapefile, otb::ogr::DataSource::Modes::Overwrite);
	std::string layername = itksys::SystemTools::GetFilenameName(shapefile.c_str());
	std::string const extension = itksys::SystemTools::GetFilenameLastExtension(shapefile.c_str());
	layername = layername.substr(0,layername.size()-(extension.size()));
	layer = ogrDS->CreateLayer(layername, &oSRS, wkbMultiPolygon, options);

	OGRFieldDefn labelField("label", OFTInteger);
	layer.CreateField(labelField, true);
	OGRFieldDefn MajorityField("Majority", OFTInteger);
	layer.CreateField(MajorityField, true);

	// Write label image
	/*
	WriterType::Pointer writer = WriterType::New();
	writer->SetInput(m_ChangeLabelFilter->GetOutput());
	writer->SetFileName("label_image.tif");
	writer->Update();
	*/

	// Filtre LabelImage vers OGRDataSource
	LabelImageToOGRDataSourceFilterType::Pointer labelToOGR = LabelImageToOGRDataSourceFilterType::New();
	labelToOGR->SetInput(m_ChangeLabelFilter->GetOutput());
	labelToOGR->SetInputMask(m_ChangeLabelFilter->GetOutput());		// La classe 0  est considérée comme du background et n'est pas vectorisée
	labelToOGR->SetFieldName("Majority");
	labelToOGR->Update();

	otb::ogr::DataSource::ConstPointer ogrDSTmp = labelToOGR->GetOutput();
	otb::ogr::Layer layerTmp = ogrDSTmp->GetLayerChecked(0);

	otb::ogr::Layer::const_iterator featIt = layerTmp.begin();

	int nveau_label = 1;
	for(; featIt!=layerTmp.end(); ++featIt)
	{
		otb::ogr::Feature dstFeature(layer.GetLayerDefn());
		dstFeature.SetFrom( *featIt, TRUE );
		layer.CreateFeature( dstFeature );
		dstFeature.ogr().SetField("label",nveau_label);
		layer.SetFeature(dstFeature);
		nveau_label +=1;
	}

	otbAppLogINFO(<<"Processing complete.");
}
