/*=========================================================================
Program:   Albedo
Module:    appOpShowImage.cpp
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

#ifndef __appOpShowImage_H__
#define __appOpShowImage_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaOp.h"

//----------------------------------------------------------------------------
// Forward references :
//----------------------------------------------------------------------------
class albaGUICheckListBox;
class albaGUIValidator;

//----------------------------------------------------------------------------
// Class Name: appOpShowImage
//----------------------------------------------------------------------------
class ALBA_EXPORT appOpShowImage: public albaOp
{
public:

  appOpShowImage(const wxString &label = "Gui Sample");
  ~appOpShowImage(); 

  albaTypeMacro(appOpShowImage, albaOp);

  /** Receive events coming from the user interface.*/
  void OnEvent(albaEventBase *alba_event);
	
	/** Return a copy of the operation.*/
  albaOp* Copy();

  /** Return true for the acceptable vme type. */
  bool Accept(albaVME*node);

  /** Builds operation's interface. */
  void OpRun();
	
	std::vector<wxString> m_VectNames;
	std::vector<wxString> m_VectIds;

private:
	void CreateGui(int index);

	wxListBox *m_listBox;
	wxBitmapButton *m_BitmapButton;

};

#endif
