/*=========================================================================
Program:   Albedo
Module:    appOpInteractorSample.cpp
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

#ifndef __appOpInteractorSample_H__
#define __appOpInteractorSample_H__

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
class appInteractor2DSample;

// Class Name: appOpInteractorSample
class APP_OPERATIONS_EXPORT appOpInteractorSample: public albaOp
{
public:

	enum INTERACTION_TYPE
	{
		LINE,
		INDICATOR,
	};

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

  appOpInteractorSample(const wxString &label = "Interactor Sample");
 ~appOpInteractorSample(); 
  
  albaTypeMacro(appOpInteractorSample, albaOp);

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

	/** Load Measures from Tags */
	void Load();
	/** Save Measures to Tags */
	void Save();

	int m_NumLines;
	int m_EditLine;
	int m_ShowText;
	int m_ShowArrow;

	int m_MeasureType;

	int m_SelectedMeasure;
	double m_Measure;
	wxString m_MeasureText;
	albaString m_MeasureTypeText;

	appInteractor2DSample *m_Interactor;

  albaVMEImage  *m_ImportedImage;
  std::string		m_FileName;
};
#endif
