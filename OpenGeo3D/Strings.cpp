#include "Strings.h"
#include <QCoreApplication>

QString Strings::appName() {
	return QString::fromUtf8(APP_NAME);
}

QString Strings::confirmToQuitApp() {
	return QString::asprintf("确定退出%s吗？", APP_NAME);
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
