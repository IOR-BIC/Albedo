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

#ifndef __appGUIHyperLink_H__
#define __appGUIHyperLink_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaObserver.h"

//----------------------------------------------------------------------------
// Forward references :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Class Name: appGUIHyperLink
// Class that handles a slider for visualizing or changing range.
//----------------------------------------------------------------------------
class appGUIHyperLink : public wxStaticText
{
public:

  /** Constructor */
  appGUIHyperLink(wxWindow *parent, wxWindowID id, const wxString& label, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0, const wxString& name = wxStaticTextNameStr);

  /** Function for setting the listener of events coming from another object*/
  void SetListener(albaObserver *listener) {m_Listener = listener;}
	
	void SetUrl(wxString url) { m_URL = url; };

protected:

	/** Initialize the values for the slider. */
  void Initialize();
	
	/** Move the cursors on mouse event. */
  void OnMouse(wxMouseEvent &event);

	albaObserver *m_Listener;

private:

  wxString m_ValueString;
	wxString m_URL;

  /** declaring event table macro */
  DECLARE_EVENT_TABLE()
};
#endif // _appGUIHyperLink_H_
