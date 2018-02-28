/*=========================================================================
Program:   Albedo
Module:    appOpCreateVMEEmpty.h
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

#ifndef __appOpCreateVMEEmpty_H__
#define __appOpCreateVMEEmpty_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "appOperationsDefines.h"
#include "mafOp.h"

//----------------------------------------------------------------------------
// Forward references :
//----------------------------------------------------------------------------
class appVMEEmpty;

/**
Class Name: appOpCreateVMEEmpty
*/
class APP_OPERATIONS_EXPORT appOpCreateVMEEmpty: public mafOp
{
public:
  appOpCreateVMEEmpty(const wxString &label = "Create VME Empty");
  ~appOpCreateVMEEmpty(); 

  mafTypeMacro(appOpCreateVMEEmpty, mafOp);

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

	appVMEEmpty *m_VMEEmpty;
};
#endif
