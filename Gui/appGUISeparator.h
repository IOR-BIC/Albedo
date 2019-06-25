/*=========================================================================
Program:   Albedo
Module:    appGUISeparator.cpp
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

#ifndef __appGUISeparator_H__
#define __appGUISeparator_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Class Name: appGUISeparator :
// appGUISeparator is a Label that notify user-click using the normal wxEvents.
//----------------------------------------------------------------------------
class appGUISeparator : public wxBoxSizer
{

public:

	//Constructor
  appGUISeparator(wxWindow *parent, long style = 0, const wxSize& size = wxDefaultSize);

protected:  

};
#endif // __appGUISeparator_H__
