/*=========================================================================
Program:   Albedo
Module:    appOperationsTests.h
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

#ifndef __CPP_UNIT_appOperationsTests_H__
#define __CPP_UNIT_appOperationsTests_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------

#include "mafDefines.h"
#include "mafObserver.h"
#include "mafVME.h"

//----------------------------------------------------------------------------
// Class Name: DummyVme
//----------------------------------------------------------------------------
class DummyVme : public mafVME
{
public:

	DummyVme() {};
	~DummyVme() {};

	mafTypeMacro(DummyVme, mafVME);

	/*virtual*/ void SetMatrix(const mafMatrix &mat) {};
	/*virtual*/ void GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes) {};
	/*virtual*/ mafVMEOutput *GetOutput();

protected:
private:
};

//----------------------------------------------------------------------------
// Class Name: DummyObserver
//----------------------------------------------------------------------------
class DummyObserver : public mafObserver
{
public:

	DummyObserver() { m_DummyVme = new DummyVme(); };
	~DummyObserver() { delete m_DummyVme; };

	/*virtual*/ void OnEvent(mafEventBase *maf_event);

protected:

	DummyVme *m_DummyVme;
};

#endif
