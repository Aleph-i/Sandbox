project (sandbox)

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



# --------------------- Executable -----------------------

addStandardLibrary()

