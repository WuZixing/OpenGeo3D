#pragma once

#include "wxWidgets.h"
#include <wx/dataview.h>
#include <g3dgrid/VoxelGrid.h>

class GridSchemaViewListCtrl : public wxDataViewListCtrl {
public:
	GridSchemaViewListCtrl(wxWindow* parent);
	virtual ~GridSchemaViewListCtrl();
	
	void BindToLocalGrid(g3dgrid::VoxelGrid* voxelGrid);
};