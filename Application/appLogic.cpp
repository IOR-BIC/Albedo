/*=========================================================================
Program:   Albedo
Module:    appLogic.cpp
Language:  C++
Date:      $Date: 2018-01-01 12:00:00 $
Version:   $Revision: 1.0.0.0 $
Authors:   Nicola Vanella
==========================================================================
Copyright (c) LTM-IOR 2018 (https://github.com/IOR-LTM)

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
//#include "appSideBar.h"

#include "mafGUIAboutDialog.h"
#include "mafGUIApplicationSettings.h"
#include "mafLogicWithManagers.h"
#include "mafOpImporterDicomOffis.h"
#include "mafOpManager.h"
#include "mafSnapshotManager.h"
#include "mafVME.h"
#include "mafVMEGroup.h"
#include "mafVMEIterator.h"
#include "mafVMEVolumeGray.h"
#include "mafView.h"
#include "mafViewManager.h"

#ifdef USE_WIZARD
#include "mafWizardManager.h"
#endif

#include "vtkObject.h"
#include "vtkPointData.h"
#include "vtkPoints.h"

//----------------------------------------------------------------------------
appLogic::appLogic() : mafLogicWithManagers()
{
	m_OpeningMSF = false;

	// Init Operations to plug in Toolbar 
	m_OpImporterDicom = new mafOpImporterDicomOffis("DICOM", true);

	m_Win->Maximize();

	// Set project file extension
	m_Extension = "alb";
}
//----------------------------------------------------------------------------
appLogic::~appLogic()
{
	mafDEL(m_OpImporterDicom);

	delete m_SideBar;
}

//----------------------------------------------------------------------------
void appLogic::Init(int argc, char **argv)
{
	mafLogicWithManagers::Init(argc, argv);
	m_Win->ShowDockPane("logbar", false);

	SetRevision("1.0");

	if (!wxDirExists(mafGetAppDataDirectory().c_str()))
		wxMkDir(mafGetAppDataDirectory().c_str());

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
	m_AboutDialog->SetWebSite("https://github.com/IOR-LTM");
	//m_AboutDialog->SetLicenseURL("https://github.com/IOR-LTM");
	//m_AboutDialog->SetExtraMessage("");

	// About Image
	wxString imageName = "AlbedoAbout";
	wxString imagesPath = appUtils::GetConfigDirectory().c_str();
	m_AboutDialog->SetImagePath(imagesPath + "/" + imageName + ".bmp");
}

//----------------------------------------------------------------------------
void appLogic::OnEvent(mafEventBase *maf_event)
{
	if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
	{
		switch (e->GetId())
		{
		case VME_PICKED:
		{
			mafVME* selectedVme = m_OpManager->GetSelectedVme();
			//
		}
		break;

		// View Events
		case VIEW_VTK_SURFACE:
		{
			ViewCreate(VIEW_START);
		}
		break;
		case VIEW_IMAGE:
		{
			ViewCreate(VIEW_START + 1);
		}
		break;

		// Operations Events 
		case OP_IMPORTER_DICOM:
		{
			mafString op = "mafOpImporterDicomOffis";
			m_OpManager->OpRun(op);
		}
		break;

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
			mafString *filename = e->GetString();
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
			mafLogicWithManagers::OnEvent(maf_event);
			break;
		} // end switch case
	} // end if SafeDowncast
	else
		mafLogicWithManagers::OnEvent(maf_event);
}

//----------------------------------------------------------------------------
void appLogic::RunOp(mafOp *op)
{
	m_OpManager->OpRun(op);
}

//----------------------------------------------------------------------------
void appLogic::ViewCreate(int viewId)
{
	mafView *currentView = NULL;
	
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
	mafVMERoot *root = this->m_VMEManager->GetRoot();
	mafVMEIterator *iter = root->NewIterator();
	mafVME *volume = NULL;

	for (mafVME *iNode = iter->GetFirstNode(); iNode; iNode = iter->GetNextNode())
	{
		if (iNode->IsA("mafVMEVolumeGray"))
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
void appLogic::VmeShow(mafVME *vme, bool visibility)
{
// 	if (vme->IsA("mafVMEVolumeGray") && visibility)
// 	{
// 
// 	}

	mafLogicWithManagers::VmeShow(vme, visibility);
}
//----------------------------------------------------------------------------
void appLogic::VmeSelect(mafVME *vme)
{
	mafLogicWithManagers::VmeSelect(vme);
}
//----------------------------------------------------------------------------
void appLogic::VmeAdded(mafVME *vme)
{
	mafLogicWithManagers::VmeAdded(vme);
}

//----------------------------------------------------------------------------
void appLogic::CreateMenu()
{
	// Include Icons
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
	mafGUI::AddMenuItem(file_menu, MENU_FILE_NEW, _("&New  \tCtrl+N"), FILE_NEW_xpm);
	mafGUI::AddMenuItem(file_menu, MENU_FILE_OPEN, _("&Open   \tCtrl+O"), FILE_OPEN_xpm);
	mafGUI::AddMenuItem(file_menu, MENU_FILE_SAVE, _("&Save  \tCtrl+S"), FILE_SAVE_xpm);
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
	mafGUI::AddMenuItem(file_menu, MENU_FILE_PRINT, _("&Print  \tCtrl+P"), FILE_PRINT_xpm);
	mafGUI::AddMenuItem(file_menu, MENU_FILE_PRINT_PREVIEW, _("Print Preview"), FILE_PRINT_PREVIEW_xpm);
	file_menu->Append(MENU_FILE_PRINT_SETUP, _("Printer Setup"));
	file_menu->Append(MENU_FILE_PRINT_PAGE_SETUP, _("Page Setup"));

	// Recent file item
	m_RecentFileMenu = new wxMenu;
	file_menu->AppendSeparator();
	file_menu->Append(0, _("Recent Files"), m_RecentFileMenu);
	file_menu->AppendSeparator();
	mafGUI::AddMenuItem(file_menu, MENU_FILE_QUIT, _("&Quit  \tCtrl+Q"), FILE_EXIT_xpm);
	
	m_MenuBar->Append(file_menu, _("&File"));

	// Edit Menu //////////////////////////////////////////////////////////////////////////
	wxMenu *edit_menu = new wxMenu;
	mafGUI::AddMenuItem(edit_menu, OP_UNDO, _("Undo  \tCtrl+Z"), EDIT_UNDO_xpm);
	mafGUI::AddMenuItem(edit_menu, OP_REDO, _("Redo  \tCtrl+Shift+Z"), EDIT_REDO_xpm);
	edit_menu->AppendSeparator();

	mafGUI::AddMenuItem(edit_menu, ID_APP_SETTINGS, _("Settings..."), EDIT_SETTINGS_xpm);
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
	mafGUI::AddMenuItem(help_menu, ABOUT_APPLICATION, _("About"), HELP_ABOUT_xpm);
	mafGUI::AddMenuItem(help_menu, REGISTER_PRODUCT, _("Register Product"), HELP_LICENCE_xpm);
	mafGUI::AddMenuItem(help_menu, HELP_HOME, _("Help"), HELP_HELP_xpm);
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
	m_ToolBar->AddTool(MENU_FILE_NEW, mafPictureFactory::GetPictureFactory()->GetBmp("FILE_NEW"), _("New project"));
	m_ToolBar->AddTool(MENU_FILE_OPEN, mafPictureFactory::GetPictureFactory()->GetBmp("FILE_OPEN"), _("Open project"));
	m_ToolBar->AddTool(MENU_FILE_SAVE, mafPictureFactory::GetPictureFactory()->GetBmp("FILE_SAVE"), _("Save project"));
	m_ToolBar->AddSeparator();

	// Importer
	m_ToolBar->AddTool(OP_IMPORTER_DICOM, mafPictureFactory::GetPictureFactory()->GetBmp("IMPORT_DICOM"), _("Importer DICOM"));
	m_ToolBar->AddSeparator();

	// Operations

	//m_ToolBar->AddSeparator();

	// Views	
	m_ToolBar->AddTool(VIEW_VTK_SURFACE, mafPictureFactory::GetPictureFactory()->GetBmp("VIEW_SURFACE_ICON"), _("View Surface"));
	m_ToolBar->AddTool(VIEW_IMAGE, mafPictureFactory::GetPictureFactory()->GetBmp("VIEW_SURFACE_ICON"), _("View Image"));
	//m_ToolBar->AddSeparator();

	if (m_UseSnapshotManager)
	{
		// Snapshot tool
		m_ToolBar->AddSeparator();
		m_ToolBar->AddTool(MENU_FILE_SNAPSHOT, mafPictureFactory::GetPictureFactory()->GetBmp("CAMERA"), _("Create Snapshot"));
		m_ToolBar->AddTool(MENU_FILE_MANAGE_SNAPSHOT, mafPictureFactory::GetPictureFactory()->GetBmp("IMAGE_PREVIEW"), _("Manage Snapshots"));
	}

	//m_ToolBar->EnableTool(OP_IMPORTER_DICOM, true);
}
//----------------------------------------------------------------------------
void appLogic::EnableMenuAndToolbar()
{
	bool enable = !(m_RunningOperation || m_WizardRunning);

	mafLogicWithManagers::EnableMenuAndToolbar();

	mafVME *node = m_OpManager->GetSelectedVme();

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
	m_SidebarStyle = mafSideBar::DOUBLE_NOTEBOOK;
	m_SideBar = new mafSideBar(m_Win, MENU_VIEW_SIDEBAR, this, m_SidebarStyle); //Default SideBar
	//m_SideBar = new appSideBar(m_Win, MENU_VIEW_SIDEBAR, this); //Custom SideBar
}