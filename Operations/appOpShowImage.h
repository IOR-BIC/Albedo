/*=========================================================================
Program:   Albedo
Module:    appOpShowImage.cpp
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

#ifndef __appOpShowImage_H__
#define __appOpShowImage_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafOp.h"

//----------------------------------------------------------------------------
// Forward references :
//----------------------------------------------------------------------------
class mafGUICheckListBox;
class mafGUIValidator;

//----------------------------------------------------------------------------
// Class Name: appOpShowImage
//----------------------------------------------------------------------------
class MAF_EXPORT appOpShowImage: public mafOp
{
public:

  appOpShowImage(const wxString &label = "Gui Sample");
  ~appOpShowImage(); 

  mafTypeMacro(appOpShowImage, mafOp);

  /** Receive events coming from the user interface.*/
  void OnEvent(mafEventBase *maf_event);
	
	/** Return a copy of the operation.*/
  mafOp* Copy();

  /** Return true for the acceptable vme type. */
  bool Accept(mafVME*node);

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
