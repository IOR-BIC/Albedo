/*=========================================================================
Program:   Albedo
Module:    appOpGuiSample.cpp
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

#ifndef __appOpGuiSample_H__
#define __appOpGuiSample_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaOp.h"

//----------------------------------------------------------------------------
// Forward references :
//----------------------------------------------------------------------------
class albaGUICheckListBox;
class albaGUIValidator;
class appGUI;

//----------------------------------------------------------------------------
// Class Name: appOpGuiSample
//----------------------------------------------------------------------------
class ALBA_EXPORT appOpGuiSample: public albaOp
{
public:

  appOpGuiSample(const wxString &label = "Gui Sample");
  ~appOpGuiSample(); 

  albaTypeMacro(appOpGuiSample, albaOp);

  /** Receive events coming from the user interface.*/
  void OnEvent(albaEventBase *alba_event);
	
	/** Return a copy of the operation.*/
  albaOp* Copy();

  /** Return true for the acceptable vme type. */
  bool Accept(albaVME*node);

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

	albaString m_String;
	albaString m_StringMultiline;

	int m_Interger;
	float m_Float;
	double m_Double;
	
	int m_IntVector[5];
	double m_DoubleVector[3];

	int h, m, s;

	int m_time[3];
	albaString m_text_time[3];
	albaString mTime;

	wxColor m_Color;

	wxListBox *m_listBox;
	albaGUI *m_Gui2;
	appGUI *m_Gui3;
};

#endif
