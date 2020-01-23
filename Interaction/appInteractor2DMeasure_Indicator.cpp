/*=========================================================================
Program:   Albedo
Module:    appInteractor2DMeasure_Indicator.cpp
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

#include "appInteractor2DMeasure_Indicator.h"
#include "appInteractor2DMeasure_Distance.h"
#include "appInteractor2DMeasure.h"

#include "vtkALBATextActorMeter.h"
#include "vtkActor2D.h"
#include "vtkConeSource.h"
#include "vtkLineSource.h"
#include "vtkMath.h"
#include "vtkPointSource.h"
#include "vtkPolyDataMapper2D.h"
#include "vtkPolyDataSource.h"
#include "vtkProperty2D.h"
#include "vtkRenderWindow.h"
#include "vtkRenderer.h"
#include "vtkViewport.h"

//------------------------------------------------------------------------------
albaCxxTypeMacro(appInteractor2DMeasure_Indicator)

//----------------------------------------------------------------------------
appInteractor2DMeasure_Indicator::appInteractor2DMeasure_Indicator() : appInteractor2DMeasure_Distance()
{
	m_ShowArrow = true;
	m_ShowText = true;
	m_ShowLine = true;
	m_TextSide = 1;

	m_MeasureTypeText = "INDICATOR";

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
}
//----------------------------------------------------------------------------
appInteractor2DMeasure_Indicator::~appInteractor2DMeasure_Indicator()
{
	m_Renderer->RemoveActor2D(m_EditConeActor);

	// Edit Actor
	vtkDEL(m_EditConeSource);
	vtkDEL(m_EditConeMapper);
	vtkDEL(m_EditConeActor);

	for (int i = 0; i < m_LineActorVector.size(); i++)
	{
		// Arrows
		m_Renderer->RemoveActor2D(m_ConeActorVector[i]);
		vtkDEL(m_ConeActorVector[i]);
		vtkDEL(m_ConeMapperVector[i]);
		vtkDEL(m_ConeSourceVector[i]);
	}

	m_ConeSourceVector.clear();
	m_ConeMapperVector.clear();
	m_ConeActorVector.clear();
}

// RENDERING
//----------------------------------------------------------------------------
void appInteractor2DMeasure_Indicator::UpdateConeActor(int index, double * point1, double * point2)
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
void appInteractor2DMeasure_Indicator::UpdatePointActor(double * point)
{
	if (m_CurrentPointIndex == POINT_2)
	{
		double colorConeSelection[3] = { 0.1 + m_ColorSelection[0], 0.1 + m_ColorSelection[1], 0.1 + m_ColorSelection[2] };
		m_EditConeActor->GetProperty()->SetColor(colorConeSelection[0], colorConeSelection[1], colorConeSelection[2]);
		m_ConeActorVector[m_CurrentMeasureIndex]->GetProperty()->SetColor(colorConeSelection[0], colorConeSelection[1], colorConeSelection[2]);
	}
	else
	{
		m_EditPointSource->SetCenter(point);
		m_EditPointSource->Update();
	}
}
//----------------------------------------------------------------------------
void appInteractor2DMeasure_Indicator::UpdateEditActors(double * point1, double * point2)
{
	// Update Edit Actors
	UpdateLineActor(-1, point1, point2);
	UpdateConeActor(-1, point1, point2);
	UpdateTextActor(-1, point1, point2);
}
//----------------------------------------------------------------------------
void appInteractor2DMeasure_Indicator::ShowEditActors()
{
	if (!m_ActorAdded)
	{
		// Add Edit Actors
		m_Renderer->AddActor2D(m_EditLineActor);
		m_Renderer->AddActor2D(m_EditConeActor);
		m_Renderer->AddActor2D(m_EditTextActor);
		m_ActorAdded = true;
	}
}
//----------------------------------------------------------------------------
void appInteractor2DMeasure_Indicator::HideEditActors()
{
	// Delete Edit Actors
	m_Renderer->RemoveActor2D(m_EditLineActor);
	m_Renderer->RemoveActor2D(m_EditConeActor);
	m_Renderer->RemoveActor2D(m_EditTextActor);
	m_ActorAdded = false;
}
//----------------------------------------------------------------------------
void appInteractor2DMeasure_Indicator::DisableMeasure(int index)
{
	double disableOpacity = 0.3;

	// Line
	m_LineActorVector[m_CurrentMeasureIndex]->GetProperty()->SetColor(m_ColorDisable[0], m_ColorDisable[1], m_ColorDisable[2]);
	m_LineActorVector[m_CurrentMeasureIndex]->GetProperty()->SetOpacity(disableOpacity);
	// Arrow
	m_ConeActorVector[m_CurrentMeasureIndex]->GetProperty()->SetColor(m_ColorDisable[0], m_ColorDisable[1], m_ColorDisable[2]);
	m_ConeActorVector[m_CurrentMeasureIndex]->GetProperty()->SetOpacity(disableOpacity);
	// Text
	m_TextActorVector[m_CurrentMeasureIndex]->SetColor(m_ColorDisable[0], m_ColorDisable[1], m_ColorDisable[2]);
	m_TextActorVector[m_CurrentMeasureIndex]->SetOpacity(disableOpacity);
}

// SET
//----------------------------------------------------------------------------
void appInteractor2DMeasure_Indicator::ShowArrow(bool show)
{
	m_ShowArrow = show;

	for (int i = 0; i < m_ConeActorVector.size(); i++)
	{
		m_ConeActorVector[i]->SetVisibility(show);
	}

	m_EditConeActor->SetVisibility(show);
}

// MEASURE
//----------------------------------------------------------------------------
void appInteractor2DMeasure_Indicator::AddMeasure(double *point1, double *point2)
{
	point1[2] = point2[2] = 0;

	UpdateEditActors(point1, point2);

	//////////////////////////////////////////////////////////////////////////
	// Add Measure
	albaString text;
	text.Printf("Indicator %.2f mm", DistanceBetweenPoints(point1, point2));
	m_MeasureVector.push_back(text);

	//////////////////////////////////////////////////////////////////////////
	// Add Line
	m_LineSourceVector.push_back(vtkLineSource::New());
	m_LineMapperVector.push_back(vtkPolyDataMapper2D::New());
	m_LineActorVector.push_back(vtkActor2D::New());

	m_Renderer->AddActor2D(m_LineActorVector[m_LineActorVector.size() - 1]);

	UpdateLineActor(m_LineSourceVector.size() - 1, point1, point2);

	//////////////////////////////////////////////////////////////////////////
	// Add Arrow
	m_ConeSourceVector.push_back(vtkConeSource::New());
	m_ConeMapperVector.push_back(vtkPolyDataMapper2D::New());
	m_ConeActorVector.push_back(vtkActor2D::New());

	m_Renderer->AddActor2D(m_ConeActorVector[m_ConeActorVector.size() - 1]);

	UpdateConeActor(m_ConeSourceVector.size() - 1, point1, point2);

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
void appInteractor2DMeasure_Indicator::EditMeasure(int index, double *point1, double *point2)
{
	if (index < 0 || index >= m_MeasuresCount)
		return;

	m_MovingMeasure = true;

	point1[2] = point2[2] = 0;

	m_LastEditing = index;

	//////////////////////////////////////////////////////////////////////////
	// Update Measure
	albaString text;
	text.Printf("Indicator %.2f mm", DistanceBetweenPoints(point1, point2));
	m_MeasureVector[index] = text;

	// Update Actors
	UpdateEditActors(point1, point2);
	// Line
	UpdateLineActor(index, point1, point2);
	// Arrow
	UpdateConeActor(index, point1, point2);
	// Text
	UpdateTextActor(index, point1, point2);

	//////////////////////////////////////////////////////////////////////////

	albaEventMacro(albaEvent(this, CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void appInteractor2DMeasure_Indicator::RemoveMeasure(int index)
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
		// Arrow
		m_Renderer->RemoveActor2D(m_ConeActorVector[index]);
		vtkDEL(m_ConeActorVector[index]);
		m_ConeActorVector.erase(m_ConeActorVector.begin() + index);

		vtkDEL(m_ConeMapperVector[index]);
		m_ConeMapperVector.erase(m_ConeMapperVector.begin() + index);

		vtkDEL(m_ConeSourceVector[index]);
		m_ConeSourceVector.erase(m_ConeSourceVector.begin() + index);

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
void appInteractor2DMeasure_Indicator::SelectMeasure(int index)
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

				// Arrow
				m_ConeActorVector[i]->GetProperty()->SetColor(m_ColorDefault[0], m_ColorDefault[1], m_ColorDefault[2]);
				m_ConeActorVector[i]->GetProperty()->SetLineWidth(1.0);
				m_ConeActorVector[i]->GetProperty()->SetOpacity(m_Opacity);

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

			// Arrow
			m_ConeActorVector[index]->GetProperty()->SetColor(m_ColorSelection[0], m_ColorSelection[1], m_ColorSelection[2]);
			m_ConeActorVector[index]->GetProperty()->SetLineWidth(3.0);

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
