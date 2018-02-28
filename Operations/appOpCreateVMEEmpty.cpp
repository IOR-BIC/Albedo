/*=========================================================================
Program:   Albedo
Module:    appOpCreateVMEEmpty.cpp
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

#include "appOpCreateVMEEmpty.h"

#include "appVMEEmpty.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(appOpCreateVMEEmpty);

//----------------------------------------------------------------------------
appOpCreateVMEEmpty::appOpCreateVMEEmpty(const wxString &label) :
mafOp(label)
{
  m_OpType	= OPTYPE_OP;
  m_Canundo = true;

  m_VMEEmpty = NULL;
}
//----------------------------------------------------------------------------
appOpCreateVMEEmpty::~appOpCreateVMEEmpty( ) 
{
  mafDEL(m_VMEEmpty);
}
//----------------------------------------------------------------------------
mafOp* appOpCreateVMEEmpty::Copy()   
{
	return new appOpCreateVMEEmpty(m_Label);
}
//----------------------------------------------------------------------------
bool appOpCreateVMEEmpty::Accept(mafVME*node)
{
	return (node && node->IsMAFType(mafVME));
}
//----------------------------------------------------------------------------
void appOpCreateVMEEmpty::OpRun()   
{
  mafNEW(m_VMEEmpty);

	m_VMEEmpty->SetName("Empty VME");

	OpStop(OP_RUN_OK);
}
//----------------------------------------------------------------------------
void appOpCreateVMEEmpty::OpDo()
{
  m_VMEEmpty->ReparentTo(m_Input);
}
//----------------------------------------------------------------------------
void appOpCreateVMEEmpty::OpStop(int result)
{
	mafEventMacro(mafEvent(this, result));
}
