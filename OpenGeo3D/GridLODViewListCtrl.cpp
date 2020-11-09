#include "GridLODViewListCtrl.h"
#include "DlgNewGridLOD.h"
#include "Events.h"
#include "Strings.h"

GridLODViewListCtrl::GridLODViewListCtrl(wxWindow* parent, g3dgrid::VoxelGrid* voxelGrid) : wxDataViewListCtrl(parent, wxID_ANY), voxelGrid_(voxelGrid){
	AppendTextColumn(Strings::LabelOfGridLOD(), wxDATAVIEW_CELL_INERT, 35);
	AppendTextColumn(Strings::LabelOfCellSizeX(), wxDATAVIEW_CELL_INERT, 70);
	AppendTextColumn(Strings::LabelOfCellSizeY(), wxDATAVIEW_CELL_INERT, 70);
	AppendTextColumn(Strings::LabelOfCellSizeZ(), wxDATAVIEW_CELL_INERT, 70);
	AppendTextColumn(Strings::LabelOfCellScaleX(), wxDATAVIEW_CELL_INERT, 70);
	AppendTextColumn(Strings::LabelOfCellScaleY(), wxDATAVIEW_CELL_INERT, 70);
	AppendTextColumn(Strings::LabelOfCellScaleZ(), wxDATAVIEW_CELL_INERT, 70);
	AppendTextColumn(Strings::LabelOfCellCount(), wxDATAVIEW_CELL_INERT, 60);

	int maxLevel = voxelGrid_->GetMaxLOD();
	for (int level = 0; level <= maxLevel; ++level) {
		g3dgrid::LOD* lod = voxelGrid_->GetLOD(level);
		if (lod != nullptr) {
			AppendLODItem(*lod);
		}
	}

	Bind(wxEVT_DATAVIEW_ITEM_CONTEXT_MENU, &GridLODViewListCtrl::OnItemContextMenu, this);
	Bind(wxEVT_MENU, &GridLODViewListCtrl::OnAppendLOD, this, Events::ID::Menu_AppendGridLOD, Events::ID::Menu_AppendGridLOD);
	Bind(wxEVT_MENU, &GridLODViewListCtrl::OnDeleteLOD, this, Events::ID::Menu_DeleteGridLOD, Events::ID::Menu_DeleteGridLOD);
}

GridLODViewListCtrl::~GridLODViewListCtrl() {

}

void GridLODViewListCtrl::OnItemContextMenu(wxDataViewEvent& event) {
	if (voxelGrid_ == nullptr) {
		return;
	}

	wxMenu ctxMenu;
	wxDataViewItem item = event.GetItem();
	if (item.IsOk()) {
		int row = ItemToRow(item);
		if (row == GetItemCount() - 1) {
			ctxMenu.SetTitle(wxString::Format("LOD - %d", GetLODLevel(item)));
			ctxMenu.Append(Events::ID::Menu_DeleteGridLOD, Strings::TitleOfMenuItemDeleteGridLOD());
			ctxMenu.AppendSeparator();
		}
	}
	wxMenuItem* menuItem = ctxMenu.Append(Events::ID::Menu_AppendGridLOD, Strings::TitleOfMenuItemAppendGridLOD());
	PopupMenu(&ctxMenu, event.GetPosition());
}

int GridLODViewListCtrl::GetLODLevel(const wxDataViewItem& item) {
	int row = ItemToRow(item);
	return row;
}

void GridLODViewListCtrl::OnAppendLOD(wxCommandEvent& event) {
	double sizeX, sizeY, sizeZ;
	int scaleX, scaleY, scaleZ;
	int nextLevel = GetItemCount();
	DlgNewGridLOD dlg(this, nextLevel);
	if (nextLevel > 0) {
		int lastRow = nextLevel - 1;
		wxString v = GetTextValue(lastRow, 1);
		v.ToCDouble(&sizeX);
		v = GetTextValue(lastRow, 2);
		v.ToCDouble(&sizeY);
		v = GetTextValue(lastRow, 3);
		v.ToCDouble(&sizeZ);
		dlg.SetParentLevelCellSize(sizeX, sizeY, sizeZ);
	}
	dlg.CenterOnParent();
	if (dlg.ShowModal() != wxID_OK) {
		return;
	}
	if (!dlg.GetCellSize(sizeX, sizeY, sizeZ) || sizeX <= ZERO_ERROR || sizeY <= ZERO_ERROR || sizeZ <= ZERO_ERROR) {
		wxMessageBox(Strings::TipOfInvalidGridCellSize(), Strings::AppName(), wxICON_ERROR);
		return;
	}
	dlg.GetCellScale(scaleX, scaleY, scaleZ);
	wxBusyCursor waiting;
	g3dgrid::LOD lod(nextLevel);
	lod.SetCellSize(sizeX, sizeY, sizeZ).SetCellScale(scaleX, scaleY, scaleZ);
	voxelGrid_->SetLOD(lod);
	
	AppendLODItem(lod);
}

void GridLODViewListCtrl::OnDeleteLOD(wxCommandEvent& event) {
	wxDataViewItem item = GetSelection();
	int level = GetLODLevel(item);
	int c = wxMessageBox(Strings::ConfirmToDeleteGridLOD(level), Strings::AppName(), wxYES_NO | wxICON_EXCLAMATION);
	if (c != wxYES) {
		return;
	}
	wxBusyCursor waiting;
	voxelGrid_->DeleteLOD(level);
	DeleteItem(ItemToRow(item));
}

void GridLODViewListCtrl::AppendLODItem(const g3dgrid::LOD& lod) {
	wxVector<wxVariant> itemValue;
	itemValue.push_back(wxString::Format("%d", lod.GetLevel()));
	double sizeX, sizeY, sizeZ;
	lod.GetCellSize(sizeX, sizeY, sizeZ);
	itemValue.push_back(wxString::FromDouble(sizeX, 6));
	itemValue.push_back(wxString::FromDouble(sizeY, 6));
	itemValue.push_back(wxString::FromDouble(sizeZ, 6));
	int scaleX, scaleY, scaleZ;
	lod.GetCellScale(scaleX, scaleY, scaleZ);
	itemValue.push_back(wxString::Format("%d", scaleX));
	itemValue.push_back(wxString::Format("%d", scaleY));
	itemValue.push_back(wxString::Format("%d", scaleZ));
	itemValue.push_back(wxString::Format("%d", 0));
	AppendItem(itemValue);
}