/*=========================================================================
Program:   Albedo
Module:    appOpEmptyTest.cpp
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

#include "appOpEmpty.h"
#include "appOpEmptyTest.h"
#include "appOperationsTests.h"

#include <cppunit/config/SourcePrefix.h>

//----------------------------------------------------------------------------
void appOpEmptyTest::TestFixture()
{
}
//----------------------------------------------------------------------------
void appOpEmptyTest::TestStaticAllocation()
{
	appOpEmpty op;
}
//----------------------------------------------------------------------------
void appOpEmptyTest::TestDynamicAllocation()
{
	appOpEmpty *op = new appOpEmpty("MyOperation");
	cppDEL(op);
}
//----------------------------------------------------------------------------
void appOpEmptyTest::TestAccept()
{
	appOpEmpty *op = new appOpEmpty();

	bool result = true;
	//result = op->Accept(inputVME);
	CPPUNIT_ASSERT(result);

	cppDEL(op);
}
//----------------------------------------------------------------------------
void appOpEmptyTest::TestOpRun()
{
	appOpEmpty *op = new appOpEmpty();

	op->TestModeOn();
	op->OpRun();

	cppDEL(op);
}
//----------------------------------------------------------------------------
void appOpEmptyTest::TestOpUndo()
{
	appOpEmpty *op = new appOpEmpty();

	DummyObserver *observer = new DummyObserver();

	op->TestModeOn();
	op->SetListener(observer);
	op->OpRun();

	op->OpUndo();

	cppDEL(op);
	cppDEL(observer);
}

