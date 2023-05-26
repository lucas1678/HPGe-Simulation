##REAL ONEEEE####
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)

project(BariumSimulation)

#####
#[[set(G4CMP_BASEDIR "/home/lbackes/G4CMP-install")
set(G4CMP_LIBDIR "${G4CMP_BASEDIR}/lib")

find_library(G4CMP_LIBRARY NAMES G4cmp HINTS ${G4CMP_LIBDIR})
find_library(QHCPP_LIBRARY NAMES qhullcpp HINTS ${G4CMP_LIBDIR})
find_library(QH_P_LIBRARY NAMES qhull_p HINTS ${G4CMP_LIBDIR})

# Set a bunch of useful variables.
set(G4CMP_INCLUDE_DIRS "${G4CMP_BASEDIR}/include/G4CMP")
set(G4CMP_LIBRARIES ${G4CMP_LIBRARY} ${QHCPP_LIBRARY} ${QH_P_LIBRARY})
set(G4CMP_CXX_FLAGS "-std=c++11")

# Users of G4CMP should add "include(${G4CMP_USE_FILE})" to CMakeLists.txt
set(G4CMP_USE_FILE ${G4CMP_BASEDIR}/cmake/UseG4CMP.cmake)]]
#####


set(CMAKE_PREFIX_PATH $ENV{G4CMPINSTALL}/../..)
find_package(G4CMP)
find_package(Geant4 REQUIRED ui_all vis_all)
include(${G4CMP_USE_FILE})
include(${Geant4_USE_FILE})

include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${G4CMP_INCLUDE_DIRS})

set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
# Set the RPATH of the installed objects *unless* they are installing to a 
# system directory, in which case they shouldn't need an explicitly set RPATH.
list(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES 
     "/home/lbackes/G4CMP-install/lib" is_SYSTEM_DIR)
set(CMAKE_INSTALL_RPATH "/home/lbackes/G4CMP-install/lib")


file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

add_executable(sim sim.cc ${sources} ${headers})
target_link_libraries(sim ${G4CMP_LIBRARIES} ${Geant4_LIBRARIES})

add_custom_target(BariumSimulation DEPENDS sim)
