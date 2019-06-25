/*=========================================================================
Program:   Albedo
Module:    appOpCursorTest.cpp
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

#include "appOpCursorTest.h"

#include "appDecl.h"

#include "mafGUI.h"
#include "mafVME.h"
#include "appGUIHintBox.h"
#include "appGUI.h"
#include "mafLogicWithManagers.h"
#include "mafDecl.h"
#include "mafViewManager.h"
#include "mafServiceClient.h"
#include "mafView.h"
#include "wx\motif\window.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(appOpCursorTest);

//----------------------------------------------------------------------------
appOpCursorTest::appOpCursorTest(wxString label, bool enable) :mafOp(label)
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
bool appOpCursorTest::Accept(mafVME *node)
{
	//return node->IsA("...");
	return m_Enabled;
}

//----------------------------------------------------------------------------
mafOp* appOpCursorTest::Copy()
{
	appOpCursorTest *cp = new appOpCursorTest(m_Label, m_Enabled);
	return cp;
}
//----------------------------------------------------------------------------
void appOpCursorTest::OpRun()
{
	// Open View if necessary
	mafEvent e(this, VIEW_SELECTED);
	mafEventMacro(e);
	if (!e.GetBool())
	{		
		mafEventMacro(mafEvent(this, ID_SHOW_IMAGE_VIEW));
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

	mafEventMacro(mafEvent(this, result));
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
void appOpCursorTest::OnEvent(mafEventBase *maf_event)
{
	if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
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
				wxString imagePath = mafGetAppDataDirectory().c_str();

				wxImage *cursorImage;
				cursorImage = new wxImage();

				cursorImage->LoadFile(imagePath + "\\Pencil.bmp", wxBITMAP_TYPE_ANY);
				cursorImage->SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_X, 6);
				cursorImage->SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_Y, 6);
				
				wxCursor cursor = wxCursor(cursorImage);
				m_View->GetWindow()->SetCursor(cursor);
			}break;

			case ID_MODE3:
			{
#include "C:/Users/vanella/AppData/Roaming/Albedo/Pencil.xpm"

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
#include "C:/Users/vanella/AppData/Roaming/Albedo/Pencil2.xpm"
				
				wxImage image = wxImage(Pencil2_xpm);

				int h = image.GetHeight();
				int w = image.GetWidth();

				image.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_X, 0);
				image.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_Y, 63);

				wxCursor cursor = wxCursor(image);
				m_View->GetWindow()->SetCursor(cursor);
			}break;

			default:
				Superclass::OnEvent(maf_event);
				break;
			}
		}
// 		else
// 		{
// 			Superclass::OnEvent(maf_event);
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
	m_Gui->Button(ID_MODE2, "Bitmap");
	m_Gui->Button(ID_MODE3, "xpm");
	m_Gui->Button(ID_MODE4, "xpm (2)");

	m_Gui->Divider(1);
	m_Gui->OkCancel();
	m_Gui->Divider();

	ShowGui();

	mafEvent e(this, VIEW_SELECTED);
	mafEventMacro(e);
	m_View = e.GetView();
}