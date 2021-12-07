/*=========================================================================
Program:   Albedo
Module:    appOpCreateEmptyVME.cpp
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

#include "appOpCreateEmptyVME.h"

#include "appEmptyVME.h"

//----------------------------------------------------------------------------
albaCxxTypeMacro(appOpCreateEmptyVME);

//----------------------------------------------------------------------------
appOpCreateEmptyVME::appOpCreateEmptyVME(const wxString &label) :
albaOp(label)
{
  m_OpType	= OPTYPE_OP;
  m_Canundo = true;

  m_EmptyVME = NULL;
}
//----------------------------------------------------------------------------
appOpCreateEmptyVME::~appOpCreateEmptyVME( ) 
{
  albaDEL(m_EmptyVME);
}
//----------------------------------------------------------------------------
albaOp* appOpCreateEmptyVME::Copy()   
{
	return new appOpCreateEmptyVME(m_Label);
}
//----------------------------------------------------------------------------
bool appOpCreateEmptyVME::InternalAccept(albaVME*node)
{
	return (node && node->IsALBAType(albaVME));
}
//----------------------------------------------------------------------------
void appOpCreateEmptyVME::OpRun()   
{
  albaNEW(m_EmptyVME);

	m_EmptyVME->SetName("Empty VME");

	OpStop(OP_RUN_OK);
}
//----------------------------------------------------------------------------
void appOpCreateEmptyVME::OpDo()
{
  m_EmptyVME->ReparentTo(m_Input);
}
//----------------------------------------------------------------------------
void appOpCreateEmptyVME::OpStop(int result)
{
	albaEventMacro(albaEvent(this, result));
}
