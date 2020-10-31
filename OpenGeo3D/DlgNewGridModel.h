#pragma once

#include "wxWidgets.h"
#include <g3dgrid/VoxelGrid.h>

class DlgNewGridModel : public wxDialog {
public:
	DlgNewGridModel(wxWindow* parent);
	virtual ~DlgNewGridModel();

	g3dgrid::VoxelGrid* MakeGrid() const;

private:
	void OnButtonOK(wxCommandEvent& event);

private:
	wxTextCtrl *ctrlName_, *ctrlDescription_, *ctrlSRS_;
	wxTextCtrl *ctrlX_, *ctrlY_, *ctrlZ_;
};
