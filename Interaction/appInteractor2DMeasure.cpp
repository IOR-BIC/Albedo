/*=========================================================================
Program:   Albedo
Module:    appInteractor2DMeasure.cpp
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

#include "appInteractor2DMeasure.h"

#include "albaDecl.h"
#include "albaDeviceButtonsPadMouse.h"
#include "albaDeviceButtonsPadTracker.h"
#include "albaEventInteraction.h"
#include "albaRWI.h"
#include "albaTagArray.h"
#include "albaVME.h"
#include "albaVMEOutput.h"
#include "albaView.h"

#include "vtkALBASmartPointer.h"
#include "vtkALBATextActorMeter.h"
#include "vtkActor2D.h"
#include "vtkAppendPolyData.h"
#include "vtkCamera.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkPointSource.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper2D.h"
#include "vtkProbeFilter.h"
#include "vtkProperty2D.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include "vtkRendererCollection.h"
#include "vtkTextProperty.h"
#include "vtkTransform.h"
#include "vtkTransformPolyDataFilter.h"

//------------------------------------------------------------------------------
albaCxxTypeMacro(appInteractor2DMeasure)

//----------------------------------------------------------------------------
appInteractor2DMeasure::appInteractor2DMeasure()
{
	m_Coordinate = vtkCoordinate::New();
	m_Coordinate->SetCoordinateSystemToWorld();

	m_Mouse = NULL;
	m_Renderer = NULL;
	m_View = NULL;

	m_ParallelView = false;				// Set on InitRenderer
	m_ParallelScale_OnStart = -1; // Set on InitRenderer

	m_Bounds = NULL;
	m_IsInBound = false;
	m_ButtonDownInside = false;

	m_TextSide = 1;
	m_Opacity = 1;
	m_CurrentMeasureIndex = -1;
	m_MeasuresCount = 0;

	m_DraggingLeft = false;
	m_EndMeasure = false;

	m_EditMeasureEnable = true; // Enable FindAndHighlight
	m_MovingMeasure = false;

	m_IsEnabled = true;

	m_ShowText = false;
	m_ShowPoint = true;

	m_MeasureVector.clear();
	m_MeasureLabelVector.clear();

	m_MeasureLabel = "";

	m_Action = ACTION_NONE;

	m_AddMeasurePhase_Counter = 0;

	m_LastSelection = -1;
	m_Distance = 0;

	SetColor(1.0, 0.0, 1.0); // Default
	SetColorSelection(0.0, 1.0, 0.0); // Selection
	SetColorDisable(1.0, 0.0, 0.0); // Disable

	// EDIT ACTORS
	// Text
	vtkNEW(m_EditTextActor);
	m_EditTextActor->SetColor(m_ColorSelection[0], m_ColorSelection[1], m_ColorSelection[2]);
	m_EditTextActor->SetOpacity(m_Opacity);

	// Point
	vtkNEW(m_EditPointSource);
	m_EditPointSource->SetNumberOfPoints(1);

	vtkNEW(m_EditPointMapper);	
	m_EditPointMapper->SetInput(m_EditPointSource->GetOutput());
	m_EditPointMapper->SetTransformCoordinate(m_Coordinate);

	vtkNEW(m_EditPointActor);
	m_EditPointActor->SetMapper(m_EditPointMapper);
	m_EditPointActor->GetProperty()->SetPointSize(10.0);
	m_EditPointActor->GetProperty()->SetColor(1, 1, 0);
}
//----------------------------------------------------------------------------
appInteractor2DMeasure::~appInteractor2DMeasure()
{
	SetAction(ACTION_NONE);

	vtkDEL(m_Coordinate);

	m_Renderer->RemoveActor2D(m_EditTextActor);
	m_Renderer->RemoveActor2D(m_EditPointActor);

	// Edit Actor
	vtkDEL(m_EditTextActor);
	vtkDEL(m_EditPointSource);
	vtkDEL(m_EditPointMapper);
	vtkDEL(m_EditPointActor);

	for (int i = 0; i < m_TextActorVector.size(); i++)
	{
		// Texts
		m_Renderer->RemoveActor2D(m_TextActorVector[i]);
		vtkDEL(m_TextActorVector[i]);
	}

	m_Renderer->GetRenderWindow()->Render();

	m_TextActorVector.clear();
	m_MeasureVector.clear();
	m_MeasureLabelVector.clear();
}

//----------------------------------------------------------------------------
void appInteractor2DMeasure::InitRenderer(albaEventInteraction *e)
{
	albaEventMacro(albaEvent(this, CAMERA_UPDATE));

	if (m_Renderer == NULL)
	{
		if (m_Mouse == NULL)
		{
			albaDevice *device = albaDevice::SafeDownCast((albaDevice*)e->GetSender());
			albaDeviceButtonsPadMouse  *mouse = albaDeviceButtonsPadMouse::SafeDownCast(device);
			m_Mouse = mouse;
		}

		m_Renderer = m_Mouse->GetRenderer();
	}

	if (m_Renderer)
	{
		m_ParallelView = m_Renderer->GetActiveCamera()->GetParallelProjection() != 0;

		if (m_ParallelScale_OnStart == -1) // Save current Parallel Scale
			m_ParallelScale_OnStart = m_Renderer->GetActiveCamera()->GetParallelScale();
	}
}
//----------------------------------------------------------------------------
void appInteractor2DMeasure::Render()
{
	if (m_Renderer)
		m_Renderer->GetRenderWindow()->Render();

	albaEventMacro(albaEvent(this, CAMERA_UPDATE));
}

/// MOUSE EVENTS /////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
void appInteractor2DMeasure::OnLeftButtonDown(albaEventInteraction *e)
{
	if (!m_IsEnabled) return;

	albaEventMacro(albaEvent(this, CAMERA_UPDATE));

	if (!m_ParallelView)
	{
		InitRenderer(e);
	}

	int shiftPressed = e->GetModifier(ALBA_SHIFT_KEY) ? 1 : 0;

	if (shiftPressed)
	{
		Superclass::OnLeftButtonDown(e);
	}
	else if (m_ParallelView && m_IsInBound)
	{
		albaEventMacro(albaEvent(this, ID_MEASURE_STARTED));

		OnButtonDown(e);
		m_DraggingLeft = true;

		double pos_2d[2];
		e->Get2DPosition(pos_2d);
		double pointCoord[3];
		ScreenToWorld(pos_2d, pointCoord);

		SelectMeasure(-1);

		if (m_Action == ACTION_ADD_MEASURE || m_Action == ACTION_EDIT_MEASURE)
		{
			DrawMeasure(pointCoord);
		}
		else if (m_Action == ACTION_MOVE_MEASURE)
		{
			ScreenToWorld(pos_2d, m_StartMousePosition);
			MoveMeasure(m_CurrentMeasureIndex, m_StartMousePosition);
		}

		if (m_CurrentMeasureIndex >= 0)
		{	
			if (m_EditPointSource && m_Action == ACTION_EDIT_MEASURE)
				UpdatePointActor(pos_2d);

			m_Renderer->GetRenderWindow()->Render();
		}
	}

	m_ButtonDownInside = m_IsInBound;
}
//----------------------------------------------------------------------------
void appInteractor2DMeasure::OnLeftButtonUp(albaEventInteraction *e)
{
	if (!m_IsEnabled) return;

	m_DraggingLeft = false;
	OnButtonUp(e);

	if (m_ParallelView && m_IsInBound && m_ButtonDownInside)
	{
		double pos_2d[2];
		double pointCoord[4];
		e->Get2DPosition(pos_2d);
		ScreenToWorld(pos_2d, pointCoord);

		if (m_Action == ACTION_ADD_MEASURE || m_Action == ACTION_EDIT_MEASURE)
		{
			double nullCoord[4] = { -1.0, -1.0, 0.0, 0.0 };
			DrawMeasure(nullCoord);
			SetAction(ACTION_NONE);
		}
		else if (m_Action == ACTION_MOVE_MEASURE)
		{
			m_EndMeasure = true;
			MoveMeasure(m_CurrentMeasureIndex, pointCoord);
		}

		if (m_EndMeasure)
		{
			if (m_Renderer)
				m_Renderer->GetRenderWindow()->Render();
		}

		if (m_MovingMeasure)
		{
			SelectMeasure(m_CurrentMeasureIndex);
			m_CurrentMeasureIndex = -1;

			m_Renderer->RemoveActor2D(m_EditPointActor);
			m_Renderer->GetRenderWindow()->Render();

			m_MovingMeasure = false;
		}

		albaEventMacro(albaEvent(this, ID_MEASURE_FINISHED));
	}
}
//----------------------------------------------------------------------------
void appInteractor2DMeasure::OnRightButtonUp(albaEventInteraction *e)
{
	if (m_CurrentMeasureIndex >= 0)
	{
		albaEventMacro(albaEvent(this, ID_MEASURE_RCLICK));
	}
}
//----------------------------------------------------------------------------
void appInteractor2DMeasure::OnMove(albaEventInteraction *e)
{
	if (!m_IsEnabled) return;

	if (!m_ParallelView)
	{
		InitRenderer(e);
	}

	if (m_ParallelView)
	{
		double pos_2d[2];
		e->Get2DPosition(pos_2d);
		double pointCoord[3];
		ScreenToWorld(pos_2d, pointCoord);

		if (m_IsInBound != IsInBound(pointCoord))
		{
			if (m_Renderer)
				m_Renderer->GetRenderWindow()->Render();

			if (m_IsInBound)
				SetAction(ACTION_ADD_MEASURE);
			else
				SetAction(ACTION_NONE);
		}

		if (m_IsInBound)
		{
			if (!m_DraggingLeft)
			{
				FindAndHighlightCurrentPoint(pointCoord);
			}
			else
			{
				if (m_Action == ACTION_ADD_MEASURE)
					DrawMeasure(pointCoord);
				else if (m_Action == ACTION_MOVE_MEASURE)
					MoveMeasure(m_CurrentMeasureIndex, pointCoord);
			}
		}
	}
}
//----------------------------------------------------------------------------
void appInteractor2DMeasure::OnEvent(albaEventBase *event)
{
	if (!m_IsEnabled) return;

	if (event->GetChannel() == MCH_INPUT)
	{
		albaID id = event->GetId();

		if (id == albaDeviceButtonsPadTracker::GetTracker3DMoveId() || id == albaDeviceButtonsPadMouse::GetMouse2DMoveId())
		{
			albaEventInteraction *e = albaEventInteraction::SafeDownCast(event);
			OnMove(e);
		}
	}

	Superclass::OnEvent(event);
}

/// MEASURE //////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------
void appInteractor2DMeasure::RemoveAllMeasures()
{
	int nMeasures = GetMeasureCount();
	for (int i = nMeasures; i >= 0; i--)
		RemoveMeasure(i);
}

/// GET-SET /////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------
albaString appInteractor2DMeasure::GetMeasure(int index)
{
	if (index >= 0 && index < m_MeasureVector.size())
		return m_MeasureVector[index];

	return "No Measure";
}
//---------------------------------------------------------------------------
albaString appInteractor2DMeasure::GetMeasureLabel(int index)
{
	if (index >= 0 && index < m_MeasureLabelVector.size())
		return m_MeasureLabelVector[index];

	return "No Label";
}
//---------------------------------------------------------------------------
void appInteractor2DMeasure::SetMeasureLabel(int index, albaString text)
{
	if (index >= 0 && index < m_MeasureLabelVector.size())
		m_MeasureLabelVector[index] = text;
}

//----------------------------------------------------------------------------
void appInteractor2DMeasure::SetColor(double r, double g, double b)
{
	m_ColorDefault[0] = r;
	m_ColorDefault[1] = g;
	m_ColorDefault[2] = b;
}
//----------------------------------------------------------------------------
void appInteractor2DMeasure::SetColorSelection(double r, double g, double b)
{
	m_ColorSelection[0] = r;
	m_ColorSelection[1] = g;
	m_ColorSelection[2] = b;
}
//----------------------------------------------------------------------------
void appInteractor2DMeasure::SetColorDisable(double r, double g, double b)
{
	m_ColorDisable[0] = r;
	m_ColorDisable[1] = g;
	m_ColorDisable[2] = b;
}
//----------------------------------------------------------------------------
void appInteractor2DMeasure::SetOpacity(double opacity)
{
	m_Opacity = opacity;

	m_EditTextActor->SetOpacity(opacity);
}
//----------------------------------------------------------------------------
void appInteractor2DMeasure::SetTextSide(int side)
{
	m_TextSide = side;
}
//----------------------------------------------------------------------------
void appInteractor2DMeasure::ShowText(bool show)
{
	m_ShowText = show;

	for (int i = 0; i < m_TextActorVector.size(); i++)
	{
		m_TextActorVector[i]->SetVisibility(show);
	}

	m_EditTextActor->SetVisibility(show);
}
//----------------------------------------------------------------------------
void appInteractor2DMeasure::SetAction(MEASURE_ACTIONS action)
{
	if (m_Action == action)
		return;

	m_Action = action;

	if (m_View)
	{
		// Set Mouse Cursor
		wxCursor cursor = wxCursor(wxCURSOR_ARROW);

		switch (m_Action)
		{
		case ACTION_ADD_MEASURE:
		case ACTION_EDIT_MEASURE:
			cursor = wxCursor(wxCURSOR_PENCIL);
			break;

		case ACTION_MOVE_MEASURE:
			cursor = wxCursor(wxCURSOR_SIZING);
			break;

		default:
			break;
		}

		m_View->GetWindow()->SetCursor(cursor);
	}
}
//----------------------------------------------------------------------------
void appInteractor2DMeasure::SetRendererByView(albaView * view)
{
	m_View = view;

	vtkRenderer *newRenderer = NULL;

	vtkRendererCollection *rc;
	rc = view->GetRWI()->GetRenderWindow()->GetRenderers();

	// Searching for a frontal renderer on render collection
	if (rc)
	{
		rc->InitTraversal();
		vtkRenderer *ren;
		while (ren = rc->GetNextItem())
			if (ren->GetLayer() == 1)//Frontal Render
			{
				newRenderer = ren;
				break;
			}
	}

	//if the renderer is changed we move all actor to new renderer 
	if (m_Renderer != newRenderer)
	{
		// Removing from old renderer
		if (m_Renderer)
		{
			RemoveAllMeasures();
		}
		// Adding to new renderer
		if (newRenderer)
		{
			// 			double p1[3];
			// 			double p2[3];
			// 
			// 			for (int i = 0; i < m_LineSourceVector.size(); i++)
			// 			{
			// 				m_LineSourceVector[i]->GetPoint1(p1);
			// 				m_LineSourceVector[i]->GetPoint2(p2);
			// 				AddMeasure(p1, p2);
			// 			}
		}
	}

	m_Renderer = newRenderer;
	m_Renderer->GetRenderWindow()->Render();
}

//----------------------------------------------------------------------------
void appInteractor2DMeasure::Enable()
{
	m_IsEnabled = true;
	SetOpacity(1);
}
//----------------------------------------------------------------------------
void appInteractor2DMeasure::Disable()
{
	m_IsEnabled = false;
	SetOpacity(0.35);
}

// UTILS /////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
double appInteractor2DMeasure::DistanceBetweenPoints(double *point1, double *point2)
{
	return sqrt(vtkMath::Distance2BetweenPoints(point1, point2));
}
//----------------------------------------------------------------------------
float appInteractor2DMeasure::DistancePointToLine(double * point, double * lineP1, double * lineP2)
{
	double point_x = point[0];
	double point_y = point[1];

	double line_x1 = lineP1[0];
	double line_y1 = lineP1[1];
	double line_x2 = lineP2[0];
	double line_y2 = lineP2[1];

	double diffX = line_x2 - line_x1;
	double diffY = line_y2 - line_y1;

	if ((diffX == 0) && (diffY == 0))
	{
		diffX = point_x - line_x1;
		diffY = point_y - line_y1;
		return sqrt(diffX * diffX + diffY * diffY);
	}

	float t = ((point_x - line_x1) * diffX + (point_y - line_y1) * diffY) / (diffX * diffX + diffY * diffY);

	if (t < 0)
	{
		// Point is nearest to the first point i.e x1 and y1
		diffX = point_x - line_x1;
		diffY = point_y - line_y1;
	}
	else if (t > 1)
	{
		// Point is nearest to the end point i.e x2 and y2
		diffX = point_x - line_x2;
		diffY = point_y - line_y2;
	}
	else
	{
		// If perpendicular line intersect the line segment.
		diffX = point_x - (line_x1 + t * diffX);
		diffY = point_y - (line_y1 + t * diffY);
	}

	// Returning shortest distance
	return sqrt(diffX * diffX + diffY * diffY);
}
//----------------------------------------------------------------------------
bool appInteractor2DMeasure::IsInBound(double *pos)
{
	double bounds[6];
	m_Renderer->ComputeVisiblePropBounds(bounds);
	m_Bounds = bounds;

	m_IsInBound = false;

	if (m_Bounds)
	{
		if (pos[0] > m_Bounds[0] && pos[0] < m_Bounds[1]) // MarginLeft & MarginRight
			if (pos[1] > m_Bounds[2] && pos[1] < m_Bounds[3]) // MarginUp & MarginDown
				m_IsInBound = true;
	}

	return m_IsInBound;
}
//----------------------------------------------------------------------------
void appInteractor2DMeasure::ScreenToWorld(double screen[2], double world[3])
{
	double wp[4];

	m_Renderer->SetDisplayPoint(screen[0], screen[1], 0);
	m_Renderer->DisplayToWorld();
	m_Renderer->GetWorldPoint(wp);

	world[0] = wp[0];
	world[1] = wp[1];
	world[2] = 0;
}

//----------------------------------------------------------------------------
void appInteractor2DMeasure::WorldToScreen(double world[3], double screen[2])
{
	double scr[3];

	m_Renderer->SetWorldPoint(world);
	m_Renderer->WorldToDisplay();
	m_Renderer->GetDisplayPoint(scr);

	screen[0] = scr[0];
	screen[1] = scr[1];
}

// LOAD/SAVE
//----------------------------------------------------------------------------
bool appInteractor2DMeasure::Load(albaVME *input, wxString tag)
{
	return true;
}
//----------------------------------------------------------------------------
bool appInteractor2DMeasure::Save(albaVME *input, wxString tag)
{
	return true;
}