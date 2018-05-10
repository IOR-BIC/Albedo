/*=========================================================================
Program:   Albedo
Module:    appGUI.cpp
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

#include "appGUI.h"
#include "appGUIHyperLink.h"
#include "appGUISeparator.h"
#include "mafGUIValidator.h"

#include "mafPics.h"
#include "mafVME.h"

enum APP_GUI_ID
{
	ID_LIST_DOUBLE_CLICK = mafVME::ID_LAST,
};

BEGIN_EVENT_TABLE(appGUI, mafGUI)
EVT_LISTBOX_DCLICK(wxID_ANY, appGUI::OnDClickList)
END_EVENT_TABLE()

//-------------------------------------------------------------------------
appGUI::appGUI(mafObserver *listener)
	:mafGUI(listener)
{
}

//-------------------------------------------------------------------------
appGUI::~appGUI()
{
}

//-------------------------------------------------------------------------
void appGUI::OnDClickList(wxCommandEvent& event)
{
	mafEventMacro(mafEvent(this, ID_LIST_DOUBLE_CLICK));
}

//----------------------------------------------------------------------------
void appGUI::HintBox(int id, wxString label, wxString title, bool showIcon)
{
	wxStaticBoxSizer *boxSizer = new wxStaticBoxSizer(wxHORIZONTAL, this, title);

	if (showIcon)
	{
		int w_id = GetWidgetId(id);
		wxBitmap bitmap = mafPictureFactory::GetPictureFactory()->GetBmp("HINT_IMAGE_ICON");
		wxBitmapButton *bmpButton = new wxBitmapButton(this, w_id, bitmap, wxPoint(0, 0), wxSize(19, 20));
		bmpButton->SetValidator(mafGUIValidator(this, w_id, bmpButton));

		boxSizer->Add(bmpButton);
		boxSizer->Add(new wxStaticText(this, NULL, " ")); // Separator
	}

	boxSizer->Add(new wxStaticText(this, NULL, label));

	Add(boxSizer);
}

//----------------------------------------------------------------------------
void appGUI::HyperLink(int id, wxString label, wxString url)
{
	int w_id = GetWidgetId(id);

	appGUIHyperLink  *link = NULL;
	link = new appGUIHyperLink(this, w_id, label);
	link->SetUrl(url);

	Add(link);
}

//----------------------------------------------------------------------------
void appGUI::Separator(long style, const wxSize& size /*= wxDefaultSize*/)
{
	Add(new appGUISeparator(this, style, size));
}

