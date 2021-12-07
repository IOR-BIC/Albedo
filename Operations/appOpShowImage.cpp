/*=========================================================================
Program:   Albedo
Module:    appOpShowImage.cpp
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

#include "appOpShowImage.h"
#include "appGUILabel.h"

#include "albaDecl.h"
#include "albaEvent.h"
#include "albaGUI.h"
#include "albaGUICheckListBox.h"
#include "albaGUIDictionaryWidget.h"
#include "albaGUIImageViewer.h"
#include "albaGUIValidator.h"
#include "albaPics.h"
#include "wx\os2\bmpbuttn.h"

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
albaCxxTypeMacro(appOpShowImage);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
appOpShowImage::appOpShowImage(const wxString &label) :
albaOp(label)
{
  m_OpType	= OPTYPE_OP;
  m_Canundo = true;

	m_listBox = NULL;
	m_BitmapButton = NULL;

	albaPictureFactory::GetPictureFactory()->GetVmeNames(m_VectNames);
	//albaPictureFactory::GetPictureFactory()->GetPicIds(m_VectIds);
}
//----------------------------------------------------------------------------
appOpShowImage::~appOpShowImage()
{	
// 	cppDEL(m_Gui2);
// 	cppDEL(m_listBox);
}

//----------------------------------------------------------------------------
albaOp* appOpShowImage::Copy()   
{
	return new appOpShowImage(m_Label);
}
//----------------------------------------------------------------------------
bool appOpShowImage::InternalAccept(albaVME*node)
{
	return true;
}

//----------------------------------------------------------------------------
void appOpShowImage::OpRun()
{
  m_Gui = new albaGUI(this);	

	m_listBox = m_Gui->ListBox(ID_LIST_SELECTION, "List", 100);

	m_listBox->Clear();

	m_VectNames.push_back("NODE_BLUE");
	m_VectNames.push_back("NODE_RED");
	m_VectNames.push_back("TOOL_FILL");

	for (int i=0; i<m_VectNames.size(); i++)
	{
		m_listBox->Append(m_VectNames[i]);
	}

	for (int i = 0; i < m_VectIds.size(); i++)
	{
		m_listBox->Append(m_VectIds[i]);
	}



	m_Gui->Label("");
	m_Gui->Divider(1);

	wxBitmap bitmap = albaPictureFactory::GetPictureFactory()->GetVmePic(m_VectNames[0]);

	if(bitmap == wxNullBitmap) 
		bitmap = albaPictureFactory::GetPictureFactory()->GetBmp(m_VectNames[0]);


	//wxBitmap bitmap = albaPictureFactory::GetPictureFactory()->GetBmp(m_VectNames[0]);
	if (m_BitmapButton == NULL)
	{
		m_BitmapButton = new wxBitmapButton(m_Gui, -1, bitmap, wxPoint(0, 0), wxSize(19, 20));
		m_Gui->Add(m_BitmapButton);
	}

	m_Gui->Label("");
	m_Gui->OkCancel();
	m_Gui->Divider(1);

  ShowGui();
}

//----------------------------------------------------------------------------
void appOpShowImage::OnEvent( albaEventBase *alba_event )
{
	int b = 1;
	int a = 0;

  switch (alba_event->GetId())
  {
    case ID_GUI_WIDGET_FIRST:
    case ID_GUI_WIDGET_SECOND:
    case ID_GUI_WIDGET:
			albaLogMessage("Event: %d - Sender: %s", alba_event->GetId(), alba_event->GetSender());
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
void appOpShowImage::CreateGui(int index)
{
	wxBitmap bitmap = albaPictureFactory::GetPictureFactory()->GetBmp(m_VectNames[index]);
	if (m_BitmapButton == NULL)
	{
		m_BitmapButton = new wxBitmapButton(m_Gui, -1, bitmap, wxPoint(0, 0), wxSize(19, 20));
		m_Gui->Add(m_BitmapButton);
	}

	m_BitmapButton->SetBitmapLabel(bitmap);
	m_BitmapButton->Update();

	m_Gui->FitGui();
	m_Gui->FitInside();
	m_Gui->Update();
}

