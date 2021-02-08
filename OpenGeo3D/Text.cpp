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

QString Text::menuQuit() {
	return QString::fromUtf8("退出(&Q)");
}

QString Text::titleOfDlgAbout() {
	return QString::fromUtf8("关于") + appName();
}

QString Text::titleOfConfirmingQuit() {
	return QString::fromUtf8("退出") + appName();
}

QString Text::confirmToQuit() {
	return QString::fromUtf8("确定退出") + appName() + QString::fromUtf8("吗？未保存的数据将会丢失。");
}
