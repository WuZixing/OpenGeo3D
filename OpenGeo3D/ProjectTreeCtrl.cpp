#include "ProjectTreeCtrl.h"
#include <g3dvtk/Actor.h>
#include <g3dvtk/ObjectFactory.h>
#include "checked_box.xpm"
#include "unchecked_box.xpm"
#include "DlgEditVoxelGrid.h"
#include "Events.h"
#include "Strings.h"

ProjectTreeCtrl::ProjectTreeCtrl(wxWindow* parent, const wxSize& size) : 
	wxTreeCtrl(parent, wxID_ANY, wxDefaultPosition, size, wxTR_DEFAULT_STYLE | wxNO_BORDER | wxTR_HIDE_ROOT | wxTR_FULL_ROW_HIGHLIGHT), 
	ItemState_Checked_(0), ItemState_Unchecked_(1) {

	g3dvtk::ObjectFactory g3dFactory;
	g3dProject_.reset(g3dFactory.NewProject());
	g3dProject_->SetID(geo3dml::Object::NewID());
	g3dProject_->SetName(Strings::NameOfDefaultG3DProject().ToUTF8().data());
	g3dGrids_ = std::make_unique<g3dgrid::GridCollection>();
	g3dGrids_->SetID(geo3dml::Object::NewID());
	g3dGrids_->SetName(Strings::NameOfGridModel().ToUTF8().data());

	wxIcon icons[2];
	icons[ItemState_Checked_] = wxIcon(xpm_checked_box);
	icons[ItemState_Unchecked_] = wxIcon(xpm_unchecked_box);
	wxImageList* states = new wxImageList(icons[0].GetWidth(), icons[0].GetHeight(), true, 2);
	states->Add(icons[0]);
	states->Add(icons[1]);
	AssignStateImageList(states);

	wxTreeItemId rootItem = AddRoot(wxEmptyString, 0);
	rootStructureModel_ = AppendItem(rootItem, Strings::NameOfStructureModel());
	SetItemData(rootStructureModel_, new G3DTreeItemData(g3dProject_.get(), G3DTreeItemData::ItemType::G3D_StructureModel));
	SetItemState(rootStructureModel_, ItemState_Checked_);
	rootOfGridModel_ = AppendItem(rootItem, Strings::NameOfGridModel());
	SetItemData(rootOfGridModel_, new G3DTreeItemData(g3dGrids_.get(), G3DTreeItemData::ItemType::G3D_GridModel));
	SetItemState(rootOfGridModel_, ItemState_Checked_);

	renderer_ = vtkSmartPointer<vtkRenderer>::New();
	transform_ = vtkSmartPointer<vtkTransform>::New();
	transform_->Identity();

	Bind(wxEVT_TREE_STATE_IMAGE_CLICK, &ProjectTreeCtrl::OnStateImageClicked, this);
	Bind(wxEVT_TREE_SEL_CHANGED, &ProjectTreeCtrl::OnItemSelected, this);
	Bind(wxEVT_TREE_ITEM_MENU, &ProjectTreeCtrl::OnItemMenu, this);
	Bind(wxEVT_COMMAND_MENU_SELECTED, &ProjectTreeCtrl::OnEditVoxelGrid, this, Events::ID::Menu_EditVoxelGrid, Events::ID::Menu_EditVoxelGrid);
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
	wxTreeItemId mapItem = GetFirstChild(rootStructureModel_, cookie);
	while (mapItem.IsOk()) {
		G3DTreeItemData* itemData = dynamic_cast<G3DTreeItemData*>(GetItemData(mapItem));
		if (itemData != nullptr && itemData->GetItemType() == G3DTreeItemData::ItemType::G3D_Map) {
			geo3dml::Map* mapInItem = static_cast<geo3dml::Map*>(itemData->GetG3DObject());
			if (mapInItem == g3dMap) {
				return mapItem;
			}
		}
		mapItem = GetNextChild(rootStructureModel_, cookie);
	}
	mapItem = AppendItem(rootStructureModel_, wxString::FromUTF8(g3dMap->GetName()));
	SetItemData(mapItem, new G3DTreeItemData(g3dMap, G3DTreeItemData::ItemType::G3D_Map));
	SetItemState(mapItem, ItemState_Checked_);
	RefreshItem(rootStructureModel_);
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

void ProjectTreeCtrl::AppendG3DGrid(g3dgrid::Grid* grid) {
	g3dGrids_->AddGrid(grid);
	UpdateSubTreeOfGrid(grid);
}

void ProjectTreeCtrl::ExpandStructureModelTree() {
	Expand(rootStructureModel_);
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

void ProjectTreeCtrl::UpdateSubTreeOfGrid(g3dgrid::Grid* g3dGrid) {
	g3dgrid::VoxelGrid* voxelGrid = dynamic_cast<g3dgrid::VoxelGrid*>(g3dGrid);
	if (voxelGrid == nullptr) {
		return;
	}
	wxTreeItemId gridItem = FindOrInsertVoxelGridItem(voxelGrid);
	RefreshItem(gridItem);
}

void ProjectTreeCtrl::OnItemMenu(wxTreeEvent& event) {
	wxTreeItemId itemId = event.GetItem();
	SelectItem(itemId);
	G3DTreeItemData* gItem = static_cast<G3DTreeItemData*>(GetItemData(itemId));
	switch (gItem->GetItemType()) {
	case G3DTreeItemData::ItemType::G3D_StructureModel:
		ShowMenuOnStructureModelItem(gItem, event.GetPoint());
		break;
	case G3DTreeItemData::ItemType::G3D_GridModel:
		ShowMenuOnGridModelItem(gItem, event.GetPoint());
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
	event.Skip();
}

void ProjectTreeCtrl::ShowMenuOnStructureModelItem(G3DTreeItemData* itemData, const wxPoint& pos) {
	wxMenu menu(GetItemText(itemData->GetId()));
	menu.Append(Events::ID::Menu_OpenGeo3DML, Strings::TitleOfMenuItemOpenGeo3DML());
	menu.Append(Events::ID::Menu_OpenSimpleDrillLog, Strings::TitleOfMenuItemOpenSimpleDrillLog());
	menu.AppendSeparator();
	menu.Append(Events::ID::Menu_SaveToGeo3DML, Strings::TitleOfMenuItemSaveToGeo3DML());
	menu.Append(Events::ID::Menu_CloseStructureModels, Strings::TitleOfMenuItemCloseStructureModels());
	PopupMenu(&menu, pos);
}

void ProjectTreeCtrl::ShowMenuOnGridModelItem(G3DTreeItemData* itemData, const wxPoint& pos) {
	wxMenu menu(GetItemText(itemData->GetId()));
	menu.Append(Events::ID::Menu_NewGridModel, Strings::TitleOfMenuItemNewGridModel());
	menu.AppendSeparator();
	menu.Append(Events::ID::Menu_CloseGridModels, Strings::TitleOfMenuItemCloseGridModels());
	PopupMenu(&menu, pos);
}

void ProjectTreeCtrl::ShowMenuOnActorItem(G3DTreeItemData* itemData, const wxPoint& pos) {

}

void ProjectTreeCtrl::ShowMenuOnVoxelGridItem(G3DTreeItemData* itemData, const wxPoint& pos) {
	wxMenu menu(GetItemText(itemData->GetId()));
	menu.Append(Events::ID::Menu_EditVoxelGrid, Strings::TitleOfMenuItemEditVoxelGrid());
	menu.AppendSeparator();
	menu.Append(Events::ID::Menu_SaveToVoxelGrid, Strings::TitleOfMenuItemSaveToVoxelGrid());
	PopupMenu(&menu, pos);
}

void ProjectTreeCtrl::OnEditVoxelGrid(wxCommandEvent& event) {
	wxTreeItemId item = GetSelection();
	if (!item.IsOk()) {
		return;
	}
	G3DTreeItemData* itemData = static_cast<G3DTreeItemData*>(GetItemData(item));
	if (itemData->GetItemType() != G3DTreeItemData::ItemType::G3D_VoxelGrid) {
		return;
	}
	g3dgrid::VoxelGrid* voxelGrid = static_cast<g3dgrid::VoxelGrid*>(itemData->GetG3DObject());
	DlgEditVoxelGrid dlg(this, voxelGrid);
	dlg.CenterOnScreen();
	dlg.ShowModal();
}

void ProjectTreeCtrl::CloseAllModels() {
	// block event handler and remove nodes
	Unbind(wxEVT_TREE_SEL_CHANGED, &ProjectTreeCtrl::OnItemSelected, this);
	RemoveChildrenFromScene(rootStructureModel_);
	DeleteChildren(rootStructureModel_);
	RemoveChildrenFromScene(rootOfGridModel_);
	DeleteChildren(rootOfGridModel_);
	// reset structure models
	g3dvtk::ObjectFactory g3dFactory;
	g3dProject_.reset(g3dFactory.NewProject());
	g3dProject_->SetID(geo3dml::Object::NewID());
	g3dProject_->SetName(Strings::NameOfDefaultG3DProject().ToUTF8().data());
	wxTreeItemData* oldData = GetItemData(rootStructureModel_);
	SetItemData(rootStructureModel_, new G3DTreeItemData(g3dProject_.get(), G3DTreeItemData::ItemType::G3D_StructureModel));
	delete oldData;
	g3dGrids_ = std::make_unique<g3dgrid::GridCollection>();
	g3dGrids_->SetID(geo3dml::Object::NewID());
	g3dGrids_->SetName(Strings::NameOfGridModel().ToUTF8().data());
	oldData = GetItemData(rootOfGridModel_);
	SetItemData(rootOfGridModel_, new G3DTreeItemData(g3dGrids_.get(), G3DTreeItemData::ItemType::G3D_GridModel));
	delete oldData;
	// force to trigger the selection changed event to make sure the dashboard know the obsolete tree item.
	wxTreeEvent evtSelection(wxEVT_TREE_SEL_CHANGED, this, GetSelection());
	OnItemSelected(evtSelection);
	// restore event handler
	Bind(wxEVT_TREE_SEL_CHANGED, &ProjectTreeCtrl::OnItemSelected, this);
}

void ProjectTreeCtrl::CloseStructureModels() {
	// block event handler and remove nodes
	Unbind(wxEVT_TREE_SEL_CHANGED, &ProjectTreeCtrl::OnItemSelected, this);
	RemoveChildrenFromScene(rootStructureModel_);
	DeleteChildren(rootStructureModel_);
	// reset structure models
	g3dvtk::ObjectFactory g3dFactory;
	g3dProject_.reset(g3dFactory.NewProject());
	g3dProject_->SetID(geo3dml::Object::NewID());
	g3dProject_->SetName(Strings::NameOfDefaultG3DProject().ToUTF8().data());
	wxTreeItemData* oldData = GetItemData(rootStructureModel_);
	SetItemData(rootStructureModel_, new G3DTreeItemData(g3dProject_.get(), G3DTreeItemData::ItemType::G3D_StructureModel));
	delete oldData;
	// force to trigger the selection changed event to make sure the dashboard know the obsolete tree item.
	wxTreeEvent evtSelection(wxEVT_TREE_SEL_CHANGED, this, GetSelection());
	OnItemSelected(evtSelection);
	// restore event handler
	Bind(wxEVT_TREE_SEL_CHANGED, &ProjectTreeCtrl::OnItemSelected, this);
}

void ProjectTreeCtrl::CloseGridModels() {
	// block event handler and remove nodes
	Unbind(wxEVT_TREE_SEL_CHANGED, &ProjectTreeCtrl::OnItemSelected, this);
	RemoveChildrenFromScene(rootOfGridModel_);
	DeleteChildren(rootOfGridModel_);
	// reset grid models
	g3dvtk::ObjectFactory g3dFactory;
	g3dGrids_ = std::make_unique<g3dgrid::GridCollection>();
	g3dGrids_->SetID(geo3dml::Object::NewID());
	g3dGrids_->SetName(Strings::NameOfGridModel().ToUTF8().data());
	wxTreeItemData* oldData = GetItemData(rootOfGridModel_);
	SetItemData(rootOfGridModel_, new G3DTreeItemData(g3dGrids_.get(), G3DTreeItemData::ItemType::G3D_GridModel));
	delete oldData;
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