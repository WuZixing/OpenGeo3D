#include "ProjectTreeCtrl.h"
#include <QtGui/QContextMenuEvent>
#include <QtWidgets/QMenu>
#include <g3dvtk/Actor.h>
#include <g3dvtk/ObjectFactory.h>
#include "Events.h"
#include "Text.h"

ProjectTreeCtrl::ProjectTreeCtrl(QWidget* parent) : QTreeWidget(parent) {
	setColumnCount(1);
	setHeaderHidden(true);

	rootOfStructureModel_ = new QTreeWidgetItem(this);
	rootOfStructureModel_->setText(0, Text::nameOfStructureModel());
	rootOfStructureModel_->setCheckState(0, Qt::CheckState::Checked);
	addTopLevelItem(rootOfStructureModel_);

	g3dvtk::ObjectFactory g3dFactory;
	g3dProject_.reset(g3dFactory.NewProject());
	g3dProject_->SetID(geo3dml::Object::NewID());
	g3dProject_->SetName(Text::nameOfStructureModel().toUtf8().constData());

	rootOfStructureModel_->setData(0, Qt::ItemDataRole::UserRole, QVariant::fromValue<void*>(g3dProject_.get()));
	rootOfStructureModel_->setText(0, QString::fromUtf8(g3dProject_->GetName().c_str()));

	renderer_ = vtkSmartPointer<vtkRenderer>::New();
	transform_ = vtkSmartPointer<vtkTransform>::New();
	transform_->Identity();

	connect(this, &ProjectTreeCtrl::itemChanged, this, &ProjectTreeCtrl::onItemChanged);
}

ProjectTreeCtrl::~ProjectTreeCtrl() {

}

vtkRenderer* ProjectTreeCtrl::getRenderer() const {
	return renderer_;
}

vtkTransform* ProjectTreeCtrl::getTransform() const {
	return transform_;
}

void ProjectTreeCtrl::appendG3DModel(geo3dml::Model* model, bool appendToDefaultMap) {
	g3dProject_->AddModel(model);
	if (!appendToDefaultMap) {
		return;
	}

	geo3dml::Map* g3dMap = getDefaultMap();
	int numOfFeatureClasses = model->GetFeatureClassCount();
	for (int i = 0; i < numOfFeatureClasses; ++i) {
		appendFeatureClassToMap(model->GetFeatureClassAt(i), g3dMap);
	}
	updateSubTreeOfMap(g3dMap);
}

void ProjectTreeCtrl::appendG3DMap(geo3dml::Map* map) {
	g3dProject_->AddMap(map);
	updateSubTreeOfMap(map);
}

void ProjectTreeCtrl::updateVoxelGridModel() {

}

void ProjectTreeCtrl::expandStructureModelTree() {
	rootOfStructureModel_->setExpanded(true);
}

void ProjectTreeCtrl::expandGridModelNodeTree() {

}

geo3dml::Map* ProjectTreeCtrl::getDefaultMap() {
	if (g3dProject_->GetMapCount() > 0) {
		return g3dProject_->GetMapAt(0);
	} else {
		g3dvtk::ObjectFactory g3dFactory;
		geo3dml::Map* map = g3dFactory.NewMap();
		map->SetID(geo3dml::Object::NewID());
		map->SetName(Text::nameOfDefaultG3DMap().toUtf8().constData());
		g3dProject_->AddMap(map);
		return map;
	}
}

void ProjectTreeCtrl::updateSubTreeOfMap(geo3dml::Map* g3dMap) {
	QTreeWidgetItem* mapItem = findOrInsertMapItem(g3dMap);
	int numberOfLayers = g3dMap->GetLayerCount();
	for (int i = 0; i < numberOfLayers; ++i) {
		bool itemExists = false;
		geo3dml::Layer* layer = g3dMap->GetLayerAt(i);
		for (int m = 0; m < mapItem->childCount(); ++m) {
			QTreeWidgetItem* child = mapItem->child(m);
			void* ptr = child->data(0, Qt::ItemDataRole::UserRole).value<void*>();
			geo3dml::Layer* layerInItem = dynamic_cast<geo3dml::Layer*>((geo3dml::Object*)ptr);
			if (layerInItem != nullptr && layerInItem == layer) {
				itemExists = true;
				break;
			}
		}
		if (itemExists) {
			continue;
		}
		QTreeWidgetItem* layerItem = new QTreeWidgetItem(mapItem);
		layerItem->setText(0, QString::fromUtf8(layer->GetName().c_str()));
		layerItem->setCheckState(0, Qt::CheckState::Checked);
		layerItem->setData(0, Qt::ItemDataRole::UserRole, QVariant::fromValue<void*>(layer));
		int numberOfActors = layer->GetActorCount();
		for (int m = 0; m < numberOfActors; ++m) {
			g3dvtk::Actor* g3dActor = dynamic_cast<g3dvtk::Actor*>(layer->GetActorAt(m));
			if (g3dActor != nullptr) {
				g3dActor->SetUserTransform(transform_);
				renderer_->AddActor(g3dActor->GetVTKProp());
				QTreeWidgetItem* actorItem = new QTreeWidgetItem(layerItem);
				actorItem->setText(0, QString::fromUtf8(g3dActor->GetName().c_str()));
				actorItem->setCheckState(0, g3dActor->IsVisible() ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
				actorItem->setData(0, Qt::ItemDataRole::UserRole, QVariant::fromValue<void*>(g3dActor));
			}
		}
	}
	update();
}

void ProjectTreeCtrl::appendFeatureClassToMap(geo3dml::FeatureClass* g3dFC, geo3dml::Map* g3dMap) {
	g3dvtk::ObjectFactory g3dFactory;
	geo3dml::Layer* layer = g3dFactory.NewLayer();
	layer->SetName(g3dFC->GetName());
	layer->BindFeatureClass(g3dFC);
	layer->BuildActorsFromFeatures(&g3dFactory);
	g3dMap->AddLayer(layer);
}

QTreeWidgetItem* ProjectTreeCtrl::findOrInsertMapItem(geo3dml::Map* g3dMap) {
	for (int c = 0; c < rootOfStructureModel_->childCount(); ++c) {
		QTreeWidgetItem* child = rootOfStructureModel_->child(c);
		void* ptr = child->data(0, Qt::ItemDataRole::UserRole).value<void*>();
		geo3dml::Map* mapInItem = dynamic_cast<geo3dml::Map*>((geo3dml::Object*)ptr);
		if (mapInItem != nullptr && mapInItem == g3dMap) {
			return child;
		}
	}
	QTreeWidgetItem* item = new QTreeWidgetItem(rootOfStructureModel_);
	item->setText(0, QString::fromUtf8(g3dMap->GetName().c_str()));
	item->setCheckState(0, Qt::CheckState::Checked);
	item->setData(0, Qt::ItemDataRole::UserRole, QVariant::fromValue<void*>(g3dMap));
	return item;
}

void ProjectTreeCtrl::closeAllModels() {

}

void ProjectTreeCtrl::closeStructureModel() {
	removeChildrenFromScene(rootOfStructureModel_);
	resetStructureModel();
	emit itemSelectionChanged();
}

void ProjectTreeCtrl::closeVoxelGridModel() {

}

void ProjectTreeCtrl::removeChildrenFromScene(QTreeWidgetItem* item) {
	int numberOfChildren = item->childCount();
	for (int m = 0; m < numberOfChildren; ++m) {
		QTreeWidgetItem* child = item->takeChild(0);
		void* ptr = child->data(0, Qt::ItemDataRole::UserRole).value<void*>();
		g3dvtk::Actor* g3dActor = dynamic_cast<g3dvtk::Actor*>((geo3dml::Object*)ptr);
		if (g3dActor != nullptr) {
			renderer_->RemoveActor(g3dActor->GetVTKProp());
		}
		removeChildrenFromScene(child);
		delete child;
	}
}

void ProjectTreeCtrl::resetStructureModel() {
	g3dvtk::ObjectFactory g3dFactory;
	g3dProject_.reset(g3dFactory.NewProject());
	g3dProject_->SetID(geo3dml::Object::NewID());
	g3dProject_->SetName(Text::nameOfStructureModel().toUtf8().constData());
	rootOfStructureModel_->setData(0, Qt::ItemDataRole::UserRole, QVariant::fromValue<void*>(g3dProject_.get()));
	rootOfStructureModel_->setText(0, QString::fromUtf8(g3dProject_->GetName().c_str()));
}

void ProjectTreeCtrl::resetVoxelGridModel() {

}

void ProjectTreeCtrl::onItemChanged(QTreeWidgetItem* item, int column) {
	if (item == nullptr || column != 0) {
		return;
	}
	Qt::CheckState state = item->checkState(column);
	switch (state) {
	case Qt::CheckState::Checked: {
		void* ptr = item->data(0, Qt::ItemDataRole::UserRole).value<void*>();
		g3dvtk::Actor* g3dActor = dynamic_cast<g3dvtk::Actor*>((geo3dml::Object*)ptr);
		if (g3dActor != nullptr) {
			g3dActor->SetVisible(true);
		}
		checkChildrenState(item, Qt::CheckState::Checked);
		Events::PostEvent(Events::Type::UpdateScene);
		break;
	}
	case Qt::CheckState::Unchecked: {
		void* ptr = item->data(0, Qt::ItemDataRole::UserRole).value<void*>();
		g3dvtk::Actor* g3dActor = dynamic_cast<g3dvtk::Actor*>((geo3dml::Object*)ptr);
		if (g3dActor != nullptr) {
			g3dActor->SetVisible(false);
		}
		checkChildrenState(item, Qt::CheckState::Unchecked);
		Events::PostEvent(Events::Type::UpdateScene);
		break;
	}
	default:
		break;
	}
}

void ProjectTreeCtrl::checkChildrenState(QTreeWidgetItem* item, Qt::CheckState state) {
	int numberOfChildren = item->childCount();
	for (int m = 0; m < numberOfChildren; ++m) {
		QTreeWidgetItem* child = item->child(m);
		if (child->checkState(0) != state) {
			child->setCheckState(0, state);
			void* ptr = child->data(0, Qt::ItemDataRole::UserRole).value<void*>();
			g3dvtk::Actor* g3dActor = dynamic_cast<g3dvtk::Actor*>((geo3dml::Object*)ptr);
			if (g3dActor != nullptr) {
				g3dActor->SetVisible(state == Qt::CheckState::Checked);
			}
			checkChildrenState(child, state);
		}
	}
}

void ProjectTreeCtrl::contextMenuEvent(QContextMenuEvent* event) {
	event->accept();

	QTreeWidgetItem* curItem = currentItem();
	if (curItem == nullptr) {
		return;
	}
	void* ptr = curItem->data(0, Qt::ItemDataRole::UserRole).value<void*>();
	if (ptr == g3dProject_.get()) {
		contextMenuOfStructureModel(event->globalPos());
	}
}

void ProjectTreeCtrl::contextMenuOfStructureModel(const QPoint& position) {
	QMenu ctxMenu(Text::labelOfStructureModel(), this);
	ctxMenu.addAction(Text::labelOfStructureModel());
	ctxMenu.addSeparator();
	ctxMenu.addAction(Text::menuOpenGeo3DML(), []() { Events::PostEvent(Events::Type::Menu_OpenGeo3DML); });
	ctxMenu.addAction(Text::menuOpenDrillLog(), []() { Events::PostEvent(Events::Type::Menu_OpenDrillLog); });
	ctxMenu.addSeparator();
	ctxMenu.addAction(Text::menuCloseStructureModel(), []() { Events::PostEvent(Events::Type::Menu_CloseStructureModel); });
	ctxMenu.exec(position);
}
