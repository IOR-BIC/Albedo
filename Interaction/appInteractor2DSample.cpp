/*=========================================================================
Program:   Albedo
Module:    appInteractor2DSample.cpp
Language:  C++
Date:      $Date: 2018-01-01 12:00:00 $
Version:   $Revision: 1.0.0.0 $
Authors:   Nicola Vanella
==========================================================================
Copyright (c) LTM-IOR 2018 (https://github.com/IOR-LTM)

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the above copyright notice for more information.
=========================================================================*/

#include "appInteractor2DSample.h"

#include "mafDecl.h"
#include "mafDeviceButtonsPadMouse.h"
#include "mafRWI.h"
#include "mafView.h"
#include "mafVME.h"
#include "mafVMEOutput.h"
#include "mafEventInteraction.h"
#include "mafDeviceButtonsPadTracker.h"

#include "vtkMAFSmartPointer.h"
#include "vtkMath.h"
#include "vtkLine.h"
#include "vtkPolyData.h"
#include "vtkPointData.h"
#include "vtkLineSource.h"
#include "vtkPolyDataMapper2D.h"
#include "vtkProperty2D.h"
#include "vtkActor2D.h"
#include "vtkTextActor.h"
#include "vtkObjectFactory.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderWindow.h"
#include "vtkRenderer.h"
#include "vtkCamera.h"
#include "vtkProbeFilter.h"
#include "vtkXYPlotActor.h"
#include "vtkTextProperty.h"
#include "vtkImageImport.h"
#include "vtkImageAccumulate.h"
#include "vtkTransform.h"
#include "vtkRendererCollection.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"
#include "vtkAppendPolyData.h"
#include "vtkConeSource.h"

#define TEXT_H_SHIFT     10

#define LINE_UPDATE_DISTANCE 1.0
#define POINT_UPDATE_DISTANCE 2.5
#define POINT_UPDATE_DISTANCE_2 (POINT_UPDATE_DISTANCE * POINT_UPDATE_DISTANCE)

//------------------------------------------------------------------------------
mafCxxTypeMacro(appInteractor2DSample)

//----------------------------------------------------------------------------
appInteractor2DSample::appInteractor2DSample()
{
	m_Coordinate = vtkCoordinate::New();
	m_Coordinate->SetCoordinateSystemToWorld();

	m_Mouse = NULL;
	m_Renderer = NULL;
	m_View = NULL;

	m_Bounds = NULL;
	m_IsInBound = false;
	m_ButtonDownInside = false;

	m_Side = 1;
	m_Opacity = 1;
	m_CurrentLineIndex = -1;
	m_CurrentPointIndex = -1;
	m_CurrentAddedLine = 0;

	m_CurrentLineIndex = -1;

	m_DraggingLeft = false;
	m_EndMeasure = false;
	m_ParallelView = false;
	m_RegisterMeasure = false;

	m_EditLinesEnable = true;
	m_CanEditLine = false;

	m_LineAction = ID_NO_ACTION;

	m_AddMeasurePhase_Counter = 0;

	m_LastSelection = -1;
	m_Distance = 0;

	// Line tools
	vtkNEW(m_EditLine);
	m_EditLine->SetPoint1(0, 0, 0);
	m_EditLine->SetPoint2(0.5, 0.5, 0);
	m_EditLine->Update();

	vtkNEW(m_LineMapper);
	m_LineMapper->SetInput(m_EditLine->GetOutput());
	m_LineMapper->SetTransformCoordinate(m_Coordinate);

	vtkNEW(m_EditLineActor);
	m_EditLineActor->SetMapper(m_LineMapper);
	m_EditLineActor->GetProperty()->SetColor(0.0, 1.0, 0.0);
	m_EditLineActor->GetProperty()->SetOpacity(m_Opacity);
	m_EditLineActor->GetProperty()->SetLineWidth(1.0);

	vtkNEW(m_CurrentPointMapper);
	m_CurrentPointMapper->SetTransformCoordinate(m_Coordinate);

	vtkNEW(m_CurrentPointActor);
	m_CurrentPointActor->SetMapper(m_CurrentPointMapper);
	m_CurrentPointActor->GetProperty()->SetPointSize(4.0);
	m_CurrentPointActor->GetProperty()->SetColor(1, 1, 0);
}
//----------------------------------------------------------------------------
appInteractor2DSample::~appInteractor2DSample()
{
	SetAction(ID_NO_ACTION);

	vtkDEL(m_Coordinate);
	vtkDEL(m_EditLine);
	vtkDEL(m_LineMapper);
	vtkDEL(m_EditLineActor);

	int num = m_LineActorVector.size();
	for (int i = 0; i < num; i++)
	{
		m_Renderer->RemoveActor2D(m_LineActorVector[i]);
		vtkDEL(m_LineActorVector[i]);
		vtkDEL(m_LineMapperVector[i]);
		vtkDEL(m_LineSourceVector[i]);

		m_Renderer->GetRenderWindow()->Render();
	}

	if (m_CurrentLineIndex >= 0)
	{
		m_Renderer->RemoveActor2D(m_CurrentPointActor);
	}

	vtkDEL(m_CurrentPointActor);
	vtkDEL(m_CurrentPointMapper);

	m_LineSourceVector.clear();
	m_LineMapperVector.clear();
	m_LineActorVector.clear();
	m_DistanceVector.clear();
}

// MOUSE EVENTS
//----------------------------------------------------------------------------
void appInteractor2DSample::OnLeftButtonDown(mafEventInteraction *e)
{
	mafEventMacro(mafEvent(this, CAMERA_UPDATE));

	if (!m_ParallelView)
	{
		InitRenderer(e);
	}

	int shiftPressed = e->GetModifier(MAF_SHIFT_KEY) ? 1 : 0;

	if (shiftPressed)
	{
		Superclass::OnLeftButtonDown(e);
	}
	else if (m_ParallelView && m_IsInBound)
	{
		OnButtonDown(e);
		m_DraggingLeft = true;

		double pos_2d[2];
		e->Get2DPosition(pos_2d);
		double pointCoord[3];
		ScreenToWorld(pos_2d, pointCoord);

		SelectLine(-1);

		if (m_LineAction == ID_ADD_LINE || m_LineAction == ID_EDIT_LINE)
		{
			DrawMeasureTool(pointCoord);
		}
		else if (m_LineAction == ID_MOVE_LINE)
		{
			ScreenToWorld(pos_2d, m_StartMousePosition);
			MoveLine(m_CurrentLineIndex, m_StartMousePosition);
		}

		if (m_CurrentLineIndex >= 0)
		{
			m_LineActorVector[m_CurrentLineIndex]->GetProperty()->SetColor(1.0, 0.0, 0.0);
			m_LineActorVector[m_CurrentLineIndex]->GetProperty()->SetOpacity(0.5);
			m_LineActorVector[m_CurrentLineIndex]->GetProperty()->SetLineWidth(1.0);

			m_Renderer->GetRenderWindow()->Render();
		}
	}

	m_ButtonDownInside = m_IsInBound;
}
//----------------------------------------------------------------------------
void appInteractor2DSample::OnLeftButtonUp(mafEventInteraction *e)
{
	m_DraggingLeft = false;
	OnButtonUp(e);

	if (m_ParallelView && m_IsInBound && m_ButtonDownInside)
	{
		double pos_2d[2];
		e->Get2DPosition(pos_2d);
		double pointCoord[4];
		ScreenToWorld(pos_2d, pointCoord);

		switch (m_LineAction)
		{
		case ID_ADD_LINE:
		case ID_EDIT_LINE:
		{
			double nullCoord[4] = { -1.0, -1.0, 0.0, 0.0 };
			DrawMeasureTool(nullCoord);
			SetAction(ID_NO_ACTION);
		}
		break;

		case ID_MOVE_LINE:
		{
			m_EndMeasure = true;
			MoveLine(m_CurrentLineIndex, pointCoord);
		}
		break;

		default:
			break;
		}

		if (m_EndMeasure)
		{
			CalculateLine();
			m_RegisterMeasure = false;
		}

		if (m_CanEditLine)
		{
			SelectLine(m_CurrentLineIndex);
			m_CurrentLineIndex = -1;

			m_Renderer->RemoveActor2D(m_CurrentPointActor);
			m_Renderer->GetRenderWindow()->Render();

			m_CanEditLine = false;
		}
	}
}
//----------------------------------------------------------------------------
void appInteractor2DSample::OnMove(mafEventInteraction *e)
{
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
			double opacity = m_IsInBound ? 1.0 : 0.5;

			for (int i = 0; i < m_LineActorVector.size(); i++)
			{
				m_LineActorVector[i]->GetProperty()->SetOpacity(opacity);
			}

			if (m_Renderer)
				m_Renderer->GetRenderWindow()->Render();

			if (m_IsInBound)
				SetAction(ID_ADD_LINE);
			else
				SetAction(ID_NO_ACTION);
		}

		if (m_IsInBound)
		{
			if (!m_DraggingLeft)
			{
				FindAndHighlightCurrentPoint(pointCoord);
			}
			else
			{
				if (m_LineAction == ID_ADD_LINE)
					DrawMeasureTool(pointCoord);
				else if (m_LineAction == ID_MOVE_LINE)
					MoveLine(m_CurrentLineIndex, pointCoord);
			}
		}
	}
}
//----------------------------------------------------------------------------
void appInteractor2DSample::InitRenderer(mafEventInteraction *e)
{
	mafEventMacro(mafEvent(this, CAMERA_UPDATE));

	if (m_Renderer == NULL)
	{
		if (m_Mouse == NULL)
		{
			mafDevice *device = mafDevice::SafeDownCast((mafDevice*)e->GetSender());
			mafDeviceButtonsPadMouse  *mouse = mafDeviceButtonsPadMouse::SafeDownCast(device);
			m_Mouse = mouse;
		}

		m_Renderer = m_Mouse->GetRenderer();
	}

	if (m_Renderer)
		m_ParallelView = m_Renderer->GetActiveCamera()->GetParallelProjection() != 0;
}
//----------------------------------------------------------------------------
void appInteractor2DSample::OnEvent(mafEventBase *event)
{
	mafID ch = event->GetChannel();

	if (ch == MCH_INPUT)
	{
		mafID id = event->GetId();

		if (id == mafDeviceButtonsPadTracker::GetTracker3DMoveId() || id == mafDeviceButtonsPadMouse::GetMouse2DMoveId())
		{
			mafEventInteraction *e = mafEventInteraction::SafeDownCast(event);
			OnMove(e);
		}
	}

	Superclass::OnEvent(event);
}

//----------------------------------------------------------------------------
void appInteractor2DSample::ScreenToWorld(double screen[2], double world[3])
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
void appInteractor2DSample::DrawMeasureTool(double * wp)
{
	// No point has yet been picked
	if (m_AddMeasurePhase_Counter == 0)
	{
		// Initialization
		m_EndMeasure = false;
		m_ActorAdded = false;
		m_Distance = 0.0;
		m_EditLine->SetPoint1(wp);

		// Edit Line
		if (m_CurrentLineIndex >= 0)
		{
			if (m_CurrentPointIndex == 1)
			{
				double tmp_p2[3];
				m_LineSourceVector[m_CurrentLineIndex]->GetPoint2(tmp_p2);
				m_EditLine->SetPoint1(tmp_p2);
			}
			else if (m_CurrentPointIndex == 2)
			{
				double tmp_p1[3];
				m_LineSourceVector[m_CurrentLineIndex]->GetPoint1(tmp_p1);
				m_EditLine->SetPoint1(tmp_p1);
			}
		}

		m_EditLine->SetPoint2(wp);
		m_EditLine->Update();

		m_AddMeasurePhase_Counter++;
	}
	// First point has been picked and the second one is being dragged
	else if (m_AddMeasurePhase_Counter == 1 && m_DraggingLeft)
	{
		if (!m_ActorAdded)
		{
			m_ActorAdded = true;
			m_Renderer->AddActor2D(m_EditLineActor);
		}

		m_EditLine->SetPoint2(wp);
		m_EditLine->Update();

		CalculateLine();
	}

	// Finished dragging the second point
	else if (m_AddMeasurePhase_Counter == 1)
	{
		m_EndMeasure = true;
		m_AddMeasurePhase_Counter++;
	}

	if (m_EndMeasure)
	{
		m_AddMeasurePhase_Counter = 0;

		// Delete temporary Line
		m_Renderer->RemoveActor2D(m_EditLineActor);

		if (m_Distance > 0.0)
		{
			double tmpPt1[3], tmpPt2[3];
			m_EditLine->GetPoint1(tmpPt1);
			m_EditLine->GetPoint2(tmpPt2);

			if (m_CurrentLineIndex >= 0)
			{
				EditMeasureLine(m_CurrentLineIndex, tmpPt1, tmpPt2);
				mafEventMacro(mafEvent(this, ID_LINE_CHANGED, m_Distance));
			}
			else
			{
				AddMeasureLine(tmpPt1, tmpPt2);
				mafEventMacro(mafEvent(this, ID_LINE_ADDED, m_Distance));
			}
		}
		else
		{
			SelectLine(m_LastSelection);
		}

		m_ActorAdded = false;
	}

	SetAction(ID_ADD_LINE);

	m_Renderer->GetRenderWindow()->Render();
}
//----------------------------------------------------------------------------
void appInteractor2DSample::CalculateLine()
{
	double p1_1[3], p2_1[3];

	m_EditLine->GetPoint1(p1_1);
	m_EditLine->GetPoint2(p2_1);

	m_Distance = sqrt(vtkMath::Distance2BetweenPoints(p1_1, p2_1));
	mafEventMacro(mafEvent(this, ID_RESULT_LINE, m_Distance));
}
//----------------------------------------------------------------------------
void appInteractor2DSample::SelectLine(int index)
{
	if (m_LineActorVector.size() != 0)
	{
		// Deselect all
		for (int i = 0; i < m_LineActorVector.size(); i++)
		{
			if (m_LineActorVector[i] != NULL)
			{
				m_LineActorVector[i]->GetProperty()->SetColor(m_ColorLine[0], m_ColorLine[1], m_ColorLine[2]);
				m_LineActorVector[i]->GetProperty()->SetOpacity(m_Opacity);
			}
		}

		if (index != -1 && m_LineActorVector[index] != NULL)
		{
			m_LineActorVector[index]->GetProperty()->SetColor(m_ColorSelection[0], m_ColorSelection[1], m_ColorSelection[2]);

			m_LastSelection = index;
			m_LastEditing = -1;
		}

		mafEventMacro(mafEvent(this, CAMERA_UPDATE));
		mafEventMacro(mafEvent(this, ID_LINE_SELECTED));
	}
}
//----------------------------------------------------------------------------
void appInteractor2DSample::AddMeasureLine(double *point1, double *point2)
{
	point1[2] = point2[2] = 0;

	m_LineSourceVector.push_back(vtkLineSource::New());
	m_LineSourceVector[m_LineSourceVector.size() - 1]->SetPoint1(point1);
	m_LineSourceVector[m_LineSourceVector.size() - 1]->SetPoint2(point2);
	m_LineSourceVector[m_LineSourceVector.size() - 1]->Update();

	m_LineMapperVector.push_back(vtkPolyDataMapper2D::New());
	m_LineMapperVector[m_LineMapperVector.size() - 1]->SetTransformCoordinate(m_Coordinate);
	m_LineMapperVector[m_LineMapperVector.size() - 1]->SetInput(m_LineSourceVector[m_LineSourceVector.size() - 1]->GetOutput());

	m_LineActorVector.push_back(vtkActor2D::New());
	m_LineActorVector[m_LineActorVector.size() - 1]->SetMapper(m_LineMapperVector[m_LineMapperVector.size() - 1]);
	m_LineActorVector[m_LineActorVector.size() - 1]->GetProperty()->SetColor(m_ColorSelection[0], m_ColorSelection[1], m_ColorSelection[2]);
	m_LineActorVector[m_LineActorVector.size() - 1]->GetProperty()->SetOpacity(m_Opacity);

	m_Renderer->AddActor2D(m_LineActorVector[m_LineActorVector.size() - 1]);

	m_EditLine->SetPoint1(point1);
	m_EditLine->SetPoint2(point2);
	m_EditLine->Update();

	m_CurrentAddedLine++;

	CalculateLine();

	// Push Distance
	m_DistanceVector.push_back(m_Distance);

	mafEventMacro(mafEvent(this, CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void appInteractor2DSample::EditMeasureLine(int index, double *point1, double *point2)
{
	if (index < 0 || index >= m_LineSourceVector.size())
		return;

	m_CanEditLine = true;

	point1[2] = point2[2] = 0;

	m_LastEditing = index;

	m_LineSourceVector[index]->SetPoint1(point1);
	m_LineSourceVector[index]->SetPoint2(point2);
	m_LineSourceVector[index]->Update();

	m_LineMapperVector[index]->SetTransformCoordinate(m_Coordinate);
	m_LineMapperVector[index]->SetInput(m_LineSourceVector[index]->GetOutput());

	m_LineActorVector[index]->SetMapper(m_LineMapperVector[index]);
	m_LineActorVector[index]->GetProperty()->SetColor(m_ColorSelection[0], m_ColorSelection[1], m_ColorSelection[2]);
	m_LineActorVector[index]->GetProperty()->SetOpacity(m_Opacity);

	m_EditLine->SetPoint1(point1);
	m_EditLine->SetPoint2(point2);
	m_EditLine->Update();

	CalculateLine();

	// Measure
	m_DistanceVector[index] = m_Distance;

	UpdateTextActor(point1, point2, index);

	mafEventMacro(mafEvent(this, CAMERA_UPDATE));
}

//----------------------------------------------------------------------------
void appInteractor2DSample::UpdateTextActor(double * point1, double * point2, int index)
{

}

//----------------------------------------------------------------------------
void appInteractor2DSample::MoveLine(int index, double * pointCoord)
{
	if (index < 0)
		return;

	double linePoint1[3];
	double linePoint2[3];

	m_LineSourceVector[index]->GetPoint1(linePoint1);
	m_LineSourceVector[index]->GetPoint2(linePoint2);

	if (!m_CanEditLine)
	{
		m_OldLineP1[0] = linePoint1[0] - m_StartMousePosition[0];
		m_OldLineP1[1] = linePoint1[1] - m_StartMousePosition[1];
		m_OldLineP2[0] = linePoint2[0] - m_StartMousePosition[0];
		m_OldLineP2[1] = linePoint2[1] - m_StartMousePosition[1];

		m_CanEditLine = true;
	}

	if (m_DraggingLeft)
	{
		if (!m_ActorAdded)
		{
			m_ActorAdded = true;
			m_Renderer->AddActor2D(m_EditLineActor);
		}

		// Initialization
		m_Distance = 0.0;

		double tmp_pos1[3];
		double tmp_pos2[3];

		tmp_pos1[0] = pointCoord[0] + m_OldLineP1[0];
		tmp_pos1[1] = pointCoord[1] + m_OldLineP1[1];
		tmp_pos1[2] = 0.0;

		tmp_pos2[0] = pointCoord[0] + m_OldLineP2[0];
		tmp_pos2[1] = pointCoord[1] + m_OldLineP2[1];
		tmp_pos2[2] = 0.0;

		m_EditLine->SetPoint1(tmp_pos1);
		m_EditLine->SetPoint2(tmp_pos2);
		m_EditLine->Update();

		CalculateLine();

		UpdateTextActor(tmp_pos1, tmp_pos2, -1);
	}
	else if (m_EditLine)
	{
		// Delete temporary Line
		m_Renderer->RemoveActor2D(m_EditLineActor);

		double tmpPt[3], tmpPt2[3];
		m_EditLine->GetPoint1(tmpPt);
		m_EditLine->GetPoint2(tmpPt2);

		EditMeasureLine(index, tmpPt, tmpPt2);
		mafEventMacro(mafEvent(this, ID_LINE_CHANGED, m_Distance));

		m_ActorAdded = false;
	}

	m_Renderer->GetRenderWindow()->Render();
}
//----------------------------------------------------------------------------
void appInteractor2DSample::RemoveLine(int index)
{
	if (m_Renderer && index < m_CurrentAddedLine)
	{
		m_Renderer->RemoveActor2D(m_LineActorVector[index]);
		vtkDEL(m_LineActorVector[index]);
		m_LineActorVector.erase(m_LineActorVector.begin() + index);
		vtkDEL(m_LineMapperVector[index]);
		m_LineMapperVector.erase(m_LineMapperVector.begin() + index);
		vtkDEL(m_LineSourceVector[index]);
		m_LineSourceVector.erase(m_LineSourceVector.begin() + index);

		m_Renderer->GetRenderWindow()->Render();

		m_DistanceVector.erase(m_DistanceVector.begin() + index);

		m_CurrentAddedLine--;

		mafEventMacro(mafEvent(this, CAMERA_UPDATE));
	}
}
//---------------------------------------------------------------------------
void appInteractor2DSample::RemoveAllLines()
{
	int nMeasures = m_DistanceVector.size();

	for (int i = nMeasures; i >= 0; i--)
	{
		RemoveLine(i);
	}
}
//---------------------------------------------------------------------------
void appInteractor2DSample::GetLinePoints(int index, double *point1, double *point2)
{
	// Return line points values
	if (index >= 0 && index < SizeLineVector())
	{
		m_LineSourceVector[index]->GetPoint1(point1);
		m_LineSourceVector[index]->GetPoint2(point2);
	}
}

//----------------------------------------------------------------------------
void appInteractor2DSample::SetRendererByView(mafView * view)
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
			for (int i = 0; i < SizeLineVector(); i++)
				m_Renderer->RemoveActor2D(m_LineActorVector[i]);
		}
		// Adding to new renderer
		if (newRenderer)
		{
			for (int i = 0; i < SizeLineVector(); i++)
				newRenderer->AddActor2D(m_LineActorVector[i]);
		}
	}

	m_Renderer = newRenderer;
	m_Renderer->GetRenderWindow()->Render();
}

//----------------------------------------------------------------------------
void appInteractor2DSample::SetColor(double r, double g, double b)
{
	m_ColorLine[0] = r;
	m_ColorLine[1] = g;
	m_ColorLine[2] = b;
}
//----------------------------------------------------------------------------
void appInteractor2DSample::SetColorSelection(double r, double g, double b)
{
	m_ColorSelection[0] = r;
	m_ColorSelection[1] = g;
	m_ColorSelection[2] = b;
}
//----------------------------------------------------------------------------
void appInteractor2DSample::SetOpacity(double opacity)
{
	m_Opacity = opacity;
}
//----------------------------------------------------------------------------
void appInteractor2DSample::SetSide(int side)
{
	m_Side = side;
}

//----------------------------------------------------------------------------
void appInteractor2DSample::FindAndHighlightCurrentPoint(double * pointCoord)
{
	SetAction(ID_ADD_LINE);

	if (m_EditLinesEnable)
		for (int i = 0; i < m_CurrentAddedLine; i++)
		{
			double linePoint1[3];
			double linePoint2[3];
			m_LineSourceVector[i]->GetPoint1(linePoint1);
			m_LineSourceVector[i]->GetPoint2(linePoint2);

			if (DistancePointToLine(pointCoord, linePoint1, linePoint2) < LINE_UPDATE_DISTANCE)
			{
				SelectLine(i);
				m_LineActorVector[i]->GetProperty()->SetLineWidth(3.0);

				if (vtkMath::Distance2BetweenPoints(linePoint1, pointCoord) < POINT_UPDATE_DISTANCE_2)
				{
					if (m_CurrentLineIndex != i)
					{
						SetAction(ID_EDIT_LINE);

						m_CurrentLineIndex = i;
						m_CurrentPointIndex = 1;

						m_Renderer->AddActor2D(m_CurrentPointActor);
						m_Renderer->GetRenderWindow()->Render();
					}
					return;
				}

				if (vtkMath::Distance2BetweenPoints(linePoint2, pointCoord) < POINT_UPDATE_DISTANCE_2)
				{
					if (m_CurrentLineIndex != i)
					{
						SetAction(ID_EDIT_LINE);

						m_CurrentLineIndex = i;
						m_CurrentPointIndex = 2;

						m_Renderer->AddActor2D(m_CurrentPointActor);
						m_Renderer->GetRenderWindow()->Render();
					}
					return;
				}

				m_CurrentLineIndex = i;
				SetAction(ID_MOVE_LINE);
				return;
			}
		}

	if (m_CurrentLineIndex >= 0)
	{
		m_LineActorVector[m_CurrentLineIndex]->GetProperty()->SetLineWidth(1.0);
		m_Renderer->RemoveActor2D(m_CurrentPointActor);
		m_Renderer->GetRenderWindow()->Render();
		m_CurrentLineIndex = -1;
		m_CurrentPointIndex = -1;
	}
}

//----------------------------------------------------------------------------
float appInteractor2DSample::DistancePointToLine(double * point, double * lineP1, double * lineP2)
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
void appInteractor2DSample::SetAction(int action)
{
	if (m_LineAction == action)
		return;

	m_LineAction = action;

	// Set Mouse Cursor
	wxCursor cursor = wxCursor(wxCURSOR_ARROW);

	switch (m_LineAction)
	{
	case ID_ADD_LINE:
	case ID_EDIT_LINE:
		cursor = wxCursor(wxCURSOR_PENCIL);
		break;

	case ID_MOVE_LINE:
		cursor = wxCursor(wxCURSOR_SIZING);
		break;

	default:
		break;
	}

	if (m_View)
		m_View->GetWindow()->SetCursor(cursor);
}

//----------------------------------------------------------------------------
bool appInteractor2DSample::IsInBound(double *pos)
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