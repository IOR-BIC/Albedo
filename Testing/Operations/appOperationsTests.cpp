/*=========================================================================
Program:   Albedo
Module:    appOperationsTests.cpp
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

// Visual Leak Detector
//#include <vld.h>

#include "appOperationsTests.h"
#include "../appFakeLogicForTest.h"
#include "appOpEmptyTest.h"

#include "albaServiceLocator.h"
#include "albaVMEOutputNULL.h"

#include "vtkFileOutputWindow.h"
#include "vtkALBASmartPointer.h"

#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

//-------------------------------------------------------------------------
albaCxxTypeMacro(DummyVme);
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
albaVMEOutput *DummyVme::GetOutput()
{
	// Allocate the right type of output on demand
	if (m_Output == NULL)
	{
		SetOutput(albaVMEOutputNULL::New()); // create the output
	}
	return m_Output;
}

//----------------------------------------------------------------------------
void DummyObserver::OnEvent(albaEventBase *alba_event)
{
	if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
		if (e->GetId() == VME_CHOOSE)
		{
			e->SetVme(m_DummyVme);
		}
	}
}

/* Main Test Executor */
//----------------------------------------------------------------------------
int	main( int argc, char* argv[] )
{
	// Create log of VTK error messages
	vtkALBASmartPointer<vtkFileOutputWindow> log;
	vtkOutputWindow::SetInstance(log);
	albaString logPath = wxGetWorkingDirectory();
	logPath << "\\vtkLog.txt";
	log->SetFileName(logPath);

	// Create the event manager and test controller
	CPPUNIT_NS::TestResult controller;

	// Add a listener that collects test result
	CPPUNIT_NS::TestResultCollector result;
	controller.addListener( &result );        

	// Add a listener that print dots as test run.
	CPPUNIT_NS::BriefTestProgressListener progress;
	controller.addListener( &progress );      

 	appFakeLogicForTest *logic = new appFakeLogicForTest();
 	albaServiceLocator::SetLogicManager(logic);

	// Add the top suite to the test runner
	CPPUNIT_NS::TestRunner runner;

	//////////////////////////////////////////////////////////////////////////
  // Add Tests
  runner.addTest(appOpEmptyTest::suite());


	//////////////////////////////////////////////////////////////////////////
	runner.run( controller );

	// Print test in a compiler compatible format.
	CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
	outputter.write(); 

	return result.wasSuccessful() ? 0 : 1;
}