/***********************************************************************************************************************
*
* Copyright (c) 2017 by Tech Soft 3D, LLC.
* The information contained herein is confidential and proprietary to Tech Soft 3D, LLC., and considered a trade secret
* as defined under civil and criminal statutes. Tech Soft 3D shall pursue its civil and criminal remedies in the event
* of unauthorized use or misappropriation of its trade secrets. Use of this information by anyone other than authorized
* employees of Tech Soft 3D, LLC. is granted only under a written non-disclosure agreement, expressly prescribing the
* scope and manner of such use.
*
***********************************************************************************************************************/

#include <A3DSDKIncludes.h>
#include "Export2XML.h"

static A3DVoid sttraverseSetNameAttribute(const A3DEntity* entity,
	TiXmlElement* xmlnode)
{
	{
		A3DRootBaseData sData;
		A3D_INITIALIZE_DATA(A3DRootBaseData, sData);
		if (A3DRootBaseGet(entity, &sData) == A3D_SUCCESS)
		{
			if (sData.m_pcName && sData.m_pcName[0] != '\0')
				xmlnode->SetAttribute("Name", sData.m_pcName);
			if (sData.m_uiPersistentId)
				xmlnode->SetAttribute("PersistentId", sData.m_uiPersistentId);
			A3DRootBaseGet(NULL, &sData);
		}
	}
	{
		A3DRootBaseWithGraphicsData sData;
		A3D_INITIALIZE_DATA(A3DRootBaseWithGraphicsData, sData);
		if (A3DRootBaseWithGraphicsGet(entity, &sData) == A3D_SUCCESS)
		{
			if (sData.m_pGraphics)
			{
				A3DGraphicsData sGraphicsData;
				A3D_INITIALIZE_DATA(A3DGraphicsData, sGraphicsData);
				if (A3DGraphicsGet(sData.m_pGraphics, &sGraphicsData) == A3D_SUCCESS)
				{
					xmlnode->SetAttribute("Layer", sGraphicsData.m_uiLayerIndex);
					xmlnode->SetAttribute("Style", sGraphicsData.m_uiStyleIndex);
					xmlnode->SetAttribute("Behaviour", sGraphicsData.m_usBehaviour);
				}
			}
		}
	}
}

static A3DVoid sttraverseXmlText(const char* ac,
	TiXmlElement* xmlfather)
{
	if (ac[0] != '\0') {
		char acUTF8[2048];
		sprintf(acUTF8, "%s ", ac);
		TiXmlText* xml = new TiXmlText(acUTF8);
		xmlfather->LinkEndChild(xml);
	}
}

static A3DVoid sttraverseXmlText(double d,
	TiXmlElement* xmlfather)
{
	char acUTF8[2048];
	sprintf(acUTF8, "%g", d);
	sttraverseXmlText(acUTF8, xmlfather);
}

	
static A3DVoid sttraverseFRMFeatureLinkedItem(const A3DFRMFeatureLinkedItem* pFeatureLinkedItem, TiXmlElement* xmlfather)
{

	TiXmlElement* xml = new TiXmlElement("A3DFRMFeatureLinkedItem");
	A3DFRMFeatureLinkedItemData sData;
	A3D_INITIALIZE_DATA(A3DFRMFeatureLinkedItemData, sData);

	traverseSource(pFeatureLinkedItem, xml, NULL, -1, -1);

	A3DInt32 iRet = A3DFRMFeatureLinkedItemGet(pFeatureLinkedItem, &sData);
	if (iRet == A3D_SUCCESS)
	{
		traverseEntityReference((A3DMiscEntityReference*)pFeatureLinkedItem, xml);
		xml->SetAttribute("m_eType", sData.m_eType);
		if (sData.m_pTargetProductOccurrence != NULL)
			_SetAttributePtr(xml, "m_pTargetProductOccurrence", (void*)sData.m_pTargetProductOccurrence);
		A3DFRMFeatureLinkedItemGet(NULL, &sData);
	}
	else
		xml->SetAttribute("error", iRet);

	xmlfather->LinkEndChild(xml);
}

static A3DVoid sttraverseFRMParameter(const A3DFRMParameter* pFeatParameter, TiXmlElement* xmlfather);

static A3DVoid sttraverseFRMFeature(const A3DFRMFeature* pFeature, TiXmlElement* xmlfather)

{
	TiXmlElement* xml = new TiXmlElement("A3DFRMFeature");
	sttraverseSetNameAttribute(pFeature, xml);
	xmlfather->LinkEndChild(xml);
	
	A3DUTF8Char* pcFeatureType = NULL;
	A3DFRMFeatureGetTypeAsString(pFeature, &pcFeatureType);
	if(pcFeatureType)
		xml->SetAttribute("Type", pcFeatureType);


	A3DFRMFeatureData sData;
	A3D_INITIALIZE_DATA(A3DFRMFeatureData, sData);
	A3DStatus iErr = A3DFRMFeatureGet(pFeature, &sData);
	if (iErr != A3D_SUCCESS)
		return ;
	char acName[30];

	switch (sData.m_eDataType)
	{
	case kA3DFRMDataInteger:
		A3DFRMIntegerData sIntegerData;
		A3D_INITIALIZE_DATA(A3DFRMIntegerData, sIntegerData);
		A3DFRMIntegerDataGet(pFeature, &sIntegerData);
		for (A3DUns32 i = 0; i < sIntegerData.m_uiValuesSize; ++i)
		{
			sprintf(acName, "value_%d", i);
			xml->SetAttribute(acName, sIntegerData.m_piValues[i]);
		}
		A3DFRMIntegerDataGet(NULL, &sIntegerData);
		break;
	case kA3DFRMDataDouble:
		A3DFRMDoubleData sDoubleData;
		A3D_INITIALIZE_DATA(A3DFRMDoubleData, sDoubleData);
		A3DFRMDoubleDataGet(pFeature, &sDoubleData);
		for (A3DUns32 i = 0; i < sDoubleData.m_uiValuesSize; ++i)
		{
			sprintf(acName, "value_%d", i);
			xml->SetDoubleAttribute(acName, sDoubleData.m_pdValues[i]);
		}
		A3DFRMDoubleDataGet(NULL, &sDoubleData);
		break;
	case kA3DFRMDataString:
		A3DFRMStringData sStringData;
		A3D_INITIALIZE_DATA(A3DFRMStringData, sStringData);
		A3DFRMStringDataGet(pFeature, &sStringData);
		for (A3DUns32 i = 0; i < sStringData.m_uiValuesSize; ++i)
		{
			sprintf(acName, "value_%d", i);
			xml->SetAttribute(acName, sStringData.m_ppcValues[i]);
		}
		A3DFRMStringDataGet(NULL, &sStringData);
		break;
	case kA3DFRMDataEnum:
	{
		A3DInt32 iEnumValue;
		A3DUTF8Char* pcValueAsString = NULL;
		A3DFRMEnumDataGet(pFeature, &iEnumValue, &pcValueAsString);
		if (pcValueAsString)
			xml->SetAttribute("Value", pcValueAsString);
		else
			xml->SetAttribute("Value", iEnumValue);
		A3DFRMEnumDataGet(NULL, &iEnumValue, &pcValueAsString);
	}
		break;
	default:
		break;
	}

	for (A3DUns32 ui = 0; ui<sData.m_uiConnectionSize; ui++)
		sttraverseFRMFeatureLinkedItem(sData.m_ppConnections[ui], xml);

	for (A3DUns32 ui = 0; ui<sData.m_uiParametersSize; ui++)
		sttraverseFRMParameter(sData.m_ppParameters[ui], xml);

	A3DFRMFeatureGet(NULL, &sData);

}

static A3DVoid sttraverseFRMParameter(const A3DFRMParameter* pParameter, TiXmlElement* xmlfather)
{
	TiXmlElement* xml = new TiXmlElement("A3DFRMParameter");
	sttraverseSetNameAttribute(pParameter, xml);
	xmlfather->LinkEndChild(xml);

	A3DFRMParameterData sData;
	A3D_INITIALIZE_DATA(A3DFRMParameterData, sData);
	A3DStatus iErr = A3DFRMParameterGet(pParameter, &sData);
	if (iErr != A3D_SUCCESS)
		return;
	switch (sData.m_eType)
	{
	case kA3DParameterType_Information:
	{
		xml->SetAttribute("Type", "INFORMATION");
		break;
	}
	case kA3DParameterType_Type:
	{
		xml->SetAttribute("Type", "TYPE");
		break;
	}
	case kA3DParameterType_Specification:
	{
		xml->SetAttribute("Type", "SPECIFICATION");
		break;
	}
	case kA3DParameterType_FeatureDefinition:
	{
		xml->SetAttribute("Type", "FEATURE DEFINITION");
		break;
	}
	case kA3DParameterType_Definition:
	{
		xml->SetAttribute("Type", "DEFINITION");
		break;
	}
	case kA3DParameterType_Container:
	{
		xml->SetAttribute("Type", "CONTAINER");
		break;
	}
	case kA3DParameterType_ContainerInternal:
	{
		xml->SetAttribute("Type", "INTERNAL CONTAINER");
		break;
	}
	case kA3DParameterType_Data:
	{
		xml->SetAttribute("Type", "DATA");
		break;
	}
	default:
		break;
	}
	for (A3DUns32 ui = 0; ui<sData.m_uiFeatureSize; ui++)
		sttraverseFRMFeature(sData.m_ppFeatures[ui], xml);
	A3DFRMParameterGet(NULL, &sData);

}
A3DVoid traverseFeatureTree(const A3DFRMFeatureTree* pFeatFeatureTree, TiXmlElement* xmlfather)
{
	TiXmlElement* xml = new TiXmlElement("A3DFRMFeatureTree");
	sttraverseSetNameAttribute(pFeatFeatureTree, xml);

	A3DFRMFeatureTreeData sData;
	A3D_INITIALIZE_DATA(A3DFRMFeatureTreeData, sData);
	if (A3DFRMFeatureTreeGet(pFeatFeatureTree, &sData) == A3D_SUCCESS)
	{
		for (A3DUns32 ui = 0; ui<sData.m_uiParametersSize; ui++)
			sttraverseFRMParameter(sData.m_ppsParameters[ui], xml);
		A3DFRMFeatureTreeGet(NULL, &sData);
	}
	xmlfather->LinkEndChild(xml);
}