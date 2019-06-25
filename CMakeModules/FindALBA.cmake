#=========================================================================
# Program:   Albedo
# Module:    FindCALBA.cmake
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
#
# Description:
# This module finds the location of ALBA include and library paths 
#

  MESSAGE (STATUS "Find: Searching for ALBA.")
  # If not built within ALBA project try standard places
  FIND_PATH(ALBA_BINARY_PATH ALBAConfig.cmake
      [HKEY_CURRENT_USER\\Software\\Kitware\\CMakeSetup\\Settings\\StartPath;WhereBuild1]
      [HKEY_CURRENT_USER\\Software\\Kitware\\CMakeSetup\\Settings\\StartPath;WhereBuild2]
      [HKEY_CURRENT_USER\\Software\\Kitware\\CMakeSetup\\Settings\\StartPath;WhereBuild3]
      [HKEY_CURRENT_USER\\Software\\Kitware\\CMakeSetup\\Settings\\StartPath;WhereBuild4]
      [HKEY_CURRENT_USER\\Software\\Kitware\\CMakeSetup\\Settings\\StartPath;WhereBuild5]
      [HKEY_CURRENT_USER\\Software\\Kitware\\CMakeSetup\\Settings\\StartPath;WhereBuild6]
      [HKEY_CURRENT_USER\\Software\\Kitware\\CMakeSetup\\Settings\\StartPath;WhereBuild7]
      [HKEY_CURRENT_USER\\Software\\Kitware\\CMakeSetup\\Settings\\StartPath;WhereBuild8]
      [HKEY_CURRENT_USER\\Software\\Kitware\\CMakeSetup\\Settings\\StartPath;WhereBuild9]
      [HKEY_CURRENT_USER\\Software\\Kitware\\CMakeSetup\\Settings\\StartPath;WhereBuild10]
      )
    
  IF(ALBA_BINARY_PATH)
    SET (ALBA_FOUND 1)
    FIND_FILE(ALBA_USE_FILE ALBAUse.cmake ${ALBA_BINARY_PATH})
    INCLUDE(${ALBA_BINARY_PATH}/ALBAConfig.cmake)
  ENDIF(ALBA_BINARY_PATH)
    
#MARK_AS_ADVANCED (
#  ALBA_BINARY_PATH
#  ALBA_FOUND
#  )

#MESSAGE("FindALBA: EOF")
