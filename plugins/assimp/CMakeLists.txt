project (assimp_sandbox)

include(${CMAKE_FRAMEWORK_DIR}/project/project.cmake)

# --------------------- Source -----------------------

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/..)

set (SOURCEFILES
  main.cpp
)

set (HEADERFILES
)

# --------------------- Dependencies -----------------------

include(${external_dir}/Assimp/Assimp.cmake)
useAssimp()

link_directories(${CMAKE_FRAMEWORK_DIR}/external/assimp/build/install/lib)
#include_directories(${CMAKE_FRAMEWORK_DIR}/external/glfw3/src/deps)

set(PROJECT_DEPS ${PROJECT_DEPS} sandbox)
set(DEP_LIBS ${DEP_LIBS} sandbox)

# --------------------- Executable -----------------------

addSharedLibrary()

