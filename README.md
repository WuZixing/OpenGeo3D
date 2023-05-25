# OpenGeo3D

+ [简介](#简介)
+ [功能规划](#功能规划)
	- [网格类型](#网格类型)
	- [建模原始数据](#建模原始数据)
	- [建模方法](#建模方法)
	- [网格模型分析](#网格模型分析)
	- [三维地质模型的可视化](#三维地质模型的可视化)
+ [编译和运行](#编译和运行)
	- [运行环境](#运行环境)
	- [依赖项](#依赖项)
	- [开发环境配置](#开发环境配置)
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

### 运行环境

**OpenGeo3D**支持跨Windows、Linux和MacOS操作系统编译和运行，但目前仅在Windows平台上执行了测试。

### 依赖项

本项目使用的第三方库有：

+ [Qt](https://www.qt.io/)

	用户界面。使用版本：v6.2 (LTS)。

+ [VTK](https://vtk.org/)

	数据可视化。建议版本：v9.2或更新版本。

+ [Geo3DML-CPP](https://github.com/WuZixing/Geo3DML-CPP)

	读写Geo3DML数据。建议版本：master分支最新版本。

+ [GDAL](https://gdal.org/)

    GIS数据文件读写。使用版本：3.6.3。

编译**OpenGeo3D**之前，请先准备好**Qt**和**VTK**开发包（包括头文件和库文件）。可参照各项目的文档自行编译，或直接安装预编译的开发包。

### 开发环境配置

以Windows操作系统为例。

(1) [Git](https://git-scm.com/)

  下载和安装最新版: [https://git-scm.com/download/win](https://git-scm.com/download/win)。安装时选择将安装路径添加到系统环境变量中。

(2) [CMake](https://cmake.org/)

  下载和安装最新版本：[https://cmake.org/download/](https://cmake.org/download/)。安装时选择将安装路径添加到系统环境变量中。

(3) [vcpkg](https://github.com/microsoft/vcpkg)

  该工具用于配置第三方开发库。

  + 先安装[Microsoft Visual Studio](https://visualstudio.microsoft.com/)。要求Visual Studio 2015 Update 3或更新版本（建议Visual Studio Community 2022），并且使用英语语言包。
  安装Qt扩展工具：`Qt VS Tools`。从Visual Studio的`Extensions`-`Manager Extension`菜单安装，或者从Qt官方安装。Qt官网下载地址：[https://download.qt.io/official_releases/vsaddin/2.9.1/](https://download.qt.io/official_releases/vsaddin/2.9.1/)。

  + 参照[官方说明](https://github.com/microsoft/vcpkg#quick-start-windows)安装`vcpkg`。

    ```console
    > git clone https://github.com/microsoft/vcpkg
    > .\vcpkg\bootstrap-vcpkg.bat
    ```

  + 对于64位开发环境，可在操作系统中添加名为`VCPKG_DEFAULT_TRIPLET`的环境变量，其值为`x64-windows`。使得`vcpkg`缺省配置64位的开发库。

  + `vcpkg`会使用已安装的最新版的Visual Studio来配置开发库。

(4) [Qt SDK](https://www.qt.io/)

  + 下载在线安装程序：[qt-unified-windows-online-installer](https://download.qt.io/official_releases/online_installers/qt-unified-windows-x64-online.exe)

  + 指定Qt仓库镜像安装SDK

    ```console
    # 命令行执行命令，选择清华大学镜像服务器。
    > qt-unified-windows-x64-online.exe --mirror https://mirrors.tuna.tsinghua.edu.cn/qt
    ```
	
    自定义安装，选择LTS版本过滤条件，安装6.2版本及Creator、Designer等开发工具，并且去掉MINGWIN开发包及源码等占用很大磁盘空间的选。

(5) [GDAL](https://gdal.org/)

  通过`vcpkg`安装`GDAL`开发库。

  ```console
  > cd vcpkg/
  > ./vcpkg.exe install --clean-buildtrees-after-build gdal
  ```

  安装时，对于下载失败的依赖项，可根据展示的URL自行下载后保存至 `vkpgk/downloads/` 目录下。注意保存的文件名需与`vcpkg`日志中展示的文件名一致。来自github.com的依赖项下载失败时，可以尝试通过网站[https://ghproxy.com](https://ghproxy.com)手动下载。

(6) [Geo3DML-CPP](https://github.com/WuZixing/Geo3DML-CPP)

  该库是通过子模块嵌入本项目中的，无需额外配置。

(7) [VTK](https://vtk.org/)

  通过`vcpkg`能安装`VTK`开发库，但不能使用用于指定版本的`Qt`库。故而仍旧使用下载源码自己编译的方式配置`VTK`。

  - 下载源码编译安装

    (1) 源码下载地址：[https://vtk.org/download/](https://vtk.org/download/)。当前最新版本为v9.2.6：[VTK-9.2.6.tar.gz](https://www.vtk.org/files/release/9.2/VTK-9.2.6.tar.gz)。

    (2) 使用**CMake**配置编译参数。注意选择`VTK_GROUP_ENABLE_Qt`，之后再配置`VTK_QT_VERSION`为6。按提示配置`Qt`参数后生成VS Solution，并编译生成开发库。

    (3) 完成编译后，以管理员权限运行Visual Studio，然后对`INSTALL`项目运行`Build`，将编译好的库安装至系统目录（缺省为：C/Program Files/VTK）。

  - 备注：通过`vcpkg`安装

    ```console
    > # vcpkg 安装 VTK 的指令。注意：需额外安装 vtk[qt]
    > cd vcpkg/
    > ./vcpkg.exe install --clean-buildtrees-after-build vtk
    > ./vcpkg.exe install --clean-buildtrees-after-build --recurse vtk[qt]
    ```

### 编译

```shell
# 下载代码
# 从github
git clone https://github.com/WuZixing/OpenGeo3D.git
# 从gitee
git clone https://gitee.com/zhizhi-link/OpenGeo3D.git

# 更新子模块
cd OpenGeo3D
git submodule update --init --recursive
```

之后请使用**CMake**配置编译参数（要选择`vcpkg`为交叉编译工具），执行编译。

### 部署

(1) 部署`Qt`依赖项

```console
> ./Qt/bin/windeployqt.exe --[debug|release] --translations en,zh_CN </path/to/the/directory/of/OpenGeo3D.exe>
```

(2) 运行`OpenGeo3D`可执行程序，按提示缺少的`VTK`动态库拷贝对应的DLL文件到程序目录。
