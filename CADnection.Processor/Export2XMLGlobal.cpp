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

int traverseUnit(A3DMiscAttributeUnit* const pUnit, TiXmlElement* setting)
{
	A3DInt32 iRet = A3D_SUCCESS;
	A3DMiscAttributeUnitData sDataUnit;
	A3D_INITIALIZE_DATA(A3DMiscAttributeUnitData, sDataUnit);

	TiXmlElement* unit = new TiXmlElement("A3DMiscAttributeUnit");

	CHECK_RET(A3DGlobalGetUnitData(pUnit, &sDataUnit));

	if (sDataUnit.m_pcName && sDataUnit.m_pcName[0] != '\0')
		unit->SetAttribute("m_pcName", sDataUnit.m_pcName);

	for (auto uj = 0u; uj < sDataUnit.m_uiBasicUnitSize; uj++)
	{
		TiXmlElement* basicunit = new TiXmlElement("A3D``");
		basicunit->SetAttribute("m_eUnit", (int)sDataUnit.m_ppBasicUnits[uj]->m_eUnit);
		basicunit->SetAttribute("m_iExponent", (int)sDataUnit.m_ppBasicUnits[uj]->m_iExponent);
		basicunit->SetAttribute("m_dFactor", (int)sDataUnit.m_ppBasicUnits[uj]->m_dFactor);
		unit->LinkEndChild(basicunit);
	}
	A3DGlobalGetUnitData(NULL, &sDataUnit);

	setting->LinkEndChild(unit);

	return iRet;
}

//######################################################################################################################
int traverseDottingPattern(const A3DGraphDottingPatternData& sData, TiXmlElement* setting)
{
	A3DInt32 iRet = A3D_SUCCESS;

	TiXmlElement* pattern = new TiXmlElement("A3DGraphDottingPatternData");

	_SetDoubleAttribute(pattern, "m_dPitch", (double) sData.m_dPitch);
	pattern->SetAttribute("m_bZigZag", (int) sData.m_bZigZag);
	pattern->SetAttribute("m_uiColorIndex", (int) sData.m_uiColorIndex);
	pattern->SetAttribute("m_uiNextPatternIndex", (int) sData.m_uiNextPatternIndex);

	setting->LinkEndChild(pattern);
	return iRet;
}

//######################################################################################################################
int traverseHatchingPattern(const A3DGraphHatchingPatternData& sData, TiXmlElement* setting)
{
	A3DInt32 iRet = A3D_SUCCESS;

	TiXmlElement *pattern= new TiXmlElement("A3DGraphHatchingPatternData");

	A3DUns32 ui, uiSize = sData.m_uiSize;
	for(ui = 0; ui < uiSize; ++ui)
	{
		TiXmlElement* hatchline = new TiXmlElement("A3DGraphHatchingPatternLineData");
		_SetDoubleAttribute(hatchline, "m_dAngle", (double) sData.m_psHatchLines[ui].m_dAngle);
		hatchline->SetAttribute("m_uiStyleIndex", (int) sData.m_psHatchLines[ui].m_uiStyleIndex);
		traversePoint2d("m_sStart" , sData.m_psHatchLines[ui].m_sStart, hatchline);
		traversePoint2d("m_sOffset", sData.m_psHatchLines[ui].m_sOffset, hatchline);
		pattern->LinkEndChild(hatchline);
	}
	pattern->SetAttribute("m_uiNextPatternIndex", (int) sData.m_uiNextPatternIndex);

	setting->LinkEndChild(pattern);
	return iRet;
}

//######################################################################################################################
int traverseSolidPattern(const A3DGraphSolidPatternData& sData, TiXmlElement* setting)
{
	A3DInt32 iRet = A3D_SUCCESS;

	TiXmlElement* pattern = new TiXmlElement("A3DGraphSolidPatternData");

	pattern->SetAttribute("m_bMaterial", (int) sData.m_bMaterial);
	pattern->SetAttribute("m_uiRgbColorIndex", (int) sData.m_uiRgbColorIndex);
	pattern->SetAttribute("m_uiNextPatternIndex", (int) sData.m_uiNextPatternIndex);

	setting->LinkEndChild(pattern);
	return iRet;
}

//######################################################################################################################
int traverseVPicturePattern(const A3DGraphVPicturePatternData& sData, TiXmlElement* setting)
{
	A3DInt32 iRet = A3D_SUCCESS;

	TiXmlElement* pattern = new TiXmlElement("A3DGraphVPicturePatternData");
	pattern->SetAttribute("m_uiNextPatternIndex", (int) sData.m_uiNextPatternIndex);

	traverseMarkupTess(sData.m_pMarkupTess, pattern);

	setting->LinkEndChild(pattern);
	return iRet;
}

//######################################################################################################################
//static bool stMakeFileWithBinarydata(A3DUns32 uiSize, A3DUns8* const pucBinaryData, const A3DUTF8Char* psFileName)
//{
//	FILE* psFile = fopen(psFileName, "wb");
//	if(psFile != NULL)
//	{
//		int iFileSize = uiSize;
//		fwrite(pucBinaryData, sizeof(char), size_t(iFileSize), psFile);
//		fclose(psFile);
//		return true;
//	}
//	return false;
//}

//######################################################################################################################
int traversePicture(const A3DGraphPictureData& sData, TiXmlElement* setting)
{
	A3DInt32 iRet = A3D_SUCCESS;

	TiXmlElement* picture = new TiXmlElement("A3DGraphPictureData");

	picture->SetAttribute("m_eFormat", (int) sData.m_eFormat);
	picture->SetAttribute("m_uiPixelWidth", (int) sData.m_uiPixelWidth);
	picture->SetAttribute("m_uiPixelHeight", (int) sData.m_uiPixelHeight);
	//stMakeFileWithBinarydata(sData.m_uiSize,sData.m_pucBinaryData,"myfilename");

	setting->LinkEndChild(picture);
	return iRet;
}

//######################################################################################################################
int traverseMaterial(const A3DGraphMaterialData& sData, TiXmlElement* setting)
{
	A3DInt32 iRet = A3D_SUCCESS;

	TiXmlElement* material = new TiXmlElement("A3DGraphMaterialData");

	_SetDoubleAttribute(material, "m_dAmbientAlpha", sData.m_dAmbientAlpha);
	_SetDoubleAttribute(material, "m_dDiffuseAlpha", sData.m_dDiffuseAlpha);
	_SetDoubleAttribute(material, "m_dEmissiveAlpha", sData.m_dEmissiveAlpha);
	_SetDoubleAttribute(material, "m_dSpecularAlpha", sData.m_dSpecularAlpha);
	_SetDoubleAttribute(material, "m_dShininess", sData.m_dShininess);

	material->SetAttribute("m_uiAmbient", (int) sData.m_uiAmbient);
	material->SetAttribute("m_uiDiffuse", (int) sData.m_uiDiffuse);
	material->SetAttribute("m_uiEmissive", (int) sData.m_uiEmissive);
	material->SetAttribute("m_uiSpecular", (int) sData.m_uiSpecular);

	setting->LinkEndChild(material);
	return iRet;
}

//######################################################################################################################
int traverseLinePattern(const A3DGraphLinePatternData& sData, TiXmlElement* setting)
{
	A3DInt32 iRet = A3D_SUCCESS;

	TiXmlElement* linepattern = new TiXmlElement("A3DGraphLinePatternData");
	traverseDoubles("m_pdLengths", sData.m_uiNumberOfLengths, sData.m_pdLengths, linepattern);
	_SetDoubleAttribute(linepattern, "m_dPhase", sData.m_dPhase);
	linepattern->SetAttribute("m_bRealLength", (int) sData.m_bRealLength);

	setting->LinkEndChild(linepattern);
	return iRet;
}

//######################################################################################################################
int traverseStyle(const A3DGraphStyleData& sStyleData, TiXmlElement* setting)
{
	A3DInt32 iRet = A3D_SUCCESS;

	TiXmlElement* style = new TiXmlElement("A3DGraphStyleData");

	style->SetAttribute("m_bMaterial", (int) sStyleData.m_bMaterial);
	style->SetAttribute("m_uiRgbColorIndex", (int) sStyleData.m_uiRgbColorIndex);
	style->SetAttribute("m_bVPicture", (int) sStyleData.m_bVPicture);
	style->SetAttribute("m_uiLinePatternIndex", (int) sStyleData.m_uiLinePatternIndex);

	if(sStyleData.m_bIsTransparencyDefined)
		style->SetAttribute("m_ucTransparency", (int) sStyleData.m_ucTransparency);

	_SetDoubleAttribute(style, "m_dWidth", sStyleData.m_dWidth);

	style->SetAttribute("m_bSpecialCulling", (int)sStyleData.m_bSpecialCulling);
	style->SetAttribute("m_bFrontCulling", (int)sStyleData.m_bFrontCulling);
	style->SetAttribute("m_bBackCulling", (int)sStyleData.m_bBackCulling);
	style->SetAttribute("m_bNoLight", (int)sStyleData.m_bNoLight);
	style->SetAttribute("m_eRenderingMode", (int)sStyleData.m_eRenderingMode);

	setting->LinkEndChild(style);
	return iRet;
}

//######################################################################################################################
int traverseGlobal(const A3DGlobal* pGlobal, TiXmlElement* setting) {
	A3DInt32 iRet = A3D_SUCCESS;
	A3DGlobalData sData;
	A3D_INITIALIZE_DATA(A3DGlobalData, sData);

	TiXmlElement* global = new TiXmlElement("A3DGlobalData");

	iRet = A3DGlobalGet(pGlobal, &sData);
	if(iRet == A3D_SUCCESS) {
		int traverseBase(const A3DEntity* pEntity, TiXmlElement* setting, json * json, int index, int parentIndex, int pmiIndex = -1);
		A3DUns32 ui, uiSize = sData.m_uiColorsSize;
		if(uiSize) { // Retrieve Colors
			A3DGraphRgbColorData sColorData;
			A3D_INITIALIZE_DATA(A3DGraphRgbColorData, sColorData);
			A3DDouble* pdAllColors = (A3DDouble*) malloc(size_t(uiSize) * 3 * sizeof(A3DDouble));
			if (pdAllColors) {
				for(ui = 0; ui < uiSize; ++ui) {
					CHECK_RET(A3DGlobalGetGraphRgbColorData(ui*3, &sColorData));
					pdAllColors[3*ui] = sColorData.m_dRed;
					pdAllColors[3*ui+1] = sColorData.m_dGreen;
					pdAllColors[3*ui+2] = sColorData.m_dBlue;
				}
				traverseDoubles("Colors", uiSize*3, pdAllColors, global);
				free(pdAllColors);
			}
		}

		if(sData.m_uiStylesSize) { // Retrieve A3DGraphStyleData(s)
			uiSize = sData.m_uiStylesSize;
			A3DGraphStyleData sStyleData;
			A3D_INITIALIZE_DATA(A3DGraphStyleData, sStyleData);
			for(ui = 0; ui < uiSize; ++ui) {
				CHECK_RET(A3DGlobalGetGraphStyleData(ui, &sStyleData));
				traverseStyle(sStyleData, global);
				A3DGlobalGetGraphStyleData(A3D_DEFAULT_STYLE_INDEX, &sStyleData);
			}
		}

		if(sData.m_uiLinePatternsSize) { // Retrieve A3DGraphLinePatternData(s)
			uiSize = sData.m_uiLinePatternsSize;
			A3DGraphLinePatternData sLinePatternData;
			A3D_INITIALIZE_DATA(A3DGraphLinePatternData, sLinePatternData);

			for(ui = 0; ui < uiSize; ++ui) {
				CHECK_RET(A3DGlobalGetGraphLinePatternData(ui, &sLinePatternData));
				traverseLinePattern(sLinePatternData, global);
				A3DGlobalGetGraphLinePatternData(A3D_DEFAULT_LINEPATTERN_INDEX, &sLinePatternData);
			}
		}

		if(sData.m_uiMaterialsSize) {
			uiSize= sData.m_uiMaterialsSize;

			A3DBool bIsTexture;
			for(ui = 0; ui < uiSize; ++ui) {
				CHECK_RET(A3DGlobalIsMaterialTexture(ui, &bIsTexture));

				if(bIsTexture) { // Retrieve A3DGraphTextureApplicationData(s) if any
					A3DGraphTextureApplicationData sTextureApplicationData;
					A3D_INITIALIZE_DATA(A3DGraphTextureApplicationData, sTextureApplicationData);
					CHECK_RET(A3DGlobalGetGraphTextureApplicationData(ui, &sTextureApplicationData));
					traverseTextureApplication(sTextureApplicationData, global);
					A3DGlobalGetGraphTextureApplicationData(A3D_DEFAULT_MATERIAL_INDEX, &sTextureApplicationData);
				}
				else {
					TiXmlElement* material = new TiXmlElement("A3DGraphMaterial");
					material->SetAttribute("index", ui);

					A3DEntity* pMaterial = NULL;
					CHECK_RET(A3DMiscPointerFromIndexGet(ui, kA3DTypeGraphMaterial, &pMaterial))
					if (pMaterial)
						traverseBase(pMaterial, material, NULL, -1, -1); // Retrieve A3DRootBaseData

					A3DGraphMaterialData sMaterialData;
					A3D_INITIALIZE_DATA(A3DGraphMaterialData, sMaterialData);
					CHECK_RET(A3DGlobalGetGraphMaterialData(ui, &sMaterialData));
					traverseMaterial(sMaterialData, material); // Retrieve A3DGraphMaterialData
					A3DGlobalGetGraphMaterialData(A3D_DEFAULT_MATERIAL_INDEX, &sMaterialData);

					global->LinkEndChild(material);
				}
			}
		}

		if(sData.m_uiTextureDefinitionsSize) // Retrieve A3DGraphTextureDefinitionData(s) if any
		{
			uiSize = sData.m_uiTextureDefinitionsSize;
			A3DGraphTextureDefinitionData sTextureDefinitionData;
			A3D_INITIALIZE_DATA(A3DGraphTextureDefinitionData, sTextureDefinitionData);
			for(ui = 0; ui < uiSize; ++ui)
			{
				CHECK_RET(A3DGlobalGetGraphTextureDefinitionData(ui, &sTextureDefinitionData));
				traverseTextureDefinition(sTextureDefinitionData, global);
				A3DGlobalGetGraphTextureDefinitionData(A3D_DEFAULT_TEXTURE_DEFINITION_INDEX, &sTextureDefinitionData);
			}
		}

		if(sData.m_uiPicturesSize) // Retrieve A3DGraphPicture(s)
		{
			uiSize = sData.m_uiPicturesSize;
			A3DGraphPictureData sPictureData;
			A3D_INITIALIZE_DATA(A3DGraphPictureData, sPictureData);

			for(ui = 0; ui < uiSize; ++ui)
			{
				TiXmlElement* picture = new TiXmlElement("A3DGraphPicture");
				picture->SetAttribute("index", ui);

				A3DEntity* pPicture = NULL;
				CHECK_RET(A3DMiscPointerFromIndexGet(ui, kA3DTypeGraphPicture, &pPicture))
				if (pPicture)
					traverseBase(pPicture, picture, NULL, -1, -1);

				CHECK_RET(A3DGlobalGetGraphPictureData(ui, &sPictureData));
				traversePicture(sPictureData, picture);
				A3DGlobalGetGraphPictureData(A3D_DEFAULT_MATERIAL_INDEX, &sPictureData);

				global->LinkEndChild(picture);
			}
		}

		if(sData.m_uiFillPatternsSize)
		{
			uiSize = sData.m_uiFillPatternsSize;
			A3DEEntityType ePatternType;
			for(ui = 0; ui < uiSize; ++ui)
			{
				CHECK_RET(A3DGlobalGetFillPatternType(ui, &ePatternType));

				switch(ePatternType)
				{
				case kA3DTypeGraphHatchingPattern:
					A3DGraphHatchingPatternData sHatchingPatternData;
					A3D_INITIALIZE_DATA(A3DGraphHatchingPatternData, sHatchingPatternData);
					CHECK_RET(A3DGlobalGetGraphHatchingPatternData(ui, &sHatchingPatternData));
					iRet = traverseHatchingPattern(sHatchingPatternData, global);
					A3DGlobalGetGraphHatchingPatternData(A3D_DEFAULT_PATTERN_INDEX, &sHatchingPatternData);
					break;

				case kA3DTypeGraphSolidPattern:
					A3DGraphSolidPatternData sSolidPatternData;
					A3D_INITIALIZE_DATA(A3DGraphSolidPatternData, sSolidPatternData);
					CHECK_RET(A3DGlobalGetGraphSolidPatternData(ui, &sSolidPatternData));
					iRet = traverseSolidPattern(sSolidPatternData, global);
					A3DGlobalGetGraphSolidPatternData(A3D_DEFAULT_PATTERN_INDEX, &sSolidPatternData);
					break;

				case kA3DTypeGraphDottingPattern:
					A3DGraphDottingPatternData sDottingPatternData;
					A3D_INITIALIZE_DATA(A3DGraphDottingPatternData, sDottingPatternData);
					CHECK_RET(A3DGlobalGetGraphDottingPatternData(ui, &sDottingPatternData));
					iRet = traverseDottingPattern(sDottingPatternData, global);
					A3DGlobalGetGraphDottingPatternData(A3D_DEFAULT_PATTERN_INDEX, &sDottingPatternData);
					break;

				case kA3DTypeGraphVPicturePattern:
					A3DGraphVPicturePatternData sVPicturePatternData;
					A3D_INITIALIZE_DATA(A3DGraphVPicturePatternData, sVPicturePatternData);
					CHECK_RET(A3DGlobalGetGraphVPicturePatternData(ui, &sVPicturePatternData));
					iRet = traverseVPicturePattern(sVPicturePatternData, global);
					A3DGlobalGetGraphVPicturePatternData(A3D_DEFAULT_PATTERN_INDEX, &sVPicturePatternData);
					break;

				default:
					break;
				}
			}
		}

		if (sData.m_uiUnitsSize)
		{
			uiSize = sData.m_uiUnitsSize;

			for (ui = 0; ui < uiSize; ++ui)
			{
				A3DMiscAttributeUnit *pUnit = nullptr;
				CHECK_RET(A3DGlobalGetUnit(ui, &pUnit));

				traverseUnit(pUnit, global);
			}
		}
	}
	else
	{
		global->SetAttribute("error", iRet);
	}

	setting->LinkEndChild(global);
	return iRet;
}
