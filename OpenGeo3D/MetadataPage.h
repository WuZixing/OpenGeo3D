#pragma once

#include <QtWidgets/QTreeWidget>
#include <QtTreePropertyBrowser>
#include <QtVariantPropertyManager>
#include <geo3dml/Project.h>
#include <geo3dml/Map.h>
#include <geo3dml/Layer.h>
#include <geo3dml/Actor.h>

class MetadataPage : public QtTreePropertyBrowser {
	Q_OBJECT

public:
	MetadataPage(QWidget* parent);
	virtual ~MetadataPage();

	void setCurrentItem(QTreeWidgetItem* item);

private:
	enum class ItemType {
		Unknown,
		G3DProject,
		G3DMap,
		G3DLayer,
		G3DActor
	};

	void setCurrentItemAsG3DProject(geo3dml::Project* g3dProject);
	void setCurrentItemAsG3DMap(geo3dml::Map* g3dMap);
	void setCurrentItemAsG3DLayer(geo3dml::Layer* g3dLayer);
	void setCurrentItemAsG3DActor(geo3dml::Actor* g3dActor);

	void setBasicMetaInfo(const QString& datasetName, const QString& datasetId, int childrenNumber);
	void setMBRInfo(double minX, double minY, double minZ, double maxX, double maxY, double maxZ);
	void setFieldInfo(QtVariantProperty* parentProp, const geo3dml::Field& field, int index, const QString& namePrefix = QString());

private:
	QtVariantPropertyManager* propManager_;
	geo3dml::Object* g3dObject_;
	ItemType itemType_;

	const QString attriReadOnly_ = QStringLiteral("readOnly");
};
