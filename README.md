# OpenGeo3D

+ [简介](#简介)
+ [功能规划](#功能规划)
	- [网格类型](#网格类型)
	- [建模原始数据](#建模原始数据)
	- [建模方法](#建模方法)
	- [网格模型分析](#网格模型分析)
	- [三维地质模型的可视化](#三维地质模型的可视化)
+ [编译和运行](#编译和运行)
	- [编译和运行环境](#编译和运行环境)
	- [依赖项](#依赖项)
	- [编译](#编译)


## 简介

**OpenGeo3D**是一个基于网格的三维地质建模工具。它将地质空间划分为网格单元，通过识别、预测网格单元的地质属性完成地质建模。该模型对应于传统的地质属性模型。在此基础上可提取地质界面，识别构造单元和地质概念，形成地质构造模型（或结构模型），并为数值分析提供基础。

## 功能规划

### 网格类型

目前仅支持长方体（包括立方体）网格。

### 建模原始数据

|#|数据类型|来源|规划周期|
|:---|:---|:---|:---:|
|1|散点数据|来自钻孔或其它调查、测试工作。|近期|
|2|三维地质结构模型|通过其它工具构建的已有三维地质结构模型（[Geo3DML](https://github.com/WuZixing/Geo3DML)格式）。|近期|
|3|三维地质属性模型|通过其它工具构建的已有三维地质属性模型（如[SGeMS](http://sgems.sourceforge.net/)生成的网格模型等）。|近期|
|4|剖面图|已有地质剖面图。|中期|
|5|平面地质图|已有的平面地质图。|中期|
|6|地球物理|来自地球物理勘探工作手段。|远期|

### 建模方法

|#|方法|说明|规划周期|
|:---|:---|:---|:---:|
|1|地质统计学|支持经典地质统计学方法：Kriging，序贯模拟（基于开源项目[GsTL](http://gstl.sourceforge.net/)）。|近期|
|2|多点地质统计学|多点地质统计学方法。|中期|
|3|机器学习|采用机器学习方法预测网格单元的地质属性。|远期|

### 网格模型分析

|#|功能|说明|规划周期|
|:---|:---|:---|:---:|
|1|模型验证|通过已知资料（如钻孔、剖面图等）验证被预测的网格的属性。|近期|
|2|提取地质界面|通过计算等值面来提取地质界面。|近期|
|3|模型不确定性分析|综合分析模型的质量，为改进模型和优化勘探工作部署提供参考建议。|中期|
|4|分析地质构造模式|以网格模型为基础分析地质构造模式。|远期|

### 三维地质模型的可视化

利用开源项目[VTK](https://vtk.org/)实现三维地质模型（结构模型和属性模型）的可视化。


## 编译和运行

### 编译和运行环境

**OpenGeo3D**支持跨Windows、Linux和MacOS操作系统编译和运行，但目前仅在Windows平台上执行了测试。

### 依赖项

本项目使用的第三方库有：

+ [wxWidgets](https://wxwidgets.org/)

	用户界面。建议版本：v3.1.4或更新版本。

+ [VTK](https://vtk.org/)

	数据可视化。建议版本：v9.0或更新版本。

+ [Geo3DML-CPP](https://github.com/WuZixing/Geo3DML-CPP)

	读写Geo3DML数据。建议版本：master分支最新版本。

+ [harshn05/wxvtk-UpdatedOnJan23_2018](https://github.com/harshn05/wxvtk-UpdatedOnJan23_2018)

	数据可视化。版本：master分支最新版本。

编译**OpenGeo3D**之前，请先准备好**wxWidgets**和**VTK**开发包（包括头文件和库文件）。可参照各项目的文档自行编译，或直接安装预编译的开发包。

**harshn05/wxvtk-UpdatedOnJan23_2018**已通过源码嵌入本项目中，无需额外引用。

**Geo3DML-CPP**则通过子模块嵌入本项目中，无需提前编译。

### 编译

```shell
# 下载代码
git clone https://github.com/WuZixing/OpenGeo3D.git

# 更新子模块
cd OpenGeo3D
git submodule update --init --recursive
```

之后请使用**CMake**配置编译参数，并执行编译。