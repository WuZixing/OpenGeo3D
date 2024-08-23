// UTF-8编码
#include "DataLoaderGeoJSON.h"
#include <QFileDialog>
#include <ogrsf_frmts.h>
#include <g3dvtk/ObjectFactory.h>
#include "Text.h"

using namespace opengeo3d;

DataLoaderGeoJSON::DataLoaderGeoJSON() {

}

DataLoaderGeoJSON::~DataLoaderGeoJSON() {

}

geo3dml::Model* DataLoaderGeoJSON::loadAsG3DModel() const {
	QString filePath = QFileDialog::getOpenFileName(nullptr, Text::menuOpenGeoJSON(), QDir::currentPath(), Text::filterOfGeoJSONFile());
	if (!filePath.isEmpty()) {
		QDir dir(filePath);
		dir.cdUp();
		QDir::setCurrent(dir.path());
	} else {
		return nullptr;
	}
	// GDALDriverManager* pDriverManager = GetGDALDriverManager();
	// GDALDriver* pGeoJSONDriver = pDriverManager->GetDriverByName("GeoJSON");
	// if (pGeoJSONDriver == nullptr) {
	// 	return nullptr;
	// }
	// 读取GeoJSON
	GDALAllRegister();
	char** papszOptions = nullptr;
	papszOptions = CSLInsertString(papszOptions, 0, "GeoJSON");
	GDALDataset* pGeoJSONDataset = (GDALDataset*)GDALOpenEx(filePath.toUtf8().constData(), GDAL_OF_VECTOR | GDAL_OF_READONLY, papszOptions, nullptr, nullptr);
	CSLDestroy(papszOptions);
	papszOptions = nullptr;
	if (pGeoJSONDataset == nullptr) {
		return nullptr;
	}
	geo3dml::Model* model = nullptr;
	OGRLayer* layer = pGeoJSONDataset->GetLayer(0);
	if (layer) {
		// feature class
		g3dvtk::ObjectFactory g3dFactory;
		geo3dml::FeatureClass* featureClass = g3dFactory.NewFeatureClass();
		featureClass->SetID("GeoJSON");
		featureClass->SetName(layer->GetName());
		// schema
		OGRFeatureDefn* fields = layer->GetLayerDefn();
		int fieldCount = fields->GetFieldCount();
		for (int m = 0; m < fieldCount; ++m) {
			const OGRFieldDefn* field = fields->GetFieldDefn(m);
			geo3dml::Field g3dField;
			g3dField.Name(field->GetNameRef()).Description(field->GetComment());
			switch (field->GetType()) {
			case OGRFieldType::OFTInteger:
			case OGRFieldType::OFTInteger64:
				g3dField.DataType(geo3dml::Field::ValueType::Integer);
				break;
			case OGRFieldType::OFTReal:
				g3dField.DataType(geo3dml::Field::ValueType::Double);
				break;
			case OGRFieldType::OFTString:
				g3dField.DataType(geo3dml::Field::ValueType::Text);
				break;
			default:
				break;
			}
			featureClass->AddField(g3dField);
		}
		// features
		int featureCount = layer->GetFeatureCount();
		for (int m = 0; m < featureCount; ++m) {
			OGRFeature* feature = layer->GetFeature(m);
			geo3dml::Feature* g3dFeature = g3dFactory.NewFeature();
			g3dFeature->SetID(std::to_string(feature->GetFID()));
			g3dFeature->SetName(g3dFeature->GetID());
			for (int n = 0; n < fieldCount; ++n) {
				const OGRFieldDefn* field = fields->GetFieldDefn(n);
				switch (field->GetType()) {
				case OGRFieldType::OFTInteger: {
					int v = feature->GetFieldAsInteger(n);
					geo3dml::FieldValue intValue(field->GetNameRef(), v);
					g3dFeature->SetField(intValue);
					break;
				}
				case OGRFieldType::OFTInteger64: {
					GIntBig v = feature->GetFieldAsInteger64(n);
					geo3dml::FieldValue intValue(field->GetNameRef(), (int)v);
					g3dFeature->SetField(intValue);
					break;
				}
				case OGRFieldType::OFTReal: {
					double v = feature->GetFieldAsDouble(n);
					geo3dml::FieldValue doubleValue(field->GetNameRef(), v);
					g3dFeature->SetField(doubleValue);
					break;
				}
				case OGRFieldType::OFTString: {
					const char* v = feature->GetFieldAsString(n);
					geo3dml::FieldValue strValue(field->GetNameRef(), v);
					g3dFeature->SetField(strValue);
					break;
				}
				default:
					break;
				}

				geo3dml::MultiLineString* mLine = g3dFactory.NewMultiLineString();
				OGRGeometry* geo = feature->GetGeometryRef();
				OGRGeometryCollection* geoCollection = dynamic_cast<OGRGeometryCollection*>(geo);
				if (geoCollection != nullptr) {
					int geoCount = geoCollection->getNumGeometries();
					for (int g = 0; g < geoCount; ++g) {
						OGRMultiPolygon* mPolygon = dynamic_cast<OGRMultiPolygon*>(geoCollection->getGeometryRef(g));
						if (mPolygon != nullptr) {
							OGRPoint pt;
							for (int i = 0; i < mPolygon->getNumGeometries(); ++i) {
								OGRPolygon* polygon = mPolygon->getGeometryRef(i);
								std::unique_ptr<geo3dml::LineString> lineString(g3dFactory.NewLineString());
								OGRLinearRing* ring = polygon->getExteriorRing();
								int pointCount = ring->getNumPoints();
								for (int j = 0; j < pointCount; ++j) {
									ring->getPoint(j, &pt);
									//mLine->AddVertexToLineString(lineIndex, pt.getX(), pt.getY(), pt.getZ());
									lineString->AddVertex(pt.getX(), pt.getY(), pt.getZ());
								}
								mLine->AddLineString(lineString.get());
								for (int k = 0; k < polygon->getNumInteriorRings(); ++k) {
									lineString.reset(g3dFactory.NewLineString());
									OGRLinearRing* ring = polygon->getInteriorRing(k);
									int pointCount = ring->getNumPoints();
									for (int j = 0; j < pointCount; ++j) {
										ring->getPoint(j, &pt);
										// mLine->AddVertexToLineString(lineIndex, pt.getX(), pt.getY(), pt.getZ());
										lineString->AddVertex(pt.getX(), pt.getY(), pt.getZ());
									}
									mLine->AddLineString(lineString.get());
								}
							}
						} else {
							OGRPolygon* polygon = dynamic_cast<OGRPolygon*>(geoCollection->getGeometryRef(g));
							if (polygon != nullptr) {
								OGRPoint pt;
								std::unique_ptr<geo3dml::LineString> lineString(g3dFactory.NewLineString());
								OGRLinearRing* ring = polygon->getExteriorRing();
								int pointCount = ring->getNumPoints();
								for (int j = 0; j < pointCount; ++j) {
									ring->getPoint(j, &pt);
									// mLine->AddVertexToLineString(lineIndex, pt.getX(), pt.getY(), pt.getZ());
									lineString->AddVertex(pt.getX(), pt.getY(), pt.getZ());
								}
								mLine->AddLineString(lineString.get());
								for (int k = 0; k < polygon->getNumInteriorRings(); ++k) {
									// lineIndex = mLine->AddLineString();
									lineString.reset(g3dFactory.NewLineString());
									OGRLinearRing* ring = polygon->getInteriorRing(k);
									int pointCount = ring->getNumPoints();
									for (int j = 0; j < pointCount; ++j) {
										ring->getPoint(j, &pt);
										// mLine->AddVertexToLineString(lineIndex, pt.getX(), pt.getY(), pt.getZ());
										lineString->AddVertex(pt.getX(), pt.getY(), pt.getZ());
									}
									mLine->AddLineString(lineString.get());
								}
							}
						}
					}
				}
				g3dFeature->SetGeometry(mLine);
			}
			featureClass->AddFeature(g3dFeature);
		}
		model = g3dFactory.NewModel();
		model->SetID("GeoJSON");
		model->SetName(featureClass->GetName());
		model->SetType(geo3dml::Model::ModelType::Model3D);
		model->AddFeatureClass(featureClass);
	}
	GDALClose(pGeoJSONDataset);
	return model;
}
