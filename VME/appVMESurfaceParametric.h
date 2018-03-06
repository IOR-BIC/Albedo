/*=========================================================================
Program:   Albedo
Module:    appVMESurfaceParametric.h
Language:  C++
Date:      $Date: 2018-01-01 12:00:00 $
Version:   $Revision: 1.0.0.0 $
Authors:   Nicola Vanella
==========================================================================
Copyright (c) LTM-IOR 2018 (http://www.ltmsoftware.org/alba.htm)

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the above copyright notice for more information.
=========================================================================*/

#ifndef __appVMESurfaceParametric_h
#define __appVMESurfaceParametric_h

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "appVMEDefines.h"

#include "mafVME.h"
#include "mafEvent.h"

//----------------------------------------------------------------------------
// Forward declarations :
//----------------------------------------------------------------------------
class mmaMaterial;
class mafVME;
class mafVMEOutputSurface;
class vtkPolyData;

//----------------------------------------------------------------------------
// Class Name : appVMESurfaceParametric 
// This class represents a parametric surface that is a specific geometry.
// Currently generated geometries are : cylinder, sphere, cube, cone and plane.
//----------------------------------------------------------------------------
class APP_VME_EXPORT appVMESurfaceParametric : public mafVME
{
public:

  mafTypeMacro(appVMESurfaceParametric, mafVME);
    
  enum PARAMETRIC_SURFACE_TYPE_ID
  {
    PARAMETRIC_SPHERE = 0,
    PARAMETRIC_CONE,
    PARAMETRIC_CYLINDER,
    PARAMETRIC_CUBE,
    PARAMETRIC_PLANE,
    PARAMETRIC_ELLIPSOID,
  };

  /** Set the geometry type to be generated: use PARAMETRIC_SURFACE_TYPE_ID as arg*/
  void SetGeometryType(int parametricSurfaceTypeID);

  /** Return the type of the parametric object.*/
  int GetGeometryType(){return m_GeometryType;};

  /** Set the radius for the parametric sphere.*/
  void SetSphereRadius(double radius);

  /** Return the radius of the parametric sphere.*/
  double GetSphereRadius() {return m_SphereRadius;};

  /** Return the radius of the parametric cylinder.*/
  double GetCylinderRadius() {return m_CylinderRadius;};

	/** Return the height of the parametric cylinder.*/
	double GetCylinderHeight() { return m_CylinderHeight; };

  /** Return the cylinder orientation axis.*/
  int GetCylinderAxis() {return m_CylinderOrientationAxis;};

  /** Copy the contents of another appVMESurfaceParametric into this one. */
  virtual int DeepCopy(mafVME *a);

  /** Compare with another appVMESurfaceParametric. */
  virtual bool Equals(mafVME *vme);

  /** return the right type of output */  
  mafVMEOutputSurface *GetSurfaceOutput();

  /**
  Return the list of timestamps for this VME. Timestamps list is 
  obtained merging timestamps for matrices and VME items*/
  virtual void GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes);

  /** return always false since (currently) the slicer is not an animated VME (position 
  is the same for all timestamps). */
  virtual bool IsAnimated();

  /** Return the suggested pipe-typename for the visualization of this vme */
  virtual mafString GetVisualPipe() {return mafString("mafPipeSurface");};

  /** Return pointer to material attribute. */
  mmaMaterial *GetMaterial();

  /** return an xpm-icon that can be used to represent this node */
  static char ** GetIcon();
	
  /** Precess events coming from other objects */ 
  virtual void OnEvent(mafEventBase *maf_event);

  /**
  Set the Pose matrix of the VME. This function modifies the MatrixVector. You can
  set or get the Pose for a specified time. When setting, if the time does not exist
  the MatrixVector creates a new KeyMatrix on the fly. When getting, the matrix vector
  interpolates on the fly according to the matrix interpolator.*/
  virtual void SetMatrix(const mafMatrix &mat);
	
	/** Returns EllipsoidXLenght */
	double GetEllipsoidXLenght() const { return m_EllipsoidXLenght; }

	/** Sets EllipsoidXLenght */
	void SetEllipsoidXLenght(double ellipsoidXLenght) { m_EllipsoidXLenght = ellipsoidXLenght; }
	
	/** Returns EllipsoidYLenght */
	double GetEllipsoidYLenght() const { return m_EllipsoidYLenght; }

	/** Sets EllipsoidYLenght */
	void SetEllipsoidYLenght(double ellipsoidYLenght) { m_EllipsoidYLenght = ellipsoidYLenght; }
	
	/** Returns EllipsoidZLenght */
	double GetEllipsoidZLenght() const { return m_EllipsoidZLenght; }

	/** Sets EllipsoidZLenght */
	void SetEllipsoidZLenght(double ellipsoidZLenght) { m_EllipsoidZLenght = ellipsoidZLenght; }

protected:

  appVMESurfaceParametric();
  virtual ~appVMESurfaceParametric();

  enum PARAMETRIC_SURFACE_WIDGET_ID
  {
    CHANGE_PARAMETER = Superclass::ID_LAST,
    CHANGE_VALUE_SPHERE,
    CHANGE_VALUE_CONE,
    CHANGE_VALUE_CYLINDER,
    CHANGE_VALUE_CUBE,
    CHANGE_VALUE_PLANE,
    CHANGE_VALUE_ELLIPSOID,
    ID_GEOMETRY_TYPE,
    ID_LAST
  };

  enum ID_ORIENTATION_AXIS
  {
    ID_X_AXIS = 0,
    ID_Y_AXIS,
    ID_Z_AXIS,
  };
  
  /** this function uses PARAMETRIC_SURFACE_TYPE_ID as argument*/
  void EnableParametricSurfaceGui(int surfaceTypeID);

	virtual int InternalStore(mafStorageElement *parent);
	virtual int InternalRestore(mafStorageElement *node);

	/** called to prepare the update of the output */
	virtual void InternalPreUpdate();

	/** update the output data structure */
	virtual void InternalUpdate();

  /** Internally used to create a new instance of the GUI.*/
  virtual mafGUI *CreateGui();

  void CreateGuiPlane();
  void EnableGuiPlane();

  void CreateGuiCube();
  void EnableGuiCube();

  void CreateGuiCylinder();
  void EnableGuiCylinder();

  void CreateGuiCone();
  void EnableGuiCone();

  void CreateGuiSphere();
  void EnableGuiSphere();

  void CreateGuiEllipsoid();
  void EnableGuiEllipsoid();
  
  mafGUI *m_GuiSphere;
  mafGUI *m_GuiCone;
  mafGUI *m_GuiCylinder;
  mafGUI *m_GuiCube;
  mafGUI *m_GuiPlane;
  mafGUI *m_GuiEllipsoid;

	mafTransform *m_Transform; 
	vtkPolyData  *m_PolyData;
  
  int m_GeometryType;
	double m_SphereRadius;
  double m_SpherePhiRes;
  double m_SphereTheRes;

  double m_ConeHeight;
  double m_ConeRadius;
  int m_ConeCapping;
  double m_ConeRes;
  int m_ConeOrientationAxis;

  double m_CylinderHeight;
  double m_CylinderRadius;
  double m_CylinderRes;
  int m_CylinderOrientationAxis;

  double m_CubeXLength;
  double m_CubeYLength;
  double m_CubeZLength;

  double m_PlaneXRes;
  double m_PlaneYRes;
  double m_PlaneOrigin[3];
  double m_PlanePoint1[3];
  double m_PlanePoint2[3];

  double m_EllipsoidXLenght;
  double m_EllipsoidYLenght;
  double m_EllipsoidZLenght;
  double m_EllipsoidPhiRes;
  double m_EllipsoidTheRes;
  int m_EllipsoidOrientationAxis;
  
private:

  appVMESurfaceParametric(const appVMESurfaceParametric&); // Not implemented
  void operator=(const appVMESurfaceParametric&); // Not implemented
};
#endif
