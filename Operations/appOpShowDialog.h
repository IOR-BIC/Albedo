/*=========================================================================
Program:   Albedo
Module:    appOpShowDialog.h
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

#ifndef __appOpShowDialog_H__
#define __appOpShowDialog_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafOp.h"
#include "appOperationsDefines.h"

//----------------------------------------------------------------------------
// Forward references :
//----------------------------------------------------------------------------
class mafGUIButton;
class mafGUIDialog;
class mafGUIPicButton;
class wxBoxSizer;
class wxImage;

//----------------------------------------------------------------------------
// Class Name: appOpShowDialog
//----------------------------------------------------------------------------
class APP_OPERATIONS_EXPORT appOpShowDialog : public mafOp
{
public:

	//----------------------------------------------------------------------------
	// Widgets ID's
	//----------------------------------------------------------------------------
	enum DIALOG_EVENT_ID
	{
		ID_OK_BTN = MINID,
		ID_CANCEL_BTN,
		ID_LINK,
	};

	/* Dialog Mode
	╔═══════╗ VERTICAL_MODE
	║ Imm.  ║ Image
	╠═══════╣
	║ Cont. ║ Contents + Buttons
	╚═══════╝
	╔═══╦═══╗ HORIZONTAL_MODE
	║ I ║ C ║
	║ m ║ o ║
	║ m ║ n ║ Contents + Buttons
	╚═══╩═══╝
	╔═══╦═══╗ HORIZONTAL_MODE_2
	║ I ║ C ║ 
	║ m ║ o ║ Contents
	╠═══╩═══╣
	╚═══════╝ Buttons*/

	enum DIALOG_MODE
	{
		VERTICAL_MODE,
		HORIZONTAL_MODE,
		HORIZONTAL_MODE_2
	};

	appOpShowDialog(const wxString &label = "Show Dialog", DIALOG_MODE mode = VERTICAL_MODE);
  ~appOpShowDialog();

  mafTypeMacro(appOpShowDialog, mafOp);

	virtual void OnEvent(mafEventBase *maf_event);

	mafOp* Copy();

	void OpRun();

	// Return true for the acceptable vme type.
	bool Accept(mafVME *node);

	void SetTitle(wxString title) { m_Title = title; };

	void SetDescription(wxString description) { m_Description = description; };

	void SetImagePath(wxString imagePath) { m_Title = imagePath; };

	void SetMode(DIALOG_MODE mode) { m_DialogMode = mode; };

protected:

	// This method is called at the end of the operation and result contain the wxOK or wxCANCEL.
	void OpStop(int result);

	void ShowDialog();

	wxString m_Title;
	wxString m_Description;
	wxString m_ImagePath;

	DIALOG_MODE m_DialogMode;

	mafGUIDialog *m_dialog;

	mafGUIButton		*m_OkBtn;
	mafGUIButton		*m_CancelBtn;
	wxImage					*m_PreviewImage;
	mafGUIPicButton *m_PreviewImageButton;
};
#endif
