/*=========================================================================
Program:   Albedo
Module:    appOpCreateSurfaceParametric.h
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

#ifndef __appOpCreateSurfaceParametric_H__
#define __appOpCreateSurfaceParametric_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "appOperationsDefines.h"
#include "mafOp.h"

//----------------------------------------------------------------------------
// Forward references :
//----------------------------------------------------------------------------
class appVMESurfaceParametric;
class mafEvent;

//----------------------------------------------------------------------------
// Class Name: appOpCreateSurfaceParametric
//----------------------------------------------------------------------------
class APP_OPERATIONS_EXPORT appOpCreateSurfaceParametric: public mafOp
{
public:
  appOpCreateSurfaceParametric(const wxString &label = "Create Parametric Surface");
  ~appOpCreateSurfaceParametric(); 

  mafTypeMacro(appOpCreateSurfaceParametric, mafOp);

  mafOp* Copy();

	/** Return true for the acceptable VME type. */
	/*virtual*/ bool Accept(mafVME *node);

	/** Builds operation's interface. */
	/*virtual*/ void OpRun();

	/** Execute the operation. */
	/*virtual*/ void OpDo();

protected: 
	/** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
	void OpStop(int result);

  //appVMESurfaceParametric *m_SurfaceParametric;
	appVMESurfaceParametric *m_SurfaceParametric;
};
#endif
