/*=========================================================================
Program:   Albedo
Module:    Albedo.cpp
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

//Enable This for memory leak detection
//#include <vld.h>

#include "appDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the APP must include "appDefines.h" as first.
// This force to include Window, wxWidgets and VTK exactly in this order.
//----------------------------------------------------------------------------

#include "Albedo.h"
#include "appDecl.h"

#include "appOpAddComment.h"
#include "appOpCreateSurfaceParametric.h"
#include "appOpEmpty.h"
#include "appOpGuiSample.h"
#include "appOpInteractorSample.h"
#include "appOpInteractorSliderSample.h"
#include "appOpCreateEmptyVME.h"
#include "appOpCursorTest.h"
#include "appOpDictionary.h"
#include "appOpMeasure2D.h"
#include "appOpShowDialog.h"
#include "appUtils.h"
#include "appViewImageCompound.h"
#include "appEmptyVME.h"
#include "appVMESurfaceParametric.h"

#include "albaGUIDicomSettings.h"
#include "albaOpExporterSTL.h"
#include "albaOpExporterVTK.h"
#include "albaOpImporterDicom.h"
#include "albaOpImporterImage.h"
#include "albaOpImporterSTL.h"
#include "albaOpImporterVTK.h"
#include "albaOpTransform.h"
#include "albaPipeFactoryVME.h"
#include "albaPipeVolumeSliceBlend.h"
#include "albaServiceLocator.h"
#include "albaVMEFactory.h" 
#include "albaViewVTK.h"
#include "albaViewGlobalSlice.h"

#include <vtkTimerLog.h>

//Comment this line to disable wizard sample
#define USE_WIZARD

#ifdef USE_WIZARD
#include "albaWizard.h"
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
  result = albaVMEFactory::Initialize();
	assert(result == ALBA_OK);

	result = albaPipeFactoryVME::Initialize();
	assert(result == ALBA_OK);

	// Plug custom VME
 	albaPlugVME<appEmptyVME>("Empty VME");
 	albaPlugVME<appVMESurfaceParametric>("Parametric Surface");

	// Plug pipe to visualize custom VME
// 	albaPlugPipe<appPipeCustomVME>("Pipe to visualize custom VME");  // Coming soon

	// Create Logic Manager
	m_Logic = new appLogic();
	albaServiceLocator::SetLogicManager(m_Logic);
	m_Logic->GetTopWin()->SetTitle("Albedo");

#ifdef USE_WIZARD
	m_Logic->PlugWizardManager(true);
#endif

	// Plug and Enable Snapshot Manager
	m_Logic->PlugSnapshotManager(true);

	m_Logic->Configure();

	SetTopWindow(albaGetFrame());

	//////////////////////////////////////////////////////////////////////////
	//Importers
	//////////////////////////////////////////////////////////////////////////

	// DICOM Importer
	albaGUIDicomSettings *dicomSettings = new albaGUIDicomSettings(NULL, "DICOM");
	m_Logic->Plug(new albaOpImporterDicom("DICOM", true), "", true, dicomSettings);

	// Images Importer
	m_Logic->Plug(new albaOpImporterImage("Images"));
	
	// STL Importer
	m_Logic->Plug(new albaOpImporterSTL("STL"));
	
	// VTK Importer
	m_Logic->Plug(new albaOpImporterVTK("VTK"));	

	//////////////////////////////////////////////////////////////////////////
	//Exporters
	//////////////////////////////////////////////////////////////////////////

	// STL Exporter
	m_Logic->Plug(new albaOpExporterSTL("STL"));

	// VTK Exporter
	m_Logic->Plug(new albaOpExporterVTK("VTK"));

	//////////////////////////////////////////////////////////////////////////
	//Operations
	//////////////////////////////////////////////////////////////////////////	

	// Empty Operation
	m_Logic->Plug(new appOpEmpty("Op Empty"));

	// Test Comment Operation
	m_Logic->Plug(new appOpAddComment("Add Comment (Prototype)"));

	// Create Empty VME Operation
	m_Logic->Plug(new appOpCreateEmptyVME("Create Empty VME"), _("Create"));

	// Create Surface Parametric Operation
	m_Logic->Plug(new appOpCreateSurfaceParametric("Create Parametric Surface"), _("Create"));

	// Transform Operation
	m_Logic->Plug(new albaOpTransform("Transform \tCtrl+T"), _("Edit"));

	// Dictionary Operation
	m_Logic->Plug(new appOpDictionary("Dictionary Editor"), _("Edit"));
	
	// Show Dialog (Vertical) Operation
	m_Logic->Plug(new appOpShowDialog("Show Dialog (Vertical)"), _("GUI"));

	// Show Dialog (Horizontal) Operation
	m_Logic->Plug(new appOpShowDialog("Show Dialog (Horizontal)", appOpShowDialog::HORIZONTAL_MODE_2), _("GUI"));

	// Test Interactor Operation
	m_Logic->Plug(new appOpInteractorSample("Interactor Sample"), _("Test"));

	// Test Interactor Slider Operation
	m_Logic->Plug(new appOpInteractorSliderSample("Interactor Slider Sample"), _("Test"));

	// Test Gui Sample Operation
	m_Logic->Plug(new appOpGuiSample("Gui Sample"), _("Test"));

	// Test Show Dialog (Vertical) Operation
	m_Logic->Plug(new appOpCursorTest("Cursor Test"), _("Test"));

	// Test Interactor Operation
	m_Logic->Plug(new appOpMeasure2D("Measure 2D (Prototype)"));

	//////////////////////////////////////////////////////////////////////////
	//Views
	//////////////////////////////////////////////////////////////////////////

	// VTK View (Surface)
	m_Logic->Plug(new albaViewVTK("Surface"));	

	// Image Compound View
	appViewImageCompound *view_image = new appViewImageCompound("Image");
	view_image->PackageView();
	m_Logic->Plug(view_image);

	//View Global Slice 
	albaViewGlobalSlice *vGlobalS = new albaViewGlobalSlice("Global Slice");
	m_Logic->Plug(vGlobalS);

#ifdef USE_WIZARD
	//////////////////////////////////////////////////////////////////////////
	//Wizard
	//////////////////////////////////////////////////////////////////////////
	
	//A simple wizard sample
	m_Logic->Plug(new appWizardSample("Wizard Sample"), "");

#endif //USE_WIZARD

	//////////////////////////////////////////////////////////////////////////

	// Splash Screen
	albaString splashImageName = "AlbedoSplash.bmp";

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
	albaPictureFactory::GetPictureFactory()->Initialize();

#include "pic/FRAME_ICON16x16.xpm"
	albaADDPIC(FRAME_ICON16x16);
#include "pic/FRAME_ICON32x32.xpm"
	albaADDPIC(FRAME_ICON32x32);
#include "pic/FRAME_ICON64x64.xpm"
	albaADDPIC(FRAME_ICON64x64);
#include "pic/FRAME_ICON128x128.xpm"
	albaADDPIC(FRAME_ICON128x128);
#include "pic/FRAME_ICON256x256.xpm"
	albaADDPIC(FRAME_ICON256x256);
#include "pic/MDICHILD_ICON.xpm"
	albaADDPIC(MDICHILD_ICON);

#include "pic/CAMERA.xpm"
	albaADDPIC(CAMERA);
#include "pic/IMPORT_DICOM.xpm"
	albaADDPIC(IMPORT_DICOM);
#include "pic/VIEW_SURFACE_ICON.xpm"
	albaADDPIC(VIEW_SURFACE_ICON);
#include "pic/SHOW_RULER_ICON.xpm"
	albaADDPIC(SHOW_RULER_ICON);
#include "pic/HIDE_RULER_ICON.xpm"
	albaADDPIC(HIDE_RULER_ICON);
#include "pic/COMMENT_ICON.xpm"
	albaADDPIC(COMMENT_ICON);
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
