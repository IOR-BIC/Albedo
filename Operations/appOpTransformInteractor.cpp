/*=========================================================================
Program:   Albedo
Module:    appOpTransformInteractor.cpp
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
#include "appInteractor2DTransform.h"
#include "appOpTransformInteractor.h"
#include "albaView.h"
#include "albaOp.h"
#include "albaSceneGraph.h"

//----------------------------------------------------------------------------
albaCxxTypeMacro(appOpTransformInteractor);

enum OP_ID
{
	ID_RESET = MINID,
	ID_MODE,
	ID_SCALE_MODE,
	ID_DEFORMATION_MODE,
};

//----------------------------------------------------------------------------
appOpTransformInteractor::appOpTransformInteractor(wxString label, bool enable) :albaOp(label)
{
	m_OpType = OPTYPE_OP;
	m_Canundo = true;
	m_Enabled = enable;

	m_mode = 0;
	m_scale_mode = 0;
	m_deformation_mode = 0;

	m_InteractorTransform = NULL;
}

//----------------------------------------------------------------------------
appOpTransformInteractor::~appOpTransformInteractor()
{
}

//----------------------------------------------------------------------------
bool appOpTransformInteractor::InternalAccept(albaVME *node)
{
	return node && node->IsA("albaVMEImage");
}

//----------------------------------------------------------------------------
albaOp* appOpTransformInteractor::Copy()
{
	appOpTransformInteractor *cp = new appOpTransformInteractor(m_Label, m_Enabled);
	return cp;
}
//----------------------------------------------------------------------------
void appOpTransformInteractor::OpRun()
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
void appOpTransformInteractor::OpStop(int result)
{
	if (!m_TestMode)
	{
		HideGui();
	}

	if (m_InteractorTransform)
	{
		// Remove Interactor
		albaEventMacro(albaEvent(this, PER_POP));
		albaDEL(m_InteractorTransform);
	}

	albaEventMacro(albaEvent(this, result));
}
//----------------------------------------------------------------------------
void appOpTransformInteractor::OpDo()
{
}

//----------------------------------------------------------------------------
void appOpTransformInteractor::OnEvent(albaEventBase *alba_event)
{
	if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
		switch (e->GetId())
		{
		case ID_RESET:
			m_InteractorTransform->ResetMeasure();
			break;

		case ID_MODE:
			m_InteractorTransform->SetMode(m_mode);
			break;

		case ID_SCALE_MODE:
			m_InteractorTransform->SetScaleMode(m_scale_mode);
			break;

		case ID_DEFORMATION_MODE:
			m_InteractorTransform->SetDeformationMode(m_deformation_mode);
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
void appOpTransformInteractor::CreateGui()
{
	// Interface:
	m_Gui = new appGUI(this);

	m_Gui->HintBox(NULL, "Edit Template Corners", "Hint");
		
	// ToDO: add your custom widgets...
	m_Gui->Button(ID_RESET, "Reset");
	
	wxString radioChoice[3] = { "Scale","Deformation", "Rotation" };
	m_Gui->Radio(ID_MODE, "Mode", &m_mode, 3, radioChoice);

	wxString radioChoice2[2] = { "Corner","Center" };
	m_Gui->Radio(ID_SCALE_MODE, "Scale Mode", &m_scale_mode, 2, radioChoice2);

	wxString radioChoice3[4] = { "Med 2pnts","Med 4pnts (Min)","Med 4pnts (Max)","Diff" };
	m_Gui->Radio(ID_DEFORMATION_MODE, "Deformation Mode", &m_deformation_mode, 4, radioChoice3);
	
	//////////////////////////////////////////////////////////////////////////
	m_Gui->Label("");
	m_Gui->Divider(1);
	m_Gui->OkCancel();
	m_Gui->Label("");
}

//----------------------------------------------------------------------------
void appOpTransformInteractor::CreateInteractor(albaView *view)
{
	// Show input if necessary
	int nodeStatus = view->GetSceneGraph()->GetNodeStatus(m_Input);
	if (nodeStatus == NODE_MUTEX_OFF)
		GetLogicManager()->VmeShow(m_Input, true);

	// Create Interactor
	m_InteractorTransform = appInteractor2DTransform::New();
	albaEventMacro(albaEvent(this, PER_PUSH, (albaObject *)m_InteractorTransform));
	m_InteractorTransform->SetListener(this);

	m_InteractorTransform->SetRendererByView(view);
	m_InteractorTransform->EnableSelect(true);

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

	m_InteractorTransform->AddMeasure(point1, point2, point3, point4);
}