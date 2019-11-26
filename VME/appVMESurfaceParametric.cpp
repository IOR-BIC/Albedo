/*=========================================================================
Program:   Albedo
Module:    appVMESurfaceParametric.cpp
Language:  C++
Date:      $Date: 2019-01-01 12:00:00 $
Version:   $Revision: 1.0.0.0 $
Authors:   Nicola Vanella
==========================================================================
Copyright (c) BIC-IOR 2019 (http://www.ltmsoftware.org/alba.htm)

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the above copyright notice for more information.
=========================================================================*/

#include "appDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the APP must include "appDefines.h" as first.
// This force to include Window, wxWidgets and VTK exactly in this order.
//----------------------------------------------------------------------------

#include "appVMESurfaceParametric.h"

#include "albaDataPipeCustom.h"
#include "albaDataPipeInterpolator.h"
#include "albaDataVector.h"
#include "albaGUI.h"
#include "albaMatrixInterpolator.h"
#include "albaMatrixVector.h"
#include "albaTagArray.h"
#include "albaTransform.h"
#include "albaVMEItemVTK.h"
#include "albaVMEOutputSurface.h"
#include "mmaMaterial.h"
#include "mmuIdFactory.h"

#include "vtkALBASmartPointer.h"
#include "vtkPolyData.h"
#include "vtkCellArray.h"
#include "vtkBitArray.h"
#include "vtkPointData.h"
#include "vtkConeSource.h"
#include "vtkCylinderSource.h"
#include "vtkPlaneSource.h"
#include "vtkCubeSource.h"
#include "vtkSphereSource.h"
#include "vtkTriangleFilter.h"
#include "vtkTransform.h"
#include "vtkTransformPolyDataFilter.h"

#include <vector>

const bool DEBUG_MODE = true;

//------------------------------------------------------------------------------
albaCxxTypeMacro(appVMESurfaceParametric);

//------------------------------------------------------------------------------
appVMESurfaceParametric::appVMESurfaceParametric()
{
  m_GeometryType = PARAMETRIC_SPHERE;
	
  m_SphereRadius = 2.0;
  m_SpherePhiRes = 10.0;
  m_SphereTheRes = 10.0;

  m_ConeHeight = 5.0;
  m_ConeRadius = 2.0;
  m_ConeCapping = 0;
  m_ConeRes = 20.0;
  m_ConeOrientationAxis = ID_X_AXIS;

  m_CylinderHeight = 5.0;
  m_CylinderRadius = 2.0;
  m_CylinderRes = 20.0;
  m_CylinderOrientationAxis = ID_Y_AXIS;

  m_CubeXLength = 2.0;
  m_CubeYLength = 2.0;
  m_CubeZLength = 2.0;

  m_PlaneXRes = 2.0;
  m_PlaneYRes = 2.0;
  m_PlaneOrigin[0] = m_PlaneOrigin[1] = m_PlaneOrigin[2] = 0;
  m_PlanePoint1[0] = 2.0;
  m_PlanePoint1[1] = m_PlanePoint1[2] = 0.0;
  m_PlanePoint2[1] = 3.0;
  m_PlanePoint2[0] = m_PlanePoint2[2] = 0.0;

  m_EllipsoidXLenght = 1.0;
  m_EllipsoidYLenght = 2.0;
  m_EllipsoidZLenght = 3.0;
  m_EllipsoidPhiRes = 10.0;
  m_EllipsoidTheRes = 10.0;
  m_EllipsoidOrientationAxis = ID_X_AXIS;

	albaNEW(m_Transform);
	albaVMEOutputSurface *output=albaVMEOutputSurface::New(); // an output with no data
	output->SetTransform(m_Transform); // force my transform in the output
	SetOutput(output);

	GetMaterial();

	vtkNEW(m_PolyData);

  // Attach a data pipe which creates a bridge between VTK and ALBA
	albaDataPipeCustom *dpipe = albaDataPipeCustom::New();
	dpipe->SetInput(m_PolyData);
	SetDataPipe(dpipe);
}
//-------------------------------------------------------------------------
appVMESurfaceParametric::~appVMESurfaceParametric()
{
	vtkDEL(m_PolyData);
  albaDEL(m_Transform);
	SetOutput(NULL);
}

//-------------------------------------------------------------------------
int appVMESurfaceParametric::DeepCopy(albaVME *a)
{ 
  if (Superclass::DeepCopy(a)==ALBA_OK)
  {
    appVMESurfaceParametric *vmeParametricSurface=appVMESurfaceParametric::SafeDownCast(a);
    m_Transform->SetMatrix(vmeParametricSurface->m_Transform->GetMatrix());
    this->m_GeometryType = vmeParametricSurface->m_GeometryType;
    this->m_SphereRadius = vmeParametricSurface->m_SphereRadius;
    this->m_SpherePhiRes = vmeParametricSurface->m_SpherePhiRes;
    this->m_SphereTheRes = vmeParametricSurface->m_SphereTheRes;

    this->m_ConeHeight = vmeParametricSurface->m_ConeHeight;
    this->m_ConeRadius = vmeParametricSurface->m_ConeRadius;
    this->m_ConeCapping = vmeParametricSurface->m_ConeCapping;
    this->m_ConeRes = vmeParametricSurface->m_ConeRes;
    this->m_ConeOrientationAxis = vmeParametricSurface->m_ConeOrientationAxis;

    this->m_CylinderHeight = vmeParametricSurface->m_CylinderHeight;
    this->m_CylinderRadius = vmeParametricSurface->m_CylinderRadius;
    this->m_CylinderRes = vmeParametricSurface->m_CylinderRes;
    this->m_CylinderOrientationAxis = vmeParametricSurface->m_CylinderOrientationAxis;

    this->m_CubeXLength = vmeParametricSurface->m_CubeXLength;
    this->m_CubeYLength = vmeParametricSurface->m_CubeYLength;
    this->m_CubeZLength = vmeParametricSurface->m_CubeZLength;

    this->m_PlaneXRes = vmeParametricSurface->m_PlaneXRes;
    this->m_PlaneYRes = vmeParametricSurface->m_PlaneYRes;

    this->m_PlaneOrigin[0] = vmeParametricSurface->m_PlaneOrigin[0];
    this->m_PlaneOrigin[1] = vmeParametricSurface->m_PlaneOrigin[1];
    this->m_PlaneOrigin[2] = vmeParametricSurface->m_PlaneOrigin[2];

    this->m_PlanePoint1[0] = vmeParametricSurface->m_PlanePoint1[0];
    this->m_PlanePoint1[1] = vmeParametricSurface->m_PlanePoint1[1];
    this->m_PlanePoint1[2] = vmeParametricSurface->m_PlanePoint1[2];

    this->m_PlanePoint2[0] = vmeParametricSurface->m_PlanePoint2[0];
    this->m_PlanePoint2[1] = vmeParametricSurface->m_PlanePoint2[1];
    this->m_PlanePoint2[2] = vmeParametricSurface->m_PlanePoint2[2];

    this->m_EllipsoidXLenght = vmeParametricSurface->m_EllipsoidXLenght;
    this->m_EllipsoidYLenght = vmeParametricSurface->m_EllipsoidYLenght;
    this->m_EllipsoidZLenght = vmeParametricSurface->m_EllipsoidZLenght;
    this->m_EllipsoidPhiRes = vmeParametricSurface->m_EllipsoidPhiRes;
    this->m_EllipsoidTheRes = vmeParametricSurface->m_EllipsoidTheRes;
    this->m_EllipsoidOrientationAxis = vmeParametricSurface->m_EllipsoidOrientationAxis;

    albaDataPipeCustom *dpipe = albaDataPipeCustom::SafeDownCast(GetDataPipe());
    if (dpipe)
    {
      dpipe->SetInput(m_PolyData);
    }

    InternalUpdate();
    return ALBA_OK;
  }  
  return ALBA_ERROR;
}

//-------------------------------------------------------------------------
bool appVMESurfaceParametric::Equals(albaVME *vme)
{
  bool ret = false;
  if (Superclass::Equals(vme))
  {
    if (     
      m_Transform->GetMatrix()==((appVMESurfaceParametric *)vme)->m_Transform->GetMatrix() &&
      this->m_GeometryType == ((appVMESurfaceParametric *)vme)->m_GeometryType &&
      this->m_SphereRadius == ((appVMESurfaceParametric *)vme)->m_SphereRadius &&
      this->m_SpherePhiRes == ((appVMESurfaceParametric *)vme)->m_SpherePhiRes &&
      this->m_SphereTheRes == ((appVMESurfaceParametric *)vme)->m_SphereTheRes &&

      this->m_ConeHeight == ((appVMESurfaceParametric *)vme)->m_ConeHeight  &&
      this->m_ConeRadius == ((appVMESurfaceParametric *)vme)->m_ConeRadius  &&
      this->m_ConeCapping == ((appVMESurfaceParametric *)vme)->m_ConeCapping  &&
      this->m_ConeRes == ((appVMESurfaceParametric *)vme)->m_ConeRes &&
      this->m_ConeOrientationAxis == ((appVMESurfaceParametric *)vme)->m_ConeOrientationAxis &&

      this->m_CylinderHeight == ((appVMESurfaceParametric *)vme)->m_CylinderHeight  &&
      this->m_CylinderRadius == ((appVMESurfaceParametric *)vme)->m_CylinderRadius  &&
      this->m_CylinderRes == ((appVMESurfaceParametric *)vme)->m_CylinderRes  &&
      this->m_CylinderOrientationAxis == ((appVMESurfaceParametric *)vme)->m_CylinderOrientationAxis  &&

      this->m_CubeXLength == ((appVMESurfaceParametric *)vme)->m_CubeXLength  &&
      this->m_CubeYLength == ((appVMESurfaceParametric *)vme)->m_CubeYLength  &&
      this->m_CubeZLength == ((appVMESurfaceParametric *)vme)->m_CubeZLength  &&

      this->m_PlaneXRes == ((appVMESurfaceParametric *)vme)->m_PlaneXRes  &&
      this->m_PlaneYRes == ((appVMESurfaceParametric *)vme)->m_PlaneYRes  &&

      this->m_PlaneOrigin[0] == ((appVMESurfaceParametric *)vme)->m_PlaneOrigin[0]  &&
      this->m_PlaneOrigin[1] == ((appVMESurfaceParametric *)vme)->m_PlaneOrigin[1]  &&
      this->m_PlaneOrigin[2] == ((appVMESurfaceParametric *)vme)->m_PlaneOrigin[2]  &&

      this->m_PlanePoint1[0] == ((appVMESurfaceParametric *)vme)->m_PlanePoint1[0]  &&
      this->m_PlanePoint1[1] == ((appVMESurfaceParametric *)vme)->m_PlanePoint1[1]  &&
      this->m_PlanePoint1[2] == ((appVMESurfaceParametric *)vme)->m_PlanePoint1[2]  &&

      this->m_PlanePoint2[0] == ((appVMESurfaceParametric *)vme)->m_PlanePoint2[0]  &&
      this->m_PlanePoint2[1] == ((appVMESurfaceParametric *)vme)->m_PlanePoint2[1]  &&
      this->m_PlanePoint2[2] == ((appVMESurfaceParametric *)vme)->m_PlanePoint2[2] &&

      this->m_EllipsoidXLenght == ((appVMESurfaceParametric *)vme)->m_EllipsoidXLenght &&
      this->m_EllipsoidYLenght == ((appVMESurfaceParametric *)vme)->m_EllipsoidYLenght &&
      this->m_EllipsoidZLenght == ((appVMESurfaceParametric *)vme)->m_EllipsoidZLenght &&
      this->m_EllipsoidPhiRes == ((appVMESurfaceParametric *)vme)->m_EllipsoidPhiRes &&
      this->m_EllipsoidTheRes == ((appVMESurfaceParametric *)vme)->m_EllipsoidTheRes &&
      this->m_EllipsoidOrientationAxis == ((appVMESurfaceParametric *)vme)->m_EllipsoidOrientationAxis
      )
    {
      ret = true;
    }
  }
 return ret;
}

// GET-SET //////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------
mmaMaterial *appVMESurfaceParametric::GetMaterial()
{
	mmaMaterial *material = (mmaMaterial *)GetAttribute("MaterialAttributes");
	if (material == NULL)
	{
		material = mmaMaterial::New();
		SetAttribute("MaterialAttributes", material);
	}
	return material;
}
//-------------------------------------------------------------------------
albaVMEOutputSurface *appVMESurfaceParametric::GetSurfaceOutput()
{
	return (albaVMEOutputSurface *)GetOutput();
}
//-------------------------------------------------------------------------
void appVMESurfaceParametric::GetLocalTimeStamps(std::vector<albaTimeStamp> &kframes)
{
	kframes.clear(); // no timestamps
	albaTimeStamp t = m_Transform->GetMatrix().GetTimeStamp();
	kframes.push_back(t);
}
//-------------------------------------------------------------------------
char** appVMESurfaceParametric::GetIcon()
{
#include "pic/VME_PROCEDURAL_ICON.xpm"
	return VME_PROCEDURAL_ICON_xpm;
}
//-------------------------------------------------------------------------
bool appVMESurfaceParametric::IsAnimated()
{
	return false;
}

//-------------------------------------------------------------------------
void appVMESurfaceParametric::SetMatrix(const albaMatrix &mat)
{
	m_Transform->SetMatrix(mat);
	//Update AbsMatrix output
	GetOutput()->GetAbsMatrix();
	Modified();
}
//-------------------------------------------------------------------------
void appVMESurfaceParametric::SetGeometryType(int parametricSurfaceTypeID)
{
	m_GeometryType = parametricSurfaceTypeID;
	Modified();
}
//-------------------------------------------------------------------------
void appVMESurfaceParametric::SetSphereRadius(double radius)
{
	m_SphereRadius = radius;
	Modified();
}

// EVENT //////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------
void appVMESurfaceParametric::OnEvent(albaEventBase *alba_event)
{
  // events to be sent up or down in the tree are simply forwarded
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
  {
    switch(e->GetId())
    {
      case ID_GEOMETRY_TYPE:
      {  
        EnableParametricSurfaceGui(m_GeometryType);
        m_Gui->Update();
        InternalUpdate();
        m_Gui->FitGui();
      }

      case CHANGE_VALUE_SPHERE:
      case CHANGE_VALUE_CUBE:
      case CHANGE_VALUE_CONE:
      case CHANGE_VALUE_CYLINDER:
      case CHANGE_VALUE_PLANE:
      case CHANGE_VALUE_ELLIPSOID:
      {
        InternalUpdate();
        e->SetId(CAMERA_UPDATE);
        ForwardUpEvent(e);
      }
      break;
      
      default:
        albaVME::OnEvent(alba_event);
    }
  }  
  else
  {
    Superclass::OnEvent(alba_event);
  }
}

//-----------------------------------------------------------------------
void appVMESurfaceParametric::InternalPreUpdate()
{
	InternalInitialize();
}
//-----------------------------------------------------------------------
void appVMESurfaceParametric::InternalUpdate()
{
	switch (m_GeometryType)
	{
	case PARAMETRIC_SPHERE:
	{
		vtkALBASmartPointer<vtkSphereSource> surf;
		surf->SetRadius(m_SphereRadius);
		surf->SetPhiResolution(m_SpherePhiRes);
		surf->SetThetaResolution(m_SphereTheRes);
		surf->Update();
		m_PolyData->DeepCopy(surf->GetOutput());
		m_PolyData->Update();
	}
	break;
	case PARAMETRIC_CONE:
	{
		vtkALBASmartPointer<vtkConeSource> surf;
		surf->SetHeight(m_ConeHeight);
		surf->SetRadius(m_ConeRadius);
		surf->SetCapping(m_ConeCapping);
		surf->SetResolution(m_ConeRes);
		surf->Update();

		vtkALBASmartPointer<vtkTransform> t;

		switch (m_ConeOrientationAxis)
		{
		case ID_X_AXIS:
			//do nothing
			break;
		case ID_Y_AXIS:
			t->RotateZ(90);
			break;
		case ID_Z_AXIS:
			t->RotateY(-90);
			break;
		default:
			break;
		}

		t->Update();

		vtkALBASmartPointer<vtkTransformPolyDataFilter> ptf;
		ptf->SetTransform(t);
		ptf->SetInput(surf->GetOutput());
		ptf->Update();

		m_PolyData->DeepCopy(ptf->GetOutput());
		m_PolyData->Update();
	}
	break;
	case PARAMETRIC_CYLINDER:
	{
		vtkALBASmartPointer<vtkCylinderSource> surf;
		surf->SetHeight(m_CylinderHeight);
		surf->SetRadius(m_CylinderRadius);
		surf->SetResolution(m_CylinderRes);
		surf->Update();

		vtkALBASmartPointer<vtkTransform> t;

		switch (m_CylinderOrientationAxis)
		{
		case ID_X_AXIS:
			t->RotateZ(90);
			break;
		case ID_Y_AXIS:
			//do nothing
			break;
		case ID_Z_AXIS:
			t->RotateX(-90);
			break;
		default:
			break;
		}

		t->Update();

		vtkALBASmartPointer<vtkTransformPolyDataFilter> ptf;
		ptf->SetTransform(t);
		ptf->SetInput(surf->GetOutput());
		ptf->Update();

		m_PolyData->DeepCopy(ptf->GetOutput());
		m_PolyData->Update();

	}
	break;
	case PARAMETRIC_CUBE:
	{
		vtkALBASmartPointer<vtkCubeSource> surf;
		surf->SetXLength(m_CubeXLength);
		surf->SetYLength(m_CubeYLength);
		surf->SetZLength(m_CubeZLength);
		surf->Update();
		m_PolyData->DeepCopy(surf->GetOutput());
		m_PolyData->Update();
	}
	break;

	case PARAMETRIC_PLANE:
	{
		vtkALBASmartPointer<vtkPlaneSource> surf;
		surf->SetXResolution(m_PlaneXRes);
		surf->SetYResolution(m_PlaneYRes);
		surf->SetOrigin(m_PlaneOrigin);
		surf->SetPoint1(m_PlanePoint1);
		surf->SetPoint2(m_PlanePoint2);
		surf->Update();
		vtkALBASmartPointer<vtkTriangleFilter> triangle;
		triangle->SetInput(surf->GetOutput());
		triangle->Update();
		m_PolyData->DeepCopy(triangle->GetOutput());
		m_PolyData->Update();
	}
	break;

	case PARAMETRIC_ELLIPSOID:
	{
		vtkALBASmartPointer<vtkSphereSource> surf;
		surf->SetRadius(m_EllipsoidYLenght);
		surf->SetPhiResolution(m_EllipsoidPhiRes);
		surf->SetThetaResolution(m_EllipsoidTheRes);
		surf->Update();

		vtkALBASmartPointer<vtkTransform> t;

		switch (m_EllipsoidOrientationAxis)
		{
		case ID_X_AXIS:
			//do nothing
			break;
		case ID_Y_AXIS:
			t->RotateZ(90);
			break;
		case ID_Z_AXIS:
			t->RotateY(-90);
			break;
		default:
			break;
		}

		t->Scale(m_EllipsoidXLenght / m_EllipsoidYLenght, 1, m_EllipsoidZLenght / m_EllipsoidYLenght);
		t->Update();

		vtkALBASmartPointer<vtkTransformPolyDataFilter> ptf;
		ptf->SetTransform(t);
		ptf->SetInput(surf->GetOutput());
		ptf->Update();

		m_PolyData->DeepCopy(ptf->GetOutput());
		m_PolyData->Update();
	}
	break;
	}
}
//-----------------------------------------------------------------------
int appVMESurfaceParametric::InternalStore(albaStorageElement *parent)
{
	if (Superclass::InternalStore(parent) == ALBA_OK)
	{
		if (
			parent->StoreMatrix("Transform", &m_Transform->GetMatrix()) == ALBA_OK &&
			parent->StoreInteger("Geometry", m_GeometryType) == ALBA_OK &&
			parent->StoreDouble("ShereRadius", m_SphereRadius) == ALBA_OK &&
			parent->StoreDouble("SpherePhiRes", m_SpherePhiRes) == ALBA_OK &&
			parent->StoreDouble("SphereThetaRes", m_SphereTheRes) == ALBA_OK &&
			parent->StoreDouble("ConeHieght", m_ConeHeight) == ALBA_OK &&
			parent->StoreDouble("ConeRadius", m_ConeRadius) == ALBA_OK &&
			parent->StoreInteger("ConeCapping", m_ConeCapping) == ALBA_OK &&
			parent->StoreDouble("ConeRes", m_ConeRes) == ALBA_OK &&
			parent->StoreInteger("ConeOrientationAxis", m_ConeOrientationAxis) == ALBA_OK &&
			parent->StoreDouble("CylinderHeight", m_CylinderHeight) == ALBA_OK &&
			parent->StoreDouble("CylinderRadius", m_CylinderRadius) == ALBA_OK &&
			parent->StoreDouble("CylinderRes", m_CylinderRes) == ALBA_OK &&
			parent->StoreInteger("CylinderOrientationAxis", m_CylinderOrientationAxis) == ALBA_OK &&
			parent->StoreDouble("CubeXLength", m_CubeXLength) == ALBA_OK &&
			parent->StoreDouble("CubeYLength", m_CubeYLength) == ALBA_OK &&
			parent->StoreDouble("CubeZLength", m_CubeZLength) == ALBA_OK &&
			parent->StoreDouble("PlaneXRes", m_PlaneXRes) == ALBA_OK &&
			parent->StoreDouble("PlaneYRes", m_PlaneYRes) == ALBA_OK &&
			parent->StoreVectorN("PlaneOrigin", m_PlaneOrigin, 3) == ALBA_OK &&
			parent->StoreVectorN("PlanePoint1", m_PlanePoint1, 3) == ALBA_OK &&
			parent->StoreVectorN("PlanePoint2", m_PlanePoint2, 3) == ALBA_OK &&
			parent->StoreDouble("EllipsoidXLenght", m_EllipsoidXLenght) == ALBA_OK &&
			parent->StoreDouble("EllipsoidYLenght", m_EllipsoidYLenght) == ALBA_OK &&
			parent->StoreDouble("EllipsoidZLenght", m_EllipsoidZLenght) == ALBA_OK &&
			parent->StoreDouble("EllipsoidTheRes", m_EllipsoidTheRes) == ALBA_OK &&
			parent->StoreDouble("EllipsoidPhiRes", m_EllipsoidPhiRes) == ALBA_OK &&
			parent->StoreInteger("EllipsoidOrientationAxis", m_CylinderOrientationAxis) == ALBA_OK
			)
			return ALBA_OK;
	}
	return ALBA_ERROR;
}
//-----------------------------------------------------------------------
int appVMESurfaceParametric::InternalRestore(albaStorageElement *node)
{
	if (Superclass::InternalRestore(node) == ALBA_OK)
	{
		albaMatrix matrix;
		if (node->RestoreMatrix("Transform", &matrix) == ALBA_OK)
		{
			m_Transform->SetMatrix(matrix);
			node->RestoreInteger("Geometry", m_GeometryType);
			node->RestoreDouble("ShereRadius", m_SphereRadius);
			node->RestoreDouble("SpherePhiRes", m_SpherePhiRes);
			node->RestoreDouble("SphereThetaRes", m_SphereTheRes);
			node->RestoreDouble("ConeHieght", m_ConeHeight);
			node->RestoreDouble("ConeRadius", m_ConeRadius);
			node->RestoreInteger("ConeCapping", m_ConeCapping);
			node->RestoreDouble("ConeRes", m_ConeRes);
			node->RestoreInteger("ConeOrientationAxis", m_ConeOrientationAxis);
			node->RestoreDouble("CylinderHeight", m_CylinderHeight);
			node->RestoreDouble("CylinderRadius", m_CylinderRadius);
			node->RestoreDouble("CylinderRes", m_CylinderRes);
			node->RestoreInteger("CylinderOrientationAxis", m_CylinderOrientationAxis);
			node->RestoreDouble("CubeXLength", m_CubeXLength);
			node->RestoreDouble("CubeYLength", m_CubeYLength);
			node->RestoreDouble("CubeZLength", m_CubeZLength);
			node->RestoreDouble("PlaneXRes", m_PlaneXRes);
			node->RestoreDouble("PlaneYRes", m_PlaneYRes);
			node->RestoreVectorN("PlaneOrigin", m_PlaneOrigin, 3);
			node->RestoreVectorN("PlanePoint1", m_PlanePoint1, 3);
			node->RestoreVectorN("PlanePoint2", m_PlanePoint2, 3);
			node->RestoreDouble("EllipsoidXLenght", m_EllipsoidXLenght);
			node->RestoreDouble("EllipsoidYLenght", m_EllipsoidYLenght);
			node->RestoreDouble("EllipsoidZLenght", m_EllipsoidZLenght);
			node->RestoreDouble("EllipsoidTheRes", m_EllipsoidTheRes);
			node->RestoreDouble("EllipsoidPhiRes", m_EllipsoidPhiRes);
			node->RestoreInteger("EllipsoidOrientationAxis", m_CylinderOrientationAxis);
			return ALBA_OK;
		}
	}
	return ALBA_ERROR;
}

// GUI //////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------
albaGUI* appVMESurfaceParametric::CreateGui()
{
	albaVME::CreateGui();
	if (m_Gui)
	{
		wxString geometryType[6] = { "Sphere", "Cone", "Cylinder", "Cube", "Plane", "Ellipsoid" };
		m_Gui->Combo(ID_GEOMETRY_TYPE, "", &m_GeometryType, 6, geometryType);
		m_Gui->Divider(2);

		CreateGuiSphere();
		m_Gui->Divider(2);
		CreateGuiCone();
		m_Gui->Divider(2);
		CreateGuiCylinder();
		m_Gui->Divider(2);
		CreateGuiCube();
		m_Gui->Divider(2);
		CreateGuiPlane();
		m_Gui->Divider(2);
		CreateGuiEllipsoid();

		m_Gui->FitGui();

		EnableParametricSurfaceGui(m_GeometryType);
		m_Gui->Update();
	}

	m_Gui->Divider();
	return m_Gui;
}
//-------------------------------------------------------------------------
void appVMESurfaceParametric::CreateGuiPlane()
{
	m_GuiPlane = new albaGUI(this);
	m_GuiPlane->Label("Plane");
	m_GuiPlane->Double(CHANGE_VALUE_PLANE, _("X Res"), &m_PlaneXRes);
	m_GuiPlane->Double(CHANGE_VALUE_PLANE, _("Y Res"), &m_PlaneYRes);
	m_GuiPlane->Vector(CHANGE_VALUE_PLANE, _("Origin"), m_PlaneOrigin);
	m_GuiPlane->Vector(CHANGE_VALUE_PLANE, _("Point 1"), m_PlanePoint1);
	m_GuiPlane->Vector(CHANGE_VALUE_PLANE, _("Point 2"), m_PlanePoint2);
	assert(m_Gui);
	m_Gui->AddGui(m_GuiPlane);
}
//-------------------------------------------------------------------------
void appVMESurfaceParametric::CreateGuiCube()
{
	m_GuiCube = new albaGUI(this);
	m_GuiCube->Label("Cube");
	m_GuiCube->Double(CHANGE_VALUE_CUBE, _("X Length"), &m_CubeXLength);
	m_GuiCube->Double(CHANGE_VALUE_CUBE, _("Y Length"), &m_CubeYLength);
	m_GuiCube->Double(CHANGE_VALUE_CUBE, _("Z Length"), &m_CubeZLength);
	assert(m_Gui);
	m_Gui->AddGui(m_GuiCube);
}
//-------------------------------------------------------------------------
void appVMESurfaceParametric::CreateGuiCylinder()
{
	m_GuiCylinder = new albaGUI(this);
	m_GuiCylinder->Label("Cylinder");
	m_GuiCylinder->Double(CHANGE_VALUE_CYLINDER, _("Height"), &m_CylinderHeight);
	m_GuiCylinder->Double(CHANGE_VALUE_CYLINDER, _("Radius"), &m_CylinderRadius);
	m_GuiCylinder->Double(CHANGE_VALUE_CYLINDER, _("Resolution"), &m_CylinderRes);
	wxString orientationArray[3] = { _("X axis"),_("Y axis"),_("Z axis") };
	m_GuiCylinder->Radio(CHANGE_VALUE_CYLINDER, "Orientation", &m_CylinderOrientationAxis, 3, orientationArray);
	assert(m_Gui);
	m_Gui->AddGui(m_GuiCylinder);
}
//-------------------------------------------------------------------------
void appVMESurfaceParametric::CreateGuiCone()
{
	m_GuiCone = new albaGUI(this);
	m_GuiCone->Label("Cone");
	m_GuiCone->Double(CHANGE_VALUE_CONE, _("Height"), &m_ConeHeight);
	m_GuiCone->Double(CHANGE_VALUE_CONE, _("Radius"), &m_ConeRadius);
	m_GuiCone->Double(CHANGE_VALUE_CONE, _("Resolution"), &m_ConeRes);
	m_GuiCone->Bool(CHANGE_VALUE_CONE, "Cap", &m_ConeCapping); // Open or closed cone
	wxString orientationArray[3] = { _("X axis"),_("Y axis"),_("Z axis") };
	m_GuiCone->Radio(CHANGE_VALUE_CONE, "Orientation", &m_ConeOrientationAxis, 3, orientationArray);
	assert(m_Gui);
	m_Gui->AddGui(m_GuiCone);
}
//-------------------------------------------------------------------------
void appVMESurfaceParametric::CreateGuiSphere()
{
	m_GuiSphere = new albaGUI(this);
	m_GuiSphere->Label("Sphere");
	m_GuiSphere->Double(CHANGE_VALUE_SPHERE, _("Radius"), &m_SphereRadius);
	m_GuiSphere->Double(CHANGE_VALUE_SPHERE, _("Phi res"), &m_SpherePhiRes);
	m_GuiSphere->Double(CHANGE_VALUE_SPHERE, _("Theta res"), &m_SphereTheRes);
	assert(m_Gui);
	m_Gui->AddGui(m_GuiSphere);
}
//-------------------------------------------------------------------------
void appVMESurfaceParametric::CreateGuiEllipsoid()
{
	m_GuiEllipsoid = new albaGUI(this);
	m_GuiEllipsoid->Label("Ellipsoid");
	m_GuiEllipsoid->Double(CHANGE_VALUE_ELLIPSOID, _("X Length"), &m_EllipsoidXLenght);
	m_GuiEllipsoid->Double(CHANGE_VALUE_ELLIPSOID, _("Y Length"), &m_EllipsoidYLenght);
	m_GuiEllipsoid->Double(CHANGE_VALUE_ELLIPSOID, _("Z Length"), &m_EllipsoidZLenght);
	m_GuiEllipsoid->Double(CHANGE_VALUE_ELLIPSOID, _("Phi res"), &m_EllipsoidPhiRes);
	m_GuiEllipsoid->Double(CHANGE_VALUE_ELLIPSOID, _("Theta res"), &m_EllipsoidTheRes);
	wxString orientationArray[3] = { _("X axis"),_("Y axis"),_("Z axis") };
	m_GuiEllipsoid->Radio(CHANGE_VALUE_ELLIPSOID, "Orientation", &m_EllipsoidOrientationAxis, 3, orientationArray);
	assert(m_Gui);
	m_Gui->AddGui(m_GuiEllipsoid);
}

//-------------------------------------------------------------------------
void appVMESurfaceParametric::EnableParametricSurfaceGui(int surfaceTypeID)
{
	switch (surfaceTypeID)
	{
	case PARAMETRIC_SPHERE:
	{
		EnableGuiSphere();

		if (DEBUG_MODE)
		{
			std::ostringstream stringStream;
			stringStream << "enabling Sphere gui" << std::endl;
			albaLogMessage(stringStream.str().c_str());
		}
	}
	break;

	case PARAMETRIC_CONE:
	{
		EnableGuiCone();
		if (DEBUG_MODE)
		{
			std::ostringstream stringStream;
			stringStream << "enabling Cone gui" << std::endl;
			albaLogMessage(stringStream.str().c_str());
		}
	}
	break;

	case PARAMETRIC_CYLINDER:
	{
		EnableGuiCylinder();
		if (DEBUG_MODE)
		{
			std::ostringstream stringStream;
			stringStream << "enabling Cylinder gui" << std::endl;
			albaLogMessage(stringStream.str().c_str());
		}
	}
	break;

	case PARAMETRIC_CUBE:
	{
		EnableGuiCube();
		if (DEBUG_MODE)
		{
			std::ostringstream stringStream;
			stringStream << "enabling Cube gui" << std::endl;
			albaLogMessage(stringStream.str().c_str());
		}
	}
	break;

	case PARAMETRIC_PLANE:
	{
		EnableGuiPlane();
		if (DEBUG_MODE)
		{
			std::ostringstream stringStream;
			stringStream << "enabling Plane gui" << std::endl;
			albaLogMessage(stringStream.str().c_str());
		}
	}
	break;

	case PARAMETRIC_ELLIPSOID:
	{
		EnableGuiEllipsoid();
		if (DEBUG_MODE)
		{
			std::ostringstream stringStream;
			stringStream << "enabling Ellipsoid gui" << std::endl;
			albaLogMessage(stringStream.str().c_str());
		}
	}
	break;

	default:
		break;
	}
}
//-------------------------------------------------------------------------
void appVMESurfaceParametric::EnableGuiPlane()
{
	m_GuiPlane->Enable(CHANGE_VALUE_PLANE, true);
	m_GuiCube->Enable(CHANGE_VALUE_CUBE, false);
	m_GuiCylinder->Enable(CHANGE_VALUE_CYLINDER, false);
	m_GuiCone->Enable(CHANGE_VALUE_CONE, false);
	m_GuiSphere->Enable(CHANGE_VALUE_SPHERE, false);
	m_GuiEllipsoid->Enable(CHANGE_VALUE_ELLIPSOID, false);
}
//-------------------------------------------------------------------------
void appVMESurfaceParametric::EnableGuiCube()
{
	m_GuiPlane->Enable(CHANGE_VALUE_PLANE, false);
	m_GuiCube->Enable(CHANGE_VALUE_CUBE, true);
	m_GuiCylinder->Enable(CHANGE_VALUE_CYLINDER, false);
	m_GuiCone->Enable(CHANGE_VALUE_CONE, false);
	m_GuiSphere->Enable(CHANGE_VALUE_SPHERE, false);
	m_GuiEllipsoid->Enable(CHANGE_VALUE_ELLIPSOID, false);
}
//-------------------------------------------------------------------------
void appVMESurfaceParametric::EnableGuiCylinder()
{
	m_GuiPlane->Enable(CHANGE_VALUE_PLANE, false);
	m_GuiCube->Enable(CHANGE_VALUE_CUBE, false);
	m_GuiCylinder->Enable(CHANGE_VALUE_CYLINDER, true);
	m_GuiCone->Enable(CHANGE_VALUE_CONE, false);
	m_GuiSphere->Enable(CHANGE_VALUE_SPHERE, false);
	m_GuiEllipsoid->Enable(CHANGE_VALUE_ELLIPSOID, false);
}
//-------------------------------------------------------------------------
void appVMESurfaceParametric::EnableGuiCone()
{
	m_GuiPlane->Enable(CHANGE_VALUE_PLANE, false);
	m_GuiCube->Enable(CHANGE_VALUE_CUBE, false);
	m_GuiCylinder->Enable(CHANGE_VALUE_CYLINDER, false);
	m_GuiCone->Enable(CHANGE_VALUE_CONE, true);
	m_GuiSphere->Enable(CHANGE_VALUE_SPHERE, false);
	m_GuiEllipsoid->Enable(CHANGE_VALUE_ELLIPSOID, false);
}
//-------------------------------------------------------------------------
void appVMESurfaceParametric::EnableGuiSphere()
{
	m_GuiPlane->Enable(CHANGE_VALUE_PLANE, false);
	m_GuiCube->Enable(CHANGE_VALUE_CUBE, false);
	m_GuiCylinder->Enable(CHANGE_VALUE_CYLINDER, false);
	m_GuiCone->Enable(CHANGE_VALUE_CONE, false);
	m_GuiSphere->Enable(CHANGE_VALUE_SPHERE, true);
	m_GuiEllipsoid->Enable(CHANGE_VALUE_ELLIPSOID, false);
}
//-------------------------------------------------------------------------
void appVMESurfaceParametric::EnableGuiEllipsoid()
{
	m_GuiPlane->Enable(CHANGE_VALUE_PLANE, false);
	m_GuiCube->Enable(CHANGE_VALUE_CUBE, false);
	m_GuiCylinder->Enable(CHANGE_VALUE_CYLINDER, false);
	m_GuiCone->Enable(CHANGE_VALUE_CONE, false);
	m_GuiSphere->Enable(CHANGE_VALUE_SPHERE, false);
	m_GuiEllipsoid->Enable(CHANGE_VALUE_ELLIPSOID, true);
}