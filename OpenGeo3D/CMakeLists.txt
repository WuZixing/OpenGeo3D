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
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)
# Qt-Solutions: QtPropertyBrowser
set(QT_SOLUTIONS_QT_PROPERTY_BROWSER_DIR "" CACHE PATH "Root path of project qt-solutions/qtpropertybrowser")
if((NOT QT_SOLUTIONS_QT_PROPERTY_BROWSER_DIR) OR (NOT EXISTS "${QT_SOLUTIONS_QT_PROPERTY_BROWSER_DIR}/src/QtTreePropertyBrowser"))
  message(SEND_ERROR "Can not find headers of Qt widget QtTreePropertyBrowser. Please set QT_SOLUTIONS_QT_PROPERTY_BROWSER_DIR to the root directory of project qt-solutions/qtpropertybrowser, and make sure the project has been built.")
endif()
set(QT_SOLUTIONS_QT_PROPERTY_BROWSER_INCULUDE_DIRS "${QT_SOLUTIONS_QT_PROPERTY_BROWSER_DIR}/src")
list(APPEND QT_SOLUTIONS_QT_PROPERTY_BROWSER_LIBRARIES
  debug ${QT_SOLUTIONS_QT_PROPERTY_BROWSER_DIR}/lib/QtSolutions_PropertyBrowser-headd.lib
  optimized ${QT_SOLUTIONS_QT_PROPERTY_BROWSER_DIR}/lib/QtSolutions_PropertyBrowser-head.lib
)

# VTK
find_package(VTK 9.0  
  COMPONENTS
    CommonCore
    GUISupportQt
  REQUIRED
 )

 include_directories(${QT_SOLUTIONS_QT_PROPERTY_BROWSER_INCULUDE_DIRS})

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

target_link_libraries(${APP_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Widgets ${QT_SOLUTIONS_QT_PROPERTY_BROWSER_LIBRARIES} ${VTK_LIBRARIES})

#qt5_create_translation(QM_FILES ${CMAKE_CURRENT_SOURCE_DIR} OpenGeo3D_zh_CN.ts)
