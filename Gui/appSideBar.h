/*=========================================================================
Program:   Albedo
Module:    appSideBar.h
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

#ifndef __appSideBar_H__
#define __appSideBar_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafAbsSideBar.h"
#include "mafEvent.h"
#include "mafGUICheckTree.h"
#include "mafGUIVMEChooserTree.h"
#include "mafObserver.h"

#include <wx/notebook.h>
#include <wx/splitter.h>
#include <wx/utils.h>

//----------------------------------------------------------------------------
// Forward reference
//----------------------------------------------------------------------------
class mafGUI;
class mafGUIHolder;
class mafGUIMDIFrame;
class mafGUINamedPanel;
class mafGUIPanel;
class mafGUIPanelStack;
class mafGUISplittedPanel;
class mafVME;
class mafView;
class wxListCtrl;

//----------------------------------------------------------------------------
// Class Name: appSideBar
//----------------------------------------------------------------------------
class appSideBar: public mafAbsSideBar
{
public:

	appSideBar(mafGUIMDIFrame* parent, int id, mafObserver *Listener);
	~appSideBar(); 
	
	// Add a new vme into the tree
	void VmeAdd(mafVME *vme);

	// Remove a vme from the tree
	void VmeRemove(mafVME *vme);

	// Notify to the tree that a vme is modified.
	void VmeModified(mafVME *vme);

	// Notify to the tree the visibility of the vme.
	void VmeShow(mafVME *vme, bool visibility);

	// Notify to the tree that the vme has been selected.
	void VmeSelected(mafVME *vme);
		
	// Show the operation's parameters gui on the tabbed panel.
	void OpShowGui(bool push_gui, mafGUIPanel *panel);

	// Hide the view/operation's gui from the tabbed panel.
	void OpHideGui(bool view_closed);

	// Plug the view settings on the tabbed panel.
	void ViewSelect(mafView *view);

	// Notify to the tree that a view has been deleted.
	void ViewDeleted(mafView *view);

	// Enable/Disable the vme selection when an operation ends/start.
	void EnableSelect(bool enable);

	// Switch the visibility of the panel.
	void Show();

	bool IsShown() {return m_Notebook->IsShown();};
  void Show(bool show) {m_Notebook->Show(show);};

	/** Respond to a VME_CHOOSE evt. Build a dialog containing the vme tree and return the vme choosed from the user. */
	std::vector<mafVME*> VmeChoose(long vme_accept_function = 0, long style = REPRESENTATION_AS_TREE, mafString title = "Choose Node", bool multiSelect = false, mafVME *vme = NULL);

	/** Open a Find VME dialog.*/
	void FindVME();

protected:

  void UpdateVmePanel();  

	wxNotebook        *m_Notebook;
	wxNotebook        *m_VmeNotebook;
	wxSplitterWindow	*m_SideSplittedPanel;

  mafGUIPanelStack	*m_OpPanel;
	mafGUIHolder			*m_ViewPropertyPanel;

	mafGUIHolder  *m_VmePanel;
  mafGUIHolder  *m_VmePipePanel;

  mafVME			*m_SelectedVme;
	mafVME			*m_OldSelectedVme;
  mafView     *m_SelectedView;
  mafObserver *m_Listener;

	mafGUI *m_AppendingGUI;
	mafGUI *m_CurrentVmeGui;
};
#endif
