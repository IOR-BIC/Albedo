/*=========================================================================
Program:   Albedo
Module:    appFakeLogicForTest.cpp
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

#include "appFakeLogicForTest.h"

#include "mafVME.h"

//-------------------------------------------------------------------------
appFakeLogicForTest::appFakeLogicForTest()
{ }

//-------------------------------------------------------------------------
appFakeLogicForTest::~appFakeLogicForTest()
{
	ClearCalls();
}

// VME ////////////////////////////////////////////////////////////////////
//-------------------------------------------------------------------------
void appFakeLogicForTest::VmeSelect(mafVME *vme)
{
	AddCall(appFakeLogicForTest::FKL_VME_SELECT, vme);
}
//-------------------------------------------------------------------------
void appFakeLogicForTest::VmeSelected(mafVME *vme){}
//-------------------------------------------------------------------------
void appFakeLogicForTest::VmeShow(mafVME *vme, bool visibility)
{
	AddCall(appFakeLogicForTest::FKL_VME_SHOW, vme);
}
//-------------------------------------------------------------------------
void appFakeLogicForTest::VmeAdd(mafVME *vme)
{
	AddCall(appFakeLogicForTest::FKL_VME_ADD, vme);
}
//-------------------------------------------------------------------------
void appFakeLogicForTest::VmeRemove(mafVME *vme)
{
	AddCall(appFakeLogicForTest::FKL_VME_REMOVE, vme);
	vme->ReparentTo(NULL);
}
//-------------------------------------------------------------------------
void appFakeLogicForTest::VmeModified(mafVME* vme)
{
	AddCall(appFakeLogicForTest::FKL_VME_MODIFIED, vme);
}

//-------------------------------------------------------------------------
void appFakeLogicForTest::VmeVisualModeChanged(mafVME * vme)
{
	AddCall(appFakeLogicForTest::FKL_VME_VISUAL_MODE_CHANGED, vme);
}

// Camera /////////////////////////////////////////////////////////////////
//-------------------------------------------------------------------------
void appFakeLogicForTest::CameraUpdate(){}
//-------------------------------------------------------------------------
void appFakeLogicForTest::CameraReset(){}

// UTILS /////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------
void appFakeLogicForTest::ClearCalls()
{
	m_Calls.clear();
}

//-------------------------------------------------------------------------
appFakeLogicForTest::calls appFakeLogicForTest::GetCall(int callNum)
{
	return m_Calls[callNum];
}

//-------------------------------------------------------------------------
void appFakeLogicForTest::AddCall(testFunctions func, mafVME* vme)
{
	calls call;
	call.testFunction = func;
	call.vme = vme;

	m_Calls.push_back(call);
}

//----------------------------------------------------------------------------
bool appFakeLogicForTest::IsOperationRunning() { return false; }

//----------------------------------------------------------------------------
const char* appFakeLogicForTest::GetMsfFileExtension()
{
	return "alb";
}

//----------------------------------------------------------------------------
void appFakeLogicForTest::VmeRemoved()
{
	AddCall(appFakeLogicForTest::FKL_VME_REMOVED, NULL);
}

//----------------------------------------------------------------------------
void appFakeLogicForTest::PrintImage(mafVMEImage *img)
{
	AddCall(appFakeLogicForTest::FKL_PRINT_IMAGE, (mafVME*)img);
}
