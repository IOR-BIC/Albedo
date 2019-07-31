/*=========================================================================
Program:   Albedo
Module:    appOpCursorTest.cpp
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

#include "appOpCursorTest.h"

#include "appDecl.h"

#include "albaGUI.h"
#include "albaVME.h"
#include "appGUIHintBox.h"
#include "appGUI.h"
#include "albaLogicWithManagers.h"
#include "albaDecl.h"
#include "albaViewManager.h"
#include "albaServiceClient.h"
#include "albaView.h"
#include "wx\motif\window.h"

//----------------------------------------------------------------------------
albaCxxTypeMacro(appOpCursorTest);

//----------------------------------------------------------------------------
appOpCursorTest::appOpCursorTest(wxString label, bool enable) :albaOp(label)
{
	m_OpType = OPTYPE_OP;
	m_Canundo = true;

	m_View = NULL;

	m_Enabled = enable;
}

//----------------------------------------------------------------------------
appOpCursorTest::~appOpCursorTest()
{
}

//----------------------------------------------------------------------------
bool appOpCursorTest::Accept(albaVME *node)
{
	//return node->IsA("...");
	return m_Enabled;
}

//----------------------------------------------------------------------------
albaOp* appOpCursorTest::Copy()
{
	appOpCursorTest *cp = new appOpCursorTest(m_Label, m_Enabled);
	return cp;
}
//----------------------------------------------------------------------------
void appOpCursorTest::OpRun()
{
	// Open View if necessary
	albaEvent e(this, VIEW_SELECTED);
	albaEventMacro(e);

	if (!e.GetBool())
	{		
		albaEventMacro(albaEvent(this, ID_SHOW_IMAGE_VIEW));
	}

	if (!m_TestMode)
	{
		CreateGui();
	}
	
	//OpStop(OP_RUN_OK);
}
//----------------------------------------------------------------------------
void appOpCursorTest::OpStop(int result)
{
	if (!m_TestMode)
	{
		HideGui();
	}

	albaEventMacro(albaEvent(this, result));
}
//----------------------------------------------------------------------------
void appOpCursorTest::OpDo()
{
}

enum TEST_ID
{
	ID_MODE1 = MINID,
	ID_MODE2,
	ID_MODE3,
	ID_MODE4,
	ID_TIME,
};

//----------------------------------------------------------------------------
void appOpCursorTest::OnEvent(albaEventBase *alba_event)
{
	if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
		m_Gui->Update();
		//if (e->GetSender() == m_Gui)
		{
			switch (e->GetId())
			{
			case wxOK:
				OpStop(OP_RUN_OK);
				break;

			case wxCANCEL:
				OpStop(OP_RUN_CANCEL);
				break;

			case ID_MODE1:
			{
				wxCursor cursor = wxCursor(wxCURSOR_ARROW);
				m_View->GetWindow()->SetCursor(cursor);
			}break;

			case ID_MODE2:
			{
				int ret_code;
				wxString imagePath;
				wxString name;
				wxString wildcard = "File bitmap (*.bmp)|*.bmp";

				wxFileDialog dialog(NULL, "Open File", imagePath, name, wildcard, wxOPEN | wxFILE_MUST_EXIST | wxHIDE_READONLY);
				dialog.SetReturnCode(wxID_OK);
				ret_code = dialog.ShowModal();
				if (ret_code == wxID_OK)
				{
					imagePath = dialog.GetPath();

					wxImage *cursorImage;
					cursorImage = new wxImage();

					cursorImage->LoadFile(imagePath, wxBITMAP_TYPE_ANY);
					cursorImage->SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_X, 6);
					cursorImage->SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_Y, 6);

					wxCursor cursor = wxCursor(cursorImage);
					m_View->GetWindow()->SetCursor(cursor);
				}

			}break;

			case ID_MODE3:
			{
			#include "pic/Cursor/Pencil.xpm"
				// 	wxBitmap *b_curs = new wxBitmap(imagePath + "\\Pencil_xpm");
				// 	int h = b_curs->GetHeight(), w = b_curs->GetWidth();
				// 
				// 	wxImage curImage = wxImage(b_curs->ConvertToImage());

				// create cursor with hotspots in the middle of the bitmap
				 	wxCursor cursor = wxCursor(Pencil_xpm);
					m_View->GetWindow()->SetCursor(cursor);
			}break;

			case ID_MODE4:
			{
				#include "pic/Cursor/Pencil_Erase.xpm"
				wxImage image = wxImage(Pencil_Erase_xpm);

				int h = image.GetHeight();
				int w = image.GetWidth();

				image.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_X, 0);
				image.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_Y, 63);

				wxCursor cursor = wxCursor(image);
				m_View->GetWindow()->SetCursor(cursor);
			}break;

			default:
				Superclass::OnEvent(alba_event);
				break;
			}
		}
// 		else
// 		{
// 			Superclass::OnEvent(alba_event);
// 		}
	}
}

//----------------------------------------------------------------------------
void appOpCursorTest::CreateGui()
{
	// Interface:
	m_Gui = new appGUI(this);

	m_Gui->HintBox(NULL, "Cursor Mouse Mode", "Hint");

	
	m_Gui->Button(ID_MODE1, "Default");
	m_Gui->Button(ID_MODE2, "Load Bitmap");
	m_Gui->Button(ID_MODE3, "xpm");
	m_Gui->Button(ID_MODE4, "xpm (2)");

	m_Gui->Divider(1);
	m_Gui->OkCancel();
	m_Gui->Divider();

	ShowGui();

	albaEvent e(this, VIEW_SELECTED);
	albaEventMacro(e);
	m_View = e.GetView();
}