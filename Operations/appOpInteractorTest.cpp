/*=========================================================================
Program:   Albedo
Module:    appOpInteractorTest.cpp
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

#include "appOpInteractorTest.h"
#include "appDecl.h"

#include "appInteractor2DMeasure.h"
#include "appInteractor2DMeasure_Point.h"
#include "appInteractor2DMeasure_Distance.h"
#include "appInteractor2DMeasure_Indicator.h"
#include "appInteractor2DMeasure_Angle.h"
#include "appInteractor2DMeasure_DistancePoint.h"

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
albaCxxTypeMacro(appOpInteractorTest);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
appOpInteractorTest::appOpInteractorTest(const wxString &label) :
albaOp(label)
{
  m_OpType  = OPTYPE_OP;
  m_Canundo = true;

	m_SelectedMeasure = -1;
	m_MeasureText = "";
	m_MeasureTypeText = " ";

  m_ImportedImage = NULL;
  m_Interactor = NULL;
}
//----------------------------------------------------------------------------
appOpInteractorTest::~appOpInteractorTest()
{
  albaDEL(m_ImportedImage);
}

//----------------------------------------------------------------------------
albaOp* appOpInteractorTest::Copy()
{
	return (new appOpInteractorTest(m_Label));
}

//----------------------------------------------------------------------------
bool appOpInteractorTest::InternalAccept(albaVME *node)
{
  return true;
}

//----------------------------------------------------------------------------
void appOpInteractorTest::OpRun()
{
	// Open View if necessary
	albaEvent e(this, VIEW_SELECTED);
	albaEventMacro(e);
	if (!e.GetBool())
	{
		albaEventMacro(albaEvent(this,ID_SHOW_IMAGE_VIEW));
	}
	
	// Create Interactor
	m_Interactor = appInteractor2DMeasure::New();
	albaEventMacro(albaEvent(this, PER_PUSH, (albaObject *)m_Interactor));
	m_Interactor->SetListener(this);
// 	m_Interactor->SetColorSelection(0, 0, 1);
// 	m_Interactor->EnableEditMeasure(false);

	albaString wildc = "Images (*.bmp;*.jpg;*.png;*.tif)| *.bmp;*.jpg;*.png;*.tif";

	if (!m_TestMode)
	{
		CreateGui();

		//m_FileName = albaGetOpenFile("", wildc.GetCStr(), "Open Image");
	}

//	if (m_FileName == "")
//	{
//		OpStop(OP_RUN_CANCEL);
//	}
//	else
	{
//		ImportImage();
//		GetLogicManager()->VmeShow(m_ImportedImage, true);
//		GetLogicManager()->VmeSelect(m_Output);

		if (!m_TestMode)
		{
			ShowGui();

			albaEvent e(this, VIEW_SELECTED);
			albaEventMacro(e);

			if (e.GetBool())
			{
				m_Interactor->SetRendererByView(e.GetView());
// 				m_Interactor->EnableEditMeasure();
// 				m_MeasureTypeText = m_Interactor->GetMeasureType();

				m_Gui->Update();
			}
		}
	}

	//OpStop(OP_RUN_OK);
}

//----------------------------------------------------------------------------
void appOpInteractorTest::CreateGui()
{
	m_Gui = new albaGUI(this);
	m_Gui->SetListener(this);
	
	m_Gui->Divider(1);
	m_Gui->Label("Measure Info", true);
	m_Gui->String(ID_MEASURE, "Type", &m_MeasureTypeText);
	m_Gui->Integer(ID_MEASURE, "Selected", &m_SelectedMeasure);
	m_Gui->String(ID_MEASURE, "Measure", &m_MeasureText);
	
	//////////////////////////////////////////////////////////////////////////
	m_Gui->Enable(ID_MEASURE, false);

	m_Gui->Label("");
	m_Gui->Label("");
	m_Gui->Divider(1);

	m_Gui->OkCancel();
	m_Gui->Label("");
}

//----------------------------------------------------------------------------
void appOpInteractorTest::OpStop(int result)
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
void appOpInteractorTest::OnEvent(albaEventBase *alba_event)
{
	if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
		if (e->GetSender() == m_Gui)
		{
			switch (e->GetId())
			{
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

			if (m_Interactor)
			{
// 				m_SelectedMeasure = m_Interactor->GetSelectedMeasureIndex();
// 				m_MeasureText = m_Interactor->GetMeasure(m_SelectedMeasure);
			}
		}
	}
}

//----------------------------------------------------------------------------
void appOpInteractorTest::ClearMeasures(bool clearAll /*Default = false*/)
{
// 	int nLines = clearAll ? 0 : m_Interactor->GetMeasureCount();
// 	while (m_Interactor->GetMeasureCount() > nLines)
// 	{
// 		// Clear Extra lines
// 		m_Interactor->RemoveMeasure(0);
// 	}
}

//----------------------------------------------------------------------------
void appOpInteractorTest::ImportImage()
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