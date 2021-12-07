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
#include "albaPics.h"
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
	m_CommentInteractor = NULL;
	
	m_Dialog = NULL;
	m_ShowHide_ImageButton = NULL;
	m_Name_textCtrl = NULL;
	m_Comment_textCtrl = NULL;
	m_Author_textCtrl = NULL;
	m_IsDialogOpened = false;

	m_Selected = -1;

	m_Name = "";
	m_Comment = "";
	m_Author = "";
	m_EditDate = "";
	m_CrationDate = "";
	m_ShowElement = 1;

	m_Current_Author = ""; // Check Author Name from settings TODO
}

//----------------------------------------------------------------------------
appOpAddComment::~appOpAddComment()
{
	if (m_Dialog)
		delete m_Dialog;
}

//----------------------------------------------------------------------------
bool appOpAddComment::InternalAccept(albaVME *node)
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
	albaEvent e1(this, VIEW_SELECTED);
	albaEventMacro(e1);

	if (!e1.GetBool())
	{
		// Open Image View
		albaEventMacro(albaEvent(this, ID_SHOW_IMAGE_VIEW));
	}

	if (!m_Input->IsA("albaVMEImage"))
	{
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
}
//----------------------------------------------------------------------------
void appOpAddComment::OpStop(int result)
{
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
				m_Name = m_Name_textCtrl->GetValue();
				m_Comment = m_Comment_textCtrl->GetValue();
				m_Author = m_Author_textCtrl->GetValue();

				// Update Author
				if (m_Current_Author = "") 
					m_Current_Author = m_Author;

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

			case ID_SH_DIALOG:
				ShowHideElement();
				UpdateDialog();
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
		UpdateGui();

		SelectElement(m_CommentInteractor->GetMeasureCount() - 1);
		
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
		ShowDialog();
	}
	break;
	case appInteractor2DMeasure::ID_MEASURE_SELECTED:
	{
		SelectElement(m_CommentInteractor->GetSelectedMeasureIndex(), false);
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

	m_Gui->Button(ID_ADD_ELEMENT, "Add");
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
		for (int i = 0; i < m_CommentInteractor->GetMeasureCount(); i++)
		{
			m_CommentListBox->Insert(m_CommentInteractor->GetName(i).GetCStr(), i);
		}

		EnableDisableGui();
		m_Gui->Update();
	}
}
//----------------------------------------------------------------------------
void appOpAddComment::EnableDisableGui()
{
	int nElemts = m_CommentInteractor->GetMeasureCount();
	int sel = m_CommentListBox->GetSelection();

	bool canEditElem = (sel > -1) && (sel < nElemts);

	m_Gui->Enable(ID_EDIT_ELEMENT, canEditElem);
	m_Gui->Enable(ID_DELETE_ELEMENT, canEditElem);

	m_Gui->Enable(ID_COMMENT, false);

	m_Gui->Enable(ID_SHOW_DIALOG, sel >= 0 && !m_IsDialogOpened);
	m_Gui->Enable(ID_SAVE_COMMENTS, nElemts > 0);
	m_Gui->Enable(ID_LOAD_COMMENTS, nElemts == 0);
}

/// ELEMENT
//----------------------------------------------------------------------------
void appOpAddComment::SelectElement(int selection, bool updateInteractor /*= false*/)
{
	if (m_Selected > -1 && m_Selected < m_CommentInteractor->GetMeasureCount())
	{
		m_CommentListBox->Deselect(m_Selected);
		m_CommentListBox->SetSelection(m_Selected, false);
	}

	if (selection > -1 && selection < m_CommentInteractor->GetMeasureCount())
	{
		m_CommentListBox->Select(selection);
		m_CommentListBox->SetSelection(selection, true);

		m_Name = m_CommentInteractor->GetName(selection);
		m_Comment = m_CommentInteractor->GetComment(selection);
		m_Author = m_CommentInteractor->GetAuthor(selection);
		m_EditDate = m_CommentInteractor->GetDateTime(selection);
		m_CrationDate = m_CommentInteractor->GetCreationDateTime(selection);

		if (updateInteractor)
			m_CommentInteractor->SelectComment(selection);
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
	double position[3] = { 100.0, 100.0, 0.0 };

	// Add Comment
	m_CommentInteractor->AddComment(position, "New Note", "Comment", "", "", "");

	UpdateGui();
	SelectElement(m_CommentInteractor->GetMeasureCount() - 1);
	ShowDialog();
}
//----------------------------------------------------------------------------
void appOpAddComment::DeleteElement()
{
	if (m_Selected > -1 && m_Selected < m_CommentInteractor->GetMeasureCount())
	{
		m_CommentInteractor->RemoveComment(m_Selected);

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
	if (m_Selected > -1 && m_Selected < m_CommentInteractor->GetMeasureCount())
	{
		if (m_Name.Length() > 0)
			m_CommentInteractor->SetName(m_Selected, m_Name);
		m_CommentInteractor->SetComment(m_Selected, m_Comment);
		m_CommentInteractor->SetAuthor(m_Selected, m_Author);

		// Update Date and Time
		time_t now = time(0);
		char timestamp[24] = "";
		strftime(timestamp, sizeof(timestamp), "%d/%m/%Y %H:%M", localtime(&now));
		m_EditDate = timestamp;

		m_CommentInteractor->SetDateTime(m_Selected, m_EditDate);

		UpdateGui();
	}
}
//----------------------------------------------------------------------------
void appOpAddComment::ShowHideElement()
{
	if (m_Selected > -1 && m_Selected < m_CommentInteractor->GetMeasureCount())
		m_CommentInteractor->ShowComment(m_Selected, !m_CommentInteractor->IsVisible(m_Selected));
}

/// FOR TESTING - IMPORT IMAGE
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

	m_Input = m_Output;

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
		m_Name_textCtrl = new wxTextCtrl(m_Dialog, ID_EDIT_ELEMENT, m_Name, wxPoint(-1, -1), wxSize(300, 25), wxALL | wxEXPAND);
		m_Name_textCtrl->SetEditable(true);
		m_Name_textCtrl->SetMaxLength(MAX_NAME_CHARS);
		m_Name_textCtrl->SetToolTip("Title - Max 30 characters");

		m_Comment_textCtrl = new wxTextCtrl(m_Dialog, ID_EDIT_ELEMENT, m_Comment, wxPoint(-1, -1), wxSize(300, 150), wxALL | wxEXPAND);
		m_Comment_textCtrl->SetEditable(true);
		m_Comment_textCtrl->SetMaxLength(MAX_COMMENT_CHARS);
		m_Comment_textCtrl->SetToolTip("Comment - Max 570 characters");

		m_Author_textCtrl = new wxTextCtrl(m_Dialog, ID_EDIT_ELEMENT, m_Author, wxPoint(-1, -1), wxSize(300, 25), wxALL | wxEXPAND);
		m_Author_textCtrl->SetEditable(true);
		m_Author_textCtrl->SetMaxLength(MAX_AUTHOR_CHARS);
		m_Author_textCtrl->SetToolTip("Author - Max 19 characters");

		m_Date_text = new wxStaticText(m_Dialog, -1, "", wxPoint(-1, -1), wxSize(-1, -1));

		//
		wxStaticBoxSizer *boxSizer = new wxStaticBoxSizer(wxVERTICAL, m_Dialog, "Edit");
		boxSizer->Add(m_Name_textCtrl, 0, wxALL | wxEXPAND, 0); // NAME INFO
		boxSizer->Add(m_Comment_textCtrl, 0, wxALL | wxEXPAND, 0); // COMMENT INFO

		wxBoxSizer *boxSizer2 = new wxBoxSizer(wxHORIZONTAL);
		boxSizer2->Add(m_Author_textCtrl, 0, wxEXPAND, 0); // AUTHOR INFO

		m_ShowHide_ImageButton = new wxBitmapButton(m_Dialog, ID_SH_DIALOG, albaPictureFactory::GetPictureFactory()->GetBmp("COMMENT_SHOW_ICON"));
		m_ShowHide_ImageButton->SetValidator(albaGUIValidator(this, ID_SH_DIALOG, m_ShowHide_ImageButton));
		m_ShowHide_ImageButton->SetToolTip("Show/Hide");
	
		boxSizer2->Add(m_ShowHide_ImageButton, 0, wxALIGN_RIGHT, 0);
		
		boxSizer->Add(boxSizer2, 0, wxALL | wxEXPAND, 0); // SHOW/HIDE BUTTON
		boxSizer->Add(m_Date_text, 0, wxALL | wxEXPAND, 0); // DATE INFO
		
		mainSizer->Add(boxSizer, 0, wxALL | wxEXPAND, 5);

		// Creating buttons
		wxBoxSizer *buttonSizer = new wxBoxSizer(wxHORIZONTAL);
		buttonSizer->SetMinSize(wxSize(300, 20));

		wxButton *delButton = new wxButton(m_Dialog, ID_DEL_DIALOG, "Delete");
		delButton->SetValidator(albaGUIValidator(this, ID_DEL_DIALOG, delButton));
		delButton->SetToolTip("Delete current Comment");
		buttonSizer->Add(delButton, 0, wxALIGN_LEFT, 5); // DELETE BUTTON

		wxBoxSizer *separatorSizer = new wxBoxSizer(wxHORIZONTAL);
		separatorSizer->SetMinSize(180, 20);
		buttonSizer->Add(separatorSizer, 0, wxEXPAND, 5);

		wxButton *okButton = new wxButton(m_Dialog, ID_CLOSE_DIALOG, "OK");
		okButton->SetValidator(albaGUIValidator(this, ID_CLOSE_DIALOG, okButton));
		okButton->SetToolTip("Confirm changes");
		buttonSizer->Add(okButton, 0, wxALIGN_RIGHT, 5); // OK BUTTON

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
	wxString date = "";

	if (m_CrationDate != "")
		date = "Created " + m_CrationDate;

	if (m_EditDate != "")
		date = "Last Modify " + m_EditDate;

	m_Date_text->SetLabel(date);

	m_Name_textCtrl->SetValue(m_Name);
	m_Comment_textCtrl->SetValue(m_Comment);

	if (m_Author == "") m_Author = m_Current_Author;
	
	m_Author_textCtrl->SetValue(m_Author);

	if (m_Selected > -1 && m_Selected < m_CommentInteractor->GetMeasureCount())
	{
		if (m_CommentInteractor->IsVisible(m_Selected))
		{
			m_ShowHide_ImageButton->SetBitmapLabel(albaPictureFactory::GetPictureFactory()->GetBmp("COMMENT_SHOW_ICON"));
			m_ShowHide_ImageButton->SetToolTip("Hide Comment");
		}
		else
		{
			m_ShowHide_ImageButton->SetBitmapLabel(albaPictureFactory::GetPictureFactory()->GetBmp("COMMENT_HIDE_ICON"));
			m_ShowHide_ImageButton->SetToolTip("Show Comment");
		}
		m_ShowHide_ImageButton->Update();

	}

}
//----------------------------------------------------------------------------
void appOpAddComment::ShowDialog()
{
	if (m_IsDialogOpened)
		HideDialog();

	if (m_CommentInteractor)
		m_CommentInteractor->Disable();

	CreateDialog();

	m_IsDialogOpened = true;
}
//----------------------------------------------------------------------------
void appOpAddComment::HideDialog()
{
	if (m_Dialog)
		m_Dialog->Hide();

	if (m_CommentInteractor)
		m_CommentInteractor->Enable();

	m_IsDialogOpened = false;
}

/// LOAD/SAVE
//----------------------------------------------------------------------------
void appOpAddComment::Load()
{
	wxString tag = "info";
	albaVME *input = m_Input;// ->GetRoot();

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
			albaString creationDate = wxInfo.SubString(0, pos - 1);

			wxInfo = wxInfo.SubString(pos + 1, wxInfo.size());
			pos = wxInfo.Find(divider);
			albaString editDate = wxInfo.SubString(0, pos - 1);

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

 			// Add Comment
			m_CommentInteractor->AddComment(point, name, comment, author, creationDate, editDate);

			if (m_Current_Author == "") 
				m_Current_Author = author;
		}
	}

	UpdateGui();

	m_CommentInteractor->Render();
}
//----------------------------------------------------------------------------
void appOpAddComment::Save()
{
	wxString tag = "info";
	albaVME *input = m_Input;// ->GetRoot();

	int nComments = m_CommentInteractor->GetMeasureCount();

	albaTagItem measureTag;
	measureTag.SetName("Comment_" + tag);
	measureTag.SetNumberOfComponents(nComments);

	//////////////////////////////////////////////////////////////////////////
	for (int i = 0; i < nComments; i++)
	{
		wxString name = m_CommentInteractor->GetName(i);
		wxString comment = m_CommentInteractor->GetComment(i);
		wxString author = m_CommentInteractor->GetAuthor(i);
		wxString dateTime_Creation = m_CommentInteractor->GetCreationDateTime(i);
		wxString dateTime_Modify = m_CommentInteractor->GetDateTime(i);

		double position[3];

		m_CommentInteractor->GetMeasurePoint(i, position);

		char tmp[1024];
		sprintf(tmp, "%s#%s#%s#%s#%.3f#%.3f#%s", name, author, dateTime_Creation, dateTime_Modify, position[0], position[1], comment);

		measureTag.SetValue(tmp, i);
	}

	if (input->GetTagArray()->IsTagPresent("Comment_" + tag))
		input->GetTagArray()->DeleteTag("Comment_" + tag);

	input->GetTagArray()->SetTag(measureTag);
}