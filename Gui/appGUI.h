/*=========================================================================
Program:   Albedo
Module:    appGUI.h
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

#ifndef __appGUI_h
#define __appGUI_h

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "appDecl.h"
#include "appGuiDefines.h"

#include "albaGUI.h"

//----------------------------------------------------------------------------
// Forward references :
//----------------------------------------------------------------------------
class albaEventBase;
class albaInteractorCompositorMouse;
class appInteractorGenericMouse;
class appInteractionVolume;
class appGUIComponent;
class appGUITransformMouse;
class albaGUIValidator;

//----------------------------------------------------------------------------
// Class Name: appGUI
//----------------------------------------------------------------------------
class APP_GUI_EXPORT appGUI : public albaGUI
{
public:

	appGUI(albaObserver *listener);
	~appGUI();

	void OnDClickList(wxCommandEvent& event);

	/* Create HintBox*/
	void HintBox(int id, wxString label, wxString title, bool showIcon);

	void Button2(int id, albaString label, albaString button_text, albaString tooltip = "", double labelW = 0.5);

	void TaskButton(int id, wxString label, bool status);
	/*Create HyperLink*/
	void HyperLink(int id, wxString label, wxString url);

	/* Create Separator. Style 0 = empty space, 1 = line, 2 = empty space + line */
	void Separator(long style, const wxSize& size = wxDefaultSize);

	/**/
	void Radio2(int id, wxString label, int *var, int numchoices = 0, const wxString choices[] = NULL, int dim = 1, wxString tooltip = "", int style = wxRA_SPECIFY_COLS);
	void RadioButton2(int id, wxString label, int selected = true, wxString tooltip = "");
	void MultipleRadioButtons(int numButtons, int numColumns, std::vector<int> &ids, std::vector<const char*> &labelsRows, std::vector<const char*> &labelsColumns, int *var);
	void RadioButtonGrid(int numRows, int numColumns, std::vector<int> &ids, std::vector<const char*> &labelsRows, std::vector<const char*> &labelsColumns, int *var);

	void MultipleRadioButtons(int numButtons, int numColumns, std::vector<int> &ids, std::vector<const char*> &labelsColumns, int *var, wxString label = "");
	void MultipleRadioButtons2(int numButtons, std::vector<int> &ids, int *var, wxString label = "", wxColour *bg_colour = NULL);
	void MultipleLabel(int numColumns, std::vector<const char*> &labels, bool bold = false);

	void Calendar(int day = -1, int month = -1, int year = -1, int dayOfWeek = -1);

	void Clock(int *hour = NULL, int *min = NULL, int *sec = NULL);
	void Clock(int id, wxString label, int var[3]);
	void Clock(int id, albaString *var);

private:
	DECLARE_EVENT_TABLE()
};

#endif
