set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(APP_NAME ${CMAKE_PROJECT_NAME})
set(APP_VERSION "" CACHE STRING "Version: vX.Y.Z")
add_definitions(-DAPP_NAME="${APP_NAME}" -DAPP_VERSION="${APP_VERSION}")
add_compile_options(/utf-8)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Qt6
find_package(Qt6 COMPONENTS Widgets REQUIRED)

# VTK 9.2
find_package(VTK 9.2 CONFIG REQUIRED)

# GDAL
find_package(GDAL 3.6 CONFIG REQUIRED)

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

aux_source_directory(. CPP_SOURCES)

add_executable(${APP_NAME} ${TARGET_PROPERTY}
  ${CPP_SOURCES}
  AppFrame.h
  BusyCursor.h
  ComboBoxItemDelegate.h
  CRSSet.h
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

