/*=========================================================================
Program:   Albedo
Module:    appInteractor2DMeasure.h
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

#ifndef __appInteractor2DMeasure_h
#define __appInteractor2DMeasure_h

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "appInteractionDefines.h"

#include "albaEvent.h"
#include "albaInteractorPER.h"

//----------------------------------------------------------------------------
// Forward references :
//----------------------------------------------------------------------------
class albaDeviceButtonsPadMouse;
class albaRWI;

class vtkALBATextActorMeter;
class vtkActor2D;
class vtkCoordinate;
class vtkLineSource;
class vtkPointSource;
class vtkPolyDataMapper2D;
class vtkRenderWindow;
class vtkRenderer;
class vtkRendererCollection;
class vtkTextActor;
class vtkXYPlotActor;

#define TEXT_H_SHIFT 10
#define MIN_UPDATE_DISTANCE 1.0

// Class Name: appInteractor2DMeasure
class APP_INTERACTION_EXPORT appInteractor2DMeasure : public albaInteractorPER
{
public:

	albaTypeMacro(appInteractor2DMeasure, albaInteractorPER);

	enum MEASURE_INTERACTIONS
	{
		ID_MEASURE_STARTED = MINID,
		ID_MEASURE_FINISHED,
		ID_MEASURE_ADDED,
		ID_MEASURE_CHANGED,
		ID_MEASURE_MOVED,
		ID_MEASURE_SELECTED,
		ID_MEASURE_RCLICK,
	};

	enum MEASURE_ACTIONS
	{
		ACTION_NONE = 0,
		ACTION_ADD_MEASURE,
		ACTION_EDIT_MEASURE,
		ACTION_MOVE_MEASURE,
	};

	virtual void OnEvent(albaEventBase *event);
	
	/// MEASURE
	/** Add Measure*/
	virtual void AddMeasure(double *point1, double *point2 = NULL) {};
	/** Edit Measure*/
	virtual void EditMeasure(int index, double *point1, double *point2 = NULL) {};
	/** Delete the Measure*/
	virtual void RemoveMeasure(int index) {};
	/** Delete all Measures*/
	virtual void RemoveAllMeasures();
	/** Select a Measure*/
	virtual void SelectMeasure(int index) {};
	/** Update All*/
	virtual void Update(int index = -1 /*Update All*/) {};

	/// LOAD/SAVE
	virtual bool Load(albaVME *input, wxString tag);
	virtual bool Save(albaVME *input, wxString tag);

	/// GET
	/** Get Measure Value*/
	albaString GetMeasure(int index);	
	/** Get Measure Extra Label*/
	albaString GetMeasureLabel(int index);
	/** Get a string type of measure*/
	albaString GetMeasureType() { return m_MeasureTypeText; };
	/** Get Number of Measures*/
	int GetMeasureCount() {	return m_MeasuresCount; };
	/** Returns the last Edited Measure index*/
	int GetLastEditedMeasureIndex() { return m_LastEditing; }
	/** Returns the Current Measure Selected index*/
	int GetSelectedMeasureIndex() { return m_LastSelection; }
	
	/// SET
	/** Set Measure Extra Label*/
	void SetMeasureLabel(int index, albaString text);
	/** Set Color Default*/
	void SetColor(double r, double g, double b);
	/** Set Color Selection*/
	void SetColorSelection(double r, double g, double b);
	/** Set Color For Disable Measure*/
	void SetColorDisable(double r, double g, double b);
	/** Set Measure Opacity*/
	void SetOpacity(double opacity);
	/** Set Side of the Text Label*/
	void SetTextSide(int side);
	/** Show/Hide Text Labels*/
	void ShowText(bool show);
	/** Set Renderer by View needed*/
	void SetRendererByView(albaView * view);

	/** Enable/Disable Editing Mode*/
	void EnableEditMeasure(bool edit = true) { m_EditMeasureEnable = edit; };

	void Enable();
	void Disable();
	bool IsEnabled() { return m_IsEnabled; };

	/* Call Rendering and Camera Update */
	void Render();

protected:

	appInteractor2DMeasure();
	virtual ~appInteractor2DMeasure();

	void InitRenderer(albaEventInteraction *e);

	/// Mouse Events
	virtual void OnLeftButtonDown(albaEventInteraction *e);
	virtual void OnLeftButtonUp(albaEventInteraction *e);
	virtual void OnRightButtonUp(albaEventInteraction *e);
	virtual void OnMove(albaEventInteraction *e);
		
	/// Draw Measure
	virtual void DrawMeasure(double * wp) {};
	virtual void MoveMeasure(int index, double * pointCoord) {};
	
	/// Update
	virtual void UpdateAllActors() {};
	virtual void UpdateEditActors(double * point1, double * point2 = NULL) {};
	virtual void UpdatePointActor(double * point) {};
	virtual void UpdateTextActor(int index, double * point) {};

	virtual void ShowEditActors() {};
	virtual void HideEditActors() {};
	virtual void DisableMeasure(int index) {};

	/** Set Measure Action (None, Add, Edit, Move) and Update mouse Cursor */
	void SetAction(MEASURE_ACTIONS action);

	/// Utils
	virtual void FindAndHighlightCurrentPoint(double * pointCoord) {};
	double DistanceBetweenPoints(double *point1, double *point2);
	float DistancePointToLine(double * point, double * lineP1, double * lineP2);
	bool IsInBound(double *pos);
	void ScreenToWorld(double screen[2], double world[3]);
	void WorldToScreen(double world[3], double screen[2]);

	// Measure Vector
	std::vector<albaString> m_MeasureVector;

	albaString m_MeasureLabel;
	albaString m_MeasureTypeText;

	albaDeviceButtonsPadMouse	*m_Mouse;
	vtkRenderer								*m_Renderer;
	albaView									*m_View;
	vtkCoordinate							*m_Coordinate;

	// Text Actor Vector
	std::vector<vtkALBATextActorMeter *> m_TextActorVector;
	
	// EDIT ACTORS

	// Text 
	vtkALBATextActorMeter	*m_EditTextActor;
	// Point
	vtkPointSource				*m_EditPointSource;
	vtkPolyDataMapper2D		*m_EditPointMapper;
	vtkActor2D						*m_EditPointActor;

	double m_Opacity;
	double m_ColorDefault[3];
	double m_ColorSelection[3];
	double m_ColorDisable[3];

	int m_TextSide;

	MEASURE_ACTIONS m_Action; // Measure Action

	bool m_IsEnabled;

	int m_CurrentMeasureIndex;
	int m_MeasuresCount;
	
	double m_OldLineP1[3];
	double m_OldLineP2[3];

	double *m_Bounds;
	bool m_IsInBound;
	bool m_ButtonDownInside;

	double m_StartMousePosition[3];
	
	bool m_EndMeasure;
	bool m_ParallelView;
	double m_ParallelScale_OnStart;

	long m_AddMeasurePhase_Counter;
	bool m_ActorAdded;

	double m_Distance;
	int m_LastSelection;
	int m_LastEditing;
	bool m_MovingMeasure;
	bool m_EditMeasureEnable;

	bool m_ShowText;
	bool m_ShowPoint;

private:
	appInteractor2DMeasure(const appInteractor2DMeasure&);   // Not implemented.
	void operator=(const appInteractor2DMeasure&);  // Not implemented.
	friend class appInteractor2DMeasureTest;
};
#endif

