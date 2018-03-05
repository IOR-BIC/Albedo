/*=========================================================================
Program:   Albedo
Module:    appGUILabel.cpp
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

#ifndef __appGUILabel_H__
#define __appGUILabel_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Class Name: appGUILabel :
// appGUILabel is a Label that notify user-click using the normal wxEvents.
//----------------------------------------------------------------------------
class appGUILabel : public wxStaticText
{
DECLARE_DYNAMIC_CLASS(appGUILabel)

public:

  appGUILabel();
  appGUILabel(wxWindow *parent, wxWindowID id, const wxString& label, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0, const wxString& name = wxStaticTextNameStr);

protected:  

  /** Notify the mouse click. */
	void OnLeftMouseButtonDown(wxMouseEvent &event);

DECLARE_EVENT_TABLE()
};
#endif // __appGUILabel_H__
