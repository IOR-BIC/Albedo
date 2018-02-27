#==========================================================================
# Program:   Albedo
# Module:    CMakeLists.txt
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
#==========================================================================

PROJECT(appOperations)

# Set your list of sources here.
SET(PROJECT_SRCS
appOpEmpty.cpp										appOpEmpty.h
appOpGuiSample.cpp								appOpGuiSample.h
#appOpInteractorSample.cpp					appOpInteractorSample.h
#appOpCreateVMEEmpty.cpp						appOpCreateVMEEmpty.h
#appOpCreateSurfaceParametric.cpp	appOpCreateSurfaceParametric.h
)

SET(PROJECT_LIBS ${PROJECT_LIBS})

IF(ALBEDO_BUILD_DLL)
  SET(BUILD_SHARED_LIBS 1)
  ADD_DEFINITIONS(-DAPP_OPERATIONS_EXPORTS)
  # Create the library.
  ADD_LIBRARY(${PROJECT_NAME} ${PROJECT_SRCS})
  TARGET_LINK_LIBRARIES(${PROJECT_NAME} mafDLL appGui appVME vtkAPP)
ELSE(ALBEDO_BUILD_DLL)
  # Create the library.
  ADD_LIBRARY(${PROJECT_NAME} ${PROJECT_SRCS})
  TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${PROJECT_LIBS})
ENDIF (ALBEDO_BUILD_DLL)