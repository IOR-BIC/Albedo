/*=========================================================================
Program:   Albedo
Module:    appOpShowDialog.cpp
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

#include "appDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the APP must include "appDefines.h" as first.
// This force to include Window, wxWidgets and VTK exactly in this order.
//----------------------------------------------------------------------------

#include "appOpShowDialog.h"

#include "appGUIHyperLink.h"
#include "appGUILabel.h"
#include "appGUISeparator.h"
#include "appUtils.h"

#include "mafDecl.h"
#include "mafGUI.h"
#include "mafGUIButton.h"
#include "mafGUIDialog.h"
#include "mafGUIPicButton.h"
#include "mafVME.h"
#include "mafVMEIterator.h"

#include <wx/bitmap.h>
#include <wx/dir.h>
#include <wx/image.h>

//----------------------------------------------------------------------------
mafCxxTypeMacro(appOpShowDialog);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
appOpShowDialog::appOpShowDialog(const wxString &label, DIALOG_MODE mode):
	mafOp(label)
{
	m_dialog = NULL;
	m_OkBtn = NULL;
	m_CancelBtn = NULL;
	m_PreviewImage = NULL;
	m_PreviewImageButton = NULL;

	m_ShowImage = true;
	m_ShowButtons = true;

	m_Title = "Show Dialog";
	m_Description = "Content";
	m_ImagePath = "/Wizard/DialogV.bmp";

	m_DialogMode = mode;

	if(mode != VERTICAL_MODE)
		m_ImagePath = "/Wizard/DialogH.bmp";
}

//----------------------------------------------------------------------------
appOpShowDialog::~appOpShowDialog()
{ 
	delete m_OkBtn;
	delete m_CancelBtn;
	delete m_PreviewImage;
	delete m_PreviewImageButton;

	cppDEL(m_dialog);
}

//----------------------------------------------------------------------------
bool appOpShowDialog::Accept(mafVME *node)
{
	return true;
}

//----------------------------------------------------------------------------
mafOp* appOpShowDialog::Copy()
{
	appOpShowDialog *op = new appOpShowDialog(m_Label, m_DialogMode);
	op->m_OpType = m_OpType;
	op->m_Canundo = m_Canundo;
	return op;
}

//----------------------------------------------------------------------------
void appOpShowDialog::OpRun()
{
	ShowDialog();
	OpStop(OP_RUN_OK);
}

//----------------------------------------------------------------------------
void appOpShowDialog::OnEvent(mafEventBase *maf_event)
{
	if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
	{
		switch (e->GetId())
		{
		case ID_OK_BTN:
		{
			m_dialog->Close();
		}
		break;

		case ID_CANCEL_BTN:
		{
			m_dialog->Close();
		}
		break;

		default:
			mafEventMacro(*e);
			break;
		}
	}
}

//----------------------------------------------------------------------------
void appOpShowDialog::OpStop(int result)
{
	mafEventMacro(mafEvent(this, result));
}

//----------------------------------------------------------------------------
void appOpShowDialog::ShowDialog()
{
	int border = 5;

	if (m_dialog == NULL)
	{
		m_dialog = new mafGUIDialog(m_Title, mafCLOSEWINDOW);
		m_dialog->SetListener(this);

		int mainStyle = wxVERTICAL;

		if (m_DialogMode == HORIZONTAL_MODE || m_DialogMode == HORIZONTAL_MODE_2)
		{
			m_ImagePath = "/Wizard/DialogH.bmp"; // TEST
			mainStyle = wxHORIZONTAL;
		}

		wxBoxSizer *mainSizer = new wxBoxSizer(mainStyle);

		//////////////////////////////////////////////////////////////////////////
		// Image
		int imagePanelWidth = 560;
		int contentPanelWidth = 260;

		wxString imagesPath = appUtils::GetConfigDirectory().c_str();
		wxString imgPath = imagesPath + m_ImagePath;

		if (m_ShowImage && wxFileExists(imgPath))
		{
			wxBitmap *previewBitmap;

			// Load and show the image
			m_PreviewImage = new wxImage();
			m_PreviewImage->LoadFile(imgPath.c_str(), wxBITMAP_TYPE_ANY);

			previewBitmap = new wxBitmap(*m_PreviewImage);
			m_PreviewImageButton = new mafGUIPicButton(m_dialog, previewBitmap, -1);

			imagePanelWidth = m_PreviewImage->GetWidth();

			mainSizer->Add(m_PreviewImageButton, 0, wxALL | wxALIGN_CENTER, 0);

			delete previewBitmap;
		}

		//////////////////////////////////////////////////////////////////////////
		// Content
		wxBoxSizer *contentSizer = new wxBoxSizer(wxVERTICAL);

		// Label Description
		if (m_Description != "")
		{
			appGUILabel *lab = new appGUILabel(m_dialog, -1, m_Description);
			contentSizer->Add(lab, 0, wxALIGN_LEFT, 0);
		}

		// Separator
		appGUISeparator *sep1 = new appGUISeparator(m_dialog, 0, wxSize(1, 100));
		contentSizer->Add(sep1, 0, wxALIGN_LEFT, 0);

		wxBoxSizer *textSizer = new wxBoxSizer(wxHORIZONTAL);

		// Label
		appGUILabel *lab1 = new appGUILabel(m_dialog, -1, "Please visit ");
		textSizer->Add(lab1, 0, wxALIGN_LEFT, 0);

		// HyperLink
		appGUIHyperLink *hlink = new appGUIHyperLink(m_dialog, ID_LINK, "hipop-plan.com");
		hlink->SetUrl("http://www.hipop-plan.com/");
		textSizer->Add(hlink, 0, wxALIGN_LEFT, 0);

		// Label
		appGUILabel *lab2 = new appGUILabel(m_dialog, -1, " for more informations");
		textSizer->Add(lab2, 0, wxALIGN_LEFT, 0);

		contentSizer->Add(textSizer, 0, wxALL, 0);

		//////////////////////////////////////////////////////////////////////////
		// Creating buttons

		wxBoxSizer *buttonSizer = new wxBoxSizer(wxHORIZONTAL);

		m_OkBtn = new mafGUIButton(m_dialog, ID_OK_BTN, "Ok", wxPoint(-1, -1));
		m_OkBtn->SetListener(this);
		buttonSizer->Add(m_OkBtn, 0, wxALIGN_LEFT, 0);

		m_CancelBtn = new mafGUIButton(m_dialog, ID_CANCEL_BTN, "Cancel", wxPoint(-1, -1));
		m_CancelBtn->SetListener(this);
		buttonSizer->Add(m_CancelBtn, 0, wxALIGN_LEFT, 0);

		//////////////////////////////////////////////////////////////////////////
		if (m_DialogMode == HORIZONTAL_MODE_2)
		{
			mainSizer->Add(contentSizer, 0, wxALL, border);

			wxBoxSizer *mainSizer_2 = new wxBoxSizer(wxVERTICAL);

			mainSizer_2->Add(mainSizer, 0, wxALL, 0);

			if (m_ShowButtons)
			{
				// Line Separator
				appGUISeparator *sep3 = new appGUISeparator(m_dialog, 1, wxSize(imagePanelWidth + contentPanelWidth - (border * 2), 2));
				mainSizer_2->Add(sep3, 0, wxALIGN_LEFT, 0);
				mainSizer_2->Add(buttonSizer, 0, wxALL, 0);
			}

			// Add to Dialog
			m_dialog->Add(mainSizer_2, 0, wxALL);
		}
		else
		{
			if (m_ShowButtons)
			{
				// Line Separator
				appGUISeparator *sep3 = new appGUISeparator(m_dialog, 2, wxSize(imagePanelWidth - (border * 2), 100));
				contentSizer->Add(sep3, 0, wxALIGN_LEFT, 0);
				contentSizer->Add(buttonSizer, 0, wxALL, 0);
			}

			mainSizer->Add(contentSizer, 0, wxALL, border);

			// Add to Dialog
			m_dialog->Add(mainSizer, 0, wxALL);
		}

		m_dialog->Fit();
	}

	// Show dialog
	wxSize s = mafGetFrame()->GetSize();
	wxPoint p = mafGetFrame()->GetPosition();
	int posX = p.x + s.GetWidth() * .5 - m_dialog->GetSize().GetWidth() * .5;
	int posY = p.y + s.GetHeight() * .5 - m_dialog->GetSize().GetHeight() * .5;
	m_dialog->SetPosition(wxPoint(posX, posY));
	m_dialog->ShowModal();
}