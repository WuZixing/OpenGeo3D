#include "Frame.h"
#include <wx/aboutdlg.h>
#include <wx/artprov.h>
#include <wx/colordlg.h> 
#include <wx/propgrid/propgrid.h>
#include <g3dvtk/ObjectFactory.h>
#include <g3dxml/XMLReader.h>
#include <g3dxml/XMLWriter.h>
#include "icon.xpm"
#include "DlgEditVoxelGrid.h"
#include "DlgNewGridModel.h"
#include "DlgOpenSimpleDrillLog.h"
#include "Events.h"
#include "Strings.h"

wxBEGIN_EVENT_TABLE(Frame, wxFrame)
    EVT_MENU_OPEN(Frame::OnMenuOpened)
    EVT_CLOSE(Frame::OnClose)
    EVT_MENU(wxID_EXIT, Frame::OnExit)
    EVT_MENU(wxID_ABOUT, Frame::OnAbout)
    EVT_MENU(Events::ID::Menu_OpenGeo3DML, Frame::OnOpenGeo3DML)
    EVT_MENU(Events::ID::Menu_OpenSimpleDrillLog, Frame::OnOpenSimpleDrillLog)
    EVT_MENU(Events::ID::Menu_CloseStructureModel, Frame::OnCloseStructureModel)
    EVT_MENU(Events::ID::Menu_OpenSGeMSGrid, Frame::OnOpenSGeMSGrid)
    EVT_MENU(Events::ID::Menu_CloseVoxelGridModel, Frame::OnCloseVoxelGridModel)
    EVT_MENU(Events::ID::Menu_CloseAllModels, Frame::OnCloseAllModels)
    EVT_MENU(Events::ID::Menu_FullView, Frame::OnFullView)
    EVT_MENU(Events::ID::Menu_BackgroundColor, Frame::OnBackgroundColor)
    EVT_MENU(Events::ID::Menu_ScaleUpZ, Frame::OnScaleUpZ)
    EVT_MENU(Events::ID::Menu_ScaleDownZ, Frame::OnScaleDownZ)
    EVT_MENU(Events::ID::Menu_CustomizedZScale, Frame::OnCustomizedZScale)
    EVT_MENU(Events::ID::Menu_ResetZScale, Frame::OnResetZScale)
    EVT_MENU(Events::ID::Menu_ProjectPanel, Frame::OnProjectPanel)
    EVT_MENU(Events::ID::Menu_SaveToGeo3DML, Frame::OnSaveToGeo3DML)
    EVT_MENU(Events::ID::Menu_SaveToVoxelGrid, Frame::OnSaveToVoxelGrid)
    EVT_MENU(Events::ID::Menu_EditVoxelGrid, Frame::OnEditVoxelGrid)
    EVT_NOTIFY_RANGE(wxEVT_NULL, Events::ID::Notify_ResetAndRefreshRenderWindow, Events::ID::Notify_RefreshRenderWindow, Frame::OnNotify)
wxEND_EVENT_TABLE()

Frame::Frame(const wxString& title) : wxFrame(nullptr, Events::ID::Window_Frame, title, wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE | wxMAXIMIZE) {
    SetIcon(wxIcon(xpm_icon));
    InitMenu();
    InitClientWindows();
    InitStatusBar();
}

Frame::~Frame() {

}

void Frame::InitMenu() {
    wxMenuBar* menuBar = new wxMenuBar();

    // File(&F)
    wxMenu* menuFile = new wxMenu();
    wxMenu* menuStructureModel = new wxMenu();
    menuStructureModel->Append(Events::ID::Menu_OpenGeo3DML, Strings::TitleOfMenuItemOpenGeo3DML());
    menuStructureModel->Append(Events::ID::Menu_OpenSimpleDrillLog, Strings::TitleOfMenuItemOpenSimpleDrillLog());
    menuStructureModel->AppendSeparator();
    menuStructureModel->Append(Events::ID::Menu_SaveToGeo3DML, Strings::TitleOfMenuItemSaveToGeo3DML());
    menuStructureModel->Append(Events::ID::Menu_CloseStructureModel, Strings::TitleOfMenuItemCloseStructureModel());
    wxMenu* menuGridModel = new wxMenu();
    menuGridModel->Append(Events::ID::Menu_EditVoxelGrid, Strings::TitleOfMenuItemEditVoxelGrid());
    menuGridModel->AppendSeparator();
    menuGridModel->Append(Events::ID::Menu_SaveToVoxelGrid, Strings::TitleOfMenuItemSaveToVoxelGrid());
    menuGridModel->Append(Events::ID::Menu_CloseVoxelGridModel, Strings::TitleOfMenuItemCloseVoxelGridModel());
    menuFile->AppendSubMenu(menuStructureModel, Strings::NameOfStructureModel());
    menuFile->AppendSubMenu(menuGridModel, Strings::NameOfGridModel());
    menuFile->Append(Events::ID::Menu_CloseAllModels, Strings::TitleOfMenuItemCloseAllModels());
    menuFile->AppendSeparator();
    menuFile->Append(wxID_EXIT, Strings::TitleOfMenuItemQuit());
    menuBar->Append(menuFile, Strings::TitleOfMenuFile());

    // Windows(&W)
    wxMenu* menuWnd = new wxMenu();
    menuWnd->Append(Events::ID::Menu_FullView, Strings::TitleOfMenuItemFullView());
    menuWnd->Append(Events::ID::Menu_BackgroundColor, Strings::TitleOfMenuItemBackgroundColor());
    menuWnd->AppendSeparator();
    menuWnd->Append(Events::ID::Menu_ScaleUpZ, Strings::TitleOfMenuItemScaleUpZ());
    menuWnd->Append(Events::ID::Menu_ScaleDownZ, Strings::TitleOfMenuItemScaleDownZ());
    menuWnd->Append(Events::ID::Menu_CustomizedZScale, Strings::TitleOfMenuItemCustomizedZScale());
    menuWnd->Append(Events::ID::Menu_ResetZScale, Strings::TitleOfMenuItemResetZScale());
    menuWnd->AppendSeparator();
    menuWnd->AppendCheckItem(Events::ID::Menu_ProjectPanel, Strings::TitleOfMenuItemProjectPanel());
    menuBar->Append(menuWnd, Strings::TitleOfMenuWindow());

    // Help(&H)
    wxMenu* menuHelp = new wxMenu();
    menuHelp->Append(wxID_ABOUT, Strings::TitleOfMenuItemAbout());
    menuBar->Append(menuHelp, Strings::TitleOfMenuHelp());

    SetMenuBar(menuBar);
}

void Frame::InitClientWindows() {
    wxSize clientSize = GetClientSize();

    auiMgr_.SetManagedWindow(this);

    wxSize size = FromDIP(wxSize(200, -1));
    size.SetHeight(clientSize.GetHeight());
    projectPanel_ = new ProjectPanel(this, size);
    auiMgr_.AddPane(projectPanel_, wxAuiPaneInfo().Caption(Strings::TitleOfProjectPanel()).Left().CloseButton(true).MaximizeButton(true));

    renderWindow_ = vtkSmartPointer<wxVTKRenderWindowInteractor>::Take(new wxVTKRenderWindowInteractor(this, wxID_ANY));
    renderWindow_->SetupRenderWindow(projectPanel_->GetRenderer());
    auiMgr_.AddPane(renderWindow_, wxAuiPaneInfo().CenterPane().PaneBorder(false));
    auiMgr_.Update();
}

void Frame::InitStatusBar() {

}

void Frame::OnClose(wxCloseEvent& event) {
    if (event.CanVeto()) {
        int c = wxMessageBox(Strings::ConfirmToQuit(), GetTitle(), wxYES_NO | wxICON_EXCLAMATION);
        if (c != wxYES) {
            event.Veto();
            return;
        }
    }
    wxBusyCursor waiting;
    Destroy();
}

void Frame::OnExit(wxCommandEvent& event) {
    Close(false);
}

void Frame::OnAbout(wxCommandEvent& event) {
    wxAboutDialogInfo aboutInfo;
    aboutInfo.SetName(Strings::AppName());
    aboutInfo.SetVersion(Strings::AppVersion());
    aboutInfo.SetDescription(Strings::AppDescription());
    wxString copyRightString(wxS("(C)2020"));
    wxDateTime dt = wxDateTime::Now();
    if (dt.GetYear() > 2020) {
        copyRightString = wxString::Format("%s-%s", copyRightString, dt.Format("%Y"));
    }
    aboutInfo.SetCopyright(copyRightString);
    aboutInfo.SetWebSite(wxS("https://github.com/WuZixing/OpenGeo3D"));
    wxAboutBox(aboutInfo);
}

void Frame::OnMenuOpened(wxMenuEvent& event) {
    wxMenu* evtMenu = event.GetMenu();
    if (evtMenu == nullptr) {
        return;
    }
    wxMenuItem* itemProjectPanel = evtMenu->FindItem(Events::ID::Menu_ProjectPanel);
    if (itemProjectPanel != nullptr) {
        wxAuiPaneInfo& pane = auiMgr_.GetPane(projectPanel_);
        itemProjectPanel->Check(pane.IsShown());
    }
}

void Frame::OnOpenGeo3DML(wxCommandEvent& event) {
    wxString filePath = wxFileSelector(Strings::TipOfOpenGeo3DML(), wxEmptyString, wxEmptyString, wxEmptyString, Strings::WildcardOfGeo3DMLFile());
    if (filePath.IsEmpty()) {
        return;
    }
    geo3dml::Object* g3dObject = NULL;
    g3dvtk::ObjectFactory g3dVtkFactory;
    g3dxml::XMLReader xmlReader(&g3dVtkFactory);
    wxBeginBusyCursor();
    g3dObject = xmlReader.LoadXMLFile(filePath.ToUTF8().data());
    wxEndBusyCursor();
    if (g3dObject == NULL) {
        wxMessageBox(wxString::FromUTF8(xmlReader.Error().c_str()), GetTitle(), wxICON_ERROR);
    } else {
        wxBusyCursor wait;
        geo3dml::Model* model = dynamic_cast<geo3dml::Model*>(g3dObject);
        if (model != NULL) {
            projectPanel_->AppendG3DModel(model, true);
        } else {
            geo3dml::Project* project = dynamic_cast<geo3dml::Project*>(g3dObject);
            if (project != NULL) {
                project->BindFeatureClassesToLayers(&g3dVtkFactory);
                int numOfMaps = project->GetMapCount();
                while (project->GetModelCount() > 0) {
                    geo3dml::Model* model = project->RemoveModelAt(0);
                    projectPanel_->AppendG3DModel(model, numOfMaps == 0);
                }
                while (project->GetMapCount() > 0) {
                    geo3dml::Map* map = project->RemoveMapAt(0);
                    projectPanel_->AppendG3DMap(map);
                }
            }
            delete g3dObject;
        }
        projectPanel_->ExpandStructureModelTree();
        Events::Notify(Events::ID::Notify_ResetAndRefreshRenderWindow);
    }
}

void Frame::OnSaveToGeo3DML(wxCommandEvent& event) {
    geo3dml::Project* g3dProject = projectPanel_->GetG3DProject();
    wxString projName = wxString::FromUTF8(g3dProject->GetName());
    int indexDefaultExtension = 1;
    wxString filePath = wxFileSelectorEx(Strings::TitleOfMenuItemSaveToGeo3DML(), wxEmptyString, projName, &indexDefaultExtension, Strings::WildcardOfGeo3DMLFileWithVersion(), wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    if (filePath.IsEmpty()) {
        return;
    }
    wxBeginBusyCursor();
    wxFileName fileName(filePath);
    projName = fileName.GetName();
    g3dProject->SetName(projName.ToUTF8().data());
    g3dxml::XMLWriter projectWriter;
    bool isOK = projectWriter.Write(g3dProject, filePath.ToUTF8().data(), indexDefaultExtension == 0 ? g3dxml::SchemaVersion::Schema_1_x : g3dxml::SchemaVersion::Schema_1_0);
    wxEndBusyCursor();
    if (isOK) {
        wxMessageBox(Strings::TipOfSucceedInSavingToGeo3DMLFile(projName), GetTitle(), wxOK | wxICON_INFORMATION | wxCENTER);
    } else {
        wxMessageBox(Strings::TipOfErrorInSavingToGeo3DMLFile(projName, wxString::FromUTF8(projectWriter.Error())), GetTitle(), wxOK | wxICON_ERROR | wxCENTER);
    }
}

void Frame::OnOpenSimpleDrillLog(wxCommandEvent& event) {
    DlgOpenSimpleDrillLog dlg(this);
    dlg.CentreOnScreen();
    if (dlg.ShowModal() != wxID_OK) {
        return;
    }
    wxBusyCursor waiting;
    geo3dml::Model* g3dModel = dlg.LoadAsG3DModel();
    if (g3dModel != nullptr) {
        projectPanel_->AppendG3DModel(g3dModel, true);
        projectPanel_->ExpandStructureModelTree();
        Events::Notify(Events::ID::Notify_ResetAndRefreshRenderWindow);
    }
}

void Frame::OnOpenSGeMSGrid(wxCommandEvent& event) {
    
}

void Frame::OnSaveToVoxelGrid(wxCommandEvent& event) {

}

void Frame::OnEditVoxelGrid(wxCommandEvent& event) {
    DlgEditVoxelGrid dlg(this, projectPanel_->GetG3DVoxelGrid());
    dlg.CenterOnScreen();
    if (dlg.ShowModal() != wxID_OK) {
        return;
    }
    wxBusyCursor waiting;
    projectPanel_->UpdateVoxelGridModel();
    projectPanel_->ExpandGridModelNodeTree();
}

void Frame::OnNotify(wxNotifyEvent& notify) {
    switch (notify.GetId()) {
    case Events::ID::Notify_ResetAndRefreshRenderWindow: {
        wxBusyCursor waiting;
        renderWindow_->ResetAndRender();
        break;
    }
    case Events::ID::Notify_RefreshRenderWindow: {
        wxBusyCursor waiting;
        renderWindow_->Render();
        break;
    }
    default:
        break;
    }
}

void Frame::OnFullView(wxCommandEvent& event) {
    Events::Notify(Events::ID::Notify_ResetAndRefreshRenderWindow);
}

void Frame::OnBackgroundColor(wxCommandEvent& event) {
    double r = 0, g = 0, b = 0;
    vtkRenderer* renderer = projectPanel_->GetRenderer();
    renderer->GetBackground(r, g, b);
    wxColour clr((unsigned char)(wxRound(r * 255) % 256), (unsigned char)(wxRound(g * 255) % 256), (unsigned char)(wxRound(b * 255) % 256));
    clr = wxGetColourFromUser(this, clr);
    if (!clr.IsOk()) {
        return;
    }
    wxBusyCursor waiting;
    r = clr.Red() / 255.0f;
    g = clr.Green() / 255.0f;
    b = clr.Blue() / 255.0f;
    renderer->SetBackground(r, g, b);
    Events::Notify(Events::ID::Notify_RefreshRenderWindow);
}

void Frame::OnScaleUpZ(wxCommandEvent& event) {
    wxBusyCursor waiting;
    vtkTransform* t = projectPanel_->GetTransform();
    double scales[3] = { 1.0 };
    t->GetScale(scales);
    scales[2] = scales[2] * 1.1;
    t->Identity();
    t->Scale(scales);
    Events::Notify(Events::ID::Notify_RefreshRenderWindow);
}

void Frame::OnScaleDownZ(wxCommandEvent& event) {
    wxBusyCursor waiting;
    vtkTransform* t = projectPanel_->GetTransform();
    double scales[3] = { 1.0 };
    t->GetScale(scales);
    scales[2] = scales[2] * 0.9;
    t->Identity();
    t->Scale(scales);
    Events::Notify(Events::ID::Notify_RefreshRenderWindow);
}

void Frame::OnCustomizedZScale(wxCommandEvent& event) {
    vtkTransform* t = projectPanel_->GetTransform();
    double scales[3] = { 1.0 };
    t->GetScale(scales);
    wxString currentScale = wxString::FromDouble(scales[2], 6);
    wxString v = wxGetTextFromUser(Strings::TipOfInputCusotimizedZScale(), GetTitle(), currentScale, this);
    if (v.IsEmpty()) {
        return;
    }
    if (v.ToDouble(&scales[2])) {
        if (scales[2] < 0 || fabs(scales[2]) < 10e-6 || scales[2] > 1000) {
            wxMessageBox(Strings::TipOfInvalidZScale(), wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_WARNING);
            return;
        }
        wxBusyCursor waiting;
        t->Identity();
        t->Scale(scales);
        Events::Notify(Events::ID::Notify_RefreshRenderWindow);
    }
}

void Frame::OnResetZScale(wxCommandEvent& event) {
    wxBusyCursor waiting;
    vtkTransform* t = projectPanel_->GetTransform();
    t->Identity();
    Events::Notify(Events::ID::Notify_RefreshRenderWindow);
}

void Frame::OnProjectPanel(wxCommandEvent& event) {
    wxBusyCursor waiting;
    wxAuiPaneInfo& pane = auiMgr_.GetPane(projectPanel_);
    pane.Show(!pane.IsShown());
    auiMgr_.Update();
}

void Frame::OnCloseAllModels(wxCommandEvent& event) {
    int c = wxMessageBox(Strings::ConfirmToCloseAllModels(), GetTitle(), wxYES_NO | wxICON_EXCLAMATION);
    if (c != wxYES) {
        return;
    }
    wxBusyCursor waiting;
    projectPanel_->CloseAllModels();
    Events::Notify(Events::ID::Notify_RefreshRenderWindow);
}

void Frame::OnCloseStructureModel(wxCommandEvent& event) {
    int c = wxMessageBox(Strings::ConfirmToCloseStructureModel(), GetTitle(), wxYES_NO | wxICON_EXCLAMATION);
    if (c != wxYES) {
        return;
    }
    wxBusyCursor waiting;
    projectPanel_->CloseStructureModel();
    Events::Notify(Events::ID::Notify_RefreshRenderWindow);
}

void Frame::OnCloseVoxelGridModel(wxCommandEvent& event) {
    int c = wxMessageBox(Strings::ConfirmToCloseVoxelGridModel(), GetTitle(), wxYES_NO | wxICON_EXCLAMATION);
    if (c != wxYES) {
        return;
    }
    wxBusyCursor waiting;
    projectPanel_->CloseVoxelGridModel();
    Events::Notify(Events::ID::Notify_RefreshRenderWindow);
}
