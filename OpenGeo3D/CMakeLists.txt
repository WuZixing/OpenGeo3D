set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(APP_NAME ${CMAKE_PROJECT_NAME})
set(APP_VERSION "" CACHE STRING "Version: vX.Y.Z")
add_definitions(-DAPP_NAME="${APP_NAME}" -DAPP_VERSION="${APP_VERSION}")
add_compile_options(/utf-8)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Qt5
#find_package(Qt5 COMPONENTS Widgets LinguistTools REQUIRED)
find_package(Qt5 COMPONENTS Widgets REQUIRED)

# QtPropertyBrowser
set(QtPropertyBrowser_INCLUDE_DIRS "" CACHE PATH "Qt-Solutions/QtPropertyBrowser include directory.")
set(QtPropertyBrowser_LIBRARY_DEBUG "" CACHE FILEPATH "Qt-Solutions/QtPropertyBrowser DEBUG library file.")
set(QtPropertyBrowser_LIBRARY_NODEBUG "" CACHE FILEPATH "Qt-Solutions/QtPropertyBrowser NODEBUG library file.")
find_file(QTPROPERTYBROWSER_HEADER "qttreepropertybrowser.h" PATHS ${QtPropertyBrowser_INCLUDE_DIRS})
if(NOT QTPROPERTYBROWSER_HEADER)
  message(SEND_ERROR "Can not find QtTreePropertyBrowser headers. Please set QtPropertyBrowser_INCLUDE_DIRS to the directory that contains file qttreepropertybrowser.h.")
endif()
if(NOT QtPropertyBrowser_LIBRARY_DEBUG)
  message(SEND_ERROR "Can not find QtTreePropertyBrowser library. Please set QtPropertyBrowser_LIBRARY_DEBUG to the DEBUG library file of QtPropertyBrowser.")
endif()
if(NOT QtPropertyBrowser_LIBRARY_NODEBUG)
  message(SEND_ERROR "Can not find QtTreePropertyBrowser library. Please set QtPropertyBrowser_LIBRARY_NODEBUG to the NODEBUG library file of QtPropertyBrowser.")
endif()
add_definitions(-DQT_QTPROPERTYBROWSER_IMPORT)
include_directories(${QtPropertyBrowser_INCLUDE_DIRS})

# VTK
find_package(VTK 9.0 REQUIRED)

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

#set(TS_FILES OpenApp_zh_CN.ts)
#list(APPEND UI_Files
#  DlgAbout.ui
#)
#qt5_wrap_ui(UI_SOURCES ${UI_Files})
aux_source_directory(. CPP_SOURCES)

add_executable(${APP_NAME} ${TARGET_PROPERTY}
  ${CPP_SOURCES}
  AppFrame.h
  BusyCursor.h
  ComboBoxItemDelegate.h
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
  ${APP_RC}
  #${UI_SOURCES}
  #${UI_Files}
  #${TS_FILES}
)
#qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})

target_link_libraries(${APP_NAME} 
  PRIVATE Qt5::Widgets ${VTK_LIBRARIES} 
  ${GEO3DML_LIBRARIES} ${G3DXML_LIBRARIES} ${G3DVTK_LIBRARIES} ${G3DGRID_LIBRARIES}
  debug ${QtPropertyBrowser_LIBRARY_DEBUG} optimized ${QtPropertyBrowser_LIBRARY_NODEBUG}
)
vtk_module_autoinit(TARGETS ${APP_NAME} MODULES ${VTK_LIBRARIES})
