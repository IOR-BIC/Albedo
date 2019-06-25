/*=========================================================================
Program:   Albedo
Module:    appViewImageCompound.h
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

#ifndef __appViewImageCompound_H__
#define __appViewImageCompound_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "appViewsDefines.h"
#include "albaViewImageCompound.h"
#include "vtkALBASimpleRulerActor2D.h"
#include "vtkRenderWindow.h"

//----------------------------------------------------------------------------
// Forward references :
//----------------------------------------------------------------------------
class albaSceneGraph;
class albaGUI;
class albaRWIBase;
class albaViewImage;
class albaGUILutSlider;
class albaGUILutSwatch;
class albaGUIFloatSlider;
class albaGUIPicButton;

class vtkLookupTable;
class vtkWindowLevelLookupTable;
class vtkALBASimpleRulerActor2D;

enum VIEW_OPTIONS
{
	VIEW_RULER = MINID,
};

/**
  Class Name: appViewImageCompound.
  Class that contain albaViewImage in order to add gui view widget like windowing buttons.
*/
class APP_VIEWS_EXPORT appViewImageCompound: public albaViewImageCompound
{
public:
  /** constructor. */
  appViewImageCompound(wxString label = "View Single Slice Compound", int num_row = 1, int num_col = 1);
  /** destructor. */
  virtual ~appViewImageCompound(); 

	/** RTTI macro.*/
	albaTypeMacro(appViewImageCompound, albaViewImageCompound);

  /**  Redefine this method to package the compounded view */
  virtual void PackageView();

  virtual albaView *Copy(albaObserver *Listener, bool lightCopyEnabled = false);

	bool RulerIsVisible() { return m_ShowRuler; };
  void ShowRuler(bool show);

	/**  Create the GUI on the bottom of the compounded view. */
	virtual void CreateGuiView();

	/** Precess events coming from other objects */
	void OnEvent(albaEventBase *alba_event);

protected:
  void SetRendererByView();

  vtkALBASimpleRulerActor2D *m_Ruler;
	vtkRenderer *m_Renderer;

	albaGUIPicButton *m_RulerButton;

  bool m_ShowRuler;
};
#endif
