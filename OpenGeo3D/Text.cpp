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

QString Text::nameOfStructureModel() {
	return QString::fromUtf8("结构模型");
}

QString Text::nameOfVoxelGridModel() {
	return QString::fromUtf8("网格模型");
}

QString Text::nameOfDefaultG3DMap() {
	return QString::fromUtf8("默认场景");
}

QString Text::menuFile() {
	return QString::fromUtf8("文件(&F)");
}

QString Text::menuStructureModel() {
	return QString::fromUtf8("结构模型");
}

QString Text::menuOpenGeo3DML() {
	return QString::fromUtf8("打开Geo3DML文件...");
}

QString Text::menuOpenDrillLog() {
	return QString::fromUtf8("打开简单钻孔记录...");
}

QString Text::menuGridModel() {
	return QString::fromUtf8("网格模型");
}

QString Text::menuCloseStructureModel() {
	return QString::fromUtf8("关闭结构模型");
}

QString Text::menuCloseAllModels() {
	return QString::fromUtf8("关闭所有数据");
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

QString Text::titleOfMetadataPage() {
	return QString::fromUtf8("数据");
}

QString Text::titleOfRenderOptionPage() {
	return QString::fromUtf8("可视化");
}

QString Text::confirmToCloseStructureModel() {
	return QString::fromUtf8("确定关闭结构模型吗？未保存的数据将会丢失。");
}

QString Text::confirmToQuit() {
	return QString::fromUtf8("确定退出") + appName() + QString::fromUtf8("吗？未保存的数据将会丢失。");
}

QString Text::filterOfGeo3DMLFiles() {
	return QString::fromUtf8("Geo3DML文件（UTF-8编码）(*.xml)");
}

QString Text::labelOfBasicMetaInfo() {
	return QString::fromUtf8("基本信息");
}

QString Text::labelOfDataset() {
	return QString::fromUtf8("数据集");
}

QString Text::labelOfSubNodeNumber() {
	return QString::fromUtf8("子节点数");
}

QString Text::labelOfId() {
	return QString::fromUtf8("ID");
}

QString Text::labelOfAABB() {
	return QString::fromUtf8("最小外包矩形");
}

QString Text::labelOfAABBMinPoint() {
	return QString::fromUtf8("最小坐标");
}

QString Text::labelOfAABBMaxPoint() {
	return QString::fromUtf8("最大坐标");
}

QString Text::labelOfStructureModel() {
	return QString::fromUtf8("结构模型");
}

QString Text::labelOf3DMap() {
	return QString::fromUtf8("地质图/模型");
}

QString Text::labelOfLayer() {
	return QString::fromUtf8("图层/要素类");
}

QString Text::labelOfActor() {
	return QString::fromUtf8("图元/要素");
}

QString Text::labelOfFeatureClass() {
	return QString::fromUtf8("要素类");
}

QString Text::labelOfFeature() {
	return QString::fromUtf8("要素");
}

QString Text::labelOfName() {
	return QString::fromUtf8("名字");
}

QString Text::labelOfSchema() {
	return QString::fromUtf8("属性表结构");
}
