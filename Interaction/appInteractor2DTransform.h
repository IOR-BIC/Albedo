/*=========================================================================
Program:   Albedo
Module:    appInteractor2DTransform.h
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

#ifndef __appInteractor2DTransform_h
#define __appInteractor2DTransform_h

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "appInteractionDefines.h"

#include "albaEvent.h"
#include "albaInteractorPER.h"
#include "vtkPolyData.h"
#include "vtkActor.h"
#include "vtkPolyDataMapper.h"

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
class vtkRendererCollection;
class vtkPointSource;

// Class Name: appInteractor2DTransform
class APP_INTERACTION_EXPORT appInteractor2DTransform : public albaInteractorPER
{
public:

	albaTypeMacro(appInteractor2DTransform, albaInteractorPER);

	virtual void OnEvent(albaEventBase *event);

	/// Measures

	/** Add Measure*/
	void AddMeasure(double point1[3], double point2[3], double point3[3], double point4[3]);
	
	/** Edit Measure*/
	void EditMeasure(int index, double point[3]);

	/** Move Measure*/
	void Traslation(int index, double point[3]);

	/** Reset Measure*/
	void ResetMeasure();

	/*Set Renderer by View needed*/
	void SetRendererByView(albaView * view);
	
	/** Set Color Default*/
	void SetColor(double r, double g, double b);
	/** Set Color Selection*/
	void SetColorSelection(double r, double g, double b);

	void SetMode(int mode) { m_Mode = mode; };
	void SetScaleMode(int mode) { m_ScaleMode = mode; };
	void SetDeformationMode(int mode) { m_DeformationMode = mode; };

protected:

	appInteractor2DTransform();
	virtual ~appInteractor2DTransform();

	// Mouse Events
	virtual void OnLeftButtonDown(albaEventInteraction *e);
	virtual void OnLeftButtonUp(albaEventInteraction *e);
	virtual void OnMove(albaEventInteraction *e);

	void InitRenderer(albaEventInteraction *e);

	void ScreenToWorld(double screen[2], double world[3]);

	/** Add Point*/
	void AddPoint(double point[3]);
	/** Add Line*/
	void AddLine(double point1[3], double point2[3]);

	void Deformation(int index, double point[3]);
	void Scaling(int index, double point[3]);
	void Rotation(int index, double point[3]);

	void UpdateLineActor(int index, double * point1, double * point2);
	void UpdatePointActor(int index, double * point);

	void FindAndHighlightCurrentPoint(double * pointCoord);
	
	bool IsInBound(double *pos);

	albaDeviceButtonsPadMouse	*m_Mouse;
	vtkRenderer								*m_Renderer;
	albaView									*m_View;
	vtkCoordinate							*m_Coordinate;

	// Persistent Points
	std::vector<vtkPointSource *>				m_PointSourceVector;
	std::vector<vtkPolyDataMapper2D *>	m_PointMapperVector;
	std::vector<vtkActor2D *>						m_PointActorVector;

	// Persistent Lines
	std::vector<vtkLineSource *>				m_LineSourceVector;
	std::vector<vtkPolyDataMapper2D *>	m_LineMapperVector;
	std::vector<vtkActor2D *>						m_LineActorVector;

	// Edit Actors
	vtkPointSource				*m_EditPointSource;
	vtkPolyDataMapper2D		*m_EditPointMapper;
	vtkActor2D						*m_EditPointActor;
	
	double m_ColorDefault[3];
	double m_ColorSelection[3];

	int m_CurrentPointIndex;
	int m_TotalPoints;

	double *m_Bounds;
	bool m_IsInBound;
	bool m_ButtonDownInside;

	bool m_ParallelView;

	double m_ResetPoint1[3]; 
	double m_ResetPoint2[3]; 
	double m_ResetPoint3[3]; 
	double m_ResetPoint4[3];
	double m_ResetPoint5[3];

	int m_Mode;
	int m_ScaleMode;
	int m_DeformationMode;

private:
	appInteractor2DTransform(const appInteractor2DTransform&);   // Not implemented.
	void operator=(const appInteractor2DTransform&);  // Not implemented.
	friend class appInteractor2DTransformTest;
};
#endif
