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
  DlgAbout.h
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
  PRIVATE Qt5::Widgets ${VTK_LIBRARIES} ${GEO3DML_LIBRARIES} ${G3DXML_LIBRARIES} ${G3DVTK_LIBRARIES} ${G3DGRID_LIBRARIES}
)
vtk_module_autoinit(TARGETS ${APP_NAME} MODULES ${VTK_LIBRARIES})
