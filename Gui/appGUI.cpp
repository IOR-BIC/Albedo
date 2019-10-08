/*=========================================================================
Program:   Albedo
Module:    appGUI.cpp
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

#include "appGUI.h"
#include "appGUIHyperLink.h"
#include "appGUISeparator.h"
#include "albaGUIValidator.h"

#include "albaPics.h"
#include "albaVME.h"

enum APP_GUI_ID
{
	ID_LIST_DOUBLE_CLICK = albaVME::ID_LAST,
};

BEGIN_EVENT_TABLE(appGUI, albaGUI)
EVT_LISTBOX_DCLICK(wxID_ANY, appGUI::OnDClickList)
END_EVENT_TABLE()


const int  M = 1;											// margin all around a row  
const int LM = 5;											// label margin             
const int HM = 2 * M;										// horizontal margin        (2)

const int LH = 18;											// label/entry height       
const int BH = 20;											// button height            

																				//const int LW	= 60;											// label width
																				//const int EW	= 60;											// entry width  - (was 48)  


const int LW = 55;	// label width Windows
const int EW = 45;											// entry width  - (was 48)  
const int FW = LW + LM + EW + HM + EW + HM + EW;		// full width               (304)
const int DW = EW + HM + EW + HM + EW;					// Data Width - Full Width without the Label (184)

static wxPoint dp = wxDefaultPosition;

//-------------------------------------------------------------------------
appGUI::appGUI(albaObserver *listener)
	:albaGUI(listener)
{
}

//-------------------------------------------------------------------------
appGUI::~appGUI()
{
}

//-------------------------------------------------------------------------
void appGUI::OnDClickList(wxCommandEvent& event)
{
	albaEventMacro(albaEvent(this, ID_LIST_DOUBLE_CLICK));
}

//----------------------------------------------------------------------------
void appGUI::HintBox(int id, wxString label, wxString title, bool showIcon)
{
	wxStaticBoxSizer *boxSizer = new wxStaticBoxSizer(wxHORIZONTAL, this, title);

	if (showIcon)
	{
		int w_id = GetWidgetId(id);
		wxBitmap bitmap = albaPictureFactory::GetPictureFactory()->GetBmp("HINT_IMAGE_ICON");
		wxBitmapButton *bmpButton = new wxBitmapButton(this, w_id, bitmap, wxPoint(0, 0), wxSize(19, 20));
		bmpButton->SetValidator(albaGUIValidator(this, w_id, bmpButton));

		boxSizer->Add(bmpButton);
		boxSizer->Add(new wxStaticText(this, NULL, " ")); // Separator
	}

	boxSizer->Add(new wxStaticText(this, NULL, label));

	Add(boxSizer);
}

//----------------------------------------------------------------------------
void appGUI::HyperLink(int id, wxString label, wxString url)
{
	int w_id = GetWidgetId(id);

	appGUIHyperLink  *link = NULL;
	link = new appGUIHyperLink(this, w_id, label);
	link->SetUrl(url);

	Add(link);
}

//----------------------------------------------------------------------------
void appGUI::Separator(long style, const wxSize& size /*= wxDefaultSize*/)
{
	Add(new appGUISeparator(this, style, size));
}

//----------------------------------------------------------------------------
void appGUI::Radio2(int id, wxString label, int* var, int numchoices, const wxString choices[], int dim, wxString tooltip, int style)
{
	wxRadioBox *radio = NULL;
	int w_id;

	if (!label.IsEmpty())
	{
		wxBoxSizer *sizer = NULL;
		sizer = new wxBoxSizer(wxHORIZONTAL);
		wxStaticText *lab = new wxStaticText(this, GetWidgetId(id), label + " ", dp, wxSize(LW, LH), wxALIGN_RIGHT);
		
		if (m_UseBackgroundColor) 
			lab->SetBackgroundColour(m_BackgroundColor);
		lab->SetFont(m_BoldFont);

		w_id = GetWidgetId(id);
		radio = new wxRadioBox(this, w_id, "", dp, wxSize(DW, -1), numchoices, choices, dim);// , style | m_EntryStyle | wxTAB_TRAVERSAL);

		sizer->Add(lab, 0, wxRIGHT, LM);
		sizer->Add(radio, 0, wxRIGHT, HM);
		Add(sizer, 0, wxALL, M);
	}
	else
	{
		w_id = GetWidgetId(id);
		radio = new wxRadioBox(this, w_id, "", dp, wxSize(FW, -1), numchoices, choices, dim, style | m_EntryStyle | wxTAB_TRAVERSAL);
		Add(radio, 0, wxRIGHT, HM);
	}

	if (m_UseBackgroundColor)
		radio->SetBackgroundColour(m_BackgroundColor);
	
	radio->SetValidator(albaGUIValidator(this, w_id, radio, var));
	radio->SetFont(m_Font);
	if (tooltip != "")
		radio->SetToolTip(tooltip);
}

//----------------------------------------------------------------------------
void appGUI::RadioButton2(int id, wxString label, int selected, wxString tooltip /*= ""*/)
{
	wxRadioButton *radioButton = NULL;
	int w_id = GetWidgetId(id);
	radioButton = new wxRadioButton(this, w_id, label);

	radioButton->SetValue(selected);

	this->Add(radioButton);
	if (tooltip != "")
		radioButton->SetToolTip(tooltip);
}

//----------------------------------------------------------------------------
void appGUI::RadioButtonGrid(int numRows, int numColumns, std::vector<int> &ids, std::vector<const char*> &labelsRows, std::vector<const char*> &labelsColumns, int *var)
{
	std::vector<int> w_ids;
	std::vector<wxRadioButton *> radio_list;
	for (int i = 0; i < ids.size(); i++)
	{
		wxRadioButton *radioButton = NULL;
		w_ids.push_back(GetWidgetId(ids[i]));
		radioButton = new wxRadioButton(this, w_ids[i], "", dp, wxSize(FW / numColumns, BH));
		radioButton->SetValue(var[i]);

		radio_list.push_back(radioButton);
	}

	wxStaticText* div = new wxStaticText(this, -1, "", dp, wxSize(-1, LH), wxALIGN_RIGHT | wxST_NO_AUTORESIZE);

	wxFlexGridSizer *fgSizer = new wxFlexGridSizer(numRows + 1, numColumns + 1, 1, 1);

	fgSizer->Add(div); // Add the position 0,0 an empty cell

	for (int i = 0; i < labelsColumns.size(); i++)
	{
		const char *a = labelsColumns[i];
		wxStaticText* lab = new wxStaticText(this, -1, labelsColumns[i], dp, wxSize(FW / (numColumns + 1), LH), wxALIGN_LEFT);
		fgSizer->Add(lab);
	}

	for (int i = 0, j = 0, k = 0; i < (numColumns + 1)*numRows; i++)
	{
		if (i % (numColumns + 1) != 0)
		{
			fgSizer->Add(radio_list[j]);
			j++;
		}
		else // Add label of rows
		{
			const char *a = labelsRows[k];
			wxStaticText* lab = new wxStaticText(this, -1, labelsRows[k], dp, wxSize(FW / (numColumns + 1), LH), wxALIGN_LEFT);
			fgSizer->Add(lab);
			k++;
		}
	}

	Add(fgSizer, 0, wxALL, M);
}

//----------------------------------------------------------------------------
void appGUI::MultipleRadioButtons(int numButtons, int numColumns, std::vector<int> &ids, std::vector<const char*> &labelsRows, std::vector<const char*> &labelsColumns, int *var)
{
	std::vector<int> w_ids;
	std::vector<wxRadioButton *> button_list;
	for (int i = 0; i < ids.size(); i++)
	{
		wxRadioButton *radioButton = NULL;
		w_ids.push_back(GetWidgetId(ids[i]));
		radioButton = new wxRadioButton(this, w_ids[i], labelsRows[i], dp, wxSize(FW / numColumns, BH));
		radioButton->SetValue(var[i]);

		button_list.push_back(radioButton);
	}

	int rows = numButtons / numColumns;
	wxFlexGridSizer *fgSizer = new wxFlexGridSizer(rows, numColumns + 1, 1, 1);

	if (labelsColumns.size() > 0)
	{
		wxStaticText* lab = new wxStaticText(this, -1, "", dp, wxSize(FW / (numColumns + 1), LH), wxALIGN_LEFT);
		fgSizer->Add(lab);
	}

	// Label Columns
	for (int i = 0; i < labelsColumns.size(); i++)
	{
		const char *a = labelsColumns[i];
		wxStaticText* lab = new wxStaticText(this, -1, labelsColumns[i], dp, wxSize(FW / (numColumns + 1), LH), wxALIGN_LEFT);
		fgSizer->Add(lab);
	}


	for (int i = 0; i < button_list.size(); i++)
	{
		if (i == 0)
		{
			// Row Label
			wxStaticText* lab = new wxStaticText(this, -1, labelsRows[i], dp, wxSize(FW / (numColumns + 1), LH), wxALIGN_LEFT);
			fgSizer->Add(lab);
		}

		fgSizer->Add(button_list[i], 0, 0);
	}

	Add(fgSizer, 0, wxALL | 240, M);
}

//----------------------------------------------------------------------------
void appGUI::MultipleRadioButtons(int numButtons, int numColumns, std::vector<int> &ids, std::vector<const char*> &labelsColumns, int *var, wxString label /*= ""*/)
{
	std::vector<int> w_ids;
	std::vector<wxRadioButton *> button_list;
	for (int i = 0; i < ids.size(); i++)
	{
		wxRadioButton *radioButton = NULL;
		w_ids.push_back(GetWidgetId(ids[i]));
		radioButton = new wxRadioButton(this, w_ids[i], "", dp, wxSize(FW / numColumns, BH));
		radioButton->SetValue(var[i]);

		button_list.push_back(radioButton);
	}

	std::vector<wxRadioBox *> box_list;
	for (int i = 0; i < ids.size(); i++)
	{
		const wxString choices2[] = { "0", "1", "2", "3", "" };
		int w_id = GetWidgetId(w_ids[0]);
		wxRadioBox *radio = new wxRadioBox(this, w_id, "", dp, wxSize(FW, -1), 4, choices2, 4);
		box_list.push_back(radio);
	}

	int rows = numButtons / numColumns;
	wxFlexGridSizer *fgSizer = new wxFlexGridSizer(rows, numColumns + 1, 1, 1);

	if (labelsColumns.size() > 0)
	{
		wxStaticText* lab = new wxStaticText(this, -1, "", dp, wxSize(FW / (numColumns + 1), LH), wxALIGN_LEFT);
		fgSizer->Add(lab);
	}

	// Label Columns
	for (int i = 0; i < labelsColumns.size(); i++)
	{
		const char *a = labelsColumns[i];
		wxStaticText* lab = new wxStaticText(this, -1, labelsColumns[i], dp, wxSize(FW / (numColumns + 1), LH), wxALIGN_LEFT);
		fgSizer->Add(lab);
	}


// 	for (int i = 0; i < button_list.size(); i++)
// 	{
// 		if (i == 0)
// 		{
// 			// Row Label
// 			wxStaticText* lab = new wxStaticText(this, -1, label, dp, wxSize(FW / (numColumns + 1), LH), wxALIGN_LEFT);
// 			fgSizer->Add(lab);
// 		}
// 
// 		fgSizer->Add(button_list[i], 0, 0);
// 	}

	for (int i = 0; i < box_list.size(); i++)
	{
		if (i == 0)
		{
			// Row Label
			wxStaticText* lab = new wxStaticText(this, -1, label, dp, wxSize(FW / (numColumns + 1), LH), wxALIGN_LEFT);
			fgSizer->Add(lab);
		}

		fgSizer->Add(box_list[i], 0, 0);
	}

	Add(fgSizer, 0, wxALL | 240, M);
}

//----------------------------------------------------------------------------
void appGUI::MultipleRadioButtons2(int numButtons, std::vector<int> &ids, int *var, wxString label /*= ""*/, wxColour *bg_colour)
{
	albaGUI *myGui = new albaGUI(this);
	
	int cellDim = FW / (numButtons + 1);

	std::vector<wxRadioButton *> button_list;
	for (int i = 0; i < ids.size(); i++)
	{
		int w_id = GetWidgetId(ids[i]);

		wxRadioButton *radioButton = new wxRadioButton(myGui, w_id, "", dp, wxSize(cellDim, BH));
		//radioButton->SetValidator(albaGUIValidator(this, w_id, radioButton));
		radioButton->SetValue(var[i]);
		button_list.push_back(radioButton);
	}

	wxFlexGridSizer *fgSizer = new wxFlexGridSizer(1, numButtons + 1, 1, 1);

	for (int i = 0; i < button_list.size(); i++)
	{
		if (i == 0)
		{
			// Row Label
			wxStaticText* lab = new wxStaticText(myGui, -1, label, dp, wxSize(cellDim, LH), wxALIGN_LEFT);
			lab->SetFont(m_BoldFont);
			fgSizer->Add(lab);
		}

		fgSizer->Add(button_list[i], 0, 0);
	}

	if (bg_colour != NULL)
		myGui->SetBackgroundColour(*bg_colour);

	myGui->Add(fgSizer, 0, wxALL, 5);	
	myGui->Update();
	myGui->FitGui();

	Add(myGui);
}

//----------------------------------------------------------------------------
void appGUI::MultipleLabel(int numColumns, std::vector<const char*> &labels, bool bold)
{
	int cellDim = FW / (numColumns + 1);
	
	wxFlexGridSizer *textSizer = new wxFlexGridSizer(1, numColumns + 1, 1, 1);

	// Label Columns
	for (int i = 0; i < labels.size(); i++)
	{
		wxStaticText* lab = new wxStaticText(this, -1, labels[i], dp, wxSize(cellDim, LH), wxALIGN_LEFT);
		if(bold) lab->SetFont(m_BoldFont);

		textSizer->Add(lab);
	}

	Add(textSizer, 0, wxALL, M);
}

//----------------------------------------------------------------------------
void appGUI::Calendar(int day, int month, int year, int dayOfWeek)
{
	int cellDim = FW / 10;

	time_t rawtime;
	time(&rawtime);
	struct tm * timeinfo = localtime(&rawtime);

	if (day == -1) day = timeinfo->tm_mday;
	if (month == -1) month = timeinfo->tm_mon + 1;
	if (year == -1) year = 1900 + timeinfo->tm_year;
	if (dayOfWeek == -1) dayOfWeek = timeinfo->tm_wday - 1;

	int d = 0;
	int numDays = 31;

	//
	wxString monthN;
	switch (month)
	{
	case 1: monthN = "Gen"; break;
	case 2: monthN = "Feb"; break;
	case 3: monthN = "Mer"; break;
	case 4: monthN = "Apr"; break;
	case 5: monthN = "Mag"; break;
	case 6: monthN = "Giu"; break;
	case 7: monthN = "Lug"; break;
	case 8: monthN = "Ago"; break;
	case 9: monthN = "Set"; break;
	case 10: monthN = "Ott"; break;
	case 11: monthN = "Nov"; break;
	case 12: monthN = "Dic"; break;
	}

	wxString date = monthN; date += " "; date += IntToString(year);
	Label(date, true);

	wxFlexGridSizer *textSizer = new wxFlexGridSizer(1, 7, 1, 1);

	for (int i = 1; i < 8; i++) // Days Name Columns
	{
		wxString dayN;
		switch (i)
		{
		case 1: dayN = "Lun"; break;
		case 2: dayN = "Mar"; break;
		case 3: dayN = "Mer"; break;
		case 4: dayN = "Gio"; break;
		case 5: dayN = "Ven"; break;
		case 6: dayN = "Sab"; break;
		case 7: dayN = "Dom"; break;
		}

		wxStaticText* lab = new wxStaticText(this, -1, dayN, dp, wxSize(cellDim, LH), wxALIGN_LEFT);

		wxStaticBoxSizer *boxSizer = new wxStaticBoxSizer(wxHORIZONTAL, this);
		boxSizer->Add(lab, LH, wxEXPAND);
		textSizer->Add(boxSizer);
	}


	for (int i = 0; i < dayOfWeek; i++) // Empty Columns
	{
		wxStaticText* lab = new wxStaticText(this, -1, " ", dp, wxSize(cellDim, LH), wxALIGN_LEFT);

		wxStaticBoxSizer *boxSizer = new wxStaticBoxSizer(wxHORIZONTAL, this);
		boxSizer->Add(lab, LH, wxEXPAND);
		textSizer->Add(boxSizer);
		d++;
	}

	switch (month)
	{
	case 2: numDays = 28; break;
	case 4:
	case 6:
	case 9:
	case 11: numDays = 30; break;
	}

	// Label Columns
	for (int i = 1; i < numDays + 1; i++)
	{
		wxString dayN = IntToString(i);

		if (i < 10) dayN = " " + dayN;
		wxStaticText* lab = new wxStaticText(this, -1, dayN, dp, wxSize(cellDim, LH), wxALIGN_LEFT);

		if (i == day)
		{
			lab->SetFont(m_BoldFont);
			lab->SetBackgroundColour(wxColour(0, 255, 0));
		}

		wxString dayName = "L";

		switch ((d) % 7)
		{
		case 0: dayName = "L"; break;
		case 1: dayName = "M"; break;
		case 2: dayName = "M"; break;
		case 3: dayName = "G"; break;
		case 4: dayName = "V"; break;
		case 5: dayName = "S"; break;
		case 6:
		{
			dayName = "D";
			lab->SetForegroundColour(wxColour(255, 0, 0));
			//lab->SetBackgroundColour(wxColour(200,200,200));
		} break;
		}

		wxStaticBoxSizer *boxSizer = new wxStaticBoxSizer(wxHORIZONTAL, this, dayName);
		boxSizer->Add(lab, LH, wxEXPAND);
		textSizer->Add(boxSizer);

		d++;
	}

	Add(textSizer, M, wxALL, 1);
}

//----------------------------------------------------------------------------
void appGUI::Clock(int* hour, int* min, int* sec)
{
	int cellDim = FW / 5;

	time_t rawtime;
	time(&rawtime);
	struct tm * timeinfo = localtime(&rawtime);

	int h = 0, m = 0, s = 0;

	if (hour == NULL) h = timeinfo->tm_hour; else h = *hour;
	if (min == NULL) m = timeinfo->tm_min; else m = *min;
	if (sec == NULL) s = timeinfo->tm_sec; else s = *sec;

	//
	wxFlexGridSizer *textSizer = new wxFlexGridSizer(1, 3, 1, 1);
	wxString text = " ";
	
	for (int i = 0; i < 3; i++)
	{
		if (i == 0) text = IntToString(h);
		if (i == 1) text = IntToString(m);
		if (i == 2) text = IntToString(s);

		wxStaticText* lab = new wxStaticText(this, -1, text, dp, wxSize(cellDim, LH), wxALIGN_RIGHT);
		//lab->SetValidator(albaGUIValidator(this, -1, lab, &text));
		lab->SetFont(m_BoldFont);
		//lab->SetBackgroundColour(wxColour(0, 255, 0));

		wxStaticBoxSizer *boxSizer = new wxStaticBoxSizer(wxHORIZONTAL, this);
		boxSizer->Add(lab, LH, wxEXPAND);
		textSizer->Add(boxSizer);
	}

	Add(textSizer, 0, wxALL, M);
}

//----------------------------------------------------------------------------
void appGUI::Clock(int id, wxString label, int var[3])
{
	int w_id;
	wxStaticText *lab = new wxStaticText(this, GetWidgetId(id), label, dp, wxSize(LW, LH), wxALIGN_RIGHT | wxST_NO_AUTORESIZE);
	if (m_UseBackgroundColor)
		lab->SetBackgroundColour(m_BackgroundColor);
	lab->SetFont(m_Font);

	w_id = GetWidgetId(id);
	wxTextCtrl  *text1 = new wxTextCtrl(this, w_id, "", dp, wxSize(EW, LH), m_EntryStyle);
	text1->SetValidator(albaGUIValidator(this, w_id, text1, var, MININT, MAXINT));
	text1->SetFont(m_Font);

	w_id = GetWidgetId(id);
	wxTextCtrl  *text2 = new wxTextCtrl(this, w_id, "", dp, wxSize(EW, LH), m_EntryStyle);
	text2->SetValidator(albaGUIValidator(this, w_id, text2, &(var[1]), MININT, MAXINT));
	text2->SetFont(m_Font);

	w_id = GetWidgetId(id);
	wxTextCtrl  *text3 = new wxTextCtrl(this, w_id, "", dp, wxSize(EW, LH), m_EntryStyle);
	text3->SetValidator(albaGUIValidator(this, w_id, text3, &(var[2]), MININT, MAXINT));
	text3->SetFont(m_Font);

	//if (tooltip != "")
	{
		text1->SetToolTip("h");
		text2->SetToolTip("m");
		text3->SetToolTip("s");
	}

// 	if (bg_colour != NULL)
// 	{
// 		text1->SetBackgroundColour(bg_colour[0]);
// 		text2->SetBackgroundColour(bg_colour[1]);
// 		text3->SetBackgroundColour(bg_colour[2]);
// 	}

	wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
	sizer->Add(lab, 0, wxRIGHT, LM);
	sizer->Add(text1, 0, wxRIGHT, HM);
	sizer->Add(text2, 0, wxRIGHT, HM);
	sizer->Add(text3, 0, wxRIGHT, HM);

	Add(sizer, 0, wxALL, M);
}

//----------------------------------------------------------------------------
void appGUI::Clock(int id, albaString *var)
{
// 	albaString *varTxt = NULL;
// 
// 	*varTxt = *IntToString(var[0]);
// 	varTxt += ':';
// 	varTxt += *IntToString(var[1]);
// 	varTxt += ':';
// 	varTxt += *IntToString(var[2]);

	int sw = LH;
	long e_style = m_EntryStyle;

	wxStaticText *lab = new wxStaticText(this, GetWidgetId(id), "", dp, wxSize(LW, LH), wxALIGN_RIGHT | wxST_NO_AUTORESIZE);
	if (m_UseBackgroundColor)
		lab->SetBackgroundColour(m_BackgroundColor);
	lab->SetFont(m_Font);

	int w_id = GetWidgetId(id);
	wxTextCtrl  *text = NULL;
	text = new wxTextCtrl(this, w_id, "", dp, wxSize(DW, sw), e_style);
	text->SetValidator(albaGUIValidator(this, w_id, text, var));
	text->SetFont(m_Font);

	wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
	sizer->Add(lab, 0, wxRIGHT, LM);
	sizer->Add(text, 0, wxRIGHT, HM);
	Add(sizer, 0, wxALL, M);
}
