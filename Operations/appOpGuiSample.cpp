/*=========================================================================
Program:   Albedo
Module:    appOpGuiSample.cpp
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

#include "appOpGuiSample.h"
#include "mafDecl.h"
#include "mafEvent.h"
#include "mafGUI.h"
#include "mafGUICheckListBox.h"
#include "mafPics.h"
#include "mafGUIValidator.h"
#include "mafGUIDictionaryWidget.h"
#include "mafGUIImageViewer.h"

//----------------------------------------------------------------------------
// Constants:
//----------------------------------------------------------------------------
enum GUI_WIDGET_ID
{
	ID_GUI_WIDGET_FIRST = MINID,
	ID_GUI_WIDGET_SECOND,
	ID_GUI_WIDGET,
	ID_LIST_SELECTION,
};

//----------------------------------------------------------------------------
mafCxxTypeMacro(appOpGuiSample);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
appOpGuiSample::appOpGuiSample(const wxString &label) :
mafOp(label)
{
  m_OpType	= OPTYPE_OP;
  m_Canundo = true;

	m_listBox = NULL;
	m_Gui2 = NULL;

	// Data
	m_String = "String";
	m_StringMultiline = "String1\nString2\nString3";
	m_Interger = 0;
	m_Float = 0.0;
	m_Double = 0.0;
  m_DoubleVector[0] = m_DoubleVector[1] = m_DoubleVector[2] = 1.0;
	m_IntVector[0] = m_IntVector[1] = m_IntVector[2] = m_IntVector[3] = m_IntVector[4] = 77;
}
//----------------------------------------------------------------------------
appOpGuiSample::~appOpGuiSample()
{	
// 	cppDEL(m_Gui2);
// 	cppDEL(m_listBox);
}

//----------------------------------------------------------------------------
mafOp* appOpGuiSample::Copy()   
{
	return new appOpGuiSample(m_Label);
}
//----------------------------------------------------------------------------
bool appOpGuiSample::Accept(mafVME*node)
{
	return true;
}

//----------------------------------------------------------------------------
void appOpGuiSample::OpRun()
{
  m_Gui = new mafGUI(this);	

	m_listBox = m_Gui->ListBox(ID_LIST_SELECTION, "List", 100);

	m_listBox->Clear();
	m_listBox->Append("Divider");
	m_listBox->Append("Labels");
	m_listBox->Append("Entry");
	m_listBox->Append("Buttons");
	m_listBox->Append("Sliders");
	m_listBox->Append("Lists");
	m_listBox->Append("Extra");

	m_Gui->Label("");
	m_Gui->OkCancel();
	m_Gui->Divider(1);

	m_Gui2 = new mafGUI(this);
	m_Gui->AddGui(m_Gui2);

  ShowGui();
}

//----------------------------------------------------------------------------
void appOpGuiSample::OnEvent( mafEventBase *maf_event )
{
	int b = 1;
	int a = 0;

  switch (maf_event->GetId())
  {
    case ID_GUI_WIDGET_FIRST:
    case ID_GUI_WIDGET_SECOND:
    case ID_GUI_WIDGET:
			mafLogMessage("Event: %d - Sender: %s", maf_event->GetId(), maf_event->GetSender());
    break;

		case ID_LIST_SELECTION:
			CreateGui(m_listBox->GetSelection());
			break;

    case wxOK:
      OpStop(OP_RUN_OK);        
    break;
    case wxCANCEL:
      OpStop(OP_RUN_CANCEL);        
    break;
  }
}

//----------------------------------------------------------------------------
void appOpGuiSample::CreateGui(int index)
{
	m_Gui2->DestroyChildren();

	switch (index)
	{
	case 0:
		GuiDivider();
		break;
	case 1:
		GuiLabels();
		break;
	case 2:
		GuiEntry();
		break;
	case 3:
		GuiButtons();
		break;
	case 4:
		GuiSliders();
		break;
	case 5:
		GuiLists();
		break;
	case 6:
		GuiExtra();
		break;
	}
	
// 	m_Gui2->Hide();
// 	m_Gui2->Show();
	m_Gui2->FitGui();
	//m_Gui2->FitInside();
	m_Gui2->Update();

// 	m_Gui->Hide();
// 	m_Gui->Show();
	//m_Gui->Fit();
	m_Gui->FitGui();
	m_Gui->FitInside();
	m_Gui->Update();
}


// GUI WIDGETS

//----------------------------------------------------------------------------
void appOpGuiSample::GuiDivider()
{
	m_Gui2->Label("Divider", true);

	// Style 0/1
	m_Gui2->Divider();
	m_Gui2->Divider(1);
}
//----------------------------------------------------------------------------
void appOpGuiSample::GuiLabels()
{
	// Label, Bold, Multiline
	m_Gui2->Label("Label");
	m_Gui2->Divider(1);

	m_Gui2->Label("Label bold", true);
	m_Gui2->Divider(1);

	m_Gui2->Label("Label\nString1\nString2\nString3", true, false);
	m_Gui2->Divider(1);

	// Var, Bold_Var, Multiline_Var
	m_Gui2->Label(&m_String, true, false);
	m_Gui2->Divider(1);

	// Label1, Label2, Bold_Label1, Bold_Label2
	m_Gui2->Label("Label1", "Label2", true, false);
	m_Gui2->Divider(1);

	// Label, Var, Bold_Label, Bold_Var
	m_Gui2->Label("Label", &m_String);
	m_Gui2->Divider(1);

	m_Gui2->Label("Label", &m_StringMultiline, true, false);
	m_Gui2->Divider(1);
}
//----------------------------------------------------------------------------
void appOpGuiSample::GuiEntry()
{
	m_Gui2->Label("Entry", true);

	m_Gui2->String(ID_GUI_WIDGET, "Label", &m_String, "Tooltip");
	m_Gui2->String(ID_GUI_WIDGET, "Label Multiline", &m_StringMultiline, "Tooltip", true);
	m_Gui2->String(ID_GUI_WIDGET, "Password", &m_String, "Tooltip", false, true);
	m_Gui2->Divider(1);

	m_Gui2->Integer(ID_GUI_WIDGET, "Integer", &m_Interger, -100, 100, "Tooltip", false);
	m_Gui2->Float(ID_GUI_WIDGET, "Float", &m_Float);
	m_Gui2->Double(ID_GUI_WIDGET, "Double", &m_Double, 0.0);
	m_Gui2->DoubleUpDown(ID_GUI_WIDGET, ID_GUI_WIDGET_FIRST, ID_GUI_WIDGET_SECOND, "UpDown", &m_Double);
	m_Gui2->Divider(1);

	m_Gui2->Vector(ID_GUI_WIDGET, "Vector", m_DoubleVector, 0.0000000001);
	//m_Gui2->Vector(ID_SPACING, "Vector", m_Spacing, 0.0000000001, 100, -100, 100, -100, 100, -100, "Tooltip", &wxColor(255, 0, 0));
	
	//ID, Label, int|double var, numElem, min, max, tooltip 
	m_Gui2->VectorN(ID_GUI_WIDGET, "VectorN", m_IntVector, 5);// , -100.0, 100.0, "tooltip");

	m_Gui2->Bool(ID_GUI_WIDGET, "Bool", &m_Interger, 0, "Tooltip");
	m_Gui2->Divider(1);

	std::vector<int> idList;
	idList.push_back(ID_GUI_WIDGET);	idList.push_back(ID_GUI_WIDGET);	idList.push_back(ID_GUI_WIDGET);
	idList.push_back(ID_GUI_WIDGET);	idList.push_back(ID_GUI_WIDGET);	idList.push_back(ID_GUI_WIDGET);
	idList.push_back(ID_GUI_WIDGET);	idList.push_back(ID_GUI_WIDGET);	idList.push_back(ID_GUI_WIDGET);

	std::vector<const char*> vect1;
	vect1.push_back("Val A");
	vect1.push_back("Val B");
	vect1.push_back("Val C");

	std::vector<const char*> vect2;
	vect2.push_back("Val 1");
	vect2.push_back("Val 2");
	vect2.push_back("Val 3");

	int var[9];
	var[0] = 0; var[1] = 0; var[2] = 0; 
	var[3] = 0; var[4] = 1;	var[5] = 0; 
	var[6] = 0; var[7] = 0; var[8] = 0;

	m_Gui2->BoolGrid(3, 3, idList, vect1, vect2, var, "Tooltip");
	m_Gui2->Divider(1);

	// Id, Label, height, row, col, tooltip
	m_Gui2->Grid(ID_GUI_WIDGET, "Grid", 100, 5, 3, "Tooltip");
	m_Gui2->Divider(1);
}
//----------------------------------------------------------------------------
void appOpGuiSample::GuiButtons()
{
	m_Gui2->Label("Buttons", true);
	
	wxString choises[3] = { _("Choise1"),_("Choise2") ,_("Choise3") };

	// Id, Label, var, numChoises, choises, dim, tooltip
	m_Gui2->Radio(ID_GUI_WIDGET, "Radio", &m_Interger, 3, choises, 1, "Tooltip");
	m_Gui2->Divider(1);

	// Id, label, selected, tooltip
	m_Gui2->RadioButton(ID_GUI_WIDGET, "RadioButton", 1, "Tooltip");
	m_Gui2->RadioButton(ID_GUI_WIDGET, "RadioButton", 0, "Tooltip");
	m_Gui2->Divider(1);

	// Id, label, var, numChoises, choises, tooltip
	m_Gui2->Combo(ID_GUI_WIDGET, "Combo", &m_Interger, 3, choises, "Tooltip");
	m_Gui2->Divider(1);

	// Id, label, valColor, tooltip
	m_Gui2->Color(ID_GUI_WIDGET, "Color", &m_Color, "Tooltip");
	m_Gui2->Divider(1);

	// Lut TODO
	
	// Id, label, Text, tooltip
	m_Gui2->Button(ID_GUI_WIDGET, m_String, "Text", "Tooltip");
	m_Gui2->Button(ID_GUI_WIDGET, "Label", "", "Tooltip");
	
	// Id, text, label, tooltip
	m_Gui2->Button(ID_GUI_WIDGET, &m_String, "Label", "Tooltip");
	m_Gui2->Divider(1);

	// FirstId, SecondId, "Label1, Label2, alignment, width
	m_Gui2->TwoButtons(ID_GUI_WIDGET_FIRST, ID_GUI_WIDGET_SECOND, "Label1", "Label2");
	m_Gui2->TwoButtons(ID_GUI_WIDGET_FIRST, ID_GUI_WIDGET_SECOND, "Label1", "Label2", 240, 50);
	m_Gui2->Divider(1);

	wxBitmap bitmap = mafPictureFactory::GetPictureFactory()->GetBmp("NODE_BLUE");
	m_Gui2->ImageButton(ID_GUI_WIDGET, "Label", bitmap, "Tooltip");
	m_Gui2->Divider(1);

	// FirstId, SecondId, label, tooltip, alignment, width
	m_Gui2->ButtonAndHelp(ID_GUI_WIDGET_FIRST, ID_GUI_WIDGET_SECOND, "Label", "Tooltip");
	m_Gui2->Divider(1);

	std::vector<int> idList;
	idList.push_back(ID_GUI_WIDGET);	idList.push_back(ID_GUI_WIDGET);	idList.push_back(ID_GUI_WIDGET);
	idList.push_back(ID_GUI_WIDGET);	idList.push_back(ID_GUI_WIDGET);	idList.push_back(ID_GUI_WIDGET);

	std::vector<const char*> vect1;
	vect1.push_back("Val A");
	vect1.push_back("Val B");
	vect1.push_back("Val C");
	vect1.push_back("Val D");
	vect1.push_back("Val E");
	vect1.push_back("Val F");

	// nButtons, nColumns, idList, vectLabels, alignment
	m_Gui2->MultipleButtons(6, 3, idList, vect1);

}
//----------------------------------------------------------------------------
void appOpGuiSample::GuiSliders()
{
	m_Gui2->Label("Sliders", true);

	// Id, Label, var, min, max, tooltip, showText
	m_Gui2->Slider(ID_GUI_WIDGET, "Slider", &m_Interger);
	m_Gui2->Slider(ID_GUI_WIDGET, "Slider", &m_Interger, -100, 100, "Tooltip", false);
	m_Gui2->Divider(1);

	// Id, Label, var, min, max, size, tooltip, textboxEnable
	m_Gui2->FloatSlider(ID_GUI_WIDGET, "Float Slider", &m_Double, -100.0, 100.0);
	m_Gui2->FloatSlider(ID_GUI_WIDGET, "Float Slider", &m_Double, -100.0, 100.0, wxSize(100,50), "Tooltip", false);

	// Id, var, min, max, minLabel, maxLabel, size, tooltip, textboxEnable
	m_Gui2->FloatSlider(ID_GUI_WIDGET, &m_Double, -100.0, 100.0, "min", "max", wxDefaultSize, "Tooltip", true);

	// Id, Label, var, min, max, size, tooltip, textboxEnable
	m_Gui2->FloatExpandedSlider(ID_GUI_WIDGET, "Float Expanded", &m_Double, -100.0, 100.0);
}
//----------------------------------------------------------------------------
void appOpGuiSample::GuiLists()
{
	m_Gui2->Label("Lists", true);

	mafGUI *rollGui = new mafGUI(this);
	
	// Id, Label, height, tooltip
	mafGUICheckListBox *checkList= rollGui->CheckList(ID_GUI_WIDGET, "CheckList", 60, "Tooltip");
		// Id, Label, checked
		checkList->AddItem(ID_GUI_WIDGET, "Item1", false);
		checkList->AddItem(ID_GUI_WIDGET, "Item2", true);
		checkList->AddItem(ID_GUI_WIDGET, "Item3", false);	

		rollGui->Divider(1);

	wxListBox *listBox = rollGui->ListBox(ID_GUI_WIDGET, "ListBox", 60, "Tooltip");
		listBox->Append("Item1");
		listBox->Append("Item2");
		listBox->Append("Item3");

		rollGui->Divider(1);
	// Id, Label, height, tooltip, style, width
	//wxListCtrl *listCtrl = rollGui->ListCtrl(ID_GUI_WIDGET, "ListCtrl", 60, "Tooltip");
		// Index, label, imageIndex
// 		listCtrl->InsertItem(0, "Item1", 0);
// 		listCtrl->InsertItem(1, "Item2", 1);
// 		listCtrl->InsertItem(2, "Item3", 0);

		wxImageList *m_Images;
		m_Images = new wxImageList(15, 15, FALSE, 3);
		m_Images->Add(mafPictureFactory::GetPictureFactory()->GetBmp("NODE_GRAY"));
		m_Images->Add(mafPictureFactory::GetPictureFactory()->GetBmp("NODE_BLUE"));

// Id, Label, height, tooltip, style, width
		wxListCtrl *listCtrl = rollGui->ListCtrl(ID_GUI_WIDGET, "", 45, "Tooltip", wxVERTICAL);

		listCtrl->SetImageList(m_Images, wxIMAGE_LIST_SMALL);
		listCtrl->SetImageList(m_Images, wxIMAGE_LIST_NORMAL);
		
		// Index, label, imageIndex
		listCtrl->InsertItem(0, "Item1 - aaaaaaaaa bbbbbbbbbb ", 0);
		listCtrl->InsertItem(1, "Item2 - aaaaaaaaa bbbbbbbbbb ", 1);

		rollGui->Divider(1);

// ID, title, rollGui, open
		m_Gui2->RollOut(ID_GUI_WIDGET, "RollOut", rollGui, true);
}
//----------------------------------------------------------------------------
void appOpGuiSample::GuiExtra()
{
	m_Gui2->Label("Extra", true);

	wxString wildcard = _("Report log (*.log)|*.log");

	// Id, label, var, wildcard, tooltip, enableTextCtrl
	m_Gui2->FileSave(ID_GUI_WIDGET, "FileSave", &m_String, wildcard, "ToolTip");
	m_Gui2->FileSave(ID_GUI_WIDGET, "", &m_String, wildcard, "ToolTip", false);
	m_Gui2->Divider(1);

	// Id, label, var, wildcard, tooltip
	m_Gui2->FileOpen(ID_GUI_WIDGET, "FileOpen", &m_String, wildcard, "Tooltip");
	m_Gui2->Divider(1);

	// TEST
	
	wxBitmap bitmap = mafPictureFactory::GetPictureFactory()->GetBmp("NODE_RED");

	m_Gui2->ImageButton(ID_GUI_WIDGET, "Ciao", bitmap, "Tooltip");
	m_Gui2->Divider(1);
	// 
	// 	//
	// 	mafGUIDictionaryWidget *m_Dict;
	// 	m_Dict = new mafGUIDictionaryWidget(m_Gui2, -1);
	// 	m_Dict->SetListener(this);
	// 	//m_Dict->SetCloud(m_Cloud);
	// 	m_Dict->InitDictionary(NULL);
	// 	m_Dict->SetTitle("List");
	// 
	// 	m_Dict->AddItem("Item1");
	// 	m_Dict->AddItem("Item2");
	// 
	// 	//m_Gui2->Add(m_Dict->GetWidget(), wxEXPAND);
	// 	m_Gui->Divider(1);


	// 	mafGUIListCtrl *m_List;
	// 	m_List = new mafGUIListCtrl(m_Gui2, ID_GUI_WIDGET, false, true);
	// 	//m_List->Show(false);
	// 	m_List->SetListener(this);
	// 	m_List->SetSize(wxSize(199, 300));
	// 
	// 	m_List->AddItem("Item1", ITEM_BLUE);
	// 	m_List->AddItem("Item2", ITEM_RED);
	// 	m_List->AddItem("Item3", ITEM_GRAY);
	// 
	// 	m_Gui2->Add(m_List, 0, wxALL | wxEXPAND, 1);

	mafGUIImageViewer *immV = new mafGUIImageViewer(m_Listener);

}

