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
static void sttraverseFrameType(int iFrameType, TiXmlElement* setting)
{
	switch(iFrameType)
	{
	case KA3DMarkupFrameNone:            setting->SetAttribute("m_eFrameType", "No Frame");            break;
	case KA3DMarkupFrameRectangle:       setting->SetAttribute("m_eFrameType", "Rectangle");           break;
	case KA3DMarkupFrameSquare:          setting->SetAttribute("m_eFrameType", "Square");              break;
	case KA3DMarkupFrameCircle:          setting->SetAttribute("m_eFrameType", "Circle");              break;
	case KA3DMarkupFrameScoredCircle:    setting->SetAttribute("m_eFrameType", "Scored Circle");       break;
	case KA3DMarkupFrameDiamond:         setting->SetAttribute("m_eFrameType", "Diamond");             break;
	case KA3DMarkupFrameTriangle:        setting->SetAttribute("m_eFrameType", "Triangle");            break;
	case KA3DMarkupFrameRightFlag:       setting->SetAttribute("m_eFrameType", "Right Flag");          break;
	case KA3DMarkupFrameLeftFlag:        setting->SetAttribute("m_eFrameType", "Left Flag");           break;
	case KA3DMarkupFrameBothFlag:        setting->SetAttribute("m_eFrameType", "Both Flag");           break;
	case KA3DMarkupFrameOblong:          setting->SetAttribute("m_eFrameType", "Oblong");              break;
	case KA3DMarkupFrameEllipse:         setting->SetAttribute("m_eFrameType", "Ellipse");             break;
	case KA3DMarkupFrameFixRectangle:    setting->SetAttribute("m_eFrameType", "Fixed Rectangle");     break;
	case KA3DMarkupFrameFixSquare:       setting->SetAttribute("m_eFrameType", "Fixed Square");        break;
	case KA3DMarkupFrameFixCircle:       setting->SetAttribute("m_eFrameType", "Fixed Circle");        break;
	case KA3DMarkupFrameFixScoredCircle: setting->SetAttribute("m_eFrameType", "Fixed Scored Circle"); break;
	case KA3DMarkupFrameFixDiamond:      setting->SetAttribute("m_eFrameType", "Fixed Diamond");       break;
	case KA3DMarkupFrameFixTriangle:     setting->SetAttribute("m_eFrameType", "Fixed Triangle");      break;
	case KA3DMarkupFrameFixRightFlag:    setting->SetAttribute("m_eFrameType", "Fixed Right Flag");    break;
	case KA3DMarkupFrameFixLeftFlag:     setting->SetAttribute("m_eFrameType", "Fixed Left Flag");     break;
	case KA3DMarkupFrameFixBothFlag:     setting->SetAttribute("m_eFrameType", "Fixed Both Flag");     break;
	case KA3DMarkupFrameFixOblong:       setting->SetAttribute("m_eFrameType", "Fixed Oblong");        break;
	case KA3DMarkupFrameFixEllipse:      setting->SetAttribute("m_eFrameType", "Fixed Ellipse");       break;
	case KA3DMarkupFrameCustom:          setting->SetAttribute("m_eFrameType", "Custom");              break;
	default:                             setting->SetAttribute("m_eFrameType", "unexpected");          break;
	}
}

//######################################################################################################################
static void sttraverseAlignmentType(int iAlignementType, TiXmlElement* setting)
{
	switch(iAlignementType)
	{
	case KA3DLeaderAlignement_Simple:                setting->SetAttribute("m_eLeaderAlignmentType", "Simple");                        break;
	case KA3DLeaderAlignement_Bellow:                setting->SetAttribute("m_eLeaderAlignmentType", "Bellow");                        break;
	case KA3DLeaderAlignement_Underline:             setting->SetAttribute("m_eLeaderAlignmentType", "Underline");                     break;
	case KA3DLeaderAlignement_ExtToMax:              setting->SetAttribute("m_eLeaderAlignmentType", "Extend to line maximum");        break;
	case KA3DLeaderAlignement_OnBoxPoint:            setting->SetAttribute("m_eLeaderAlignmentType", "On box point");                  break;
	case KA3DLeaderAlignement_SimpleOnBoxPoint:      setting->SetAttribute("m_eLeaderAlignmentType", "Simple on box point");           break;
	case KA3DLeaderAlignement_Maximal_Underline:     setting->SetAttribute("m_eLeaderAlignmentType", "Underline maximum");	           break;
	case KA3DLeaderAlignement_Simple_OppositeAnchor: setting->SetAttribute("m_eLeaderAlignmentType", "Simple, opposite anchor point"); break;
	default:                                         setting->SetAttribute("m_eLeaderAlignmentType", "unexpected");                    break;
	}
}

//######################################################################################################################
static void sttraverseTextPosition(const A3DMDTextPosition* pTextPosition, TiXmlElement* setting)
{
	if(pTextPosition == NULL)
	{
		setting->SetAttribute("text_position", "not defined");
		return;
	}

	A3DMDTextPositionData sTextPositionData;
	A3D_INITIALIZE_DATA(A3DMDTextPositionData, sTextPositionData);
	if(A3DMDTextPositionGet(pTextPosition, &sTextPositionData) != A3D_SUCCESS)
	{
		setting->SetAttribute("error", "text_properties");
		return;
	}

	TiXmlElement* markup_textposition = new TiXmlElement("A3DMDTextPositionData");
	setting->LinkEndChild(markup_textposition);
	traversePoint("m_sPosition"  , sTextPositionData.m_sPosition  , markup_textposition);
	traversePoint("m_sBaseVector", sTextPositionData.m_sBaseVector, markup_textposition);
	traversePoint("m_sUpVector"  , sTextPositionData.m_sUpVector  , markup_textposition);
	A3DMDTextPositionGet(NULL, &sTextPositionData);
}

//######################################################################################################################

int traverseMarkupTextProperties(const A3DMDTextProperties* pTextProperties, TiXmlElement* setting, json* jsonData, int index, int pmiIndex)
{
	if(pTextProperties == NULL)
	{
		setting->SetAttribute("text_properties", "not defined");
		return A3D_SUCCESS;
	}

	A3DStatus iRet = A3D_SUCCESS;
	A3DMDTextPropertiesData sTextPropertiesData;
	A3D_INITIALIZE_DATA(A3DMDTextPropertiesData, sTextPropertiesData);
	iRet = A3DMDTextPropertiesGet(pTextProperties, &sTextPropertiesData);
	if(iRet != A3D_SUCCESS)
	{
		setting->SetAttribute("error", "text_properties");
		return A3D_SUCCESS;
	}

	TiXmlElement* markup_textprops = new TiXmlElement("A3DMDTextPropertiesData");
	setting->LinkEndChild(markup_textprops);
	markup_textprops->SetAttribute("m_pcFont",	sTextPropertiesData.m_pcFont ? sTextPropertiesData.m_pcFont : "null");
	markup_textprops->SetDoubleAttribute("m_dAngle", sTextPropertiesData.m_dAngle);
	markup_textprops->SetAttribute("m_bSlant", sTextPropertiesData.m_bSlant ? "true" : "false");
	markup_textprops->SetAttribute("m_ucThickness", sTextPropertiesData.m_ucThickness);
	markup_textprops->SetAttribute("m_ucSize", sTextPropertiesData.m_ucSize);
	markup_textprops->SetAttribute("m_usLineStyle", sTextPropertiesData.m_usLineStyle);
	markup_textprops->SetDoubleAttribute("m_dCharHeight", sTextPropertiesData.m_dCharHeight);
	markup_textprops->SetDoubleAttribute("m_dCharSpacing", sTextPropertiesData.m_dCharSpacing);
	markup_textprops->SetDoubleAttribute("m_dCharWidth", sTextPropertiesData.m_dCharWidth);
	markup_textprops->SetAttribute("m_iCharFixedRatioMode", sTextPropertiesData.m_iCharFixedRatioMode);

	switch(sTextPropertiesData.m_eGlobalTextScoring)
	{
	case KEA3DMDTextPropertiesScoreNone:
		markup_textprops->SetAttribute("m_eGlobalTextScoring", "KEA3DMDTextPropertiesScoreNone");
		break;
	case KEA3DMDTextPropertiesUnderScored:
		markup_textprops->SetAttribute("m_eGlobalTextScoring", "KEA3DMDTextPropertiesUnderScored");
		break;
	case KEA3DMDTextPropertiesScored:
		markup_textprops->SetAttribute("m_eGlobalTextScoring", "KEA3DMDTextPropertiesScored");
		break;
	case KEA3DMDTextPropertiesOverScored:
		markup_textprops->SetAttribute("m_eGlobalTextScoring", "KEA3DMDTextPropertiesOverScored");
		break;
	default: 
		markup_textprops->SetAttribute("m_eGlobalTextScoring", "unexpected");
	}

	switch(sTextPropertiesData.m_eFormat)
	{
	case KEA3DMDTextPropertiesFormatNormal:
		markup_textprops->SetAttribute("m_eFormat", "KEA3DMDTextPropertiesFormatNormal");
		break;
	case KEA3DMDTextPropertiesFormatUnderLine:
		markup_textprops->SetAttribute("m_eFormat", "KEA3DMDTextPropertiesFormatUnderLine");
		break;
	case KEA3DMDTextPropertiesFormatOverLine:
		markup_textprops->SetAttribute("m_eFormat", "KEA3DMDTextPropertiesFormatOverLine");
		break;
	case KEA3DMDTextPropertiesFormatExposant:
		markup_textprops->SetAttribute("m_eFormat", "KEA3DMDTextPropertiesFormatExposant");
		break;
	case KEA3DMDTextPropertiesFormatindice:
		markup_textprops->SetAttribute("m_eFormat", "KEA3DMDTextPropertiesFormatindice");
		break;
	default: 
		markup_textprops->SetAttribute("m_eFormat", "unexpected");
	}

	switch(sTextPropertiesData.m_eJustification)
	{
	case KEA3DMDTextPropertiesJustificationLeft:
		markup_textprops->SetAttribute("m_eJustification", "KEA3DMDTextPropertiesJustificationLeft");
		break;
	case KEA3DMDTextPropertiesJustificationCenter:
		markup_textprops->SetAttribute("m_eJustification", "KEA3DMDTextPropertiesJustificationCenter");
		break;
	case KEA3DMDTextPropertiesJustificationRight:
		markup_textprops->SetAttribute("m_eJustification", "KEA3DMDTextPropertiesJustificationRight");
		break;
	default: 
		markup_textprops->SetAttribute("m_eJustification", "unexpected");
	}

	return A3DMDTextPropertiesGet(NULL, &sTextPropertiesData);
}

//######################################################################################################################

int traverseMarkupText(const A3DMarkupText* pMarkup, TiXmlElement* setting, json* jsonData, int index, int pmiIndex)
{
	A3DMarkupTextData sMarkupData;
	A3D_INITIALIZE_DATA(A3DMarkupTextData, sMarkupData);
	TiXmlElement* markup_text = new TiXmlElement("A3DMarkupTextData");
	setting->LinkEndChild(markup_text);
	A3DStatus iRet = A3DMarkupTextGet(pMarkup, &sMarkupData);
	if(iRet != A3D_SUCCESS)
	{
		markup_text->SetAttribute("error", iRet);
		return iRet;
	}

	char acName[20];
	for(A3DUns32 i = 0; i < sMarkupData.m_uiLinesSize; i++)
	{
		sprintf(acName, "line_%u", i);
		markup_text->SetAttribute(acName, sMarkupData.m_ppLines[i]);
	}
	for(A3DUns32 i = 0; i < sMarkupData.m_uiFileLinksSize; i++)
	{
		sprintf(acName, "file_link_%u", i);
		markup_text->SetAttribute(acName, sMarkupData.m_ppFileLinks[i]);
	}
	sttraverseFrameType(sMarkupData.m_eFrameType, markup_text);
	sttraverseAlignmentType(sMarkupData.m_eLeaderAlignementType, markup_text);
	sttraverseTextPosition(sMarkupData.m_psTextPosition, markup_text);
	markup_text->SetDoubleAttribute("Wrapping_width", sMarkupData.m_dWrappingWidth);
	traverseMarkupTextProperties(sMarkupData.m_psTextPosition, markup_text, jsonData, index, pmiIndex);

	return A3DMarkupTextGet(NULL, &sMarkupData);
}

//######################################################################################################################
int traverseMarkupRichText(const A3DMarkupRichText* pMarkup, TiXmlElement* setting, json* jsonData, int index, int pmiIndex)
{
	A3DMarkupRichTextData sMarkupData;
	A3D_INITIALIZE_DATA(A3DMarkupRichTextData, sMarkupData);
	TiXmlElement* markup_text = new TiXmlElement("A3DMarkupRichTextData");
	setting->LinkEndChild(markup_text);
	A3DStatus iRet = A3DMarkupRichTextGet(pMarkup, &sMarkupData);
	if(iRet != A3D_SUCCESS)
	{
		markup_text->SetAttribute("error", iRet);
		return iRet;
	}

	markup_text->SetAttribute("m_pcRichText", sMarkupData.m_pcRichText?sMarkupData.m_pcRichText:"NULL");
	if (pmiIndex != -1 && sMarkupData.m_pcRichText) {
		(*jsonData)["docs"][index]["PMI"][pmiIndex]["Text"] = sMarkupData.m_pcRichText;
	}
	char acName[20];
	for(A3DUns32 i = 0; i < sMarkupData.m_uiFileLinksSize; i++)
	{
		sprintf(acName, "file_link_%u", i);
		markup_text->SetAttribute(acName, sMarkupData.m_ppFileLinks[i]);
	}
	sttraverseFrameType(sMarkupData.m_eFrameType, markup_text);
	sttraverseAlignmentType(sMarkupData.m_eLeaderAlignementType, markup_text);
	sttraverseTextPosition(sMarkupData.m_psTextPosition, markup_text);
	markup_text->SetDoubleAttribute("Wrapping_width", sMarkupData.m_dWrappingWidth);

	return A3DMarkupRichTextGet(NULL, &sMarkupData);
}
