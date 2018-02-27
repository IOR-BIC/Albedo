/*=========================================================================
Program:   Albedo
Module:    appViewManager.cpp
Language:  C++
Date:      $Date: 2018-01-01 12:00:00 $
Version:   $Revision: 1.0.0.0 $
Authors:   Nicola Vanella
==========================================================================
Copyright (c) LTM-IOR 2018 (http://www.ltmsoftware.org/alba.htm)

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the above copyright notice for more information.
=========================================================================*/

#include "appDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the APP must include "appDefines.h" as first.
// This force to include Window, wxWidgets and VTK exactly in this order.
//----------------------------------------------------------------------------

#include "appViewManager.h"
#include "mafView.h"

//----------------------------------------------------------------------------
appViewManager::appViewManager()
{
}
//----------------------------------------------------------------------------
appViewManager::~appViewManager()
{	
}

//----------------------------------------------------------------------------
void appViewManager::VmeShow(mafVME *n, bool show)
{
	mafViewManager::VmeShow(n, show);
}

//----------------------------------------------------------------------------
void appViewManager::ViewDelete(mafView *view)
{
	bool selected = (view == m_SelectedView);
	mafViewManager::ViewDelete(view);

	if (selected && m_ViewList)
		OnEvent(&mafEvent(this, VIEW_SELECT, m_ViewList));
}
