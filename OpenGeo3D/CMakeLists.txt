set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(APP_NAME ${CMAKE_PROJECT_NAME})
set(APP_VERSION "" CACHE STRING "Version: vX.Y.Z")
add_definitions(-DAPP_NAME="${APP_NAME}" -DAPP_VERSION="${APP_VERSION}")
if(MSVC)
  add_compile_options(/utf-8)
endif()

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Qt6
find_package(Qt6 COMPONENTS Widgets REQUIRED)

# VTK 9.2
find_package(VTK CONFIG REQUIRED)

# GDAL
find_package(GDAL CONFIG REQUIRED)

include_directories(${GEO3DML_INCLUDE_DIRS})
include_directories(${G3DXML_INCLUDE_DIRS})
include_directories(${G3DVTK_INCLUDE_DIRS})
include_directories(${G3DGRID_INCLUDE_DIRS})

if(MSVC)
  set(TARGET_PROPERTY "WIN32")
  set(APP_RC "${CMAKE_CURRENT_SOURCE_DIR}/OpenGeo3D.rc")
else()
  set(TARGET_PROPERTY "")
endif(MSVC)

# get git hash
macro(get_git_hash _git_hash)
    find_package(Git QUIET)     # 查找Git，QUIET静默方式不报错
    if(GIT_FOUND)
      execute_process(          # 执行一个子进程
        COMMAND ${GIT_EXECUTABLE} log -1 --pretty=format:%h # 命令
        OUTPUT_VARIABLE ${_git_hash}        # 输出字符串存入变量
        OUTPUT_STRIP_TRAILING_WHITESPACE    # 删除字符串尾的换行符
        ERROR_QUIET                         # 对执行错误静默
        WORKING_DIRECTORY                   # 执行路径
          ${CMAKE_CURRENT_SOURCE_DIR}
        )
    endif()
endmacro()

# get git branch
macro(get_git_branch _git_branch)
    find_package(Git QUIET)     # 查找Git，QUIET静默方式不报错
    if(GIT_FOUND)
      execute_process(          # 执行一个子进程
        COMMAND ${GIT_EXECUTABLE} symbolic-ref --short -q HEAD # 命令
        OUTPUT_VARIABLE ${_git_branch}      # 输出字符串存入变量
        OUTPUT_STRIP_TRAILING_WHITESPACE    # 删除字符串尾的换行符
        ERROR_QUIET                         # 对执行错误静默
        WORKING_DIRECTORY                   # 执行路径
          ${CMAKE_CURRENT_SOURCE_DIR}
        )
    endif()
endmacro()
# 获取当前的GIT_HASH
set(GIT_HASH "")
get_git_hash(GIT_HASH)
message(STATUS "Git hash is ${GIT_HASH}")
# 获取当前的分支
set(GIT_BRANCH "")
get_git_branch(GIT_BRANCH)
message(STATUS "Git branch is ${GIT_BRANCH}")
string(TIMESTAMP BUILD_TIMESTAMP %Y-%m-%dT%H:%M:%S)
message(STATUS "Build time is ${BUILD_TIMESTAMP}")
configure_file(
  config/gitVersion.h.in  # 输入
  ${CMAKE_CURRENT_SOURCE_DIR}/generated/gitVersion.h # 输出
  @ONLY
)

aux_source_directory(. CPP_SOURCES)

add_executable(${APP_NAME} ${TARGET_PROPERTY}
  ${CPP_SOURCES}
  config/gitVersion.h.in
  AppFrame.h
  BusyCursor.h
  ComboBoxItemDelegate.h
  CRSSet.h
  DataLoaderGeoJSON.h
  DlgAbout.h
  DlgOpenSimpleDrillLog.h
  DrillLogFile.h
  Events.h
  GroupOfSimpleDrillLogFiles.h
  MetadataPage.h
  OpenApp.h
  ProjectMetaBook.h
  ProjectPanel.h
  ProjectTreeCtrl.h
  RenderOptionPage.h
  RenderWidget.h
  Text.h
  icon.xpm
  icon.ico
  QtPropertyBrowser/qtbuttonpropertybrowser.h
  QtPropertyBrowser/qtbuttonpropertybrowser.cpp
  QtPropertyBrowser/qteditorfactory.h
  QtPropertyBrowser/qteditorfactory.cpp
  QtPropertyBrowser/qtgroupboxpropertybrowser.h
  QtPropertyBrowser/qtgroupboxpropertybrowser.cpp
  QtPropertyBrowser/qtpropertybrowser.h
  QtPropertyBrowser/qtpropertybrowser.cpp
  QtPropertyBrowser/qtpropertybrowserutils_p.h
  QtPropertyBrowser/qtpropertybrowserutils.cpp
  QtPropertyBrowser/qtpropertymanager.h
  QtPropertyBrowser/qtpropertymanager.cpp
  QtPropertyBrowser/qttreepropertybrowser.h
  QtPropertyBrowser/qttreepropertybrowser.cpp
  QtPropertyBrowser/qtvariantproperty.h
  QtPropertyBrowser/qtvariantproperty.cpp
  ${APP_RC}
)

target_link_libraries(${APP_NAME} 
  PRIVATE Qt::Widgets ${VTK_LIBRARIES} GDAL::GDAL
  ${GEO3DML_LIBRARIES} ${G3DXML_LIBRARIES} ${G3DVTK_LIBRARIES} ${G3DGRID_LIBRARIES}
)

set_target_properties(${APP_NAME} PROPERTIES
  RESOURCE "${APP_RC}"
)

# Qt resources
file(GLOB QT_PROPERTY_BROWSER_IMAGES LIST_DIRECTORIES FALSE RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" QtPropertyBrowser/images/*.*)
qt_add_resources(${APP_NAME} "qt-property-browser"
  PREFIX "/qt-property-browser/images"
  BASE "QtPropertyBrowser/images"
  FILES ${QT_PROPERTY_BROWSER_IMAGES} 
)

