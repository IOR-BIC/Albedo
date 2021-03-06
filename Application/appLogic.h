/*=========================================================================
Program:   Albedo
Module:    appLogic.h
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

#ifndef __appLogic_H__
#define __appLogic_H__

//----------------------------------------------------------------------------
// Includes :
//----------------------------------------------------------------------------
#include "albaLogicWithManagers.h"

//----------------------------------------------------------------------------
// Forward reference :
//----------------------------------------------------------------------------
class albaOp;

//----------------------------------------------------------------------------
// Class Name: appLogic
//----------------------------------------------------------------------------
class appLogic: public albaLogicWithManagers
{
public:

  /** Constructor. */
	appLogic();

  /** Destructor. */
	virtual ~appLogic(); 

	/*virtual*/ void OnEvent(albaEventBase *alba_event);
		
	/** Program Initialization */
	/*virtual*/ void Init(int argc, char **argv);

	void RunOp(albaOp *op);

	albaOp* GetOp(int opId);

	/** Respond to a VME_SELECT event. Update the selection on the tree and view representation.*/
	virtual void VmeSelect(albaVME *vme);

	/** Respond to a VME_ADDED event. propagate event. to SideBar, ViewManager, ecc.. */
	virtual void VmeAdded(albaVME *vme);

protected:
  
	/** Respond to a VME_SHOW event. Show/Hide the vme */
	virtual void VmeShow(albaVME *vme, bool visibility);
	  	
  /** Redefined to add Meter controls*/
  virtual void CreateToolbar();
  
	/** Enable/Disable menu items when an operation start/end running */
	virtual void EnableMenuAndToolbar();

	/** Show arbitrary view vertical tiled with blend ct view */
  void ShowVMEOnView();
	
	/** A new View has Been Created - Logic must create the View Frame */
	virtual void ViewCreate(int viewId);

	/** Virtual method to create the side bar */
	virtual void CreateControlPanel();

	/** Redefined to add View,Op, Import, Export menu */
	virtual void CreateMenu();

	/** Show contextual menu for views when right mouse click arrives */
	virtual void ViewContextualMenu(bool vme_menu) {};

	/* Initialize About Dialog */
	void InitAboutDialog();

  bool m_OpeningMSF;

	// TaskBar
	albaAbsSideBar *m_TaskBar;

private:
	//
};
#endif
