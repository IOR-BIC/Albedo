/*=========================================================================
Program:   Albedo
Module:    appInteractor2DMeasure_Distance.cpp
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

#include "appInteractor2DMeasure_Distance.h"
#include "appInteractor2DMeasure.h"
#include "vtkActor2D.h"
#include "vtkLineSource.h"
#include "vtkMath.h"
#include "vtkPolyDataMapper2D.h"
#include "vtkPolyDataSource.h"
#include "vtkProperty2D.h"
#include "vtkRenderWindow.h"
#include "vtkRenderer.h"
#include "vtkViewport.h"
#include "vtkALBATextActorMeter.h"

//------------------------------------------------------------------------------
albaCxxTypeMacro(appInteractor2DMeasure_Distance)

//----------------------------------------------------------------------------
appInteractor2DMeasure_Distance::appInteractor2DMeasure_Distance() : appInteractor2DMeasure()
{
	m_ShowText = true;
	m_ShowLine = true;
	m_TextSide = 1;

	m_MeasureTypeText = "DISTANCE";

	m_CurrentPointIndex = NO_POINT;

	// EDIT ACTORS
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
}
//----------------------------------------------------------------------------
appInteractor2DMeasure_Distance::~appInteractor2DMeasure_Distance()
{
	m_Renderer->RemoveActor2D(m_EditLineActor);

	// Edit Actor
	vtkDEL(m_EditLineSource);
	vtkDEL(m_EditLineMapper);
	vtkDEL(m_EditLineActor);

	for (int i = 0; i < m_LineActorVector.size(); i++)
	{
		// Lines
		m_Renderer->RemoveActor2D(m_LineActorVector[i]);
		vtkDEL(m_LineActorVector[i]);
		vtkDEL(m_LineMapperVector[i]);
		vtkDEL(m_LineSourceVector[i]);
	}

	m_LineSourceVector.clear();
	m_LineMapperVector.clear();
	m_LineActorVector.clear();
}

// RENDERING
//----------------------------------------------------------------------------
void appInteractor2DMeasure_Distance::DrawMeasure(double * wp)
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
		if (m_CurrentMeasureIndex >= 0)
		{
			DisableMeasure(m_CurrentMeasureIndex);

			if (m_CurrentPointIndex == POINT_1)
			{
				double tmp_p2[3];
				m_LineSourceVector[m_CurrentMeasureIndex]->GetPoint2(tmp_p2);
				m_EditLineSource->SetPoint1(tmp_p2);
			}
			else if (m_CurrentPointIndex == POINT_2)
			{
				double tmp_p1[3];
				m_LineSourceVector[m_CurrentMeasureIndex]->GetPoint1(tmp_p1);
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
		ShowEditActors();

		double editPoint1[3];
		m_EditLineSource->GetPoint1(editPoint1);
		//UpdateLineActor(-1, editPoint1, wp);

		m_Distance = DistanceBetweenPoints(editPoint1, wp);

		//UpdateTextActor(-1, editPoint1, wp);
		UpdateEditActors(editPoint1, wp);
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

		HideEditActors();

		if (m_Distance > 0.0)
		{
			double tmpPt1[3], tmpPt2[3];
			m_EditLineSource->GetPoint1(tmpPt1);
			m_EditLineSource->GetPoint2(tmpPt2);

			if (m_CurrentMeasureIndex >= 0)
			{
				EditMeasure(m_CurrentMeasureIndex, tmpPt1, tmpPt2);
				albaEventMacro(albaEvent(this, ID_MEASURE_CHANGED, GetMeasure(m_CurrentMeasureIndex)));
			}
			else
			{
				AddMeasure(tmpPt1, tmpPt2);
				albaEventMacro(albaEvent(this, ID_MEASURE_ADDED, GetMeasure(m_MeasuresCount - 1)));
			}
		}
		else
		{
			SelectMeasure(m_LastSelection);
		}
	}

	SetAction(ID_ADD_MEASURE);

	m_Renderer->GetRenderWindow()->Render();
}
//----------------------------------------------------------------------------
void appInteractor2DMeasure_Distance::MoveMeasure(int index, double * point)
{
	if (index < 0)
		return;

	double linePoint1[3];
	double linePoint2[3];

	m_LineSourceVector[index]->GetPoint1(linePoint1);
	m_LineSourceVector[index]->GetPoint2(linePoint2);

	if (!m_MovingMeasure)
	{
		m_OldLineP1[0] = linePoint1[0] - m_StartMousePosition[0];
		m_OldLineP1[1] = linePoint1[1] - m_StartMousePosition[1];
		m_OldLineP2[0] = linePoint2[0] - m_StartMousePosition[0];
		m_OldLineP2[1] = linePoint2[1] - m_StartMousePosition[1];

		m_MovingMeasure = true;
	}

	if (m_DraggingLeft)
	{
		ShowEditActors();

		DisableMeasure(index);

		// Initialization
		m_Distance = 0.0;

		double tmp_pos1[3];
		double tmp_pos2[3];

		tmp_pos1[0] = point[0] + m_OldLineP1[0];
		tmp_pos1[1] = point[1] + m_OldLineP1[1];
		tmp_pos1[2] = 0.0;

		tmp_pos2[0] = point[0] + m_OldLineP2[0];
		tmp_pos2[1] = point[1] + m_OldLineP2[1];
		tmp_pos2[2] = 0.0;
		
		m_Distance = DistanceBetweenPoints(tmp_pos1, tmp_pos2);

		UpdateEditActors(tmp_pos1, tmp_pos2);
	}
	else if (m_EditLineSource)
	{
		HideEditActors();

		double tmpPt[3], tmpPt2[3];
		m_EditLineSource->GetPoint1(tmpPt);
		m_EditLineSource->GetPoint2(tmpPt2);

		EditMeasure(index, tmpPt, tmpPt2);
		albaEventMacro(albaEvent(this, ID_MEASURE_CHANGED, m_Distance));

		m_ActorAdded = false;
	}

	m_Renderer->GetRenderWindow()->Render();
}

//----------------------------------------------------------------------------
void appInteractor2DMeasure_Distance::UpdateLineActor(int index, double * point1, double * point2)
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
		m_LineActorVector[index]->SetVisibility(m_ShowLine);
	}
	else
	{
		m_EditLineSource->SetPoint1(point1);
		m_EditLineSource->SetPoint2(point2);
		m_EditLineSource->Update();
		m_EditLineActor->SetVisibility(m_ShowLine);

		m_EditLineMapper->SetTransformCoordinate(m_Coordinate);
	}
}
//----------------------------------------------------------------------------
void appInteractor2DMeasure_Distance::UpdateTextActor(int index, double * point1, double * point2)
{
	double text_pos[3];
	text_pos[0] = (point1[0] + point2[0]) / 2;
	text_pos[1] = (point1[1] + point2[1]) / 2;
	text_pos[2] = (point1[2] + point2[2]) / 2;

	Superclass::UpdateTextActor(index, text_pos);
}

void appInteractor2DMeasure_Distance::DisableMeasure(int index)
{
	// Old Line
	m_LineActorVector[m_CurrentMeasureIndex]->GetProperty()->SetColor(m_ColorDisable[0], m_ColorDisable[1], m_ColorDisable[2]);
	// Old Text
	m_TextActorVector[m_CurrentMeasureIndex]->SetColor(m_ColorDisable[0], m_ColorDisable[1], m_ColorDisable[2]);
}

//----------------------------------------------------------------------------
void appInteractor2DMeasure_Distance::ShowLine(bool show)
{
	m_ShowLine = show;

	for (int i = 0; i < m_LineActorVector.size(); i++)
	{
		m_LineActorVector[i]->SetVisibility(show);
	}

	m_EditLineActor->SetVisibility(show);
}

// MEASURE
//----------------------------------------------------------------------------
void appInteractor2DMeasure_Distance::AddMeasure(double *point1, double *point2)
{
	point1[2] = point2[2] = 0;

	// Update Edit Actors
	UpdateLineActor(-1, point1, point2);
	UpdateTextActor(-1, point1, point2);

	//////////////////////////////////////////////////////////////////////////
	// Add Measure
	albaString text;
	text.Printf("Distance %.2f mm", DistanceBetweenPoints(point1, point2));
	m_MeasureVector.push_back(text);

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

	m_MeasuresCount++;

	albaEventMacro(albaEvent(this, CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void appInteractor2DMeasure_Distance::EditMeasure(int index, double *point1, double *point2)
{
	if (index < 0 || index >= m_MeasuresCount)
		return;

	m_MovingMeasure = true;

	point1[2] = point2[2] = 0;

	m_LastEditing = index;

	//////////////////////////////////////////////////////////////////////////

	// Update Measure
	albaString text;
	text.Printf("Distance %.2f mm", DistanceBetweenPoints(point1, point2));
	m_MeasureVector[index] = text;

	// Update Edit Actors
	UpdateLineActor(-1, point1, point2);
	UpdateTextActor(-1, point1, point2);

	// Line
	UpdateLineActor(index, point1, point2);

	// Text
	UpdateTextActor(index, point1, point2);

	//////////////////////////////////////////////////////////////////////////

	albaEventMacro(albaEvent(this, CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void appInteractor2DMeasure_Distance::RemoveMeasure(int index)
{
	if (m_Renderer && index < m_MeasuresCount)
	{
		//////////////////////////////////////////////////////////////////////////
		// Measure
		m_MeasureVector.erase(m_MeasureVector.begin() + index);

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

		m_Renderer->GetRenderWindow()->Render();

		m_MeasuresCount--;

		albaEventMacro(albaEvent(this, CAMERA_UPDATE));
	}
}
//----------------------------------------------------------------------------
void appInteractor2DMeasure_Distance::SelectMeasure(int index)
{
	if (m_MeasuresCount > 0)
	{
		// Deselect all
		for (int i = 0; i < m_MeasuresCount; i++)
		{
			if (m_LineActorVector[i] != NULL)
			{
				// Line
				m_LineActorVector[i]->GetProperty()->SetColor(m_ColorDefault[0], m_ColorDefault[1], m_ColorDefault[2]);
				m_LineActorVector[i]->GetProperty()->SetLineWidth(1.0);
				m_LineActorVector[i]->GetProperty()->SetOpacity(m_Opacity);

				// Text
				m_TextActorVector[i]->SetColor(m_ColorDefault[0], m_ColorDefault[1], m_ColorDefault[2]);
				m_TextActorVector[i]->GetProperty()->SetOpacity(m_Opacity);
			}
		}

		if (index != -1 && m_LineActorVector[index] != NULL)
		{
			// Line
			m_LineActorVector[index]->GetProperty()->SetColor(m_ColorSelection[0], m_ColorSelection[1], m_ColorSelection[2]);
			m_LineActorVector[index]->GetProperty()->SetLineWidth(3.0);

			// Text
			m_TextActorVector[index]->SetColor(m_ColorSelection[0], m_ColorSelection[1], m_ColorSelection[2]);

			m_LastSelection = index;
			m_LastEditing = -1;
		}

		if (m_Renderer)
			m_Renderer->GetRenderWindow()->Render();

		albaEventMacro(albaEvent(this, CAMERA_UPDATE));
		albaEventMacro(albaEvent(this, ID_MEASURE_SELECTED));
	}
}

//---------------------------------------------------------------------------
void appInteractor2DMeasure_Distance::GetMeasureLinePoints(int index, double *point1, double *point2)
{
	// Return line points values
	if (index >= 0 && index < m_MeasuresCount)
	{
		m_LineSourceVector[index]->GetPoint1(point1);
		m_LineSourceVector[index]->GetPoint2(point2);
	}
}

//----------------------------------------------------------------------------
void appInteractor2DMeasure_Distance::FindAndHighlightCurrentPoint(double * point)
{
	SetAction(ID_ADD_MEASURE);

	if (m_EditMeasureEnable)
	{
		for (int i = 0; i < m_MeasuresCount; i++)
		{
			double linePoint1[3];
			double linePoint2[3];
			m_LineSourceVector[i]->GetPoint1(linePoint1);
			m_LineSourceVector[i]->GetPoint2(linePoint2);

			if (DistancePointToLine(point, linePoint1, linePoint2) < LINE_UPDATE_DISTANCE)
			{
				SelectMeasure(i);

				if (vtkMath::Distance2BetweenPoints(linePoint1, point) < POINT_UPDATE_DISTANCE_2)
				{
					if (m_CurrentMeasureIndex != i)
					{
						SetAction(ID_EDIT_MEASURE);

						m_CurrentMeasureIndex = i;
						m_CurrentPointIndex = POINT_1;

						m_Renderer->AddActor2D(m_EditPointActor);
						UpdatePointActor(linePoint1);
						m_Renderer->GetRenderWindow()->Render();
					}
					return;
				}

				if (vtkMath::Distance2BetweenPoints(linePoint2, point) < POINT_UPDATE_DISTANCE_2)
				{
					if (m_CurrentMeasureIndex != i)
					{
						SetAction(ID_EDIT_MEASURE);

						m_CurrentMeasureIndex = i;
						m_CurrentPointIndex = POINT_2;

						m_Renderer->AddActor2D(m_EditPointActor);
						UpdatePointActor(linePoint2);
						m_Renderer->GetRenderWindow()->Render();
					}
					return;
				}

				m_CurrentMeasureIndex = i;
				SetAction(ID_MOVE_MEASURE);
				return;
			}
		}

		if (m_CurrentMeasureIndex >= 0)
		{
			m_Renderer->RemoveActor2D(m_EditPointActor);
			m_Renderer->GetRenderWindow()->Render();
			m_CurrentMeasureIndex = -1;
			m_CurrentPointIndex = NO_POINT;
		}
	}
}

//----------------------------------------------------------------------------
void appInteractor2DMeasure_Distance::ShowEditActors()
{
	if (!m_ActorAdded)
	{
		// Add Edit Actors
		m_Renderer->AddActor2D(m_EditLineActor);
		m_Renderer->AddActor2D(m_EditTextActor);
		m_ActorAdded = true;
	}
}
//----------------------------------------------------------------------------
void appInteractor2DMeasure_Distance::HideEditActors()
{
	// Delete Edit Actors
	m_Renderer->RemoveActor2D(m_EditLineActor);
	m_Renderer->RemoveActor2D(m_EditTextActor);
	m_ActorAdded = false;
}
//----------------------------------------------------------------------------
void appInteractor2DMeasure_Distance::UpdateEditActors(double * point1, double * point2)
{
	// Update Edit Actors
	UpdateLineActor(-1, point1, point2);
	UpdateTextActor(-1, point1, point2);
}