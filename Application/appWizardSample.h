/*=========================================================================
Program:   Albedo
Module:    appWizardSample.h
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

#ifndef __appWizardSample_H__
#define __appWizardSample_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaWizard.h"

//----------------------------------------------------------------------------
// Forward references :
//----------------------------------------------------------------------------
class albaVME;
class albaVMEGizmo;
class mmiCompositorMouse;

//----------------------------------------------------------------------------
// Class Name: appWizardSample
//----------------------------------------------------------------------------
class appWizardSample : public albaWizard
{
public:

  /** Default constructor */
  appWizardSample(const wxString &label);
  
  /** Accept function */
  virtual bool Accept(albaVME* vme);
};
#endif








