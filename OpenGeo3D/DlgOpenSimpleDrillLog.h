#pragma once

#include "wxWidgets.h"
#include <geo3dml/Model.h>
#include "SizerOfSimpleDrillLogFiles.h"

class DlgOpenSimpleDrillLog : public wxDialog {
public:
	DlgOpenSimpleDrillLog(wxWindow* parent);
	virtual ~DlgOpenSimpleDrillLog();

	geo3dml::Model* LoadAsG3DModel() const;

private:
	void OnButtonOK(wxCommandEvent& event);

private:
	SizerOfSimpleDrillLogFiles* sizerOfDrillFiles_;
};
