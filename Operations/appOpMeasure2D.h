/*=========================================================================
Program:   Albedo
Module:    appOpMeasure2D.cpp
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

#ifndef __appOpMeasure2D_H__
#define __appOpMeasure2D_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "appOperationsDefines.h"

#include "albaOp.h"
#include <string>
#include <vector>
#include "appInteractor2DMeasure.h"

//----------------------------------------------------------------------------
// Forward references :
//----------------------------------------------------------------------------
class albaEvent;
class albaVMEImage;
class albaVMEVolumeRGB;
class appInteractor2DMeasure_Point;
class appInteractor2DMeasure_Distance;
class appInteractor2DMeasure_Indicator;
class appInteractor2DMeasure_DistancePoint;
class appInteractor2DMeasure_Angle;

// Class Name: appOpMeasure2D
class APP_OPERATIONS_EXPORT appOpMeasure2D: public albaOp
{
public:

	enum INTERACTION_TYPE
	{
		POINT,
		DISTANCE,
		INDICATOR,
		DISTANCE_POINT,
		XP,
		ANGLE,
	};

	//Widgets ID's	
	enum OP_INTERACTION_SAMPLE_ID
	{
		ID_EDIT_LINE = MINID,
		ID_SELECT_INTERACTOR,
		ID_MEASURE_LIST,
		ID_SELECT_MEASURE,
		ID_REMOVE_MEASURE,
		ID_NUM_LINES,
		ID_SHOW_TEXT,
		ID_SHOW_ARROW,
		ID_LOAD_MEASURES,
		ID_SAVE_MEASURES,
		ID_MEASURE,
		ID_MEASURE_LAB,
	};

  appOpMeasure2D(const wxString &label = "Interactor Sample");
 ~appOpMeasure2D(); 
  
  albaTypeMacro(appOpMeasure2D, albaOp);

  albaOp* Copy();

	/** Builds operation's interface. */
  void OpRun();

	void InitInteractors();

  /** Precess events coming from other objects */
  void OnEvent(albaEventBase *alba_event);

	/** Return an xpm-icon that can be used to represent this operation */
	virtual char ** GetIcon();

protected:

	/** Return true for the acceptable vme type. */
	/*virtual*/ bool InternalAccept(albaVME *node);

  /** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
  void OpStop(int result);

  /** Create the GUI */
  virtual void CreateGui();

	/** Set the image filename to import */
	void SetFileName(const char *file_name) { m_FileName = file_name; };

	void ImportImage(); // Load Image for Testing

	/** Select a Measure Interactor */
	void SetMeasureInteractor(int index);

	/* Update Measure ListBox */
	void UpdateMeasureList();

	/** Remove Measures */
	void RemoveMeasure(bool clearAll = false);

	/** Load Measures from Tags */
	void Load();
	/** Save Measures to Tags */
	void Save();

	int m_SelectedMeasure;

	albaString m_Measure;
	albaString m_MeasureLabel;
	albaString m_MeasureType;

	wxListBox *m_MeasureListBox;

	std::vector<appInteractor2DMeasure *> m_InteractorVector;

	int m_CurrentInteractor;
	int m_SelectedInteractor;

  albaVMEImage  *m_ImportedImage;
  std::string		m_FileName;
};
#endif
