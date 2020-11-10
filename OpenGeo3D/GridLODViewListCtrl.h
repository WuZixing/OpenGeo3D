#pragma once

#include "wxWidgets.h"
#include <wx/dataview.h>
#include <g3dgrid/VoxelGrid.h>

class GridLODViewListCtrl : public wxDataViewListCtrl {
public:
	GridLODViewListCtrl(wxWindow* parent, const wxSize& size = wxDefaultSize, bool enableEdit = true);
	virtual ~GridLODViewListCtrl();

	void BindToLocalGrid(g3dgrid::VoxelGrid* voxelGrid);

private:
	void OnItemContextMenu(wxDataViewEvent& event);
	void OnAppendLOD(wxCommandEvent& event);
	void OnDeleteLOD(wxCommandEvent& event);

private:
	int GetLODLevel(const wxDataViewItem& item);
	void AppendLODItem(const g3dgrid::LOD& lod);

private:
	g3dgrid::VoxelGrid* voxelGrid_;
};