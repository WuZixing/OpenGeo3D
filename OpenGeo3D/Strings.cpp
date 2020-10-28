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

wxString Strings::TitleOfMenuItemOpenGeo3DML() {
	return wxString::FromUTF8("打开Geo3DML文件...");
}

wxString Strings::TitleOfMenuItemOpenSGeMSGrid() {
	return wxString::FromUTF8("打开SGeMS网格数据...");
}

wxString Strings::TitleOfMenuItemQuit() {
	return wxString::FromUTF8("退出(&Q)");
}

wxString Strings::TitleOfMenuItemAbout() {
	return wxString::FromUTF8("关于(&A)");
}

wxString Strings::TitleOfMenuItemFullView() {
	return wxString::FromUTF8("全景");
}

wxString Strings::ConfirmToQuit() {
	return wxString::Format(wxString::FromUTF8("确定退出%s吗？"), Strings::AppName());
}

wxString Strings::TitleOfProjectPanel() {
	return wxString::FromUTF8("模型");
}

wxString Strings::TitleOfDataInfo() {
	return wxString::FromUTF8("数据");
}

wxString Strings::TitleOfVisInfo() {
	return wxString::FromUTF8("可视化");
}

wxString Strings::NameOfDefaultG3DProject() {
	return wxString::FromUTF8("默认Geo3DML工程");
}

wxString Strings::NameOfDefaultG3DMap() {
	return wxString::FromUTF8("默认地质图");
}

wxString Strings::NameOfStructureModel() {
	return wxString::FromUTF8("结构模型");
}
wxString Strings::NameOfGridModel() {
	return wxString::FromUTF8("网格模型");
}

wxString Strings::NameOfClassG3DMap() {
	return wxString::FromUTF8("地质图/模型");
}
wxString Strings::NameOfClassG3DLayer() {
	return wxString::FromUTF8("图层/要素类");
}

wxString Strings::NameOfClassG3DActor() {
	return wxString::FromUTF8("图元/要素");
}

wxString Strings::NameOfClassG3DPoint() {
	return wxString::FromUTF8("Point");
}

wxString Strings::NameOfClassG3DMPoint() {
	return wxString::FromUTF8("Multiple Point");
}

wxString Strings::NameOfClassG3DAnnotation() {
	return wxString::FromUTF8("Annotation");
}

wxString Strings::NameOfClassG3DLineString() {
	return wxString::FromUTF8("Line String");
}

wxString Strings::NameOfClassG3DTIN() {
	return wxString::FromUTF8("TIN");
}

wxString Strings::NameOfClassG3DUniformGrid() {
	return wxString::FromUTF8("Uniform Grid");
}

wxString Strings::NameOfClassG3DCornerGrid() {
	return wxString::FromUTF8("Corner Grid");
}

wxString Strings::NameOfClassUnknown() {
	return wxString::FromUTF8("Unknown");
}

wxString Strings::NameOfRenderRepresentationModePoint() {
	return wxString::FromUTF8("顶点");
}

wxString Strings::NameOfRenderRepresentationModeWireframe() {
	return wxString::FromUTF8("线框");
}

wxString Strings::NameOfRenderRepresentationModeSurface() {
	return wxString::FromUTF8("面");
}

wxString Strings::TipOfOpenGeo3DML() {
	return wxString::FromUTF8("打开Geo3DML文件(UTF-8编码)");
}

wxString Strings::WildcardOfOpenGeo3DML() {
	return wxString::FromUTF8("Geo3DML文件(UTF-8编码)(*.xml)|*.xml");
}

wxString Strings::MetadataCategoryBasicInfo() {
	return wxString::FromUTF8("基本信息");
}

wxString Strings::MetadataEntryDatasetClassName() {
	return wxString::FromUTF8("数据集");
}

wxString Strings::MetadataEntryId() {
	return wxString::FromUTF8("ID");
}

wxString Strings::MetadataEntryName() {
	return wxString::FromUTF8("名字");
}

wxString Strings::MetadataEntryChildrenNumber() {
	return wxString::FromUTF8("子节点数");
}

wxString Strings::MetadataCategoryMBR() {
	return wxString::FromUTF8("最小外包矩形");
}

wxString Strings::MetadataEntryMBRRangeX() {
	return wxString::FromUTF8("X范围");
}

wxString Strings::MetadataEntryMBRRangeY() {
	return wxString::FromUTF8("Y范围");
}

wxString Strings::MetadataEntryMBRRangeZ() {
	return wxString::FromUTF8("Z范围");
}

wxString Strings::MetadataCategoryFeatureClass() {
	return wxString::FromUTF8("要素类");
}

wxString Strings::MetadataCategorySchema() {
	return wxString::FromUTF8("属性表结构");
}

wxString Strings::MetadataCategoryFeature() {
	return wxString::FromUTF8("要素");
}

wxString Strings::MetadataCategoryGeometry() {
	return wxString::FromUTF8("几何对象");
}

wxString Strings::MetadataCategoryShapeProperty() {
	return wxString::FromUTF8("属性场");
}

wxString Strings::MetadataEntryClassName() {
	return wxString::FromUTF8("数据类型");
}

wxString Strings::MetadataEntryGeometryLODLevel() {
	return wxString::FromUTF8("LOD");
}

wxString Strings::MetadataEntryNumberOfVertices() {
	return wxString::FromUTF8("顶点数");
}

wxString Strings::MetadataEntryNumberOfTriangles() {
	return wxString::FromUTF8("三角形数");
}

wxString Strings::MetadataEntryGridOrigin() {
	return wxString::FromUTF8("网格原点");
}

wxString Strings::MetadataEntryGridCellSize() {
	return wxString::FromUTF8("网格单元大小");
}

wxString Strings::MetadataEntryGridCellDimension() {
	return wxString::FromUTF8("网格单元维度");
}

wxString Strings::RenderCategoryObjectOption() {
	return wxString::FromUTF8("绘制参数");
}

wxString Strings::RenderCategoryShapePropertyOption() {
	return wxString::FromUTF8("属性场绘制参数");
}

wxString Strings::RenderEntryColor() {
	return wxString::FromUTF8("颜色");
}

wxString Strings::RenderEntryTransparency() {
	return wxString::FromUTF8("透明度");
}

wxString Strings::RenderEntryRepresentationMode() {
	return wxString::FromUTF8("绘制方式");
}

wxString Strings::RenderEntryDrawVertex() {
	return wxString::FromUTF8("绘制顶点");
}
wxString Strings::RenderEntryDrawWireframe() {
	return wxString::FromUTF8("绘制线框");
}