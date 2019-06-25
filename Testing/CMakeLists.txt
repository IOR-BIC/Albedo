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

PROJECT (appTests)

# Load CppUnit stuff
INCLUDE(${ALBEDO_SOURCE_DIR}/CMakeModules/FindCppUnit.cmake)
IF (CPPUNIT_FOUND)
  #MESSAGE("Found Cppunit library")
  INCLUDE_DIRECTORIES(${CPPUNIT_INCLUDE_DIR})
  LINK_LIBRARIES(${CPPUNIT_LIBRARIES})  
ELSE (CPPUNIT_FOUND)
  MESSAGE("Cppunit library not found! Please fill the corresponding cache entries")
ENDIF (CPPUNIT_FOUND)

# Set your list of sources here.
SET(PROJECT_SRCS 
		appTest.cpp 							appTest.h
		appFakeLogicForTest.cpp		appFakeLogicForTest.h
		)
	
# Create the library.
ADD_LIBRARY(${PROJECT_NAME} ${PROJECT_SRCS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${PROJECT_LIBS})
  
# OPTION(BUILD_VIEWS_TESTING "Enable building of testing for View classes" ON)
# IF (BUILD_VIEWS_TESTING)
  # SUBDIRS(Views)
# ENDIF (BUILD_VIEWS_TESTING)

# OPTION(BUILD_VME_TESTING "Enable building of testing for VME classes" ON)
# IF (BUILD_VME_TESTING)
  # SUBDIRS(VME)
# ENDIF (BUILD_VME_TESTING)

OPTION(BUILD_OPERATIONS_TESTING "Enable building of testing for OPERATIONS classes" ON)
IF (BUILD_OPERATIONS_TESTING)
  SUBDIRS(Operations)
ENDIF (BUILD_OPERATIONS_TESTING)

# OPTION(BUILD_COMMON_TESTING "Enable building of testing for COMMON classes" ON)
# IF (BUILD_COMMON_TESTING)
  # SUBDIRS(Common)
# ENDIF (BUILD_COMMON_TESTING)

# OPTION(BUILD_WEB_SERVICES_TESTING "Enable building of testing for Web Services" OFF)
# IF (BUILD_WEB_SERVICES_TESTING)
  # SUBDIRS(Common)
# ENDIF (BUILD_WEB_SERVICES_TESTING)

#SUBDIRS(UnitTestsData)

