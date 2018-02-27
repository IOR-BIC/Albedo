/*=========================================================================
Program:   Albedo
Module:    appOpCreateSurfaceParametric.cpp
Language:  C++
Date:      $Date: 2018-01-01 12:00:00 $
Version:   $Revision: 1.0.0.0 $
Authors:   Nicola Vanella
==========================================================================
Copyright (c) LTM-IOR 2018 (http://www.ltmsoftware.org/alba.htm)

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the above copyright notice for more information.
=========================================================================*/

#include "appDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the APP must include "appDefines.h" as first.
// This force to include Window, wxWidgets and VTK exactly in this order.
//----------------------------------------------------------------------------

#include "appOpCreateSurfaceParametric.h"

#include "appVMESurfaceParametric.h"

#include "mafDecl.h"
#include "mafEvent.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(appOpCreateSurfaceParametric);

//----------------------------------------------------------------------------
appOpCreateSurfaceParametric::appOpCreateSurfaceParametric(const wxString &label) :
mafOp(label)
{
  m_OpType	= OPTYPE_OP;
  m_Canundo = true;

  m_SurfaceParametric = NULL;
}
//----------------------------------------------------------------------------
appOpCreateSurfaceParametric::~appOpCreateSurfaceParametric( ) 
{
  mafDEL(m_SurfaceParametric);
}
//----------------------------------------------------------------------------
mafOp* appOpCreateSurfaceParametric::Copy()   
{
	return new appOpCreateSurfaceParametric(m_Label);
}
//----------------------------------------------------------------------------
bool appOpCreateSurfaceParametric::Accept(mafVME*node)
{
	return (node && node->IsMAFType(mafVME));
}
//----------------------------------------------------------------------------
void appOpCreateSurfaceParametric::OpRun()   
{
  mafNEW(m_SurfaceParametric);
  m_SurfaceParametric->SetName("Parametric Surface");
  m_Output = m_SurfaceParametric;

	OpStop(OP_RUN_OK);
}
//----------------------------------------------------------------------------
void appOpCreateSurfaceParametric::OpDo()
{
  m_SurfaceParametric->ReparentTo(m_Input);
}
//----------------------------------------------------------------------------
void appOpCreateSurfaceParametric::OpStop(int result)
{
	mafEventMacro(mafEvent(this, result));
}
