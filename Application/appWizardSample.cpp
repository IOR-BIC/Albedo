/*=========================================================================
Program:   Albedo
Module:    appWizardSample.cpp
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

#include "appWizardSample.h"

#include "albaWizardBlockInformation.h"
#include "albaWizardBlockOperation.h"
#include "albaWizardBlockSelection.h"

//--------------------------------------------------------------------------------
appWizardSample::appWizardSample( const wxString &label ):albaWizard(label)
{
  albaWizardBlockInformation *infoBlock=new albaWizardBlockInformation("START");
  infoBlock->SetBoxLabel("Info");
  infoBlock->SetDescription("Import Loop:\nYou can import and show a series of VTK files.");
  infoBlock->SetWindowTitle("Wizard Sample");
  infoBlock->SetNextBlock("Import");
  infoBlock->SetBlockProgress(10);
  
  //Start Block (import a VTK file)   
  albaWizardBlockOperation *opBlock=new albaWizardBlockOperation("Import");
  opBlock->SetRequiredView("");
  opBlock->VmeSelect("root");
  opBlock->SetRequiredOperation("albaOpImporterVTK");
  opBlock->SetNextBlock("opPause");
  opBlock->SetBlockProgress(25);

  //Move block (position the Block)
  albaWizardBlockOperation *opBlock2=new albaWizardBlockOperation("opPause");
  opBlock2->SetRequiredView("VTK view");
  opBlock2->VmeSelect("lastChild");
  opBlock2->SetRequiredOperation("PAUSE");
  opBlock2->SetNextBlock("Select");
  opBlock2->SetBlockProgress(75);
  
  //Select Block ask user if want to import another file
  albaWizardBlockSelection *selectorBlock=new albaWizardBlockSelection("Select");
  selectorBlock->SetWindowTitle("Test Selector");
  selectorBlock->SetDescription("Wizard Selector Test window");
  selectorBlock->AddChoice("Import another VTK","deleteBlock"); 
  selectorBlock->AddChoice("End Wizard", "END");
  selectorBlock->SetBlockProgress(100);

  //Move block (position the Block)
  albaWizardBlockOperation *deleteBlock=new albaWizardBlockOperation("deleteBlock");
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
bool appWizardSample::Accept(albaVME* vme)
{
  return true;
}

