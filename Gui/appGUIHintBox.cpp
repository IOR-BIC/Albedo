/*=========================================================================
Program:   Albedo
Module:    appGUIHintBox.cpp
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

#include "appGUIHintBox.h"


//----------------------------------------------------------------------------
appGUIHintBox::appGUIHintBox(wxWindow *parent, wxWindowID id, wxString title, wxString label) : wxStaticBoxSizer(wxHORIZONTAL, parent, title)
{

#include "pic/HINT_IMAGE_ICON.xpm"

// 	GetStaticBox()->SetBackgroundColour(wxColor(0, 200, 0));
// 	GetStaticBox()->SetupColours();
// 	GetStaticBox()->SetOwnBackgroundColour(wxColor(0, 200, 0));

	Add(new wxBitmapButton(parent, NULL, wxBitmap(HINT_IMAGE_ICON_xpm), wxPoint(0, 0), wxSize(19, 20)));
	Add(new wxStaticText(parent, NULL, " "));
	Add(new wxStaticText(parent, NULL, label));	

}

