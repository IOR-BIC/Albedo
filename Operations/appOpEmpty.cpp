/*=========================================================================
Program:   Albedo
Module:    appOpEmpty.cpp
Language:  C++
Date:      $Date: 2018-01-01 12:00:00 $
Version:   $Revision: 1.0.0.0 $
Authors:   Nicola Vanella
==========================================================================
Copyright (c) LTM-IOR 2018 (http://www.ltmsoftware.org/alba.htm)

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the above copyright notice for more information.
=========================================================================*/

#include "appDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the APP must include "appDefines.h" as first.
// This force to include Window, wxWidgets and VTK exactly in this order.
//----------------------------------------------------------------------------

#include "appOpEmpty.h"

#include "appDecl.h"

#include "mafGUI.h"
#include "mafVME.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(appOpEmpty);

//----------------------------------------------------------------------------
appOpEmpty::appOpEmpty(wxString label, bool enable) :mafOp(label)
{
	m_OpType = OPTYPE_OP;
	m_Canundo = true;

	m_Enabled = enable;
}

//----------------------------------------------------------------------------
appOpEmpty::~appOpEmpty()
{
}

//----------------------------------------------------------------------------
bool appOpEmpty::Accept(mafVME *node)
{
	//return node->IsA("...");
	return m_Enabled;
}

//----------------------------------------------------------------------------
mafOp* appOpEmpty::Copy()
{
	appOpEmpty *cp = new appOpEmpty(m_Label, m_Enabled);
	return cp;
}
//----------------------------------------------------------------------------
void appOpEmpty::OpRun()
{
	if (!m_TestMode)
	{
		CreateGui();
	}
	
	//OpStop(OP_RUN_OK);
}
//----------------------------------------------------------------------------
void appOpEmpty::OpStop(int result)
{
	if (!m_TestMode)
	{
		HideGui();
	}

	mafEventMacro(mafEvent(this, result));
}
//----------------------------------------------------------------------------
void appOpEmpty::OpDo()
{
}

//----------------------------------------------------------------------------
void appOpEmpty::OnEvent(mafEventBase *maf_event)
{
	if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
	{
		if (e->GetSender() == m_Gui)
		{
			switch (e->GetId())
			{
			case wxOK:
				OpStop(OP_RUN_OK);
				break;

			case wxCANCEL:
				OpStop(OP_RUN_CANCEL);
				break;
			}
		}
		else
		{
			Superclass::OnEvent(maf_event);
		}
	}
}

//----------------------------------------------------------------------------
void appOpEmpty::CreateGui()
{
	// Interface:
	m_Gui = new mafGUI(this);

	// ToDO: add your custom widgets...

	m_Gui->Label("Press a button");

	m_Gui->OkCancel();
	m_Gui->Divider();

	ShowGui();
}





