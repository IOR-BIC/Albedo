/*=========================================================================
Program:   Albedo
Module:    appOpInteractorSliderSample.cpp
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

#ifndef __appOpInteractorSliderSample_H__
#define __appOpInteractorSliderSample_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "appOperationsDefines.h"

#include "albaOp.h"
#include "albaVMEVolumeGray.h"

//----------------------------------------------------------------------------
// Forward references :
//----------------------------------------------------------------------------
class albaEvent;
class albaInteractorSlider;
class albaView;
class vtkRectilinearGrid;
class vtkImageData;

// Class Name: appOpInteractorSliderSample
class APP_OPERATIONS_EXPORT appOpInteractorSliderSample: public albaOp
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

	//virtual void OnEvent(albaEventBase *alba_event);
  
  albaTypeMacro(appOpInteractorSliderSample, albaOp);

  albaOp* Copy();

	/** Builds operation's interface. */
  void OpRun();

  /** Precess events coming from other objects */
  void OnEvent(albaEventBase *alba_event);

protected:

	/** Return true for the acceptable vme type. */
	/*virtual*/ bool InternalAccept(albaVME *node);

  /** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
  void OpStop(int result);

  /** Create the GUI */
  virtual void CreateGui();

	albaInteractorSlider *m_InteractorSlider;
	albaView *m_View;

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

	void GetSliceOrigin(double *origin);
	void UpdateVolumeSlice();

	albaVMEVolumeGray* m_Volume;	//<Input volume
	int m_SlicePlane;						//<Current slicing plane (xy,xz,yx)

	enum PLANE_TYPE
	{
		YZ = 0,
		XZ,
		XY,
	};
};
#endif
