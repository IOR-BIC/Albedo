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
	OP_IMPORTER_IMAGE,
	OP_IMPORTER_STL,
	OP_IMPORTER_VTK,
	OP_EXPORTER_STL,
	OP_EXPORTER_VTK,
	OP_EMPTY,
// 	ID_OP_CREATE_EMPTY_VME,
// 	ID_OP_CREATE_SURFACE_PARAMETRIC,
// 	ID_OP_TRANSFORM,
// 	ID_OP_DICTIONARY,
// 	ID_OP_DIALOG_VERTICAL,
// 	ID_OP_DIALOG_HORIZONTAL,
 	ID_OP_INTERACTOR_TEST,
// 	ID_OP_INTERACTOR_SLIDER,
// 	ID_OP_GUI_SAMPLE,
 	ID_OP_CURSOR_TEST,
	ID_OP_ADD_MEASURE,
	ID_OP_ADD_COMMENT,
	ID_SHOW_VIEW_VTK_SURFACE,
	ID_SHOW_IMAGE_VIEW,
	ID_SHOW_SLICE_VIEW,
};

enum APP_VIEW_ID
{
	VIEW_VTK = VIEW_START,
	VIEW_IMAGE,
	VIEW_GLOBAL_SLICE,
};

#endif