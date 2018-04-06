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



//----------------------------------------------------------------------------
mafCxxTypeMacro(appOpInteractorSample);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
appOpInteractorSample::appOpInteractorSample(const wxString &label) :
mafOp(label)
{
  m_OpType  = OPTYPE_OP;
  m_Canundo = true;

	m_OneLine = 1;
	m_EditLine = 0;

  m_ImportedImage = NULL;
  m_Interactor = NULL;
}
//----------------------------------------------------------------------------
appOpInteractorSample::~appOpInteractorSample()
{
  mafDEL(m_ImportedImage);
}

//----------------------------------------------------------------------------
mafOp* appOpInteractorSample::Copy()
{
	return (new appOpInteractorSample(m_Label));
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
		mafEventMacro(mafEvent(this,ID_SHOW_IMAGE_VIEW));
	}
	
	// Create Interactor
	m_Interactor = appInteractor2DSample::New();
	mafEventMacro(mafEvent(this, PER_PUSH, (mafObject *)m_Interactor));
	m_Interactor->SetListener(this);
	m_Interactor->SetColorSelection(0, 0, 1);
	m_Interactor->CanEditLines(false);

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

	m_Gui->Label("Draw Line", true);
	m_Gui->Bool(-1, "One Line", &m_OneLine);
	m_Gui->Bool(ID_EDIT_LINE, "Edit Line", &m_EditLine);

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
	mafEventMacro(mafEvent(this, PER_POP));
	mafDEL(m_Interactor);

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
			case ID_EDIT_LINE:
			{
				m_Interactor->CanEditLines(m_EditLine == 1);
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
				if (m_OneLine == 1)
				{
					while (m_Interactor->SizeLineVector() > 1)
					{
						// Clear others lines
						m_Interactor->RemoveLine(0);
					}
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
	// Build image sequence

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

	// Set Tags
	mafTagItem tag_Nature;
	tag_Nature.SetName("VME_NATURE");
	tag_Nature.SetValue("NATURAL");

	m_Output->GetTagArray()->SetTag(tag_Nature);
}


