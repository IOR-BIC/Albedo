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

PROJECT (Albedo)

SET(PROJECT_SRCS 
Albedo.cpp 					Albedo.h
appLogic.cpp 				appLogic.h
appViewManager.cpp 	appViewManager.h
appWizardSample.cpp	appWizardSample.h
appDecl.h
)

IF (NOT EXECUTABLE_OUTPUT_PATH)
  SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin/ CACHE INTERNAL 
  "Single output directory for building all executables.")
ENDIF (NOT EXECUTABLE_OUTPUT_PATH)

IF (WIN32)
	#Newer versions of CMakes supports CXX_COMPILER info, so it is not necessary to test CMAKE_GENERATOR
	IF (${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC AND NOT ${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 17.0)		
		SET(NO_MANIFEST 1)
	ELSE (${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC AND NOT ${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 17.0)
		#Prehaps an older version, i.e., it does not support VS 10, 12, 13
		IF(${CMAKE_GENERATOR} STREQUAL "Visual Studio 9 2008" OR ${CMAKE_GENERATOR} STREQUAL "Visual Studio 8 2005" OR ${CMAKE_GENERATOR} STREQUAL "Visual Studio 10" OR ${CMAKE_GENERATOR} STREQUAL "Visual Studio 11")
		  SET(NO_MANIFEST 1)
		ELSE(${CMAKE_GENERATOR} STREQUAL "Visual Studio 9 2008" OR ${CMAKE_GENERATOR} STREQUAL "Visual Studio 8 2005" OR ${CMAKE_GENERATOR} STREQUAL "Visual Studio 10" OR ${CMAKE_GENERATOR} STREQUAL "Visual Studio 11")
		  SET(NO_MANIFEST 0)
		ENDIF(${CMAKE_GENERATOR} STREQUAL "Visual Studio 9 2008" OR ${CMAKE_GENERATOR} STREQUAL "Visual Studio 8 2005" OR ${CMAKE_GENERATOR} STREQUAL "Visual Studio 10" OR ${CMAKE_GENERATOR} STREQUAL "Visual Studio 11")	
	ENDIF (${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC AND NOT ${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 17.0)
	
	CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/app.rc.in ${PROJECT_BINARY_DIR}/app.rc @ONLY)
  ADD_EXECUTABLE(${PROJECT_NAME} WIN32 ${PROJECT_SRCS} "${PROJECT_BINARY_DIR}/app.rc")
ELSE (WIN32)
  ADD_EXECUTABLE(${PROJECT_NAME} ${PROJECT_SRCS} )
ENDIF (WIN32)

SET(PROJECT_LIBS albaOperations albaGui albaViews albaVME albaInteraction albaService albaVME albaVisualPipes albaOperations vtkALBA appOperations appInteraction appViews appCommon appGui appVME)# GPUAPI))

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${PROJECT_LIBS})

