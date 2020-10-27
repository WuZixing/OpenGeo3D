#include "RenderOptionPage.h"

RenderOptionPage::RenderOptionPage(wxWindow* parent) : wxPropertyGrid(parent) {

}

RenderOptionPage::~RenderOptionPage() {

}

void RenderOptionPage::BindToItem(geo3dml::Object* g3dObject, G3DTreeItemData::ItemType itemType) {
	Clear();
	switch (itemType) {
	case G3DTreeItemData::ItemType::G3D_StructureModel: {
		geo3dml::Project* g3dProject = static_cast<geo3dml::Project*>(g3dObject);
		BindToStructureModels(g3dProject);
		break;
	}
	case G3DTreeItemData::ItemType::G3D_GridModel:
		break;
	case G3DTreeItemData::ItemType::G3D_Map: {
		geo3dml::Map* g3dMap = static_cast<geo3dml::Map*>(g3dObject);
		BindToG3DMap(g3dMap);
		break;
	}
	case G3DTreeItemData::ItemType::G3D_Layer: {
		geo3dml::Layer* g3dLayer = static_cast<geo3dml::Layer*>(g3dObject);
		BindToG3DLayer(g3dLayer);
		break;
	}
	case G3DTreeItemData::ItemType::G3D_Actor: {
		geo3dml::Actor* g3dActor = static_cast<geo3dml::Actor*>(g3dObject);
		BindToG3DActor(g3dActor);
		break;
	}
	default:
		break;
	}
}

void RenderOptionPage::BindToStructureModels(geo3dml::Project* g3dProject) {

}

void RenderOptionPage::BindToG3DMap(geo3dml::Map* g3dMap) {

}

void RenderOptionPage::BindToG3DLayer(geo3dml::Layer* g3dLayer) {

}

void RenderOptionPage::BindToG3DActor(geo3dml::Actor* g3dActor) {

}