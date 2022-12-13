project (opengl_sandbox)

include(${CMAKE_FRAMEWORK_DIR}/project/project.cmake)

# --------------------- Source -----------------------

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/..)

set (SOURCEFILES
  test.cpp
)

set (HEADERFILES
  opengl_object.h
  shader/opengl_shader.h
  test.h
)

# --------------------- Dependencies -----------------------

link_directories(${CMAKE_FRAMEWORK_DIR}/external/glfw3/build/install/lib)
include_directories(${CMAKE_FRAMEWORK_DIR}/external/glfw3/src/deps)

set(PROJECT_DEPS ${PROJECT_DEPS} sandbox glfw_sandbox glm_sandbox)
set(DEP_LIBS ${DEP_LIBS} sandbox glfw_sandbox glm_sandbox)

# --------------------- Executable -----------------------

addSharedLibrary()

