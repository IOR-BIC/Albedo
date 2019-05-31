/*=========================================================================
Program:   Albedo
Module:    appOpInteractorSliderSample.cpp
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

#ifndef __appOpInteractorSliderSample_H__
#define __appOpInteractorSliderSample_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "appOperationsDefines.h"

#include "mafOp.h"

//----------------------------------------------------------------------------
// Forward references :
//----------------------------------------------------------------------------
class mafEvent;
class mafInteractorSlider;

// Class Name: appOpInteractorSliderSample
class APP_OPERATIONS_EXPORT appOpInteractorSliderSample: public mafOp
{
public:

	//Widgets ID's	
	enum OP_INTERACTION_SAMPLE_ID
	{
		ID_NONE = MINID,
		ID_SLIDER_SHOW,
		ID_SLIDER_LABELS_SHOW,
		ID_SLIDER_EDIT,
		ID_SLIDER_INIT,
	};

  appOpInteractorSliderSample(const wxString &label = "Interactor Slider Sample");
 ~appOpInteractorSliderSample(); 

	//virtual void OnEvent(mafEventBase *maf_event);
  
  mafTypeMacro(appOpInteractorSliderSample, mafOp);

  mafOp* Copy();

	/** Return true for the acceptable vme type. */
  bool Accept(mafVME *node);

	/** Builds operation's interface. */
  void OpRun();

  /** Precess events coming from other objects */
  void OnEvent(mafEventBase *maf_event);

protected:

  /** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
  void OpStop(int result);

  /** Create the GUI */
  virtual void CreateGui();

	mafInteractorSlider *m_InteractorSlider;
	mafView *m_View;

	double m_SliderValue;
	double m_SliderMin;
	double m_SliderMax;
	int m_SliderSteps;

	int m_SliderPosX;
	int m_SliderPosY;
	int m_SliderLenght;
	int m_SliderOrientation;

	int m_SliderShow;
	int m_SliderLabelsShow;
};
#endif
