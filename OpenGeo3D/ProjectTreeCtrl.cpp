#include "ProjectTreeCtrl.h"
#include <g3dvtk/Actor.h>
#include <g3dvtk/ObjectFactory.h>
#include "checked_box.xpm"
#include "unchecked_box.xpm"
#include "DlgStructureModelGridding.h"
#include "Events.h"
#include "Strings.h"

ProjectTreeCtrl::ProjectTreeCtrl(wxWindow* parent, const wxSize& size) : 
	wxTreeCtrl(parent, wxID_ANY, wxDefaultPosition, size, wxTR_DEFAULT_STYLE | wxNO_BORDER | wxTR_HIDE_ROOT | wxTR_FULL_ROW_HIGHLIGHT), 
	ItemState_Checked_(0), ItemState_Unchecked_(1) {

	g3dvtk::ObjectFactory g3dFactory;
	g3dProject_.reset(g3dFactory.NewProject());
	g3dProject_->SetID(geo3dml::Object::NewID());
	g3dProject_->SetName(Strings::NameOfStructureModel().ToUTF8().data());
	g3dVoxelGrid_ = std::make_unique<g3dgrid::VoxelGrid>();
	g3dVoxelGrid_->SetID(geo3dml::Object::NewID());
	g3dVoxelGrid_->SetName(Strings::NameOfGridModel().ToUTF8().data());
	g3dVoxelGrid_->SetSRS("CGCS2000");
	g3dVoxelGrid_->SetDescription(Strings::NameOfVoxelGrid().ToUTF8().data());

	wxIcon icons[2];
	icons[ItemState_Checked_] = wxIcon(xpm_checked_box);
	icons[ItemState_Unchecked_] = wxIcon(xpm_unchecked_box);
	wxImageList* states = new wxImageList(icons[0].GetWidth(), icons[0].GetHeight(), true, 2);
	states->Add(icons[0]);
	states->Add(icons[1]);
	AssignStateImageList(states);

	wxTreeItemId rootItem = AddRoot(wxEmptyString, 0);
	rootOfStructureModel_ = AppendItem(rootItem, Strings::NameOfStructureModel());
	SetItemData(rootOfStructureModel_, new G3DTreeItemData(g3dProject_.get(), G3DTreeItemData::ItemType::G3D_StructureModel));
	SetItemState(rootOfStructureModel_, ItemState_Checked_);
	rootOfGridModel_ = AppendItem(rootItem, Strings::NameOfGridModel());
	SetItemData(rootOfGridModel_, new G3DTreeItemData(g3dVoxelGrid_.get(), G3DTreeItemData::ItemType::G3D_VoxelGrid));
	SetItemState(rootOfGridModel_, ItemState_Checked_);

	renderer_ = vtkSmartPointer<vtkRenderer>::New();
	transform_ = vtkSmartPointer<vtkTransform>::New();
	transform_->Identity();

	Bind(wxEVT_TREE_STATE_IMAGE_CLICK, &ProjectTreeCtrl::OnStateImageClicked, this);
	Bind(wxEVT_TREE_SEL_CHANGED, &ProjectTreeCtrl::OnItemSelected, this);
	Bind(wxEVT_TREE_ITEM_MENU, &ProjectTreeCtrl::OnItemMenu, this);
	Bind(wxEVT_COMMAND_MENU_SELECTED, &ProjectTreeCtrl::OnStructureModelGridding, this, Events::ID::Menu_StructureModelGridding, Events::ID::Menu_StructureModelGridding);
}

ProjectTreeCtrl::~ProjectTreeCtrl() {
	
}

void ProjectTreeCtrl::UpdateSubTreeOfMap(geo3dml::Map* g3dMap) {
	wxTreeItemId mapItem = FindOrInsertMapItem(g3dMap);
	int numberOfLayers = g3dMap->GetLayerCount();
	for (int i = 0; i < numberOfLayers; ++i) {
		geo3dml::Layer* layer = g3dMap->GetLayerAt(i);
		wxTreeItemIdValue cookieOfLayer = nullptr;
		wxTreeItemId layerItem = GetFirstChild(mapItem, cookieOfLayer);
		while (layerItem.IsOk()) {
			G3DTreeItemData* itemData = dynamic_cast<G3DTreeItemData*>(GetItemData(layerItem));
			if (itemData != nullptr && itemData->GetItemType() == G3DTreeItemData::ItemType::G3D_Layer) {
				geo3dml::Layer* layerInItem = static_cast<geo3dml::Layer*>(itemData->GetG3DObject());
				if (layerInItem == layer) {
					break;
				}
			}
			layerItem = GetNextChild(mapItem, cookieOfLayer);
		}
		if (layerItem.IsOk()) {
			continue;
		}
		layerItem = AppendItem(mapItem, wxString::FromUTF8(layer->GetName()));
		SetItemData(layerItem, new G3DTreeItemData(layer, G3DTreeItemData::ItemType::G3D_Layer));
		SetItemState(layerItem, ItemState_Checked_);
		int numberOfActors = layer->GetActorCount();
		for (int m = 0; m < numberOfActors; ++m) {
			g3dvtk::Actor* g3dActor = dynamic_cast<g3dvtk::Actor*>(layer->GetActorAt(m));
			if (g3dActor != nullptr) {
				g3dActor->SetUserTransform(transform_);
				renderer_->AddActor(g3dActor->GetVTKProp());
				wxTreeItemId actorItem = AppendItem(layerItem, wxString::FromUTF8(g3dActor->GetName()));
				SetItemData(actorItem, new G3DTreeItemData(g3dActor, G3DTreeItemData::ItemType::G3D_Actor));
				SetItemState(actorItem, g3dActor->IsVisible() ? ItemState_Checked_ : ItemState_Unchecked_);
			}
		}
	}
	RefreshItem(mapItem);
}

wxTreeItemId ProjectTreeCtrl::FindOrInsertMapItem(geo3dml::Map* g3dMap) {
	wxTreeItemIdValue cookie = nullptr;
	wxTreeItemId mapItem = GetFirstChild(rootOfStructureModel_, cookie);
	while (mapItem.IsOk()) {
		G3DTreeItemData* itemData = dynamic_cast<G3DTreeItemData*>(GetItemData(mapItem));
		if (itemData != nullptr && itemData->GetItemType() == G3DTreeItemData::ItemType::G3D_Map) {
			geo3dml::Map* mapInItem = static_cast<geo3dml::Map*>(itemData->GetG3DObject());
			if (mapInItem == g3dMap) {
				return mapItem;
			}
		}
		mapItem = GetNextChild(rootOfStructureModel_, cookie);
	}
	mapItem = AppendItem(rootOfStructureModel_, wxString::FromUTF8(g3dMap->GetName()));
	SetItemData(mapItem, new G3DTreeItemData(g3dMap, G3DTreeItemData::ItemType::G3D_Map));
	SetItemState(mapItem, ItemState_Checked_);
	RefreshItem(rootOfStructureModel_);
	return mapItem;
}

void ProjectTreeCtrl::OnStateImageClicked(wxTreeEvent& event) {
	wxBusyCursor waiting;
	wxTreeItemId item = event.GetItem();
	int state = GetItemState(item);
	if (state == ProjectTreeCtrl::ItemState_Checked_) {
		CheckItem(item, ProjectTreeCtrl::ItemState_Unchecked_);
	} else {
		CheckItem(item, ProjectTreeCtrl::ItemState_Checked_);
	}
	Events::Notify(Events::ID::Notify_RefreshRenderWindow);
}

void ProjectTreeCtrl::CheckItem(wxTreeItemId item, int checkState) {
	SetItemState(item, checkState);
	G3DTreeItemData* itemData = static_cast<G3DTreeItemData*>(GetItemData(item));
	if (itemData->GetItemType() == G3DTreeItemData::ItemType::G3D_Actor) {
		geo3dml::Actor* g3dActor = static_cast<geo3dml::Actor*>(itemData->GetG3DObject());
		g3dActor->SetVisible(checkState == ProjectTreeCtrl::ItemState_Checked_);
	}
	// check children
	wxTreeItemIdValue cookie = nullptr;
	wxTreeItemId subItem = GetFirstChild(item, cookie);
	while (subItem.IsOk()) {
		CheckItem(subItem, checkState);
		subItem = GetNextChild(item, cookie);
	}
}

void ProjectTreeCtrl::AppendG3DModel(geo3dml::Model* model, bool appendToDefaultMap) {
	g3dProject_->AddModel(model);
	if (!appendToDefaultMap) {
		return;
	}

	geo3dml::Map* g3dMap = GetDefaultMap();
	int numOfFeatureClasses = model->GetFeatureClassCount();
	for (int i = 0; i < numOfFeatureClasses; ++i) {
		AppendFeatureClassToMap(model->GetFeatureClassAt(i), g3dMap);
	}
	UpdateSubTreeOfMap(g3dMap);
}

void ProjectTreeCtrl::AppendG3DMap(geo3dml::Map* map) {
	g3dProject_->AddMap(map);
	UpdateSubTreeOfMap(map);
}

void ProjectTreeCtrl::UpdateVoxelGridModel() {
	SetItemText(rootOfGridModel_, wxString::FromUTF8(g3dVoxelGrid_->GetName()));
	int maxLOD = g3dVoxelGrid_->GetMaxLOD();
	for (int level = 0; level <= maxLOD; ++level) {
		g3dgrid::LOD* lod = g3dVoxelGrid_->GetLOD(level);
		wxTreeItemIdValue cookieOfLOD = nullptr;
		wxTreeItemId lodItem = GetFirstChild(rootOfGridModel_, cookieOfLOD);
		while (lodItem.IsOk()) {
			G3DTreeItemData* itemData = dynamic_cast<G3DTreeItemData*>(GetItemData(lodItem));
			if (itemData != nullptr && itemData->GetItemType() == G3DTreeItemData::ItemType::G3D_GridLOD) {
				g3dgrid::LOD* lodInItem = static_cast<g3dgrid::LOD*>(itemData->GetG3DObject());
				if (lodInItem == lod) {
					break;
				}
			}
			lodItem = GetNextChild(rootOfGridModel_, cookieOfLOD);
		}
		if (lodItem.IsOk()) {
			continue;
		}
		lodItem = AppendItem(rootOfGridModel_, wxString::Format(wxS("LOD - %d"), level));
		SetItemData(lodItem, new G3DTreeItemData(lod, G3DTreeItemData::ItemType::G3D_GridLOD));
		SetItemState(lodItem, ItemState_Checked_);
		/*
		int numberOfActors = layer->GetActorCount();
		for (int m = 0; m < numberOfActors; ++m) {
			g3dvtk::Actor* g3dActor = dynamic_cast<g3dvtk::Actor*>(layer->GetActorAt(m));
			if (g3dActor != nullptr) {
				g3dActor->SetUserTransform(transform_);
				renderer_->AddActor(g3dActor->GetVTKProp());
				wxTreeItemId actorItem = AppendItem(layerItem, wxString::FromUTF8(g3dActor->GetName()));
				SetItemData(actorItem, new G3DTreeItemData(g3dActor, G3DTreeItemData::ItemType::G3D_Actor));
				SetItemState(actorItem, g3dActor->IsVisible() ? ItemState_Checked_ : ItemState_Unchecked_);
			}
		}
		*/
	}
	RefreshItem(rootOfGridModel_);
}

void ProjectTreeCtrl::ExpandStructureModelTree() {
	Expand(rootOfStructureModel_);
}

void ProjectTreeCtrl::ExpandGridModelNodeTree() {
	Expand(rootOfGridModel_);
}

geo3dml::Map* ProjectTreeCtrl::GetDefaultMap() {
	if (g3dProject_->GetMapCount() > 0) {
		return g3dProject_->GetMapAt(0);
	} else {
		g3dvtk::ObjectFactory g3dFactory;
		geo3dml::Map* map = g3dFactory.NewMap();
		map->SetID(geo3dml::Object::NewID());
		map->SetName(Strings::NameOfDefaultG3DMap().ToUTF8().data());
		g3dProject_->AddMap(map);
		return map;
	}
}

void ProjectTreeCtrl::AppendFeatureClassToMap(geo3dml::FeatureClass* g3dFC, geo3dml::Map* g3dMap) {
	g3dvtk::ObjectFactory g3dFactory;
	geo3dml::Layer* layer = g3dFactory.NewLayer();
	layer->SetName(g3dFC->GetName());
	layer->BindFeatureClass(g3dFC);
	layer->BuildActorsFromFeatures(&g3dFactory);
	g3dMap->AddLayer(layer);
}

int ProjectTreeCtrl::CountVisibleLayersOfStructureModel() {
	int count = 0;
	wxTreeItemIdValue mapCookie = nullptr;
	wxTreeItemId mapItem = GetFirstChild(rootOfStructureModel_, mapCookie);
	while (mapItem.IsOk()) {
		wxTreeItemIdValue layerCookie = nullptr;
		wxTreeItemId layerItem = GetFirstChild(mapItem, layerCookie);
		while (layerItem.IsOk()) {
			int state = GetItemState(layerItem);
			if (state == ItemState_Checked_) {
				++count;
			}
			layerItem = GetNextChild(mapItem, layerCookie);
		}
		mapItem = GetNextChild(rootOfStructureModel_, mapCookie);
	}
	return count;
}

void ProjectTreeCtrl::OnItemSelected(wxTreeEvent& event) {
	wxNotifyEvent evt;
	evt.SetId(Events::ID::Notify_ProjectTreeItemSelected);
	G3DTreeItemData* itemData = nullptr;
	wxTreeItemId itemId = event.GetItem();
	if (itemId.IsOk()) {
		itemData = static_cast<G3DTreeItemData*>(GetItemData(itemId));
	}
	if (itemData != nullptr) {
		evt.SetClientData(itemData->GetG3DObject());
		evt.SetInt(itemData->GetItemType());
	} else {
		evt.SetClientData(nullptr);
		evt.SetInt(G3DTreeItemData::ItemType::Unknown);
	}
	wxPostEvent(GetParent(), evt);
}

wxTreeItemId ProjectTreeCtrl::FindOrInsertVoxelGridItem(g3dgrid::VoxelGrid* g3dVoxelGrid) {
	wxTreeItemIdValue cookie = nullptr;
	wxTreeItemId gridItem = GetFirstChild(rootOfGridModel_, cookie);
	while (gridItem.IsOk()) {
		G3DTreeItemData* itemData = dynamic_cast<G3DTreeItemData*>(GetItemData(gridItem));
		if (itemData != nullptr && itemData->GetItemType() == G3DTreeItemData::ItemType::G3D_VoxelGrid) {
			g3dgrid::VoxelGrid* gridInItem = static_cast<g3dgrid::VoxelGrid*>(itemData->GetG3DObject());
			if (gridInItem == g3dVoxelGrid) {
				return gridItem;
			}
		}
		gridItem = GetNextChild(rootOfGridModel_, cookie);
	}
	gridItem = AppendItem(rootOfGridModel_, wxString::FromUTF8(g3dVoxelGrid->GetName()));
	SetItemData(gridItem, new G3DTreeItemData(g3dVoxelGrid, G3DTreeItemData::ItemType::G3D_VoxelGrid));
	SetItemState(gridItem, ItemState_Checked_);
	RefreshItem(rootOfGridModel_);
	return gridItem;
}

void ProjectTreeCtrl::OnItemMenu(wxTreeEvent& event) {
	event.Skip();
	wxTreeItemId itemId = event.GetItem();
	SelectItem(itemId);
	G3DTreeItemData* gItem = static_cast<G3DTreeItemData*>(GetItemData(itemId));
	switch (gItem->GetItemType()) {
	case G3DTreeItemData::ItemType::G3D_StructureModel:
		ShowMenuOnStructureModelItem(gItem, event.GetPoint());
		break;
	case G3DTreeItemData::ItemType::G3D_VoxelGrid:
		ShowMenuOnVoxelGridItem(gItem, event.GetPoint());
		break;
	case G3DTreeItemData::ItemType::G3D_Actor:
		ShowMenuOnActorItem(gItem, event.GetPoint());
		break;
	default:
		return;
	}
}

void ProjectTreeCtrl::ShowMenuOnStructureModelItem(G3DTreeItemData* itemData, const wxPoint& pos) {
	wxMenu menu(GetItemText(itemData->GetId()));
	menu.Append(Events::ID::Menu_OpenGeo3DML, Strings::TitleOfMenuItemOpenGeo3DML());
	menu.Append(Events::ID::Menu_OpenSimpleDrillLog, Strings::TitleOfMenuItemOpenSimpleDrillLog());
	menu.AppendSeparator();
	wxMenuItem* item = menu.Append(Events::ID::Menu_StructureModelGridding, Strings::TitleOfMenuItemStructureModelGridding());
	item->Enable(CountVisibleLayersOfStructureModel() > 0);
	menu.AppendSeparator();
	menu.Append(Events::ID::Menu_SaveToGeo3DML, Strings::TitleOfMenuItemSaveToGeo3DML());
	menu.Append(Events::ID::Menu_CloseStructureModel, Strings::TitleOfMenuItemCloseStructureModel());
	PopupMenu(&menu, pos);
}

void ProjectTreeCtrl::ShowMenuOnActorItem(G3DTreeItemData* itemData, const wxPoint& pos) {

}

void ProjectTreeCtrl::ShowMenuOnVoxelGridItem(G3DTreeItemData* itemData, const wxPoint& pos) {
	wxMenu menu(GetItemText(itemData->GetId()));
	menu.Append(Events::ID::Menu_EditVoxelGrid, Strings::TitleOfMenuItemEditVoxelGrid());
	menu.AppendSeparator();
	menu.Append(Events::ID::Menu_SaveToVoxelGrid, Strings::TitleOfMenuItemSaveToVoxelGrid());
	menu.Append(Events::ID::Menu_CloseVoxelGridModel, Strings::TitleOfMenuItemCloseVoxelGridModel());
	PopupMenu(&menu, pos);
}

void ProjectTreeCtrl::CloseAllModels() {
	// block event handler and remove nodes
	Unbind(wxEVT_TREE_SEL_CHANGED, &ProjectTreeCtrl::OnItemSelected, this);
	RemoveChildrenFromScene(rootOfStructureModel_);
	DeleteChildren(rootOfStructureModel_);
	RemoveChildrenFromScene(rootOfGridModel_);
	DeleteChildren(rootOfGridModel_);
	// reset structure and grid model
	ResetStructureModel();
	ResetVoxelGridModel();
	// force to trigger the selection changed event to make sure the dashboard know the obsolete tree item.
	wxTreeEvent evtSelection(wxEVT_TREE_SEL_CHANGED, this, GetSelection());
	OnItemSelected(evtSelection);
	// restore event handler
	Bind(wxEVT_TREE_SEL_CHANGED, &ProjectTreeCtrl::OnItemSelected, this);
}

void ProjectTreeCtrl::CloseStructureModel() {
	// block event handler and remove nodes
	Unbind(wxEVT_TREE_SEL_CHANGED, &ProjectTreeCtrl::OnItemSelected, this);
	RemoveChildrenFromScene(rootOfStructureModel_);
	DeleteChildren(rootOfStructureModel_);
	// reset structure models
	ResetStructureModel();
	// force to trigger the selection changed event to make sure the dashboard know the obsolete tree item.
	wxTreeEvent evtSelection(wxEVT_TREE_SEL_CHANGED, this, GetSelection());
	OnItemSelected(evtSelection);
	// restore event handler
	Bind(wxEVT_TREE_SEL_CHANGED, &ProjectTreeCtrl::OnItemSelected, this);
}

void ProjectTreeCtrl::CloseVoxelGridModel() {
	// block event handler and remove nodes
	Unbind(wxEVT_TREE_SEL_CHANGED, &ProjectTreeCtrl::OnItemSelected, this);
	RemoveChildrenFromScene(rootOfGridModel_);
	DeleteChildren(rootOfGridModel_);
	// reset grid models
	ResetVoxelGridModel();
	// force to trigger the selection changed event to make sure the dashboard know the obsolete tree item.
	wxTreeEvent evtSelection(wxEVT_TREE_SEL_CHANGED, this, GetSelection());
	OnItemSelected(evtSelection);
	// restore event handler
	Bind(wxEVT_TREE_SEL_CHANGED, &ProjectTreeCtrl::OnItemSelected, this);
}

void ProjectTreeCtrl::RemoveChildrenFromScene(const wxTreeItemId& item) {
	wxTreeItemIdValue cookie = nullptr;
	wxTreeItemId child = GetFirstChild(item, cookie);
	while (child.IsOk()) {
		G3DTreeItemData* itemData = dynamic_cast<G3DTreeItemData*>(GetItemData(child));
		if (itemData != nullptr && itemData->GetItemType() == G3DTreeItemData::ItemType::G3D_Actor) {
			g3dvtk::Actor* g3dActor = static_cast<g3dvtk::Actor*>(itemData->GetG3DObject());
			renderer_->RemoveActor(g3dActor->GetVTKProp());
		}
		RemoveChildrenFromScene(child);
		child = GetNextChild(item, cookie);
	}
}

void ProjectTreeCtrl::ResetStructureModel() {
	g3dvtk::ObjectFactory g3dFactory;
	g3dProject_.reset(g3dFactory.NewProject());
	g3dProject_->SetID(geo3dml::Object::NewID());
	g3dProject_->SetName(Strings::NameOfStructureModel().ToUTF8().data());
	wxTreeItemData* oldData = GetItemData(rootOfStructureModel_);
	SetItemData(rootOfStructureModel_, new G3DTreeItemData(g3dProject_.get(), G3DTreeItemData::ItemType::G3D_StructureModel));
	SetItemText(rootOfStructureModel_, wxString::FromUTF8(g3dProject_->GetName()));
	delete oldData;
}

void ProjectTreeCtrl::ResetVoxelGridModel() {
	g3dVoxelGrid_ = std::make_unique<g3dgrid::VoxelGrid>();
	g3dVoxelGrid_->SetID(geo3dml::Object::NewID());
	g3dVoxelGrid_->SetName(Strings::NameOfGridModel().ToUTF8().data());
	g3dVoxelGrid_->SetSRS("CGCS2000");
	g3dVoxelGrid_->SetDescription(Strings::NameOfVoxelGrid().ToUTF8().data());
	wxTreeItemData* oldData = GetItemData(rootOfGridModel_);
	SetItemData(rootOfGridModel_, new G3DTreeItemData(g3dVoxelGrid_.get(), G3DTreeItemData::ItemType::G3D_VoxelGrid));
	SetItemText(rootOfGridModel_, wxString::FromUTF8(g3dVoxelGrid_->GetName()));
	delete oldData;
}

void ProjectTreeCtrl::OnStructureModelGridding(wxCommandEvent& event) {
	DlgStructureModelGridding dlg(this);
	wxTreeItemIdValue mapCookie = nullptr;
	wxTreeItemId mapItem = GetFirstChild(rootOfStructureModel_, mapCookie);
	while (mapItem.IsOk()) {
		wxTreeItemIdValue layerCookie = nullptr;
		wxTreeItemId layerItem = GetFirstChild(mapItem, layerCookie);
		while (layerItem.IsOk()) {
			if (GetItemState(layerItem) == ItemState_Checked_) {
				G3DTreeItemData* itemData = static_cast<G3DTreeItemData*>(GetItemData(layerItem));
				geo3dml::Layer* g3dLayer = static_cast<geo3dml::Layer*>(itemData->GetG3DObject());
				geo3dml::FeatureClass* g3dFeatureClass = g3dLayer->GetBindingFeatureClass();
				dlg.AppendFeatureClassFromStructureModel(g3dFeatureClass);
			}
			layerItem = GetNextChild(mapItem, layerCookie);
		}
		mapItem = GetNextChild(rootOfStructureModel_, mapCookie);
	}
	dlg.SetLocalGridModel(g3dVoxelGrid_.get());
	dlg.CenterOnScreen();
	dlg.ShowModal();
}