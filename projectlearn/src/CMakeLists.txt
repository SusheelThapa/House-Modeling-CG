find_package( OpenGL REQUIRED )
include_directories( ${OPENGL_INCLUDE_DIRS} )
include_directories(${MyProject_SOURCE_DIR}/projectlearn/include)
include_directories(${MyProject_SOURCE_DIR}/glfw/include)
include_directories(${MyProject_SOURCE_DIR}/assimp/include)
include_directories(${MyProject_SOURCE_DIR}/build/assimp/include) #after build cmake creates a config.h header file in build for assimp
include_directories(${MyProject_SOURCE_DIR}/glm)
include_directories(${MyProject_SOURCE_DIR}/imgui) 

set( ${PROJECT_NAME}-SRC
     main.cpp
     glad.c
)
add_executable( ${PROJECT_NAME}  ${MyProject-SRC} )
if(WIN32)
	target_link_libraries( ${PROJECT_NAME} opengl32.lib glfw glm assimp imgui User32.lib Shell32.lib Gdi32.lib)
endif()
if(UNIX AND NOT APPLE)
	target_link_libraries( ${PROJECT_NAME} glfw glm assimp imgui)
endif()