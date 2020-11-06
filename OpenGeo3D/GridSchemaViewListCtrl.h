#pragma once

#include "wxWidgets.h"
#include <wx/dataview.h>
#include <g3dgrid/VoxelGrid.h>

class GridSchemaViewListCtrl : public wxDataViewListCtrl {
public:
	GridSchemaViewListCtrl(wxWindow* parent, g3dgrid::VoxelGrid* voxelGrid);
	virtual ~GridSchemaViewListCtrl();
	
};