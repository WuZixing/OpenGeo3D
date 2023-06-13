// UTF-8编码
#include "Text.h"
#include <QtGui/QKeySequence>

QString Text::appName() {
	return QStringLiteral(APP_NAME);
}

QString Text::appVersion() {
	return QStringLiteral(APP_VERSION);
}

QString Text::appDescription() {
	return appName() + QStringLiteral("是一个开源的、以网格为基础的三维地质建模工具。");
}

QString Text::appWebSite() {
	return QStringLiteral("https://github.com/WuZixing/OpenGeo3D");
}

QString Text::nameOfStructureModel() {
	return QStringLiteral("结构模型");
}

QString Text::nameOfVoxelGridModel() {
	return QStringLiteral("网格模型");
}

QString Text::nameOfDefaultG3DMap() {
	return QStringLiteral("默认场景");
}

QString Text::nameOfClassUnknown() {
	return QStringLiteral("Unknown");
}

QString Text::nameOfClassG3DPoint() {
	return QStringLiteral("Point");
}

QString Text::nameOfClassG3DMPoint() {
	return QStringLiteral("Multiple Point");
}

QString Text::nameOfClassG3DAnnotation() {
	return QStringLiteral("Annotation");
}

QString Text::nameOfClassG3DLineString() {
	return QStringLiteral("Line String");
}

QString Text::nameOfClassG3DTIN() {
	return QStringLiteral("TIN");
}

QString Text::nameOfClassG3DUniformGrid() {
	return QStringLiteral("Uniform Grid");
}

QString Text::nameOfClassG3DCornerGrid() {
	return QStringLiteral("Corner Grid");
}

QString Text::nameOfClassG3DGTPVolume() {
	return QStringLiteral("GTP Grid");
}

QString Text::nameOfClassRectifiedGrid() {
	return QStringLiteral("Rectified Grid");
}

QString Text::nameOfClassTetrahedronVolume() {
	return QStringLiteral("Tetrahedron Grid");
}

QString Text::nameOfClassCuboidVolume() {
	return QStringLiteral("Hexahedron Grid");
}

QString Text::nameOfRepresentationModePoints() {
	return QStringLiteral("顶点");
}

QString Text::nameOfRepresentationModeWireframe() {
	return QStringLiteral("线框");
}

QString Text::nameOfRepresentationModeSurface() {
	return QStringLiteral("面");
}

QString Text::nameOfDrillModel() {
	return QStringLiteral("钻孔");
}

QString Text::nameOfDrillFeatureClass() {
	return QStringLiteral("钻孔");
}

QString Text::nameOfDrillPositionFeature() {
	return QStringLiteral("钻孔位置");
}

QString Text::menuFile() {
	return QStringLiteral("文件(&F)");
}

QString Text::menuStructureModel() {
	return QStringLiteral("结构模型");
}

QString Text::menuOpenGeo3DML() {
	return QStringLiteral("打开Geo3DML文件...");
}

QString Text::menuOpenDrillLog() {
	return QStringLiteral("打开简单钻孔记录...");
}

QString Text::menuGridModel() {
	return QStringLiteral("网格模型");
}

QString Text::menuCloseStructureModel() {
	return QStringLiteral("关闭结构模型");
}

QString Text::menuCloseAllModels() {
	return QStringLiteral("关闭所有数据");
}

QString Text::menuHelp() {
	return QStringLiteral("帮助(&H)");
}

QString Text::menuAbout() {
	return QStringLiteral("关于(&A)...");
}

QString Text::menuQuit() {
	return QStringLiteral("退出(&Q)");
}

QString Text::menuWindow() {
	return QStringLiteral("窗口(&W)");
}

QString Text::menuFullView() {
	return QStringLiteral("全景(&R)\t") + QKeySequence(Qt::Modifier::CTRL | Qt::Key::Key_R).toString();
}

QString Text::menuBackgroundColor() {
	return QStringLiteral("背景颜色(&B)\t") + QKeySequence(Qt::Modifier::CTRL | Qt::Key::Key_B).toString();
}

QString Text::menuScaleZUp() {
	return QStringLiteral("Z轴拉伸(&U)\t") + QKeySequence(Qt::Modifier::CTRL | Qt::Key::Key_Up).toString();
}

QString Text::menuScaleZDown() {
	return QStringLiteral("Z轴压缩(&D)\t") + QKeySequence(Qt::Modifier::CTRL | Qt::Key::Key_Down).toString();
}

QString Text::menuCustomizedZScale() {
	return QStringLiteral("自定义Z轴缩放...");
}

QString Text::menuResetZScale() {
	return QStringLiteral("取消Z轴缩放");
}

QString Text::menuProjectPanel() {
	return QStringLiteral("模型列表");
}

QString Text::menuSaveToGeo3DML() {
	return QStringLiteral("保存为Geo3DML文件...");
}

QString Text::titleOfDlgAbout() {
	return QStringLiteral("关于") + appName();
}

QString Text::titleOfConfirmingQuit() {
	return QStringLiteral("退出") + appName();
}

QString Text::titleOfProjectPanel() {
	return QStringLiteral("模型");
}

QString Text::titleOfMetadataPage() {
	return QStringLiteral("数据");
}

QString Text::titleOfRenderOptionPage() {
	return QStringLiteral("可视化");
}

QString Text::confirmToCloseStructureModel() {
	return QStringLiteral("确定关闭结构模型吗？未保存的数据将会丢失。");
}

QString Text::confirmToCloseAllModels() {
	return QStringLiteral("确定关闭所有模型吗？未保存的数据将会丢失。");
}

QString Text::confirmToQuit() {
	return QStringLiteral("确定退出") + appName() + QStringLiteral("吗？未保存的数据将会丢失。");
}

QString Text::filterOfGeo3DMLFile() {
	return QStringLiteral("Geo3DML文件（UTF-8编码）(*.xml)");
}

QString Text::filterOfGeo3DMLFileWithVersion() {
	return QStringLiteral("Geo3DML（v1.x）文件(*.xml);;Geo3DML（v1.0）文件(*.xml)");
}

QString Text::filterOfDrillPositionFile() {
	return QStringLiteral("钻孔位置(*.csv;*.prn)");
}

QString Text::filterOfDrillLogFile() {
	return QStringLiteral("钻孔测试数据(*.csv;*.prn)");
}

QString Text::filterOfShpFile() {
	return QStringLiteral("Shape文件(*.shp)");
}

QString Text::labelOfBasicMetaInfo() {
	return QStringLiteral("基本信息");
}

QString Text::labelOfDataset() {
	return QStringLiteral("数据集");
}

QString Text::labelOfSubNodeNumber() {
	return QStringLiteral("子节点数");
}

QString Text::labelOfId() {
	return QStringLiteral("ID");
}

QString Text::labelOfAABB() {
	return QStringLiteral("最小外包矩形");
}

QString Text::labelOfAABBMinPoint() {
	return QStringLiteral("最小坐标");
}

QString Text::labelOfAABBMaxPoint() {
	return QStringLiteral("最大坐标");
}

QString Text::labelOfProject() {
	return QStringLiteral("工程");
}

QString Text::labelOfStructureModel() {
	return QStringLiteral("结构模型");
}

QString Text::labelOfMap() {
	return QStringLiteral("地质图");
}

QString Text::labelOfMapOrModel() {
	return QStringLiteral("地质图|模型");
}

QString Text::labelOfLayer() {
	return QStringLiteral("图层");
}

QString Text::labelOfLayerOrFeatureClass() {
	return QStringLiteral("图层|要素类");
}

QString Text::labelOfActor() {
	return QStringLiteral("图元|要素");
}

QString Text::labelOfModel() {
	return QStringLiteral("模型");
}

QString Text::labelOfFeatureClass() {
	return QStringLiteral("要素类");
}

QString Text::labelOfFeature() {
	return QStringLiteral("要素");
}

QString Text::labelOfName() {
	return QStringLiteral("名字");
}

QString Text::labelOfSchema() {
	return QStringLiteral("属性表结构");
}

QString Text::labelOfGeometry() {
	return QStringLiteral("几何对象");
}

QString Text::labelOfLOD() {
	return QStringLiteral("LOD");
}

QString Text::labelOfClassName() {
	return QStringLiteral("数据类型");
}

QString Text::labelOfNumberOfVertices() {
	return QStringLiteral("顶点数");
}

QString Text::labelOfNumberOfTriangles() {
	return QStringLiteral("三角形数");
}

QString Text::labelOfNumberOfPrisms() {
	return QStringLiteral("三棱柱数");
}

QString Text::labelOfNumberOfTetrahedrons() {
	return QStringLiteral("四面体数");
}

QString Text::labelOfNumberOfHexahedrons() {
	return QStringLiteral("六面体数");
}

QString Text::labelOfAxisVectorI() {
	return QStringLiteral("I轴向量");
}

QString Text::labelOfAxisVectorJ() {
	return QStringLiteral("J轴向量");
}

QString Text::labelOfAxisVectorK() {
	return QStringLiteral("K轴向量");
}

QString Text::labelOfGridOrigin() {
	return QStringLiteral("网格原点");
}

QString Text::labelOfGridCellSize() {
	return QStringLiteral("网格单元大小");
}

QString Text::labelOfGridCellDimension() {
	return QStringLiteral("网格单元数目");
}

QString Text::labelOfGridCellEnvelope() {
	return QStringLiteral("网格范围");
}

QString Text::labelOfGridCellEnvelopeLow() {
	return QStringLiteral("最小坐标");
}

QString Text::labelOfGridCellEnvelopeHigh() {
	return QStringLiteral("最大坐标");
}

QString Text::labelOfRenderOption() {
	return QStringLiteral("绘制参数");
}

QString Text::labelOfColor() {
	return QStringLiteral("颜色");
}

QString Text::labelOfRepresentationMode() {
	return QStringLiteral("绘制方式");
}

QString Text::labelOfDrawEdge() {
	return QStringLiteral("绘制线框");
}

QString Text::tipOfCustomizedZScale() {
	return QStringLiteral("请输入Z轴缩放系数（大于0，且小于1000）：");
}

QString Text::labelOfSimpleDrillLogGroup() {
	return QStringLiteral("钻孔数据");
}

QString Text::labelOfDrillPositionFilePath() {
	return QStringLiteral("钻孔位置");
}

QString Text::labelOfSavingDrillPositionToFile() {
	return QStringLiteral("保存钻孔位置到Shape文件");
}

QString Text::labelOfOpen() {
	return QStringLiteral("打开...");
}

QString Text::labelOfSave() {
	return QStringLiteral("保存...");
}

QString Text::labelOfNo() {
	return QStringLiteral("#");
}

QString Text::labelOfDrillNo() {
	return QStringLiteral("钻孔编号");
}

QString Text::labelOfDrillDepth() {
	return QStringLiteral("孔深");
}

QString Text::labelOfX() {
	return QStringLiteral("X");
}

QString Text::labelOfY() {
	return QStringLiteral("Y");
}

QString Text::labelOfZ() {
	return QStringLiteral("Z");
}

QString Text::labelOfDrillLog() {
	return QStringLiteral("钻孔测试记录");
}

QString Text::labelOfAppend() {
	return QStringLiteral("添加...");
}

QString Text::labelOfClear() {
	return QStringLiteral("清除");
}

QString Text::labelOfFilePath() {
	return QStringLiteral("文件路径");
}

QString Text::labelOfField() {
	return QStringLiteral("属性字段");
}

QString Text::labelOfFieldValueType() {
	return QStringLiteral("数据类型");
}

QString Text::labelOfCRS() {
	return QStringLiteral("坐标参照系");
}

QString Text::labelOfDescription() {
	return QStringLiteral("说明");
}

QString Text::tipOfSpecifyFieldValueType() {
	return QStringLiteral("请为属性字段指定适当的“数据类型”。");
}

QString Text::errorOfOpenFile(const QString& filePath, const QString& errorMessage) {
	return QStringLiteral("打开文件 %1 失败：\r\n%2").arg(filePath, errorMessage);
}

QString Text::confirmToClearFileList() {
	return QStringLiteral("确认清空文件列表吗？");
}

QString Text::tipOfEmptyDrillPosition() {
	return QStringLiteral("请指定钻孔位置文件。");
}

QString Text::tipOfEmptyDrillPositionShpFilePath() {
	return QStringLiteral("请指定保存钻孔位子的Shape文件的路径。");
}

QString Text::tipOfEmptyDrillLogFiles() {
	return QStringLiteral("请指定钻孔测试记录数据。");
}

QString Text::tipOfEmptyCRS() {
	return QStringLiteral("请指定坐标参照系。");
}

QString Text::tipOfUnknownFieldValueType() {
	return QStringLiteral("请指定属性字段的数据类型。");
}

QString Text::tipOfSucceedInSavingToGeo3DMLFile(const QString& modelName) {
	return QStringLiteral("导出模型“%1”到Geo3DML文件成功。").arg(modelName);
}

QString Text::tipOfErrorInSavingToGeo3DMLFile(const QString& modelName, const QString& error) {
	return QStringLiteral("导出模型“%1”到Geo3DML文件出错：%2").arg(modelName).arg(error);
}
