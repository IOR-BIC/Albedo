/*=========================================================================
Program:   Albedo
Module:    appWizardSample.cpp
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

#include "appWizardSample.h"

#include "mafWizardBlockInformation.h"
#include "mafWizardBlockOperation.h"
#include "mafWizardBlockSelection.h"

//--------------------------------------------------------------------------------
appWizardSample::appWizardSample( const wxString &label ):mafWizard(label)
{
  mafWizardBlockInformation *infoBlock=new mafWizardBlockInformation("START");
  infoBlock->SetBoxLabel("Info");
  infoBlock->SetDescription("Import Loop:\nYou can import and show a series of VTK files.");
  infoBlock->SetWindowTitle("Wizard Sample");
  infoBlock->SetNextBlock("Import");
  infoBlock->SetBlockProgress(10);
  
  //Start Block (import a VTK file)   
  mafWizardBlockOperation *opBlock=new mafWizardBlockOperation("Import");
  opBlock->SetRequiredView("");
  opBlock->VmeSelect("root");
  opBlock->SetRequiredOperation("mafOpImporterVTK");
  opBlock->SetNextBlock("opPause");
  opBlock->SetBlockProgress(25);

  //Move block (position the Block)
  mafWizardBlockOperation *opBlock2=new mafWizardBlockOperation("opPause");
  opBlock2->SetRequiredView("VTK view");
  opBlock2->VmeSelect("lastChild");
  opBlock2->SetRequiredOperation("PAUSE");
  opBlock2->SetNextBlock("Select");
  opBlock2->SetBlockProgress(75);
  
  //Select Block ask user if want to import another file
  mafWizardBlockSelection *selectorBlock=new mafWizardBlockSelection("Select");
  selectorBlock->SetWindowTitle("Test Selector");
  selectorBlock->SetDescription("Wizard Selector Test window");
  selectorBlock->AddChoice("Import another VTK","deleteBlock"); 
  selectorBlock->AddChoice("End Wizard", "END");
  selectorBlock->SetBlockProgress(100);

  //Move block (position the Block)
  mafWizardBlockOperation *deleteBlock=new mafWizardBlockOperation("deleteBlock");
  deleteBlock->SetRequiredOperation("DELETE");
  deleteBlock->SetNextBlock("Import");
	
  //Add blocks to the wizard
  AddBlock(infoBlock);
  AddBlock(selectorBlock);
  AddBlock(opBlock);
  AddBlock(opBlock2);
  AddBlock(deleteBlock);

  ShowProgressBar();
}

//--------------------------------------------------------------------------------
bool appWizardSample::Accept(mafVME* vme)
{
  return true;
}

