/*=========================================================================
Program:   Albedo
Module:    appViewManager.h
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

#ifndef __appViewManager_H__
#define __appViewManager_H__

//----------------------------------------------------------------------------
// Includes :
//----------------------------------------------------------------------------
#include "mafViewManager.h"

//----------------------------------------------------------------------------
// Forward references :
//----------------------------------------------------------------------------
class mafVME;

//----------------------------------------------------------------------------
// Class Name: appViewManager
// Customized class managing showing custom VMEs in the selected view
//----------------------------------------------------------------------------
class appViewManager : public mafViewManager
{
public:

	/** Constructor */
	appViewManager();

	/** Destructor */
	~appViewManager();

	/** Show/Hide the VME */
	void VmeShow(mafVME *n, bool show);

	/** Remove the view from the ViewList and delete it */
	virtual void ViewDelete(mafView *view);

protected:
	//
};
#endif
