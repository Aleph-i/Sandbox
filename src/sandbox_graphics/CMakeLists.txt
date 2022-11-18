project (sandbox_graphics)

include(${CMAKE_FRAMEWORK_DIR}/project/project.cmake)

# --------------------- Source -----------------------

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/..)

set (SOURCEFILES
  renderer.cpp
)

set (HEADERFILES
  renderer.h
)

# --------------------- Dependencies -----------------------

set(PROJECT_DEPS ${PROJECT_DEPS} sandbox)
set(DEP_LIBS ${DEP_LIBS} sandbox)

# --------------------- Executable -----------------------

addStandardLibrary()

