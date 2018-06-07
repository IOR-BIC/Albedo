/*=========================================================================
Program:   Albedo
Module:    appOpEmpty.cpp
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

#include "appOpEmpty.h"

#include "appDecl.h"

#include "mafGUI.h"
#include "mafVME.h"
#include "appGUIHintBox.h"
#include "appGUI.h"

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

enum TEST_ID
{
	ID_SCORE = MINID,
	ID_SCORE2,
	ID_SCORE3,
	ID_SCORE4,
	ID_TIME,
};

//----------------------------------------------------------------------------
void appOpEmpty::OnEvent(mafEventBase *maf_event)
{
	if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
	{
		time_t rawtime;
		time(&rawtime);
		struct tm * timeinfo = localtime(&rawtime);
		m_time[0] = timeinfo->tm_hour;
		m_time[1] = timeinfo->tm_min;
		m_time[2] = timeinfo->tm_sec;

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

			case ID_SCORE:
			{
				int v = var1[0];
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
void appOpEmpty::CreateGui()
{
	// Interface:
	m_Gui = new appGUI(this);

	m_Gui->HintBox(NULL, "This operation in empty.", "Hint");

//	((appGUI*)m_Gui)->HyperLink(NULL, "Link", "https://github.com/IOR-BIC");

// 	((appGUI*)m_Gui)->Separator(0, wxSize(1, 100));
//	((appGUI*)m_Gui)->Separator(2, wxSize(250, 1));
	
	TestGui();
	// ToDO: add your custom widgets...

	//m_Gui->Label("Press a button");

	m_Gui->OkCancel();
	m_Gui->Divider();

	ShowGui();
}



//----------------------------------------------------------------------------
void appOpEmpty::TestGui()
{
	appGUI *myGui = ((appGUI*)m_Gui);

	// 	std::vector<const char*> scoresVect;
	// 	scoresVect.push_back(" 0 ");	scoresVect.push_back(" 1 ");	scoresVect.push_back(" 2 ");	scoresVect.push_back(" 3 ");

	//	int var[4] = { 0,0,0,0 };

	// 	for (int i = 0; i < 4; i++)
	// 	{
	// 		var1[i] = 0;
	// 		var2[i] = 0;
	// 		var3[i] = 0;
	// 		var4[i] = 0;
	// 
	// 	}


	// 	std::vector<int> idList;
	// 	idList.push_back(ID_SCORE);	idList.push_back(ID_SCORE);	idList.push_back(ID_SCORE);	idList.push_back(ID_SCORE);

		//////////////////////////////////////////////////////////////////////////

	// 	myGui->Label(_("Anterior Scores"), true);
	// 	myGui->Divider(2);
	// 	myGui->MultipleLabel(4, scoresVect);
	// 	myGui->MultipleRadioButtons2(4, idList, var1, " L1", &wxColour(230, 230, 230));
	// 	myGui->MultipleRadioButtons2(4, idList, var2, " L2");
	// 	myGui->MultipleRadioButtons2(4, idList, var3, " L3", &wxColour(230, 230, 230));
	// 	myGui->MultipleRadioButtons2(4, idList, var4, " L4");
	// 
	// 	myGui->Label("");
	// 
	// 	myGui->Label(_("Posterior Scores"), true);
	// 	myGui->Divider(2);
	// 	myGui->MultipleLabel(4, scoresVect);
	// 	myGui->MultipleRadioButtons2(4, idList, var, " L1", &wxColour(230, 230, 230));
	// 	myGui->MultipleRadioButtons2(4, idList, var, " L2");
	// 	myGui->MultipleRadioButtons2(4, idList, var, " L3", &wxColour(230, 230, 230));
	// 	myGui->MultipleRadioButtons2(4, idList, var, " L4");
	// 
	// 
	// 	myGui->MultipleRadioButtons(4, 4, idList, scoresVect, var1, "Lab");

	wxString choises[4] = { " "," "," "," " };

	std::vector<const char*> scoresVect2;
	scoresVect2.push_back(" "); scoresVect2.push_back(" "); scoresVect2.push_back("  0");	scoresVect2.push_back("  1");	scoresVect2.push_back("  2");	scoresVect2.push_back("  3");

	myGui->Label("Anterior Scores", true);
	myGui->MultipleLabel(6, scoresVect2);
	myGui->Radio2(ID_SCORE, "L1", var + 0, 4, choises, 5);
	myGui->Radio2(ID_SCORE, "L2", var + 1, 4, choises, 5);
	myGui->Radio2(ID_SCORE, "L3", var + 2, 4, choises, 5);
	myGui->Radio2(ID_SCORE, "L4", var + 3, 4, choises, 5);

	myGui->Label("");
	myGui->Divider(2);

	myGui->Label("Posterior Scores", true);
	myGui->MultipleLabel(6, scoresVect2);
	myGui->Radio2(ID_SCORE, "L1", var + 4, 4, choises, 5);
	myGui->Radio2(ID_SCORE, "L2", var + 5, 4, choises, 5);
	myGui->Radio2(ID_SCORE, "L3", var + 6, 4, choises, 5);
	myGui->Radio2(ID_SCORE, "L4", var + 7, 4, choises, 5);

	myGui->Label("");
	myGui->Divider(2);

	myGui->Calendar();

	time_t rawtime;
	time(&rawtime);
	struct tm * timeinfo = localtime(&rawtime);

	m_time[0] = timeinfo->tm_hour;
	m_time[1] = timeinfo->tm_min;
	m_time[2] = timeinfo->tm_sec;

	//myGui->Clock(m_time + 0, m_time + 1, m_time + 2);

	myGui->Clock(ID_TIME, "", m_time);

	mTime = IntToString(m_time[0]);
	mTime += IntToString(m_time[1]);
	mTime += IntToString(m_time[2]);

	myGui->Clock(ID_TIME, &mTime);
	myGui->Enable(ID_TIME, false);

}





