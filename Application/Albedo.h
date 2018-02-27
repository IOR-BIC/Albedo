/*=========================================================================
Program:   Albedo
Module:    Albedo.h
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

#ifndef __Albedo_H__
#define __Albedo_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "appLogic.h" 

//----------------------------------------------------------------------------
// Class Name: Albedo
//----------------------------------------------------------------------------
class Albedo : public wxApp
{
public:

	/**/
	bool OnInit();
	
	/**/
	int  OnExit();

	/** This function filters wxEvents and is used to control global hotKeys */
	int FilterEvent(wxEvent& event);

protected:

	/** Include icon files */
	void InitializeIcons();

	/** Initialize register */
	void InitializeReg();

	/**Manage Fatal Exception - Try to save project */
	void OnFatalException();

	appLogic *m_Logic;
};
DECLARE_APP(Albedo)
#endif 
