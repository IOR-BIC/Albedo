/*=========================================================================
Program:   Albedo
Module:    appOpTransformInteractor.h
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

#ifndef __appOpTransformInteractor_H__
#define __appOpTransformInteractor_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "appOperationsDefines.h"
#include "appDecl.h"
#include "albaOp.h"

//----------------------------------------------------------------------------
// Forward references :
//----------------------------------------------------------------------------
class appInteractor2DTransform;

//----------------------------------------------------------------------------
// Class Name: appOpTransformInteractor
//----------------------------------------------------------------------------
class APP_OPERATIONS_EXPORT appOpTransformInteractor : public albaOp
{
public:
	/** Constructor. */
	appOpTransformInteractor(wxString label = "Apply Template", bool enable = true);

	/** Destructor. */
	~appOpTransformInteractor();

	/** RTTI macro. */
	albaTypeMacro(appOpTransformInteractor, albaOp);

	/** Return a copy of the operation */
	/*virtual*/ albaOp* Copy();

	/** Builds operation's interface. */
	/*virtual*/ void OpRun();

	/** Execute the operation. */
	/*virtual*/ void OpDo();

	/** Receive events coming from the user interface.*/
	void OnEvent(albaEventBase *alba_event);
  
	void EnableOp(bool enable) { m_Enabled = enable; };

protected:

	/** Return true for the acceptable vme type. */
	/*virtual*/ bool InternalAccept(albaVME *node);

	/** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
	/*virtual*/ void OpStop(int result);	

	/** Create the Operation GUI */
	virtual void CreateGui();

	/** Create and Initialize Interactor Template*/
	void CreateInteractor(albaView *view);

	appInteractor2DTransform *m_InteractorTransform;

	bool m_Enabled;

	int m_mode;
	int m_scale_mode;
	int m_deformation_mode;
};
#endif
