#include "OpenGeo3D.h"
#include "Config.h"
#include "Frame.h"
#include "Strings.h"

wxIMPLEMENT_APP(OpenGeo3D);

bool OpenGeo3D::OnInit() {
	if (!wxApp::OnInit()) {
		return false;
	}

	wxLocale locale(wxLANGUAGE_CHINESE_SIMPLIFIED);
	wxString appName(Strings::AppName());
	SetAppName(appName);
	Frame* frame = new Frame(appName);
	frame->Show(true);
	frame->Maximize();

	return true;
}
