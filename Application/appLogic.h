/*=========================================================================
Program:   Albedo
Module:    appLogic.h
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

#ifndef __appLogic_H__
#define __appLogic_H__

//----------------------------------------------------------------------------
// Includes :
//----------------------------------------------------------------------------
#include "mafLogicWithManagers.h"

//----------------------------------------------------------------------------
// Forward reference :
//----------------------------------------------------------------------------
class mafOpImporterDicom;

//----------------------------------------------------------------------------
// Class Name: appLogic
//----------------------------------------------------------------------------
class appLogic: public mafLogicWithManagers
{
public:

  /** Constructor. */
	appLogic();

  /** Destructor. */
	virtual ~appLogic(); 

	/*virtual*/ void OnEvent(mafEventBase *maf_event);
		
	/** Program Initialization */
	/*virtual*/ void Init(int argc, char **argv);

	void RunOp(mafOp *op);

	/** Respond to a VME_SELECT event. Update the selection on the tree and view representation.*/
	virtual void VmeSelect(mafVME *vme);

	/** Respond to a VME_ADDED event. propagate event. to SideBar, ViewManager, ecc.. */
	virtual void VmeAdded(mafVME *vme);

protected:
  
	/** Respond to a VME_SHOW event. Show/Hide the vme */
	virtual void VmeShow(mafVME *vme, bool visibility);
	  	
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

	// Operations
	mafOpImporterDicom	*m_OpImporterDicom;

private:
	//
};
#endif
