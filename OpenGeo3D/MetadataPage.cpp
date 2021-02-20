#include "MetadataPage.h"
#include <geo3dml/Project.h>
#include <geo3dml/Map.h>
#include <geo3dml/Layer.h>
#include <geo3dml/Actor.h>
#include "BusyCursor.h"
#include "Text.h"

MetadataPage::MetadataPage(QWidget* parent) : QtTreePropertyBrowser(parent) {
	g3dObject_ = nullptr;
	itemType_ = ItemType::Unknown;
	setResizeMode(QtTreePropertyBrowser::ResizeMode::Interactive);
	propManager_ = new QtVariantPropertyManager(this);
	QtVariantEditorFactory* propFactory = new QtVariantEditorFactory(this);
	setFactoryForManager(propManager_, propFactory);
	setRootIsDecorated(false);
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
	} else {
		geo3dml::Map* g3dMap = dynamic_cast<geo3dml::Map*>(g3dObject_);
		if (g3dMap != nullptr) {
			itemType_ = ItemType::G3DMap;
		} else {
			geo3dml::Layer* g3dLayer = dynamic_cast<geo3dml::Layer*>(g3dObject_);
			if (g3dLayer != nullptr) {
				itemType_ = ItemType::G3DLayer;
			} else {
				geo3dml::Actor* g3dActor = dynamic_cast<geo3dml::Actor*>(g3dObject_);
				if (g3dActor != nullptr) {
					itemType_ = ItemType::G3DActor;
				}
			}
		}
	}
	updateBasicMetaInfo(item->childCount());

	update();
}

void MetadataPage::updateBasicMetaInfo(int subNodeNumber) {
	QString datasetName;
	double minX = 0, minY = 0, minZ = 0, maxX = 0, maxY = 0, maxZ = 0;
	switch (itemType_) {
	case ItemType::G3DProject: {
		datasetName = Text::labelOfStructureModel();
		geo3dml::Project* g3dProject = static_cast<geo3dml::Project*>(g3dObject_);
		g3dProject->GetMinimumBoundingRectangle(minX, minY, minZ, maxX, maxY, maxZ);
		break;
	}
	case ItemType::G3DMap: {
		datasetName = Text::labelOf3DMap();
		geo3dml::Map* g3dMap = static_cast<geo3dml::Map*>(g3dObject_);
		g3dMap->GetMinimumBoundingRectangle(minX, minY, minZ, maxX, maxY, maxZ);
		break;
	}
	case ItemType::G3DLayer: {
		datasetName = Text::labelOfLayer();
		geo3dml::Layer* g3dLayer = static_cast<geo3dml::Layer*>(g3dObject_);
		g3dLayer->GetMinimumBoundingRectangle(minX, minY, minZ, maxX, maxY, maxZ);
		break;
	}
	case ItemType::G3DActor: {
		datasetName = Text::labelOfActor();
		geo3dml::Actor* g3dActor = static_cast<geo3dml::Actor*>(g3dObject_);
		g3dActor->GetMinimumBoundingRectangle(minX, minY, minZ, maxX, maxY, maxZ);
		break;
	}
	default:
		break;
	}

	QtProperty* propBasicInfo = propManager_->addProperty(QtVariantPropertyManager::groupTypeId(), Text::labelOfBasicMetaInfo());
	QtVariantProperty* propItem = propManager_->addProperty(QMetaType::Type::QString, Text::labelOfDataset());
	propItem->setValue(datasetName);
	propItem->setEnabled(false);
	propBasicInfo->addSubProperty(propItem);

	propItem = propManager_->addProperty(QMetaType::Type::Int, Text::labelOfSubNodeNumber());
	propItem->setValue(subNodeNumber);
	propItem->setAttribute(attriReadOnly_, true);
	propBasicInfo->addSubProperty(propItem);

	propItem = propManager_->addProperty(QMetaType::Type::QString, Text::labelOfId());
	propItem->setValue(QString::fromUtf8(g3dObject_->GetID().c_str()));
	propItem->setAttribute(attriReadOnly_, true);
	propBasicInfo->addSubProperty(propItem);

	QtVariantProperty* propAABB = propManager_->addProperty(QtVariantPropertyManager::groupTypeId(), Text::labelOfAABB());
	propItem = propManager_->addProperty(QMetaType::Type::QString, Text::labelOfAABBMinPoint());
	propItem->setValue(QStringLiteral("(%1, %2, %3)").arg(QString::number(minX, 'f')).arg(QString::number(minY, 'f')).arg(QString::number(minZ, 'f')));
	propItem->setAttribute(attriReadOnly_, true);
	propAABB->addSubProperty(propItem);
	propItem = propManager_->addProperty(QMetaType::Type::QString, Text::labelOfAABBMaxPoint());
	propItem->setValue(QStringLiteral("(%1, %2, %3)").arg(QString::number(maxX, 'f')).arg(QString::number(maxY, 'f')).arg(QString::number(maxZ, 'f')));
	propItem->setAttribute(attriReadOnly_, true);
	propAABB->addSubProperty(propItem);

	addProperty(propBasicInfo);
	addProperty(propAABB);
}
