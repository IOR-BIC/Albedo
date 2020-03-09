/*=========================================================================
Program:   Albedo
Module:    appOpMeasure2D.cpp
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

#include "appDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the APP must include "appDefines.h" as first.
// This force to include Window, wxWidgets and VTK exactly in this order.
//----------------------------------------------------------------------------

#include "appOpMeasure2D.h"
#include "appDecl.h"

#include "appInteractor2DMeasure_Point.h"
#include "appInteractor2DMeasure_Distance.h"
#include "appInteractor2DMeasure_Indicator.h"
#include "appInteractor2DMeasure_DistancePoint.h"
#include "appInteractor2DMeasure_Angle.h"

#include "albaDecl.h"
#include "albaEvent.h"
#include "albaGUI.h"
#include "albaGUIValidator.h"
#include "albaTagArray.h"
#include "albaVMEImage.h"
#include "albaVMEItem.h"
#include "albaVMEVolumeRGB.h"

#include "vtkBMPReader.h"
#include "vtkImageData.h"
#include "vtkImageFlip.h"
#include "vtkImageResample.h"
#include "vtkJPEGReader.h"
#include "vtkPNGReader.h"
#include "vtkPointData.h"
#include "vtkTIFFReader.h"

//----------------------------------------------------------------------------
albaCxxTypeMacro(appOpMeasure2D);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
appOpMeasure2D::appOpMeasure2D(const wxString &label) :
albaOp(label)
{
  m_OpType  = OPTYPE_OP;
  m_Canundo = true;

	m_InteractorVector.clear();
	m_SelectedInteractor = m_CurrentInteractor = INTERACTION_TYPE::POINT;

	m_MeasureListBox = NULL;

	m_SelectedMeasure = -1;
	m_Measure = "";
	m_MeasureLabel = "";
	m_MeasureType = "";

  m_ImportedImage = NULL; // For Test
}
//----------------------------------------------------------------------------
appOpMeasure2D::~appOpMeasure2D()
{
  albaDEL(m_ImportedImage);
}

//----------------------------------------------------------------------------
albaOp* appOpMeasure2D::Copy()
{
	return (new appOpMeasure2D(m_Label));
}

//----------------------------------------------------------------------------
bool appOpMeasure2D::Accept(albaVME *node)
{
  return true;
	//return node && node->IsA("albaVMEImage");
}

//----------------------------------------------------------------------------
char ** appOpMeasure2D::GetIcon()
{
#include "pic/MENU_OP_MEASURE_2D.xpm"
	return MENU_OP_MEASURES_2D_xpm;
}

//----------------------------------------------------------------------------
void appOpMeasure2D::OpRun()
{
	// Open View if necessary
	albaEvent e(this, VIEW_SELECTED);
	albaEventMacro(e);
	if (!e.GetBool())
	{
		albaEventMacro(albaEvent(this,ID_SHOW_IMAGE_VIEW));
	}

	InitInteractors();

	//////////////////////////////////////////////////////////////////////////
	albaString wildc = "Images (*.bmp;*.jpg;*.png;*.tif)| *.bmp;*.jpg;*.png;*.tif";

	bool res  = m_Input && m_Input->IsA("albaVMEImage");

	if (!m_TestMode)
	{
		CreateGui();

		if(!res) m_FileName = albaGetOpenFile("", wildc.GetCStr(), "Open Image");
	}

	if (m_FileName == "" && (!res))
	{
		OpStop(OP_RUN_CANCEL);
	}
	else
	{
		if (!res)
		{
			ImportImage();
		}

		GetLogicManager()->VmeShow(m_ImportedImage, true);
		GetLogicManager()->VmeSelect(m_Output);

		if (!m_TestMode)
		{
			ShowGui();

			albaEvent e(this, VIEW_SELECTED);
			albaEventMacro(e);

			if (e.GetBool())
			{
				for(int i=0; i<m_InteractorVector.size();i++)
				m_InteractorVector[i]->SetRendererByView(e.GetView());
			}
		}
	}
}

//----------------------------------------------------------------------------
void appOpMeasure2D::InitInteractors()
{
	// Create Interactor Point
	appInteractor2DMeasure_Point *InteractorPoint = appInteractor2DMeasure_Point::New();
	if (m_CurrentInteractor == POINT) albaEventMacro(albaEvent(this, PER_PUSH, (albaObject *)InteractorPoint));
	InteractorPoint->SetListener(this);
	InteractorPoint->SetColor(0.5, 0, 1);
	InteractorPoint->EnableEditMeasure(true);
	InteractorPoint->HidePoints();
	m_InteractorVector.push_back(InteractorPoint);
	
	// Create Interactor Distance
	appInteractor2DMeasure_Distance *InteractorDistance = appInteractor2DMeasure_Distance::New();
	if (m_CurrentInteractor == DISTANCE)	albaEventMacro(albaEvent(this, PER_PUSH, (albaObject *)InteractorDistance));
	InteractorDistance->SetListener(this);
	InteractorDistance->SetColor(0, 0, 1);
	InteractorDistance->EnableEditMeasure(true);
	m_InteractorVector.push_back(InteractorDistance);

	// Create Interactor Indicator
	appInteractor2DMeasure_Indicator *InteractorIndicator = appInteractor2DMeasure_Indicator::New();
	if (m_CurrentInteractor == INDICATOR) albaEventMacro(albaEvent(this, PER_PUSH, (albaObject *)InteractorIndicator));
	InteractorIndicator->SetListener(this);
	InteractorIndicator->SetColor(0, 1, 1);
	InteractorIndicator->EnableEditMeasure(true);
	m_InteractorVector.push_back(InteractorIndicator);

	// Create Interactor Distance Point
	appInteractor2DMeasure_DistancePoint *InteractorDistancePoint = appInteractor2DMeasure_DistancePoint::New();
	if (m_CurrentInteractor == DISTANCE_POINT) albaEventMacro(albaEvent(this, PER_PUSH, (albaObject *)InteractorDistancePoint));
	InteractorDistancePoint->SetListener(this);
	InteractorDistancePoint->SetColor(0, 1, 1);
	InteractorDistancePoint->EnableEditMeasure(true);
	m_InteractorVector.push_back(InteractorDistancePoint);

	// Create Interactor Angle // TO FIX
//  appInteractor2DMeasure_Angle *InteractorAngle = appInteractor2DMeasure_Angle::New();
//  if (m_CurrentInteractor == ANGLE)	albaEventMacro(albaEvent(this, PER_PUSH, (albaObject *)InteractorAngle));
// 	InteractorAngle->SetListener(this);
// 	InteractorAngle->SetColor(1, 0, 1);
// 	InteractorAngle->EnableEditMeasure(true);
// 	InteractorAngle.push_back(m_InteractorAngle);

	m_MeasureType = m_InteractorVector[m_CurrentInteractor]->GetMeasureType();
}

//----------------------------------------------------------------------------
void appOpMeasure2D::CreateGui()
{
	m_Gui = new albaGUI(this);
	m_Gui->SetListener(this);

	m_Gui->Divider(1);
	wxString choises[5] = { _("Point"),_("Distance"),_("Indicator"),_("Distance Point"),_("Angle (to fix)") };
	m_Gui->Combo(ID_SELECT_INTERACTOR, "", &m_SelectedInteractor, 5, choises, "Select Measure Type");
	
	m_MeasureListBox = m_Gui->ListBox(ID_MEASURE_LIST, "", 200);
	m_Gui->Divider();
	m_Gui->Button(ID_REMOVE_MEASURE, _("Remove"));

	m_Gui->Divider(1);
	m_Gui->Label("Measure", true);
	m_Gui->String(ID_MEASURE, "Type", &m_MeasureType);
	m_Gui->Integer(ID_MEASURE, "Selected", &m_SelectedMeasure);
	m_Gui->String(ID_MEASURE, "Measure", &m_Measure);
	m_Gui->String(ID_MEASURE_LAB, "Label", &m_MeasureLabel);

	m_Gui->Divider(1);
	m_Gui->Label("Storage", true);
	m_Gui->TwoButtons(ID_LOAD_MEASURES, ID_SAVE_MEASURES, "Load", "Save");

	//////////////////////////////////////////////////////////////////////////
	m_Gui->Enable(ID_MEASURE, false);
	m_Gui->Enable(ID_REMOVE_MEASURE, m_MeasureListBox->GetCount() != 0);

	m_Gui->Label("");
	m_Gui->Label("");
	m_Gui->Divider(1);

	m_Gui->OkCancel();
	m_Gui->Label("");
}

//----------------------------------------------------------------------------
void appOpMeasure2D::OpStop(int result)
{
	if (!m_TestMode)
	{
		m_MeasureListBox->Clear();
		HideGui();
	}
	
	// Remove Interactor
	albaEventMacro(albaEvent(this, PER_POP));

	for (int i = 0; i < m_InteractorVector.size(); i++)
		albaDEL(m_InteractorVector[i]);

	m_InteractorVector.clear();

	albaEventMacro(albaEvent(this, result));
}

//----------------------------------------------------------------------------
void appOpMeasure2D::OnEvent(albaEventBase *alba_event)
{
	if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
		if (e->GetSender() == m_Gui) // FROM GUI
		{
			switch (e->GetId())
			{
			case ID_SELECT_INTERACTOR:
			{
				SetMeasureInteractor(m_SelectedInteractor);
			}
			break;
			case ID_MEASURE_LIST:
			{
				int selection = m_MeasureListBox->GetSelection();
				m_InteractorVector[m_CurrentInteractor]->SelectMeasure(selection);
				m_Gui->Update();
			}
			break;

			case ID_MEASURE_LAB:
			{
				int nMeasures = m_InteractorVector[m_CurrentInteractor]->GetMeasureCount();
// 				if (nMeasures > 0 && m_SelectedMeasure >= 0 && m_SelectedMeasure < nMeasures)
// 					m_InteractorVector[m_CurrentInteractor]->SetMeasureLabel(m_SelectedMeasure, m_MeasureLabel);

				m_InteractorVector[m_CurrentInteractor]->Update(m_SelectedMeasure);
				UpdateMeasureList();
			}
			break;

			case ID_REMOVE_MEASURE:
			{
				RemoveMeasure();
			}
			break;

			case ID_LOAD_MEASURES:
			{
				RemoveMeasure(true);
				Load();
			}
			break;
			case ID_SAVE_MEASURES:
			{
				Save();
			}
			break;

			case wxOK:
				OpStop(OP_RUN_OK);
				break;

			case wxCANCEL:
				OpStop(OP_RUN_CANCEL);
				break;
			}
		}
		else
		{
			switch (e->GetId()) // FROM INTERACTOR
			{
			case appInteractor2DMeasure::ID_MEASURE_ADDED:
			case appInteractor2DMeasure::ID_MEASURE_CHANGED:
			{
				// Update Measure Gui Entry
				m_SelectedMeasure = m_InteractorVector[m_CurrentInteractor]->GetSelectedMeasureIndex();
				m_Measure = m_InteractorVector[m_CurrentInteractor]->GetMeasure(m_SelectedMeasure);
				m_Gui->Update();

				UpdateMeasureList();
			}
			break;
			case appInteractor2DMeasure::ID_MEASURE_SELECTED:
			{				
				// Update Measure Gui Entry
				m_SelectedMeasure = m_InteractorVector[m_CurrentInteractor]->GetSelectedMeasureIndex();
				m_Measure = m_InteractorVector[m_CurrentInteractor]->GetMeasure(m_SelectedMeasure);
				m_MeasureLabel = m_InteractorVector[m_CurrentInteractor]->GetMeasureLabel(m_SelectedMeasure);

				//m_MeasureListBox->Select(m_SelectedMeasure); // TO FIX
				m_Gui->Update();
			}
			break;
			}
		}
	}
}

//----------------------------------------------------------------------------
void appOpMeasure2D::SetMeasureInteractor(int index)
{
	m_InteractorVector[m_CurrentInteractor]->Disable();

	m_CurrentInteractor = index;

	albaEventMacro(albaEvent(this, PER_POP));
	albaEventMacro(albaEvent(this, PER_PUSH, (albaObject *)m_InteractorVector[m_CurrentInteractor]));

	m_InteractorVector[m_CurrentInteractor]->SetListener(this);
	m_InteractorVector[m_CurrentInteractor]->Enable();

	m_MeasureType = m_InteractorVector[m_CurrentInteractor]->GetMeasureType();

	UpdateMeasureList();
}

//----------------------------------------------------------------------------
void appOpMeasure2D::UpdateMeasureList()
{
	if (m_MeasureListBox)
	{
		m_MeasureListBox->Clear();

		if (m_InteractorVector.size() > 0)
			for (int i = 0; i < m_InteractorVector[m_CurrentInteractor]->GetMeasureCount(); i++)
			{
				wxString measure;/* = m_InteractorVector[m_CurrentInteractor]->GetMeasureLabel(i);
				if (measure.IsEmpty())*/
					measure = m_InteractorVector[m_CurrentInteractor]->GetMeasure(i);

				m_MeasureListBox->Append(_(measure));
			}

		m_Gui->Enable(ID_REMOVE_MEASURE, m_MeasureListBox->GetCount() != 0);

		GetLogicManager()->CameraUpdate();
	}
}

//----------------------------------------------------------------------------
void appOpMeasure2D::RemoveMeasure(bool clearAll /*Default = false*/)
{
	if (clearAll)
	{
		m_InteractorVector[m_CurrentInteractor]->RemoveAllMeasures();
		m_MeasureListBox->Clear();
	}
	else
	{
		int sel = m_SelectedMeasure;

		if (sel >= 0 && sel < m_InteractorVector[m_CurrentInteractor]->GetMeasureCount())
		{
			m_InteractorVector[m_CurrentInteractor]->RemoveMeasure(sel);
			m_MeasureListBox->Delete(sel);

			if ((sel - 1) >= 0 && (sel - 1) < m_InteractorVector[m_CurrentInteractor]->GetMeasureCount())
			{
				m_InteractorVector[m_CurrentInteractor]->SelectMeasure(sel - 1);
				m_MeasureListBox->Select(sel - 1);
			}

			((albaGUIValidator *)m_MeasureListBox->GetValidator())->TransferFromWindow();
			m_MeasureListBox->Update();
		}
	}

	m_Gui->Enable(ID_REMOVE_MEASURE, m_MeasureListBox->GetCount() != 0);
}

//----------------------------------------------------------------------------
void appOpMeasure2D::ImportImage()
{
	// Build image sequence

	wxString path, name, ext;

	albaNEW(m_ImportedImage);

	vtkImageReader2 *imageReader;

	wxSplitPath(m_FileName.c_str(), &path, &name, &ext);
	ext.MakeUpper();

	if (ext == "BMP")
		imageReader = vtkImageReader::New();
	else if (ext == "JPG" || ext == "JPEG")
		imageReader = vtkJPEGReader::New();
	else if (ext == "PNG")
		imageReader = vtkPNGReader::New();
	else if (ext == "TIF" || ext == "TIFF")
		imageReader = vtkTIFFReader::New();
	else
	{
		wxMessageBox("Unable to import %s, unrecognized type", m_FileName.c_str());
		m_ImportedImage->ReparentTo(NULL);
		return;
	}

	imageReader->SetFileName(m_FileName.c_str());
	imageReader->Update();

	vtkALBASmartPointer<vtkImageFlip> imageFlipFilter;

	imageFlipFilter->SetFilteredAxis(1); // flip z axis
	imageFlipFilter->SetInput(imageReader->GetOutput());
	imageFlipFilter->Update();

	vtkImageData *grayScaleImage;
	vtkNEW(grayScaleImage);

	grayScaleImage->DeepCopy(imageFlipFilter->GetOutput());
	grayScaleImage->Update();

	for (int i = 0; i < grayScaleImage->GetNumberOfPoints(); i++)
	{
		double *values, meanValue;
		values = grayScaleImage->GetPointData()->GetScalars()->GetTuple3(i);
		meanValue = (values[0] + values[1] + values[2]) / 3.0;
		grayScaleImage->GetPointData()->GetScalars()->SetTuple3(i, meanValue, meanValue, meanValue);
	}
	grayScaleImage->Update();

	m_ImportedImage->SetData(grayScaleImage, 0);
	m_ImportedImage->SetName(name);

	m_ImportedImage->SetTimeStamp(0);
	m_ImportedImage->ReparentTo(m_Input);

	m_Output = m_ImportedImage;

	vtkDEL(grayScaleImage);
	vtkDEL(imageReader);

	// Set Tags
	albaTagItem tag_Nature;
	tag_Nature.SetName("VME_NATURE");
	tag_Nature.SetValue("NATURAL");

	m_Output->GetTagArray()->SetTag(tag_Nature);
}

/// LOAD/SAVE - TODO
//----------------------------------------------------------------------------
void appOpMeasure2D::Load()
{
	wxString tag = "";
	albaVME *input = m_Input->GetRoot();

	m_InteractorVector[m_CurrentInteractor]->Load(input, tag);
	m_InteractorVector[m_CurrentInteractor]->Update();

	UpdateMeasureList();
}
//----------------------------------------------------------------------------
void appOpMeasure2D::Save()
{
	wxString tag = "";
	albaVME *input = m_Input->GetRoot();

	m_InteractorVector[m_CurrentInteractor]->Save(input, tag);
}