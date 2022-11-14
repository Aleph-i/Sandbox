project (glfw_sandbox)

include(${CMAKE_FRAMEWORK_DIR}/project/project.cmake)

# --------------------- Source -----------------------

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/..)

set (SOURCEFILES
  test.cpp
)

set (HEADERFILES
  test.h
)

# --------------------- Dependencies -----------------------

set(glfwArgs -DCMAKE_INSTALL_PREFIX=${glfw_INSTALL_DIR} -DUSE_MSVC_RUNTIME_LIBRARY_DLL=ON -DGLFW_BUILD_DOCS=OFF -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}  -DCMAKE_DEBUG_POSTFIX=d)
add_external(glfw3
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  CMAKE_ARGS glfwArgs
  LIB_NAME glfw3
  DEBUG_POSTFIX d
)
include_directories(${CMAKE_FRAMEWORK_DIR}/external/glfw3/build/install/include)
include_directories(${CMAKE_FRAMEWORK_DIR}/external/glfw3/src/deps)

set(PROJECT_DEPS ${PROJECT_DEPS} sandbox)
set(DEP_LIBS ${DEP_LIBS} sandbox pthread)

# --------------------- Executable -----------------------

addSharedLibrary()

