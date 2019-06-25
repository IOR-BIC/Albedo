/*=========================================================================
Program:   Albedo
Module:    appGUILabel.cpp
Language:  C++
Date:      $Date: 2019-01-01 12:00:00 $
Version:   $Revision: 1.0.0.0 $
Authors:   Nicola Vanella
==========================================================================
Copyright (c) BIC-IOR 2019 (http://www.ltmsoftware.org/alba.htm)

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the above copyright notice for more information.
=========================================================================*/

#include "appDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the APP must include "appDefines.h" as first.
// This force to include Window, wxWidgets and VTK exactly in this order.
//----------------------------------------------------------------------------

#include "appGUILabel.h"

//----------------------------------------------------------------------------
// EVENT_TABLE
BEGIN_EVENT_TABLE(appGUILabel,wxStaticText)
    EVT_LEFT_DOWN(appGUILabel::OnLeftMouseButtonDown)
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(appGUILabel,wxStaticText)


//----------------------------------------------------------------------------
appGUILabel::appGUILabel()
{ }

//----------------------------------------------------------------------------
appGUILabel::appGUILabel(wxWindow *parent, wxWindowID id, const wxString& label, const wxPoint& pos /*= wxDefaultPosition*/, const wxSize& size /*= wxDefaultSize*/, long style /*= 0*/, const wxString& name /*= wxStaticTextNameStr*/)
{
	Create(parent, id, label, pos, size, style, name);
}

//----------------------------------------------------------------------------
void appGUILabel::OnLeftMouseButtonDown(wxMouseEvent &event)
{
	wxCommandEvent e(wxEVT_COMMAND_BUTTON_CLICKED, GetId());
	e.SetEventObject(this);
	ProcessEvent(e);
}
