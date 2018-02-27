/*=========================================================================
Program:   Albedo
Module:    appWizardSample.h
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

#ifndef __appWizardSample_H__
#define __appWizardSample_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafWizard.h"

//----------------------------------------------------------------------------
// Forward references :
//----------------------------------------------------------------------------
class mafVME;
class mafVMEGizmo;
class mmiCompositorMouse;

//----------------------------------------------------------------------------
// Class Name: appWizardSample
//----------------------------------------------------------------------------
class appWizardSample : public mafWizard
{
public:

  /** Default constructor */
  appWizardSample(const wxString &label);
  
  /** Accept function */
  virtual bool Accept(mafVME* vme);
};
#endif








