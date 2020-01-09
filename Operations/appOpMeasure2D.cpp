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

	m_NumLines = 5;

	m_SelectedMeasure = -1;
	m_Measure = "";
	m_MeasureType = "";

  m_ImportedImage = NULL;

	m_MeasureListBox = NULL;

	m_InteractorPoint = NULL;
	m_InteractorDistance = NULL;
	m_InteractorIndicator = NULL;
	m_InteractorAngle = NULL;

	m_SelectedInteractor = m_CurrentInteractor = INTERACTION_TYPE::INDICATOR;
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
				m_InteractorPoint->SetRendererByView(e.GetView());
				m_InteractorDistance->SetRendererByView(e.GetView());
				m_InteractorAngle->SetRendererByView(e.GetView());
				m_InteractorIndicator->SetRendererByView(e.GetView());
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
// 	if (m_Input->GetTagArray() && m_Input->GetTagArray()->IsTagPresent("MeasureLineType"))
// 	{
// 		wxString description = "Trovate Misure. \nCaricarle?";
// 		int result = wxMessageBox(description, "Load Measures", wxYES_NO | wxYES_DEFAULT | wxICON_QUESTION | wxCENTRE | wxSTAY_ON_TOP);
// 
// 		if (result == wxYES)
// 		{ 
// 			Load();
// 		}
// 	}
}

//----------------------------------------------------------------------------
void appOpMeasure2D::InitInteractors()
{
	// Create Interactor Point
	m_InteractorPoint = appInteractor2DMeasure_Point::New();
	if (m_CurrentInteractor == POINT) albaEventMacro(albaEvent(this, PER_PUSH, (albaObject *)m_InteractorPoint));
	m_InteractorPoint->SetListener(this);
	m_InteractorPoint->SetColor(0.5, 0, 1);
	m_InteractorPoint->EnableEditMeasure(true);
	m_InteractorVector.push_back(m_InteractorPoint);

	// Create Interactor Distance
	m_InteractorDistance = appInteractor2DMeasure_Distance::New();
	if (m_CurrentInteractor == DISTANCE)	albaEventMacro(albaEvent(this, PER_PUSH, (albaObject *)m_InteractorDistance));
	m_InteractorDistance->SetListener(this);
	m_InteractorDistance->SetColor(0, 0, 1);
	m_InteractorDistance->EnableEditMeasure(true);
	m_InteractorVector.push_back(m_InteractorDistance);

	// Create Interactor Indicator
	m_InteractorIndicator = appInteractor2DMeasure_Indicator::New();
	if (m_CurrentInteractor == INDICATOR) albaEventMacro(albaEvent(this, PER_PUSH, (albaObject *)m_InteractorIndicator));
	m_InteractorIndicator->SetListener(this);
	m_InteractorIndicator->SetColor(0, 1, 1);
	m_InteractorIndicator->EnableEditMeasure(true);
	m_InteractorVector.push_back(m_InteractorIndicator);

	// Create Interactor Angle
	m_InteractorAngle = appInteractor2DMeasure_Angle::New();
	if (m_CurrentInteractor == ANGLE)	albaEventMacro(albaEvent(this, PER_PUSH, (albaObject *)m_InteractorAngle));
	m_InteractorAngle->SetListener(this);
	m_InteractorAngle->SetColor(1, 0, 1);
	m_InteractorAngle->EnableEditMeasure(true);
	m_InteractorVector.push_back(m_InteractorAngle);

	m_MeasureType = m_InteractorVector[m_CurrentInteractor]->GetMeasureType();
}

//----------------------------------------------------------------------------
void appOpMeasure2D::CreateGui()
{
	m_Gui = new albaGUI(this);
	m_Gui->SetListener(this);

	m_Gui->Divider(1);
	wxString choises[4] = { _("Point"),_("Distance"),_("Indicator"),_("Angle (to fix)") };
	m_Gui->Combo(ID_SELECT_INTERACTOR, "", &m_SelectedInteractor, 4, choises, "Select Measure Type");
	
	m_MeasureListBox = m_Gui->ListBox(ID_MEASURE_LIST, "", 200);
	m_Gui->Divider();
	m_Gui->Button(ID_REMOVE_MEASURE, _("Remove"));

	m_Gui->Divider(1);
	m_Gui->Label("Measure", true);
	m_Gui->String(ID_MEASURE, "Type", &m_MeasureType);
	m_Gui->Integer(ID_MEASURE, "Selected", &m_SelectedMeasure);
	m_Gui->String(ID_MEASURE, "Measure", &m_Measure);

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
		HideGui();
	}
	
	// Remove Interactor
	albaEventMacro(albaEvent(this, PER_POP));
	albaDEL(m_InteractorPoint);
	albaDEL(m_InteractorDistance);
	albaDEL(m_InteractorIndicator);
	albaDEL(m_InteractorAngle);

	m_InteractorVector.clear();

	albaEventMacro(albaEvent(this, result));
}

//----------------------------------------------------------------------------
void appOpMeasure2D::OnEvent(albaEventBase *alba_event)
{
	if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
		if (e->GetSender() == m_Gui)
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
			switch (e->GetId())
			{
			case appInteractor2DMeasure::ID_MEASURE_ADDED:
			case appInteractor2DMeasure::ID_MEASURE_CHANGED:
			{
				// Update Measure Gui Entry
				m_SelectedMeasure = m_InteractorVector[m_CurrentInteractor]->GetSelectedMeasureIndex();
				m_Measure = RoundValue(e->GetDouble());
				m_Gui->Update();

				UpdateMeasureList();
			}
			break;
			case appInteractor2DMeasure::ID_MEASURE_SELECTED:
			{				
				// Update Measure Gui Entry
				m_SelectedMeasure = m_InteractorVector[m_CurrentInteractor]->GetSelectedMeasureIndex();
				m_Measure = m_InteractorVector[m_CurrentInteractor]->GetMeasure(m_SelectedMeasure);

				m_MeasureListBox->Select(m_SelectedMeasure);
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
				m_MeasureListBox->Append(_(m_InteractorVector[m_CurrentInteractor]->GetMeasure(i)));
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
		int sel = m_MeasureListBox->GetSelection();

		m_InteractorVector[m_CurrentInteractor]->RemoveMeasure(sel);
		m_MeasureListBox->Delete(sel);

		m_InteractorVector[m_CurrentInteractor]->SelectMeasure(sel - 1);
		m_MeasureListBox->Select(sel - 1);

		((albaGUIValidator *)m_MeasureListBox->GetValidator())->TransferFromWindow();
		m_MeasureListBox->Update();
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

// LOAD/SAVE
//----------------------------------------------------------------------------
void appOpMeasure2D::Load()
{
	wxString tag = "";
	albaVME *input = m_Input->GetRoot();

	m_InteractorVector[m_CurrentInteractor]->Load(input, tag);

	UpdateMeasureList();
}
//----------------------------------------------------------------------------
void appOpMeasure2D::Save()
{
	wxString tag = "";
	albaVME *input = m_Input->GetRoot();

	m_InteractorVector[m_CurrentInteractor]->Save(input, tag);
}