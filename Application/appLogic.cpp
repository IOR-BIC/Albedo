/*=========================================================================
Program:   Albedo
Module:    appLogic.cpp
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

#include "appDecl.h"
#include "appUtils.h"
#include "appLogic.h"
#include "appSideBar.h"

#include "albaGUIAboutDialog.h"
#include "albaGUIApplicationSettings.h"
#include "albaLogicWithManagers.h"
#include "albaOpImporterDicom.h"
#include "albaOpManager.h"
#include "albaSnapshotManager.h"
#include "albaVME.h"
#include "albaVMEGroup.h"
#include "albaVMEIterator.h"
#include "albaVMEVolumeGray.h"
#include "albaView.h"
#include "albaViewManager.h"

#ifdef USE_WIZARD
#include "albaWizardManager.h"
#endif

#include "vtkObject.h"
#include "vtkPointData.h"
#include "vtkPoints.h"
#include "appTaskBar.h"

//----------------------------------------------------------------------------
appLogic::appLogic() : albaLogicWithManagers()
{
	m_OpeningMSF = false;

	// Init Operations to plug in Toolbar 
	m_OpImporterDicom = new albaOpImporterDicom("DICOM", true);

	m_Win->Maximize();

	// Set project file extension
	m_Extension = "alb";
}
//----------------------------------------------------------------------------
appLogic::~appLogic()
{
	albaDEL(m_OpImporterDicom);

	// must be deleted after m_VMEManager
	cppDEL(m_TaskBar);

	delete m_SideBar;
}

//----------------------------------------------------------------------------
void appLogic::Init(int argc, char **argv)
{
	albaLogicWithManagers::Init(argc, argv);
	m_Win->ShowDockPane("logbar", false);

	SetRevision("1.0");

	if (!wxDirExists(albaGetAppDataDirectory().c_str()))
		wxMkDir(albaGetAppDataDirectory().c_str());

	InitAboutDialog();
	
	// Create and Open View
	//ViewCreate(VIEW_START + 1);
}

//----------------------------------------------------------------------------
void appLogic::InitAboutDialog()
{
	// Init About Dialog
	m_AboutDialog->SetTitle(m_AppTitle);
	m_AboutDialog->SetRevision(m_Revision.GetCStr());
	m_AboutDialog->SetWebSite("https://github.com/IOR-BIC");
	//m_AboutDialog->SetLicenseURL("https://github.com/IOR-BIC");
	//m_AboutDialog->SetExtraMessage("");

	// About Image
	wxString imageName = "AlbedoAbout";
	wxString imagesPath = appUtils::GetConfigDirectory().c_str();
	m_AboutDialog->SetImagePath(imagesPath + "/" + imageName + ".bmp");
}

//----------------------------------------------------------------------------
void appLogic::OnEvent(albaEventBase *alba_event)
{
	if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
		switch (e->GetId())
		{
		case VME_PICKED:
		{
			albaVME* selectedVme = m_OpManager->GetSelectedVme();
			//
		}
		break;

		// View Events
		case ID_SHOW_VIEW_VTK_SURFACE:
		{
			ViewCreate(VIEW_START);
		}
		break;
		case ID_SHOW_IMAGE_VIEW:
		{
			ViewCreate(VIEW_START + 1);
		}
		break;
		case ID_SHOW_SLICE_VIEW:
		{
			ViewCreate(VIEW_START + 2);
		}
		break;

		// Operations Events 
		case OP_IMPORTER_DICOM:
		{
			albaString op = "albaOpImporterDicom";
			m_OpManager->OpRun(op);
		}
		break;

		case ID_ADD_COMMENT:
		{
			albaString op = "appOpAddComment";
			m_OpManager->OpRun(op);
		}
		break;

// 		case ID_IMPORTER_DICOM:
// 		case ID_ADD_VME:
// 		{
// 			m_OpManager->OpRun(e->GetId() - MENU_VIEW_USER_FIRST + OP_USER);
// 		}
// 		break;

		// WX Events
		case wxID_FILE1:
		case wxID_FILE2:
		case wxID_FILE3:
		case wxID_FILE4:
		case wxID_FILE5:
		case wxID_FILE6:
		case wxID_FILE7:
		case wxID_FILE8:
		case wxID_FILE9:
		{
			m_OpeningMSF = true;
			OnFileHistory(e->GetId());
			m_OpeningMSF = false;

			ShowVMEOnView();
		}
		break;
		case MENU_FILE_OPEN:
		{
			m_OpeningMSF = true;
			albaString *filename = e->GetString();
			if (filename)
			{
				OnFileOpen((*filename).GetCStr());
			}
			else
			{
				OnFileOpen();
			}

			UpdateFrameTitle();
			m_OpeningMSF = false;

			ShowVMEOnView();
		}
		break;
		case ID_GET_FILENAME:
		{
			e->SetString(&(m_VMEManager->GetFileName()));
		}
		break;

		default:
			albaLogicWithManagers::OnEvent(alba_event);
			break;
		} // end switch case
	} // end if SafeDowncast
	else
		albaLogicWithManagers::OnEvent(alba_event);
}

//----------------------------------------------------------------------------
void appLogic::RunOp(albaOp *op)
{
	m_OpManager->OpRun(op);
}

//----------------------------------------------------------------------------
void appLogic::ViewCreate(int viewId)
{
	albaView *currentView = NULL;
	
	//currentView = m_ViewManager->GetSelectedView(); // Create only one view

	if (!currentView)
	{
		currentView = m_ViewManager->ViewCreate(viewId);
		currentView->GetFrame()->SetMaxSize(currentView->GetFrame()->GetSize());
		ShowVMEOnView();
	}
}

//----------------------------------------------------------------------------
void appLogic::ShowVMEOnView()
{
	albaVMERoot *root = this->m_VMEManager->GetRoot();
	albaVMEIterator *iter = root->NewIterator();
	albaVME *volume = NULL;

	for (albaVME *iNode = iter->GetFirstNode(); iNode; iNode = iter->GetNextNode())
	{
		if (iNode->IsA("albaVMEVolumeGray"))
		{
			volume = iNode;
		}
	}
	iter->Delete();

	if (volume)
	{
		VmeShow(volume, true);
	}
}
//----------------------------------------------------------------------------
void appLogic::VmeShow(albaVME *vme, bool visibility)
{
// 	if (vme->IsA("albaVMEVolumeGray") && visibility)
// 	{
// 
// 	}

	albaLogicWithManagers::VmeShow(vme, visibility);

	m_TaskBar->VmeShow(vme, visibility);
}
//----------------------------------------------------------------------------
void appLogic::VmeSelect(albaVME *vme)
{
// 	if (vme->IsA("appEmptyVME"))
// 	{
// 
// 	}

	albaLogicWithManagers::VmeSelect(vme);

	m_TaskBar->VmeSelected(vme);
}
//----------------------------------------------------------------------------
void appLogic::VmeAdded(albaVME *vme)
{
	albaLogicWithManagers::VmeAdded(vme);

	m_TaskBar->VmeAdd(vme);
}

//----------------------------------------------------------------------------
void appLogic::CreateMenu()
{
	// Include Icons
#include "pic/menu/EDIT_COPY.xpm"
#include "pic/menu/EDIT_CUT.xpm"
#include "pic/menu/EDIT_DELETE.xpm"
#include "pic/menu/EDIT_PASTE.xpm"
#include "pic/menu/EDIT_REPARENT.xpm"
#include "pic/menu/EDIT_FIND.xpm"
#include "pic/menu/EDIT_REDO.xpm"
#include "pic/menu/EDIT_SETTINGS.xpm"
#include "pic/menu/EDIT_UNDO.xpm"
#include "pic/menu/FILE_EXIT.xpm"
#include "pic/menu/FILE_NEW.xpm"
#include "pic/menu/FILE_OPEN.xpm"
#include "pic/menu/FILE_PRINT.xpm"
#include "pic/menu/FILE_PRINT_PREVIEW.xpm"
#include "pic/menu/FILE_SAVE.xpm"
#include "pic/menu/HELP_ABOUT.xpm"
#include "pic/menu/HELP_HELP.xpm"
#include "pic/menu/HELP_LICENCE.xpm"

	m_MenuBar = new wxMenuBar;

	// File Menu //////////////////////////////////////////////////////////////////////////
	wxMenu *file_menu = new wxMenu;
	albaGUI::AddMenuItem(file_menu, MENU_FILE_NEW, _("&New  \tCtrl+N"), FILE_NEW_xpm);
	albaGUI::AddMenuItem(file_menu, MENU_FILE_OPEN, _("&Open   \tCtrl+O"), FILE_OPEN_xpm);
	albaGUI::AddMenuItem(file_menu, MENU_FILE_SAVE, _("&Save  \tCtrl+S"), FILE_SAVE_xpm);
	file_menu->Append(MENU_FILE_SAVEAS, _("Save &As  \tCtrl+Shift+S"));

	file_menu->AppendSeparator();

	// Import menu item
	m_ImportMenu = new wxMenu;
	file_menu->Append(0, _("Import"), m_ImportMenu);

	// Export menu item
	m_ExportMenu = new wxMenu;
	file_menu->Append(0, _("Export"), m_ExportMenu);

	// Print menu item
	file_menu->AppendSeparator();
	albaGUI::AddMenuItem(file_menu, MENU_FILE_PRINT, _("&Print  \tCtrl+P"), FILE_PRINT_xpm);
	albaGUI::AddMenuItem(file_menu, MENU_FILE_PRINT_PREVIEW, _("Print Preview"), FILE_PRINT_PREVIEW_xpm);
	file_menu->Append(MENU_FILE_PRINT_SETUP, _("Printer Setup"));
	file_menu->Append(MENU_FILE_PRINT_PAGE_SETUP, _("Page Setup"));

	// Recent file item
	m_RecentFileMenu = new wxMenu;
	file_menu->AppendSeparator();
	file_menu->Append(0, _("Recent Files"), m_RecentFileMenu);
	file_menu->AppendSeparator();
	albaGUI::AddMenuItem(file_menu, MENU_FILE_QUIT, _("&Quit  \tCtrl+Q"), FILE_EXIT_xpm);
	
	m_MenuBar->Append(file_menu, _("&File"));

	// Edit Menu //////////////////////////////////////////////////////////////////////////
	wxMenu *edit_menu = new wxMenu;
	albaGUI::AddMenuItem(edit_menu, OP_UNDO, _("Undo  \tCtrl+Z"), EDIT_UNDO_xpm);
	albaGUI::AddMenuItem(edit_menu, OP_REDO, _("Redo  \tCtrl+Shift+Z"), EDIT_REDO_xpm);

	edit_menu->AppendSeparator();
	albaGUI::AddMenuItem(edit_menu, OP_CUT, _("Cut   \tCtrl+Shift+X"), EDIT_CUT_xpm);
	albaGUI::AddMenuItem(edit_menu, OP_COPY, _("Copy  \tCtrl+Shift+C"), EDIT_COPY_xpm);
	albaGUI::AddMenuItem(edit_menu, OP_PASTE, _("Paste \tCtrl+Shift+V"), EDIT_PASTE_xpm);
	albaGUI::AddMenuItem(edit_menu, OP_DELETE, _("Delete  \tCtrl+Shift+Del"), EDIT_DELETE_xpm);

	edit_menu->AppendSeparator();
	albaGUI::AddMenuItem(edit_menu, OP_REPARENT, _("Reparent to... \tCtrl+Shift+R"), EDIT_REPARENT_xpm);
	albaGUI::AddMenuItem(edit_menu, MENU_EDIT_FIND_VME, _("Find VME \tCtrl+Shift+F"), EDIT_FIND_xpm);

	albaGUI::AddMenuItem(edit_menu, ID_APP_SETTINGS, _("Settings..."), EDIT_SETTINGS_xpm);
	m_MenuBar->Append(edit_menu, _("&Edit"));

	// View Menu //////////////////////////////////////////////////////////////////////////
	m_ViewMenu = new wxMenu;
	m_MenuBar->Append(m_ViewMenu, _("&View"));

	// Operations Menu ////////////////////////////////////////////////////////////////////
	m_OpMenu = new wxMenu;
	m_MenuBar->Append(m_OpMenu, _("&Operations"));

	// Wizard Menu ////////////////////////////////////////////////////////////////////

	if (m_UseWizardManager)
	{
		m_WizardMenu = new wxMenu;
		m_MenuBar->Append(m_WizardMenu, _("&Wizard"));
	}

	// Help Menu //////////////////////////////////////////////////////////////////////////
	wxMenu *help_menu = new wxMenu;
	albaGUI::AddMenuItem(help_menu, ABOUT_APPLICATION, _("About"), HELP_ABOUT_xpm);
	//albaGUI::AddMenuItem(help_menu, REGISTER_PRODUCT, _("Register Product"), HELP_LICENCE_xpm);
	albaGUI::AddMenuItem(help_menu, HELP_HOME, _("Help"), HELP_HELP_xpm);
	m_MenuBar->Append(help_menu, _("&Help"));

	m_Win->SetMenuBar(m_MenuBar);

	EnableItem(MENU_FILE_PRINT, false);
	EnableItem(MENU_FILE_PRINT_PREVIEW, false);
	EnableItem(MENU_FILE_PRINT_SETUP, false);
	EnableItem(MENU_FILE_PRINT_PAGE_SETUP, false);
}
//----------------------------------------------------------------------------
void appLogic::CreateToolbar()
{
	m_ToolBar = new wxToolBar(m_Win, MENU_VIEW_TOOLBAR, wxPoint(0, 0), wxSize(-1, -1), wxTB_FLAT | wxTB_NODIVIDER);
	m_ToolBar->SetMargins(0, 0);
	m_ToolBar->SetToolSeparation(2);
	m_ToolBar->SetToolBitmapSize(wxSize(20, 20));

	// File
	m_ToolBar->AddTool(MENU_FILE_NEW, albaPictureFactory::GetPictureFactory()->GetBmp("FILE_NEW"), _("New project"));
	m_ToolBar->AddTool(MENU_FILE_OPEN, albaPictureFactory::GetPictureFactory()->GetBmp("FILE_OPEN"), _("Open project"));
	m_ToolBar->AddTool(MENU_FILE_SAVE, albaPictureFactory::GetPictureFactory()->GetBmp("FILE_SAVE"), _("Save project"));
	m_ToolBar->AddSeparator();

	// Importer
	m_ToolBar->AddTool(OP_IMPORTER_DICOM, albaPictureFactory::GetPictureFactory()->GetBmp("IMPORT_DICOM"), _("Importer DICOM"));
	m_ToolBar->AddSeparator();

	// Operations

	//m_ToolBar->AddSeparator();

	// Views	
	m_ToolBar->AddTool(ID_SHOW_VIEW_VTK_SURFACE, albaPictureFactory::GetPictureFactory()->GetBmp("VIEW_SURFACE_ICON"), _("View Surface"));
	m_ToolBar->AddTool(ID_SHOW_IMAGE_VIEW, albaPictureFactory::GetPictureFactory()->GetBmp("VIEW_SURFACE_ICON"), _("View Image"));
	m_ToolBar->AddTool(ID_SHOW_SLICE_VIEW, albaPictureFactory::GetPictureFactory()->GetBmp("VIEW_SURFACE_ICON"), _("View Slice"));
	//m_ToolBar->AddSeparator();

	if (m_UseSnapshotManager)
	{
		// Snapshot tool
		m_ToolBar->AddSeparator();
		m_ToolBar->AddTool(MENU_FILE_SNAPSHOT, albaPictureFactory::GetPictureFactory()->GetBmp("CAMERA"), _("Create Snapshot"));
		m_ToolBar->AddTool(MENU_FILE_MANAGE_SNAPSHOT, albaPictureFactory::GetPictureFactory()->GetBmp("IMAGE_PREVIEW"), _("Manage Snapshots"));
	}

	m_ToolBar->AddTool(ID_ADD_COMMENT, albaPictureFactory::GetPictureFactory()->GetBmp("COMMENT_ICON"), _("Comment"));

	//m_ToolBar->EnableTool(OP_IMPORTER_DICOM, true);
}
//----------------------------------------------------------------------------
void appLogic::EnableMenuAndToolbar()
{
	bool enable = !(m_RunningOperation || m_WizardRunning);

	albaLogicWithManagers::EnableMenuAndToolbar();

	albaVME *node = m_OpManager->GetSelectedVme();

 	m_ToolBar->EnableTool(OP_IMPORTER_DICOM, enable && node && m_OpImporterDicom->Accept(node));

	if (m_UseSnapshotManager && m_SnapshotManager)
	{
		EnableItem(MENU_FILE_MANAGE_SNAPSHOT, enable && m_SnapshotManager->HasSnapshots(m_VMEManager->GetRoot()));
		EnableItem(MENU_FILE_SNAPSHOT, enable && m_ViewManager->GetSelectedView() != NULL);
	}
}

//----------------------------------------------------------------------------
void appLogic::CreateControlPanel()
{
	m_SidebarStyle = albaSideBar::DOUBLE_NOTEBOOK;
	m_SideBar = new albaSideBar(m_Win, MENU_VIEW_SIDEBAR, this, m_SidebarStyle); //Default SideBar
	//m_SideBar = new appSideBar(m_Win, MENU_VIEW_SIDEBAR, this); //Custom SideBar

	m_TaskBar = new appTaskBar(m_Win, MENU_VIEW_TASKBAR, this); //Default TaskBar
}