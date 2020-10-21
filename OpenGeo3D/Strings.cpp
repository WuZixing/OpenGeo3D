#include "Strings.h"

QString Strings::appName() {
	return QString::fromUtf8(APP_NAME);
}

QString Strings::confirmToQuitApp() {
	return QString::asprintf("确定退出%s吗？", APP_NAME);
}

QString Strings::menuTitleOfFile() {
	return QString::fromUtf8("文件(&F)");
}

QString Strings::menuTitleOfQuit() {
	return QString::fromUtf8("退出(&Q)");
}

QString Strings::menuTitleOfWindows() {
	return QString::fromUtf8("窗口(&W)");
}

QString Strings::menuTitleOfHelp() {
	return QString::fromUtf8("帮助(&H)");
}

QString Strings::menuTitleOfAbout() {
	return QString::fromUtf8("关于(&A)");
}

QString Strings::aboutApp() {
	return QString::asprintf("%s v%s", APP_NAME, APP_VERSION);
}

QString Strings::titleOfSceneTree() {
	return QString::fromUtf8("图层");
}

QString Strings::titleOfSceneDataTab() {
	return QString::fromUtf8("数据");
}

QString Strings::titleOfSceneVisualizationTab() {
	return QString::fromUtf8("可视化");
}
