/*=========================================================================
Program:   Albedo
Module:    appOpGuiSample.cpp
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

#ifndef __appOpGuiSample_H__
#define __appOpGuiSample_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafOp.h"

//----------------------------------------------------------------------------
// Forward references :
//----------------------------------------------------------------------------
class mafGUICheckListBox;
class mafGUIValidator;
class appGUI;

//----------------------------------------------------------------------------
// Class Name: appOpGuiSample
//----------------------------------------------------------------------------
class MAF_EXPORT appOpGuiSample: public mafOp
{
public:

  appOpGuiSample(const wxString &label = "Gui Sample");
  ~appOpGuiSample(); 

  mafTypeMacro(appOpGuiSample, mafOp);

  /** Receive events coming from the user interface.*/
  void OnEvent(mafEventBase *maf_event);
	
	/** Return a copy of the operation.*/
  mafOp* Copy();

  /** Return true for the acceptable vme type. */
  bool Accept(mafVME*node);

  /** Builds operation's interface. */
  void OpRun();
	
private:
	void CreateGui(int index);

	void GuiDivider();
	void GuiLabels();
	void GuiEntry();
	void GuiButtons();
	void GuiSliders();
	void GuiLists();
	void GuiExtra();
	void GuiClock();

	mafString m_String;
	mafString m_StringMultiline;

	int m_Interger;
	float m_Float;
	double m_Double;
	
	int m_IntVector[5];
	double m_DoubleVector[3];

	int h, m, s;

	int m_time[3];
	mafString m_text_time[3];
	mafString mTime;

	wxColor m_Color;

	wxListBox *m_listBox;
	mafGUI *m_Gui2;
	appGUI *m_Gui3;
};

#endif
