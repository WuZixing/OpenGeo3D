#pragma once

#include <QtWidgets/QTreeWidget>
#include <QtTreePropertyBrowser>
#include <QtVariantPropertyManager>
#include <geo3dml/Project.h>

class MetadataPage : public QtTreePropertyBrowser {
	Q_OBJECT

public:
	MetadataPage(QWidget* parent);
	virtual ~MetadataPage();

	void setCurrentItem(QTreeWidgetItem* item);

private:
	void setCurrentItemAsG3DProject(geo3dml::Project* g3dProject);
	void setCurrentItemAsG3DMap(geo3dml::Map* g3dMap);
	void setCurrentItemAsG3DLayer(geo3dml::Layer* g3dLayer);
	void setCurrentItemAsG3DActor(geo3dml::Actor* g3dActor);

	void setBasicMetaInfo(const QString& datasetName, const QString& datasetId, int childrenNumber);
	void setMBRInfo(double minX, double minY, double minZ, double maxX, double maxY, double maxZ);
	void setFieldInfo(QtVariantProperty* parentProp, const geo3dml::Field& field, int index);
	void setFeatureInfo(geo3dml::Feature* g3dFeature);
	void setGeometryInfo(geo3dml::Geometry* g3dGeometry);
	void setShapePropertyOfGeometry(QtVariantProperty* parentProp, geo3dml::ShapeProperty* g3dShapeProperty);

private:
	QtVariantPropertyManager* propManager_;
	geo3dml::Object* g3dObject_;

	const QString attriReadOnly_ = QStringLiteral("readOnly");
};
