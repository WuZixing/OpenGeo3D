# Geo3DML SDK
find_file(GEO3DML_HEADER "geo3dml/ObjectFactory.h" PATHS ${GEO3DML_INCLUDE_DIRS})
if(NOT GEO3DML_HEADER)
	message(SEND_ERROR "Can not find Geo3DML headers. Please set GEO3DML_INCLUDE_DIRS to the directory that contains file geo3dml/ObjectFactory.h.")
endif()

# output variables
set(G3DGRID_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/include CACHE PATH "")
set(G3DGRID_LIBRARIES libG3DGRID CACHE STRING "")

aux_source_directory(. G3DGRID_SOURCES)
list(APPEND G3DGRID_HEADERS
	${PROJECT_SOURCE_DIR}/include/g3dgrid/LOD.h
	${PROJECT_SOURCE_DIR}/include/g3dgrid/Voxel.h
	${PROJECT_SOURCE_DIR}/include/g3dgrid/VoxelGrid.h
)

include_directories(${G3DGRID_INCLUDE_DIRS})
include_directories(${GEO3DML_INCLUDE_DIRS})
add_library(${G3DGRID_LIBRARIES}
	${G3DGRID_SOURCES}
	${G3DGRID_HEADERS}
)