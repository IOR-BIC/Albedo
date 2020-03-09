/*=========================================================================
Program:   Albedo
Module:    appInteractor2DMeasure_DistancePoint.cpp
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

#include "appInteractor2DMeasure_DistancePoint.h"
#include "appInteractor2DMeasure.h"

#include "albaTagArray.h"
#include "albaVME.h"

#include "vtkALBATextActorMeter.h"
#include "vtkActor2D.h"
#include "vtkLineSource.h"
#include "vtkMath.h"
#include "vtkPolyDataMapper2D.h"
#include "vtkPolyDataSource.h"
#include "vtkProperty2D.h"
#include "vtkRenderWindow.h"
#include "vtkRenderer.h"
#include "vtkViewport.h"
#include "appInteractor2DMeasure_Point.h"
#include "vtkPointSource.h"

//------------------------------------------------------------------------------
albaCxxTypeMacro(appInteractor2DMeasure_DistancePoint)

//----------------------------------------------------------------------------
appInteractor2DMeasure_DistancePoint::appInteractor2DMeasure_DistancePoint() : appInteractor2DMeasure_Point()
{
	m_ShowText = true;
	m_ShowLine = true;
	m_TextSide = -2;

	m_MeasureTypeText = "DISTANCE POINT";

	m_CurrentPointIndex = NO_POINT;
	m_Group = 3;

	// EDIT ACTORS
	// Line
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
appInteractor2DMeasure_DistancePoint::~appInteractor2DMeasure_DistancePoint()
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

/// RENDERING ////////////////////////////////////////////////////////////////


/// UPDATE ///////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
void appInteractor2DMeasure_DistancePoint::UpdateLineActor(int index, double * point1, double * point2)
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
void appInteractor2DMeasure_DistancePoint::UpdateEditActors(double * point1, double * point2)
{
	// Update Edit Actors
	UpdatePointActor(-1, point1);

// 	for (int p = 1; p < m_LinePointsVector.size(); p++)
// 	{
// 		int nLines = m_LinePointsVector[p].size();
// 
// 		for (int l = 0; l < nLines; l++)
// 		{
// 			double point1[3], point2[3];
// 			m_PointSourceVector[p-1]->GetCenter(point1);
// 			m_PointSourceVector[p]->GetCenter(point2);
// 
// 			// Line
// 			UpdateLineActor(l, point1, point2);
// 		}
// 	}


	// Update Measure Label
	albaString text;
	text.Printf("Point (%.2f, %.2f)", point1[0], point1[1]);
	m_MeasureLabel = text;

	UpdateTextActor(-1, point1);
}
//----------------------------------------------------------------------------
void appInteractor2DMeasure_DistancePoint::ShowEditActors()
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
void appInteractor2DMeasure_DistancePoint::HideEditActors()
{
	// Delete Edit Actors
	m_Renderer->RemoveActor2D(m_EditLineActor);
	m_Renderer->RemoveActor2D(m_EditTextActor);
	m_ActorAdded = false;
}

//----------------------------------------------------------------------------
int appInteractor2DMeasure_DistancePoint::GetGroupByPoint(int index)
{
	for (int i = 0; i < m_LinePointsVector.size(); i++)
	{
		int nPoints = m_LinePointsVector[i].IndexPointVect.size();

		for (int p = 0; p < nPoints; p++)
		{
			if (m_LinePointsVector[i].IndexPointVect[p] == index) return i;
		}
	}

	return -1;
}
//----------------------------------------------------------------------------
int appInteractor2DMeasure_DistancePoint::GetGroupByLine(int index)
{
	for (int i = 0; i < m_LinePointsVector.size(); i++)
	{
		int nLines = m_LinePointsVector[i].IndexLineVect.size();

		for (int l = 0; l < nLines; l++)
		{
			if (m_LinePointsVector[i].IndexLineVect[l] == index) return i;
		}
	}

	return -1;
}

// SET
//----------------------------------------------------------------------------
void appInteractor2DMeasure_DistancePoint::ShowLine(bool show)
{
	m_ShowLine = show;

	for (int i = 0; i < m_LineActorVector.size(); i++)
	{
		m_LineActorVector[i]->SetVisibility(show);
	}

	m_EditLineActor->SetVisibility(show);
}

/// MEASURE //////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
void appInteractor2DMeasure_DistancePoint::AddMeasure(double *point)
{
	Superclass::AddMeasure(point);

	if (m_MeasuresCount > 1)
	{
		if (m_MeasuresCount%m_Group == 0)
		{
			Group group;

			for (int i = 0; i < m_Group; i++)
			{
				group.IndexPointVect.push_back(m_MeasuresCount - (i + 1)); // Add Index Points
			}

			for (int i = 1; i < m_Group; i++)
			{
				double point1[3], point2[3];
				m_PointSourceVector[i - 1]->GetCenter(point1);
				m_PointSourceVector[i]->GetCenter(point2);

				// Add Line
				m_LineSourceVector.push_back(vtkLineSource::New());
				m_LineMapperVector.push_back(vtkPolyDataMapper2D::New());
				m_LineActorVector.push_back(vtkActor2D::New());
				
				m_Renderer->AddActor2D(m_LineActorVector[m_LineActorVector.size() - 1]);
				
				UpdateLineActor(m_LineSourceVector.size() - 1, point1, point2);

				group.IndexLineVect.push_back(m_LineSourceVector.size() - 1);  // Add Index Line
			}
			
			m_LinePointsVector.push_back(group);
		}
	}

	m_LastPointAdded[0] = point[0];
	m_LastPointAdded[1] = point[1];
	m_LastPointAdded[2] = 0;
}
//----------------------------------------------------------------------------
void appInteractor2DMeasure_DistancePoint::EditMeasure(int index, double *point)
{
	Superclass::EditMeasure(index, point);

	if (m_MeasuresCount > 1)
	{
		int group = GetGroupByPoint(index);
		if(group >= 0)
		{
			int nLines = m_LinePointsVector[group].IndexLineVect.size();
			int nPoints = m_LinePointsVector[group].IndexPointVect.size();
			int pIndex = 0;

			for (int l = 0; l < nLines; l++)
			{
				double point1[3], point2[3];

				m_PointSourceVector[m_LinePointsVector[group].IndexPointVect[pIndex]]->GetCenter(point1);
				m_PointSourceVector[m_LinePointsVector[group].IndexPointVect[pIndex + 1]]->GetCenter(point2);
				UpdateLineActor(l, point1, point2);

				pIndex++;
			}
		}
	}

	m_LastPointEdited[0] = point[0];
	m_LastPointEdited[1] = point[1];
	m_LastPointEdited[2] = 0;
}
//----------------------------------------------------------------------------
void appInteractor2DMeasure_DistancePoint::RemoveMeasure(int index)
{
	Superclass::RemoveMeasure(index);
	
// 
// 		//////////////////////////////////////////////////////////////////////////
// 		// Line
// 		m_Renderer->RemoveActor2D(m_LineActorVector[index]);
// 		vtkDEL(m_LineActorVector[index]);
// 		m_LineActorVector.erase(m_LineActorVector.begin() + index);
// 
// 		vtkDEL(m_LineMapperVector[index]);
// 		m_LineMapperVector.erase(m_LineMapperVector.begin() + index);
// 
// 		vtkDEL(m_LineSourceVector[index]);
// 		m_LineSourceVector.erase(m_LineSourceVector.begin() + index);

}
//----------------------------------------------------------------------------
void appInteractor2DMeasure_DistancePoint::SelectMeasure(int index)
{
	Superclass::SelectMeasure(index);

// 	if (m_MeasuresCount > 0)
// 	{
// 		// Deselect all
// 		for (int i = 0; i < m_MeasuresCount; i++)
// 		{
// 			if (m_LineActorVector[i] != NULL)
// 			{
// 				// Line
// 				m_LineActorVector[i]->GetProperty()->SetColor(m_ColorDefault[0], m_ColorDefault[1], m_ColorDefault[2]);
// 				m_LineActorVector[i]->GetProperty()->SetLineWidth(1.0);
// 				m_LineActorVector[i]->GetProperty()->SetOpacity(m_Opacity);
// 
// 				// Text
// 				m_TextActorVector[i]->SetColor(m_ColorDefault[0], m_ColorDefault[1], m_ColorDefault[2]);
// 				m_TextActorVector[i]->GetProperty()->SetOpacity(m_Opacity);
// 			}
// 		}
// 
// 		if (index != -1 && m_LineActorVector[index] != NULL)
// 		{
// 			// Line
// 			m_LineActorVector[index]->GetProperty()->SetColor(m_ColorSelection[0], m_ColorSelection[1], m_ColorSelection[2]);
// 			m_LineActorVector[index]->GetProperty()->SetLineWidth(3.0);
// 
// 			// Text
// 			m_TextActorVector[index]->SetColor(m_ColorSelection[0], m_ColorSelection[1], m_ColorSelection[2]);
// 
// 			m_LastSelection = index;
// 			m_LastEditing = -1;
// 		}
// 
// 		Render();
// 
// 		albaEventMacro(albaEvent(this, ID_MEASURE_SELECTED));
// 	}
}

/// UTILS ///////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------
void appInteractor2DMeasure_DistancePoint::GetMeasureLinePoints(int index, double *point1, double *point2)
{
	// Return line points values
	if (index >= 0 && index < m_MeasuresCount)
	{
		m_LineSourceVector[index]->GetPoint1(point1);
		m_LineSourceVector[index]->GetPoint2(point2);
	}
}
