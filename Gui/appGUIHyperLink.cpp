/*=========================================================================
Program:   Albedo
Module:    appGUIHyperLink.cpp
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

#include "appDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the APP must include "appDefines.h" as first.
// This force to include Window, wxWidgets and VTK exactly in this order.
//----------------------------------------------------------------------------

#include "appGUIHyperLink.h"

#include "math.h"
#include "wx/event.h"
#include "albaLogicWithManagers.h"

BEGIN_EVENT_TABLE(appGUIHyperLink, wxControl)
  EVT_LEFT_DOWN(appGUIHyperLink::OnMouse)	
	EVT_LEFT_UP(appGUIHyperLink::OnMouse)
	EVT_LEAVE_WINDOW(appGUIHyperLink::OnMouse)
	EVT_ENTER_WINDOW(appGUIHyperLink::OnMouse)
END_EVENT_TABLE()

//----------------------------------------------------------------------------
appGUIHyperLink::appGUIHyperLink(wxWindow *parent, wxWindowID id, const wxString& label, const wxPoint& pos /*= wxDefaultPosition*/, const wxSize& size /*= wxDefaultSize*/, long style /*= 0*/, const wxString& name /*= wxStaticTextNameStr*/)
{
	this->Initialize();

	m_Listener = NULL;
	m_ValueString = label;
	m_URL = "";

	this->SetForegroundColour(wxColor(0, 0, 255));

	Create(parent, id, label, pos, size, style, name);

	wxFont underlineFont = wxFont(8, wxDEFAULT, wxNORMAL, wxNORMAL, true);
	this->SetFont(underlineFont);
	
}

//----------------------------------------------------------------------------
void appGUIHyperLink::Initialize()
{

}

//----------------------------------------------------------------------------
void appGUIHyperLink::OnMouse(wxMouseEvent &event)
{
	if (event.Entering())
	{
		this->SetForegroundColour(wxColor(0, 0, 155));
		SetLabel(m_ValueString);
	}
	else if (event.Leaving())
	{
		this->SetForegroundColour(wxColor(0, 0, 255));
		SetLabel(m_ValueString);
	}
	else if (event.ButtonDown())
	{
		this->SetForegroundColour(wxColor(255, 255, 0));
		SetLabel(m_ValueString);
	}
	else if (event.ButtonUp())
	{
		this->SetForegroundColour(wxColor(0, 0, 255));
		SetLabel(m_ValueString);

		if (m_URL != "")
			albaLogicWithManagers::ShowWebSite(m_URL);
	}
}
