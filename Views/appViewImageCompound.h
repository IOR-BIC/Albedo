/*=========================================================================
Program:   Albedo
Module:    appViewImageCompound.h
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

#ifndef __appViewImageCompound_H__
#define __appViewImageCompound_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "appViewsDefines.h"
#include "mafViewImageCompound.h"
#include "vtkMAFSimpleRulerActor2D.h"
#include "vtkRenderWindow.h"

//----------------------------------------------------------------------------
// Forward references :
//----------------------------------------------------------------------------
class mafSceneGraph;
class mafGUI;
class mafRWIBase;
class mafViewImage;
class mafGUILutSlider;
class mafGUILutSwatch;
class mafGUIFloatSlider;
class mafGUIPicButton;

class vtkLookupTable;
class vtkWindowLevelLookupTable;
class vtkMAFSimpleRulerActor2D;

enum VIEW_OPTIONS
{
	VIEW_RULER = MINID,
};

/**
  Class Name: appViewImageCompound.
  Class that contain mafViewImage in order to add gui view widget like windowing buttons.
*/
class APP_VIEWS_EXPORT appViewImageCompound: public mafViewImageCompound
{
public:
  /** constructor. */
  appViewImageCompound(wxString label = "View Single Slice Compound", int num_row = 1, int num_col = 1);
  /** destructor. */
  virtual ~appViewImageCompound(); 

	/** RTTI macro.*/
	mafTypeMacro(appViewImageCompound, mafViewImageCompound);

  /**  Redefine this method to package the compounded view */
  virtual void PackageView();

  virtual mafView *Copy(mafObserver *Listener, bool lightCopyEnabled = false);

	bool RulerIsVisible() { return m_ShowRuler; };
  void ShowRuler(bool show);

	/**  Create the GUI on the bottom of the compounded view. */
	virtual void CreateGuiView();

	/** Precess events coming from other objects */
	void OnEvent(mafEventBase *maf_event);

protected:
  void SetRendererByView();

  vtkMAFSimpleRulerActor2D *m_Ruler;
	vtkRenderer *m_Renderer;

	mafGUIPicButton *m_RulerButton;

  bool m_ShowRuler;
};
#endif
