/*=========================================================================
Program:   Albedo
Module:    appOpShowDialog.h
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

#ifndef __appOpShowDialog_H__
#define __appOpShowDialog_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaOp.h"
#include "appOperationsDefines.h"

//----------------------------------------------------------------------------
// Forward references :
//----------------------------------------------------------------------------
class albaGUIButton;
class albaGUIDialog;
class albaGUIPicButton;
class wxBoxSizer;
class wxImage;

//----------------------------------------------------------------------------
// Class Name: appOpShowDialog
//----------------------------------------------------------------------------
class APP_OPERATIONS_EXPORT appOpShowDialog : public albaOp
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

  albaTypeMacro(appOpShowDialog, albaOp);

	virtual void OnEvent(albaEventBase *alba_event);

	albaOp* Copy();

	void OpRun();

	// Return true for the acceptable vme type.
	bool Accept(albaVME *node);

	void SetTitle(wxString title) { m_Title = title; };

	void SetDescription(wxString description) { m_Description = description; };

	void SetImagePath(wxString imagePath) { m_Title = imagePath; };

	void SetMode(DIALOG_MODE mode) { m_DialogMode = mode; };

	void ShowImage(bool show)		{ m_ShowImage = show; };
	void ShowButtons(bool show) { m_ShowButtons = show; };

protected:

	// This method is called at the end of the operation and result contain the wxOK or wxCANCEL.
	void OpStop(int result);

	void ShowDialog();

	wxString m_Title;
	wxString m_Description;
	wxString m_ImagePath;

	bool m_ShowImage;
	bool m_ShowButtons;

	DIALOG_MODE m_DialogMode;

	albaGUIDialog *m_dialog;

	albaGUIButton		*m_OkBtn;
	albaGUIButton		*m_CancelBtn;
	wxImage					*m_PreviewImage;
	albaGUIPicButton *m_PreviewImageButton;
};
#endif
