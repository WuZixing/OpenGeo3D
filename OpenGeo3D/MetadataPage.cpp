#include "MetadataPage.h"
#include "BusyCursor.h"
#include "Text.h"

MetadataPage::MetadataPage(QWidget* parent) : QtTreePropertyBrowser(parent) {
	g3dObject_ = nullptr;
	itemType_ = ItemType::Unknown;
	propManager_ = new QtVariantPropertyManager(this);
	QtVariantEditorFactory* propFactory = new QtVariantEditorFactory(this);
	setFactoryForManager(propManager_, propFactory);
	setRootIsDecorated(false);
	setResizeMode(QtTreePropertyBrowser::ResizeMode::Interactive);
	setAlternatingRowColors(false);
	setHeaderVisible(false);
}

MetadataPage::~MetadataPage() {

}

void MetadataPage::setCurrentItem(QTreeWidgetItem* item) {
	BusyCursor waiting;
	propManager_->clear();
	if (item == nullptr) {
		return;
	}
	void* ptr = item->data(0, Qt::ItemDataRole::UserRole).value<void*>();
	geo3dml::Object* g3dObject = static_cast<geo3dml::Object*>(ptr);
	if (g3dObject == g3dObject_) {
	 	return;
	}
	g3dObject_ = g3dObject;
	itemType_ = ItemType::Unknown;
	if (g3dObject_ == nullptr) {
		return;
	}
	geo3dml::Project* g3dProject = dynamic_cast<geo3dml::Project*>(g3dObject_);
	if (g3dProject != nullptr) {
		itemType_ = ItemType::G3DProject;
		setCurrentItemAsG3DProject(g3dProject);
	} else {
		geo3dml::Map* g3dMap = dynamic_cast<geo3dml::Map*>(g3dObject_);
		if (g3dMap != nullptr) {
			itemType_ = ItemType::G3DMap;
			setCurrentItemAsG3DMap(g3dMap);
		} else {
			geo3dml::Layer* g3dLayer = dynamic_cast<geo3dml::Layer*>(g3dObject_);
			if (g3dLayer != nullptr) {
				itemType_ = ItemType::G3DLayer;
				setCurrentItemAsG3DLayer(g3dLayer);
			} else {
				geo3dml::Actor* g3dActor = dynamic_cast<geo3dml::Actor*>(g3dObject_);
				if (g3dActor != nullptr) {
					itemType_ = ItemType::G3DActor;
					setCurrentItemAsG3DActor(g3dActor);
				}
			}
		}
	}

	update();
}

void MetadataPage::setCurrentItemAsG3DProject(geo3dml::Project* g3dProject) {
	setBasicMetaInfo(Text::labelOfStructureModel(), QString::fromUtf8(g3dProject->GetID().c_str()), g3dProject->GetMapCount());
	double minX = 0, minY = 0, minZ = 0, maxX = 0, maxY = 0, maxZ = 0;
	g3dProject->GetMinimumBoundingRectangle(minX, minY, minZ, maxX, maxY, maxZ);
	setMBRInfo(minX, minY, minZ, maxX, maxY, maxZ);
}

void MetadataPage::setCurrentItemAsG3DMap(geo3dml::Map* g3dMap) {
	setBasicMetaInfo(Text::labelOf3DMap(), QString::fromUtf8(g3dMap->GetID().c_str()), g3dMap->GetLayerCount());
	double minX = 0, minY = 0, minZ = 0, maxX = 0, maxY = 0, maxZ = 0;
	g3dMap->GetMinimumBoundingRectangle(minX, minY, minZ, maxX, maxY, maxZ);
	setMBRInfo(minX, minY, minZ, maxX, maxY, maxZ);
}

void MetadataPage::setCurrentItemAsG3DLayer(geo3dml::Layer* g3dLayer) {
	setBasicMetaInfo(Text::labelOfLayer(), QString::fromUtf8(g3dLayer->GetID().c_str()), g3dLayer->GetActorCount());
	double minX = 0, minY = 0, minZ = 0, maxX = 0, maxY = 0, maxZ = 0;
	g3dLayer->GetMinimumBoundingRectangle(minX, minY, minZ, maxX, maxY, maxZ);
	setMBRInfo(minX, minY, minZ, maxX, maxY, maxZ);
	// feature class
	geo3dml::FeatureClass* g3dFeatureClass = g3dLayer->GetBindingFeatureClass();
	QtVariantProperty* propFeatureClass = propManager_->addProperty(QtVariantPropertyManager::groupTypeId(), Text::labelOfFeatureClass());
	QString strId, strName;
	if (g3dFeatureClass != nullptr) {
		strId = QString::fromUtf8(g3dFeatureClass->GetID().c_str());
		strName = QString::fromUtf8(g3dFeatureClass->GetName().c_str());
	} else {
		strId = QString::fromUtf8(g3dLayer->GetID().c_str());
	}
	QtVariantProperty* propItem = propManager_->addProperty(QMetaType::Type::QString, Text::labelOfId());
	propItem->setValue(strId);
	propItem->setAttribute(attriReadOnly_, true);
	propFeatureClass->addSubProperty(propItem);
	propItem = propManager_->addProperty(QMetaType::Type::QString, Text::labelOfName());
	propItem->setValue(strName);
	propItem->setAttribute(attriReadOnly_, true);
	propFeatureClass->addSubProperty(propItem);
	// schema
	QtVariantProperty* propSchema = propManager_->addProperty(QtVariantPropertyManager::groupTypeId(), Text::labelOfSchema());
	if (g3dFeatureClass != nullptr) {
		for (int i = 0; i < g3dFeatureClass->GetFieldCount(); ++i) {
			const geo3dml::Field& field = g3dFeatureClass->GetFieldAt(i);
			setFieldInfo(propSchema, field, i);
		}
	}
	propFeatureClass->addSubProperty(propSchema);

	QtBrowserItem* groupItem = addProperty(propFeatureClass);
	QList<QtBrowserItem*> children = groupItem->children();
	auto citor = children.cbegin();
	while (citor != children.cend()) {
		if ((*citor)->property() == propSchema) {
			QList<QtBrowserItem*> fieldItems = (*citor)->children();
			for (QtBrowserItem* item : fieldItems) {
				setExpanded(item, false);
			}
			break;
		}
		++citor;
	}
}

void MetadataPage::setCurrentItemAsG3DActor(geo3dml::Actor* g3dActor) {
	setBasicMetaInfo(Text::labelOfActor(), QString::fromUtf8(g3dActor->GetID().c_str()), 0);
	double minX = 0, minY = 0, minZ = 0, maxX = 0, maxY = 0, maxZ = 0;
	g3dActor->GetMinimumBoundingRectangle(minX, minY, minZ, maxX, maxY, maxZ);
	setMBRInfo(minX, minY, minZ, maxX, maxY, maxZ);
}

void MetadataPage::setBasicMetaInfo(const QString& datasetName, const QString& datasetId, int childrenNumber) {
	QtVariantProperty* propBasicInfo = propManager_->addProperty(QtVariantPropertyManager::groupTypeId(), Text::labelOfBasicMetaInfo());
	QtVariantProperty* propItem = propManager_->addProperty(QMetaType::Type::QString, Text::labelOfDataset());
	propItem->setValue(datasetName);
	propItem->setEnabled(false);
	propBasicInfo->addSubProperty(propItem);

	propItem = propManager_->addProperty(QMetaType::Type::Int, Text::labelOfSubNodeNumber());
	propItem->setValue(childrenNumber);
	propItem->setAttribute(attriReadOnly_, true);
	propBasicInfo->addSubProperty(propItem);

	propItem = propManager_->addProperty(QMetaType::Type::QString, Text::labelOfId());
	propItem->setValue(datasetId);
	propItem->setAttribute(attriReadOnly_, true);
	propBasicInfo->addSubProperty(propItem);

	addProperty(propBasicInfo);
}

void MetadataPage::setMBRInfo(double minX, double minY, double minZ, double maxX, double maxY, double maxZ) {
	QtVariantProperty* propAABB = propManager_->addProperty(QtVariantPropertyManager::groupTypeId(), Text::labelOfAABB());
	QtVariantProperty* propItem = propManager_->addProperty(QMetaType::Type::QString, Text::labelOfAABBMinPoint());
	propItem->setValue(QStringLiteral("(%1, %2, %3)").arg(QString::number(minX, 'f')).arg(QString::number(minY, 'f')).arg(QString::number(minZ, 'f')));
	propItem->setAttribute(attriReadOnly_, true);
	propAABB->addSubProperty(propItem);

	propItem = propManager_->addProperty(QMetaType::Type::QString, Text::labelOfAABBMaxPoint());
	propItem->setValue(QStringLiteral("(%1, %2, %3)").arg(QString::number(maxX, 'f')).arg(QString::number(maxY, 'f')).arg(QString::number(maxZ, 'f')));
	propItem->setAttribute(attriReadOnly_, true);
	propAABB->addSubProperty(propItem);

	addProperty(propAABB);
}

void MetadataPage::setFieldInfo(QtVariantProperty* parentProp, const geo3dml::Field& field, int index, const QString& namePrefix) {
	QtVariantProperty* propField = propManager_->addProperty(QMetaType::Type::QString, QString::number(index + 1));
	propField->setValue(QString::fromUtf8(field.Name().c_str()));
	propField->setAttribute(attriReadOnly_, true);
	QtVariantProperty* propItem = propManager_->addProperty(QMetaType::Type::QString, QStringLiteral("Label"));
	propItem->setValue(QString::fromUtf8(field.Label().c_str()));
	propItem->setAttribute(attriReadOnly_, true);
	propField->addSubProperty(propItem);
	propItem = propManager_->addProperty(QMetaType::Type::QString, QStringLiteral("DataType"));
	propItem->setValue(QString::fromUtf8(geo3dml::Field::ValueTypeToName(field.DataType()).c_str()));
	propItem->setAttribute(attriReadOnly_, true);
	propField->addSubProperty(propItem);
	propItem = propManager_->addProperty(QMetaType::Type::QString, QStringLiteral("UOM"));
	propItem->setValue(QString::fromUtf8(field.Uom().c_str()));
	propItem->setAttribute(attriReadOnly_, true);
	propField->addSubProperty(propItem);
	propItem = propManager_->addProperty(QMetaType::Type::QString, QStringLiteral("Definition"));
	propItem->setValue(QString::fromUtf8(field.Definition().c_str()));
	propItem->setAttribute(attriReadOnly_, true);
	propField->addSubProperty(propItem);
	propItem = propManager_->addProperty(QMetaType::Type::QString, QStringLiteral("Descriptoin"));
	propItem->setValue(QString::fromUtf8(field.Description().c_str()));
	propItem->setAttribute(attriReadOnly_, true);
	propField->addSubProperty(propItem);
	propField->setEnabled(false);

	parentProp->addSubProperty(propField);
}
