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
int traverseTextureApplication(const A3DGraphTextureApplicationData& sData, TiXmlElement* setting)
{
	A3DInt32 iRet = A3D_SUCCESS;
	TiXmlElement *textureapplication = new TiXmlElement("A3DGraphTextureApplicationData");

	textureapplication->SetAttribute("m_uiMaterialIndex", (int) sData.m_uiMaterialIndex);
	textureapplication->SetAttribute("m_uiTextureDefinitionIndex", (int) sData.m_uiTextureDefinitionIndex);
	textureapplication->SetAttribute("m_iUVCoordinatesIndex", (int) sData.m_iUVCoordinatesIndex);
	textureapplication->SetAttribute("m_uiNextTextureApplicationIndex", (int) sData.m_uiNextTextureApplicationIndex);
	setting->LinkEndChild(textureapplication);

	return iRet;
}

//######################################################################################################################
static int sttraverseTextureTransformation(const A3DGraphTextureTransformationData& sData, TiXmlElement* setting)
{
	A3DInt32 iRet = A3D_SUCCESS;
	TiXmlElement *texturetransformation = new TiXmlElement("A3DGraphTextureTransformationData");

	texturetransformation->SetAttribute("m_bTextureFlipS", (int) sData.m_bTextureFlipS);
	texturetransformation->SetAttribute("m_bTextureFlipT", (int) sData.m_bTextureFlipT);
	traverseDoubles("m_dMatrix",16,(A3DDouble*)(&(sData.m_dMatrix)),texturetransformation);
	texturetransformation->SetAttribute("m_bIs2D", (int) sData.m_bIs2D);
	setting->LinkEndChild(texturetransformation);

	return iRet;
}

//######################################################################################################################
int traverseTextureDefinition(const A3DGraphTextureDefinitionData& sData, TiXmlElement* setting)
{
	A3DInt32 iRet = A3D_SUCCESS;
	TiXmlElement *texturedefinition = new TiXmlElement("A3DGraphTextureDefinitionData");

	texturedefinition->SetAttribute("m_uiPictureIndex", (int) sData.m_uiPictureIndex);
	texturedefinition->SetAttribute("m_ucTextureDimension", (int) sData.m_ucTextureDimension);
	texturedefinition->SetAttribute("m_eMappingType", (int) sData.m_eMappingType);
	texturedefinition->SetAttribute("m_eMappingOperator", (int) sData.m_eMappingOperator);

	// Commented out
	// traverseTransfo3d(sData.m_pOperatorTransfo, texturedefinition);

	texturedefinition->SetAttribute("m_uiMappingAttributes", (int) sData.m_uiMappingAttributes);
	traverseDoubles("m_pdMappingAttributesIntensity", sData.m_uiMappingAttributesIntensitySize,
		sData.m_pdMappingAttributesIntensity, texturedefinition);
	traverseUChars("m_pucMappingAttributesComponents", sData.m_uiMappingAttributesComponentsSize,
		sData.m_pucMappingAttributesComponents, texturedefinition);
	texturedefinition->SetAttribute("m_eTextureFunction", (int) sData.m_eTextureFunction);
	texturedefinition->SetAttribute("m_dRed", (int) sData.m_dRed);
	texturedefinition->SetAttribute("m_dGreen", (int) sData.m_dGreen);
	texturedefinition->SetAttribute("m_dBlue", (int) sData.m_dBlue);
	texturedefinition->SetAttribute("m_dAlpha", (int) sData.m_dAlpha);
	texturedefinition->SetAttribute("m_eBlend_src_RGB", (int) sData.m_eBlend_src_RGB);
	texturedefinition->SetAttribute("m_eBlend_dst_RGB", (int) sData.m_eBlend_dst_RGB);
	texturedefinition->SetAttribute("m_eBlend_src_Alpha", (int) sData.m_eBlend_src_Alpha);
	texturedefinition->SetAttribute("m_eBlend_dst_Alpha", (int) sData.m_eBlend_dst_Alpha);
	texturedefinition->SetAttribute("m_ucTextureApplyingMode", (int) sData.m_ucTextureApplyingMode);
	texturedefinition->SetAttribute("m_eTextureAlphaTest", (int) sData.m_eTextureAlphaTest);
	texturedefinition->SetAttribute("m_dAlphaTestReference", (int) sData.m_dAlphaTestReference);
	texturedefinition->SetAttribute("m_eTextureWrappingModeS", (int) sData.m_eTextureWrappingModeS);
	texturedefinition->SetAttribute("m_eTextureWrappingModeT", (int) sData.m_eTextureWrappingModeT);

	if(sData.m_pTextureTransfo != NULL)
	{
		A3DGraphTextureTransformationData sTransfoData;
		A3D_INITIALIZE_DATA(A3DGraphTextureTransformationData, sTransfoData);
		iRet = A3DGraphTextureTransformationGet(sData.m_pTextureTransfo,&sTransfoData);
		if(iRet == A3D_SUCCESS)
		{
			sttraverseTextureTransformation(sTransfoData,texturedefinition);
			A3DGraphTextureTransformationGet(NULL,&sTransfoData);
		}
	}
	setting->LinkEndChild(texturedefinition);

	return iRet;
}
