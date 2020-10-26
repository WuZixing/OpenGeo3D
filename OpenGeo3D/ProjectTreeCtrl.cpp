#include "ProjectTreeCtrl.h"
#include <g3dvtk/Actor.h>
#include "Events.h"
#include "G3DTreeItemData.h"
#include "Strings.h"
#include "checked_box.xpm"
#include "unchecked_box.xpm"

ProjectTreeCtrl::ProjectTreeCtrl(wxWindow* parent, const wxSize& size) : 
	wxTreeCtrl(parent, wxID_ANY, wxDefaultPosition, size, wxTR_DEFAULT_STYLE | wxNO_BORDER | wxTR_HIDE_ROOT | wxTR_FULL_ROW_HIGHLIGHT), 
	ItemState_Checked_(0), ItemState_Unchecked_(1) {

	wxIcon icons[2];
	icons[ItemState_Checked_] = wxIcon(xpm_checked_box);
	icons[ItemState_Unchecked_] = wxIcon(xpm_unchecked_box);
	wxImageList* states = new wxImageList(icons[0].GetWidth(), icons[0].GetHeight(), true, 2);
	states->Add(icons[0]);
	states->Add(icons[1]);
	AssignStateImageList(states);

	wxTreeItemId rootItem = AddRoot(wxEmptyString, 0);
	rootStructureModel_ = AppendItem(rootItem, Strings::NameOfStructureModel());
	SetItemData(rootStructureModel_, new G3DTreeItemData(NULL, G3DTreeItemData::ItemType::G3D_StructureModel));
	SetItemState(rootStructureModel_, ItemState_Checked_);
	rootOfGridMode_ = AppendItem(rootItem, Strings::NameOfGridModel());
	SetItemData(rootOfGridMode_, new G3DTreeItemData(NULL, G3DTreeItemData::ItemType::G3D_GridModel));
	SetItemState(rootOfGridMode_, ItemState_Checked_);

	renderer_ = vtkSmartPointer<vtkRenderer>::New();
	transform_ = vtkSmartPointer<vtkTransform>::New();
	transform_->Identity();

	Bind(wxEVT_TREE_STATE_IMAGE_CLICK, &ProjectTreeCtrl::OnStateImageClicked, this);
}

ProjectTreeCtrl::~ProjectTreeCtrl() {

}

void ProjectTreeCtrl::UpdateSubTreeOfMap(geo3dml::Map* g3dMap) {
	wxTreeItemId mapItem = FindOrInsertMapItem(g3dMap);
	int numberOfLayers = g3dMap->GetLayerCount();
	for (int i = 0; i < numberOfLayers; ++i) {
		geo3dml::Layer* layer = g3dMap->GetLayerAt(i);
		wxTreeItemIdValue cookieOfLayer = NULL;
		wxTreeItemId layerItem = GetFirstChild(mapItem, cookieOfLayer);
		while (layerItem.IsOk()) {
			G3DTreeItemData* itemData = dynamic_cast<G3DTreeItemData*>(GetItemData(layerItem));
			if (itemData != NULL && itemData->GetItemType() == G3DTreeItemData::ItemType::G3D_Layer) {
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
			if (g3dActor != NULL) {
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
	wxTreeItemIdValue cookie = NULL;
	wxTreeItemId mapItem = GetFirstChild(rootStructureModel_, cookie);
	while (mapItem.IsOk()) {
		G3DTreeItemData* itemData = dynamic_cast<G3DTreeItemData*>(GetItemData(mapItem));
		if (itemData != NULL && itemData->GetItemType() == G3DTreeItemData::ItemType::G3D_Map) {
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
	wxTreeItemIdValue cookie = NULL;
	wxTreeItemId subItem = GetFirstChild(item, cookie);
	while (subItem.IsOk()) {
		CheckItem(subItem, checkState);
		subItem = GetNextChild(item, cookie);
	}
}
