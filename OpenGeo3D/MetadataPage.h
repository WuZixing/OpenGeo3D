#pragma once

#include <QtWidgets/QTreeWidget>
#include <QtTreePropertyBrowser>
#include <QtVariantPropertyManager>
#include <geo3dml/Geo3DML.h>

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

	void updateBasicMetaInfo(int subNodeNumber);

private:
	QtVariantPropertyManager* propManager_;
	geo3dml::Object* g3dObject_;
	ItemType itemType_;

	const QString attriReadOnly_ = QStringLiteral("readOnly");
};
