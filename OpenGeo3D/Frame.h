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
	void OnOpenSGeMSGrid(wxCommandEvent& event);
	void OnFullView(wxCommandEvent& event);
	void OnBackgroundColor(wxCommandEvent& event);
	void OnScaleUpZ(wxCommandEvent& event);
	void OnScaleDownZ(wxCommandEvent& event);
	void OnCustomizedZScale(wxCommandEvent& event);
	void OnResetZScale(wxCommandEvent& event);
	void OnProjectPanel(wxCommandEvent& event);
	void OnLogWindow(wxCommandEvent& event);
	void OnNotify(wxNotifyEvent& notify);
	void OnCloseAllModels(wxCommandEvent& event);
	void OnCloseStructureModel(wxCommandEvent& event);
	void OnCloseVoxelGridModel(wxCommandEvent& event);
	void OnSaveToGeo3DML(wxCommandEvent& event);
	void OnSaveToVoxelGrid(wxCommandEvent& event);
	void OnEditVoxelGrid(wxCommandEvent& event);

private:
	void InitMenu();
	void InitClientWindows();
	void InitStatusBar();

private:
	wxAuiManager auiMgr_;
	ProjectPanel* projectPanel_;
	vtkSmartPointer<wxVTKRenderWindowInteractor> renderWindow_;
	wxLogWindow* logWindow_;

	wxDECLARE_EVENT_TABLE();
};
