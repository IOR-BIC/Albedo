/*=========================================================================
Program:   Albedo
Module:    appOpInteractorSample.cpp
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

#include "appOpInteractorSample.h"
#include "appDecl.h"
#include "appInteractor2DSample.h"

#include "albaDecl.h"
#include "albaEvent.h"
#include "albaGUI.h"
#include "albaTagArray.h"
#include "albaVMEImage.h"
#include "albaVMEItem.h"
#include "albaVMEVolumeRGB.h"

/*#include "vtkALBASmartPointer.h"*/
#include "vtkBMPReader.h"
#include "vtkImageData.h"
#include "vtkImageFlip.h"
#include "vtkImageResample.h"
#include "vtkJPEGReader.h"
#include "vtkPNGReader.h"
#include "vtkPointData.h"
#include "vtkTIFFReader.h"

//----------------------------------------------------------------------------
albaCxxTypeMacro(appOpInteractorSample);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
appOpInteractorSample::appOpInteractorSample(const wxString &label) :
albaOp(label)
{
  m_OpType  = OPTYPE_OP;
  m_Canundo = true;

	m_NumLines = 5;
	m_EditLine = 1;
	m_ShowText = 1;
	m_ShowArrow = 0;

	m_SelectedMeasure = -1;
	m_Measure = 0;
	m_MeasureText = "";

	m_MeasureType = INTERACTION_TYPE::LINE;
	m_MeasureTypeText = "LINE";

  m_ImportedImage = NULL;
  m_Interactor = NULL;
}
//----------------------------------------------------------------------------
appOpInteractorSample::~appOpInteractorSample()
{
  albaDEL(m_ImportedImage);
}

//----------------------------------------------------------------------------
albaOp* appOpInteractorSample::Copy()
{
	return (new appOpInteractorSample(m_Label));
}

//----------------------------------------------------------------------------
bool appOpInteractorSample::InternalAccept(albaVME *node)
{
  return true;
}

//----------------------------------------------------------------------------
void appOpInteractorSample::OpRun()
{
	// Open View if necessary
	albaEvent e(this, VIEW_SELECTED);
	albaEventMacro(e);
	if (!e.GetBool())
	{
		albaEventMacro(albaEvent(this,ID_SHOW_IMAGE_VIEW));
	}
	
	// Create Interactor
	m_Interactor = appInteractor2DSample::New();
	albaEventMacro(albaEvent(this, PER_PUSH, (albaObject *)m_Interactor));
	m_Interactor->SetListener(this);
	m_Interactor->SetColorSelection(0, 0, 1);
	m_Interactor->EnableEditMeasure(false);

	albaString wildc = "Images (*.bmp;*.jpg;*.png;*.tif)| *.bmp;*.jpg;*.png;*.tif";

	if (!m_TestMode)
	{
		CreateGui();

		m_FileName = albaGetOpenFile("", wildc.GetCStr(), "Open Image");
	}

	if (m_FileName == "")
	{
		OpStop(OP_RUN_CANCEL);
	}
	else
	{
		ImportImage();
		GetLogicManager()->VmeShow(m_ImportedImage, true);
		GetLogicManager()->VmeSelect(m_Output);

		if (!m_TestMode)
		{
			ShowGui();

			albaEvent e(this, VIEW_SELECTED);
			albaEventMacro(e);

			if (e.GetBool())
			{
				m_Interactor->SetRendererByView(e.GetView());

				m_Interactor->EnableEditMeasure(m_EditLine == 1);
				m_Interactor->ShowText(m_ShowText == 1);
				m_Interactor->ShowArrow(m_ShowArrow == 1);

				// Test
				//m_Interactor->SetTextSide(2);
				//m_Interactor->SetOpacity(0.5);
			}
		}
	}
}

//----------------------------------------------------------------------------
void appOpInteractorSample::CreateGui()
{
	m_Gui = new albaGUI(this);
	m_Gui->SetListener(this);

	m_Gui->Divider(1);
	m_Gui->Label("Draw Line", true);
	m_Gui->Integer(ID_NUM_LINES, "Lines", &m_NumLines, 1);
	m_Gui->Bool(ID_EDIT_LINE, "Edit Line", &m_EditLine,1);
	m_Gui->Bool(ID_SHOW_TEXT, "Show Text", &m_ShowText,1);
	m_Gui->Bool(ID_SHOW_ARROW, "Show Arrow", &m_ShowArrow,1);

	m_Gui->Divider(1);
	m_Gui->Label("Measure", true);
	m_Gui->String(ID_MEASURE, "Type", &m_MeasureTypeText);
	m_Gui->Integer(ID_MEASURE, "Selected", &m_SelectedMeasure);
	m_Gui->Double(ID_MEASURE, "Measure", &m_Measure);
	
	m_Gui->Divider(1);
	m_Gui->Label("Storage", true);
	m_Gui->TwoButtons(ID_LOAD_MEASURES, ID_SAVE_MEASURES, "Load", "Save");

	//////////////////////////////////////////////////////////////////////////
	m_Gui->Enable(ID_MEASURE, false);

	m_Gui->Label("");
	m_Gui->Label("");
	m_Gui->Divider(1);

	m_Gui->OkCancel();
	m_Gui->Label("");
}

//----------------------------------------------------------------------------
void appOpInteractorSample::OpStop(int result)
{
	if (!m_TestMode)
	{
		HideGui();
	}
	
	// Remove Interactor
	albaEventMacro(albaEvent(this, PER_POP));
	albaDEL(m_Interactor);

	albaEventMacro(albaEvent(this, result));
}

//----------------------------------------------------------------------------
void appOpInteractorSample::OnEvent(albaEventBase *alba_event)
{
	if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
		if (e->GetSender() == m_Gui)
		{
			switch (e->GetId())
			{
			case ID_LOAD_MEASURES:
			{		
				ClearMeasures(true);
				Load();
			}
			break;
			case ID_SAVE_MEASURES:
			{
				Save();
			}
			break;
			case ID_EDIT_LINE:
			{
				m_Interactor->EnableEditMeasure(m_EditLine == 1);
			}
			break;

			case ID_SHOW_TEXT:
			{
				m_Interactor->ShowText(m_ShowText == 1);
			}
			break;

			case ID_SHOW_ARROW:
			{
				m_Interactor->ShowArrow(m_ShowArrow == 1);
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
			case appInteractor2DSample::ID_LINE_ADDED:
			case appInteractor2DSample::ID_LINE_CHANGED:
			{
				// Update Measure Gui Entry
				m_SelectedMeasure = m_Interactor->GetSelectedMeasureIndex();
				m_Measure = RoundValue(e->GetDouble());
				m_Gui->Update();

				ClearMeasures();
			}
			break;
			case appInteractor2DSample::ID_LINE_SELECTED:
			{				
				// Update Measure Gui Entry
				m_SelectedMeasure = m_Interactor->GetSelectedMeasureIndex();
				m_Measure = m_Interactor->GetMeasure(m_SelectedMeasure);
				m_Gui->Update();
			}
			break;
			}
		}
	}
}

//----------------------------------------------------------------------------
void appOpInteractorSample::ClearMeasures(bool clearAll /*Default = false*/)
{
	int nLines = clearAll ? 0 : m_NumLines;
	while (m_Interactor->GetMeasureCount() > nLines)
	{
		// Clear Extra lines
		m_Interactor->RemoveMeasure(0);
	}
}

//----------------------------------------------------------------------------
void appOpInteractorSample::ImportImage()
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

//----------------------------------------------------------------------------
void appOpInteractorSample::Load()
{
	wxString tag = "";
	albaVME *input = m_Input->GetRoot();

	if (input->GetTagArray()->IsTagPresent(tag + "MeasureLineP1") &&
		input->GetTagArray()->IsTagPresent(tag + "MeasureLineP2"))
	{
		double point1[3], point2[3];
		albaTagItem *measureLineTypeTag = input->GetTagArray()->GetTag(tag + "MeasureLineType");
		albaTagItem *measureLineP1Tag = input->GetTagArray()->GetTag(tag + "MeasureLineP1");
		albaTagItem *measureLineP2Tag = input->GetTagArray()->GetTag(tag + "MeasureLineP2");

		int nLines = measureLineP1Tag->GetNumberOfComponents() / 2;

		// Reload line points
		for (int i = 0; i < nLines; i++)
		{
			point1[0] = measureLineP1Tag->GetValueAsDouble(i * 2 + 0);
			point1[1] = measureLineP1Tag->GetValueAsDouble(i * 2 + 1);
			point1[2] = 0.0;

			point2[0] = measureLineP2Tag->GetValueAsDouble(i * 2 + 0);
			point2[1] = measureLineP2Tag->GetValueAsDouble(i * 2 + 1);
			point2[2] = 0.0;

			albaString measureType = measureLineTypeTag->GetValue(i);

			if (measureType == "LINE")
			{
				m_MeasureType = INTERACTION_TYPE::LINE;
				m_Interactor->AddMeasure(point1, point2);
				m_MeasureTypeText = measureType;
			}
			if (measureType == "INDICATOR")
			{
				m_MeasureType = INTERACTION_TYPE::INDICATOR;
				m_Interactor->AddMeasure(point1, point2);
				m_MeasureTypeText = measureType;
			}
		}
		m_Gui->Update();
		m_Interactor->SelectMeasure(-1);
		GetLogicManager()->CameraUpdate();
	}
}
//----------------------------------------------------------------------------
void appOpInteractorSample::Save()
{
	wxString tag = "";
	albaVME *input = m_Input->GetRoot();

	// Saving lines to image tag to restore them on operation recall
	int nLines = m_Interactor->GetMeasureCount();

	albaTagItem measureLineTypeTag;
	measureLineTypeTag.SetName(tag + "MeasureLineType");
	measureLineTypeTag.SetNumberOfComponents(nLines);

	albaTagItem measureLineP1Tag;
	measureLineP1Tag.SetName(tag + "MeasureLineP1");
	measureLineP1Tag.SetNumberOfComponents(nLines * 2);

	albaTagItem measureLineP2Tag;
	measureLineP2Tag.SetName(tag + "MeasureLineP2");
	measureLineP2Tag.SetNumberOfComponents(nLines * 2);

	for (int i = 0; i < nLines; i++)
	{
		double point1[3], point2[3];
		m_Interactor->GetMeasureLinePoints(i, point1, point2);

		albaString measureType = "LINE";

		if (m_MeasureType == INTERACTION_TYPE::LINE)
			measureType = "LINE";
		if (m_MeasureType == INTERACTION_TYPE::INDICATOR)
			measureType = "INDICATOR";

		measureLineTypeTag.SetValue(measureType, i);

		measureLineP1Tag.SetValue(point1[0], i * 2 + 0);
		measureLineP1Tag.SetValue(point1[1], i * 2 + 1);
		measureLineP2Tag.SetValue(point2[0], i * 2 + 0);
		measureLineP2Tag.SetValue(point2[1], i * 2 + 1);
	}

	if (input->GetTagArray()->IsTagPresent(tag + "MeasureLineType"))
		input->GetTagArray()->DeleteTag(tag + "MeasureLineType");

	if (input->GetTagArray()->IsTagPresent(tag + "MeasureLineP1"))
		input->GetTagArray()->DeleteTag(tag + "MeasureLine1P1");

	if (input->GetTagArray()->IsTagPresent(tag + "MeasureLineP2"))
		input->GetTagArray()->DeleteTag(tag + "MeasureLine1P2");

	input->GetTagArray()->SetTag(measureLineTypeTag);
	input->GetTagArray()->SetTag(measureLineP1Tag);
	input->GetTagArray()->SetTag(measureLineP2Tag);
}