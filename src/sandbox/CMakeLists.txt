project (sandbox)

include(${CMAKE_FRAMEWORK_DIR}/project/project.cmake)

# --------------------- Source -----------------------

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/..)

set (SOURCEFILES
  entity.cpp
  component.cpp
  plugin/plugin_manager.cpp
  plugin/plugin.cpp
  plugin/plugin_interface.cpp
)

set (HEADERFILES
  entity.h
  component.h
  plugin/plugin_manager.h
  plugin/plugin.h
  plugin/plugin_interface.h
)

# --------------------- Dependencies -----------------------



# --------------------- Executable -----------------------

addStandardLibrary()

