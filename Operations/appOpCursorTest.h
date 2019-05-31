/*=========================================================================
Program:   Albedo
Module:    appOpCursorTest.h
Language:  C++
Date:      $Date: 2018-01-01 12:00:00 $
Version:   $Revision: 1.0.0.0 $
Authors:   Nicola Vanella
==========================================================================
Copyright (c) LTM-IOR 2018 (https://github.com/IOR-BIC)

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the above copyright notice for more information.
=========================================================================*/

#ifndef __appOpCursorTest_H__
#define __appOpCursorTest_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "appOperationsDefines.h"
#include "appDecl.h"
#include "mafOp.h"

class mafLogicWithManagers;
class mafView;
//----------------------------------------------------------------------------
// Forward references :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Class Name: appOpCursorTest
//----------------------------------------------------------------------------
class APP_OPERATIONS_EXPORT appOpCursorTest : public mafOp
{
public:
	/** Constructor. */
	appOpCursorTest(wxString label = "Op Empty", bool enable = true);

	/** Destructor. */
	~appOpCursorTest();

	/** RTTI macro. */
	mafTypeMacro(appOpCursorTest, mafOp);

	/** Return a copy of the operation */
	/*virtual*/ mafOp* Copy();

	/** Return true for the acceptable vme type. */
	/*virtual*/ bool Accept(mafVME *node);

	/** Builds operation's interface. */
	/*virtual*/ void OpRun();

	/** Execute the operation. */
	/*virtual*/ void OpDo();

	/** Receive events coming from the user interface.*/
	void OnEvent(mafEventBase *maf_event);
  
	void EnableOp(bool enable) { m_Enabled = enable; };

protected:

	/** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
	/*virtual*/ void OpStop(int result);	

	/** Create the Operation GUI */
	virtual void CreateGui();

	mafView* m_View;

	bool m_Enabled;
};

#endif
