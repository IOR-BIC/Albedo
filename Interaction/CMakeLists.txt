#=========================================================================
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
#=========================================================================*/

PROJECT(appInteraction)

# Set your list of sources here.
SET(PROJECT_SRCS
appInteractor2DSample.cpp							appInteractor2DSample.h
appInteractor2DMeasure.cpp						appInteractor2DMeasure.h
appInteractor2DMeasure_Point.cpp			appInteractor2DMeasure_Point.h
appInteractor2DMeasure_Distance.cpp		appInteractor2DMeasure_Distance.h
appInteractor2DMeasure_Angle.cpp			appInteractor2DMeasure_Angle.h
appInteractor2DMeasure_Indicator.cpp	appInteractor2DMeasure_Indicator.h
appInteractor2DMeasure_Comment.cpp	appInteractor2DMeasure_Comment.h
appInteractor2DMeasure_DistancePoint.cpp	appInteractor2DMeasure_DistancePoint.h
)

SET(PROJECT_LIBS ${PROJECT_LIBS})

IF(ALBEDO_BUILD_DLL)
  SET(BUILD_SHARED_LIBS 1)
  ADD_DEFINITIONS(-DAPP_INTERACTIONS_EXPORTS)
  # Create the library.
  ADD_LIBRARY(${PROJECT_NAME} ${PROJECT_SRCS})
  TARGET_LINK_LIBRARIES(${PROJECT_NAME} albaDLL appVME appViews)
ELSE(ALBEDO_BUILD_DLL)
  # Create the library.
  ADD_LIBRARY(${PROJECT_NAME} ${PROJECT_SRCS})
  TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${PROJECT_LIBS})
ENDIF (ALBEDO_BUILD_DLL)