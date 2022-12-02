project (glm_sandbox)

include(${CMAKE_FRAMEWORK_DIR}/project/project.cmake)

# --------------------- Source -----------------------

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/..)

set (SOURCEFILES
  main.cpp
)

set (HEADERFILES
)

# --------------------- Dependencies -----------------------

add_external(glm 
	GIT_REPOSITORY https://github.com/g-truc/glm.git
)
add_definitions(-DGLM_FORCE_INLINE)

set(PROJECT_DEPS ${PROJECT_DEPS} sandbox sandbox_graphics)
set(DEP_LIBS ${DEP_LIBS} sandbox sandbox_graphics)

# --------------------- Executable -----------------------

addSharedLibrary()

