# OpenGeo3D

* [简介](#简介)
* [功能规划](#功能规划)
	* [网格类型](#网格类型)
	* [建模原始数据](#建模原始数据)
	* [建模方法](#建模方法)
	* [网格模型分析](#网格模型分析)
	* [三维地质模型的可视化](#三维地质模型的可视化)
* [编译和运行](#编译和运行)


## 简介

**OpenGeo3D**是一个基于网格的三维地质建模工具。它将地质空间划分为网格单元，通过识别、预测网格单元的地质属性完成地质建模。该模型对应于传统的地质属性模型。在此基础上可继续提取地质界面，识别构造单元和地质概念，形成地质构造模型（或结构模型）。

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

拟以开源项目[VTK](https://vtk.org/)为基础提供三维地质模型（结构模型和属性模型）的三维可视化。


## 编译和运行


