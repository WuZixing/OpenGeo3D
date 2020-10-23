set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(APP_NAME ${CMAKE_PROJECT_NAME})
set(APP_VERSION "" CACHE STRING "Version: vX.Y.Z")
add_definitions(-DAPP_NAME="${APP_NAME}" -DAPP_VERSION="${APP_VERSION}")

# wxWidgets
find_package(wxWidgets 3.1 REQUIRED COMPONENTS core base adv propgrid aui)
include(${wxWidgets_USE_FILE})

# VTK
find_package(VTK 9.0 REQUIRED)

include_directories(${GEO3DML_INCLUDE_DIRS})
include_directories(${G3DXML_INCLUDE_DIRS})
include_directories(${G3DVTK_INCLUDE_DIRS})

if(MSVC)
  set(TARGET_PROPERTY "WIN32")
else()
  set(TARGET_PROPERTY "")
endif(MSVC)
aux_source_directory(. CPP_SOURCES)
add_executable(${APP_NAME} ${TARGET_PROPERTY}
  ${CPP_SOURCES}
  Config.h
  Frame.h
  OpenGeo3D.h
  ScenePanel.h
  Strings.h
  wxWidgets.h
  wxVTKRenderWindowInteractor.h
)

target_link_libraries(${APP_NAME} 
  PRIVATE ${wxWidgets_LIBRARIES} ${VTK_LIBRARIES} ${GEO3DML_LIBRARIES} ${G3DXML_LIBRARIES} ${G3DVTK_LIBRARIES}
)
vtk_module_autoinit(TARGETS ${APP_NAME} MODULES ${VTK_LIBRARIES})
