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
static TiXmlElement* stfonts = NULL;

//######################################################################################################################
A3DVoid _InitializeFontsArray()
{
	if(stfonts == NULL)
		stfonts = new TiXmlElement("Fonts");
}

//######################################################################################################################
A3DVoid _TerminateFontsArray()
{
	delete stfonts;
	stfonts = NULL;
}

//######################################################################################################################
static A3DBool stCompareFontData(const TiXmlElement* node, const A3DFontData* psFontData)
{
	if(node->Attribute("m_pcFamilyName") && strcmp(node->Attribute("m_pcFamilyName"), psFontData->m_pcFamilyName) != 0)
		return false;

	int iSize;
	if(node->Attribute("m_uiSize", &iSize) && iSize != (int) psFontData->m_uiSize)
		return false;
	if(node->Attribute("m_eCharset", &iSize) && iSize != psFontData->m_eCharset)
		return false;
	if(node->Attribute("m_cAttributes", &iSize) && iSize != psFontData->m_cAttributes)
		return false;

	return true;
}

//######################################################################################################################
static A3DVoid stAddFont(const A3DFontData* psFontData)
{
	if(stfonts == NULL || psFontData == NULL)
		return;

	TiXmlElement* node = stfonts->FirstChildElement();
	while(node)
	{
		if(stCompareFontData(node, psFontData))
			return;
		node = node->NextSiblingElement();
	}

	TiXmlElement* font = new TiXmlElement("A3DFontData");
	font->SetAttribute("m_pcFamilyName", psFontData->m_pcFamilyName);
	font->SetAttribute("m_eCharset", psFontData->m_eCharset);
	font->SetAttribute("m_uiSize", (int)psFontData->m_uiSize);
	font->SetAttribute("m_cAttributes", psFontData->m_cAttributes);
	stfonts->LinkEndChild(font);
}

//######################################################################################################################
static A3DInt32 stExportFontsFromMarkupTessellation(const A3DTessMarkupData* psTess)
{
	if(psTess->m_uiCodesSize == 0)
		return A3D_SUCCESS;

	A3DFontKeyData sFontKeyData;
	A3DFontData sFontData;
	unsigned int uiCount, uiExtraDataType;
	const A3DUns32* puiStart = &psTess->m_puiCodes[0];
	const A3DUns32* puiEnd = &psTess->m_puiCodes[psTess->m_uiCodesSize-1];

	for(; puiStart < puiEnd; puiStart++)
	{
		uiCount = *puiStart & kA3DMarkupIntegerMask;
		if((*puiStart & kA3DMarkupIsExtraData) != 0)
		{
			uiExtraDataType = (*puiStart & kA3DMarkupExtraDataType);
			switch(uiExtraDataType)		// forced to decode these modes to traverse the inside
			{
			case kA3DMarkupFaceViewMask:
			case kA3DMarkupFrameDrawMask:
			case kA3DMarkupFixedSizeMask:
				puiStart += 1;
				break;
			case kA3DMarkupFontMask:
				A3D_INITIALIZE_DATA(A3DFontKeyData, sFontKeyData);
				sFontKeyData.m_iFontFamilyIndex = *(puiStart+2);
				sFontKeyData.m_iFontStyleIndex = (*(puiStart+3) & kA3DFontKeyStyle) >> 24;
				sFontKeyData.m_iFontSizeIndex = (*(puiStart+3) & kA3DFontKeySize) >> 12;
				sFontKeyData.m_cAttributes = (A3DInt8) (*(puiStart+3) & kA3DFontKeyAttrib);
				A3D_INITIALIZE_DATA(A3DFontData, sFontData);
				CHECK_RET(A3DGlobalFontKeyGet(&sFontKeyData, &sFontData));
				stAddFont(&sFontData);
				puiStart += size_t(uiCount + 1);
				break;
			default:
				puiStart += size_t(uiCount + 1);
				break;
			}
		}
		// forced to decode that mode to traverse the inside
		else if((*puiStart & kA3DMarkupIsMatrix) != 0)
			puiStart += 1;
		else
			puiStart += size_t(uiCount + 1);
	}

	return A3D_SUCCESS;
}

//######################################################################################################################
int traverseFonts(TiXmlElement* setting)
{
	if(stfonts == NULL)
		return A3D_ERROR;

	TiXmlElement* node = setting->FirstChildElement("A3DGlobalData");
	if(node == NULL)
		return A3D_ERROR;

	TiXmlElement* font = stfonts->FirstChildElement();
	while(font)
	{
		TiXmlElement* newfont = new TiXmlElement(*font);
		node->LinkEndChild(newfont);
		font = font->NextSiblingElement();
	}

	return A3D_SUCCESS;
}

//######################################################################################################################
static A3DInt32 stTraverseTessBase(const A3DTessBase* pTess, TiXmlElement* setting)
{
	A3DTessBaseData sData;
	A3D_INITIALIZE_DATA(A3DTessBaseData, sData);

	TiXmlElement* tessbase = new TiXmlElement("A3DTessBaseData");

	A3DInt32 iRet = A3DTessBaseGet(pTess, &sData);
	if(iRet == A3D_SUCCESS)
	{
		tessbase->SetAttribute("m_bIsCalculated", sData.m_bIsCalculated ? 1 : 0);
		traverseDoubles("m_pdCoords", sData.m_uiCoordSize, sData.m_pdCoords, tessbase);

		A3DTessBaseGet(NULL, &sData);
	}
	setting->LinkEndChild(tessbase);

	return iRet;
}

//######################################################################################################################
int traverseMarkupTess(const A3DTessMarkup* pMarkupTess, TiXmlElement* setting)
{
	A3DInt32 iRet = A3D_SUCCESS;
	A3DTessMarkupData sData;
	A3D_INITIALIZE_DATA(A3DTessMarkupData, sData);

	TiXmlElement* markuptess = new TiXmlElement("A3DTessMarkupData");

	stTraverseTessBase(pMarkupTess, markuptess);

	iRet = A3DTessMarkupGet(pMarkupTess, &sData);
	if(iRet == A3D_SUCCESS)
	{
		traverseUInts("m_puiCodes",sData.m_uiCodesSize, sData.m_puiCodes, markuptess);

		for(A3DUns32 i = 0; i < sData.m_uiTextsSize; ++i)
		{
			TiXmlElement* text = new TiXmlElement("pcText");
			text->SetAttribute("data", sData.m_ppcTexts[i]);
			markuptess->LinkEndChild(text);
		}

		if(sData.m_pcLabel)
			markuptess->SetAttribute("m_pcLabel", sData.m_pcLabel);
		markuptess->SetAttribute("m_cBehaviour", (int) sData.m_cBehaviour);

		stExportFontsFromMarkupTessellation(&sData);
		A3DTessMarkupGet(NULL, &sData);
	}
	else
	{
		markuptess->SetAttribute("error", iRet);
	}

	setting->LinkEndChild(markuptess);
	return iRet;
}

//######################################################################################################################
static int traverse3DWireTess(const A3DTess3DWire* pWireTess, TiXmlElement* setting)
{
	A3DInt32 iRet = A3D_SUCCESS;
	A3DTess3DWireData sData;
	A3D_INITIALIZE_DATA(A3DTess3DWireData, sData);

	TiXmlElement* wiretess = new TiXmlElement("A3DTess3DWireData");

	stTraverseTessBase(pWireTess, wiretess);

	iRet = A3DTess3DWireGet(pWireTess, &sData);
	if(iRet == A3D_SUCCESS)
	{
		traverseUInts("m_puiSizesWires", sData.m_uiSizesWiresSize, sData.m_puiSizesWires, wiretess);
		traverseUChars("m_pucRGBAVertices", sData.m_uiRGBAVerticesSize, sData.m_pucRGBAVertices, wiretess);

		A3DTess3DWireGet(NULL, &sData);
	}
	else
	{
		wiretess->SetAttribute("error", iRet);
	}
	setting->LinkEndChild(wiretess);

	return iRet;
}

//######################################################################################################################
static int traverseFaceTessData(const A3DTessFaceData& sFaceTessData, TiXmlElement* setting)
{
	A3DInt32 iRet = A3D_SUCCESS;

	TiXmlElement* facetess = new TiXmlElement("A3DTessFaceData");

	facetess->SetAttribute("m_usUsedEntitiesFlags", (int) sFaceTessData.m_usUsedEntitiesFlags);
	facetess->SetAttribute("m_uiStartTriangulated", (int) sFaceTessData.m_uiStartTriangulated);
	facetess->SetAttribute("m_uiStartWire", (int) sFaceTessData.m_uiStartWire);
	facetess->SetAttribute("m_uiTextureCoordIndexesSize", (int) sFaceTessData.m_uiTextureCoordIndexesSize);

	traverseUInts("m_puiStyleIndexes", sFaceTessData.m_uiStyleIndexesSize, sFaceTessData.m_puiStyleIndexes, facetess);
	traverseUInts("m_puiSizesTriangulated", sFaceTessData.m_uiSizesTriangulatedSize,
		sFaceTessData.m_puiSizesTriangulated, facetess);
	traverseUInts("m_puiSizesWires", sFaceTessData.m_uiSizesWiresSize, sFaceTessData.m_puiSizesWires, facetess);
	traverseUChars("m_pucRGBAVertices", sFaceTessData.m_uiRGBAVerticesSize, sFaceTessData.m_pucRGBAVertices, facetess);

	setting->LinkEndChild(facetess);
	return iRet;
}

//######################################################################################################################
static int sttraverse3DTess(const A3DTess3D* pTess, TiXmlElement* setting)
{
	A3DInt32 iRet = A3D_SUCCESS;
	A3DTess3DData sData;
	A3D_INITIALIZE_DATA(A3DTess3DData, sData);

	TiXmlElement* tess = new TiXmlElement("A3DTess3DData");

	stTraverseTessBase(pTess, tess);

	iRet = A3DTess3DGet(pTess, &sData);
	if(iRet == A3D_SUCCESS)
	{
		tess->SetAttribute("m_bHasFaces", (int)(sData.m_bHasFaces));
		traverseDoubles("m_pdNormals", sData.m_uiNormalSize, sData.m_pdNormals, tess);
		traverseUInts("m_puiWireIndexes", sData.m_uiWireIndexSize, sData.m_puiWireIndexes, tess);
		traverseUInts("m_puiTriangulatedIndexes", sData.m_uiTriangulatedIndexSize, sData.m_puiTriangulatedIndexes,
			tess);
		traverseDoubles("m_pdTextureCoords", sData.m_uiTextureCoordSize, sData.m_pdTextureCoords, tess);
		tess->SetDoubleAttribute("m_dCreaseAngle", sData.m_dCreaseAngle);

		for(A3DUns32 ui = 0; ui < sData.m_uiFaceTessSize; ++ui)
			traverseFaceTessData(sData.m_psFaceTessData[ui], tess);

		A3DTess3DGet(NULL, &sData);
	}
	else
	{
		tess->SetAttribute("error", iRet);
	}
	setting->LinkEndChild(tess);

	return iRet;
}

//######################################################################################################################
int traverseTessBase(const A3DTessBase* pTess, TiXmlElement* setting)
{
	A3DEEntityType eType;
	A3DInt32 iRet = A3DEntityGetType(pTess, &eType);
	if(iRet == A3D_SUCCESS)
	{
		switch(eType)
		{
		case kA3DTypeTess3D:
			sttraverse3DTess(pTess,setting);
			break;
		case kA3DTypeTess3DWire:
			traverse3DWireTess(pTess,setting);
			break;
		case kA3DTypeTessMarkup:
			traverseMarkupTess(pTess,setting);
			break;
		default:
			break;
		}
	}

	return iRet;
}
