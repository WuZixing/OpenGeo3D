#pragma once

#include "wxWidgets.h"
#include <wx/aui/framemanager.h>
#include "wxVTKRenderWindowInteractor.h"
#include "ProjectPanel.h"

class Frame : public wxFrame {
public:
	Frame(const wxString& title);
	virtual ~Frame();

private:
	void OnClose(wxCloseEvent& event);
	void OnExit(wxCommandEvent& event);
	void OnAbout(wxCommandEvent& event);
	void OnMenuOpened(wxMenuEvent& event);
	void OnOpenGeo3DML(wxCommandEvent& event);
	void OnOpenSimpleDrillLog(wxCommandEvent& event);
	void OnNewGridModel(wxCommandEvent& event);
	void OnOpenSGeMSGrid(wxCommandEvent& event);
	void OnFullView(wxCommandEvent& event);
	void OnBackgroundColor(wxCommandEvent& event);
	void OnScaleUpZ(wxCommandEvent& event);
	void OnScaleDownZ(wxCommandEvent& event);
	void OnCustomizedZScale(wxCommandEvent& event);
	void OnResetZScale(wxCommandEvent& event);
	void OnProjectPanel(wxCommandEvent& event);
	void OnNotify(wxNotifyEvent& notify);

private:
	void InitMenu();
	void InitClientWindows();
	void InitStatusBar();

private:
	wxAuiManager auiMgr_;
	ProjectPanel* projectPanel_;
	vtkSmartPointer<wxVTKRenderWindowInteractor> renderWindow_;

	wxDECLARE_EVENT_TABLE();
};
