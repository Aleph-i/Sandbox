project (stb_sandbox)

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

add_external(stb
  GIT_REPOSITORY https://github.com/nothings/stb.git
  HEADER_ONLY
)
include_directories(${external_dir}/stb/src)

set(PROJECT_DEPS ${PROJECT_DEPS} sandbox)
set(DEP_LIBS ${DEP_LIBS} sandbox)

# --------------------- Executable -----------------------

addSharedLibrary()

