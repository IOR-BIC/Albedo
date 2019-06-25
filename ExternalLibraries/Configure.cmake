#=========================================================================
# Program:   Albedo
# Module:    Configue.cmake
# Language:  CMake
# Date:      $Date: 2019-01-01 12:00:00 $
# Version:   $Revision: 1.0.0.0 $
# Authors:   Nicola Vanella
#==========================================================================
# Copyright (c) BIC-IOR 2019 (https://github.com/IOR-BIC)
#
# This software is distributed WITHOUT ANY WARRANTY; without even
# the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
# PURPOSE. See the above copyright notice for more information.
#=========================================================================*/

# this defines APPExternalLibraries_SUBPROJECT
INCLUDE (${APPExternalLibraries_SOURCE_DIR}/modules/APPExternalLibrariesConfigMacro.cmake)
SET (DEBUG FALSE)

# options for configuring APPExternalLibraries libraries
OPTION(ALBEDO_USE_YOURLIBRARY "Find and Link YOURLIBRARY library." ON)

# configure CMake modules for APPExternalLibraries
APPExternalLibraries_SUBPROJECT (APPExternalLibraries_MODULES modules)


# Sample code to include an external library
# IF (APP_USE_YOURLIBRARY)
#   APPExternalLibraries_SUBPROJECT(YOURLIBRARY YOURLIBRARY)
# ENDIF(APP_USE_YOURLIBRARY)

# Create module files in build directory
IF (NOT CONFIGURE_ERROR)
  CONFIGURE_FILE(${APPExternalLibraries_SOURCE_DIR}/UseAPPExternalLibraries.cmake.in ${APPExternalLibraries_BINARY_DIR}/UseAPPExternalLibraries.cmake @ONLY IMMEDIATE)
ENDIF (NOT CONFIGURE_ERROR)

MARK_AS_ADVANCED (
  APPExternalLibraries_BUILD_TARGETS
)

#Write the list of variables and their values that are defined by CMake when running this CMakeList file
IF (DEBUG_MESSAGES)
  #FILE(WRITE ${APPExternalLibraries_BINARY_DIR}/AllVariables.txt "")
  GET_CMAKE_PROPERTY(VARS VARIABLES)
  FOREACH(var ${VARS})
  	FILE(APPEND ${APPExternalLibraries_BINARY_DIR}/AllVariables.txt 
  							"${var} \"${${var}}\"\n")
  ENDFOREACH(var ${VARS})
ENDIF (DEBUG_MESSAGES)
