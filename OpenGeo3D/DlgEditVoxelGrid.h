#pragma once

#include "wxWidgets.h"
#include <g3dgrid/VoxelGrid.h>

class DlgEditVoxelGrid : public wxDialog {
public:
	DlgEditVoxelGrid(wxWindow* parent, g3dgrid::VoxelGrid* voxelGrid);
	virtual ~DlgEditVoxelGrid();

private:
	void OnButtonOK(wxCommandEvent& event);

private:
	g3dgrid::VoxelGrid* voxelGrid_;
	wxTextCtrl *ctrlID_, *ctrlName_, *ctrlDescription_, *ctrlSRS_;
	wxTextCtrl *ctrlX_, *ctrlY_, *ctrlZ_;
};
