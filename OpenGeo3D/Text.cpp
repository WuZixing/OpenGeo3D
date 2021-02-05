#include "Text.h"

QString Text::appName() {
	return QString(APP_NAME);
}

QString Text::appVersion() {
	return QString(APP_VERSION);
}

QString Text::appDescription() {
	return appName() + QString::fromUtf8("是一个开源的、以网格为基础的三维地质建模工具。");
}

QString Text::appWebSite() {
	return QStringLiteral("https://github.com/WuZixing/OpenGeo3D");
}

QString Text::menuHelp() {
	return QString::fromUtf8("帮助(&H)");
}

QString Text::menuAbout() {
	return QString::fromUtf8("关于(&A)...");
}

QString Text::titleOfDlgAbout() {
	return QString::fromUtf8("关于") + appName();
}
