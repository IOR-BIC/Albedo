/*=========================================================================
Program:   Albedo
Module:    appInteractor2DTransform.cpp
Language:  C++
Date:      $Date: 2019-01-01 12:00:00 $
Version:   $Revision: 1.0.0.0 $
Authors:   Nicola Vanella
==========================================================================
Copyright (c) BIC-IOR 2020 (https://github.com/IOR-BIC)

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the above copyright notice for more information.
=========================================================================*/

#include "appInteractor2DTransform.h"

#include "albaDecl.h"
#include "albaDefines.h"
#include "albaDeviceButtonsPadMouse.h"
#include "albaDeviceButtonsPadTracker.h"
#include "albaEventInteraction.h"
#include "albaRWI.h"
#include "albaVME.h"
#include "albaVMEOutput.h"
#include "albaView.h"
#include "appUtils.h"

#include "vtkALBASmartPointer.h"
#include "vtkActor.h"
#include "vtkActor2D.h"
#include "vtkCamera.h"
#include "vtkCellArray.h"
#include "vtkDataSet.h"
#include "vtkDataSetAttributes.h"
#include "vtkFloatArray.h"
#include "vtkLine.h"
#include "vtkLineSource.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"
#include "vtkPNGReader.h"
#include "vtkPointData.h"
#include "vtkPointSource.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkPolyDataMapper2D.h"
#include "vtkPolygon.h"
#include "vtkProperty2D.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include "vtkRendererCollection.h"
#include "vtkTexture.h"

#define POINT_UPDATE_DISTANCE 2.5
#define POINT_UPDATE_DISTANCE_2 (POINT_UPDATE_DISTANCE * POINT_UPDATE_DISTANCE)

//------------------------------------------------------------------------------
albaCxxTypeMacro(appInteractor2DTransform)

//----------------------------------------------------------------------------
appInteractor2DTransform::appInteractor2DTransform()
{
	m_Coordinate = vtkCoordinate::New();
	m_Coordinate->SetCoordinateSystemToWorld();

	m_Mouse = NULL;
	m_Renderer = NULL;
	m_View = NULL;

	m_Bounds = NULL;
	m_IsInBound = false;
	m_ButtonDownInside = false;

	m_CurrentPointIndex = -1;
	m_TotalPoints = 0;

	m_DraggingLeft = false;
	m_ParallelView = false;

	m_ColorDefault[0] = 1;	m_ColorDefault[1] = 0;	m_ColorDefault[2] = 1;
	m_ColorSelection[0] = 0;	m_ColorSelection[1] = 1;	m_ColorSelection[2] = 0;

	m_Mode = 0;
	m_ScaleMode = 0;
	m_DeformationMode = 0;

	// Point tool
	vtkNEW(m_EditPointSource);
	m_EditPointSource->SetNumberOfPoints(1);

	vtkNEW(m_EditPointMapper);
	m_EditPointMapper->SetInput(m_EditPointSource->GetOutput());
	m_EditPointMapper->SetTransformCoordinate(m_Coordinate);

	vtkNEW(m_EditPointActor);
	m_EditPointActor->SetMapper(m_EditPointMapper);
	m_EditPointActor->GetProperty()->SetPointSize(10.0);
	m_EditPointActor->GetProperty()->SetColor(m_ColorSelection[0], m_ColorSelection[1], m_ColorSelection[2]);
}
//----------------------------------------------------------------------------
appInteractor2DTransform::~appInteractor2DTransform()
{
	vtkDEL(m_Coordinate);

	m_Renderer->RemoveActor2D(m_EditPointActor);

	// Edit Actor
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
	}

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

	m_LineSourceVector.clear();
	m_LineMapperVector.clear();
	m_LineActorVector.clear();
}

// MOUSE EVENTS
//----------------------------------------------------------------------------
void appInteractor2DTransform::OnLeftButtonDown(albaEventInteraction *e)
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

		if (m_CurrentPointIndex >= 0)
		{
			double pos_2d[2];
			e->Get2DPosition(pos_2d);
			double pointCoord[3];
			ScreenToWorld(pos_2d, pointCoord);
			
			EditMeasure(m_CurrentPointIndex, pointCoord);
		}
	}

	m_ButtonDownInside = m_IsInBound;
}
//----------------------------------------------------------------------------
void appInteractor2DTransform::OnLeftButtonUp(albaEventInteraction *e)
{
	m_DraggingLeft = false;
	OnButtonUp(e);

	if (m_ParallelView && m_IsInBound && m_ButtonDownInside)
	{
		double pos_2d[2];
		double pointCoord[4];
		e->Get2DPosition(pos_2d);
		ScreenToWorld(pos_2d, pointCoord);
		
		UpdatePointActor(m_CurrentPointIndex, pointCoord);
	}
}
//----------------------------------------------------------------------------
void appInteractor2DTransform::OnMove(albaEventInteraction *e)
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
		}

		if (m_IsInBound)
		{
			if (!m_DraggingLeft)
			{
				FindAndHighlightCurrentPoint(pointCoord);
			}
			else
			{
				EditMeasure(m_CurrentPointIndex, pointCoord);
			}
		}
	}
}

//----------------------------------------------------------------------------
void appInteractor2DTransform::OnEvent(albaEventBase *event)
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
void appInteractor2DTransform::UpdateLineActor(int index, double * point1, double * point2)
{
	if (index > -1)
	{
		point1[2] = 0;
		point2[2] = 0;

		m_LineSourceVector[index]->SetPoint1(point1);
		m_LineSourceVector[index]->SetPoint2(point2);
		m_LineSourceVector[index]->Update();

		m_LineMapperVector[index]->SetTransformCoordinate(m_Coordinate);
		m_LineMapperVector[index]->SetInput(m_LineSourceVector[index]->GetOutput());

		m_LineActorVector[index]->SetMapper(m_LineMapperVector[index]);
		m_LineActorVector[index]->GetProperty()->SetColor(m_ColorDefault[0], m_ColorDefault[1], m_ColorDefault[2]);
	}
}
//----------------------------------------------------------------------------
void appInteractor2DTransform::UpdatePointActor(int index, double * point)
{
	point[2] = 0;

	if (index > -1)
	{
		m_PointSourceVector[index]->SetCenter(point);
		m_PointSourceVector[index]->Update();

		m_PointMapperVector[index]->SetTransformCoordinate(m_Coordinate);
		m_PointMapperVector[index]->SetInput(m_PointSourceVector[index]->GetOutput());

		m_PointActorVector[index]->SetMapper(m_PointMapperVector[index]);
		m_PointActorVector[index]->GetProperty()->SetPointSize(10.0);
		m_PointActorVector[index]->GetProperty()->SetColor(m_ColorDefault[0], m_ColorDefault[1], m_ColorDefault[2]);
	}
	else
	{
		m_EditPointSource->SetCenter(point);
		m_EditPointSource->Update();
	}
}

// MEASURE
//----------------------------------------------------------------------------
void appInteractor2DTransform::AddMeasure(double point1[3], double point2[3], double point3[3], double point4[3])
{
	m_ResetPoint1[0] = point1[0];
	m_ResetPoint1[1] = point1[1];
	m_ResetPoint1[2] = point1[2];

	m_ResetPoint2[0] = point2[0];
	m_ResetPoint2[1] = point2[1];
	m_ResetPoint2[2] = point2[2];

	m_ResetPoint3[0] = point3[0];
	m_ResetPoint3[1] = point3[1];
	m_ResetPoint3[2] = point3[2];

	m_ResetPoint4[0] = point4[0];
	m_ResetPoint4[1] = point4[1];
	m_ResetPoint4[2] = point4[2];

	// Points
	AddPoint(point1);
	AddPoint(point2);
	AddPoint(point3);
	AddPoint(point4);

	// MidPoint
	double point5[3];
	point5[0] = (point1[0] + point3[0]) / 2;
	point5[1] = (point1[1] + point3[1]) / 2;

	m_ResetPoint5[0] = point5[0];
	m_ResetPoint5[1] = point5[1];
	m_ResetPoint5[2] = point5[2];

	AddPoint(point5);

	// Lines
	AddLine(point1, point2);
	AddLine(point2, point3);
	AddLine(point3, point4);
	AddLine(point4, point1);

	AddLine(point1, point5); // Extra Line
	AddLine(point2, point5); // Extra Line
	AddLine(point3, point5); // Extra Line
	AddLine(point4, point5); // Extra Line

	//////////////////////////////////////////////////////////////////////////

	albaEventMacro(albaEvent(this, CAMERA_UPDATE));
}

//----------------------------------------------------------------------------
void appInteractor2DTransform::AddPoint(double point[3])
{
	point[2] = 0;

	// Add Points
	m_PointSourceVector.push_back(vtkPointSource::New());
	m_PointSourceVector[m_PointSourceVector.size() - 1]->SetNumberOfPoints(1);
	m_PointMapperVector.push_back(vtkPolyDataMapper2D::New());
	m_PointActorVector.push_back(vtkActor2D::New());

	m_Renderer->AddActor2D(m_PointActorVector[m_PointActorVector.size() - 1]);

	UpdatePointActor(m_PointActorVector.size() - 1, point);

	m_TotalPoints++;
}
//----------------------------------------------------------------------------
void appInteractor2DTransform::AddLine(double point1[3], double point2[3])
{
	point1[2] = 0;
	point2[2] = 0;

	// Add Line
	m_LineSourceVector.push_back(vtkLineSource::New());
	m_LineMapperVector.push_back(vtkPolyDataMapper2D::New());
	m_LineActorVector.push_back(vtkActor2D::New());

	m_Renderer->AddActor2D(m_LineActorVector[m_LineActorVector.size() - 1]);

	UpdateLineActor(m_LineSourceVector.size() - 1, point1, point2);
}

//----------------------------------------------------------------------------
void appInteractor2DTransform::EditMeasure(int index, double point[3])
{
	if (index < 0 || index >= m_PointSourceVector.size())
		return;

	point[2] = 0;

	//////////////////////////////////////////////////////////////////////////

	if (index == 4) 
	{
		Traslation(m_CurrentPointIndex, point);
	}
	else
	{
		switch (m_Mode)
		{
		case 0: Scaling(index, point); break;
		case 1: Deformation(index, point); break;
		case 2: Rotation(index, point); break;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	if (m_Renderer)
		m_Renderer->GetRenderWindow()->Render();

	albaEventMacro(albaEvent(this, CAMERA_UPDATE));
}

//----------------------------------------------------------------------------
void appInteractor2DTransform::ResetMeasure()
{
	// Points
	//UpdatePointActor(-1, point); // Edit Point Actor
	UpdatePointActor(0, m_ResetPoint1);
	UpdatePointActor(1, m_ResetPoint2);
	UpdatePointActor(2, m_ResetPoint3);
	UpdatePointActor(3, m_ResetPoint4);
	UpdatePointActor(4, m_ResetPoint5);

	// Lines
	UpdateLineActor(0, m_ResetPoint1, m_ResetPoint2);
	UpdateLineActor(1, m_ResetPoint2, m_ResetPoint3);
	UpdateLineActor(2, m_ResetPoint3, m_ResetPoint4);
	UpdateLineActor(3, m_ResetPoint4, m_ResetPoint1);

	UpdateLineActor(4, m_ResetPoint1, m_ResetPoint5); // Extra Line
	UpdateLineActor(5, m_ResetPoint2, m_ResetPoint5); // Extra Line
	UpdateLineActor(6, m_ResetPoint3, m_ResetPoint5); // Extra Line
	UpdateLineActor(7, m_ResetPoint4, m_ResetPoint5); // Extra Line

	//////////////////////////////////////////////////////////////////////////
	if (m_Renderer)
		m_Renderer->GetRenderWindow()->Render();

	albaEventMacro(albaEvent(this, CAMERA_UPDATE));
}

//----------------------------------------------------------------------------
void appInteractor2DTransform::InitRenderer(albaEventInteraction *e)
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
void appInteractor2DTransform::SetRendererByView(albaView * view)
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
			if (ren->GetLayer() == 1) // Frontal Render
			{
				newRenderer = ren;
				break;
			}
	}

	m_Renderer = newRenderer;
	m_Renderer->GetRenderWindow()->Render();
}

//----------------------------------------------------------------------------
void appInteractor2DTransform::ScreenToWorld(double screen[2], double world[3])
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
void appInteractor2DTransform::SetColor(double r, double g, double b)
{
	m_ColorDefault[0] = r;
	m_ColorDefault[1] = g;
	m_ColorDefault[2] = b;
}
//----------------------------------------------------------------------------
void appInteractor2DTransform::SetColorSelection(double r, double g, double b)
{
	m_ColorSelection[0] = r;
	m_ColorSelection[1] = g;
	m_ColorSelection[2] = b;
}

//----------------------------------------------------------------------------
void appInteractor2DTransform::FindAndHighlightCurrentPoint(double * pointCoord)
{
	for (int i = 0; i < m_TotalPoints; i++)
	{
		double point[3];
		m_PointSourceVector[i]->GetCenter(point);

		if (vtkMath::Distance2BetweenPoints(point, pointCoord) < POINT_UPDATE_DISTANCE_2)
		{
			//SelectMeasure(i);

			if (m_CurrentPointIndex != i)
			{
				m_CurrentPointIndex = i;

				m_Renderer->AddActor2D(m_EditPointActor);
				UpdatePointActor(-1, point);
				m_Renderer->GetRenderWindow()->Render();

				// Set Mouse Cursor
				wxCursor cursor = wxCursor(wxCURSOR_SIZING);
				m_View->GetWindow()->SetCursor(cursor);
			}
			return;
		}
	}

	if (m_CurrentPointIndex >= 0)
	{
		m_Renderer->RemoveActor2D(m_EditPointActor);
		m_Renderer->GetRenderWindow()->Render();
		m_CurrentPointIndex = -1;

		// Set Mouse Cursor
		wxCursor cursor = wxCursor(wxCURSOR_ARROW);
		m_View->GetWindow()->SetCursor(cursor);
	}
}

//----------------------------------------------------------------------------
bool appInteractor2DTransform::IsInBound(double *pos)
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
void appInteractor2DTransform::Traslation(int index, double point[3])
{
	double point1[3]; double point2[3]; double point3[3]; double point4[3];
	double point5[3];

	m_PointSourceVector[0]->GetCenter(point1);
	m_PointSourceVector[1]->GetCenter(point2);
	m_PointSourceVector[2]->GetCenter(point3);
	m_PointSourceVector[3]->GetCenter(point4);
	m_PointSourceVector[4]->GetCenter(point5);

	//////////////////////////////////////////////////////////////////////////

	double newPoint1[3]; double newPoint2[3]; double newPoint3[3]; double newPoint4[3];

	newPoint1[0] = point1[0] - point5[0] + point[0];
	newPoint1[1] = point1[1] - point5[1] + point[1];

	newPoint2[0] = point2[0] - point5[0] + point[0];
	newPoint2[1] = point2[1] - point5[1] + point[1];

	newPoint3[0] = point3[0] - point5[0] + point[0];
	newPoint3[1] = point3[1] - point5[1] + point[1];

	newPoint4[0] = point4[0] - point5[0] + point[0];
	newPoint4[1] = point4[1] - point5[1] + point[1];

	// Points
	UpdatePointActor(-1, point); // Edit Point Actor
	UpdatePointActor(0, newPoint1);
	UpdatePointActor(1, newPoint2);
	UpdatePointActor(2, newPoint3);
	UpdatePointActor(3, newPoint4);
	UpdatePointActor(4, point);

	// Lines
	UpdateLineActor(0, newPoint1, newPoint2);
	UpdateLineActor(1, newPoint2, newPoint3);
	UpdateLineActor(2, newPoint3, newPoint4);
	UpdateLineActor(3, newPoint4, newPoint1);

	UpdateLineActor(4, newPoint1, point); // Extra Line
	UpdateLineActor(5, newPoint2, point); // Extra Line
	UpdateLineActor(6, newPoint3, point); // Extra Line
	UpdateLineActor(7, newPoint4, point); // Extra Line
}
//----------------------------------------------------------------------------
void appInteractor2DTransform::Deformation(int index, double point[3])
{
	double point1[3]; double point2[3]; double point3[3]; double point4[3];
	double point5[3];

	m_PointSourceVector[0]->GetCenter(point1);
	m_PointSourceVector[1]->GetCenter(point2);
	m_PointSourceVector[2]->GetCenter(point3);
	m_PointSourceVector[3]->GetCenter(point4);
	m_PointSourceVector[4]->GetCenter(point5);

	//////////////////////////////////////////////////////////////////////////

	// Points
	double oldPoint[3];

	switch (index)
	{
	case 0:
	{
		oldPoint[0] = point1[0];
		oldPoint[1] = point1[1];

		if (point[0] > point2[0]) point[0] = point2[0] - 1.0;
		if (point[1] > point4[1]) point[1] = point4[1] - 1.0;
	}
	break;
	case 1:
	{
		oldPoint[0] = point2[0];
		oldPoint[1] = point2[1];

		if (point[0] < point1[0]) point[0] = point1[0] + 1.0;
		if (point[1] > point3[1]) point[1] = point3[1] - 1.0;
	}
	break;
	case 2:
	{
		oldPoint[0] = point3[0];
		oldPoint[1] = point3[1];

		if (point[0] < point4[0]) point[0] = point4[0] + 1.0;
		if (point[1] < point2[1]) point[1] = point2[1] + 1.0;
	}
	break;
	case 3:
	{
		oldPoint[0] = point4[0];
		oldPoint[1] = point4[1];

		if (point[0] > point3[0]) point[0] = point3[0] - 1.0;
		if (point[1] < point1[1]) point[1] = point1[1] + 1.0;
	}
	break;
	}

	UpdatePointActor(-1, point); // Edit Point Actor
	UpdatePointActor(index, point);

	// Mid Point
	switch (m_DeformationMode)
	{
	case 0: //Med 2pnts
	{
		if (index == 0 || index == 2)
		{
			point5[0] = (point1[0] + point3[0]) / 2;
			point5[1] = (point1[1] + point3[1]) / 2;
		}
		else if (index == 1 || index == 3)
		{
			point5[0] = (point4[0] + point2[0]) / 2;
			point5[1] = (point4[1] + point2[1]) / 2;
		}
	}
	break;

	case 1: //Med 4pnts(Min)
	{
		point5[0] = (MAX(point1[0], point4[0]) + MIN(point2[0], point3[0])) / 2;
		point5[1] = (MAX(point1[1], point2[1]) + MIN(point4[1], point3[1])) / 2;
	}
	break;

	case 2: //Med 4pnts(Max)
	{
		point5[0] = (MIN(point1[0], point4[0]) + MAX(point2[0], point3[0])) / 2;
		point5[1] = (MIN(point1[1], point2[1]) + MAX(point4[1], point3[1])) / 2;
	}
	break;

	case 3: //Diff
	{
		point5[0] -= (oldPoint[0] - point[0]);
		point5[1] -= (oldPoint[1] - point[1]);

		if (point5[0] < MAX(point1[0], point4[0])) point5[0] = MAX(point1[0], point4[0]) + 1.0; // Left
		if (point5[0] > MIN(point2[0], point3[0])) point5[0] = MIN(point2[0], point3[0]) - 1.0; // Right
		if (point5[1] < MAX(point1[1], point2[1])) point5[1] = MAX(point1[1], point2[1]) + 1.0; // Up
		if (point5[1] > MIN(point3[1], point4[1])) point5[1] = MIN(point3[1], point4[1]) - 1.0; // Down
	}
	break;
	}

	UpdatePointActor(4, point5);

	// Lines
	UpdateLineActor(0, point1, point2);
	UpdateLineActor(1, point2, point3);
	UpdateLineActor(2, point3, point4);
	UpdateLineActor(3, point4, point1);

	UpdateLineActor(4, point1, point5); // Extra Line
	UpdateLineActor(5, point2, point5); // Extra Line
	UpdateLineActor(6, point3, point5); // Extra Line
	UpdateLineActor(7, point4, point5); // Extra Line
}
//----------------------------------------------------------------------------
void appInteractor2DTransform::Scaling(int index, double point[3])
{
	double point1[3]; double point2[3]; double point3[3]; double point4[3];
	double point5[3];

	m_PointSourceVector[0]->GetCenter(point1);
	m_PointSourceVector[1]->GetCenter(point2);
	m_PointSourceVector[2]->GetCenter(point3);
	m_PointSourceVector[3]->GetCenter(point4);
	m_PointSourceVector[4]->GetCenter(point5);

	//////////////////////////////////////////////////////////////////////////

	double oldPoint[3];

	// Points
	switch (index)
	{
	case 0:
	{
		if (m_ScaleMode == 0)
		{
			point4[0] = point[0];
			point2[1] = point[1];
		}
		else
		{
			point3[0] += (point1[0] - point[0]);
			point3[1] += (point1[1] - point[1]);

			point2[0] = point3[0];
			point2[1] = point[1];
			point4[0] = point[0];
			point4[1] = point3[1];

			UpdatePointActor(2, point3);
		}

		UpdatePointActor(1, point2);
		UpdatePointActor(3, point4);
	}
	break;
	case 1:
	{
		if (m_ScaleMode == 0)
		{
			point3[0] = point[0];
			point1[1] = point[1];
		}
		else
		{
			point4[0] += (point2[0] - point[0]);
			point4[1] += (point2[1] - point[1]);

			point1[0] = point4[0];
			point1[1] = point[1];
			point3[0] = point[0];
			point3[1] = point4[1];

			UpdatePointActor(3, point4);
		}

		UpdatePointActor(0, point1);
		UpdatePointActor(2, point3);
	}
	break;
	case 2:
	{
		if (m_ScaleMode == 0)
		{
			point2[0] = point[0];
			point4[1] = point[1];
		}
		else
		{
			point1[0] += (point3[0] - point[0]);
			point1[1] += (point3[1] - point[1]);

			point2[0] = point[0];
			point2[1] = point1[1];
			point4[0] = point1[0];
			point4[1] = point[1];

			UpdatePointActor(0, point1);
		}

		UpdatePointActor(1, point2);
		UpdatePointActor(3, point4);
	}
	break;
	case 3:
	{
		if (m_ScaleMode == 0)
		{
			point1[0] = point[0];
			point3[1] = point[1];
		}
		else
		{
			point2[0] += (point4[0] - point[0]);
			point2[1] += (point4[1] - point[1]);

			point1[0] = point[0];
			point1[1] = point2[1];
			point3[0] = point2[0];
			point3[1] = point[1];

			UpdatePointActor(1, point2);
		}

		UpdatePointActor(0, point1);
		UpdatePointActor(2, point3);
	}
	break;
	}

	UpdatePointActor(-1, point); // Edit Point Actor
	UpdatePointActor(index, point);

	// Mid Point
	if (index == 0 || index == 2)
	{
		point5[0] = (point1[0] + point3[0]) / 2;
		point5[1] = (point1[1] + point3[1]) / 2;
	}
	else if (index == 1 || index == 3)
	{
		point5[0] = (point4[0] + point2[0]) / 2;
		point5[1] = (point4[1] + point2[1]) / 2;
	}

	UpdatePointActor(4, point5);

	// Lines
	UpdateLineActor(0, point1, point2);
	UpdateLineActor(1, point2, point3);
	UpdateLineActor(2, point3, point4);
	UpdateLineActor(3, point4, point1);

	UpdateLineActor(4, point1, point5); // Extra Line
	UpdateLineActor(5, point2, point5); // Extra Line
	UpdateLineActor(6, point3, point5); // Extra Line
	UpdateLineActor(7, point4, point5); // Extra Line
}
//----------------------------------------------------------------------------
void appInteractor2DTransform::Rotation(int index, double point[3])
{
	double point1[3]; double point2[3]; double point3[3]; double point4[3];
	double point5[3];

	m_PointSourceVector[0]->GetCenter(point1);
	m_PointSourceVector[1]->GetCenter(point2);
	m_PointSourceVector[2]->GetCenter(point3);
	m_PointSourceVector[3]->GetCenter(point4);
	m_PointSourceVector[4]->GetCenter(point5);

	//////////////////////////////////////////////////////////////////////////

	// Points
	UpdatePointActor(0, point1);
	UpdatePointActor(1, point2);
	UpdatePointActor(2, point3);
	UpdatePointActor(3, point4);
	UpdatePointActor(4, point5);

	// Lines
	UpdateLineActor(0, point1, point2);
	UpdateLineActor(1, point2, point3);
	UpdateLineActor(2, point3, point4);
	UpdateLineActor(3, point4, point1);

	UpdateLineActor(4, point1, point5); // Extra Line
	UpdateLineActor(5, point2, point5); // Extra Line
	UpdateLineActor(6, point3, point5); // Extra Line
	UpdateLineActor(7, point4, point5); // Extra Line
}
