project (tests)

include(${CMAKE_FRAMEWORK_DIR}/project/project.cmake)

# --------------------- Source -----------------------

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/..)

set (SOURCEFILES
  entity_component.cpp
)

set (HEADERFILES
)

# --------------------- Dependencies -----------------------

add_external(gtest
  GIT_REPOSITORY https://github.com/google/googletest.git
  LIB_NAME gtest
  GIT_TAG main
)

set(PROJECT_DEPS ${PROJECT_DEPS} sandbox)
set(DEP_LIBS ${DEP_LIBS} sandbox gtest_main pthread dl)

# --------------------- Executable -----------------------

addStandardExecutable()

