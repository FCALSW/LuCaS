# - Finds ROOT instalation
# This module sets up ROOT information 
# It defines:
# ROOT_CONFIG_EXECUTABLE   root-config if the ROOT is found
# ROOT_FOUND               If the ROOT is found
# ROOT_INCLUDE_DIR         PATH to the include directory
# ROOT_LIBRARIES           Most common libraries
# ROOT_LIBRARY_DIR         PATH to the library directory 

#
# try using enviroment vars if set

if( NOT EXISTS ${ROOT_CONFIG_EXECUTABLE} )

message ( STATUS "Looking for ROOT :" )

if( IS_DIRECTORY ${ROOT_DIR} )
message ( STATUS "                 - in ${ROOT_DIR}" )

   find_program( ROOT_CONFIG_EXECUTABLE root-config
               PATHS $ENV{ROOT_DIR}/bin ${ROOTSYS}/bin NO_DEFAULT_PATH )
else( IS_DIRECTORY ${ROOT_DIR} )
message ( STATUS "                 - in default locations : ${CMAKE_PREFIX_PATH}" )
   find_program(ROOT_CONFIG_EXECUTABLE root-config )
endif( IS_DIRECTORY ${ROOT_DIR} )

endif(NOT EXISTS ${ROOT_CONFIG_EXECUTABLE})

# last chance, check /opt 
#
if( NOT EXISTS ${ROOT_CONFIG_EXECUTABLE} )
message ( STATUS "                 - in /opt" )

       execute_process( 
       	COMMAND find /opt \( -name "root-config" \) -print -quit
	OUTPUT_VARIABLE ROOT_CONFIG_EXECUTABLE 
	OUTPUT_STRIP_TRAILING_WHITESPACE )

endif(NOT EXISTS ${ROOT_CONFIG_EXECUTABLE} )

  # Make variables changeble to the advanced user
MARK_AS_ADVANCED(ROOT_CONFIG_EXECUTABLE)

if(NOT EXISTS ${ROOT_CONFIG_EXECUTABLE} )
  set(ROOT_FOUND FALSE)
else()    
  set(ROOT_FOUND TRUE) 
  execute_process(
    COMMAND ${ROOT_CONFIG_EXECUTABLE} --prefix
    OUTPUT_VARIABLE _rootsys 
    OUTPUT_STRIP_TRAILING_WHITESPACE)
    set(ROOT_DIR  ${_rootsys} CACHE FILEPATH " ROOT home prefix" FORCE)

  execute_process(
    COMMAND ${ROOT_CONFIG_EXECUTABLE} --version 
    OUTPUT_VARIABLE ROOT_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE)

  execute_process(
    COMMAND ${ROOT_CONFIG_EXECUTABLE} --incdir
    OUTPUT_VARIABLE ROOT_INCLUDE_DIR
    OUTPUT_STRIP_TRAILING_WHITESPACE)

  execute_process(
    COMMAND ${ROOT_CONFIG_EXECUTABLE} --libs
    OUTPUT_VARIABLE ROOT_LIBRARIES
    OUTPUT_STRIP_TRAILING_WHITESPACE)
  set( ROOT_LIBRARIES "${ROOT_LIBRARIES} -lTreePlayer")

  execute_process(
    COMMAND ${ROOT_CONFIG_EXECUTABLE} --libdir
    OUTPUT_VARIABLE ROOT_LIB_DIR
    OUTPUT_STRIP_TRAILING_WHITESPACE)

  set(ROOT_CONFIG_EXECUTABLE ${ROOT_CONFIG_EXECUTABLE} CACHE FILE " root config executable name with prefix" FORCE)
  set(ROOT_LIBRARY_DIR ${ROOT_LIB_DIR} CACHE FILEPATH " ROOT libraries prefix" )


  if(NOT ROOT_FIND_QUIETLY)
    message(STATUS "Found ROOT ${ROOT_VERSION} in ${ROOTSYS}")
  endif( NOT ROOT_FIND_QUIETLY)

endif(NOT EXISTS ${ROOT_CONFIG_EXECUTABLE})


include(CMakeMacroParseArguments)
find_program(ROOT_CINT_EXECUTABLE rootcint PATHS ${ROOT_DIR}/bin NO_DEFAULT_PATH )
find_program(ROOT_GENREFLEX_EXECUTABLE genreflex PATHS ${ROOT_DIR}/bin NO_DEFAULT_PATH )
find_package(GCCXML)

# set scope for  rootcint 
#
SET( LD_LIBRARY_PATH_VAR LD_LIBRARY_PATH )
SET( LD_LIBRARY_PATH_CONTENTS $ENV{${LD_LIBRARY_PATH_VAR}} )
set ( ROOT_CINT_WRAP ${LD_LIBRARY_PATH_VAR}=${ROOT_LIB_DIR}:${LD_LIBRARY_PATH_CONTENTS} ${ROOT_CINT_EXECUTABLE} )

#----------------------------------------------------------------------------
# function ROOT_GENERATE_DICTIONARY( dictionary   
#                                    header1 header2 ... 
#                                    LINKDEF linkdef1 ... 
#                                    OPTIONS opt1...)
function(ROOT_GENERATE_DICTIONARY dictionary)
  PARSE_ARGUMENTS(ARG "" "" "LINKDEF;OPTIONS" "" ${ARGN})
  #---Get the list of header files-------------------------
  set(headerfiles)
  foreach(fp ${ARG_UNPARSED_ARGUMENTS})
    file(GLOB files ${fp})
    if(files)
      foreach(f ${files})
        if(NOT f MATCHES LinkDef)
          set(headerfiles ${headerfiles} ${f})
        endif()
      endforeach()
    else()
      set(headerfiles ${headerfiles} ${fp})
    endif()
  endforeach()
  #---Get the list of include directories------------------
  get_directory_property(incdirs INCLUDE_DIRECTORIES)
  set(includedirs) 
  foreach( d ${incdirs})    
   set(includedirs ${includedirs} -I${d})
  endforeach()
  #---Get LinkDef.h file------------------------------------
  set(linkdefs)
  foreach( f ${ARG_LINKDEF})
    if( IS_ABSOLUTE ${f})
      set(linkdefs ${linkdefs} ${f})
    else() 
      if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/inc/${f})
        set(linkdefs ${linkdefs} ${CMAKE_CURRENT_SOURCE_DIR}/inc/${f})
      else()
        set(linkdefs ${linkdefs} ${CMAKE_CURRENT_SOURCE_DIR}/${f})
      endif()
    endif()
  endforeach()
  #---call rootcint------------------------------------------
  add_custom_command(OUTPUT ${dictionary}.cxx ${dictionary}.h
                     COMMAND ${ROOT_CINT_WRAP} -cint -f  ${dictionary}.cxx 
                                          -c ${ARG_OPTIONS} ${includedirs} ${headerfiles} ${linkdefs} 
                     DEPENDS ${headerfiles} ${linkdefs})
endfunction()

  
