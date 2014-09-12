# - Try to find the CLHEP High Energy Physics library and headers
# ===========================================================================
# Variables used by this module which can be used to change the default
# behaviour, and hence need to be set before calling find_package:
#
#  CLHEP_ROOT_DIR        The preferred installation prefix for searching for 
#                        CLHEP. Set this if the module has problems finding 
#                        the proper CLHEP installation.
#
# If you don't supply CLHEP_ROOT_DIR, the module will search on the standard
# system paths. On UNIX, the module will also try to find the clhep-config
# program in the PATH, and if found will use the prefix supplied by this
# program as a HINT on where to find the CLHEP headers and libraries.
#
# You can re-run CMake with a different version of CLHEP_ROOT_DIR to 
# force a new search for CLHEP using the new version of CLHEP_ROOT_DIR.
#
# ============================================================================
# Variables set by this module:
#
#  CLHEP_FOUND           System has CLHEP.
#
#  CLHEP_INCLUDE_DIRS    CLHEP include directories: not cached.
#
#  CLHEP_LIBRARIES       Link to these to use the CLHEP library: not cached.
#
# ===========================================================================
# If CLHEP is installed in a non-standard way, e.g. a non GNU-style install
# of <prefix>/{lib,include}, then this module may fail to locate the headers
# and libraries as needed. In this case, the following cached variables can
# be editted to point to the correct locations.
#
#  CLHEP_INCLUDE_DIR    The path to the CLHEP include directory: cached
#
#  CLHEP_LIBRARY        The path to the CLHEP library: cached
#
#
# Author  2013  Bogdan Pawlik <bogdan.pawlik@ifj.edu.pl>>.
#============================================================================


#----------------------------------------------------------------------------
# find clhep config
#
if ( GEANT4_CONFIG_DEBUG )
message(STATUS " CLHEP_ROOT_DIR obtained ${CLHEP_ROOT_DIR}")
endif()

if( CLHEP_CONFIG_EXECUTABLE-NOTFOUND )
FIND_PROGRAM( CLHEP_CONFIG_EXECUTABLE clhep-config
      PATHSS ${CLHEP_ROOT_DIR}
      PATHS_SUFFIXES bin
      NO_DEFAULT_PATH )

IF ( NOT CLHEP_ROOT_DIR )
    FIND_PROGRAM( CLHEP_CONFIG_EXECUTABLE clhep-config )
ENDIF()
ENDIF()

IF ( CLHEP_CONFIG_EXECUTABLE )

# clhep version
#+
    EXECUTE_PROCESS( COMMAND "${CLHEP_CONFIG_EXECUTABLE}" --version
        OUTPUT_VARIABLE _output
        RESULT_VARIABLE _exit_code
        OUTPUT_STRIP_TRAILING_WHITESPACE)
    IF( _exit_code EQUAL 0 )
        SET( CLHEP_VERSION ${_output} )
        SEPARATE_ARGUMENTS( CLHEP_VERSION )
        LIST( REMOVE_AT CLHEP_VERSION 0 ) 
    ELSE()
        SET( CLHEP_VERSION )
    ENDIF()

# clhep prefix
#+
    EXECUTE_PROCESS( COMMAND "${CLHEP_CONFIG_EXECUTABLE}" --prefix |sed {s/"//}|sed {s/"//}
        OUTPUT_VARIABLE CLHEP_ROOT
        RESULT_VARIABLE _exit_code
        OUTPUT_STRIP_TRAILING_WHITESPACE)
    IF( NOT _exit_code EQUAL 0 )
        SET( CLHEP_ROOT )
    ENDIF()

ENDIF()

#----------------------------------------------------------------------------
# clhep headers dir
#+
find_path(CLHEP_INCLUDE_DIR 
    NAMES CLHEP/Units/defs.h
    PATHS ${CLHEP_ROOT_DIR}
    PATH_SUFFIXES include
    NO_DEFAULT_PATH
    DOC "Path to the CLHEP headers" 
)
# Find the CLHEP library
# Prefer lib64 if available.
FIND_LIBRARY(CLHEP_LIBRARY
    NAMES  CLHEP
    HINTS ${CLHEP_ROOT_DIR}
    PATH_SUFFIXES lib64 lib
    DOC "Path to the CLHEP library"
)

IF( CLHEP_LIBRARY )
  EXECUTE_PROCESS( COMMAND dirname ${CLHEP_LIBRARY}
	OUTPUT_VARIABLE CLHEP_LIBRARY_DIR
	OUTPUT_STRIP_TRAILING_WHITESPACE)
ENDIF()

    SET ( CLHEP_LIBRARIES_DIR ${CLHEP_LIBRARY_DIR})
    SET ( CLHEP_LIBRARIES ${CLHEP_LIBRARY})
    SET ( CLHEP_INCLUDE_DIRS ${CLHEP_INCLUDE_DIR})

#----------------------------------------------------------------------------
# Versions COMPATIBLE if RequestedVersion > FoundVersion
# Also check if versions exact

IF( CLHEP_VERSION )

    MESSAGE(STATUS "Found CLHEP Version ${CLHEP_VERSION}")
  
    IF(CLHEP_FIND_VERSION)
        SET(CLHEP_VERSIONING_TESTS CLHEP_VERSION_COMPATIBLE)

        IF("${CLHEP_VERSION}" VERSION_LESS "${CLHEP_FIND_VERSION}")
            SET(CLHEP_VERSION_COMPATIBLE FALSE)
        ELSE()
            SET(CLHEP_VERSION_COMPATIBLE TRUE)

            IF(CLHEP_FIND_VERSION_EXACT)
                IF("${CLHEP_VERSION}" VERSION_EQUAL "${CLHEP_FIND_VERSION}")
                    SET(CLHEP_VERSION_EXACT TRUE)
                ENDIF()
                    LIST(APPEND CLHEP_VERSIONING_TESTS CLHEP_VERSION_EXACT)
            ENDIF()
        ENDIF()
    ENDIF()
ENDIF( CLHEP_VERSION )

#----------------------------------------------------------------------------
# Construct an error message for FPHSA
#
set(CLHEP_DEFAULT_MSG "Could NOT find CLHEP:\n")

if(NOT CLHEP_INCLUDE_DIR)
    set(CLHEP_DEFAULT_MSG "${CLHEP_DEFAULT_MSG}CLHEP Header Path Not Found\n")
endif()

if(NOT CLHEP_LIBRARY)
    set(CLHEP_DEFAULT_MSG "${CLHEP_DEFAULT_MSG}CLHEP Library Not Found\n")
endif()

if(CLHEP_FIND_VERSION)
    if(NOT CLHEP_VERSION_COMPATIBLE)
        set(CLHEP_DEFAULT_MSG "${CLHEP_DEFAULT_MSG}Incompatible versions, ${CLHEP_VERSION}(found) < ${CLHEP_FIND_VERSION}(required)\n")
    endif()

    if(CLHEP_FIND_VERSION_EXACT)
        if(NOT CLHEP_VERSION_EXACT)
            set(CLHEP_DEFAULT_MSG "${CLHEP_DEFAULT_MSG}Non-exact versions, ${CLHEP_VERSION}(found) != ${CLHEP_FIND_VERSION}(required)\n")
        endif()
    endif()
endif()



#----------------------------------------------------------------------------
# Handle the QUIETLY and REQUIRED arguments, setting CLHEP_FOUND to TRUE if
# all listed variables are TRUE
#
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    CLHEP 
    "${CLHEP_DEFAULT_MSG}"
    CLHEP_LIBRARY
    CLHEP_INCLUDE_DIR
    CLHEP_LIBRARY_DIR
    ${CLHEP_VERSIONING_TESTS}
    )


#----------------------------------------------------------------------------
# If we found CLHEP, set the needed non-cache variables
#
if(CLHEP_FOUND)
    set(CLHEP_LIBRARIES ${CLHEP_LIBRARY})
    set(CLHEP_INCLUDE_DIRS ${CLHEP_INCLUDE_DIR})
endif()

#----------------------------------------------------------------------------
# Mark cache variables that can be adjusted as advanced
#
    MARK_AS_ADVANCED( CLHEP_CONFIG_EXECUTABLE )
    MARK_AS_ADVANCED( CLHEP_INCLUDE_DIRS )
    MARK_AS_ADVANCED( CLHEP_LIBRARY )

