/*=========================================================================
Program:   Albedo
Module:    appOpDictionary.cpp
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

#include "appOpDictionary.h"

#include "appDecl.h"

#include "albaGUI.h"
#include "albaGUIDictionaryWidget.h"
#include "albaVME.h"
#include "albaXMLString.h"

#include "mmuDOMTreeErrorReporter.h"

#include "xercesc\dom\DOMDocument.hpp"
#include "xercesc\framework\LocalFileFormatTarget.hpp"
#include "xercesc\parsers\XercesDOMParser.hpp"
#include "xercesc\util\PlatformUtils.hpp"
#include "xercesc\util\XercesDefs.hpp"

//----------------------------------------------------------------------------
albaCxxTypeMacro(appOpDictionary);

//----------------------------------------------------------------------------
appOpDictionary::appOpDictionary(wxString label) :albaOp(label)
{
	m_OpType = OPTYPE_OP;
	m_Canundo = true;
	
	m_DictionaryName = "Dictionary";
	m_DictionaryVersion = "1.0";

	m_DictionaryTypeName = "Dictionary";
	m_DictionaryGroupTypeName = "Group";
	m_DictionaryElementTypeName = "Element";

	m_SelectedGroup = 0;
	m_SelectedElement = "";

	m_ElementToEdit = "";
	m_GroupToEdit = "";

	m_IncrementalName = 0;

	m_ElementVect.clear();
	m_ElementDict = NULL;
}

//----------------------------------------------------------------------------
appOpDictionary::~appOpDictionary()
{
	m_ElementVect.clear();
	m_GroupVect.clear();
}

//----------------------------------------------------------------------------
bool appOpDictionary::Accept(albaVME *node)
{
	return true;
}

//----------------------------------------------------------------------------
albaOp* appOpDictionary::Copy()
{
	appOpDictionary *op = new appOpDictionary(m_Label);
	op->m_OpType = m_OpType;
	op->m_Canundo = m_Canundo;

	return op;
}

//----------------------------------------------------------------------------
void appOpDictionary::OpRun()
{
	if (!m_TestMode)
	{
		CreateGui();
	}
}
//----------------------------------------------------------------------------
void appOpDictionary::OpStop(int result)
{
	if (!m_TestMode)
	{
		m_GroupComboBox->Clear();
		delete m_GroupComboBox;
		delete m_ElementDict;

		HideGui();
	}

	albaEventMacro(albaEvent(this, result));
}
//----------------------------------------------------------------------------
void appOpDictionary::OpDo()
{
}

// GUI
//----------------------------------------------------------------------------
void appOpDictionary::CreateGui()
{
	// Interface:
	m_Gui = new albaGUI(this);
	m_Gui->SetListener(this);

	// Dictionary
	m_Gui->Label(_("Dictionary"));

	std::vector<int> idDictVect;
	idDictVect.push_back(ID_RESET_DICTIONARY);
	idDictVect.push_back(ID_LOAD_DICTIONARY);
	idDictVect.push_back(ID_SAVE_DICTIONARY);

	std::vector<const char*> labelDictVect;
	labelDictVect.push_back("New");
	labelDictVect.push_back("Load");
	labelDictVect.push_back("Save");

	m_Gui->MultipleButtons(3, 3, idDictVect, labelDictVect);
	m_Gui->Divider(2);

	m_Gui->String(-1, _("Name"), &m_DictionaryName);
	//m_Gui->String(-1, _("Version"), &m_DictionaryVersion);
	m_Gui->Divider(2);

	const wxString choices[] = { "" };
	m_GroupComboBox = m_Gui->Combo(ID_SELECT_GROUP, "", &m_SelectedGroup, 1, choices);
	m_Gui->Divider(2);

	// Setup GUI Dictionary
	m_ElementDict = new albaGUIDictionaryWidget(m_Gui, -1);
	m_ElementDict->SetListener(this);
	//m_Dict->SetCloud(m_Cloud);
	m_ElementDict->InitDictionary(NULL);
	//m_Dict->SetTitle("List");
	m_Gui->Add(m_ElementDict->GetWidget(), wxEXPAND);
	m_Gui->Divider(2);

	// Group
	m_Gui->String(ID_EDIT_GROUP, _("Group"), &m_GroupToEdit);

	std::vector<int> idGroupVect;
	idGroupVect.push_back(ID_ADD_GROUP);
	idGroupVect.push_back(ID_DELETE_GROUP);
	idGroupVect.push_back(ID_RESET_GROUP);

	std::vector<const char*> labelGroupVect;
	labelGroupVect.push_back("Add");
	labelGroupVect.push_back("Delete");
	labelGroupVect.push_back("Reset");

	m_Gui->MultipleButtons(3, 3, idGroupVect, labelGroupVect);
	m_Gui->Divider(2);

	// Element
	m_Gui->String(ID_EDIT_ELEMENT, _("Element"), &m_ElementToEdit);

	std::vector<int> idElemVect;
	idElemVect.push_back(ID_ADD_ELEMENT);
	idElemVect.push_back(ID_DELETE_ELEMENT);
	idElemVect.push_back(ID_MOVE_UP_ELEMENT);
	idElemVect.push_back(ID_MOVE_DOWN_ELEMENT);

	std::vector<const char*> labelElemVect;
	labelElemVect.push_back("Add");
	labelElemVect.push_back("Delete");
	labelElemVect.push_back("Up");
	labelElemVect.push_back("Down");

	m_Gui->MultipleButtons(4, 4, idElemVect, labelElemVect);
	m_Gui->Divider(2);

	m_Gui->Bool(ID_INCREMENTAL_NAME, _("Increment"), &m_IncrementalName, 0, "Generate Incremental Name (name_[x])");

	m_Gui->Divider(2);
	m_Gui->Label("");

	m_Gui->OkCancel();
	m_Gui->Divider();

	ShowGui();

	EnableDisableGui();
}
//----------------------------------------------------------------------------
void appOpDictionary::UpdateGui()
{
	if (!m_TestMode)
	{
		m_GroupComboBox->Clear();

		for (int i = 0; i < m_GroupVect.size(); i++)
		{
			m_GroupComboBox->AppendString(m_GroupVect[i]);
		}

		if (m_GroupVect.size() > m_SelectedGroup)
		{
			m_ElementDict->SetTitle(m_GroupVect[m_SelectedGroup]);
			m_ElementDict->InitDictionary(&m_ElementVect[m_SelectedGroup]);
		}

		m_Gui->Update();
	}
}
//----------------------------------------------------------------------------
void appOpDictionary::EnableDisableGui()
{
	int nGroups = m_GroupVect.size();
	int nElemts = (m_ElementVect.size() > m_SelectedGroup) ? m_ElementVect[m_SelectedGroup].size() : 0;
	int elemPos = m_ElementDict->SelectItem(m_SelectedElement);

	bool canEditElem = (elemPos > -1) && (elemPos < nElemts);

	m_Gui->Enable(ID_ADD_ELEMENT, nGroups > 0);
	m_Gui->Enable(ID_EDIT_ELEMENT, canEditElem);
	m_Gui->Enable(ID_DELETE_ELEMENT, canEditElem);
	m_Gui->Enable(ID_MOVE_UP_ELEMENT, canEditElem && (elemPos > 0));
	m_Gui->Enable(ID_MOVE_DOWN_ELEMENT, canEditElem && (elemPos < nElemts - 1));

	m_Gui->Enable(ID_SELECT_GROUP, nGroups > 0);
	m_Gui->Enable(ID_DELETE_GROUP, m_SelectedGroup < nGroups);
	m_Gui->Enable(ID_RESET_GROUP, nGroups > 0);
	m_Gui->Enable(ID_EDIT_GROUP, nGroups > 0);
}

//----------------------------------------------------------------------------
void appOpDictionary::OnEvent(albaEventBase *alba_event)
{
	if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
		{
			switch (e->GetId())
			{
				// Load - Save Events
			case ID_LOAD_DICTIONARY:
			{
				// Select Dictionary File
				wxString wild_dict = "Dictionary file (*.dic)|*.dic|All files (*.*)|*.*";
				wxString folder = albaGetLastUserFolder().c_str();

				wxString fileName = albaGetOpenFile(folder, wild_dict, "Choose Dictionary File").c_str();

				// Load
				if (fileName != "")
				{
					if (LoadDictionary(fileName) == ALBA_OK)
					{
						if (!m_TestMode)	// Update GUI
						{
							m_GroupComboBox->Clear();

							for (int i = 0; i < m_GroupVect.size(); i++)
							{
								m_GroupComboBox->AppendString(m_GroupVect[i]);
							}

							m_SelectedGroup = 0;
							m_SelectedElement = "";

							if (m_GroupVect.size() > 0)
								SelectGroup(m_SelectedGroup);
						}
					}
				}
			}
			break;

			case ID_SAVE_DICTIONARY:
			{
				// Select Dictionary File
				albaString wild_dict = "Dictionary file (*.dic)|*.dic|All files (*.*)|*.*";
				albaString initialFileName = albaGetLastUserFolder().c_str();
				initialFileName.Append("\\newDictionary.xml");

				wxString fileName = albaGetSaveFile(initialFileName.GetCStr(), wild_dict).c_str();

				// Save
				if (fileName != "")
					SaveDictionary(fileName);
			}
			break;

			// Elements Events
			case ITEM_SELECTED:
			{
				SelectElement(*(e->GetString())); // From GUI Dictionary Widget
			}
			break;

			case ID_ADD_ELEMENT:
			{
				AddElement();
			}
			break;

			case ID_DELETE_ELEMENT:
			{
				DeleteElement();
			}
			break;

			case ID_EDIT_ELEMENT:
			{
				EditElement();
			}
			break;

			case ID_MOVE_UP_ELEMENT:
			{
				MoveUpElement();
			}
			break;

			case ID_MOVE_DOWN_ELEMENT:
			{
				MoveDownElement();
			}
			break;

			// Group Events
			case ID_SELECT_GROUP:
			{
				SelectGroup(m_SelectedGroup);
			}
			break;

			case ID_ADD_GROUP:
			{
				AddGroup();
			}
			break;

			case ID_DELETE_GROUP:
			{
				DeleteGroup();
			}
			break;

			case ID_EDIT_GROUP:
			{
				EditGroup();
			}
			break;

			case ID_RESET_GROUP:
			{
				ResetGroup();
			}
			break;

			case ID_RESET_DICTIONARY:
			{
				ResetDictionary();
			}
			break;

			// Ok-Cancel Events
			case wxOK:
				OpStop(OP_RUN_OK);
				break;

			case wxCANCEL:
				OpStop(OP_RUN_CANCEL);
				break;

			default:
				Superclass::OnEvent(alba_event);
				break;
			}
		}
	}
}

// GROUP
//----------------------------------------------------------------------------
void appOpDictionary::SelectGroup(int selection)
{
	m_SelectedGroup = selection;
	m_GroupToEdit = m_GroupVect[m_SelectedGroup];

	UpdateGui();
	EnableDisableGui();
}
//----------------------------------------------------------------------------
void appOpDictionary::AddGroup()
{
	StringVector newVect;
	m_ElementVect.push_back(newVect);

	wxString groupName = "New_Group";

	if (m_IncrementalName)
	{
		char string[100];
		sprintf(string, "_%d", m_GroupVect.size() + 1);
		groupName.append(string);
	}

	m_GroupVect.push_back(groupName);

	SelectGroup(m_GroupVect.size() - 1);
}
//----------------------------------------------------------------------------
void appOpDictionary::DeleteGroup()
{
	if (m_GroupVect.size() > m_SelectedGroup)
	{
		m_GroupVect.erase(m_GroupVect.begin() + m_SelectedGroup);
		m_ElementVect.erase(m_ElementVect.begin() + m_SelectedGroup);
		
		if (m_GroupVect.size() > 0)
		{
			SelectGroup(m_GroupVect.size() - 1);
			SelectElement("");
		}
		else
		{
			// Reset All
			m_SelectedGroup = 0;
			m_SelectedElement = "";

			m_ElementToEdit = "";
			m_GroupToEdit = "";

			m_ElementDict->InitDictionary(NULL);
			m_GroupComboBox->Clear();

			m_Gui->Update();
			EnableDisableGui();
		}
	}
}
//----------------------------------------------------------------------------
void appOpDictionary::EditGroup()
{
	if (m_GroupVect.size() > m_SelectedGroup)
	{
		m_GroupVect[m_SelectedGroup] = m_GroupToEdit;

		UpdateGui();
		EnableDisableGui();
	}
}
//----------------------------------------------------------------------------
void appOpDictionary::ResetGroup()
{
	if (m_GroupVect.size() > m_SelectedGroup)
	{
		m_ElementVect[m_SelectedGroup].clear();

		m_SelectedElement = "";
		m_ElementToEdit = "";

		UpdateGui();
		EnableDisableGui();
	}
}

// ELEMENT
//----------------------------------------------------------------------------
void appOpDictionary::SelectElement(albaString selection)
{
	m_ElementDict->DeselectItem(m_SelectedElement);
	m_ElementDict->SelectItem(selection);

	m_SelectedElement = selection;
	m_ElementToEdit = selection;

	if (!m_TestMode)
	{
		m_Gui->Update();
		EnableDisableGui();
	}
}
//----------------------------------------------------------------------------
void appOpDictionary::AddElement()
{
	if (m_GroupVect.size() > m_SelectedGroup)
	{
		wxString newElement = "New_Element";

		if (m_IncrementalName)
		{
			char string[100];
			sprintf(string, "_%d", m_ElementVect[m_SelectedGroup].size() + 1);
			newElement.append(string);
		}

		m_ElementVect[m_SelectedGroup].push_back(newElement);
		
		UpdateGui();
		SelectElement(newElement);
	}
}
//----------------------------------------------------------------------------
void appOpDictionary::DeleteElement()
{
	if (m_GroupVect.size() > m_SelectedGroup)
	{
		int pos = m_ElementDict->SelectItem(m_SelectedElement);
		if (pos > -1 && pos < m_ElementVect[m_SelectedGroup].size())
		{
			m_ElementVect[m_SelectedGroup].erase(m_ElementVect[m_SelectedGroup].begin() + pos);
			
			SelectGroup(m_SelectedGroup);

			if (pos > 0)
				SelectElement(m_ElementVect[m_SelectedGroup][pos - 1]);
			else
				SelectElement("");
		}
	}
}
//----------------------------------------------------------------------------
void appOpDictionary::EditElement()
{
	int pos = m_ElementDict->SelectItem(m_SelectedElement);
	if (pos > -1 && pos < m_ElementVect[m_SelectedGroup].size())
	{
		m_ElementVect[m_SelectedGroup][pos] = m_ElementToEdit;

		UpdateGui();
		EnableDisableGui();
	}
}
//----------------------------------------------------------------------------
void appOpDictionary::MoveUpElement()
{
	int pos = m_ElementDict->SelectItem(m_SelectedElement);
	if (pos > 0 && pos < m_ElementVect[m_SelectedGroup].size())
	{
		wxString aux = m_ElementVect[m_SelectedGroup][pos - 1];
		m_ElementVect[m_SelectedGroup][pos - 1] = m_SelectedElement;
		m_ElementVect[m_SelectedGroup][pos] = aux;

		UpdateGui();
		SelectElement(m_SelectedElement);
	}
}
//----------------------------------------------------------------------------
void appOpDictionary::MoveDownElement()
{
	int pos = m_ElementDict->SelectItem(m_SelectedElement);
	if (pos > -1 && pos < m_ElementVect[m_SelectedGroup].size()-1)
	{
		wxString aux = m_ElementVect[m_SelectedGroup][pos + 1];
		m_ElementVect[m_SelectedGroup][pos + 1] = m_SelectedElement;
		m_ElementVect[m_SelectedGroup][pos] = aux;

		UpdateGui();
		SelectElement(m_SelectedElement);
	}
}


//----------------------------------------------------------------------------
// NEW-LOAD-SAVE DICTIONARY
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void appOpDictionary::ResetDictionary()
{
	m_ElementVect.clear();
	m_GroupVect.clear();

	m_DictionaryName = "Dictionary";

	m_SelectedGroup = 0;
	m_SelectedElement = "";

	m_ElementToEdit = "";
	m_GroupToEdit = "";

	if (!m_TestMode)
	{
		m_ElementDict->InitDictionary(NULL);
		m_GroupComboBox->Clear();
		m_Gui->Update();
		EnableDisableGui();
	}
}

//----------------------------------------------------------------------------
int appOpDictionary::LoadDictionary(wxString fileName)
{
	m_GroupVect.clear();
	m_ElementVect.clear();
		
	try //Open the file xml
	{
		XERCES_CPP_NAMESPACE_QUALIFIER XMLPlatformUtils::Initialize();
	}
	catch (const XERCES_CPP_NAMESPACE_QUALIFIER XMLException& toCatch)
	{
		// Do your failure processing here
		return ALBA_ERROR;
	}

	XERCES_CPP_NAMESPACE_QUALIFIER XercesDOMParser *XMLParser = new  XERCES_CPP_NAMESPACE_QUALIFIER XercesDOMParser;

	XMLParser->setValidationScheme(XERCES_CPP_NAMESPACE_QUALIFIER XercesDOMParser::Val_Auto);
	XMLParser->setDoNamespaces(false);
	XMLParser->setDoSchema(false);
	XMLParser->setCreateEntityReferenceNodes(false);

	mmuDOMTreeErrorReporter *errReporter = new mmuDOMTreeErrorReporter();
	XMLParser->setErrorHandler(errReporter);

	try
	{
		XMLParser->parse(fileName);
		int errorCount = XMLParser->getErrorCount();

		if (errorCount != 0)
		{
			// Errors while parsing...
			albaErrorMessage("Errors while parsing XML file");
			return ALBA_ERROR;
		}

		// extract the root element and wrap inside a albaXMLElement
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc = XMLParser->getDocument();
		XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *root = doc->getDocumentElement();
		assert(root);

		if (CheckNodeElement(root, m_DictionaryTypeName))
		{
			albaString name = GetElementAttribute(root, "Name");
			albaString version = GetElementAttribute(root, "Version");

			m_DictionaryName = name;
			m_DictionaryVersion = version;

			// Check file version
			if (version != "1.0")
			{
				albaLogMessage("Wrong file Version:\n version:%s", version.GetCStr());
				return ALBA_ERROR;
			}
		}
		else
		{
			albaLogMessage("Wrong check root node");
			return ALBA_ERROR;
		}

		XERCES_CPP_NAMESPACE_QUALIFIER DOMNodeList *typesChildren = root->getChildNodes();

		for (unsigned int i = 0; i < typesChildren->getLength(); i++)
		{
			// Reading Type nodes 
			XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *typeNode = typesChildren->item(i);

			if (CheckNodeElement(typeNode, m_DictionaryGroupTypeName))
			{
				wxString dicName = GetElementAttribute(typeNode, "Name");
								
				m_GroupVect.push_back(dicName);

				StringVector newVect;

				XERCES_CPP_NAMESPACE_QUALIFIER DOMNodeList *typeChildren = typeNode->getChildNodes();

				for (unsigned int j = 0; j < typeChildren->getLength(); j++)
				{
					// Reading Type nodes 
					XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *typeNode = typeChildren->item(j);

					if (CheckNodeElement(typeNode, m_DictionaryElementTypeName))
					{
						newVect.push_back(typeNode->getTextContent());
					}
				}

				m_ElementVect.push_back(newVect);
			}
		}
	}
	catch (const  XERCES_CPP_NAMESPACE_QUALIFIER XMLException& toCatch)
	{
		return ALBA_ERROR;
	}
	catch (const  XERCES_CPP_NAMESPACE_QUALIFIER DOMException& toCatch)
	{
		return ALBA_ERROR;
	}
	catch (...)
	{
		return ALBA_ERROR;
	}

	cppDEL(errReporter);
	delete XMLParser;

	// Terminate the XML library
	XERCES_CPP_NAMESPACE_QUALIFIER XMLPlatformUtils::Terminate();

	albaLogMessage(_("Dictionary file Loaded"));

	return ALBA_OK;
}

//---------------------------------------------------------------------------
int appOpDictionary::SaveDictionary(const char *dictionaryFileName)
{	
	try // Open the file xml
	{
		XERCES_CPP_NAMESPACE_QUALIFIER XMLPlatformUtils::Initialize();
	}
	catch (const XERCES_CPP_NAMESPACE_QUALIFIER XMLException& toCatch)
	{
		// Do your failure processing here
		return ALBA_ERROR;
	}

	XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc;
	XMLCh tempStr[100];
	XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode("LS", tempStr, 99);
	XERCES_CPP_NAMESPACE_QUALIFIER DOMImplementation *impl = XERCES_CPP_NAMESPACE_QUALIFIER DOMImplementationRegistry::getDOMImplementation(tempStr);
	XERCES_CPP_NAMESPACE_QUALIFIER DOMWriter* theSerializer = ((XERCES_CPP_NAMESPACE_QUALIFIER DOMImplementationLS*)impl)->createDOMWriter();
	theSerializer->setNewLine(albaXMLString("\r"));

	if (theSerializer->canSetFeature(XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgDOMWRTFormatPrettyPrint, true))
		theSerializer->setFeature(XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgDOMWRTFormatPrettyPrint, true);

	doc = impl->createDocument(NULL, albaXMLString(m_DictionaryTypeName), NULL);

	doc->setEncoding(albaXMLString("UTF-8"));
	doc->setStandalone(true);
	doc->setVersion(albaXMLString("1.0"));

	// Extract root element and wrap it with an albaXMLElement object
	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *root = doc->getDocumentElement();
	assert(root);

	// Attach version attribute to the root node
	root->setAttribute(albaXMLString("Name"), albaXMLString(m_DictionaryName));
	root->setAttribute(albaXMLString("Version"), albaXMLString(m_DictionaryVersion));

	for (int i = 0; i < m_ElementVect.size(); i++)
	{
		wxString groupName = m_GroupVect[i];

		// GROUP
		XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *group = doc->createElement(albaXMLString(m_DictionaryGroupTypeName));
		group->setAttribute(albaXMLString("Name"), albaXMLString(groupName));

		for (int j = 0; j < m_ElementVect[i].size(); j++)
		{
			// ELEMENTS
			XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *elem = doc->createElement(albaXMLString(m_DictionaryElementTypeName));

			XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node = doc->createTextNode(albaXMLString(m_DictionaryElementTypeName));
			node->setNodeValue(albaXMLString(m_ElementVect[i][j]));
			elem->appendChild(node);

			group->appendChild(elem);
		}
		root->appendChild(group);
	}

	//

	XERCES_CPP_NAMESPACE_QUALIFIER XMLFormatTarget *XMLTarget;
	albaString fileName = dictionaryFileName;

	XMLTarget = new XERCES_CPP_NAMESPACE_QUALIFIER LocalFileFormatTarget(fileName);

	try
	{
		// Do the serialization through DOMWriter::writeNode();
		theSerializer->writeNode(XMLTarget, *doc);
	}
	catch (const XERCES_CPP_NAMESPACE_QUALIFIER  XMLException& toCatch)
	{
		char* message = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode(toCatch.getMessage());
		XERCES_CPP_NAMESPACE_QUALIFIER XMLString::release(&message);
		return ALBA_ERROR;
	}
	catch (const XERCES_CPP_NAMESPACE_QUALIFIER DOMException& toCatch)
	{
		char* message = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode(toCatch.msg);
		XERCES_CPP_NAMESPACE_QUALIFIER XMLString::release(&message);
		return ALBA_ERROR;
	}
	catch (...)
	{
		return ALBA_ERROR;
	}

	theSerializer->release();
	cppDEL(XMLTarget);
	doc->release();

	XERCES_CPP_NAMESPACE_QUALIFIER XMLPlatformUtils::Terminate();

	albaLogMessage(wxString::Format("Dictionary has been written %s", fileName.GetCStr()));

	return ALBA_OK;
}

//---------------------------------------------------------------------------
bool appOpDictionary::CheckNodeElement(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node, const char *elementName)
{
	// Reading nodes
	if (node->getNodeType() != XERCES_CPP_NAMESPACE_QUALIFIER DOMNode::ELEMENT_NODE)
		return false;

	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *nodeElement = (XERCES_CPP_NAMESPACE_QUALIFIER DOMElement*)node;
	albaString nameElement = "";
	nameElement = albaXMLString(nodeElement->getTagName());

	return (nameElement == elementName);
}
//--------------------------------------------------------------------------
albaString appOpDictionary::GetElementAttribute(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node, const char *attributeName)
{
	if (node->getNodeType() != XERCES_CPP_NAMESPACE_QUALIFIER DOMNode::ELEMENT_NODE)
		return "";

	return albaXMLString(((XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *)node)->getAttribute(albaXMLString(attributeName)));
}
