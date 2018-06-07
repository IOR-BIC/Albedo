/*=========================================================================
Program:   Albedo
Module:    appGUIHintBox.cpp
Language:  C++
Date:      $Date: 2018-01-01 12:00:00 $
Version:   $Revision: 1.0.0.0 $
Authors:   Nicola Vanella
==========================================================================
Copyright (c) LTM-IOR 2018 (https://github.com/IOR-BIC)

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the above copyright notice for more information.
=========================================================================*/

#ifndef __appGUIHintBox_H__
#define __appGUIHintBox_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafObserver.h"

//----------------------------------------------------------------------------
// Forward references :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Class Name: appGUIHintBox
// Class that handles a slider for visualizing or changing range.
//----------------------------------------------------------------------------
class appGUIHintBox : public wxStaticBoxSizer
{
public:

  /** Constructor */
  appGUIHintBox(wxWindow *parent, wxWindowID id, wxString title, wxString label);

  /** Function for setting the listener of events coming from another object*/
  void SetListener(mafObserver *listener) {m_Listener = listener;}
	
	void SetUrl(wxString url) { m_URL = url; };
	wxString GetURL() { return m_URL; };

protected:
	
	mafObserver *m_Listener;

private:
	
	wxString m_URL;
};
#endif // _appGUIHintBox_H_
