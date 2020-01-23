/*=========================================================================
Program:   Albedo
Module:    appDecl.h
Language:  C++
Date:      $Date: 2019-01-01 12:00:00 $
Version:   $Revision: 1.0.0.0 $
Authors:   Nicola Vanella
==========================================================================
Copyright (c) BIC-IOR 2019 (https://github.com/IOR-BIC)

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the above copyright notice for more information.
=========================================================================*/

#ifndef __appDecl_H__
#define __appDecl_H__

#include "appDefines.h"
#include "albaDecl.h"

//----------------------------------------------------------------------------
// Class Name: appDecl
//----------------------------------------------------------------------------

enum APP_MAIN_EVENT_ID
{
	ID_SELECTED_PROSTHESIS = MED_EVT_USER_START,
	MENU_VIEW_TASKBAR,
};

enum APP_MENU_ID
{
  OP_IMPORTER_DICOM = MENU_VIEW_USER_FIRST,
	ID_SHOW_VIEW_VTK_SURFACE,
	ID_SHOW_IMAGE_VIEW,
	ID_SHOW_SLICE_VIEW,
};

#endif