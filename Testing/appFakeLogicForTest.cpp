/*=========================================================================
Program:   Albedo
Module:    appFakeLogicForTest.cpp
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

#include "appFakeLogicForTest.h"

#include "albaVME.h"

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
void appFakeLogicForTest::VmeSelect(albaVME *vme)
{
	AddCall(appFakeLogicForTest::FKL_VME_SELECT, vme);
}
//-------------------------------------------------------------------------
void appFakeLogicForTest::VmeSelected(albaVME *vme){}
//-------------------------------------------------------------------------
void appFakeLogicForTest::VmeShow(albaVME *vme, bool visibility)
{
	AddCall(appFakeLogicForTest::FKL_VME_SHOW, vme);
}
//-------------------------------------------------------------------------
void appFakeLogicForTest::VmeAdd(albaVME *vme)
{
	AddCall(appFakeLogicForTest::FKL_VME_ADD, vme);
}
//-------------------------------------------------------------------------
void appFakeLogicForTest::VmeRemove(albaVME *vme)
{
	AddCall(appFakeLogicForTest::FKL_VME_REMOVE, vme);
	vme->ReparentTo(NULL);
}
//-------------------------------------------------------------------------
void appFakeLogicForTest::VmeModified(albaVME* vme)
{
	AddCall(appFakeLogicForTest::FKL_VME_MODIFIED, vme);
}

//-------------------------------------------------------------------------
void appFakeLogicForTest::VmeVisualModeChanged(albaVME * vme)
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
void appFakeLogicForTest::AddCall(testFunctions func, albaVME* vme)
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
void appFakeLogicForTest::PrintImage(albaVMEImage *img)
{
	AddCall(appFakeLogicForTest::FKL_PRINT_IMAGE, (albaVME*)img);
}

//----------------------------------------------------------------------------
albaOpManager * appFakeLogicForTest::GetOpManager() const
{
	return NULL;
}
