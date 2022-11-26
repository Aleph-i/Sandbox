project (cppws_sandbox)

include(${CMAKE_FRAMEWORK_DIR}/project/project.cmake)

# --------------------- Source -----------------------

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/..)

set (SOURCEFILES
  main.cpp
)

set (HEADERFILES
)

# --------------------- Dependencies -----------------------

add_external(cppws
  GIT_REPOSITORY https://github.com/dtorban/CppWebServer.git
  LIB_NAME CppWebServer
)

set(DEP_INCLUDES ${DEP_INCLUDES} ${external_dir}/CppWebServerLib/src/src)
set(DEP_INCLUDES ${DEP_INCLUDES} ${external_dir}/CppWebServerLib/src/cmake/framework/external/WebSockets/build/install/include)
set(DEP_LINK_DIRS ${DEP_LINK_DIRS} 
  ${external_dir}/CppWebServerLib/src/cmake/framework/external/WebSockets/build/install/lib
)
set(DEP_INCLUDES ${DEP_INCLUDES} 
	${external_dir}/CppWebServerLib/src/cmake/framework/external/picojson/src
)
set(ALL_LIBS ${ALL_LIBS} websockets crypto ssl z)

set(PROJECT_DEPS ${PROJECT_DEPS} sandbox picojson_sandbox)
set(DEP_LIBS ${DEP_LIBS} sandbox picojson_sandbox)

# --------------------- Executable -----------------------

addSharedLibrary()

