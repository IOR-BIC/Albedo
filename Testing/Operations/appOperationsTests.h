/*=========================================================================
Program:   Albedo
Module:    appOperationsTests.h
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

#ifndef __CPP_UNIT_appOperationsTests_H__
#define __CPP_UNIT_appOperationsTests_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------

#include "albaDefines.h"
#include "albaObserver.h"
#include "albaVME.h"

//----------------------------------------------------------------------------
// Class Name: DummyVme
//----------------------------------------------------------------------------
class DummyVme : public albaVME
{
public:

	DummyVme() {};
	~DummyVme() {};

	albaTypeMacro(DummyVme, albaVME);

	/*virtual*/ void SetMatrix(const albaMatrix &mat) {};
	/*virtual*/ void GetLocalTimeStamps(std::vector<albaTimeStamp> &kframes) {};
	/*virtual*/ albaVMEOutput *GetOutput();

protected:
private:
};

//----------------------------------------------------------------------------
// Class Name: DummyObserver
//----------------------------------------------------------------------------
class DummyObserver : public albaObserver
{
public:

	DummyObserver() { m_DummyVme = new DummyVme(); };
	~DummyObserver() { delete m_DummyVme; };

	/*virtual*/ void OnEvent(albaEventBase *alba_event);

protected:

	DummyVme *m_DummyVme;
};

#endif
