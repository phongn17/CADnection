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



//######################################################################################################################
int traverseAttribute(const A3DMiscAttribute* pAttribute, TiXmlElement* setting, json* jsonData, int index, int pmiIndex)
{
	A3DMiscAttributeData sData;
	A3D_INITIALIZE_DATA(A3DMiscAttributeData, sData);

	A3DInt32 iRet = A3DMiscAttributeGet(pAttribute, &sData);
	if(iRet == A3D_SUCCESS)
	{
		std::string name = "";
		TiXmlElement* attribute = new TiXmlElement("A3DMiscAttributeData");
		attribute->SetAttribute("m_bTitleIsInt", (int) sData.m_bTitleIsInt);
		if(sData.m_bTitleIsInt)
		{
			A3DUns32 uiVal;
			memcpy(&uiVal,sData.m_pcTitle, sizeof(A3DUns32));
			attribute->SetAttribute("m_pcTitle", (int)uiVal);
		}
		else
		{
			if (sData.m_pcTitle && sData.m_pcTitle[0] != '\0') {
				attribute->SetAttribute("m_pcTitle", sData.m_pcTitle);
				name = sData.m_pcTitle;
			}
		}

		std::string value = "";
		std::string attrName;
		for(A3DUns32 ui = 0; ui < sData.m_uiSize; ++ui)
		{
			TiXmlElement* single = new TiXmlElement("m_asSingleAttributesData");
			attrName = "";
			single->SetAttribute("m_bTitleIsInt", (int) sData.m_asSingleAttributesData[ui].m_bTitleIsInt);
			if(sData.m_asSingleAttributesData[ui].m_pcTitle == NULL)
				single->SetAttribute("m_pcTitle", "NULL");
			else if(sData.m_asSingleAttributesData[ui].m_bTitleIsInt)
			{
				A3DUns32 uiVal;
				memcpy(&uiVal,sData.m_asSingleAttributesData[ui].m_pcTitle, sizeof(A3DUns32));
				single->SetAttribute("m_pcTitle", (int)uiVal);
			}
			else
			{
				if (sData.m_asSingleAttributesData[ui].m_pcTitle && sData.m_asSingleAttributesData[ui].m_pcTitle[0] != '\0') {
					single->SetAttribute("m_pcTitle", sData.m_asSingleAttributesData[ui].m_pcTitle);
					attrName = sData.m_asSingleAttributesData[ui].m_pcTitle;
				}
			}

			A3DInt32 iVal;

			switch(sData.m_asSingleAttributesData[ui].m_eType)
			{
			case kA3DModellerAttributeTypeInt:
				memcpy(&iVal, sData.m_asSingleAttributesData[ui].m_pcData, sizeof(A3DInt32));
				single->SetAttribute("m_eType", "kA3DModellerAttributeTypeInt");
				single->SetAttribute("m_pcData", iVal);
				if (!attrName.empty()) {
					if (jsonData) {
						if (pmiIndex == -1)
							(*jsonData)["docs"][index][attrName.c_str()] = iVal;
						else
							(*jsonData)["docs"][index]["PMI"][pmiIndex][attrName.c_str()] = iVal;
					}
				}
				break;

			case kA3DModellerAttributeTypeReal:
				A3DDouble dVal;
				memcpy(&dVal, sData.m_asSingleAttributesData[ui].m_pcData, sizeof(A3DDouble));
				single->SetAttribute("m_eType", "kA3DModellerAttributeTypeReal");
				_SetDoubleAttribute(single, "m_pcData", dVal);
				if (!attrName.empty()) {
					if (jsonData) {
						if (pmiIndex == -1)
							(*jsonData)["docs"][index][attrName.c_str()] = dVal;
						else
							(*jsonData)["docs"][index]["PMI"][pmiIndex][attrName.c_str()] = dVal;
					}
				}
				if (sData.m_asSingleAttributesData[ui].m_usUnit!= A3D_DEFAULT_NO_UNIT)
					single->SetAttribute("m_usUnit", sData.m_asSingleAttributesData[ui].m_usUnit);
				break;

			case kA3DModellerAttributeTypeTime:
				memcpy(&iVal, sData.m_asSingleAttributesData[ui].m_pcData, sizeof(A3DInt32));
				single->SetAttribute("m_eType", "kA3DModellerAttributeTypeTime");
				single->SetAttribute("m_pcData", iVal);
				if (!attrName.empty()) {
					if (jsonData) {
						if (pmiIndex == -1)
							(*jsonData)["docs"][index][attrName.c_str()] = iVal;
						else
							(*jsonData)["docs"][index]["PMI"][pmiIndex][attrName.c_str()] = iVal;
					}
				}
				break;

			case kA3DModellerAttributeTypeString:
				single->SetAttribute("m_eType", "kA3DModellerAttributeTypeString");
				if (sData.m_asSingleAttributesData[ui].m_pcData && sData.m_asSingleAttributesData[ui].m_pcData[0] != '\0') {
					single->SetAttribute("m_pcData", sData.m_asSingleAttributesData[ui].m_pcData);
					if (!attrName.empty()) {
						if (jsonData) {
							if (pmiIndex == -1)
								(*jsonData)["docs"][index][attrName.c_str()] = sData.m_asSingleAttributesData[ui].m_pcData;
							else
								(*jsonData)["docs"][index]["PMI"][pmiIndex][attrName.c_str()] = sData.m_asSingleAttributesData[ui].m_pcData;
						}
					}
					else {
						if (!value.empty()) {
							value += ", ";
						}
						value += sData.m_asSingleAttributesData[ui].m_pcData;
					}
				}
				break;

			default:
				break;
			}
			attribute->LinkEndChild(single);
		}

		if (!name.empty() && !value.empty()) {
			if (jsonData) {
				if (pmiIndex == -1)
					(*jsonData)["docs"][index][name.c_str()] = value.c_str();
				else
					(*jsonData)["docs"][index]["PMI"][pmiIndex][name.c_str()] = value.c_str();
			}
		}

		setting->LinkEndChild(attribute);
		A3DMiscAttributeGet(NULL, &sData);
	}

	return A3D_SUCCESS;
}

//######################################################################################################################
int traverseGraphics(const A3DGraphics* pGraphics, TiXmlElement* setting)
{
	A3DGraphicsData sData;
	A3D_INITIALIZE_DATA(A3DGraphicsData, sData);

	A3DInt32 iRet = A3DGraphicsGet(pGraphics, &sData);
	if(iRet == A3D_SUCCESS)
	{
		TiXmlElement* graphics = new TiXmlElement("A3DGraphicsData");

		if(sData.m_uiLayerIndex != A3D_DEFAULT_LAYER)
			graphics->SetAttribute("m_uiLayerIndex", (int) sData.m_uiLayerIndex);
		if(sData.m_uiStyleIndex != A3D_DEFAULT_STYLE_INDEX)
			graphics->SetAttribute("m_uiStyleIndex", (int) sData.m_uiStyleIndex);

		graphics->SetAttribute("m_usBehaviour", (int) sData.m_usBehaviour);

		CHECK_RET(A3DGraphicsGet(NULL, &sData));
		setting->LinkEndChild(graphics);
	}

	return iRet;
}

//######################################################################################################################
int traverseBase(const A3DEntity* pEntity, TiXmlElement* setting, json* jsonData, int index, int parentIndex, int pmiIndex = -1)
{
	A3DRootBaseData sData;
	A3D_INITIALIZE_DATA(A3DRootBaseData, sData);
	A3DInt32 iRet = A3DRootBaseGet(pEntity, &sData);
	if(iRet == A3D_SUCCESS)
	{
		TiXmlElement* base = new TiXmlElement("A3DRootBaseData");
		if (sData.m_pcName && sData.m_pcName[0] != '\0') {
			base->SetAttribute("m_pcName", sData.m_pcName);
			if (jsonData) {
				if (pmiIndex == -1)
					(*jsonData)["docs"][index]["Name"] = sData.m_pcName;
				else
					(*jsonData)["docs"][index]["PMI"][pmiIndex]["Name"] = sData.m_pcName;
			}
		}

		base->SetAttribute("m_uiPersistentId", sData.m_uiPersistentId);
		base->SetAttribute("m_uiNonPersistentId", sData.m_uiNonPersistentId);
		if (sData.m_pcPersistentId) {
			base->SetAttribute("m_pcPersistentId", sData.m_pcPersistentId);
			if (jsonData) {
				if (pmiIndex == -1) {
					(*jsonData)["docs"][index]["Id"] = sData.m_pcPersistentId;
					if (parentIndex != -1) {
						if (!Export2Json::PartExists(jsonData, parentIndex, sData.m_pcPersistentId)) {
							(*jsonData)["docs"][parentIndex]["ComposedOf"][index - parentIndex - 1] = (*jsonData)["docs"][index]["Id"];
							(*jsonData)["docs"][index]["WhereUsed"] = (*jsonData)["docs"][parentIndex]["Id"];
						}
					}
				}
				else
					(*jsonData)["docs"][index]["PMI"][pmiIndex]["Id"] = sData.m_pcPersistentId;
			}
		}
		else if (jsonData) {
			if (pmiIndex == -1) {
				string partId = boost::lexical_cast<std::string>(boost::uuids::random_generator()());
				(*jsonData)["docs"][index]["Id"] = partId.c_str();
				if (parentIndex != -1) {
					if (!Export2Json::PartExists(jsonData, parentIndex, partId.c_str())) {
						(*jsonData)["docs"][parentIndex]["ComposedOf"][index - parentIndex - 1] = (*jsonData)["docs"][index]["Id"];
						(*jsonData)["docs"][index]["WhereUsed"] = (*jsonData)["docs"][parentIndex]["Id"];
					}
				}
			}
		}

		for(A3DUns32 ui = 0; ui < sData.m_uiSize; ++ui)
			traverseAttribute(sData.m_ppAttributes[ui], base, jsonData, index, pmiIndex);

		A3DBoundingBoxData sBoundingBox;
		A3D_INITIALIZE_DATA(A3DBoundingBoxData, sBoundingBox);
		iRet = A3DMiscGetBoundingBox(pEntity, &sBoundingBox);
		if(iRet == A3D_SUCCESS)
		{
			if((sBoundingBox.m_sMin.m_dX + sBoundingBox.m_sMin.m_dY + sBoundingBox.m_sMin.m_dZ + sBoundingBox.m_sMax.m_dX + sBoundingBox.m_sMax.m_dY + sBoundingBox.m_sMax.m_dZ) != 0.0)
			{
				char acBBox[256];
				sprintf(acBBox, "Min(%f,%f,%f) Max(%f,%f,%f)", sBoundingBox.m_sMin.m_dX, sBoundingBox.m_sMin.m_dY, sBoundingBox.m_sMin.m_dZ, sBoundingBox.m_sMax.m_dX, sBoundingBox.m_sMax.m_dY, sBoundingBox.m_sMax.m_dZ);
				base->SetAttribute("BoundingBox", acBBox);
				A3DMiscGetBoundingBox(NULL, &sBoundingBox);
			}
		}

		setting->LinkEndChild(base);
		A3DRootBaseGet(NULL, &sData);
	}

	return A3D_SUCCESS;
}

//######################################################################################################################
int traverseBaseWithGraphics(const A3DEntity* pEntity, TiXmlElement* setting, json* jsonData, int index, int parentIndex, int pmiIndex)
{
	A3DRootBaseWithGraphicsData sData;
	A3D_INITIALIZE_DATA(A3DRootBaseWithGraphicsData, sData);

	A3DInt32 iRet = A3DRootBaseWithGraphicsGet(pEntity, &sData);
	if(iRet == A3D_SUCCESS)
	{
		TiXmlElement* basewithgraphics = new TiXmlElement("A3DRootBaseWithGraphicsData");
		traverseBase(pEntity,basewithgraphics, jsonData, index, parentIndex, pmiIndex);
		if(sData.m_pGraphics != NULL)
			traverseGraphics(sData.m_pGraphics,basewithgraphics);
		setting->LinkEndChild(basewithgraphics);
		A3DRootBaseWithGraphicsGet(NULL, &sData);
	}

	return A3D_SUCCESS;
}

//######################################################################################################################
int traverseSource(const A3DEntity* pEntity, TiXmlElement* setting, json* jsonData, int index, int parentIndex, int pmiIndex)
{
	if(A3DEntityIsBaseWithGraphicsType(pEntity))
		return traverseBaseWithGraphics(pEntity, setting, jsonData, index, parentIndex, pmiIndex);
	else if(A3DEntityIsBaseType(pEntity))
		return traverseBase(pEntity, setting, jsonData, index, parentIndex, pmiIndex);

	return A3D_SUCCESS;
}
