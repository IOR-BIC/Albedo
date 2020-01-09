﻿/*=========================================================================
Program:   Albedo
Module:    appInteractor2DMeasure_Point.cpp
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

#include "appInteractor2DMeasure_Point.h"
#include "appInteractor2DMeasure.h"

#include "vtkALBATextActorMeter.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkLineSource.h"
#include "vtkPointSource.h"
#include "albaDefines.h"
#include "vtkPolyDataMapper2D.h"
#include "vtkActor2D.h"
#include "vtkProperty2D.h"

//------------------------------------------------------------------------------
albaCxxTypeMacro(appInteractor2DMeasure_Point)

//----------------------------------------------------------------------------
appInteractor2DMeasure_Point::appInteractor2DMeasure_Point() : appInteractor2DMeasure()
{
	m_ShowText = true;
	m_TextSide = -2;
	m_MovingMeasure = false;
	m_EditMeasureEnable = false;

	m_MeasureTypeText = "POINT";
}
//----------------------------------------------------------------------------
appInteractor2DMeasure_Point::~appInteractor2DMeasure_Point()
{
	SetAction(ID_NO_ACTION);

	vtkDEL(m_Coordinate);

	m_Renderer->RemoveActor2D(m_EditPointActor);

	// Edit Actor
	vtkDEL(m_EditPointSource);
	vtkDEL(m_EditPointMapper);
	vtkDEL(m_EditPointActor);

	for (int i = 0; i < m_PointActorVector.size(); i++)
	{
		// Points
		m_Renderer->RemoveActor2D(m_PointActorVector[i]);
		vtkDEL(m_PointActorVector[i]);
		vtkDEL(m_PointMapperVector[i]);
		vtkDEL(m_PointSourceVector[i]);
	}

	m_Renderer->GetRenderWindow()->Render();

	m_PointSourceVector.clear();
	m_PointMapperVector.clear();
	m_PointActorVector.clear();
}

// RENDERING
//----------------------------------------------------------------------------
void appInteractor2DMeasure_Point::DrawMeasure(double * wp)
{
	// No point has yet been picked
	if (m_AddMeasurePhase_Counter == 0)
	{
		// Initialization
		m_EndMeasure = false;
		m_ActorAdded = false;
		m_Distance = 0.0;

		ShowEditActors();

		UpdateEditActors(wp);

		m_AddMeasurePhase_Counter++;
	}
	// Point has been picked and now is being dragged
	else if (m_AddMeasurePhase_Counter == 1 && m_DraggingLeft)
	{
		UpdateEditActors(wp);
	}
	// Finished dragging the point
	else if (m_AddMeasurePhase_Counter == 1)
	{
		double tmpPt[3];
		m_EditPointSource->GetCenter(tmpPt);

			if (m_CurrentMeasureIndex >= 0)
			{
				EditMeasure(m_CurrentMeasureIndex, tmpPt);
				albaEventMacro(albaEvent(this, ID_MEASURE_CHANGED, GetMeasure(m_CurrentMeasureIndex)));
			}
			else
			{
				AddMeasure(tmpPt);
				albaEventMacro(albaEvent(this, ID_MEASURE_ADDED, GetMeasure(m_MeasuresCount - 1)));
			}

		m_AddMeasurePhase_Counter++;
		m_EndMeasure = true;
	}

	if (m_EndMeasure)
	{
		m_AddMeasurePhase_Counter = 0;

		HideEditActors();
	}

	SetAction(ID_ADD_MEASURE);

	m_Renderer->GetRenderWindow()->Render();
}
//----------------------------------------------------------------------------
void appInteractor2DMeasure_Point::MoveMeasure(int index, double * pointCoord)
{
	if (index < 0)
		return;

	double point[3];

	m_PointSourceVector[index]->GetCenter(point);

	if (!m_MovingMeasure)
	{
		m_OldLineP1[0] = point[0] - m_StartMousePosition[0];
		m_OldLineP1[1] = point[1] - m_StartMousePosition[1];

		m_MovingMeasure = true;
	}

	if (m_DraggingLeft)
	{
		DisableMeasure(index);

		ShowEditActors();

		double tmp_pos[3];

		tmp_pos[0] = pointCoord[0] + m_OldLineP1[0];
		tmp_pos[1] = pointCoord[1] + m_OldLineP1[1];
		tmp_pos[2] = 0.0;

		UpdateEditActors(tmp_pos);
	}
	else if (m_EditPointSource)
	{
		HideEditActors();

		double tmpPt[3];
		m_EditPointSource->GetCenter(tmpPt);

		EditMeasure(index, tmpPt);
		albaEventMacro(albaEvent(this, ID_MEASURE_CHANGED, m_Distance)); // TODO

		m_ActorAdded = false;
	}

	m_Renderer->GetRenderWindow()->Render();
}

//----------------------------------------------------------------------------
void appInteractor2DMeasure_Point::UpdatePointActor(int index, double * point)
{
	if (index > -1)
	{
		m_PointSourceVector[index]->SetCenter(point);
		m_PointSourceVector[index]->Update();

		m_PointMapperVector[index]->SetTransformCoordinate(m_Coordinate);
		m_PointMapperVector[index]->SetInput(m_PointSourceVector[index]->GetOutput());

		m_PointActorVector[index]->SetMapper(m_PointMapperVector[index]);
		m_PointActorVector[index]->GetProperty()->SetPointSize(10.0);

		if (m_LastSelection == index)
			m_PointActorVector[index]->GetProperty()->SetColor(m_ColorSelection[0], m_ColorSelection[1], m_ColorSelection[2]);
		else
			m_PointActorVector[index]->GetProperty()->SetColor(m_ColorDefault[0], m_ColorDefault[1], m_ColorDefault[2]);

		m_PointActorVector[index]->GetProperty()->SetOpacity(m_Opacity);
	}
	else
	{
		m_EditPointSource->SetCenter(point);
		m_EditPointSource->Update();
	}
}
//----------------------------------------------------------------------------
void appInteractor2DMeasure_Point::UpdateEditActors(double * point1, double * point2)
{
	// Update Edit Actors
	UpdatePointActor(-1, point1);
	UpdateTextActor(-1, point1);
}
//----------------------------------------------------------------------------
void appInteractor2DMeasure_Point::ShowEditActors()
{
	if (!m_ActorAdded)
	{
		// Add Edit Actors
		m_Renderer->AddActor2D(m_EditPointActor);
		m_Renderer->AddActor2D(m_EditTextActor);
		m_ActorAdded = true;
	}
}
//----------------------------------------------------------------------------
void appInteractor2DMeasure_Point::HideEditActors()
{
	// Delete Edit Actors
	m_Renderer->RemoveActor2D(m_EditPointActor);
	m_Renderer->RemoveActor2D(m_EditTextActor);
	m_ActorAdded = false;
}
//----------------------------------------------------------------------------
void appInteractor2DMeasure_Point::DisableMeasure(int index)
{
	double disableOpacity = 0.3;

	// Point
	m_PointActorVector[index]->GetProperty()->SetColor(m_ColorDisable[0], m_ColorDisable[1], m_ColorDisable[2]);
	m_PointActorVector[index]->GetProperty()->SetOpacity(disableOpacity);

	// Text
	m_TextActorVector[m_CurrentMeasureIndex]->SetColor(m_ColorDisable[0], m_ColorDisable[1], m_ColorDisable[2]);
	m_TextActorVector[m_CurrentMeasureIndex]->SetOpacity(disableOpacity);
}

// MEASURE
//----------------------------------------------------------------------------
void appInteractor2DMeasure_Point::AddMeasure(double *point)
{
	point[2] = 0;

	// Update Actors
	UpdateEditActors(point);

	//////////////////////////////////////////////////////////////////////////
	// Add Measure
	albaString text;
	text.Printf("Point n. %d ", m_MeasuresCount + 1);
	m_MeasureVector.push_back(text);

	//////////////////////////////////////////////////////////////////////////
	// Add Point
	m_PointSourceVector.push_back(vtkPointSource::New());
	m_PointMapperVector.push_back(vtkPolyDataMapper2D::New());
	m_PointActorVector.push_back(vtkActor2D::New());

	m_Renderer->AddActor2D(m_PointActorVector[m_PointActorVector.size() - 1]);

	UpdatePointActor(m_PointActorVector.size() - 1, point);

	//////////////////////////////////////////////////////////////////////////
	// Add Text
	m_TextActorVector.push_back(vtkALBATextActorMeter::New());
	m_Renderer->AddActor2D(m_TextActorVector[m_TextActorVector.size() - 1]);

	UpdateTextActor(m_TextActorVector.size() - 1, point);

	//////////////////////////////////////////////////////////////////////////

	m_MeasuresCount++;

	albaEventMacro(albaEvent(this, CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void appInteractor2DMeasure_Point::EditMeasure(int index, double *point)
{
	if (index < 0 || index >= m_MeasuresCount)
		return;

	m_MovingMeasure = true;

	point[2] = 0;

	m_LastEditing = index;

	//////////////////////////////////////////////////////////////////////////
	// Update Measure
	albaString text;
	text.Printf("Point n. %d ", m_MeasuresCount + 1);
	m_MeasureVector[index] = text;

	// Update Actors
	UpdateEditActors(point);
	// Point
	UpdatePointActor(index, point);
	// Text
	UpdateTextActor(index, point);

	//////////////////////////////////////////////////////////////////////////

	albaEventMacro(albaEvent(this, CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void appInteractor2DMeasure_Point::RemoveMeasure(int index)
{
	if (m_Renderer && index < m_MeasuresCount)
	{
		//////////////////////////////////////////////////////////////////////////
		// Measure
		m_MeasureVector.erase(m_MeasureVector.begin() + index);

		//////////////////////////////////////////////////////////////////////////
		// Point
		m_Renderer->RemoveActor2D(m_PointActorVector[index]);
		vtkDEL(m_PointActorVector[index]);
		m_PointActorVector.erase(m_PointActorVector.begin() + index);

		vtkDEL(m_PointMapperVector[index]);
		m_PointMapperVector.erase(m_PointMapperVector.begin() + index);

		vtkDEL(m_PointSourceVector[index]);
		m_PointSourceVector.erase(m_PointSourceVector.begin() + index);

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
void appInteractor2DMeasure_Point::SelectMeasure(int index)
{
	if (m_MeasuresCount > 0)
	{
		// Deselect all
		for (int i = 0; i < m_MeasuresCount; i++)
		{
			if (m_PointActorVector[i] != NULL)
			{
				m_PointActorVector[i]->GetProperty()->SetColor(m_ColorDefault[0], m_ColorDefault[1], m_ColorDefault[2]);
				m_PointActorVector[i]->GetProperty()->SetLineWidth(1.0);
				m_PointActorVector[i]->GetProperty()->SetOpacity(m_Opacity);

				m_TextActorVector[i]->SetColor(m_ColorDefault[0], m_ColorDefault[1], m_ColorDefault[2]);
				m_TextActorVector[i]->GetProperty()->SetOpacity(m_Opacity);
			}
		}

		if (index != -1 && m_PointActorVector[index] != NULL)
		{
			m_PointActorVector[index]->GetProperty()->SetColor(m_ColorSelection[0], m_ColorSelection[1], m_ColorSelection[2]);
			m_PointActorVector[index]->GetProperty()->SetLineWidth(3.0);

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

// UTILS
//---------------------------------------------------------------------------
void appInteractor2DMeasure_Point::GetMeasurePoint(int index, double *point)
{
	// Return point value
	if (index >= 0 && index < m_MeasuresCount)
	{
		m_PointSourceVector[index]->GetCenter(point);
	}
}
//----------------------------------------------------------------------------
void appInteractor2DMeasure_Point::FindAndHighlightCurrentPoint(double * pointCoord)
{
	SetAction(ID_ADD_MEASURE);

	if (m_EditMeasureEnable)
	{
		for (int i = 0; i < m_MeasuresCount; i++)
		{
			double tmpPoint[3];

			m_PointSourceVector[i]->GetCenter(tmpPoint);

			if (DistanceBetweenPoints(pointCoord, tmpPoint) < MIN_UPDATE_DISTANCE)
			{
				SelectMeasure(i);

				m_CurrentMeasureIndex = i;

				m_Renderer->AddActor2D(m_EditPointActor);
				UpdatePointActor(-1, tmpPoint);
				m_Renderer->GetRenderWindow()->Render();

				SetAction(ID_MOVE_MEASURE);
			}
		}

		if (m_CurrentMeasureIndex >= 0)
		{
			m_Renderer->RemoveActor2D(m_EditPointActor);
			m_Renderer->GetRenderWindow()->Render();
			m_CurrentMeasureIndex = -1;
		}
	}
}
