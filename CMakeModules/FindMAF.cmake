#=========================================================================
# Program:   Albedo
# Module:    FindCMAF.cmake
# Language:  CMake
# Date:      $Date: 2018-01-01 12:00:00 $
# Version:   $Revision: 1.0.0.0 $
# Authors:   Nicola Vanella
#==========================================================================
# Copyright (c) LTM-IOR 2018 (http://www.ltmsoftware.org/alba.htm)
#
# This software is distributed WITHOUT ANY WARRANTY; without even
# the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
# PURPOSE. See the above copyright notice for more information.
#=========================================================================*/
#
# Description:
# This module finds the location of MAF include and library paths 
#

  MESSAGE (STATUS "Find: Searching for MAF.")
  # If not built within MAF project try standard places
  FIND_PATH(MAF_BINARY_PATH MAFConfig.cmake
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
    
  IF(MAF_BINARY_PATH)
    SET (MAF_FOUND 1)
    FIND_FILE(MAF_USE_FILE MAFUse.cmake ${MAF_BINARY_PATH})
    INCLUDE(${MAF_BINARY_PATH}/MAFConfig.cmake)
  ENDIF(MAF_BINARY_PATH)
    
#MARK_AS_ADVANCED (
#  MAF_BINARY_PATH
#  MAF_FOUND
#  )

#MESSAGE("FindMAF: EOF")
