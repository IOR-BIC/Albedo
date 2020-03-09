/*=========================================================================
Program:   Albedo
Module:    appOpInteractorTest.cpp
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

#ifndef __appOpInteractorTest_H__
#define __appOpInteractorTest_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "appOperationsDefines.h"

#include "albaOp.h"
#include <string>
#include <vector>

//----------------------------------------------------------------------------
// Forward references :
//----------------------------------------------------------------------------
class albaEvent;
class albaVMEImage;
class albaVMEVolumeRGB;

class appInteractor2DMeasure;
//class appInteractor2DMeasure_Point;
//class appInteractor2DMeasure_Distance;
//class appInteractor2DMeasure_Indicator;
//class appInteractor2DMeasure_Angle;
class appInteractor2DMeasure_DistancePoint;

// Class Name: appOpInteractorTest
class APP_OPERATIONS_EXPORT appOpInteractorTest: public albaOp
{
public:

	//Widgets ID's	
	enum OP_INTERACTION_SAMPLE_ID
	{
		ID_EDIT_LINE = MINID,
		ID_ONE_LINE,
		ID_NUM_LINES,
		ID_SHOW_TEXT,
		ID_SHOW_ARROW,
		ID_LOAD_MEASURES,
		ID_SAVE_MEASURES,
		ID_MEASURE,
	};

  appOpInteractorTest(const wxString &label = "Interactor Sample");
 ~appOpInteractorTest(); 
  
  albaTypeMacro(appOpInteractorTest, albaOp);

  albaOp* Copy();

	/** Return true for the acceptable vme type. */
  bool Accept(albaVME *node);

	/** Builds operation's interface. */
  void OpRun();

  /** Precess events coming from other objects */
  void OnEvent(albaEventBase *alba_event);

protected:

  /** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
  void OpStop(int result);

  /** Create the GUI */
  virtual void CreateGui();

	/** Set the image filename to import */
	void SetFileName(const char *file_name) { m_FileName = file_name; };

	void ImportImage(); // Load Image for Testing

	/** Remove Measures */
	void ClearMeasures(bool clearAll = false);

	int m_SelectedMeasure;
	wxString m_MeasureText;
	albaString m_MeasureTypeText;

// 	appInteractor2DMeasure *m_Interactor;
// 	appInteractor2DMeasure_Point *m_Interactor;
// 	appInteractor2DMeasure_Distance *m_Interactor;
//	appInteractor2DMeasure_Indicator *m_Interactor;
//	appInteractor2DMeasure_Angle *m_Interactor;
	appInteractor2DMeasure_DistancePoint *m_Interactor;

  albaVMEImage  *m_ImportedImage;
  std::string		m_FileName;
};
#endif
