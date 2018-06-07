#==========================================================================
# Program:   Albedo
# Module:    CMakeLists.txt
# Language:  CMake
# Date:      $Date: 2018-01-01 12:00:00 $
# Version:   $Revision: 1.0.0.0 $
# Authors:   Nicola Vanella
#==========================================================================
# Copyright (c) LTM-IOR 2018 (https://github.com/IOR-BIC)
#
# This software is distributed WITHOUT ANY WARRANTY; without even
# the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
# PURPOSE. See the above copyright notice for more information.
#==========================================================================

PROJECT (OPERATIONS_TEST)

LINK_LIBRARIES(appTests appOperations mafService mafVME)

#-----------------------------------------------------------------------------
# tests using cppunit testing framework
#-----------------------------------------------------------------------------

SET(PROJECT_SRCS
	appOperationsTests.cpp 	appOperationsTests.h
	appOpEmptyTest.cpp 			appOpEmptyTest.h
	)

ADD_EXECUTABLE(appOperationsTests ${PROJECT_SRCS})
ADD_TEST(appOperationsTests  ${EXECUTABLE_OUTPUT_PATH}/appOperationsTests)

IF (WIN32)
  SET_TARGET_PROPERTIES(appOperationsTests PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)
