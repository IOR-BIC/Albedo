/*=========================================================================
Program:   Albedo
Module:    appTest.cpp
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

#ifndef __CPP_UNIT_appTest_H__
#define __CPP_UNIT_appTest_H__

#include "mafIncludeWX.h"
#include "mafString.h"

#include "vtkImageData.h"
#include "vtkPointData.h"
#include "vtkRenderWindow.h"
#include "vtkRenderer.h"

#include <cppunit/TestResult.h>
#include <cppunit/extensions/HelperMacros.h>

class mafString;
class vtkImageData;
class vtkPointData;

//----------------------------------------------------------------------------
// Class Name: TestApp
// Helper class used to build a fake application needed by the wxConfig
// to store settings into the registry with the same name of the application
//----------------------------------------------------------------------------
class TestApp : public wxApp
{
public:
	bool OnInit();
	int  OnExit();
	bool Yield(bool onlyIfNeeded = false);
};

DECLARE_APP(TestApp)

#define COMPARE_IMAGES(imageName, ...) CompareImage(getTestNamer__().getFixtureName().c_str(), imageName, ##__VA_ARGS__)
#define GET_TEST_DATA_DIR() GetTestDataDir(getTestNamer__().getFixtureName().c_str())

//----------------------------------------------------------------------------
// Class Name: appTest
//----------------------------------------------------------------------------
class appTest : public CPPUNIT_NS::TestFixture
{
public: 
	appTest();
  
	/** CPPUNIT fixture: executed before each test, prepares an application for test runs
			DO NOT OVERLOAD THIS METHOD, use BeforeTest instead */
  void setUp();

	/** CPPUNIT fixture: executed after each test, clean an application for test runs
			DO NOT OVERLOAD THIS METHOD, use AfterTest instead */
  void tearDown();

	/** Test specific stuff executed before each test */
	virtual void BeforeTest() {};

	/** Test specific stuff executed after each test */
	virtual void AfterTest() {};

protected:
	void CompareImage(mafString suiteName, mafString imageName, int index = -1);
	
	mafString GetTestDataDir(mafString suiteName);

	TestApp *m_App;
	vtkRenderer *m_Renderer;
	vtkRenderWindow *m_RenderWindow;

	mafString m_WorkingDir;
};

#endif
