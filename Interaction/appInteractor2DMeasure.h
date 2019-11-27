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

class vtkLineSource;
class vtkCoordinate;
class vtkPolyDataMapper2D;
class vtkActor2D;
class vtkRenderWindow;
class vtkRenderer;
class vtkXYPlotActor;
class vtkTextActor;
class vtkRendererCollection;
class vtkALBATextActorMeter;
class vtkPointSource;

#define TEXT_H_SHIFT     10

#define LINE_UPDATE_DISTANCE 1.0
#define POINT_UPDATE_DISTANCE 2.5
#define POINT_UPDATE_DISTANCE_2 (POINT_UPDATE_DISTANCE * POINT_UPDATE_DISTANCE)

// Class Name: appInteractor2DMeasure
class APP_INTERACTION_EXPORT appInteractor2DMeasure : public albaInteractorPER
{
public:

	albaTypeMacro(appInteractor2DMeasure, albaInteractorPER);

	enum MEASURE_INTERACTIONS
	{
		ID_RESULT_LINE = MINID,
		ID_MEASURE_ADDED,
		ID_MEASURE_CHANGED,
		ID_MEASURE_MOVED,
		ID_MEASURE_SELECTED,
	};

	enum MEASURE_ACTIONS
	{
		ID_NO_ACTION = 0,
		ID_ADD_MEASURE,
		ID_EDIT_MEASURE,
		ID_MOVE_MEASURE,
	};

	virtual void OnEvent(albaEventBase *event);

	albaString GetMeasureType() { return m_MeasureTypeText; };

	// Measures
	/** Add Measure*/
	virtual void AddMeasure(double point1[3], double point2[3]);
	/** Edit Measure*/
	virtual void EditMeasure(int index, double point1[3], double point2[3]);
	/** Delete the Measure*/
	virtual void RemoveMeasure(int index);
	/** Delete all Measures*/
	virtual void RemoveAllMeasures();
	/** Select a Measure*/
	virtual void SelectMeasure(int index);

	/** Get Measure Value*/
	albaString GetMeasure(int index);
	
	/** Get Number of Measures*/
	int GetMeasureCount() {	return m_MeasuresCount; };
	/** Returns the last Edited Measure index*/
	int GetLastEditedMeasureIndex() { return m_LastEditing; }
	/** Returns the Current Measure Selected index*/
	int GetSelectedMeasureIndex() { return m_LastSelection; }

	/*Set Renderer by View needed*/
	void SetRendererByView(albaView * view);

	/** Enable/Disable Editing Mode */
	void EnableEditMeasure(bool edit = true) { m_EditMeasureEnable = edit; };

	/** Set Color Default*/
	void SetColor(double r, double g, double b);
	/** Set Color Selection*/
	void SetColorSelection(double r, double g, double b);
	/** Set Color For Disable Measure*/
	void SetColorDisable(double r, double g, double b);
	/** Set Measure Opacity*/
	void SetOpacity(double opacity);
	/*Set Side of the Text Label*/
	void SetTextSide(int side);
	
	/** Show/Hide Text Labels*/
	void ShowText(bool show);

protected:

	appInteractor2DMeasure();
	virtual ~appInteractor2DMeasure();

	// Mouse Events
	virtual void OnLeftButtonDown(albaEventInteraction *e);
	virtual void OnLeftButtonUp(albaEventInteraction *e);
	virtual void OnMove(albaEventInteraction *e);

	void InitRenderer(albaEventInteraction *e);
	void ScreenToWorld(double screen[2], double world[3]);

	// Draw Measure
	virtual void DrawMeasure(double * wp);
	virtual void MoveMeasure(int index, double * pointCoord);
	
	void SetAction(int action);

	virtual void UpdateTextActor(int index, double * point);
	virtual void UpdatePointActor(double * point);

	virtual void DisableMeasure(int index);
	virtual void FindAndHighlightCurrentPoint(double * pointCoord);

	virtual void ShowEditActors();
	virtual void HideEditActors();

	virtual void UpdateEditActors(double * point1, double * point2);

	double DistanceBetweenPoints(double *point1, double *point2);
	float DistancePointToLine(double * point, double * lineP1, double * lineP2);

	bool IsInBound(double *pos);

	std::vector<albaString> m_MeasureVector;
	albaString m_MeasureTypeText;

	albaDeviceButtonsPadMouse	*m_Mouse;
	vtkRenderer								*m_Renderer;
	albaView									*m_View;

	vtkCoordinate					*m_Coordinate;

		// Text Vector
	std::vector<vtkALBATextActorMeter *> m_TextActorVector;
	
	// EDIT ATORS
	vtkALBATextActorMeter	*m_EditTextActor;

	vtkPointSource				*m_EditPointSource;
	vtkPolyDataMapper2D		*m_EditPointMapper;
	vtkActor2D						*m_EditPointActor;

	double m_Opacity;
	double m_ColorDefault[3];
	double m_ColorSelection[3];
	double m_ColorDisable[3];

	int m_TextSide;

	int m_Action; // Measure Action

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

