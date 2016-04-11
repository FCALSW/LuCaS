#----------------------------------------------------------------------------
# $Id$
# Setup the project
#+
CMAKE_MINIMUM_REQUIRED(VERSION 2.8.3 FATAL_ERROR)

# project name
#+
PROJECT(Lucas)

# project version
SET( ${PROJECT_NAME}_VERSION_MAJOR 2 )
SET( ${PROJECT_NAME}_VERSION_MINOR 0 )
SET( ${PROJECT_NAME}_VERSION_PATCH 0 )
SET( PROJECT_VERSION  ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_PATCH} )


# require proper c++
#+

ADD_DEFINITIONS( "-Wall -ansi -pedantic -std=c++11 " )
ADD_DEFINITIONS( "-Wno-long-long" )

# find Lucas cmake modules
#+
SET( CMAKE_MODULE_PATH  ${PROJECT_SOURCE_DIR}/cmake/modules ${CMAKE_MODULE_path} )

# set default installation destination
#+
IF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
   SET(CMAKE_INSTALL_PREFIX $ENV{HOME}/${PROJECT_NAME}/${PROJECT_VERSION} CACHE PATH "Lucas install prefix" FORCE)
ENDIF()
# Project options
#+
OPTION(LUCAS_ROOT_DICTIONARY "Build root dictionary needed to analyze Lucas ROOT file output" ON)
OPTION(LUCAS_LCSD_DEBUG "Toggle ON/OFF debug for LuCaS LCSensitiveDetector " OFF)
IF( LUCAS_LCSD_DEBUG )
  ADD_DEFINITIONS( -DLCSD_DEBUG )
ENDIF( LUCAS_LCSD_DEBUG )

# Find Geant4 package, activating all available UI and Vis drivers by default
#+
OPTION(GEANT4_UIVIS_ALL "Build  with all Geant4 UI and Vis drivers" OFF)
OPTION(GEANT4_UIVIS_TCSH "Build with Geant4 UI-tcsh and Vis-opengl only" ON)
# debug option
#+
OPTION(GEANT4_CONFIG_DEBUG "Turn on extra printout for debug purposes" OFF)
if( GEANT4_CONFIG_DEBUG )   
    set( Geant4_CONFIG_DEBUG 1)
endif()

# You can set GEANT4_UIVIS to OFF via the command line or ccmake/cmake-gui
# to build a batch mode only executable
#+
if(GEANT4_UIVIS_ALL)
  FIND_PACKAGE(Geant4 9.6.1 REQUIRED ui_all vis_all gdml )
else()
   if(GEANT4_UIVIS_TCSH)
      FIND_PACKAGE(Geant4 9.6.1 REQUIRED ui_tcsh vis_opengl_x11 gdml)
   else()
      FIND_PACKAGE(Geant4 9.6.1 REQUIRED gdml)
   endif()
endif()

if( GEANT4_CONFIG_DEBUG )   
    message( STATUS " ************** Geant4_USE_FILE ${Geant4_USE_FILE}")	
    if( Geant4_gdml_FOUND )
	message(STATUS " XERCESC library required ")
	if( Geant4_LIBDEPS_LOADED )
	    message(STATUS " Libdeps loaded ")
	endif() 
    endif()
endif()
#----------------------------------------------------------------------------
# Setup Geant4 include directories and compile definitions
#
if ( GEANT4_FOUND )
  INCLUDE( ${Geant4_USE_FILE} )
else()
  MESSAGE( FATAL_ERROR " Geant4 not found !!!! ")
endif()

# find ROOT ( required package )
#+
IF(NOT IS_DIRECTORY ${ROOT_DIR} )
 IF( IS_DIRECTORY $ENV{ROOTSYS} )
   SET( ROOT_DIR $ENV{ROOTSYS} CACHE PATH "ROOT instalation directory" FORCE)
 ELSE()
   SET( ROOT_DIR "ROOT_DIR-NOTFOUND" CACHE PATH "ROOT instalation directory" FORCE)
 ENDIF( IS_DIRECTORY $ENV{ROOTSYS} )
ENDIF()
   MARK_AS_ADVANCED( ROOT_DIR )

FIND_PACKAGE( ROOT REQUIRED )
if(NOT ROOT_FOUND)
  message(FATAL_ERROR "Lucas : ROOT package not found ! Set ROOT_DIR variable pointing to Your ROOT installation dir ")  
  return()
endif()
#----------------------------------------------------------------------------
# Lucas requires shared libraries
#
if(BUILD_STATIC_LIBS AND NOT BUILD_SHARED_LIBS )
  message(STATUS "Lucas : Geant4 built only Static Libraires. --> Lucas build disabled")
  return()
endif()
#----------------------------------------------------------------------------
#  LCIO required
#
MARK_AS_ADVANCED( LCIO_DIR )
FIND_PACKAGE( LCIO REQUIRED lcioDict)
if( LCIO_LIBRARY_DIRS )
  set( LCIO_LIBS "-L${LCIO_LIBRARY_DIRS} -llcioDict -llcio -lsio")
endif()

#----------------------------------------------------------------------------
# Locate sources and headers for this project
#
set( PROJECT_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include )
INCLUDE_DIRECTORIES(${PROJECT_INCLUDE_DIR} ${GEANT4_INCLUDE_DIR} ${ROOT_INCLUDE_DIR} ${LCIO_INCLUDE_DIRS})
FILE(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
FILE(GLOB headers ${PROJECT_INCLUDE_DIR}/*.hh)
FILE(GLOB headers ${PROJECT_INCLUDE_DIR}/*.h)
FILE(GLOB headers ${ROOT_INCLUDE_DIR}/*.h)

if( GEANT4_CONFIG_DEBUG )   
    message( STATUS " ************** Lucas include dir ${PROJECT_INCLUDE_DIR}")
    message( STATUS " ************** ROOT_INCLUDE_DIR ${ROOT_INCLUDE_DIR}")
    message( STATUS " ************** ROOT_VERSION     ${ROOT_VERSION}")
    message( STATUS " ************** ROOT_LIBS        ${ROOT_LIBRARIES}")
    message( STATUS " ************** GEANT4_INLUDE_DIR ${GEANT4_INCLUDE_DIR}")
    message( STATUS " ************** GEANT4_LIB_DIR ${GEANT4_LIB_DIR}")
    message( STATUS " ************** GEANT4_INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS}")
    message( STATUS " ************** GEANT4_LIBRARIES  ${GEANT4_LIBRARIES}")
endif()


if ( LUCAS_ROOT_DICTIONARY )
endif()
#----------------------------------------------------------------------------
# Generate dictionaries, add ROOT libraries properties
#
#
set( dictionary LucasDict )
SET(rdict_pcm ${dictionary}_rdict )
add_custom_command(OUTPUT ${dictionary}.cc ${dictionary}.h ${rdict_pcm}.pcm
                   COMMAND ${ROOT_CINT_WRAP} -f ${dictionary}.cc 
                                    -c -I${PROJECT_SOURCE_DIR}/include
                                     Track_t.hh Hit_t.hh LinkDef.h )
#                   COMMAND cp ${dictionary}.h ${PROJECT_SOURCE_DIR}/include )
#                   COMMAND cp ${dictionary}.h ${PROJECT_SOURCE_DIR}/include )

LIST(APPEND headers ${PROJECT_SOURCE_DIR}/include/${dictionary}.h)
ADD_LIBRARY(${dictionary} SHARED ${dictionary}.cc)
SET(libsuffix .so)

SET(ROOT_LIBRARY_PROPERTIES ${ROOT_LIBRARY_PROPERTIES} SUFFIX ${libsuffix})
SET_TARGET_PROPERTIES(${dictionary} PROPERTIES ${ROOT_LIBRARY_PROPERTIES})
TARGET_LINK_LIBRARIES(${dictionary} ${GEANT4_LIBRARIES} ${ROOT_LIBRARIES} )

# Add scripts to configure run time enviroment
#+
CONFIGURE_FILE( "${PROJECT_SOURCE_DIR}/cmake/templates/setup.sh.in"  "setup.sh"  @ONLY)
CONFIGURE_FILE( "${PROJECT_SOURCE_DIR}/cmake/templates/setup.csh.in" "setup.csh" @ONLY)
CONFIGURE_FILE( "${PROJECT_SOURCE_DIR}/cmake/templates/DidField.hh.in" "${PROJECT_INCLUDE_DIR}/DidField.hh" @ONLY)
CONFIGURE_FILE( "${PROJECT_SOURCE_DIR}/cmake/templates/banner.hh.in" "${PROJECT_INCLUDE_DIR}/banner.hh" @ONLY)

#----------------------------------------------------------------------------
# Add the library and executable, and link it to the Geant4 libraries
#
ADD_LIBRARY( Lucas SHARED ${sources} )
TARGET_LINK_LIBRARIES( Lucas ${GEANT4_LIBRARIES} ${ROOT_LIBRARIES} ${dictionary} ${LCIO_LIBS} )

ADD_EXECUTABLE(bin_Lucas Lucas.cc )
SET_TARGET_PROPERTIES( bin_Lucas PROPERTIES OUTPUT_NAME Lucas )
TARGET_LINK_LIBRARIES( bin_Lucas ${dictionary} Lucas ${GEANT4_LIBRARIES} ${ROOT_LIBRARIES} ${LCIO_LIBS} )

#----------------------------------------------------------------------------
# Copy all examples to installation place
install ( FILES ${PROJECT_SOURCE_DIR}/README DESTINATION ${CMAKE_INSTALL_PREFIX} )
install ( DIRECTORY ${PROJECT_SOURCE_DIR}/examples DESTINATION ${CMAKE_INSTALL_PREFIX} )
install ( DIRECTORY ${PROJECT_SOURCE_DIR}/data DESTINATION ${CMAKE_INSTALL_PREFIX} )

# get ROOT major version
       EXECUTE_PROCESS( COMMAND dirname ${ROOT_VERSION}
	 OUTPUT_VARIABLE ROOT_MAJOR_VERSION
	 RESULT_VARIABLE _exit_code
	 OUTPUT_STRIP_TRAILING_WHITESPACE
       )

IF( ROOT_MAJOR_VERSION VERSION_GREATER 6.00 )
  INSTALL ( FILES ${CMAKE_CURRENT_BINARY_DIR}/${rdict_pcm}.pcm DESTINATION lib )
ENDIF(ROOT_MAJOR_VERSION VERSION_GREATER 6.00)
#----------------------------------------------------------------------------
# Install the executable and libraries to 'bin'and 'lib' directory under CMAKE_INSTALL_PREFIX
#
INSTALL(PROGRAMS ${PROJECT_BINARY_DIR}/setup.sh ${PROJECT_BINARY_DIR}/setup.csh DESTINATION bin)
INSTALL(TARGETS bin_Lucas DESTINATION bin)
INSTALL(TARGETS Lucas ${dictionary} DESTINATION lib)


