/*=========================================================================
Program:   Albedo
Module:    appDecl.h
Language:  C++
Date:      $Date: 2018-01-01 12:00:00 $
Version:   $Revision: 1.0.0.0 $
Authors:   Nicola Vanella
==========================================================================
Copyright (c) LTM-IOR 2018 (https://github.com/IOR-LTM)

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the above copyright notice for more information.
=========================================================================*/

#ifndef __appDecl_H__
#define __appDecl_H__

#include "appDefines.h"
#include "mafDecl.h"

//----------------------------------------------------------------------------
// Class Name: appDecl
//----------------------------------------------------------------------------

enum APP_MAIN_EVENT_ID
{
	ID_SELECTED_PROSTHESIS = MED_EVT_USER_START,
};

enum APP_MENU_ID
{
  OP_IMPORTER_DICOM = MENU_VIEW_USER_FIRST,
	VIEW_VTK_SURFACE,
	VIEW_IMAGE,
};

#endif