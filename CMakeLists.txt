# Setup the project
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(mvp_project)

# Find Geant4 package, activating all available UI and Vis drivers by default
find_package(Geant4 REQUIRED ui_all vis_all)

# Setup Geant4 include directories and compile definitions
include(${Geant4_USE_FILE})
include_directories(${PROJECT_SOURCE_DIR}/include)

# Locate sources and headers for this project
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

# Add the executable, and link it to the Geant4 libraries
add_executable(minimal minimal.cc ${sources} ${headers})
target_link_libraries(minimal ${Geant4_LIBRARIES})

file(COPY ${PROJECT_SOURCE_DIR}/vis.mac DESTINATION ${PROJECT_BINARY_DIR})
