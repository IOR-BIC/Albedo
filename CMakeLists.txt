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

SET (CMAKE_BACKWARDS_COMPATIBILITY "2.4" CACHE INTERNAL "This project is for CMake 2.4")
CMAKE_MINIMUM_REQUIRED(VERSION 2.4) 

if(COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
	cmake_policy(SET CMP0048 NEW)
endif(COMMAND cmake_policy)

PROJECT (ALBEDO)

#-----------------------------------------------------------------------------
# This enables support for Visual Studio: Edit & Continue, OpenMP, and parallel compilation
#-----------------------------------------------------------------------------
IF (${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
	#Add OpenMP Support
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /openmp")

	#Add support for Edit & Continue
	SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /ZI")
	IF (NOT ${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 17.0)
		SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
		SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /SAFESEH:NO")
	ENDIF (NOT ${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 17.0)
ENDIF (${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)

# Create the header to configure the classes.
CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/appDefines.h.in 
							 ${PROJECT_BINARY_DIR}/appDefines.h 
							 @ONLY)

#-----------------------------------------------------------------------------
# This define the APP_SUBPROJECT macro
#-----------------------------------------------------------------------------
INCLUDE (${ALBEDO_SOURCE_DIR}/CMakeModules/APPConfigMacro.cmake)

#-----------------------------------------------------------------------------
# Where to place output binaries
#-----------------------------------------------------------------------------
SET (LIBRARY_OUTPUT_PATH ${ALBEDO_BINARY_DIR}/bin/ CACHE INTERNAL
     "Single output directory for building all libraries.")

SET (EXECUTABLE_OUTPUT_PATH ${ALBEDO_BINARY_DIR}/bin/ CACHE INTERNAL 
     "Single output directory for building all executables.")

MARK_AS_ADVANCED(LIBRARY_OUTPUT_PATH)

FIND_PATH(MANUAL_HTML_DIR Albedo/Albedo_help.html)

#-----------------------------------------------------------------------------
# Options for Albedo building
#-----------------------------------------------------------------------------
OPTION(BUILD_SHARED_LIBS "Build with shared libraries." 0)
MARK_AS_ADVANCED (BUILD_SHARED_LIBS)

OPTION(BUILD_TESTING "Build the testing tree." OFF)

#-----------------------------------------------------------------------------
# Find required libraries
#-----------------------------------------------------------------------------

INCLUDE("${PROJECT_SOURCE_DIR}/CMakeModules/FindMAF.cmake")
INCLUDE("${MAF_BINARY_PATH}/MAFConfig.cmake")

# Find MAF library
IF(MAF_FOUND)
	
  INCLUDE("${MAF_USE_FILE}")
  
	# IF(ALBEDO_BUILD_DLL)
    # SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /FORCE:MULTIPLE")
	  # SET(ALBEDO_BUILD_DLL 1)
	# ELSE(ALBEDO_BUILD_DLL)
      SET(ALBEDO_BUILD_DLL 0)
	# ENDIF (ALBEDO_BUILD_DLL)
  
  CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/Operations/appOperationsDefines.h.in ${PROJECT_BINARY_DIR}/appOperationsDefines.h @ONLY)
  CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/VME/appVMEDefines.h.in ${PROJECT_BINARY_DIR}/appVMEDefines.h @ONLY)
  CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/Common/appCommonDefines.h.in ${PROJECT_BINARY_DIR}/appCommonDefines.h @ONLY)
  #CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/Gui/appGuiDefines.h.in ${PROJECT_BINARY_DIR}/appGuiDefines.h @ONLY)
  CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/Views/appViewsDefines.h.in ${PROJECT_BINARY_DIR}/appViewsDefines.h @ONLY)
  CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/Interaction/appInteractionDefines.h.in ${PROJECT_BINARY_DIR}/appInteractionDefines.h @ONLY)
  
  # Add single dirs to inclusion path
  INCLUDE_DIRECTORIES(BEFORE ".")
  INCLUDE_DIRECTORIES(${ALBEDO_BINARY_DIR})
  INCLUDE_DIRECTORIES(${ALBEDO_SOURCE_DIR}/Application)
  INCLUDE_DIRECTORIES(${ALBEDO_SOURCE_DIR}/Common)
  INCLUDE_DIRECTORIES(${ALBEDO_SOURCE_DIR}/Operations)
  INCLUDE_DIRECTORIES(${ALBEDO_SOURCE_DIR}/Interaction)
  INCLUDE_DIRECTORIES(${ALBEDO_SOURCE_DIR}/VME)
  INCLUDE_DIRECTORIES(${ALBEDO_SOURCE_DIR}/Views)
  INCLUDE_DIRECTORIES(${ALBEDO_SOURCE_DIR}/Gui)
  #INCLUDE_DIRECTORIES(${ALBEDO_SOURCE_DIR}/vtkAPP)
  #INCLUDE_DIRECTORIES(${ALBEDO_BINARY_DIR}/vtkAPP)
  
  #SUBDIRS (vtkAPP)
  SUBDIRS (Application)
  SUBDIRS (Common)
  SUBDIRS (Operations)
  SUBDIRS (Interaction)  
  SUBDIRS (Views)
  SUBDIRS (VME)
  SUBDIRS (Gui)
  
  # Configuring Doxygen documentation  
  SUBDIRS (Documentation)
  
  # Export Build settings and library dependencies
  INCLUDE(CMakeExportBuildSettings)
  
  CMAKE_EXPORT_BUILD_SETTINGS(
    ${ALBEDO_BINARY_DIR}/AlbedoBuildSettings.cmake)
  
  export(TARGETS FILE ${ALBEDO_BINARY_DIR}/AlbedoLibraryDepends.cmake)

  CONFIGURE_FILE(
    ${ALBEDO_SOURCE_DIR}/CMakeInput/AlbedoConfig.cmake.in
    ${ALBEDO_BINARY_DIR}/AlbedoConfig.cmake
  @ONLY IMMEDIATE)
  
  CONFIGURE_FILE(
    ${ALBEDO_SOURCE_DIR}/CMakeInput/AlbedoUse.cmake.in
    ${ALBEDO_BINARY_DIR}/AlbedoUse.cmake
  @ONLY IMMEDIATE)
  
  #-----------------------------------------------------------------------------
  # Testing stuff
  #-----------------------------------------------------------------------------
  IF(BUILD_TESTING)
    
    MESSAGE("variable ALBEDO_SOURCE_DIR = " ${ALBEDO_SOURCE_DIR})
    FIND_PATH(ALBEDO_DATA_ROOT "TestingDataRoot.txt" "${ALBEDO_SOURCE_DIR}/Testing/UnitTestsData" "${ALBEDO_SOURCE_DIR}/../TestingData" "${ALBEDO_SOURCE_DIR}/../mmData/TestingData" "${ALBEDO_SOURCE_DIR}/../../TestingData"  DOC "Test data repository")
    ENABLE_TESTING()
      
    # where will executable tests be written ?
    IF (EXECUTABLE_OUTPUT_PATH)
        SET (CXX_TEST_PATH ${EXECUTABLE_OUTPUT_PATH})
    ELSE (EXECUTABLE_OUTPUT_PATH)
        SET (CXX_TEST_PATH .)
    ENDIF (EXECUTABLE_OUTPUT_PATH)
      
    SUBDIRS (Testing)
    
  ENDIF(BUILD_TESTING)
  
ELSE (MAF_FOUND)
  MESSAGE (FATAL_ERROR "MAF library not found, cannot build application!!")
ENDIF (MAF_FOUND)

#-----------------------------------------------------------------------------
# ALBEDO deploy stuff.  
#-----------------------------------------------------------------------------

# This is specific for NSIS Nullsoft Installer that runs currently under Windows only and Parabuild.
# An ALBEDO_VERSION variable is created to gather versioning information
IF (WIN32)
    
  # Installer useful values
  SET(ALBEDO_EXECUTABLE_OUTPUT_PATH "${EXECUTABLE_OUTPUT_PATH}/Release/" CACHE PATH "Source directory for Albedo executable. This is used by the installer to find the executable.")
  
  STRING(REPLACE "/" "\\" ALBEDO_EXECUTABLE_OUTPUT_PATH_WIN ${ALBEDO_EXECUTABLE_OUTPUT_PATH})
  SET(ALBEDO_EXECUTABLE_OUTPUT_PATH_WIN ${ALBEDO_EXECUTABLE_OUTPUT_PATH_WIN} CACHE INTERNAL "get the executable from this directory")
  
  # MESSAGE("variable value = " ${ALBEDO_EXECUTABLE_OUTPUT_PATH})
  # MESSAGE("variable value = " ${ALBEDO_EXECUTABLE_OUTPUT_PATH_WIN})
  
  # Test for environment variable reading
  # MESSAG# "$ENV{PATH}")

  # Automatic values from parabuild created temporary environment variable
  SET(BUILD_NUMBER $ENV{BUILD_NUMBER} CACHE INTERNAL "build number")
  
  # Create the main version variable:
  SET(ALBEDO_VERSION "Build_${BUILD_NUMBER}" CACHE INTERNAL "big version identifier concatenating other identifier")
  
  # MESSAGE("variable value = " ${ALBEDO_VERSION})
    
  # Config installer file
  # Set the installer directory variable
  SET(ALBEDO_INSTALLER_DIR ${PROJECT_SOURCE_DIR}/Installer)
  # Convert it to Windows format
  STRING (REPLACE "/" "\\" ALBEDO_INSTALLER_DIR_WIN ${ALBEDO_INSTALLER_DIR} )
  # MESSAGE("variable ALBEDO_INSTALLER_DIR_WIN = " ${ALBEDO_INSTALLER_DIR_WIN})
  
  # Installer executable will be written to this directory
  SET(ALBEDO_INSTALLER_EXECUTABLE_OUTPUT_DIR ${ALBEDO_EXECUTABLE_OUTPUT_PATH} CACHE PATH "the installer executable will be written to this directory ")
  # Convert it to Windows format  
  STRING(REPLACE "/" "\\" ALBEDO_INSTALLER_EXECUTABLE_OUTPUT_DIR_WIN ${ALBEDO_INSTALLER_EXECUTABLE_OUTPUT_DIR})
  SET(ALBEDO_INSTALLER_EXECUTABLE_OUTPUT_DIR_WIN ${ALBEDO_INSTALLER_EXECUTABLE_OUTPUT_DIR_WIN} CACHE INTERNAL "get the executable  from this directory")
  # MESSAGE("variable ALBEDO_INSTALLER_EXECUTABLE_OUTPUT_DIR_WIN = " ${ALBEDO_INSTALLER_EXECUTABLE_OUTPUT_DIR_WIN})
  
  # Installer Configuration
  CONFIGURE_FILE(${ALBEDO_INSTALLER_DIR}/AlbedoInstallerScript.nsi.in  ${ALBEDO_EXECUTABLE_OUTPUT_PATH}/AlbedoInstallerScript.nsi @ONLY)
  CONFIGURE_FILE(${MAF_BINARY_PATH}/INSTALLER/EnumUsersReg.nsh  ${ALBEDO_EXECUTABLE_OUTPUT_PATH}/EnumUsersReg.nsh @ONLY)
  CONFIGURE_FILE(${MAF_BINARY_PATH}/INSTALLER/InstallerCommon.nsh  ${ALBEDO_EXECUTABLE_OUTPUT_PATH}/InstallerCommon.nsh @ONLY)
  
  IF(EXISTS "${PROJECT_SOURCE_DIR}/ParabuildScripts/")
    CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/ParabuildScripts/tagCVS.bat.in ${PROJECT_SOURCE_DIR}/ParabuildScripts/tagCVS.bat @ONLY)
  ENDIF (EXISTS "${PROJECT_SOURCE_DIR}/ParabuildScripts/")
  
ENDIF (WIN32)