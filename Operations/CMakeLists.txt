#==========================================================================
# Program:   Albedo
# Module:    CMakeLists.txt
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
#==========================================================================

PROJECT(appOperations)

# Set your list of sources here.
SET(PROJECT_SRCS
appOpEmpty.cpp						appOpEmpty.h
appOpGuiSample.cpp					appOpGuiSample.h
appOpInteractorSample.cpp			appOpInteractorSample.h
appOpInteractorSliderSample.cpp		appOpInteractorSliderSample.h
appOpCreateEmptyVME.cpp				appOpCreateEmptyVME.h
appOpCreateSurfaceParametric.cpp	appOpCreateSurfaceParametric.h
appOpDictionary.cpp					appOpDictionary.h
appOpShowDialog.cpp					appOpShowDialog.h
appOpShowImage.cpp					appOpShowImage.h
appOpCursorTest.cpp					appOpCursorTest.h
appOpMeasure2D.cpp					appOpMeasure2D.h
appOpAddComment.cpp					appOpAddComment.h
appOpInteractorTest.cpp				appOpInteractorTest.h
appOpApplyTemplate.cpp				appOpApplyTemplate.h
appOpTransformInteractor.cpp		appOpTransformInteractor.h
)

SET(PROJECT_LIBS ${PROJECT_LIBS})

IF(ALBEDO_BUILD_DLL)
  SET(BUILD_SHARED_LIBS 1)
  ADD_DEFINITIONS(-DAPP_OPERATIONS_EXPORTS)
  # Create the library.
  ADD_LIBRARY(${PROJECT_NAME} ${PROJECT_SRCS})
  TARGET_LINK_LIBRARIES(${PROJECT_NAME} albaDLL appGui appVME vtkAPP)
ELSE(ALBEDO_BUILD_DLL)
  # Create the library.
  ADD_LIBRARY(${PROJECT_NAME} ${PROJECT_SRCS})
  TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${PROJECT_LIBS})
ENDIF (ALBEDO_BUILD_DLL)