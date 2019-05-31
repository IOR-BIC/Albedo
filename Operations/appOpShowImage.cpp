/*=========================================================================
Program:   Albedo
Module:    appOpShowImage.cpp
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

#include "appDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the APP must include "appDefines.h" as first.
// This force to include Window, wxWidgets and VTK exactly in this order.
//----------------------------------------------------------------------------

#include "appOpShowImage.h"
#include "appGUILabel.h"

#include "mafDecl.h"
#include "mafEvent.h"
#include "mafGUI.h"
#include "mafGUICheckListBox.h"
#include "mafGUIDictionaryWidget.h"
#include "mafGUIImageViewer.h"
#include "mafGUIValidator.h"
#include "mafPics.h"
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
mafCxxTypeMacro(appOpShowImage);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
appOpShowImage::appOpShowImage(const wxString &label) :
mafOp(label)
{
  m_OpType	= OPTYPE_OP;
  m_Canundo = true;

	m_listBox = NULL;
	m_BitmapButton = NULL;

	mafPictureFactory::GetPictureFactory()->GetVmeNames(m_VectNames);
	//mafPictureFactory::GetPictureFactory()->GetPicIds(m_VectIds);
}
//----------------------------------------------------------------------------
appOpShowImage::~appOpShowImage()
{	
// 	cppDEL(m_Gui2);
// 	cppDEL(m_listBox);
}

//----------------------------------------------------------------------------
mafOp* appOpShowImage::Copy()   
{
	return new appOpShowImage(m_Label);
}
//----------------------------------------------------------------------------
bool appOpShowImage::Accept(mafVME*node)
{
	return true;
}

//----------------------------------------------------------------------------
void appOpShowImage::OpRun()
{
  m_Gui = new mafGUI(this);	

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

	wxBitmap bitmap = mafPictureFactory::GetPictureFactory()->GetVmePic(m_VectNames[0]);

	if(bitmap == wxNullBitmap) 
		bitmap = mafPictureFactory::GetPictureFactory()->GetBmp(m_VectNames[0]);


	//wxBitmap bitmap = mafPictureFactory::GetPictureFactory()->GetBmp(m_VectNames[0]);
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
void appOpShowImage::OnEvent( mafEventBase *maf_event )
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
void appOpShowImage::CreateGui(int index)
{
	wxBitmap bitmap = mafPictureFactory::GetPictureFactory()->GetBmp(m_VectNames[index]);
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

