/*=========================================================================
Program:   Albedo
Module:    appInteractor2DSample.cpp
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

#include "appInteractor2DSample.h"

#include "albaDecl.h"
#include "albaDeviceButtonsPadMouse.h"
#include "albaRWI.h"
#include "albaView.h"
#include "albaVME.h"
#include "albaVMEOutput.h"
#include "albaEventInteraction.h"
#include "albaDeviceButtonsPadTracker.h"

#include "vtkALBASmartPointer.h"
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
#include "vtkALBATextActorMeter.h"
#include "vtkPointSource.h"
#include "vtkConeSource.h"

#define TEXT_H_SHIFT     10

#define LINE_UPDATE_DISTANCE 1.0
#define POINT_UPDATE_DISTANCE 2.5
#define POINT_UPDATE_DISTANCE_2 (POINT_UPDATE_DISTANCE * POINT_UPDATE_DISTANCE)

//------------------------------------------------------------------------------
albaCxxTypeMacro(appInteractor2DSample)

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

	m_TextSide = 1;
	m_Opacity = 1;
	m_CurrentLineIndex = -1;
	m_CurrentPointIndex = -1;
	m_CurrentAddedLine = 0;

	m_CurrentLineIndex = -1;

	m_DraggingLeft = false;
	m_EndMeasure = false;
	m_ParallelView = false;
	m_RegisterMeasure = false;

	m_EditMeasureEnable = true;
	m_CanEditLine = false;
	m_ShowText = false;
	m_ShowArrow = false;

	m_LineAction = ID_NO_ACTION;

	m_AddMeasurePhase_Counter = 0;

	m_LastSelection = -1;
	m_Distance = 0;

	m_ColorDefault[0] = 1;
	m_ColorDefault[1] = 0;
	m_ColorDefault[2] = 1;
	m_ColorSelection[0] = 0;
	m_ColorSelection[1] = 1;
	m_ColorSelection[2] = 0;
	m_ColorDisable[0] = 1;
	m_ColorDisable[1] = 0;
	m_ColorDisable[2] = 0;

	// Line tool
	vtkNEW(m_EditLineSource);
	m_EditLineSource->SetPoint1(0, 0, 0);
	m_EditLineSource->SetPoint2(0.5, 0.5, 0);
	m_EditLineSource->Update();

	vtkNEW(m_EditLineMapper);
	m_EditLineMapper->SetInput(m_EditLineSource->GetOutput());
	m_EditLineMapper->SetTransformCoordinate(m_Coordinate);

	vtkNEW(m_EditLineActor);
	m_EditLineActor->SetMapper(m_EditLineMapper);
	m_EditLineActor->GetProperty()->SetColor(m_ColorSelection[0], m_ColorSelection[1], m_ColorSelection[2]);
	m_EditLineActor->GetProperty()->SetOpacity(m_Opacity);
	m_EditLineActor->GetProperty()->SetLineWidth(1.0);

	// Text tool
	vtkNEW(m_EditTextActor);
	m_EditTextActor->SetColor(m_ColorSelection[0], m_ColorSelection[1], m_ColorSelection[2]);
	m_EditTextActor->SetOpacity(m_Opacity);

	// Arrow tool
	vtkNEW(m_EditConeSource);
	vtkNEW(m_EditConeMapper);
	m_EditConeMapper->SetInput(m_EditConeSource->GetOutput());
	m_EditConeMapper->SetTransformCoordinate(m_Coordinate);

	vtkNEW(m_EditConeActor);
	m_EditConeActor->SetMapper(m_EditConeMapper);
	m_EditConeActor->GetProperty()->SetColor(m_ColorSelection[0], m_ColorSelection[1], m_ColorSelection[2]);
	m_EditConeActor->GetProperty()->SetOpacity(m_Opacity);
	m_EditConeActor->GetProperty()->SetLineWidth(1.0);

	// Point tool
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
appInteractor2DSample::~appInteractor2DSample()
{
	SetAction(ID_NO_ACTION);

	vtkDEL(m_Coordinate);

	m_Renderer->RemoveActor2D(m_EditLineActor);
	m_Renderer->RemoveActor2D(m_EditTextActor);
	m_Renderer->RemoveActor2D(m_EditConeActor);
	m_Renderer->RemoveActor2D(m_EditPointActor);

	// Edit Actor
	vtkDEL(m_EditLineSource);
	vtkDEL(m_EditLineMapper);
	vtkDEL(m_EditLineActor);
	vtkDEL(m_EditTextActor);
	vtkDEL(m_EditConeSource);
	vtkDEL(m_EditConeMapper);
	vtkDEL(m_EditConeActor);
	vtkDEL(m_EditPointSource);
	vtkDEL(m_EditPointMapper);
	vtkDEL(m_EditPointActor);

	for (int i = 0; i < m_LineActorVector.size(); i++)
	{
		// Lines
		m_Renderer->RemoveActor2D(m_LineActorVector[i]);
		vtkDEL(m_LineActorVector[i]);
		vtkDEL(m_LineMapperVector[i]);
		vtkDEL(m_LineSourceVector[i]);

		// Texts
		m_Renderer->RemoveActor2D(m_TextActorVector[i]);
		vtkDEL(m_TextActorVector[i]);

		// Arrows
		m_Renderer->RemoveActor2D(m_ConeActorVector[i]);
		vtkDEL(m_ConeActorVector[i]);
		vtkDEL(m_ConeMapperVector[i]);
		vtkDEL(m_ConeSourceVector[i]);
	}

	m_Renderer->GetRenderWindow()->Render();

	m_LineSourceVector.clear();
	m_LineMapperVector.clear();
	m_LineActorVector.clear();

	m_TextActorVector.clear();

	m_ConeSourceVector.clear();
	m_ConeMapperVector.clear();
	m_ConeActorVector.clear();

	m_MeasureVector.clear();
}

// MOUSE EVENTS
//----------------------------------------------------------------------------
void appInteractor2DSample::OnLeftButtonDown(albaEventInteraction *e)
{
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
		OnButtonDown(e);
		m_DraggingLeft = true;

		double pos_2d[2];
		e->Get2DPosition(pos_2d);
		double pointCoord[3];
		ScreenToWorld(pos_2d, pointCoord);

		SelectMeasure(-1);

		if (m_LineAction == ID_ADD_LINE || m_LineAction == ID_EDIT_LINE)
		{
			DrawMeasure(pointCoord);
		}
		else if (m_LineAction == ID_MOVE_LINE)
		{
			ScreenToWorld(pos_2d, m_StartMousePosition);
			MoveMeasure(m_CurrentLineIndex, m_StartMousePosition);
		}

		if (m_CurrentLineIndex >= 0)
		{
			// Old Line Actor
			m_LineActorVector[m_CurrentLineIndex]->GetProperty()->SetColor(m_ColorDisable[0], m_ColorDisable[1], m_ColorDisable[2]);
			// Old Text Actor
			m_TextActorVector[m_CurrentLineIndex]->SetColor(m_ColorDisable[0], m_ColorDisable[1], m_ColorDisable[2]);
			// Old Arrow Actor
			m_ConeActorVector[m_CurrentLineIndex]->GetProperty()->SetColor(m_ColorDisable[0], m_ColorDisable[1], m_ColorDisable[2]);
			
			if (m_EditPointSource && m_LineAction == ID_EDIT_LINE)
				UpdatePointActor(pos_2d);

			m_Renderer->GetRenderWindow()->Render();
		}
	}

	m_ButtonDownInside = m_IsInBound;
}
//----------------------------------------------------------------------------
void appInteractor2DSample::OnLeftButtonUp(albaEventInteraction *e)
{
	m_DraggingLeft = false;
	OnButtonUp(e);

	if (m_ParallelView && m_IsInBound && m_ButtonDownInside)
	{
		double pos_2d[2];
		double pointCoord[4];
		e->Get2DPosition(pos_2d);
		ScreenToWorld(pos_2d, pointCoord);

		switch (m_LineAction)
		{
		case ID_ADD_LINE:
		case ID_EDIT_LINE:
		{
			double nullCoord[4] = { -1.0, -1.0, 0.0, 0.0 };
			DrawMeasure(nullCoord);
			SetAction(ID_NO_ACTION);
		}
		break;

		case ID_MOVE_LINE:
		{
			m_EndMeasure = true;
			MoveMeasure(m_CurrentLineIndex, pointCoord);
		}
		break;

		default:
			break;
		}

		if (m_EndMeasure)
		{
			CalculateDistance();
			m_RegisterMeasure = false;

			if (m_Renderer)
				m_Renderer->GetRenderWindow()->Render();
		}

		if (m_CanEditLine)
		{
			SelectMeasure(m_CurrentLineIndex);
			m_CurrentLineIndex = -1;

			m_Renderer->RemoveActor2D(m_EditPointActor);
			m_Renderer->GetRenderWindow()->Render();

			m_CanEditLine = false;
		}
	}
}
//----------------------------------------------------------------------------
void appInteractor2DSample::OnMove(albaEventInteraction *e)
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
					DrawMeasure(pointCoord);
				else if (m_LineAction == ID_MOVE_LINE)
					MoveMeasure(m_CurrentLineIndex, pointCoord);
			}
		}
	}
}

//----------------------------------------------------------------------------
void appInteractor2DSample::OnEvent(albaEventBase *event)
{
	albaID ch = event->GetChannel();

	if (ch == MCH_INPUT)
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

// RENDERING
//----------------------------------------------------------------------------
void appInteractor2DSample::DrawMeasure(double * wp)
{
	// No point has yet been picked
	if (m_AddMeasurePhase_Counter == 0)
	{
		// Initialization
		m_EndMeasure = false;
		m_ActorAdded = false;
		m_Distance = 0.0;
		m_EditLineSource->SetPoint1(wp);

		// Edit Line
		if (m_CurrentLineIndex >= 0)
		{
			if (m_CurrentPointIndex == 1)
			{
				double tmp_p2[3];
				m_LineSourceVector[m_CurrentLineIndex]->GetPoint2(tmp_p2);
				m_EditLineSource->SetPoint1(tmp_p2);
			}
			else if (m_CurrentPointIndex == 2)
			{
				double tmp_p1[3];
				m_LineSourceVector[m_CurrentLineIndex]->GetPoint1(tmp_p1);
				m_EditLineSource->SetPoint1(tmp_p1);
			}
		}

		m_EditLineSource->SetPoint2(wp);
		m_EditLineSource->Update();

		m_AddMeasurePhase_Counter++;
	}
	// First point has been picked and the second one is being dragged
	else if (m_AddMeasurePhase_Counter == 1 && m_DraggingLeft)
	{
		if (!m_ActorAdded)
		{
			m_ActorAdded = true;
			m_Renderer->AddActor2D(m_EditLineActor);
			m_Renderer->AddActor2D(m_EditTextActor);
		}

		double editPoint1[3];
		m_EditLineSource->GetPoint1(editPoint1);
		UpdateLineActor(-1, editPoint1, wp);
		
		CalculateDistance();

		UpdateTextActor(-1, editPoint1, wp);
		UpdateConeActor(-1, editPoint1, wp);
		UpdatePointActor(wp);
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

		// Delete temporary Line and Text
		m_Renderer->RemoveActor2D(m_EditLineActor);
		m_Renderer->RemoveActor2D(m_EditTextActor);

		if (m_Distance > 0.0)
		{
			double tmpPt1[3], tmpPt2[3];
			m_EditLineSource->GetPoint1(tmpPt1);
			m_EditLineSource->GetPoint2(tmpPt2);

			if (m_CurrentLineIndex >= 0)
			{
				EditMeasure(m_CurrentLineIndex, tmpPt1, tmpPt2);
				albaEventMacro(albaEvent(this, ID_LINE_CHANGED, m_Distance));
			}
			else
			{
				AddMeasure(tmpPt1, tmpPt2);
				albaEventMacro(albaEvent(this, ID_LINE_ADDED, m_Distance));
			}
		}
		else
		{
			SelectMeasure(m_LastSelection);
		}

		m_ActorAdded = false;
	}

	SetAction(ID_ADD_LINE);

	m_Renderer->GetRenderWindow()->Render();
}
//----------------------------------------------------------------------------
void appInteractor2DSample::MoveMeasure(int index, double * pointCoord)
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
			// Add Edit Actors
			m_ActorAdded = true;
			m_Renderer->AddActor2D(m_EditLineActor);
			m_Renderer->AddActor2D(m_EditTextActor);
			m_Renderer->AddActor2D(m_EditConeActor);
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

		UpdateLineActor(-1, tmp_pos1, tmp_pos2);

		CalculateDistance();

		UpdateTextActor(-1, tmp_pos1, tmp_pos2);
		UpdateConeActor(-1, tmp_pos1, tmp_pos2);
	}
	else if (m_EditLineSource)
	{
		// Remove Edit Actors
		m_Renderer->RemoveActor2D(m_EditLineActor);
		m_Renderer->RemoveActor2D(m_EditTextActor);
		m_Renderer->RemoveActor2D(m_EditConeActor);

		double tmpPt[3], tmpPt2[3];
		m_EditLineSource->GetPoint1(tmpPt);
		m_EditLineSource->GetPoint2(tmpPt2);

		EditMeasure(index, tmpPt, tmpPt2);
		albaEventMacro(albaEvent(this, ID_LINE_CHANGED, m_Distance));

		m_ActorAdded = false;
	}

	m_Renderer->GetRenderWindow()->Render();
}

//----------------------------------------------------------------------------
void appInteractor2DSample::UpdateLineActor(int index, double * point1, double * point2)
{
	if (index > -1)
	{
		m_LineSourceVector[index]->SetPoint1(point1);
		m_LineSourceVector[index]->SetPoint2(point2);
		m_LineSourceVector[index]->Update();

		m_LineMapperVector[index]->SetTransformCoordinate(m_Coordinate);
		m_LineMapperVector[index]->SetInput(m_LineSourceVector[index]->GetOutput());

		m_LineActorVector[index]->SetMapper(m_LineMapperVector[index]);

		if (m_LastSelection == index)
			m_LineActorVector[index]->GetProperty()->SetColor(m_ColorSelection[0], m_ColorSelection[1], m_ColorSelection[2]);
		else
			m_LineActorVector[index]->GetProperty()->SetColor(m_ColorDefault[0], m_ColorDefault[1], m_ColorDefault[2]);

		m_LineActorVector[index]->GetProperty()->SetOpacity(m_Opacity);
	}
	else
	{
		m_EditLineSource->SetPoint1(point1);
		m_EditLineSource->SetPoint2(point2);
		m_EditLineSource->Update();

		m_EditLineMapper->SetTransformCoordinate(m_Coordinate);
	}
}
//----------------------------------------------------------------------------
void appInteractor2DSample::UpdateTextActor(int index, double * point1, double * point2)
{
	albaString ds = "Text";
	ds = wxString::Format(_("%.2f"), m_Distance);

	double text_pos[3];
	text_pos[0] = (point1[0] + point2[0]) / 2;
	text_pos[1] = (point1[1] + point2[1]) / 2;
	text_pos[2] = (point1[2] + point2[2]) / 2;

	text_pos[0] -= m_TextSide *TEXT_H_SHIFT;

	if (index > -1)
	{
		m_TextActorVector[index]->SetText(ds);
		m_TextActorVector[index]->SetTextPosition(text_pos);

		if (m_LastSelection == index)
			m_TextActorVector[index]->SetColor(m_ColorSelection[0], m_ColorSelection[1], m_ColorSelection[2]);
		else
			m_TextActorVector[index]->SetColor(m_ColorDefault[0], m_ColorDefault[1], m_ColorDefault[2]);

		m_TextActorVector[index]->SetOpacity(m_Opacity);
		m_TextActorVector[index]->SetVisibility(m_ShowText);
	}
	else
	{
		m_EditTextActor->SetText(ds);
		m_EditTextActor->SetTextPosition(text_pos);
		m_EditTextActor->SetVisibility(m_ShowText);
	}
}
//----------------------------------------------------------------------------
void appInteractor2DSample::UpdateConeActor(int index, double * point1, double * point2)
{
	double distance = 0;
	distance = sqrt(vtkMath::Distance2BetweenPoints(point1, point2));

	double direction[3];
	direction[0] = point2[0] - point1[0];
	direction[1] = point2[1] - point1[1];
	direction[2] = point2[2] - point1[2];

	if (index > -1)
	{
		m_ConeSourceVector[index]->SetCenter(point2);
		m_ConeSourceVector[index]->SetDirection(direction);
		m_ConeSourceVector[index]->SetRadius(distance / 30.0);
		m_ConeSourceVector[index]->SetHeight(distance / 20.0);
		m_ConeSourceVector[index]->SetResolution(8);
		m_ConeSourceVector[index]->Update();

		m_ConeMapperVector[index]->SetTransformCoordinate(m_Coordinate);
		m_ConeMapperVector[index]->SetInput(m_ConeSourceVector[index]->GetOutput());

		m_ConeActorVector[index]->SetMapper(m_ConeMapperVector[index]);

		if (m_LastSelection == index)
			m_ConeActorVector[index]->GetProperty()->SetColor(m_ColorSelection[0], m_ColorSelection[1], m_ColorSelection[2]);
		else
			m_ConeActorVector[index]->GetProperty()->SetColor(m_ColorDefault[0], m_ColorDefault[1], m_ColorDefault[2]);

		m_ConeActorVector[index]->GetProperty()->SetOpacity(m_Opacity);
		m_ConeActorVector[index]->SetVisibility(m_ShowArrow);
	}
	else
	{
		m_EditConeSource->SetCenter(point2);
		m_EditConeSource->SetDirection(direction);
		m_EditConeSource->SetRadius(distance / 30.0);
		m_EditConeSource->SetHeight(distance / 20.0);
		m_EditConeSource->SetResolution(8);
		m_EditConeSource->Update();

		m_EditConeMapper->SetTransformCoordinate(m_Coordinate);
		m_EditConeMapper->SetInput(m_EditConeSource->GetOutput());

		m_EditConeActor->SetMapper(m_EditConeMapper);
		m_EditConeActor->SetVisibility(m_ShowArrow);
	}
}
//----------------------------------------------------------------------------
void appInteractor2DSample::UpdatePointActor(double * point)
{
	m_EditPointSource->SetCenter(point);
	m_EditPointSource->Update();
}

// MEASURE
//----------------------------------------------------------------------------
void appInteractor2DSample::AddMeasure(double *point1, double *point2)
{
	point1[2] = point2[2] = 0;

	// Update Edit Actors
	m_EditLineSource->SetPoint1(point1);
	m_EditLineSource->SetPoint2(point2);
	m_EditLineSource->Update();

	CalculateDistance();

	// Push Distance
	m_MeasureVector.push_back(m_Distance);

	//////////////////////////////////////////////////////////////////////////
	// Add Line
	m_LineSourceVector.push_back(vtkLineSource::New());
	m_LineMapperVector.push_back(vtkPolyDataMapper2D::New());
	m_LineActorVector.push_back(vtkActor2D::New());

	m_Renderer->AddActor2D(m_LineActorVector[m_LineActorVector.size() - 1]);

	UpdateLineActor(m_LineSourceVector.size() - 1, point1, point2);

	//////////////////////////////////////////////////////////////////////////
	// Add Text
	m_TextActorVector.push_back(vtkALBATextActorMeter::New());
	m_Renderer->AddActor2D(m_TextActorVector[m_TextActorVector.size() - 1]);

	UpdateTextActor(m_TextActorVector.size() - 1, point1, point2);

	//////////////////////////////////////////////////////////////////////////
	// Add Arrow
	m_ConeSourceVector.push_back(vtkConeSource::New());
	m_ConeMapperVector.push_back(vtkPolyDataMapper2D::New());
	m_ConeActorVector.push_back(vtkActor2D::New());
	m_Renderer->AddActor2D(m_ConeActorVector[m_ConeActorVector.size() - 1]);

	UpdateConeActor(m_ConeActorVector.size() - 1, point1, point2);
	//////////////////////////////////////////////////////////////////////////

	m_CurrentAddedLine++;

	albaEventMacro(albaEvent(this, CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void appInteractor2DSample::EditMeasure(int index, double *point1, double *point2)
{
	if (index < 0 || index >= m_LineSourceVector.size())
		return;

	m_CanEditLine = true;

	point1[2] = point2[2] = 0;

	m_LastEditing = index;

	//////////////////////////////////////////////////////////////////////////

	// Edit Actors
	UpdateLineActor(-1, point1, point2);
	UpdateTextActor(-1, point1, point2);
	UpdateConeActor(-1, point1, point2);

	// Distance
	CalculateDistance();
	m_MeasureVector[index] = m_Distance;

	// Line
	UpdateLineActor(index, point1, point2);

	// Text
	UpdateTextActor(index, point1, point2);

	// Arrow
	UpdateConeActor(index, point1, point2);

	//////////////////////////////////////////////////////////////////////////

	albaEventMacro(albaEvent(this, CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void appInteractor2DSample::RemoveMeasure(int index)
{
	if (m_Renderer && index < m_CurrentAddedLine)
	{
		//////////////////////////////////////////////////////////////////////////
		// Line
		m_Renderer->RemoveActor2D(m_LineActorVector[index]);
		vtkDEL(m_LineActorVector[index]);
		m_LineActorVector.erase(m_LineActorVector.begin() + index);

		vtkDEL(m_LineMapperVector[index]);
		m_LineMapperVector.erase(m_LineMapperVector.begin() + index);

		vtkDEL(m_LineSourceVector[index]);
		m_LineSourceVector.erase(m_LineSourceVector.begin() + index);

		//////////////////////////////////////////////////////////////////////////
		// Text
		m_Renderer->RemoveActor2D(m_TextActorVector[index]);
		vtkDEL(m_TextActorVector[index]);
		m_TextActorVector.erase(m_TextActorVector.begin() + index);

		//////////////////////////////////////////////////////////////////////////
		// Arrow
		m_Renderer->RemoveActor2D(m_ConeActorVector[index]);
		vtkDEL(m_ConeActorVector[index]);
		m_ConeActorVector.erase(m_ConeActorVector.begin() + index);

		vtkDEL(m_ConeMapperVector[index]);
		m_ConeMapperVector.erase(m_ConeMapperVector.begin() + index);

		vtkDEL(m_ConeSourceVector[index]);
		m_ConeSourceVector.erase(m_ConeSourceVector.begin() + index);
		//////////////////////////////////////////////////////////////////////////

		m_Renderer->GetRenderWindow()->Render();

		m_MeasureVector.erase(m_MeasureVector.begin() + index);

		m_CurrentAddedLine--;

		albaEventMacro(albaEvent(this, CAMERA_UPDATE));
	}
}
//---------------------------------------------------------------------------
void appInteractor2DSample::RemoveAllMeasures()
{
	int nMeasures = m_MeasureVector.size();

	for (int i = nMeasures; i >= 0; i--)
	{
		RemoveMeasure(i);
	}
}
//----------------------------------------------------------------------------
void appInteractor2DSample::SelectMeasure(int index)
{
	if (m_LineActorVector.size() != 0)
	{
		// Deselect all
		for (int i = 0; i < m_LineActorVector.size(); i++)
		{
			if (m_LineActorVector[i] != NULL)
			{
				m_LineActorVector[i]->GetProperty()->SetColor(m_ColorDefault[0], m_ColorDefault[1], m_ColorDefault[2]);
				m_LineActorVector[i]->GetProperty()->SetLineWidth(1.0);
				m_LineActorVector[i]->GetProperty()->SetOpacity(m_Opacity);

				m_TextActorVector[i]->SetColor(m_ColorDefault[0], m_ColorDefault[1], m_ColorDefault[2]);
				m_TextActorVector[i]->GetProperty()->SetOpacity(m_Opacity);

				m_ConeActorVector[i]->GetProperty()->SetColor(m_ColorDefault[0], m_ColorDefault[1], m_ColorDefault[2]);
				m_ConeActorVector[i]->GetProperty()->SetOpacity(m_Opacity);
			}
		}

		if (index != -1 && m_LineActorVector[index] != NULL)
		{
			m_LineActorVector[index]->GetProperty()->SetColor(m_ColorSelection[0], m_ColorSelection[1], m_ColorSelection[2]);
			m_LineActorVector[index]->GetProperty()->SetLineWidth(3.0);

			m_TextActorVector[index]->SetColor(m_ColorSelection[0], m_ColorSelection[1], m_ColorSelection[2]);

			m_ConeActorVector[index]->GetProperty()->SetColor(m_ColorSelection[0], m_ColorSelection[1], m_ColorSelection[2]);

			m_LastSelection = index;
			m_LastEditing = -1;
		}

		if (m_Renderer)
			m_Renderer->GetRenderWindow()->Render();

		albaEventMacro(albaEvent(this, CAMERA_UPDATE));
		albaEventMacro(albaEvent(this, ID_LINE_SELECTED));
	}
}

//---------------------------------------------------------------------------
double appInteractor2DSample::GetMeasure(int index)
{
	if (index > 0 && index < m_MeasureVector.size())
		return m_MeasureVector[index];

	return -1;
}
//---------------------------------------------------------------------------
void appInteractor2DSample::GetMeasureLinePoints(int index, double *point1, double *point2)
{
	// Return line points values
	if (index >= 0 && index < m_LineSourceVector.size())
	{
		m_LineSourceVector[index]->GetPoint1(point1);
		m_LineSourceVector[index]->GetPoint2(point2);
	}
}

//----------------------------------------------------------------------------
void appInteractor2DSample::CalculateDistance()
{
	double p1_1[3], p2_1[3];

	m_EditLineSource->GetPoint1(p1_1);
	m_EditLineSource->GetPoint2(p2_1);

	m_Distance = sqrt(vtkMath::Distance2BetweenPoints(p1_1, p2_1));
	albaEventMacro(albaEvent(this, ID_RESULT_LINE, m_Distance));
}

//----------------------------------------------------------------------------
void appInteractor2DSample::InitRenderer(albaEventInteraction *e)
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
		m_ParallelView = m_Renderer->GetActiveCamera()->GetParallelProjection() != 0;
}
//----------------------------------------------------------------------------
void appInteractor2DSample::SetRendererByView(albaView * view)
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
			double p1[3];
			double p2[3];

			for (int i = 0; i < m_LineSourceVector.size(); i++)
			{
				m_LineSourceVector[i]->GetPoint1(p1);
				m_LineSourceVector[i]->GetPoint2(p2);
				AddMeasure(p1, p2);
			}
		}
	}

	m_Renderer = newRenderer;
	m_Renderer->GetRenderWindow()->Render();
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

// SET
//----------------------------------------------------------------------------
void appInteractor2DSample::SetColor(double r, double g, double b)
{
	m_ColorDefault[0] = r;
	m_ColorDefault[1] = g;
	m_ColorDefault[2] = b;
}
//----------------------------------------------------------------------------
void appInteractor2DSample::SetColorSelection(double r, double g, double b)
{
	m_ColorSelection[0] = r;
	m_ColorSelection[1] = g;
	m_ColorSelection[2] = b;
}
//----------------------------------------------------------------------------
void appInteractor2DSample::SetColorDisable(double r, double g, double b)
{
	m_ColorDisable[0] = r;
	m_ColorDisable[1] = g;
	m_ColorDisable[2] = b;
}
//----------------------------------------------------------------------------
void appInteractor2DSample::SetOpacity(double opacity)
{
	m_Opacity = opacity;

	m_EditTextActor->SetOpacity(opacity);
	m_EditConeActor->GetProperty()->SetOpacity(opacity);
}
//----------------------------------------------------------------------------
void appInteractor2DSample::SetTextSide(int side)
{
	m_TextSide = side;
}

//----------------------------------------------------------------------------
void appInteractor2DSample::ShowText(bool show)
{
	m_ShowText = show;

	for (int i = 0; i < m_TextActorVector.size(); i++)
	{
		m_TextActorVector[i]->SetVisibility(show);
	}

	m_EditTextActor->SetVisibility(show);
}
//----------------------------------------------------------------------------
void appInteractor2DSample::ShowArrow(bool show)
{
	m_ShowArrow = show;

	for (int i = 0; i < m_ConeActorVector.size(); i++)
	{
		m_ConeActorVector[i]->SetVisibility(show);
	}

	m_EditConeActor->SetVisibility(show);
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
void appInteractor2DSample::FindAndHighlightCurrentPoint(double * pointCoord)
{
	SetAction(ID_ADD_LINE);

	if (m_EditMeasureEnable)
	{
		for (int i = 0; i < m_CurrentAddedLine; i++)
		{
			double linePoint1[3];
			double linePoint2[3];
			m_LineSourceVector[i]->GetPoint1(linePoint1);
			m_LineSourceVector[i]->GetPoint2(linePoint2);

			if (DistancePointToLine(pointCoord, linePoint1, linePoint2) < LINE_UPDATE_DISTANCE)
			{
				SelectMeasure(i);

				if (vtkMath::Distance2BetweenPoints(linePoint1, pointCoord) < POINT_UPDATE_DISTANCE_2)
				{
					if (m_CurrentLineIndex != i)
					{
						SetAction(ID_EDIT_LINE);

						m_CurrentLineIndex = i;
						m_CurrentPointIndex = 1;

						m_Renderer->AddActor2D(m_EditPointActor);
						UpdatePointActor(linePoint1);
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

						m_Renderer->AddActor2D(m_EditPointActor);
						UpdatePointActor(linePoint2);
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
			m_Renderer->RemoveActor2D(m_EditPointActor);
			m_Renderer->GetRenderWindow()->Render();
			m_CurrentLineIndex = -1;
			m_CurrentPointIndex = -1;
		}
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