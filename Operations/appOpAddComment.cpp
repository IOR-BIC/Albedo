/*=========================================================================
Program:   Albedo
Module:    appOpAddComment.cpp
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

#include "appOpAddComment.h"

#include "appDecl.h"
#include "appInteractor2DMeasure_Comment.h"
#include "albaGUI.h"
#include "albaVME.h"

#include "albaGUIDialog.h"
#include "albaGUIValidator.h"
#include "albaTagArray.h"
#include <string>

// Begin For Testing

#include "albaVMEImage.h"
#include "albaVMEItem.h"
#include "albaVMEVolumeRGB.h"

#include "vtkBMPReader.h"
#include "vtkImageData.h"
#include "vtkImageFlip.h"
#include "vtkImageResample.h"
#include "vtkJPEGReader.h"
#include "vtkPNGReader.h"
#include "vtkPointData.h"
#include "vtkTIFFReader.h"
// End For Testing

#define MAX_NAME_CHARS 30
#define MAX_COMMENT_CHARS 570
#define MAX_AUTHOR_CHARS 19
#define MAX_DATETIME_CHARS 16 // Default
//----------------------------------------------------------------------------
albaCxxTypeMacro(appOpAddComment);

//----------------------------------------------------------------------------
appOpAddComment::appOpAddComment(wxString label, bool enable) :albaOp(label)
{
	m_OpType = OPTYPE_OP;
	m_Canundo = true;

	m_Enabled = enable;

	m_CommentListBox = NULL;
	m_CommentDict = NULL;
	m_CommentInteractor = NULL;
	
	m_Dialog = NULL;
	m_Name_textCtrl = NULL;
	m_Comment_textCtrl = NULL;
	m_Author_textCtrl = NULL;
	m_IsDialogOpened = false;

	m_CommentsVect.clear();

	m_Selected = -1;
	m_NameToEdit = "";
	m_CommentToEdit = "";
	m_AuthorToEdit = "";
	m_DateToEdit = "";
	m_CrationDate = "";
	m_ShowElement = 1;

	m_Current_Author = ""; // Check Author Name from settings TODO
}

//----------------------------------------------------------------------------
appOpAddComment::~appOpAddComment()
{
	m_CommentsVect.clear();

	if (m_Dialog)
		delete m_Dialog;
}

//----------------------------------------------------------------------------
bool appOpAddComment::Accept(albaVME *node)
{
	//return node->IsA("...");
	return m_Enabled;
}

//----------------------------------------------------------------------------
char ** appOpAddComment::GetIcon()
{
#include "pic/MENU_OP_COMMENT.xpm"
	return MENU_OP_COMMENT_xpm;
}

//----------------------------------------------------------------------------
albaOp* appOpAddComment::Copy()
{
	appOpAddComment *cp = new appOpAddComment(m_Label, m_Enabled);
	return cp;
}
//----------------------------------------------------------------------------
void appOpAddComment::OpRun()
{
	if (!m_Input->IsA("albaVMEImage"))
	{
		// Open Image View
		albaEventMacro(albaEvent(this, ID_SHOW_IMAGE_VIEW));

		ImportImage(); // For Testing
	}
	else
	{
		GetLogicManager()->VmeShow(m_Input, true);
		GetLogicManager()->VmeSelect(m_Input);
	}

	albaEvent e(this, VIEW_SELECTED);
	albaEventMacro(e);

	if (e.GetBool())
	{
		// Create Comment Interactor
		m_CommentInteractor = appInteractor2DMeasure_Comment::New();
		albaEventMacro(albaEvent(this, PER_PUSH, (albaObject *)m_CommentInteractor));
		m_CommentInteractor->SetListener(this);
		m_CommentInteractor->SetRendererByView(e.GetView());
	}

	if (!m_TestMode)
	{
		CreateGui();
	}

	// Load Comments from Tags
	Load();

	//TEST - Add Element
// 	CommentData newComment;
// 
// 	newComment.Name = "Test Nota";
// 	newComment.Comment = "Commento di Test";
// 	newComment.Author = "Nik";
// 	newComment.DateTime_Modify = "Now";
// 	newComment.Position[0] = 100.0;
// 	newComment.Position[1] = 100.0;
// 
// 	AddElement(newComment);
// 
// 	m_CommentInteractor->AddComment(newComment.Name, newComment.Comment, newComment.Author, newComment.DateTime_Modify, newComment.Position);
}
//----------------------------------------------------------------------------
void appOpAddComment::OpStop(int result)
{
	delete m_CommentDict;

	if (!m_TestMode)
	{
		HideGui();
	}

	HideDialog();

	if (result == OP_RUN_OK)
	{
		// Save Comments to Tags
		Save();
	}

	if (m_CommentInteractor)
	{
		// Remove Interactor
		albaEventMacro(albaEvent(this, PER_POP));
		albaDEL(m_CommentInteractor);
	}

	albaEventMacro(albaEvent(this, result));
}
//----------------------------------------------------------------------------
void appOpAddComment::OpDo()
{
}

/// EVENTS
//----------------------------------------------------------------------------
void appOpAddComment::OnEvent(albaEventBase *alba_event)
{
	if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
		if (e->GetSender() == m_Gui)
		{
			OnEvent_FromGui(alba_event);
		}
		else if (e->GetSender() == m_CommentInteractor)
		{
			Onevent_FromInteractor(alba_event);
		}
		else
		{
			switch (e->GetId())
			{
			// Dialog Events
			case ID_CLOSE_DIALOG:
			{
				m_NameToEdit = m_Name_textCtrl->GetValue();
				m_CommentToEdit = m_Comment_textCtrl->GetValue();
				m_AuthorToEdit = m_Author_textCtrl->GetValue();

				if (m_Current_Author = "") 
					m_Current_Author = m_AuthorToEdit;

				EditElement();
				HideDialog();
			}
			break;

			case ID_DEL_DIALOG:
			{
				DeleteElement();
				HideDialog();
			}
			break;

			default:
				Superclass::OnEvent(alba_event);
				break;
			}
		}
	}
}
//----------------------------------------------------------------------------
void appOpAddComment::OnEvent_FromGui(albaEventBase * alba_event)
{
	switch (alba_event->GetId())
	{
	case ID_SELECT_ELEMENT: // From ListBox
		SelectElement(m_CommentListBox->GetSelection());
	break;

	case ID_ADD_ELEMENT:
		NewElement();
	break;

	case ID_DELETE_ELEMENT:
		DeleteElement();
	break;

	case ID_EDIT_ELEMENT:
		EditElement();
	break;

	case ID_SHOW_ELEMENT:
		ShowHideElement();
	break;

	case ID_SHOW_DIALOG:
		ShowDialog();
	break;

	case ID_LOAD_COMMENTS:
		Load();
	break;

	case ID_SAVE_COMMENTS:
		Save();
	break;

	case wxOK:
		OpStop(OP_RUN_OK);
		break;

	case wxCANCEL:
		OpStop(OP_RUN_CANCEL);
		break;

	default:
		Superclass::OnEvent(alba_event);
		break;
	}
}
//----------------------------------------------------------------------------
void appOpAddComment::Onevent_FromInteractor(albaEventBase * alba_event)
{
	switch (alba_event->GetId())
	{
		// Interactor Events
	case appInteractor2DMeasure::ID_MEASURE_ADDED:
	{
		NewElement(); // To ListBox
		SelectElement(m_CommentsVect.size() - 1);
		ShowDialog();
	}
	break;
	case appInteractor2DMeasure::ID_MEASURE_CHANGED:
	{
		EditElement();
	}
	break;
	case appInteractor2DMeasure::ID_MEASURE_RCLICK:
	{
		int sel = m_CommentInteractor->GetSelectedMeasureIndex();
		ShowDialog();
	}
	break;
	case appInteractor2DMeasure::ID_MEASURE_SELECTED:
	{
		int sel = m_CommentInteractor->GetSelectedMeasureIndex();
		SelectElement(sel, false);
	}
	break;
	}
}

/// GUI
//----------------------------------------------------------------------------
void appOpAddComment::CreateGui()
{
	m_Gui = new albaGUI(this);

	// Comments List
	m_CommentListBox = m_Gui->ListBox(ID_SELECT_ELEMENT, "List", 200);	
	m_Gui->Divider(2);

// 	m_Gui->String(ID_EDIT_ELEMENT, _("Comment"), &m_NameToEdit);
// 	m_Gui->String(ID_EDIT_ELEMENT, _(""), &m_CommentToEdit, "", true, false, true, 100);
// 	m_Gui->String(ID_EDIT_ELEMENT, _("Author"), &m_AuthorToEdit, "", false, false, true, 12);
// 	m_Gui->String(ID_COMMENT, _("Date"), &m_DateToEdit);
// 	m_Gui->Integer(ID_COMMENT, "Selection", &m_Selected);
	
	m_Gui->Button(ID_DELETE_ELEMENT, "Delete");
	m_Gui->Button(ID_SHOW_ELEMENT, "Expand/Collapse");
	m_Gui->Button(ID_SHOW_DIALOG, "Edit");
	m_Gui->Divider(2);

	m_Gui->TwoButtons(ID_LOAD_COMMENTS, ID_SAVE_COMMENTS, "Load", "Save");

	//////////////////////////////////////////////////////////////////////////
	EnableDisableGui();

	m_Gui->Label("");
	m_Gui->Label("");
	m_Gui->Divider(1);

	m_Gui->OkCancel();
	m_Gui->Divider();

	ShowGui();
}
//----------------------------------------------------------------------------
void appOpAddComment::UpdateGui()
{
	if (!m_TestMode)
	{
		m_CommentListBox->Clear();
		for (int i=0; i<m_CommentsVect.size(); i++)
		{
			m_CommentListBox->Insert(m_CommentsVect[i].Name, i);
		}

		EnableDisableGui();
		m_Gui->Update();
	}
}
//----------------------------------------------------------------------------
void appOpAddComment::EnableDisableGui()
{
	int nElemts = m_CommentsVect.size();
	int elemPos = m_CommentListBox->GetSelection();

	bool canEditElem = (elemPos > -1) && (elemPos < nElemts);

	m_Gui->Enable(ID_EDIT_ELEMENT, canEditElem);
	m_Gui->Enable(ID_DELETE_ELEMENT, canEditElem);

	m_Gui->Enable(ID_COMMENT, false);

	m_Gui->Enable(ID_SHOW_DIALOG, elemPos >= 0 && !m_IsDialogOpened);
	m_Gui->Enable(ID_SAVE_COMMENTS, nElemts > 0);
	m_Gui->Enable(ID_LOAD_COMMENTS, nElemts == 0);
}

// ELEMENT
//----------------------------------------------------------------------------
void appOpAddComment::SelectElement(int selection, bool updateInteractor /*= false*/)
{
	if (m_Selected > -1 && m_Selected < m_CommentsVect.size())
	{
		m_CommentListBox->Deselect(m_Selected);
		m_CommentListBox->SetSelection(m_Selected, false);
	}

	if (selection > -1 && selection < m_CommentsVect.size())
	{
		m_CommentListBox->Select(selection);
		m_CommentListBox->SetSelection(selection, true);

		m_NameToEdit = m_CommentsVect[selection].Name;
		m_CommentToEdit = m_CommentsVect[selection].Comment;
		m_AuthorToEdit = m_CommentsVect[selection].Author;
		m_DateToEdit = m_CommentsVect[selection].DateTime_Modify;
		m_CrationDate = m_CommentsVect[selection].DateTime_Creation;

		if (m_CommentInteractor && updateInteractor)
			m_CommentInteractor->SelectMeasure(selection);
	}

	m_Selected = selection;

	if (!m_TestMode)
	{
		m_Gui->Update();
		EnableDisableGui();
	}
}
//----------------------------------------------------------------------------
void appOpAddComment::NewElement()
{
	wxString name = "Note";

	// Incremental Name
	char string[100];
	sprintf(string, "_%d", m_CommentsVect.size() + 1);
	name.append(string);

	// Date and Time
	time_t now = time(0);
	char timestamp[24] = "";
	strftime(timestamp, sizeof(timestamp), "%d/%m/%Y %H:%M", localtime(&now));

	// Create new Element
	CommentData newComment;
	newComment.Name = name;
	newComment.Comment = "Comment";
	newComment.Author = m_Current_Author;
	newComment.DateTime_Modify = timestamp;
	newComment.DateTime_Creation = timestamp;
	newComment.Position[0] = 0.0;
	newComment.Position[1] = 0.0;

	AddElement(newComment);

	if (m_CommentInteractor)
	{
		m_CommentInteractor->AddComment(name, newComment.Comment, newComment.Author, timestamp);

		int index = m_CommentInteractor->GetMeasureCount() - 1;
		double point[3];
		m_CommentInteractor->GetPosition(index, point);
		m_CommentsVect[index].Position[0] = point[0];
		m_CommentsVect[index].Position[1] = point[1];
	}
}
//----------------------------------------------------------------------------
void appOpAddComment::AddElement(CommentData info)
{
	m_CommentsVect.push_back(info);

	UpdateGui();
}
//----------------------------------------------------------------------------
void appOpAddComment::DeleteElement()
{
		if (m_Selected > -1 && m_Selected < m_CommentsVect.size())
		{
			m_CommentsVect.erase(m_CommentsVect.begin() + m_Selected);

			if (m_CommentInteractor)
			{
				m_CommentInteractor->RemoveComment(m_Selected);
			}

			UpdateGui();

			if (m_Selected > 0)
			{
				SelectElement(m_Selected - 1);
			}
			else
			{
				SelectElement(-1);
			}
		}
}
//----------------------------------------------------------------------------
void appOpAddComment::EditElement()
{
	if (m_Selected > -1 && m_Selected < m_CommentsVect.size())
	{
		if(m_NameToEdit.Length() > 0)
			m_CommentsVect[m_Selected].Name = m_NameToEdit;
		m_CommentsVect[m_Selected].Comment = m_CommentToEdit;
 		m_CommentsVect[m_Selected].Author = m_AuthorToEdit;

		// Update Date and Time
		time_t now = time(0);
		char timestamp[24] = "";
		strftime(timestamp, sizeof(timestamp), "%d/%m/%Y %H:%M", localtime(&now));
		m_CommentsVect[m_Selected].DateTime_Modify = m_DateToEdit = timestamp;
		
		if (m_CommentInteractor)
		{
			m_CommentInteractor->EditComment(m_Selected, m_CommentsVect[m_Selected].Name, m_CommentToEdit, m_AuthorToEdit, m_DateToEdit);

			double point[3];
			m_CommentInteractor->GetPosition(m_Selected, point);
			m_CommentsVect[m_Selected].Position[0] = point[0];
			m_CommentsVect[m_Selected].Position[1] = point[1];
		}
		
		UpdateGui();
	}
}
//----------------------------------------------------------------------------
void appOpAddComment::ShowHideElement()
{
	if (m_Selected > -1 && m_Selected < m_CommentsVect.size())
	{
		if (m_CommentInteractor)
			m_CommentInteractor->ShowComment(m_Selected, !m_CommentInteractor->IsVisible(m_Selected));
	}
}

/// FOR TESTING
//----------------------------------------------------------------------------
int appOpAddComment::ImportImage()
{
	// Build image sequence
	albaVMEImage *m_ImportedImage;
	albaNEW(m_ImportedImage);

	albaString wildc = "Images (*.bmp;*.jpg;*.png;*.tif)| *.bmp;*.jpg;*.png;*.tif";
	
	std::string fileName = albaGetOpenFile("", wildc.GetCStr(), "Open Image");

	if (fileName == "") return -1;

	wxString path, name, ext;
	wxSplitPath(fileName.c_str(), &path, &name, &ext);
	ext.MakeUpper();

	vtkImageReader2 *imageReader;

	if (ext == "BMP")
		imageReader = vtkImageReader::New();
	else if (ext == "JPG" || ext == "JPEG")
		imageReader = vtkJPEGReader::New();
	else if (ext == "PNG")
		imageReader = vtkPNGReader::New();
	else if (ext == "TIF" || ext == "TIFF")
		imageReader = vtkTIFFReader::New();
	else
	{
		wxMessageBox("Unable to import %s, unrecognized type", fileName.c_str());
		m_ImportedImage->ReparentTo(NULL);
		return -1;
	}

	imageReader->SetFileName(fileName.c_str());
	imageReader->Update();

	vtkALBASmartPointer<vtkImageFlip> imageFlipFilter;

	imageFlipFilter->SetFilteredAxis(1); // flip z axis
	imageFlipFilter->SetInput(imageReader->GetOutput());
	imageFlipFilter->Update();

	vtkImageData *grayScaleImage;
	vtkNEW(grayScaleImage);

	grayScaleImage->DeepCopy(imageFlipFilter->GetOutput());
	grayScaleImage->Update();

	for (int i = 0; i < grayScaleImage->GetNumberOfPoints(); i++)
	{
		double *values, meanValue;
		values = grayScaleImage->GetPointData()->GetScalars()->GetTuple3(i);
		meanValue = (values[0] + values[1] + values[2]) / 3.0;
		grayScaleImage->GetPointData()->GetScalars()->SetTuple3(i, meanValue, meanValue, meanValue);
	}
	grayScaleImage->Update();

	m_ImportedImage->SetData(grayScaleImage, 0);
	m_ImportedImage->SetName(name);

	m_ImportedImage->SetTimeStamp(0);
	m_ImportedImage->ReparentTo(m_Input);

	m_Output = m_ImportedImage;

	vtkDEL(grayScaleImage);
	vtkDEL(imageReader);

	// Set Tags
	albaTagItem tag_Nature;
	tag_Nature.SetName("VME_NATURE");
	tag_Nature.SetValue("NATURAL");

	m_Output->GetTagArray()->SetTag(tag_Nature);

	GetLogicManager()->VmeShow(m_ImportedImage, true);
	GetLogicManager()->VmeSelect(m_Output);

	albaDEL(m_ImportedImage);

	return 1;
}

/// DIALOG
//----------------------------------------------------------------------------
void appOpAddComment::CreateDialog()
{
	// Create Dialog
	if (!m_Dialog)
	{
		// Create Dialog 
		m_Dialog = new albaGUIDialog("Comment", albaCLOSEWINDOW);
		m_Dialog->SetListener(this);

		wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
		mainSizer->SetMinSize(wxSize(300, 200));
		//////////////////////////////////////////////////////////////////////////

		// Info
		m_Name_textCtrl = new wxTextCtrl(m_Dialog, ID_EDIT_ELEMENT, m_NameToEdit, wxPoint(-1, -1), wxSize(300, 25), wxALL | wxEXPAND);
		m_Name_textCtrl->SetEditable(true);
		m_Name_textCtrl->SetMaxLength(MAX_NAME_CHARS);
		m_Name_textCtrl->SetToolTip("Title - Max 30 characters");

		m_Comment_textCtrl = new wxTextCtrl(m_Dialog, ID_EDIT_ELEMENT, m_CommentToEdit, wxPoint(-1, -1), wxSize(300, 150), wxALL | wxEXPAND);
		m_Comment_textCtrl->SetEditable(true);
		m_Comment_textCtrl->SetMaxLength(MAX_COMMENT_CHARS);
		m_Comment_textCtrl->SetToolTip("Comment - Max 570 characters");

		m_Author_textCtrl = new wxTextCtrl(m_Dialog, ID_EDIT_ELEMENT, m_AuthorToEdit, wxPoint(-1, -1), wxSize(300, 25), wxALL | wxEXPAND);
		m_Author_textCtrl->SetEditable(true);
		m_Author_textCtrl->SetMaxLength(MAX_AUTHOR_CHARS);
		m_Author_textCtrl->SetToolTip("Author - Max 19 characters");


		m_Date_text = new wxStaticText(m_Dialog, -1, "", wxPoint(-1, -1), wxSize(-1, -1), wxALIGN_RIGHT);

		wxStaticBoxSizer *labelSizer1 = new wxStaticBoxSizer(wxVERTICAL, m_Dialog, "Edit");
		labelSizer1->Add(m_Name_textCtrl, 0, wxALL | wxEXPAND, 0);
		labelSizer1->Add(m_Comment_textCtrl, 0, wxALL | wxEXPAND, 0);
		labelSizer1->Add(m_Author_textCtrl, 0, wxALL | wxEXPAND, 0);
		labelSizer1->Add(m_Date_text, 0, wxALL | wxEXPAND, 0);

		mainSizer->Add(labelSizer1, 0, wxALL | wxEXPAND, 5);

		// Creating buttons
		wxBoxSizer *buttonSizer = new wxBoxSizer(wxHORIZONTAL);
		buttonSizer->SetMinSize(wxSize(300, 20));

		wxButton *delButton = new wxButton(m_Dialog, ID_DEL_DIALOG, "Delete");
		delButton->SetValidator(albaGUIValidator(this, ID_DEL_DIALOG, delButton));
		delButton->SetToolTip("Delete current Comment");
		buttonSizer->Add(delButton, 0, wxALIGN_LEFT, 5);

		wxBoxSizer *separatorSizer = new wxBoxSizer(wxHORIZONTAL);
		separatorSizer->SetMinSize(180, 20);
		buttonSizer->Add(separatorSizer, 0, wxEXPAND, 5);

		wxButton *okButton = new wxButton(m_Dialog, ID_CLOSE_DIALOG, "OK");
		okButton->SetValidator(albaGUIValidator(this, ID_CLOSE_DIALOG, okButton));
		okButton->SetToolTip("Confirm changes");
		buttonSizer->Add(okButton, 0, wxALIGN_RIGHT, 5);

		mainSizer->Add(buttonSizer, 0, wxALL | wxEXPAND, 5);
		
		//////////////////////////////////////////////////////////////////////////
		m_Dialog->Add(mainSizer, 0, wxALL);
		m_Dialog->Fit();

		// Show dialog
		wxSize s = albaGetFrame()->GetSize();
		wxPoint p = albaGetFrame()->GetPosition();
		int posX = p.x + s.GetWidth() * .5 - m_Dialog->GetSize().GetWidth() * .5;
		int posY = p.y + s.GetHeight() * .5 - m_Dialog->GetSize().GetHeight() * .5;
		m_Dialog->SetPosition(wxPoint(posX, posY));
	}
	else
	{
		UpdateDialog();
	}

	m_Dialog->Show(true);
}
//----------------------------------------------------------------------------
void appOpAddComment::UpdateDialog()
{
	wxString title = "";
	
	if (m_CrationDate != "")
		title = "Created " + m_CrationDate;

	if (m_DateToEdit != m_CrationDate)
		title = "Last Modify " + m_DateToEdit;
	
	m_Date_text->SetLabel(title);

	m_Name_textCtrl->SetValue(m_NameToEdit);
	m_Comment_textCtrl->SetValue(m_CommentToEdit);
	m_Author_textCtrl->SetValue(m_AuthorToEdit);
}
//----------------------------------------------------------------------------
void appOpAddComment::ShowDialog()
{
	if (m_IsDialogOpened)
		HideDialog();

	CreateDialog();

	m_IsDialogOpened = true;
}
//----------------------------------------------------------------------------
void appOpAddComment::HideDialog()
{
	if (m_Dialog)
		m_Dialog->Hide();

	m_IsDialogOpened = false;
}

/// LOAD/SAVE
//----------------------------------------------------------------------------
void appOpAddComment::Load()
{
	wxString tag = "info";
	albaVME *input = m_Input->GetRoot();

	double point[3] = { 0,0,0 };

	albaString info;
	albaString divider = "#";

	if (input->GetTagArray()->IsTagPresent("Comment_" + tag))
	{
		albaTagItem *measureTag = input->GetTagArray()->GetTag("Comment_" + tag);

		int nComponents = measureTag->GetNumberOfComponents();

		for (int i = 0; i < nComponents; i++)
		{
			info = measureTag->GetValue(i);

			// Parse string
			wxString wxInfo = info;
			int pos = wxInfo.Find(divider);
			albaString name = wxInfo.SubString(0, pos - 1);

			wxInfo = wxInfo.SubString(pos + 1, wxInfo.size());
			pos = wxInfo.Find(divider);
			albaString author = wxInfo.SubString(0, pos - 1);

			wxInfo = wxInfo.SubString(pos + 1, wxInfo.size());
			pos = wxInfo.Find(divider);
			albaString dateCreation = wxInfo.SubString(0, pos - 1);

			wxInfo = wxInfo.SubString(pos + 1, wxInfo.size());
			pos = wxInfo.Find(divider);
			albaString date = wxInfo.SubString(0, pos - 1);

			wxInfo = wxInfo.SubString(pos + 1, wxInfo.size());
			pos = wxInfo.Find(divider);
			wxString posX = wxInfo.SubString(0, pos - 1);

			wxInfo = wxInfo.SubString(pos + 1, wxInfo.size());
			pos = wxInfo.Find(divider);
			wxString posY = wxInfo.SubString(0, pos - 1);

			double val;
			posX.ToDouble(&val);
			point[0] = val;
			posY.ToDouble(&val);
			point[1] = val;

			albaString comment = wxInfo.SubString(pos + 1, wxInfo.size());

			// Created new Comment
			CommentData newComment;

			newComment.Name = name;
			newComment.Comment = comment;
			newComment.Author = author;
			newComment.DateTime_Creation = dateCreation;
			newComment.DateTime_Modify = date;
			newComment.Position[0] = point[0];
			newComment.Position[1] = point[1];

			AddElement(newComment); // To ListBox
			m_CommentInteractor->AddComment(name, comment, author, date, point); // To Interactor
		}
	}

	UpdateGui();
}
//----------------------------------------------------------------------------
void appOpAddComment::Save()
{
	wxString tag = "info";
	albaVME *input = m_Input->GetRoot();

	int nComments = m_CommentsVect.size();

	albaTagItem measureTag;
	measureTag.SetName("Comment_" + tag);
	measureTag.SetNumberOfComponents(nComments);

	//////////////////////////////////////////////////////////////////////////
	for (int i = 0; i < m_CommentsVect.size(); i++)
	{
		char tmp[1024];
		sprintf(tmp, "%s#%s#%s#%s#%.3f#%.3f#%s", m_CommentsVect[i].Name, m_CommentsVect[i].Author, m_CommentsVect[i].DateTime_Creation, m_CommentsVect[i].DateTime_Modify, m_CommentsVect[i].Position[0], m_CommentsVect[i].Position[1], m_CommentsVect[i].Comment);

		measureTag.SetValue(tmp, i);
	}

	if (input->GetTagArray()->IsTagPresent("Comment_" + tag))
		input->GetTagArray()->DeleteTag("Comment_" + tag);

	input->GetTagArray()->SetTag(measureTag);
}