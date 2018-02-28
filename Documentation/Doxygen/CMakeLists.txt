#=========================================================================
# Program:   Albedo
# Module:    CMakeLists.txt
# Language:  CMake
# Date:      $Date: 2018-01-01 12:00:00 $
# Version:   $Revision: 1.0.0.0 $
# Authors:   Nicola Vanella
#==========================================================================
# Copyright (c) LTM-IOR 2018 (https://github.com/IOR-LTM)
#
# This software is distributed WITHOUT ANY WARRANTY; without even
# the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
# PURPOSE. See the above copyright notice for more information.
#=========================================================================*/

PROJECT(appDoxygen)

INCLUDE (${CMAKE_ROOT}/Modules/Documentation.cmake OPTIONAL)

IF (BUILD_DOCUMENTATION)
  SET (DOXYFILE ${PROJECT_BINARY_DIR}/app_doxyfile)
  GET_FILENAME_COMPONENT(DOT_PATH ${DOT} PATH)
  CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/app_doxyfile.in
  ${PROJECT_BINARY_DIR}/app_doxyfile @ONLY)
             
  IF (WIN32)
    CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/makedoc.bat.in
    ${PROJECT_BINARY_DIR}/makedoc.bat IMMEDIATE)
  ENDIF (WIN32)

ENDIF (BUILD_DOCUMENTATION)
