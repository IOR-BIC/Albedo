/*=========================================================================
Program:   Albedo
Module:    appOpInteractorSample.cpp
Language:  C++
Date:      $Date: 2018-01-01 12:00:00 $
Version:   $Revision: 1.0.0.0 $
Authors:   Nicola Vanella
==========================================================================
Copyright (c) LTM-IOR 2018 (https://github.com/IOR-LTM)

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

#include "mafDecl.h"
#include "mafEvent.h"
#include "mafGUI.h"

#include "mafTagArray.h"
#include "mafVMEVolumeRGB.h"
#include "mafVMEImage.h"
#include "mafVMEItem.h"

#include "vtkMAFSmartPointer.h"
#include "vtkImageData.h"
#include "vtkBMPReader.h"
#include "vtkJPEGReader.h"
#include "vtkPNGReader.h"
#include "vtkTIFFReader.h"
#include "vtkImageFlip.h"
#include "vtkImageResample.h"
#include "vtkPointData.h"

#include <algorithm>

//----------------------------------------------------------------------------
// Widgets ID's
//----------------------------------------------------------------------------
enum IMPORTER_IMAGE_ID
{
	ID_MEASURE_UNIT = MINID,
	ID_SET_CALIBRATION,
};

//----------------------------------------------------------------------------
mafCxxTypeMacro(appOpInteractorSample);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
appOpInteractorSample::appOpInteractorSample(const wxString &label) :
mafOp(label)
{
  m_OpType  = OPTYPE_OP;
  m_Canundo = true;

  m_Measure= 0;
  m_MeasureUnit = 0;

  m_ImportedImage = NULL;
  m_DistanceInteractor2D = NULL;
}
//----------------------------------------------------------------------------
appOpInteractorSample::~appOpInteractorSample()
{
  mafDEL(m_ImportedImage);
}

//----------------------------------------------------------------------------
bool appOpInteractorSample::Accept(mafVME *node)
{
  return true;
}
//----------------------------------------------------------------------------
void appOpInteractorSample::OpRun()
{
	// Open View if necessary
	mafEvent e(this, VIEW_SELECTED);
	mafEventMacro(e);
	if (!e.GetBool())
	{
		//mafEventMacro(mafEvent(this,ID_SHOW_IMAGE_VIEW));
	}
	
	m_DistanceInteractor2D = appInteractor2DSample::New();
	m_DistanceInteractor2D->ShowLabel(false);
	mafEventMacro(mafEvent(this, PER_PUSH, (mafObject *)m_DistanceInteractor2D));
	m_DistanceInteractor2D->SetListener(this);
	m_DistanceInteractor2D->SetColorSelection(0, 0, 1);
	m_DistanceInteractor2D->CanEditMeasures(false);

	mafString wildc = "Images (*.bmp;*.jpg;*.png;*.tif)| *.bmp;*.jpg;*.png;*.tif";

	if (!m_TestMode)
	{
		CreateGui();

		m_FileName = mafGetOpenFile("", wildc.GetCStr(), "Open Image");
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
		}
	}
}
//----------------------------------------------------------------------------
void appOpInteractorSample::CreateGui()
{
	m_Gui = new mafGUI(this);
	m_Gui->SetListener(this);

	m_Gui->Divider(2);

	m_Gui->Label("Calibration", true);

	const wxString unitChoices[] = { "Inch", "Centimeter" };
	m_Gui->Combo(ID_MEASURE_UNIT, "", &m_MeasureUnit, 2, unitChoices);
	m_Gui->Label("");
	m_Gui->Label("");
	m_Gui->Divider(1);

	m_Gui->OkCancel();
	m_Gui->Label("");

	m_Gui->Enable(wxOK, false);
}
//----------------------------------------------------------------------------
mafOp* appOpInteractorSample::Copy()
{
	return (new appOpInteractorSample(m_Label));
}
//----------------------------------------------------------------------------
void appOpInteractorSample::OpStop(int result)
{
	if (!m_TestMode)
	{
		HideGui();
	}

	if (result == OP_RUN_OK)
	{
		double unitToMm;

		unitToMm = (m_MeasureUnit == 0) ? 25.4 : 10;
		double scaleFactor = unitToMm / m_Measure;

		vtkImageData *toScale;
		toScale = vtkImageData::SafeDownCast(m_ImportedImage->GetOutput()->GetVTKData());
		double spacing[3];
		toScale->GetSpacing(spacing);
		spacing[0] *= scaleFactor;
		spacing[1] *= scaleFactor;
		spacing[2] *= scaleFactor;
		toScale->SetSpacing(spacing);
		m_ImportedImage->SetData(toScale, 0);

		GetLogicManager()->VmeVisualModeChanged(m_ImportedImage);
	}
	else
	{
		if (m_ImportedImage)
			m_ImportedImage->ReparentTo(NULL);
	}

	mafEventMacro(mafEvent(this, PER_POP));
	mafDEL(m_DistanceInteractor2D);

	mafEventMacro(mafEvent(this, result));
}

//----------------------------------------------------------------------------
void appOpInteractorSample::OnEvent(mafEventBase *maf_event)
{
	if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
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
			switch (e->GetId())
			{
			case appInteractor2DSample::ID_MEASURE_ADDED:
			case appInteractor2DSample::ID_MEASURE_CHANGED:
			{
				m_Measure = e->GetDouble();

				m_Gui->Enable(wxOK, m_Measure > 0.0);
				m_Gui->Update();

				while (m_DistanceInteractor2D->SizeMeasureVector() > 1)
				{
					m_DistanceInteractor2D->RemoveMeasure(0);
				}
			}
			break;
			}
		}
	}
}

//----------------------------------------------------------------------------
void appOpInteractorSample::ImportImage()
{
	BuildImage(); // Build image sequence

	mafTagItem tag_Nature;
	tag_Nature.SetName("VME_NATURE");
	tag_Nature.SetValue("NATURAL");

	m_Output->GetTagArray()->SetTag(tag_Nature);
}
//----------------------------------------------------------------------------
void appOpInteractorSample::BuildImage()
{
	wxString path, name, ext;

	mafNEW(m_ImportedImage);

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

	vtkMAFSmartPointer<vtkImageFlip> imageFlipFilter;

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
}


