#pragma once

#include "wxWidgets.h"
#include <wx/aui/auibook.h>
#include <wx/splitter.h>
#include <wx/treectrl.h>

class ScenePanel : public wxSplitterWindow {
public:
	ScenePanel(wxWindow* parent, const wxSize& size);
	virtual ~ScenePanel();

private:
	wxTreeCtrl* sceneTree_;
	wxAuiNotebook* dataPropNotebook_;
	//wxNotebook* dataPropNotebook_;
};