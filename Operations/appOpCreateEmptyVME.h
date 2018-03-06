/*=========================================================================
Program:   Albedo
Module:    appOpCreateEmptyVME.h
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

#ifndef __appOpCreateEmptyVME_H__
#define __appOpCreateEmptyVME_H__

//----------------------------------------------------------------------------
// Includes :
//----------------------------------------------------------------------------
#include "appOperationsDefines.h"
#include "mafOp.h"

//----------------------------------------------------------------------------
// Forward references :
//----------------------------------------------------------------------------
class appEmptyVME;

//----------------------------------------------------------------------------
// Class Name: appOpCreateEmptyVME
//----------------------------------------------------------------------------
class APP_OPERATIONS_EXPORT appOpCreateEmptyVME: public mafOp
{
public:
  appOpCreateEmptyVME(const wxString &label = "Create VME Empty");
  ~appOpCreateEmptyVME(); 

  mafTypeMacro(appOpCreateEmptyVME, mafOp);

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

	appEmptyVME *m_EmptyVME;
};
#endif
