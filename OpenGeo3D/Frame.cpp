#include "Frame.h"
#include <wx/aboutdlg.h>
#include <wx/artprov.h>
#include <wx/propgrid/propgrid.h>
#include "Strings.h"

wxBEGIN_EVENT_TABLE(Frame, wxFrame)
    EVT_CLOSE(Frame::OnClose)
    EVT_MENU(wxID_OPEN, Frame::OnOpenFile)
    EVT_MENU(wxID_EXIT, Frame::OnExit)
    EVT_MENU(wxID_ABOUT, Frame::OnAbout)
    EVT_MENU_OPEN(Frame::OnMenuOpened)
wxEND_EVENT_TABLE()

Frame::Frame(const wxString& title) : wxFrame(nullptr, ID_Frame, title, wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE | wxMAXIMIZE) {
    InitMenu();
    InitClientWindows();
    InitStatusBar();
}

Frame::~Frame() {
    // to avoid the VTK complain of deleting an object with non-zero reference count.
    if (renderWindow_ != NULL) {
        wxWindow* parent = renderWindow_->GetParent();
        if (parent != NULL) {
            parent->RemoveChild(renderWindow_);
        }
        renderWindow_->Delete();
    }
}

void Frame::InitMenu() {
    wxMenuBar* menuBar = new wxMenuBar();

    // File(&F)
    wxMenu* menuFile = new wxMenu();
    menuFile->Append(wxID_OPEN, Strings::TitleOfMenuItemOpenFile());
    menuFile->AppendSeparator();
    menuFile->Append(wxID_EXIT, Strings::TitleOfMenuItemQuit());
    menuBar->Append(menuFile, Strings::TitleOfMenuFile());

    // Windows(&W)
    wxMenu* menuWnd = new wxMenu();
    menuBar->Append(menuWnd, Strings::TitleOfMenuWindow());

    // Help(&H)
    wxMenu* menuHelp = new wxMenu();
    menuHelp->Append(wxID_ABOUT, Strings::TitleOfMenuItemAbout());
    menuBar->Append(menuHelp, Strings::TitleOfMenuHelp());

    SetMenuBar(menuBar);
}

void Frame::InitClientWindows() {
    auiMgr_.SetManagedWindow(this);

    /*
    treeCtrlScene_ = new SceneTree(this, FromDIP(wxSize(200, 250)));
    auiMgr_.AddPane(treeCtrlScene_, 
        wxAuiPaneInfo().Name(Strings::TitleOfSceneTree()).Caption(Strings::TitleOfSceneTree()).Left().Layer(0).Position(0).CloseButton(true).MaximizeButton(true));
    
    notebookDataProp_ = CreateDataPropNotebook();
    //auiMgr_.AddPane(notebookDataProp_, wxAuiPaneInfo().Name("DataProp").Caption("Notebook").Left().Layer(0).Position(1).CloseButton(true).MaximizeButton(true));
    auiMgr_.AddPane(notebookDataProp_, wxAuiPaneInfo().Left().Layer(0).Position(1).CloseButton(false).MaximizeButton(false));
    */

    scenePanel_ = new ScenePanel(this, FromDIP(wxSize(200, 250)));
    auiMgr_.AddPane(scenePanel_, wxAuiPaneInfo().Caption(Strings::TitleOfSceneTree()).Left().CloseButton(true).MaximizeButton(true));

    renderWindow_ = new wxVTKRenderWindowInteractor(this, wxID_ANY);
    renderWindow_->SetupRenderWindow();
    auiMgr_.AddPane(renderWindow_, wxAuiPaneInfo().CenterPane().PaneBorder(false));
    auiMgr_.Update();
}

void Frame::InitStatusBar() {

}

wxAuiNotebook* Frame::CreateDataPropNotebook() {
    // create the notebook off-window to avoid flicker
    wxSize client_size = GetClientSize();

    wxAuiNotebook* ctrl = new wxAuiNotebook(this, wxID_ANY,
        wxPoint(client_size.x, client_size.y),
        FromDIP(wxSize(160, 200)),
        wxAUI_NB_TOP | wxAUI_NB_CLOSE_ON_ACTIVE_TAB);
    ctrl->Freeze();

    wxBitmap page_bmp = wxArtProvider::GetBitmap(wxART_INFORMATION, wxART_OTHER, FromDIP(wxSize(16, 16)));

    ctrl->AddPage(new wxPropertyGrid(this), wxT("Data"), true, page_bmp);
    ctrl->AddPage(new wxPropertyGrid(this), wxT("Visualization"), false, page_bmp);

    ctrl->Thaw();
    return ctrl;
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
    wxString copyRightString("(C)2020");
    wxDateTime dt = wxDateTime::Now();
    if (dt.GetYear() > 2020) {
        copyRightString = wxString::Format("%s-%s", copyRightString, dt.Format("%Y"));
    }
    aboutInfo.SetCopyright(copyRightString);
    aboutInfo.SetWebSite("https://github.com/WuZixing/OpenGeo3D");
    wxAboutBox(aboutInfo);
}

void Frame::OnMenuOpened(wxMenuEvent& event) {

}

void Frame::OnOpenFile(wxCommandEvent& event) {

}