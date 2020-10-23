#pragma once

#include "wxWidgets.h"
#include <wx/aui/framemanager.h>
#include <wx/aui/auibook.h>
#include "wxVTKRenderWindowInteractor.h"
#include "ScenePanel.h"

class Frame : public wxFrame {
public:
	Frame(const wxString& title);
	virtual ~Frame();

private:
	enum {
		ID_Frame = wxID_HIGHEST + 1
	};

private:
	void OnClose(wxCloseEvent& event);
	void OnExit(wxCommandEvent& event);
	void OnAbout(wxCommandEvent& event);
	void OnMenuOpened(wxMenuEvent& event);
	void OnOpenFile(wxCommandEvent& event);

private:
	void InitMenu();
	void InitClientWindows();
	void InitStatusBar();

	wxAuiNotebook* CreateDataPropNotebook();

private:
	wxAuiManager auiMgr_;
	ScenePanel* scenePanel_;
	wxVTKRenderWindowInteractor* renderWindow_;

	wxDECLARE_EVENT_TABLE();
};
