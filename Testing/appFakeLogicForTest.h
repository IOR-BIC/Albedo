/*=========================================================================
Program:   Albedo
Module:    appFakeLogicForTest.h
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

#ifndef _appFakeLogicForTest_H
#define _appFakeLogicForTest_H

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafAbsLogicManager.h"
#include <vector>

//----------------------------------------------------------------------------
// Class Name: appFakeLogicForTest
//----------------------------------------------------------------------------
class appFakeLogicForTest : public mafAbsLogicManager
{
public:

	enum testFunctions
	{
		FKL_VME_SELECT,
		FKL_VME_ADD,
		FKL_VME_REMOVE,
		FKL_VME_MODIFIED,
		FKL_VME_SHOW,
		FKL_VME_VISUAL_MODE_CHANGED,
		FKL_VME_REMOVED,
		FKL_PRINT_IMAGE
	};

	struct calls
	{
		testFunctions testFunction;
		mafVME *vme;
	};

	appFakeLogicForTest();
	~appFakeLogicForTest();

	// VME
	virtual void VmeSelect(mafVME *vme);

	virtual void VmeSelected(mafVME *vme);

	virtual void VmeShow(mafVME *vme, bool visibility);

	virtual void VmeAdd(mafVME *vme);

	virtual void VmeRemove(mafVME *vme);

	virtual void VmeModified(mafVME* vme);

	virtual void VmeVisualModeChanged(mafVME * vme);

	// Camera
	virtual void CameraUpdate();

	virtual void CameraReset();

	void ClearCalls();

	calls GetCall(int callNum);

	void AddCall(testFunctions func, mafVME* vme);

	virtual bool IsOperationRunning();
	
	virtual const char* GetMsfFileExtension() override;
	
	virtual void VmeRemoved() override;
	
	virtual void PrintImage(mafVMEImage *img) override;

protected:

	std::vector<calls> m_Calls;
};
#endif