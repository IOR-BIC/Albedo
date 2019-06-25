/*=========================================================================
Program:   Albedo
Module:    appViewManager.cpp
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

#include "appDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the APP must include "appDefines.h" as first.
// This force to include Window, wxWidgets and VTK exactly in this order.
//----------------------------------------------------------------------------

#include "appViewManager.h"
#include "albaView.h"

//----------------------------------------------------------------------------
appViewManager::appViewManager()
{
}
//----------------------------------------------------------------------------
appViewManager::~appViewManager()
{	
}

//----------------------------------------------------------------------------
void appViewManager::VmeShow(albaVME *n, bool show)
{
	albaViewManager::VmeShow(n, show);
}

//----------------------------------------------------------------------------
void appViewManager::ViewDelete(albaView *view)
{
	bool selected = (view == m_SelectedView);
	albaViewManager::ViewDelete(view);

	if (selected && m_ViewList)
		OnEvent(&albaEvent(this, VIEW_SELECT, m_ViewList));
}
