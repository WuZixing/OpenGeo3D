#include "Strings.h"

wxString Strings::AppName() {
	return wxString::FromUTF8(APP_NAME);
}

wxString Strings::AppVersion() {
	return wxString::FromUTF8(APP_VERSION);
}

wxString Strings::AppDescription() {
	return Strings::AppName() + wxString::FromUTF8("是一个开源的、基于网格模型的三维地质建模工具。");
}

wxString Strings::TitleOfMenuFile() {
	return wxString::FromUTF8("文件(&F)");
}

wxString Strings::TitleOfMenuWindow() {
	return wxString::FromUTF8("窗口(&W)");
}

wxString Strings::TitleOfMenuHelp() {
	return wxString::FromUTF8("帮助(&H)");
}

wxString Strings::TitleOfMenuItemOpenFile() {
	return wxString::FromUTF8("打开...(&O)");
}

wxString Strings::TitleOfMenuItemQuit() {
	return wxString::FromUTF8("退出(&Q)");
}

wxString Strings::TitleOfMenuItemAbout() {
	return wxString::FromUTF8("关于(&A)");
}

wxString Strings::ConfirmToQuit() {
	return wxString::Format(wxString::FromUTF8("确定退出%s吗？"), Strings::AppName());
}

wxString Strings::TitleOfSceneTree() {
	return wxString::FromUTF8("图层");
}

wxString Strings::TitleOfDataInfo() {
	return wxString::FromUTF8("数据");
}

wxString Strings::TitleOfVisInfo() {
	return wxString::FromUTF8("可视化");
}

wxString Strings::NameOfStructureModel() {
	return wxString::FromUTF8("结构模型");
}
wxString Strings::NameOfGridModel() {
	return wxString::FromUTF8("网格模型");
}