/*=========================================================================
Program:   Albedo
Module:    appOpEmptyTest.h
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

#ifndef __CPP_UNIT_appOpEmptyTest_H__
#define __CPP_UNIT_appOpEmptyTest_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "../appTest.h"


//----------------------------------------------------------------------------
// Class Name: appOpEmptyTest
//----------------------------------------------------------------------------
class appOpEmptyTest : public appTest
{
  public:

    CPPUNIT_TEST_SUITE( appOpEmptyTest );	
		CPPUNIT_TEST(TestFixture); // Just to test that the fixture has no leaks
		CPPUNIT_TEST(TestStaticAllocation);
		CPPUNIT_TEST(TestDynamicAllocation);
		CPPUNIT_TEST(TestAccept);
		CPPUNIT_TEST(TestOpRun);
		CPPUNIT_TEST(TestOpUndo);
    CPPUNIT_TEST_SUITE_END();

private:

		void TestFixture();
		void TestStaticAllocation();
		void TestDynamicAllocation();
		void TestAccept();
		void TestOpRun();
		void TestOpUndo();
};
#endif
