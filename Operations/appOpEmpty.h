/*=========================================================================
Program:   Albedo
Module:    appOpEmpty.h
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

#ifndef __appOpEmpty_H__
#define __appOpEmpty_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "appOperationsDefines.h"
#include "appDecl.h"
#include "mafOp.h"

//----------------------------------------------------------------------------
// Forward references :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Class Name: appOpEmpty
//----------------------------------------------------------------------------
class APP_OPERATIONS_EXPORT appOpEmpty : public mafOp
{
public:
	/** Constructor. */
	appOpEmpty(wxString label = "Op Empty", bool enable = true);

	/** Destructor. */
	~appOpEmpty();

	/** RTTI macro. */
	mafTypeMacro(appOpEmpty, mafOp);

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

	bool m_Enabled;
};
#endif
