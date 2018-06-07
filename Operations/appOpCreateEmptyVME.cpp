/*=========================================================================
Program:   Albedo
Module:    appOpCreateEmptyVME.cpp
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

#include "appDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the APP must include "appDefines.h" as first.
// This force to include Window, wxWidgets and VTK exactly in this order.
//----------------------------------------------------------------------------

#include "appOpCreateEmptyVME.h"

#include "appEmptyVME.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(appOpCreateEmptyVME);

//----------------------------------------------------------------------------
appOpCreateEmptyVME::appOpCreateEmptyVME(const wxString &label) :
mafOp(label)
{
  m_OpType	= OPTYPE_OP;
  m_Canundo = true;

  m_EmptyVME = NULL;
}
//----------------------------------------------------------------------------
appOpCreateEmptyVME::~appOpCreateEmptyVME( ) 
{
  mafDEL(m_EmptyVME);
}
//----------------------------------------------------------------------------
mafOp* appOpCreateEmptyVME::Copy()   
{
	return new appOpCreateEmptyVME(m_Label);
}
//----------------------------------------------------------------------------
bool appOpCreateEmptyVME::Accept(mafVME*node)
{
	return (node && node->IsMAFType(mafVME));
}
//----------------------------------------------------------------------------
void appOpCreateEmptyVME::OpRun()   
{
  mafNEW(m_EmptyVME);

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
	mafEventMacro(mafEvent(this, result));
}
