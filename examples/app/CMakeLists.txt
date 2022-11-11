project (app)

include(${CMAKE_FRAMEWORK_DIR}/project/project.cmake)

# --------------------- Source -----------------------

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/..)

set (SOURCEFILES
  main.cpp
)

set (HEADERFILES
)

# --------------------- Dependencies -----------------------

set(PROJECT_DEPS ${PROJECT_DEPS} sandbox)
set(DEP_LIBS ${DEP_LIBS} sandbox dl)

# --------------------- Executable -----------------------

addStandardExecutable()

