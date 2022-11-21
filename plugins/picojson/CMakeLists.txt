project (picojson_sandbox)

include(${CMAKE_FRAMEWORK_DIR}/project/project.cmake)

# --------------------- Source -----------------------

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/..)

set (SOURCEFILES
  main.cpp
)

set (HEADERFILES
)

# --------------------- Dependencies -----------------------

add_external(picojson
	GIT_REPOSITORY https://github.com/kazuho/picojson.git
	HEADER_ONLY
)
set(DEP_INCLUDES ${DEP_INCLUDES} ${external_dir}/picojson/src)

set(PROJECT_DEPS ${PROJECT_DEPS} sandbox)
set(DEP_LIBS ${DEP_LIBS} sandbox)

# --------------------- Executable -----------------------

addSharedLibrary()

