/*=========================================================================
Program:   Albedo
Module:    appInteractor2DSample.h
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

#ifndef __appInteractor2DSample_h
#define __appInteractor2DSample_h

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
class vtkConeSource;

// Class Name: appInteractor2DSample
class APP_INTERACTION_EXPORT appInteractor2DSample : public albaInteractorPER
{
public:

	albaTypeMacro(appInteractor2DSample, albaInteractorPER);

	enum LINE_INTERACTIONS
	{
		ID_RESULT_LINE = MINID,
		ID_LINE_ADDED,
		ID_LINE_CHANGED,
		ID_LINE_MOVED,
		ID_LINE_SELECTED,
	};

	enum LINE_ACTIONS
	{
		ID_NO_ACTION = 0,
		ID_ADD_LINE,
		ID_EDIT_LINE,
		ID_MOVE_LINE,
	};

	virtual void OnEvent(albaEventBase *event);

	// Measures
	/** Add Measure*/
	void AddMeasure(double point1[3], double point2[3]);
	/** Edit Measure*/
	void EditMeasure(int index, double point1[3], double point2[3]);
	/** Delete the Measure*/
	void RemoveMeasure(int index);
	/** Delete all Measures*/
	void RemoveAllMeasures();
	/** Select a Measure*/
	void SelectMeasure(int index);

	/** Get Measure Value*/
	double GetMeasure(int index);
	/** Get Number of Measures*/
	int GetMeasureCount() { return m_MeasureVector.size(); };
	/** Returns the last Edited Measure index*/
	int GetLastEditedMeasureIndex() { return m_LastEditing; }
	/** Returns the Current Measure Selected index*/
	int GetSelectedMeasureIndex() { return m_LastSelection; }
	/** Get measure line extremity points*/
	void GetMeasureLinePoints(int index, double *point1, double *point2);

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
	/** Show/Hide Arrow Indicator*/
	void ShowArrow(bool show);

protected:

	appInteractor2DSample();
	virtual ~appInteractor2DSample();

	// Mouse Events
	virtual void OnLeftButtonDown(albaEventInteraction *e);
	virtual void OnLeftButtonUp(albaEventInteraction *e);
	virtual void OnMove(albaEventInteraction *e);

	void InitRenderer(albaEventInteraction *e);

	// Draw the Line
	void DrawMeasure(double * wp);
	void MoveMeasure(int index, double * pointCoord);

	void ScreenToWorld(double screen[2], double world[3]);

	void SetAction(int action);

	void UpdateLineActor(int index, double * point1, double * point2);
	void UpdateTextActor(int index, double * point1, double * point2);
	void UpdateConeActor(int index, double * point1, double * point2);
	void UpdatePointActor(double * point);
	
	void CalculateDistance();

	void FindAndHighlightCurrentPoint(double * pointCoord);

	float DistancePointToLine(double * point, double * lineP1, double * lineP2);

	bool IsInBound(double *pos);

	// Persistent line BEGIN
	std::vector<vtkLineSource *>				m_LineSourceVector;
	std::vector<vtkPolyDataMapper2D *>	m_LineMapperVector;
	std::vector<vtkActor2D *>						m_LineActorVector;

	std::vector<vtkALBATextActorMeter *> m_TextActorVector;

	std::vector<vtkConeSource *>				m_ConeSourceVector;
	std::vector<vtkPolyDataMapper2D *>	m_ConeMapperVector;
	std::vector<vtkActor2D *>						m_ConeActorVector;

	albaDeviceButtonsPadMouse	*m_Mouse;
	vtkRenderer								*m_Renderer;
	albaView									*m_View;

	std::vector<double> m_MeasureVector;

	vtkCoordinate					*m_Coordinate;

	// Edit Actors
	vtkLineSource					*m_EditLineSource;
	vtkPolyDataMapper2D		*m_EditLineMapper;
	vtkActor2D						*m_EditLineActor;

	vtkALBATextActorMeter	*m_EditTextActor;

	vtkConeSource					*m_EditConeSource;
	vtkPolyDataMapper2D		*m_EditConeMapper;
	vtkActor2D						*m_EditConeActor;

	vtkPointSource				*m_EditPointSource;
	vtkPolyDataMapper2D		*m_EditPointMapper;
	vtkActor2D						*m_EditPointActor;


	double m_Opacity;
	double m_ColorDefault[3];
	double m_ColorSelection[3];
	double m_ColorDisable[3];

	int m_TextSide;

	int m_LineAction;

	int m_CurrentLineIndex;
	int m_CurrentPointIndex;
	int m_CurrentAddedLine;
	int m_TotalPoints;

	double *m_Bounds;
	bool m_IsInBound;
	bool m_ButtonDownInside;

	double m_StartMousePosition[3];

	double m_OldLineP1[3];
	double m_OldLineP2[3];

	bool m_EndMeasure;
	bool m_ParallelView;
	bool m_RegisterMeasure;

	long m_AddMeasurePhase_Counter;
	bool m_ActorAdded;

	double m_Distance;
	int m_LastSelection;
	int m_LastEditing;
	bool m_CanEditLine;
	bool m_EditMeasureEnable;
	bool m_ShowText;
	bool m_ShowArrow;

private:
	appInteractor2DSample(const appInteractor2DSample&);   // Not implemented.
	void operator=(const appInteractor2DSample&);  // Not implemented.
	friend class appInteractor2DSampleTest;
};
#endif

