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
class vtkAPPTextActorMeter;
class vtkAPPPointCloudSource;

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

	void AddMeasureLine(double point1[3], double point2[3]);

	void RemoveLine(int index);
	void RemoveAllLines();

	void SelectLine(int index);

	int SizeLineVector() { return m_DistanceVector.size(); }

	/** Get line extremity points*/
	void GetLinePoints(int index, double *point1, double *point2);
	double GetLine(int index) { return m_DistanceVector[index]; }

	int GetLastEditedLine() { return m_LastEditing; }
	int GetLineSelection() { return m_LastSelection; }

	void SetRendererByView(albaView * view);

	void CanEditLines(bool edit = true) { m_EditLinesEnable = edit; };

	void SetSide(int side);
	void SetColor(double r, double g, double b);
	void SetColorSelection(double r, double g, double b);
	void SetOpacity(double opacity);
	void SetAction(int action);

	void SetBounds(double *bounds) { m_Bounds = bounds; };

protected:

	appInteractor2DSample();
	virtual ~appInteractor2DSample();

	virtual void OnLeftButtonDown(albaEventInteraction *e);
	virtual void OnLeftButtonUp(albaEventInteraction *e);
	virtual void OnMove(albaEventInteraction *e);

	void InitRenderer(albaEventInteraction *e);

	// Draw the Line
	void DrawMeasureTool(double * wp);

	void EditMeasureLine(int index, double point1[3], double point2[3]);

	void UpdateTextActor(double * point1, double * point2, int index);

	void MoveLine(int index, double * pointCoord);

	void CalculateLine();
	void ScreenToWorld(double screen[2], double world[3]);
	void FindAndHighlightCurrentPoint(double * pointCoord);

	float DistancePointToLine(double * point, double * lineP1, double * lineP2);

	bool IsInBound(double *pos);

	// Persistent line BEGIN
	std::vector<vtkActor2D *> m_LineActorVector;
	std::vector<vtkPolyDataMapper2D *> m_LineMapperVector;
	std::vector<vtkLineSource *> m_LineSourceVector;

	albaDeviceButtonsPadMouse	*m_Mouse;
	vtkRenderer								*m_Renderer;
	albaView										*m_View;

	double *m_Bounds;
	bool m_IsInBound;
	bool m_ButtonDownInside;

	vtkCoordinate       *m_Coordinate;
	vtkLineSource       *m_EditLine;
	vtkPolyDataMapper2D *m_LineMapper;
	vtkActor2D          *m_EditLineActor;

	double m_Opacity;
	double m_ColorLine[3];
	double m_ColorSelection[3];
	int m_Side;

	vtkAPPPointCloudSource	*m_CurrentPointSource;
	vtkPolyDataMapper2D			*m_CurrentPointMapper;
	vtkActor2D							*m_CurrentPointActor;

	int m_LineAction;

	int m_CurrentLineIndex;
	int m_CurrentPointIndex;
	int m_CurrentAddedLine;
	int m_TotalPoints;

	double m_StartMousePosition[3];

	double m_OldLineP1[3];
	double m_OldLineP2[3];

	bool m_EndMeasure;
	bool m_ParallelView;
	bool m_RegisterMeasure;

	long m_AddMeasurePhase_Counter;
	bool m_ActorAdded;

	std::vector<double> m_DistanceVector;

	double m_Distance;
	int m_LastSelection;
	int m_LastEditing;
	bool m_CanEditLine;
	bool m_EditLinesEnable;

private:
	appInteractor2DSample(const appInteractor2DSample&);   // Not implemented.
	void operator=(const appInteractor2DSample&);  // Not implemented.
	friend class appInteractor2DSampleTest;
};
#endif

