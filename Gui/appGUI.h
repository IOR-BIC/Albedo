/*=========================================================================
Program:   Albedo
Module:    appGUI.h
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

#ifndef __appGUI_h
#define __appGUI_h

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "appDecl.h"
#include "appGuiDefines.h"

#include "mafGUI.h"

//----------------------------------------------------------------------------
// Forward references :
//----------------------------------------------------------------------------
class mafEventBase;
class mafInteractorCompositorMouse;
class appInteractorGenericMouse;
class appInteractionVolume;
class appGUIComponent;
class appGUITransformMouse;

//----------------------------------------------------------------------------
// Class Name: appGUI
//----------------------------------------------------------------------------
class APP_GUI_EXPORT appGUI : public mafGUI
{
public:

	appGUI(mafObserver *listener);
	~appGUI();

	void OnDClickList(wxCommandEvent& event);

	/* Create HintBox*/
	void HintBox(int id, wxString label, wxString title, bool showIcon);

	/*Create HyperLink*/
	void HyperLink(int id, wxString label, wxString url);

	/* Create Separator. Style 0 = empty space, 1 = line, 2 = empty space + line */
	void Separator(long style, const wxSize& size = wxDefaultSize);

private:
	DECLARE_EVENT_TABLE()
};

#endif
