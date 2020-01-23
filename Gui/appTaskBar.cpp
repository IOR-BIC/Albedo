/*=========================================================================
Program:   Albedo
Module:    appTaskBar.cpp
Language:  C++
Date:      $Date: 2019-01-01 12:00:00 $
Version:   $Revision: 1.0.0.0 $
Authors:   Nicola Vanella
==========================================================================
Copyright (c) BIC-IOR 2019 (http://www.ltmsoftware.org/alba.htm)

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the above copyright notice for more information.
=========================================================================*/

#include "appDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the APP must include "appDefines.h" as first.
// This force to include Window, wxWidgets and VTK exactly in this order.
//----------------------------------------------------------------------------

#include "appTaskBar.h"

#include "albaGUI.h"
#include "albaGUIDialogFindVme.h"
#include "albaGUIDockManager.h"
#include "albaGUIHolder.h"
#include "albaGUIMDIFrame.h"
#include "albaGUIPanel.h"
#include "albaGUIPanelStack.h"
#include "albaGUISplittedPanel.h"
#include "albaGUITree.h"
#include "albaGUIVMEChooser.h"
#include "albaPipe.h"
#include "albaTransform.h"
#include "albaVME.h"
#include "albaVMEIterator.h"
#include "albaVMEOutput.h"
#include "albaVMERoot.h"
#include "albaView.h"
#include "albaViewVTK.h"
#include "appGUI.h"

//----------------------------------------------------------------------------
appTaskBar::appTaskBar(albaGUIMDIFrame* parent, int id, albaObserver *Listener)
{
	m_SelectedVme = NULL;
	m_SelectedView = NULL;

	m_AppendingGUI = NULL;

	m_Listener = Listener;

	//Notebook
	m_Notebook = new wxNotebook(parent, id);
	m_Notebook->SetFont(wxFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT)));

	//Task panel
	m_TaskPanel = new albaGUIHolder(m_Notebook, -1, false, true);
	m_Notebook->AddPage(m_TaskPanel, _("Task"));

	parent->AddDockPane(m_Notebook, wxPaneInfo()
		.Name("taskbar")
		.Caption(wxT("Task Panel"))
		.Left()
		.Layer(2)
		.MinSize(240, 450)
		.TopDockable(false)
		.BottomDockable(false)
	);



	m_TaskGroup.clear();

	std::vector<task> t1;
	t1.push_back({ 1, "Task 1.1 - Select VME", false, 0 });
	t1.push_back({ 2, "Task 1.2", true, 0 });
	t1.push_back({ 3, "Task 1.3", false, 0 });

	m_TaskGroup.push_back({ 1, "Group 1", t1 });

	std::vector<task> t2;
	t2.push_back({ 1, "Task 2.1", false, 0 });
	t2.push_back({ 2, "Task 2.2", true, 0 });

	m_TaskGroup.push_back({ 2, "Group 2", t2 });

	std::vector<task> t3;
	t3.push_back({ 1, "Task 3.1", false, 0 });
	t3.push_back({ 2, "Task 3.2", true, 0 });
	t3.push_back({ 3, "Task 3.2", true, 0 });

	m_TaskGroup.push_back({ 3, "Group 3", t3 });

	std::vector<task> t4;
	t4.push_back({ 1, "Save", true, 1 });

	m_TaskGroup.push_back({ 4, "Group 4", t4 });
	}
//----------------------------------------------------------------------------
appTaskBar::~appTaskBar()
{
}

//----------------------------------------------------------------------------
void appTaskBar::OpShowGui(bool push_gui, albaGUIPanel *panel)
{
	m_Notebook->Show(true);
// 	if(push_gui)
// 	{
// 		m_Notebook->SetSelection(2);
// 		m_OpPanel->Push(panel);
// 	}
	UpdateTaskPanel();
}
//----------------------------------------------------------------------------
void appTaskBar::OpHideGui(bool view_closed)
{
// 	if(view_closed)
// 	{
// 		m_Notebook->SetSelection(0);
// 		m_ViewPropertyPanel->RemoveCurrentGui();
// 		m_ViewPropertyPanel->SetTitle("");
// 	}
// 	else
// 	{
// 		m_OpPanel->Pop();
// 		m_Notebook->SetSelection(0);
// 	}

	UpdateTaskPanel();
}
//----------------------------------------------------------------------------
void appTaskBar::ViewSelect(albaView *view)
{
	UpdateTaskPanel();
}
//----------------------------------------------------------------------------
void appTaskBar::ViewDeleted(albaView *view)
{
  //ViewSelect(NULL);
}
//----------------------------------------------------------------------------
void appTaskBar::EnableSelect(bool enable)
{
}
//----------------------------------------------------------------------------
void appTaskBar::VmeAdd(albaVME *vme)
{
	UpdateTaskPanel();
}
//----------------------------------------------------------------------------
void appTaskBar::VmeRemove(albaVME *vme)
{
	UpdateTaskPanel();
}
//----------------------------------------------------------------------------
void appTaskBar::VmeModified(albaVME *vme)
{
	UpdateTaskPanel();
}
//----------------------------------------------------------------------------
void appTaskBar::VmeShow(albaVME *vme, bool visibility)
{
	UpdateTaskPanel();
}
//----------------------------------------------------------------------------
void appTaskBar::VmeSelected(albaVME *vme)
{
   m_SelectedVme = vme;
   UpdateTaskPanel();
}

//----------------------------------------------------------------------------
std::vector<albaVME*> appTaskBar::VmeChoose(void *vme_accept_function, long style, albaString title, bool multiSelect, albaVME *vme)
{
	albaErrorMessage("Error on choose VME");
	std::vector<albaVME*> emptyList;
	return emptyList;
}

//----------------------------------------------------------------------------
void appTaskBar::FindVME()
{
	albaErrorMessage("Error on find VME");
}

//----------------------------------------------------------------------------
void appTaskBar::Show()
{
	m_Notebook->Show(!m_Notebook->IsShown());
}

//----------------------------------------------------------------------------
void appTaskBar::UpdateTaskPanel()
{
	if (m_AppendingGUI && m_CurrentTaskGui)
		m_AppendingGUI->Remove(m_CurrentTaskGui);

	m_AppendingGUI = NULL;
	m_AppendingGUI = new albaGUI(NULL); //TODO Update Gui

	m_CurrentTaskGui = GetTaskGui();

	if (m_CurrentTaskGui)
	{
		m_AppendingGUI->AddGui(m_CurrentTaskGui);
	}
	else
	{
		if (m_SelectedVme)
		{
			albaString text = "Vme Selected: ";
			text += m_SelectedVme->GetName();
			m_AppendingGUI->Label(text);
		}
		else
			m_AppendingGUI->Label("Empty");
	}

	m_AppendingGUI->Label("");
	m_AppendingGUI->FitGui();
	m_AppendingGUI->Update();

	m_TaskPanel->Put(m_AppendingGUI);

	m_AppendingGUI->SetListener(m_Listener);
}

//----------------------------------------------------------------------------
albaGUI* appTaskBar::GetTaskGui()
{
	UpdateTaskGui();

	appGUI	*task_gui = NULL;

	task_gui = new appGUI(NULL);

	int currentGroup = -1;

	for (int i = 0; i < m_TaskGroup.size(); i++)
	{
		if (m_TaskGroup[i].id != currentGroup)
		{
			task_gui->Label("");
			task_gui->Divider(1);
			task_gui->Label(m_TaskGroup[i].description, true);
			currentGroup = m_TaskGroup[i].id;
		}

		for (int j = 0; j < m_TaskGroup[i].tasks.size(); j++)
		{
			switch (m_TaskGroup[i].tasks[j].type)
			{
			case 0:
				task_gui->TaskButton(NULL, m_TaskGroup[i].tasks[j].description.GetCStr(), m_TaskGroup[i].tasks[j].status);
				break;

			case 1:
				task_gui->Button(NULL, m_TaskGroup[i].tasks[j].description.GetCStr());
				break;

			default:
				break;
			}
		}
	}

	return task_gui;
}

//----------------------------------------------------------------------------
void appTaskBar::UpdateTaskGui()
{
	if (m_CurrentTaskGui!=NULL)
	{

	}

	m_TaskGroup[0].tasks[0].description = "VME selected";
	m_TaskGroup[0].tasks[0].status = m_SelectedVme != NULL;
	if (m_SelectedVme != NULL)
	{
		m_TaskGroup[0].tasks[0].description = "Task 1.1 - VME selected";
	}
	else
	{
		m_TaskGroup[0].tasks[0].description = "Task 1.1 - Select VME";
	}
	
}
