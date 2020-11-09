#pragma once

#include "wxWidgets.h"
#include <wx/spinctrl.h>

class DlgNewGridLOD : public wxDialog {
public:
	DlgNewGridLOD(wxWindow* parent, int level);
	virtual ~DlgNewGridLOD();

	void SetParentLevelCellSize(double x, double y, double z);
	bool GetCellSize(double& x, double& y, double& z) const;
	bool GetCellScale(int& x, int& y, int& z) const;

private:
	void OnSetScaleX(wxSpinEvent& evt);
	void OnSetScaleY(wxSpinEvent& evt);
	void OnSetScaleZ(wxSpinEvent& evt);
	void OnButtonOK(wxCommandEvent& event);

private:
	int level_;
	wxTextCtrl *ctrlSizeX_, *ctrlSizeY_, *ctrlSizeZ_;
	wxSpinCtrl *ctrlScaleX_, *ctrlScaleY_, *ctrlScaleZ_;
	double parentSizeX_, parentSizeY_, parentSizeZ_;
};