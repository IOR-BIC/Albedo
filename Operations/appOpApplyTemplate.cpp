/*=========================================================================
Program:   Albedo
Module:    appOpApplyTemplate.cpp
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

#include "albaDefines.h"
#include "albaGUI.h"
#include "albaVME.h"

#include "appDecl.h"
#include "appGUI.h"
#include "appInteractor2DTemplate.h"
#include "appOpApplyTemplate.h"
#include "albaView.h"
#include "albaOp.h"
#include "albaSceneGraph.h"

//----------------------------------------------------------------------------
albaCxxTypeMacro(appOpApplyTemplate);

enum OP_ID
{
	ID_RESET = MINID,
	ID_MODE,
};

//----------------------------------------------------------------------------
appOpApplyTemplate::appOpApplyTemplate(wxString label, bool enable) :albaOp(label)
{
	m_OpType = OPTYPE_OP;
	m_Canundo = true;
	m_Enabled = enable;

	m_mode = 0;
	m_InteractorTemplate = NULL;
}

//----------------------------------------------------------------------------
appOpApplyTemplate::~appOpApplyTemplate()
{
}

//----------------------------------------------------------------------------
bool appOpApplyTemplate::InternalAccept(albaVME *node)
{
	return node && node->IsA("albaVMEImage");
}

//----------------------------------------------------------------------------
albaOp* appOpApplyTemplate::Copy()
{
	appOpApplyTemplate *cp = new appOpApplyTemplate(m_Label, m_Enabled);
	return cp;
}
//----------------------------------------------------------------------------
void appOpApplyTemplate::OpRun()
{
	if (!m_TestMode)
	{
		CreateGui();

		albaEvent e(this, VIEW_SELECTED);
		albaEventMacro(e);

		if (!e.GetBool())
		{
			// Open View
			albaEventMacro(albaEvent(this, ID_SHOW_IMAGE_VIEW));

			albaEvent e2(this, VIEW_SELECTED);
			albaEventMacro(e2);

			CreateInteractor(e2.GetView());
		}
		else
		{
			CreateInteractor(e.GetView());
		}

		ShowGui();
	}
}
//----------------------------------------------------------------------------
void appOpApplyTemplate::OpStop(int result)
{
	if (!m_TestMode)
	{
		HideGui();
	}

	if (m_InteractorTemplate)
	{
		// Remove Interactor
		albaEventMacro(albaEvent(this, PER_POP));
		albaDEL(m_InteractorTemplate);
	}

	albaEventMacro(albaEvent(this, result));
}
//----------------------------------------------------------------------------
void appOpApplyTemplate::OpDo()
{
}

//----------------------------------------------------------------------------
void appOpApplyTemplate::OnEvent(albaEventBase *alba_event)
{
	if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
		switch (e->GetId())
		{
		case ID_RESET:
			m_InteractorTemplate->ResetMeasure();
			break;

		case ID_MODE:
			m_InteractorTemplate->SetMode(m_mode);
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
}

//----------------------------------------------------------------------------
void appOpApplyTemplate::CreateGui()
{
	// Interface:
	m_Gui = new appGUI(this);

	m_Gui->HintBox(NULL, "Edit Template Corners", "Hint");
		
	// ToDO: add your custom widgets...
	m_Gui->Button(ID_RESET, "Reset");
	
	wxString radioChoice[4] = { "Med 2pnts","Med 4pnts (Min)","Med 4pnts (Max)","Diff" };
	m_Gui->Radio(ID_MODE, "Mode", &m_mode, 4, radioChoice);
	
	//////////////////////////////////////////////////////////////////////////
	m_Gui->Label("");
	m_Gui->Divider(1);
	m_Gui->OkCancel();
	m_Gui->Label("");
}

//----------------------------------------------------------------------------
void appOpApplyTemplate::CreateInteractor(albaView *view)
{
	// Show input if necessary
	int nodeStatus = view->GetSceneGraph()->GetNodeStatus(m_Input);
	if (nodeStatus == NODE_MUTEX_OFF)
		GetLogicManager()->VmeShow(m_Input, true);

	// Create Interactor
	m_InteractorTemplate = appInteractor2DTemplate::New();
	albaEventMacro(albaEvent(this, PER_PUSH, (albaObject *)m_InteractorTemplate));
	m_InteractorTemplate->SetListener(this);

	m_InteractorTemplate->SetRendererByView(view);
	m_InteractorTemplate->EnableSelect(true);

	// Get Image Size
	double bounds[6];
	m_Input->GetOutput()->GetBounds(bounds);
	int immW = bounds[1];
	int immH = bounds[3];

	int ts = 300; // Template Size

	// Add Template Points
	double point2[3];	double point1[3];	double point3[3];	double point4[3];

	point1[0] = (immW / 2) - (ts / 2);	point1[1] = (immH / 2) - (ts / 2); point1[2] = 0;
	point2[0] = (immW / 2) + (ts / 2);	point2[1] = (immH / 2) - (ts / 2); point2[2] = 0;
	point3[0] = (immW / 2) + (ts / 2);	point3[1] = (immH / 2) + (ts / 2); point3[2] = 0;
	point4[0] = (immW / 2) - (ts / 2);	point4[1] = (immH / 2) + (ts / 2); point4[2] = 0;

	m_InteractorTemplate->AddMeasure(point1, point2, point3, point4);
}