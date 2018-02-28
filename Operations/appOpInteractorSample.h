/*=========================================================================
Program:   Albedo
Module:    appOpInteractorSample.cpp
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

#ifndef __appOpInteractorSample_H__
#define __appOpInteractorSample_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafOp.h"
#include <string>
#include <vector>

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafEvent;
class mafVMEImage;
class mafVMEVolumeRGB;
class appInteractor2DSample;

#ifdef MAF_EXPORTS
#include "mafDllMacros.h"
EXPORT_STL_VECTOR(MAF_EXPORT,std::string);
#endif

/**
Class Name: appOpInteractorSample
*/
class MAF_EXPORT appOpInteractorSample: public mafOp
{
public:
  appOpInteractorSample(const wxString &label = "Interactor Sample");
 ~appOpInteractorSample(); 

	//virtual void OnEvent(mafEventBase *maf_event);
  
  mafTypeMacro(appOpInteractorSample, mafOp);

  mafOp* Copy();

	/** Return true for the acceptable vme type. */
  bool Accept(mafVME *node);

	/** Builds operation's interface. */
  void OpRun();

  /** Precess events coming from other objects */
  void OnEvent(mafEventBase *maf_event);

  /** Set the filename for the .stl to import */
  void SetFileName(const char *file_name);  
  
  void ImportImage();
protected:
  /** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
  void OpStop(int result);

  /** Create the GUI */
  virtual void CreateGui();
  
  void BuildImage();

  double m_Measure;
  int m_MeasureUnit;

	appInteractor2DSample *m_DistanceInteractor2D;

  mafVMEImage  *m_ImportedImage;
  std::string		m_FileName;
};
#endif
