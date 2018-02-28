/*=========================================================================
Program:   Albedo
Module:    appTest.cpp
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

#include "appTest.h"

#include "vtkBMPReader.h"
#include "vtkBMPWriter.h"
#include "vtkDataArray.h"
#include "vtkDataSet.h"
#include "vtkDataSetAttributes.h"
#include "vtkImageData.h"
#include "vtkImageMathematics.h"
#include "vtkImageSource.h"
#include "vtkJPEGReader.h"
#include "vtkJPEGWriter.h"
#include "vtkPNGReader.h"
#include "vtkPNGWriter.h"
#include "vtkWindowToImageFilter.h"

#include <cppunit/config/SourcePrefix.h>

IMPLEMENT_APP(TestApp)

//--------------------------------------------------------------------------------
bool TestApp::OnInit()
{
	return TRUE;
}

//--------------------------------------------------------------------------------
int TestApp::OnExit()
{
	wxApp::CleanUp();
	return 0;
}

//--------------------------------------------------------------------------------
bool TestApp::Yield(bool onlyIfNeeded)
{
	return true;
};

//----------------------------------------------------------------------------
appTest::appTest()
{
	m_Renderer = NULL;
	m_RenderWindow = NULL;

	m_WorkingDir = wxGetWorkingDirectory();
}

//----------------------------------------------------------------------------
void appTest::setUp()
{
	m_App = new TestApp();  // Instantiate the application class
	m_App->argc = 0;        // set the number of input argument to 0
	m_App->argv = NULL;     // set to NULL the input argument's parameters
	wxTheApp->SetAppName("mafUserTest"); // Set the name for the application

	//NOTE, wxLog produces some memory leaks, set false during test
	wxLog::EnableLogging(true);

	//Run Test Specific Stuff
	BeforeTest();
}
//----------------------------------------------------------------------------
void appTest::tearDown()
{
	//Clean Test Specific Stuff
	AfterTest();
		
	cppDEL(m_App);  // Destroy the application
	wxAppConsole::SetInstance(NULL);	
}

//----------------------------------------------------------------------------
void appTest::CompareImage(mafString suiteName, mafString imageName, int index)
{
	if (!m_RenderWindow) 
		return;

	// Visualization control
	m_RenderWindow->OffScreenRenderingOn();
	vtkWindowToImageFilter *windowToImage;
	vtkNEW(windowToImage);
	windowToImage->SetInput(m_RenderWindow);
	//windowToImage->SetMagnification(magnification);
	windowToImage->Update();
	m_RenderWindow->OffScreenRenderingOff();

	wxString imageFolder = GetTestDataDir(suiteName);
	mafString imageFileBase;
	if (index >= 0)
	{
		imageFileBase.Printf("%s\\%s%d", imageFolder.c_str(), imageName.GetCStr(), index);
	}
	else
	{
		imageFileBase.Printf("%s\\%s", imageFolder.c_str(), imageName.GetCStr());
	}

	wxString ext = ".png";
	vtkPNGWriter *imageWriter;
	vtkNEW(imageWriter);
	
	// Write comparing image
	wxString imageFileStored = imageFileBase + ext;

	if (wxFileExists(imageFileStored))
	{
		// Read original Image
		vtkPNGReader *imageReader;
		vtkNEW(imageReader);
		imageReader->SetFileName(imageFileStored);
		imageReader->Update();
		vtkImageData *imDataOrig = imageReader->GetOutput();

		vtkImageData *imDataComp = windowToImage->GetOutput(); // imageReader2->GetOutput();

		// Compare
		vtkImageMathematics *imageMath = vtkImageMathematics::New();
		imageMath->SetInput1(imDataOrig);
		imageMath->SetInput2(imDataComp);
		imageMath->SetOperationToSubtract();
		imageMath->Update();

		double srR[2] = { -1,1 };
		imageMath->GetOutput()->GetPointData()->GetScalars()->GetRange(srR);

		bool result = srR[0] == 0.0 && srR[1] == 0.0;

		wxString imageFileDiff = imageFileBase + "_DIFF" + ext;
		wxString imageFileNew = imageFileBase + "_NEW" + ext;

		if (wxFileExists(imageFileDiff))
			wxRemoveFile(imageFileDiff);
		if (wxFileExists(imageFileNew))
			wxRemoveFile(imageFileNew);
		
		if (!result)
		{
			imageWriter->SetInput(imageMath->GetOutput());
			imageWriter->SetFileName(imageFileDiff);
			imageWriter->Write();
			imageWriter->SetInput(imDataComp);
			imageWriter->SetFileName(imageFileNew);
			imageWriter->Write();

			mafLogMessage("CompareImages has found differences. File %s stored.", imageFileNew);
		}

		CPPUNIT_ASSERT(result);

		// End visualization control
		vtkDEL(imageMath);
		vtkDEL(imageReader);
	}
	else
	{
		//First run storing file
		imageWriter->SetInput(windowToImage->GetOutput());
		imageWriter->SetFileName(imageFileStored);
		imageWriter->Write();
		return;
	}

	vtkDEL(imageWriter);
	vtkDEL(windowToImage);
}

//----------------------------------------------------------------------------
mafString appTest::GetTestDataDir(mafString suiteName)
{
	wxString testDataFolder = m_WorkingDir;
	testDataFolder += "\\" + suiteName;

	if (!wxDirExists(testDataFolder))
	{
		wxMkdir(testDataFolder);
	}

	return testDataFolder;
}