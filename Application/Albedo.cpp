/*=========================================================================
Program:   Albedo
Module:    Albedo.cpp
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

//Enable This for memory leak detection
//#include <vld.h>

#include "appDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the APP must include "appDefines.h" as first.
// This force to include Window, wxWidgets and VTK exactly in this order.
//----------------------------------------------------------------------------

#include "Albedo.h"
#include "appDecl.h"

#include "appOpCreateSurfaceParametric.h"
#include "appOpEmpty.h"
#include "appOpGuiSample.h"
//#include "appOpInteractorSample.h"
#include "appOpCreateEmptyVME.h"
#include "appOpDictionary.h"
#include "appOpShowDialog.h"
#include "appUtils.h"
#include "appViewImageCompound.h"
#include "appEmptyVME.h"
#include "appVMESurfaceParametric.h"

#include "mafGUIDicomSettings.h"
#include "mafOpExporterSTL.h"
#include "mafOpExporterVTK.h"
#include "mafOpImporterDicomOffis.h"
#include "mafOpImporterImage.h"
#include "mafOpImporterSTL.h"
#include "mafOpImporterVTK.h"
#include "mafOpMAFTransform.h"
#include "mafPipeFactoryVME.h"
#include "mafPipeVolumeSliceBlend.h"
#include "mafServiceLocator.h"
#include "mafVMEFactory.h" 
#include "mafViewVTK.h"

#include <vtkTimerLog.h>

//Comment this line to disable wizard sample
#define USE_WIZARD

#ifdef USE_WIZARD
#include "mafWizard.h"
#include "appWizardSample.h"
#endif

//--------------------------------------------------------------------------------
// Create the Application
//--------------------------------------------------------------------------------

IMPLEMENT_APP(Albedo)

//--------------------------------------------------------------------------------
bool Albedo::OnInit()
{
	InitializeIcons();
	
	InitializeReg();

	int result;
  result = mafVMEFactory::Initialize();
	assert(result == MAF_OK);

	result = mafPipeFactoryVME::Initialize();
	assert(result == MAF_OK);

	// Plug custom VME
 	mafPlugVME<appEmptyVME>("Empty VME");
 	mafPlugVME<appVMESurfaceParametric>("Parametric Surface");

	// Plug pipe to visualize custom VME
// 	mafPlugPipe<appPipeCustomVME>("Pipe to visualize custom VME");  // Coming soon

	// Create Logic Manager
	m_Logic = new appLogic();
	mafServiceLocator::SetLogicManager(m_Logic);
	m_Logic->GetTopWin()->SetTitle("Albedo");

#ifdef USE_WIZARD
	m_Logic->PlugWizardManager(true);
#endif

	// Plug and Enable Snapshot Manager
	m_Logic->PlugSnapshotManager(true);

	m_Logic->Configure();

	SetTopWindow(mafGetFrame());

	//////////////////////////////////////////////////////////////////////////
	//Importers
	//////////////////////////////////////////////////////////////////////////

	// DICOM Importer
	mafGUIDicomSettings *dicomSettings = new mafGUIDicomSettings(NULL, "DICOM");
	m_Logic->Plug(new mafOpImporterDicomOffis("DICOM", true), "", true, dicomSettings);

	// Images Importer
	m_Logic->Plug(new mafOpImporterImage("Images"));
	
	// STL Importer
	m_Logic->Plug(new mafOpImporterSTL("STL"));
	
	// VTK Importer
	m_Logic->Plug(new mafOpImporterVTK("VTK"));	

	//////////////////////////////////////////////////////////////////////////
	//Exporters
	//////////////////////////////////////////////////////////////////////////

	// STL Exporter
	m_Logic->Plug(new mafOpExporterSTL("STL"));

	// VTK Exporter
	m_Logic->Plug(new mafOpExporterVTK("VTK"));

	//////////////////////////////////////////////////////////////////////////
	//Operations
	//////////////////////////////////////////////////////////////////////////	

	// Empty Operation
	m_Logic->Plug(new appOpEmpty("Op Empty"));

	// Transform Operation
	m_Logic->Plug(new mafOpMAFTransform("Transform \tCtrl+T"));// , _("Modify"));

	// Gui Interactor Operation
	//m_Logic->Plug(new appOpInteractorSample("Interactor Sample")); // Coming soon

	// Dictionary Operation
	m_Logic->Plug(new appOpDictionary("Dictionary Editor"));
	
	// Gui Sample Operation
	m_Logic->Plug(new appOpGuiSample("Gui Sample"), _("GUI"));

	// Show Dialog (Vertical) Operation
	m_Logic->Plug(new appOpShowDialog("Show Dialog (Vertical)"), _("GUI"));

	// Show Dialog (Horizontal) Operation
	m_Logic->Plug(new appOpShowDialog("Show Dialog (Horizontal)", appOpShowDialog::HORIZONTAL_MODE_2), _("GUI"));

	// Create Empty VME Operation
	m_Logic->Plug(new appOpCreateEmptyVME("Create Empty VME"), _("Create"));

	// Create Surface Parametric Operation
	m_Logic->Plug(new appOpCreateSurfaceParametric("Create Parametric Surface"), _("Create"));

	//////////////////////////////////////////////////////////////////////////
	//Views
	//////////////////////////////////////////////////////////////////////////

	// VTK View (Surface)
	m_Logic->Plug(new mafViewVTK("Surface"));	

	// Image Compound View
	appViewImageCompound *view_image = new appViewImageCompound("Image");
	view_image->PackageView();
	m_Logic->Plug(view_image);

#ifdef USE_WIZARD
	//////////////////////////////////////////////////////////////////////////
	//Wizard
	//////////////////////////////////////////////////////////////////////////
	
	//A simple wizard sample
	m_Logic->Plug(new appWizardSample("Wizard Sample"), "");

#endif //USE_WIZARD

	//////////////////////////////////////////////////////////////////////////

	// Splash Screen
	mafString splashImageName = "AlbedoSplash.bmp";

	wxString splashDir = appUtils::GetConfigDirectory().c_str();
	wxBitmap splashBitmap;
	splashBitmap.LoadFile(splashDir + "\\" + splashImageName, wxBITMAP_TYPE_BMP);
	// m_Logic->ShowSplashScreen(splashBitmap); // Uncomment to show SplashScreen

	//////////////////////////////////////////////////////////////////////////
	wxHandleFatalExceptions();

	m_Logic->Show();
	m_Logic->Init(argc, argv); // Calls FileNew - which create the root

	return TRUE;
}

//--------------------------------------------------------------------------------
int Albedo::OnExit()
{
  cppDEL(m_Logic);

  // This hack is fixing VTK internal memory leak
  vtkTimerLog::CleanupLog();
  return 0;
}

//--------------------------------------------------------------------------------
void Albedo::InitializeIcons()
{
	mafPictureFactory::GetPictureFactory()->Initialize();

#include "pic/FRAME_ICON16x16.xpm"
	mafADDPIC(FRAME_ICON16x16);
#include "pic/FRAME_ICON32x32.xpm"
	mafADDPIC(FRAME_ICON32x32);
#include "pic/MDICHILD_ICON.xpm"
	mafADDPIC(MDICHILD_ICON);

#include "pic/CAMERA.xpm"
	mafADDPIC(CAMERA);
#include "pic/IMPORT_DICOM.xpm"
	mafADDPIC(IMPORT_DICOM);
#include "pic/VIEW_SURFACE_ICON.xpm"
	mafADDPIC(VIEW_SURFACE_ICON);
#include "pic/SHOW_RULER_ICON.xpm"
	mafADDPIC(SHOW_RULER_ICON);
#include "pic/HIDE_RULER_ICON.xpm"
	mafADDPIC(HIDE_RULER_ICON);
}

//--------------------------------------------------------------------------------
void Albedo::InitializeReg()
{
  wxConfig *config = new wxConfig(wxEmptyString);

  long long_item;
  wxString string_item;

  config->SetPath("Importer Dicom"); 

  if(!config->Read("EnableTimeBar", &long_item))
  {
    config->Write("EnableTimeBar",FALSE);
  }
		  
  if(!config->Read("EnableTypeImage", &long_item))
  {
    config->Write("EnableTypeImage",FALSE);
  }
	  
  cppDEL(config);
}

//----------------------------------------------------------------------------
void Albedo::OnFatalException()
{
	m_Logic->HandleException();
}

//--------------------------------------------------------------------------------
int Albedo::FilterEvent(wxEvent& event)
{
	int keyCode = ((wxKeyEvent&)event).GetKeyCode();
	bool controlDown = ((wxKeyEvent&)event).ControlDown();

	// Press Button
	if (event.GetEventType() == wxEVT_KEY_DOWN)
	{
		if (keyCode == 'A' && controlDown) //Check CTRL+A Key  
		{
			//
			return true;
		}
		else if (keyCode == 'O' && !m_Logic->IsOperationRunning()) //Check O Key  
		{
			//
			return true;
		}
 	}

	// Release Button
	if (event.GetEventType() == wxEVT_KEY_UP && (keyCode == 'O') && !m_Logic->IsOperationRunning())
	{
		//	
	}

	return -1;
}
