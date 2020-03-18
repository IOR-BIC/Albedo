/*=========================================================================
Program:   Albedo
Module:    appOpApplyTemplate.h
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

#ifndef __appOpApplyTemplate_H__
#define __appOpApplyTemplate_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "appOperationsDefines.h"
#include "appDecl.h"
#include "albaOp.h"

//----------------------------------------------------------------------------
// Forward references :
//----------------------------------------------------------------------------
class appInteractor2DTemplate;

//----------------------------------------------------------------------------
// Class Name: appOpApplyTemplate
//----------------------------------------------------------------------------
class APP_OPERATIONS_EXPORT appOpApplyTemplate : public albaOp
{
public:
	/** Constructor. */
	appOpApplyTemplate(wxString label = "Apply Template", bool enable = true);

	/** Destructor. */
	~appOpApplyTemplate();

	/** RTTI macro. */
	albaTypeMacro(appOpApplyTemplate, albaOp);

	/** Return a copy of the operation */
	/*virtual*/ albaOp* Copy();

	/** Return true for the acceptable vme type. */
	/*virtual*/ bool Accept(albaVME *node);

	/** Builds operation's interface. */
	/*virtual*/ void OpRun();

	/** Execute the operation. */
	/*virtual*/ void OpDo();

	/** Receive events coming from the user interface.*/
	void OnEvent(albaEventBase *alba_event);
  
	void EnableOp(bool enable) { m_Enabled = enable; };

protected:

	/** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
	/*virtual*/ void OpStop(int result);	

	/** Create the Operation GUI */
	virtual void CreateGui();

	/** Create and Initialize Interactor Template*/
	void CreateInteractor(albaView *view);

	appInteractor2DTemplate *m_InteractorTemplate;

	bool m_Enabled;
	int m_mode;
};
#endif
