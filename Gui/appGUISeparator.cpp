/*=========================================================================
Program:   Albedo
Module:    appGUISeparator.cpp
Language:  C++
Date:      $Date: 2018-01-01 12:00:00 $
Version:   $Revision: 1.0.0.0 $
Authors:   Nicola Vanella
==========================================================================
Copyright (c) LTM-IOR 2018 (http://www.ltmsoftware.org/alba.htm)

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the above copyright notice for more information.
=========================================================================*/

#include "appDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the APP must include "appDefines.h" as first.
// This force to include Window, wxWidgets and VTK exactly in this order.
//----------------------------------------------------------------------------

#include "appGUISeparator.h"

#include <wx/statline.h>

//----------------------------------------------------------------------------
appGUISeparator::appGUISeparator(wxWindow *parent, long style, const wxSize& size /*= wxDefaultSize*/) : wxBoxSizer(wxVERTICAL)
{
	if (style == 0) // Simple empty space
	{
		wxStaticText* div = new wxStaticText(parent, -1, "", wxDefaultPosition, size, 0);
		Add(div, 0, wxALL, 0);
	}

	if (style == 1) // Line space
	{
		wxStaticLine *div = new wxStaticLine(parent, -1, wxDefaultPosition, size);
		Add(div, 0, wxALL, 0);
	}

	if (style == 2) // Empty space + Line
	{
		wxStaticText* div1 = new wxStaticText(parent, -1, "", wxDefaultPosition, wxSize(size.GetX(), size.GetY() - 2), 0);
		Add(div1, 0, wxALL, 0);

		wxStaticLine *div2 = new wxStaticLine(parent, -1, wxDefaultPosition, wxSize(size.GetX(), 2));
		Add(div2, 0, wxALL, 0);
	}
}