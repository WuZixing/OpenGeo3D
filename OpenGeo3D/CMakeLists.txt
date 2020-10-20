set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(APP_NAME ${CMAKE_PROJECT_NAME})
set(APP_VERSION "" CACHE STRING "Version")
add_definitions(-DAPP_NAME="${APP_NAME}" -DAPP_VERSION="${APP_VERSION}")

# Qt
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
#find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets LinguistTools REQUIRED)
set(QT_VERSION_MAJOR 5)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets LinguistTools REQUIRED)

# VTK
find_package(VTK 9.0  
  COMPONENTS
    CommonCore
    GUISupportQt
  REQUIRED
 )

if(MSVC)
  set(TARGET_PROPERTY "WIN32")
else()
  set(TARGET_PROPERTY "")
endif(MSVC)
aux_source_directory(. CPP_SOURCES)
add_executable(${APP_NAME} ${TARGET_PROPERTY}
  ${CPP_SOURCES}
  MainWindow.h
  Strings.h
)

target_link_libraries(${APP_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Widgets ${VTK_LIBRARIES})

#qt5_create_translation(QM_FILES ${CMAKE_CURRENT_SOURCE_DIR} OpenGeo3D_zh_CN.ts)
