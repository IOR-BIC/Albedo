/*=========================================================================
Program:   Albedo
Module:    appOpCreateEmptyVME.h
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

#ifndef __appOpCreateEmptyVME_H__
#define __appOpCreateEmptyVME_H__

//----------------------------------------------------------------------------
// Includes :
//----------------------------------------------------------------------------
#include "appOperationsDefines.h"
#include "albaOp.h"

//----------------------------------------------------------------------------
// Forward references :
//----------------------------------------------------------------------------
class appEmptyVME;

//----------------------------------------------------------------------------
// Class Name: appOpCreateEmptyVME
//----------------------------------------------------------------------------
class APP_OPERATIONS_EXPORT appOpCreateEmptyVME: public albaOp
{
public:
  appOpCreateEmptyVME(const wxString &label = "Create VME Empty");
  ~appOpCreateEmptyVME(); 

  albaTypeMacro(appOpCreateEmptyVME, albaOp);

  albaOp* Copy();

	/** Return true for the acceptable VME type. */
	/*virtual*/ bool Accept(albaVME *node);

	/** Builds operation's interface. */
	/*virtual*/ void OpRun();

	/** Execute the operation. */
	/*virtual*/ void OpDo();

protected: 
	/** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
	void OpStop(int result);

	appEmptyVME *m_EmptyVME;
};
#endif
