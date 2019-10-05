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
static void sttraverseDimensionType(int iDimensionType, TiXmlElement* setting, json* jsonData, int index, int pmiIndex)
{
	switch(iDimensionType)
	{
		case KEA3DMDDimensionTypeDistance: 
			setting->SetAttribute("m_eType", "KEA3DMDDimensionTypeDistance"); 
			if (pmiIndex != -1)
				(*jsonData)["docs"][index]["PMI"][pmiIndex]["DimensionType"] = "Distance";
			break;
		case KEA3DMDDimensionTypeDistanceOffset: 
			setting->SetAttribute("m_eType", "KEA3DMDDimensionTypeDistanceOffset"); 
			if (pmiIndex != -1)
				(*jsonData)["docs"][index]["PMI"][pmiIndex]["DimensionType"] = "Distance Offset";
			break;
		case KEA3DMDDimensionTypeLength: 
			setting->SetAttribute("m_eType", "KEA3DMDDimensionTypeLength"); 
			if (pmiIndex != -1)
				(*jsonData)["docs"][index]["PMI"][pmiIndex]["DimensionType"] = "Length";
			break;
		case KEA3DMDDimensionTypeLengthCurvilinear: 
			setting->SetAttribute("m_eType", "KEA3DMDDimensionTypeLengthCurvilinear"); 
			if (pmiIndex != -1)
				(*jsonData)["docs"][index]["PMI"][pmiIndex]["DimensionType"] = "Length Curvilinear";
			break;
		case KEA3DMDDimensionTypeAngle: 
			setting->SetAttribute("m_eType", "KEA3DMDDimensionTypeAngle"); 
			if (pmiIndex != -1)
				(*jsonData)["docs"][index]["PMI"][pmiIndex]["DimensionType"] = "Angle";
			break;
		case KEA3DMDDimensionTypeRadius: 
			setting->SetAttribute("m_eType", "KEA3DMDDimensionTypeRadius"); 
			if (pmiIndex != -1)
				(*jsonData)["docs"][index]["PMI"][pmiIndex]["DimensionType"] = "Distance";
			break;
		case KEA3DMDDimensionTypeRadiusTangent: 
			setting->SetAttribute("m_eType", "KEA3DMDDimensionTypeRadiusTangent"); 
			if (pmiIndex != -1)
				(*jsonData)["docs"][index]["PMI"][pmiIndex]["DimensionType"] = "Radius Tangent";
			break;
		case KEA3DMDDimensionTypeRadiusCylinder: 
			setting->SetAttribute("m_eType", "KEA3DMDDimensionTypeRadiusCylinder"); 
			if (pmiIndex != -1)
				(*jsonData)["docs"][index]["PMI"][pmiIndex]["DimensionType"] = "Radius Cylinder";
			break;
		case KEA3DMDDimensionTypeRadiusEdge: 
			setting->SetAttribute("m_eType", "KEA3DMDDimensionTypeRadiusEdge"); 
			if (pmiIndex != -1)
				(*jsonData)["docs"][index]["PMI"][pmiIndex]["DimensionType"] = "Radius Edge";
			break;
		case KEA3DMDDimensionTypeDiameter: 
			setting->SetAttribute("m_eType", "KEA3DMDDimensionTypeDiameter"); 
			if (pmiIndex != -1)
				(*jsonData)["docs"][index]["PMI"][pmiIndex]["DimensionType"] = "Diameter";
			break;
		case KEA3DMDDimensionTypeDiameterTangent: 
			setting->SetAttribute("m_eType", "KEA3DMDDimensionTypeDiameterTangent"); 
			if (pmiIndex != -1)
				(*jsonData)["docs"][index]["PMI"][pmiIndex]["DimensionType"] = "Diameter Tangent";
			break;
		case KEA3DMDDimensionTypeDiameterCylinder: 
			setting->SetAttribute("m_eType", "KEA3DMDDimensionTypeDiameterCylinder"); 
			if (pmiIndex != -1)
				(*jsonData)["docs"][index]["PMI"][pmiIndex]["DimensionType"] = "Diameter Cylinder";
			break;
		case KEA3DMDDimensionTypeDiameterEdge: 
			setting->SetAttribute("m_eType", "KEA3DMDDimensionTypeDiameterEdge"); 
			if (pmiIndex != -1)
				(*jsonData)["docs"][index]["PMI"][pmiIndex]["DimensionType"] = "Diameter Edge";
			break;
		case KEA3DMDDimensionTypeDiameterCone: 
			setting->SetAttribute("m_eType", "KEA3DMDDimensionTypeDiameterCone"); 
			if (pmiIndex != -1)
				(*jsonData)["docs"][index]["PMI"][pmiIndex]["DimensionType"] = "Diameter Cone";
			break;
		case KEA3DMDDimensionTypeChamfer: 
			setting->SetAttribute("m_eType", "KEA3DMDDimensionTypeChamfer"); 
			if (pmiIndex != -1)
				(*jsonData)["docs"][index]["PMI"][pmiIndex]["DimensionType"] = "Chamfer";
			break;
		case KEA3DMDDimensionTypeSlope: 
			setting->SetAttribute("m_eType", "KEA3DMDDimensionTypeSlope"); 
			if (pmiIndex != -1)
				(*jsonData)["docs"][index]["PMI"][pmiIndex]["DimensionType"] = "Slope";
			break;
		default: 
			setting->SetAttribute("m_eType", "unexpected"); 
			if (pmiIndex != -1)
				(*jsonData)["docs"][index]["PMI"][pmiIndex]["DimensionType"] = "unexpected";
			break;
	}
}

//######################################################################################################################

static void sttraverseDimensionValueFormat(const A3DMDDimensionValueFormat* psValueFormat, TiXmlElement* setting)
{
	if(psValueFormat == NULL)
	{
		setting->SetAttribute("psValueFormat", "NULL");
		return;
	}

	A3DMDDimensionValueFormatData sValueFormatData;
	A3D_INITIALIZE_DATA(A3DMDDimensionValueFormatData, sValueFormatData);

	TiXmlElement* markup_dimvalueformat = new TiXmlElement("A3DMDDimensionValueFormatData");
	setting->LinkEndChild(markup_dimvalueformat);

	A3DStatus iRet = A3DMDDimensionValueFormatGet(psValueFormat, &sValueFormatData);
	if(iRet != A3D_SUCCESS)
	{
		markup_dimvalueformat->SetAttribute("error", iRet);
		return;
	}

	markup_dimvalueformat->SetAttribute("m_pcName", sValueFormatData.m_pcName?sValueFormatData.m_pcName:"dimension");
	markup_dimvalueformat->SetAttribute("m_iType", sValueFormatData.m_iType);
	markup_dimvalueformat->SetAttribute("m_iUnit", sValueFormatData.m_iUnit);
	markup_dimvalueformat->SetDoubleAttribute("m_dGlobFact", sValueFormatData.m_dGlobFact);
	markup_dimvalueformat->SetAttribute("m_iNulFac_1", sValueFormatData.m_iNulFac_1);
	markup_dimvalueformat->SetAttribute("m_iNulFac_2", sValueFormatData.m_iNulFac_2);
	markup_dimvalueformat->SetAttribute("m_iExise", sValueFormatData.m_iExise);
	markup_dimvalueformat->SetAttribute("m_iSep1000", sValueFormatData.m_iSep1000);
	markup_dimvalueformat->SetDoubleAttribute("m_dFact_1", sValueFormatData.m_dFact_1);
	markup_dimvalueformat->SetDoubleAttribute("m_dFact_2", sValueFormatData.m_dFact_2);
	markup_dimvalueformat->SetDoubleAttribute("m_dFact_3", sValueFormatData.m_dFact_3);
	markup_dimvalueformat->SetDoubleAttribute("m_dValPos_1", sValueFormatData.m_dValPos_1);
	markup_dimvalueformat->SetDoubleAttribute("m_dValPos_2", sValueFormatData.m_dValPos_2);
	markup_dimvalueformat->SetDoubleAttribute("m_dValPos_3", sValueFormatData.m_dValPos_3);
	markup_dimvalueformat->SetAttribute("m_pcSepar_1",
		sValueFormatData.m_pcSepar_1 ? sValueFormatData.m_pcSepar_1 : "null");
	markup_dimvalueformat->SetAttribute("m_pcSepar_1",
		sValueFormatData.m_pcSepar_2 ? sValueFormatData.m_pcSepar_2 : "null");
	markup_dimvalueformat->SetAttribute("m_pcSepar_1",
		sValueFormatData.m_pcSepar_3 ? sValueFormatData.m_pcSepar_3 : "null");
	markup_dimvalueformat->SetDoubleAttribute("m_dSepScl_1", sValueFormatData.m_dSepScl_1);
	markup_dimvalueformat->SetDoubleAttribute("m_dSepScl_2", sValueFormatData.m_dSepScl_2);
	markup_dimvalueformat->SetDoubleAttribute("m_dSepScl_3", sValueFormatData.m_dSepScl_3);
	markup_dimvalueformat->SetDoubleAttribute("m_dSepPos_1", sValueFormatData.m_dSepPos_1);
	markup_dimvalueformat->SetDoubleAttribute("m_dSepPos_2", sValueFormatData.m_dSepPos_2);
	markup_dimvalueformat->SetDoubleAttribute("m_dSepPos_3", sValueFormatData.m_dSepPos_3);
	markup_dimvalueformat->SetDoubleAttribute("m_dRestY", sValueFormatData.m_dRestY);
	markup_dimvalueformat->SetAttribute("m_iFinZer", sValueFormatData.m_iFinZer);
	markup_dimvalueformat->SetAttribute("m_iSepNum", sValueFormatData.m_iSepNum);
	markup_dimvalueformat->SetAttribute("m_iTypFrac", sValueFormatData.m_iTypFrac);
	markup_dimvalueformat->SetAttribute("m_iSepDen", sValueFormatData.m_iSepDen);
	markup_dimvalueformat->SetDoubleAttribute("m_dOperY", sValueFormatData.m_dOperY);
	markup_dimvalueformat->SetAttribute("m_iNulOther", sValueFormatData.m_iNulOther);
	markup_dimvalueformat->SetDoubleAttribute("m_dResScl", sValueFormatData.m_dResScl);
	markup_dimvalueformat->SetAttribute("m_iFact", sValueFormatData.m_iFact);
	markup_dimvalueformat->SetDoubleAttribute("m_dRestX", sValueFormatData.m_dRestX);

	A3DMDDimensionValueFormatGet(NULL, &sValueFormatData);
}

//######################################################################################################################
static void sttraverseDimensionToleranceSimpleFormat(const A3DMDDimensionSimpleToleranceFormat* psToleranceFormat,
													 TiXmlElement* setting)
{
	if(psToleranceFormat == NULL)
	{
		setting->SetAttribute("psToleranceFormat", "NULL");
		return;
	}
	A3DMDDimensionSimpleToleranceFormatData sFormatData;
	A3D_INITIALIZE_DATA(A3DMDDimensionSimpleToleranceFormatData, sFormatData);

	TiXmlElement* markup_dimtolformat = new TiXmlElement("A3DMDDimensionSimpleToleranceFormat");
	setting->LinkEndChild(markup_dimtolformat);

	A3DStatus iRet = A3DMDDimensionSimpleToleranceFormatGet(psToleranceFormat, &sFormatData);
	if(iRet != A3D_SUCCESS)
	{
		markup_dimtolformat->SetAttribute("error", iRet);
		return;
	}

	markup_dimtolformat->SetAttribute("m_pcName", (sFormatData.m_pcName && sFormatData.m_pcName[0]!=0)?sFormatData.m_pcName:"no name");
	markup_dimtolformat->SetAttribute("m_iType", sFormatData.m_iType);
	markup_dimtolformat->SetAttribute("m_iSepar_1", sFormatData.m_iSepar_1);
	markup_dimtolformat->SetAttribute("m_iSepar_2", sFormatData.m_iSepar_2);
	markup_dimtolformat->SetDoubleAttribute("m_dSymbolH", sFormatData.m_dSymbolH);
	markup_dimtolformat->SetAttribute("m_iSepTo_1", sFormatData.m_iSepTo_1);
	markup_dimtolformat->SetAttribute("m_iSepTo_2", sFormatData.m_iSepTo_2);
	markup_dimtolformat->SetAttribute("m_iSepTo_3", sFormatData.m_iSepTo_3);
	markup_dimtolformat->SetAttribute("m_iTrailing", sFormatData.m_iTrailing);
	markup_dimtolformat->SetAttribute("m_iFractLine", sFormatData.m_iFractLine);
	markup_dimtolformat->SetAttribute("m_iPtOnValue", sFormatData.m_iPtOnValue);
	markup_dimtolformat->SetAttribute("m_iAnchorPt", sFormatData.m_iAnchorPt);
	markup_dimtolformat->SetDoubleAttribute("m_dIntX", sFormatData.m_dIntX);
	markup_dimtolformat->SetDoubleAttribute("m_dIntY", sFormatData.m_dIntY);
	markup_dimtolformat->SetDoubleAttribute("m_dExtX", sFormatData.m_dExtX);
	markup_dimtolformat->SetDoubleAttribute("m_dExtY", sFormatData.m_dExtY);

	markup_dimtolformat->SetAttribute("m_iMergeSame", sFormatData.m_iMergeSame);
	markup_dimtolformat->SetAttribute("m_iShowNull", sFormatData.m_iShowNull);

	markup_dimtolformat->SetDoubleAttribute("m_dScale", sFormatData.m_dScale);

	A3DMDDimensionSimpleToleranceFormatGet(NULL, &sFormatData);
}

//######################################################################################################################
static void sttraverseDimensionToleranceFormat(const A3DMDDimensionToleranceFormat* psToleranceFormat,
											   TiXmlElement* setting)
{
	A3DEEntityType eType = kA3DTypeUnknown;
	if(psToleranceFormat == NULL)
	{
		setting->SetAttribute("psToleranceFormat", "NULL");
		return;
	}

	A3DStatus iRet = A3DEntityGetType(psToleranceFormat, &eType);
	if(iRet != A3D_SUCCESS)
	{
		setting->SetAttribute("psToleranceFormat", "type error");
		return;
	}

	if(eType == kA3DTypeMDDimensionSimpleToleranceFormat)
	{
		sttraverseDimensionToleranceSimpleFormat((A3DMDDimensionSimpleToleranceFormat*)psToleranceFormat, setting);
	}
	else if(eType == kA3DTypeMDDimensionCombinedToleranceFormat)
	{
		A3DMDDimensionCombinedToleranceFormatData sFormatData;
		A3D_INITIALIZE_DATA(A3DMDDimensionCombinedToleranceFormatData, sFormatData);

		TiXmlElement* markup_dimtolformat = new TiXmlElement("A3DMDDimensionCombinedToleranceFormatData");
		setting->LinkEndChild(markup_dimtolformat);

		iRet = A3DMDDimensionCombinedToleranceFormatGet(psToleranceFormat, &sFormatData);
		if(iRet != A3D_SUCCESS)
		{
			markup_dimtolformat->SetAttribute("psToleranceFormat", "NULL");
			return;
		}

		markup_dimtolformat->SetAttribute("m_pcName", (sFormatData.m_pcName && sFormatData.m_pcName[0]!=0)?sFormatData.m_pcName:"no name");
		markup_dimtolformat->SetAttribute("m_iType", sFormatData.m_iType);
		sttraverseDimensionToleranceSimpleFormat(sFormatData.m_pToleranceFormat1, markup_dimtolformat);
		markup_dimtolformat->SetDoubleAttribute("m_dExtX1", sFormatData.m_dExtX1);
		markup_dimtolformat->SetDoubleAttribute("m_dExtY1", sFormatData.m_dExtY1);
		markup_dimtolformat->SetAttribute("m_iAnchorPt1", sFormatData.m_iAnchorPt1);
		markup_dimtolformat->SetAttribute("m_iPtOnValue1", sFormatData.m_iPtOnValue1);
		markup_dimtolformat->SetAttribute("m_iSepar_1", sFormatData.m_iSepar_1);
		sttraverseDimensionToleranceSimpleFormat(sFormatData.m_pToleranceFormat2, markup_dimtolformat);
		markup_dimtolformat->SetDoubleAttribute("m_dExtX2", sFormatData.m_dExtX2);
		markup_dimtolformat->SetDoubleAttribute("m_dExtY2", sFormatData.m_dExtY2);
		markup_dimtolformat->SetAttribute("m_iAnchorPt2", sFormatData.m_iAnchorPt2);
		markup_dimtolformat->SetAttribute("m_iPtOnValue2", sFormatData.m_iPtOnValue2);
		markup_dimtolformat->SetAttribute("m_iSepar_2", sFormatData.m_iSepar_2);
		A3DMDDimensionCombinedToleranceFormatGet(NULL, &sFormatData);
	}
	else
		setting->SetAttribute("A3DMDDimensionToleranceFormat", "unexpected type");
}

//######################################################################################################################
static void sttraverseDimensionValue(const A3DMDDimensionValue* psValue, TiXmlElement* setting)
{
	A3DMDDimensionValueData sValueData;
	A3D_INITIALIZE_DATA(A3DMDDimensionValueData, sValueData);

	TiXmlElement* markup_dimvalue = new TiXmlElement("A3DMDDimensionValueData");
	setting->LinkEndChild(markup_dimvalue);

	A3DStatus iRet = A3DMDDimensionValueGet(psValue, &sValueData);
	if(iRet != A3D_SUCCESS)
	{
		markup_dimvalue->SetAttribute("error", iRet);
		return;
	}
	if(sValueData.m_iType == 0)
		markup_dimvalue->SetAttribute("m_iType", "true value");
	else if(sValueData.m_iType == 1)
		markup_dimvalue->SetAttribute("m_iType", "fake num value");
	else if(sValueData.m_iType == 2)
		markup_dimvalue->SetAttribute("m_iType", "fake alpha num value");
	else
		markup_dimvalue->SetAttribute("m_iType", "unexpected");

	markup_dimvalue->SetAttribute("m_bFormat", sValueData.m_bFormat ? "decimal format" : "fractional");
	markup_dimvalue->SetDoubleAttribute("m_dAccuracy", sValueData.m_dAccuracy);
	markup_dimvalue->SetDoubleAttribute("m_dToleranceAccuracy", sValueData.m_dToleranceAccuracy);
	markup_dimvalue->SetAttribute("m_pcFakeValue", sValueData.m_pcFakeValue ? sValueData.m_pcFakeValue : "null");
	markup_dimvalue->SetDoubleAttribute("m_dTolNumSup", sValueData.m_dTolNumSup);
	markup_dimvalue->SetDoubleAttribute("m_dTolNumInf", sValueData.m_dTolNumInf);
	markup_dimvalue->SetAttribute("m_pcTolTxtSup", sValueData.m_pcTolTxtSup ? sValueData.m_pcTolTxtSup : "null");
	markup_dimvalue->SetAttribute("m_pcTolTxtInf", sValueData.m_pcTolTxtInf ? sValueData.m_pcTolTxtInf : "null");
	markup_dimvalue->SetAttribute("m_pcBeforeText", sValueData.m_pcBeforeText ? sValueData.m_pcBeforeText : "null");
	markup_dimvalue->SetAttribute("m_pcAfterText", sValueData.m_pcAfterText ? sValueData.m_pcAfterText : "null");
	markup_dimvalue->SetAttribute("m_pcAboveText", sValueData.m_pcAboveText ? sValueData.m_pcAboveText : "null");
	markup_dimvalue->SetAttribute("m_pcBelowText", sValueData.m_pcBelowText ? sValueData.m_pcBelowText : "null");
	markup_dimvalue->SetAttribute("m_usLastSeparDefinedNum", sValueData.m_usLastSeparDefinedNum);

	markup_dimvalue->SetAttribute("m_iOption", sValueData.m_iOption);
	markup_dimvalue->SetDoubleAttribute("m_dDeltaForMinMax", sValueData.m_dDeltaForMinMax);

	sttraverseDimensionValueFormat(sValueData.m_pDimValueFormat, markup_dimvalue);
	sttraverseDimensionToleranceFormat(sValueData.m_pToleranceFormat, markup_dimvalue);

	if(sValueData.m_pChamferDimSecondValue)
		sttraverseDimensionValue(sValueData.m_pChamferDimSecondValue, markup_dimvalue);

	A3DMDDimensionValueGet(NULL, &sValueData);
}

//######################################################################################################################
static void sttraverseDimensionExtentionLine(const A3DMDDimensionExtentionLine* psExtentionLine, TiXmlElement* setting)
{
	A3DMDDimensionExtentionLineData sData;
	A3D_INITIALIZE_DATA(A3DMDDimensionExtentionLineData, sData);

	TiXmlElement* markup_extline = new TiXmlElement("A3DMDDimensionExtentionLineData");
	setting->LinkEndChild(markup_extline);

	A3DStatus iRet = A3DMDDimensionExtentionLineGet(psExtentionLine, &sData);
	if(iRet != A3D_SUCCESS)
	{
		markup_extline->SetAttribute("error", iRet);
		return;
	}

	if(sData.m_pFunnel)
	{
		A3DMDDimensionFunnelData sFunnelData;
		A3D_INITIALIZE_DATA(A3DMDDimensionFunnelData, sFunnelData);

		TiXmlElement* markup_funnel = new TiXmlElement("A3DMDDimensionFunnelData");
		markup_extline->LinkEndChild(markup_funnel);

		iRet = A3DMDDimensionFunnelGet(sData.m_pFunnel, &sFunnelData);
		if(iRet != A3D_SUCCESS)
		{
			markup_funnel->SetAttribute("error", iRet);
			return;
		}

		markup_funnel->SetAttribute("m_usIndex", sFunnelData.m_usIndex);
		markup_funnel->SetAttribute("m_bMode", sFunnelData.m_bMode ? "true" : "false");
		markup_funnel->SetDoubleAttribute("m_dAngle", sFunnelData.m_dAngle);
		markup_funnel->SetDoubleAttribute("m_dHeight", sFunnelData.m_dHeight);
		markup_funnel->SetDoubleAttribute("m_dWidth", sFunnelData.m_dWidth);
		A3DMDDimensionFunnelGet(NULL, &sFunnelData);
	}

	TiXmlElement* markup_ext1 = new TiXmlElement("m_sExtremity1");
	markup_extline->LinkEndChild(markup_ext1);
	markup_ext1->SetAttribute("m_bShow", sData.m_sExtremity1.m_bShow ? "true" : "false");
	markup_ext1->SetDoubleAttribute("m_dBlanking", sData.m_sExtremity1.m_dBlanking);
	markup_ext1->SetDoubleAttribute("m_dOverrun", sData.m_sExtremity1.m_dOverrun);

	TiXmlElement* markup_ext2 = new TiXmlElement("m_sExtremity2");
	markup_extline->LinkEndChild(markup_ext2);
	markup_ext2->SetAttribute("m_bShow", sData.m_sExtremity2.m_bShow ? "true" : "false");
	markup_ext2->SetDoubleAttribute("m_dBlanking", sData.m_sExtremity2.m_dBlanking);
	markup_ext2->SetDoubleAttribute("m_dOverrun", sData.m_sExtremity2.m_dOverrun);

	markup_extline->SetDoubleAttribute("m_dLineSlant", sData.m_dLineSlant);
	markup_extline->SetAttribute("m_usStyle", sData.m_usStyle);
	markup_extline->SetDoubleAttribute("m_fThickness", sData.m_fThickness);

	A3DMDDimensionExtentionLineGet(NULL, &sData);
}

//######################################################################################################################
static void sttraverseDimensionLineSymbol(const A3DMDDimensionLineSymbol* psLineSymbol, TiXmlElement* setting)
{
	if(psLineSymbol == NULL)
		return;

	A3DMDDimensionLineSymbolData sData;
	A3D_INITIALIZE_DATA(A3DMDDimensionLineSymbolData, sData);

	TiXmlElement* markup_dimline = new TiXmlElement("A3DMDDimensionLineSymbolData");
	setting->LinkEndChild(markup_dimline);

	A3DStatus iRet = A3DMDDimensionLineSymbolGet(psLineSymbol, &sData);
	if(iRet != A3D_SUCCESS)
	{
		markup_dimline->SetAttribute("error", iRet);
		return;
	}

	markup_dimline->SetAttribute("m_eShape", sData.m_eShape);
	markup_dimline->SetAttribute("m_iLineColor", sData.m_iLineColor);
	markup_dimline->SetDoubleAttribute("m_fThickness", sData.m_fThickness);
	markup_dimline->SetDoubleAttribute("m_fArrowLength", sData.m_fArrowLength);
	markup_dimline->SetDoubleAttribute("m_fArrowAngle", sData.m_fArrowAngle);
	A3DMDDimensionLineSymbolGet(NULL, &sData);
}


//######################################################################################################################
static void sttraverseDimensionSecondPart(const A3DMDDimensionSecondPart* psSecondPart, TiXmlElement* setting)
{
	if (psSecondPart == NULL)
		return;

	A3DMDDimensionSecondPartData sData;
	A3D_INITIALIZE_DATA(A3DMDDimensionSecondPartData, sData);

	TiXmlElement* markup_secondpart = new TiXmlElement("A3DMDDimensionSecondPartData");
	setting->LinkEndChild(markup_secondpart);

	A3DStatus iRet = A3DMDDimensionSecondPartGet(psSecondPart, &sData);
	if (iRet != A3D_SUCCESS)
	{
		markup_secondpart->SetAttribute("error", iRet);
		return;
	}

	markup_secondpart->SetAttribute("m_eOrientation", sData.m_eOrientation);
	markup_secondpart->SetDoubleAttribute("m_dAngle", sData.m_dAngle);
	markup_secondpart->SetDoubleAttribute("m_dOffSet", sData.m_dOffSet);

	A3DMDDimensionSecondPartGet(NULL, &sData);
}


//######################################################################################################################
static void sttraverseDimensionForeshortened(const A3DMDDimensionForeshortened* psForeshortened, TiXmlElement* setting)
{
	if (psForeshortened == NULL)
		return;

	A3DMDDimensionForeshortenedData sData;
	A3D_INITIALIZE_DATA(A3DMDDimensionForeshortenedData, sData);

	TiXmlElement* markup_Foreshortened = new TiXmlElement("A3DMDDimensionForeshortenedData");
	setting->LinkEndChild(markup_Foreshortened);

	A3DStatus iRet = A3DMDDimensionForeshortenedGet(psForeshortened, &sData);
	if (iRet != A3D_SUCCESS)
	{
		markup_Foreshortened->SetAttribute("error", iRet);
		return;
	}

	markup_Foreshortened->SetAttribute("m_bOrientation", sData.m_bOrientation);
	markup_Foreshortened->SetAttribute("m_bTextPosition", sData.m_bTextPosition);
	markup_Foreshortened->SetDoubleAttribute("m_dAngle", sData.m_dAngle);
	markup_Foreshortened->SetDoubleAttribute("m_fRatio", sData.m_fRatio);
	markup_Foreshortened->SetAttribute("m_usPointScale", sData.m_usPointScale);

	A3DMDDimensionForeshortenedGet(NULL, &sData);
}

//######################################################################################################################
static void sttraverseDimensionLine(const A3DMDDimensionLine* psLine, TiXmlElement* setting)
{
	if (psLine == NULL)
		return;

	A3DMDDimensionLineData sData;
	A3D_INITIALIZE_DATA(A3DMDDimensionLineData, sData);

	TiXmlElement* markup_dimline = new TiXmlElement("A3DMDDimensionLineData");
	setting->LinkEndChild(markup_dimline);

	A3DStatus iRet = A3DMDDimensionLineGet(psLine, &sData);
	if (iRet != A3D_SUCCESS)
	{
		markup_dimline->SetAttribute("error", iRet);
		return;
	}
	if (sData.m_pSymbol1)
	{
		TiXmlElement* Symbol1 = new TiXmlElement("Symbol1");
		markup_dimline->LinkEndChild(Symbol1);
		sttraverseDimensionLineSymbol(sData.m_pSymbol1, Symbol1);
	}
	if (sData.m_pSymbol2)
	{
		TiXmlElement* Symbol2 = new TiXmlElement("Symbol2");
		markup_dimline->LinkEndChild(Symbol2);
		sttraverseDimensionLineSymbol(sData.m_pSymbol2, Symbol2);
	}
	if (sData.m_pLeaderSymbol)
	{
		TiXmlElement* LeaderSymbol = new TiXmlElement("LeaderSymbol");
		markup_dimline->LinkEndChild(LeaderSymbol);
		sttraverseDimensionLineSymbol(sData.m_pLeaderSymbol, LeaderSymbol);
	}

	markup_dimline->SetAttribute("m_eGraphRepresentation", sData.m_eGraphRepresentation);
	if (sData.m_pSecondPart)
	{
		TiXmlElement* SecondPart = new TiXmlElement("SecondPart");
		markup_dimline->LinkEndChild(SecondPart);
		sttraverseDimensionSecondPart(sData.m_pSecondPart, SecondPart);
	}
	
	markup_dimline->SetAttribute("m_usStyle", sData.m_usStyle);
	markup_dimline->SetDoubleAttribute("m_fThickness", sData.m_fThickness);
	markup_dimline->SetAttribute("m_usReversal", sData.m_usReversal);

	if (sData.m_psForeshortened)
	{
		TiXmlElement* Foreshortened = new TiXmlElement("Foreshortened");
		markup_dimline->LinkEndChild(Foreshortened);
		sttraverseDimensionForeshortened(sData.m_psForeshortened, Foreshortened);
	}
	markup_dimline->SetDoubleAttribute("m_dLeaderAngle", sData.m_dLeaderAngle);
	markup_dimline->SetAttribute("m_eExtension", sData.m_eExtension);

	if (sData.m_pExplicitCurve)
	{
		TiXmlElement* ExplicitCurve = new TiXmlElement("ExplicitCurve");
		markup_dimline->LinkEndChild(ExplicitCurve);
		// Commented out
		// traverseCurve(sData.m_pExplicitCurve, ExplicitCurve);
	}

	A3DMDDimensionLineGet(NULL, &sData);
}

//######################################################################################################################
int traverseMarkupDimension(const A3DMarkupDimension* pMarkup, TiXmlElement* setting, json* jsonData, int index, int pmiIndex)
{
	A3DMarkupDimensionData sMarkupData;
	A3D_INITIALIZE_DATA(A3DMarkupDimensionData, sMarkupData);

	TiXmlElement* markup_dim = new TiXmlElement("A3DMarkupDimensionData");
	setting->LinkEndChild(markup_dim);

	A3DStatus iRet = A3DMarkupDimensionGet(pMarkup, &sMarkupData);
	if(iRet != A3D_SUCCESS)
	{
		markup_dim->SetAttribute("error", iRet);
		return iRet;
	}

	sttraverseDimensionType(sMarkupData.m_eType, markup_dim, jsonData, index, pmiIndex);
	sttraverseDimensionValue(sMarkupData.m_pMainValue, markup_dim);

	if(sMarkupData.m_pDualValue)
	{
		TiXmlElement* markup_dualval = new TiXmlElement("m_pDualValue");
		markup_dim->LinkEndChild(markup_dualval);

		if(sMarkupData.m_eDualDisplay == KEA3DMDDimensionDualDisplayNone)
			markup_dualval->SetAttribute("m_eDualDisplay", "KEA3DMDDimensionDualDisplayNone");
		else if(sMarkupData.m_eDualDisplay == KEA3DMDDimensionDualDisplayBelow)
			markup_dualval->SetAttribute("m_eDualDisplay", "KEA3DMDDimensionDualDisplayBelow");
		else if(sMarkupData.m_eDualDisplay == KEA3DMDDimensionDualDisplayFractional)
			markup_dualval->SetAttribute("m_eDualDisplay", "KEA3DMDDimensionDualDisplayFractional");
		else if(sMarkupData.m_eDualDisplay == KEA3DMDDimensionDualDisplaySideBySide)
			markup_dualval->SetAttribute("m_eDualDisplay", "KEA3DMDDimensionDualDisplaySideBySide");
		else
			markup_dualval->SetAttribute("m_eDualDisplay", "unexpected");

		sttraverseDimensionValue(sMarkupData.m_pDualValue, markup_dualval);
	}

	markup_dim->SetDoubleAttribute("m_dValue", sMarkupData.m_dValue);
	markup_dim->SetAttribute("m_eSymbol", sMarkupData.m_eSymbol);
	markup_dim->SetAttribute("m_eAdditionnalSymbol", sMarkupData.m_eAdditionnalSymbol);
	markup_dim->SetAttribute("m_pcSuffixe", sMarkupData.m_pcSuffixe?sMarkupData.m_pcSuffixe : "null");
	markup_dim->SetAttribute("m_pcPrefixe", sMarkupData.m_pcPrefixe?sMarkupData.m_pcPrefixe : "null");
	if(sMarkupData.m_eType == KEA3DMDDimensionTypeChamfer)
		markup_dim->SetDoubleAttribute("m_dChamferDimSecondValue", sMarkupData.m_dChamferDimSecondValue);

	sttraverseDimensionExtentionLine(sMarkupData.m_pExtentionLine, markup_dim);
	sttraverseDimensionLine(sMarkupData.m_pLine, markup_dim);
	markup_dim->SetAttribute("m_eOrientation", sMarkupData.m_eOrientation);
	markup_dim->SetAttribute("m_usPosition", sMarkupData.m_usPosition);

	TiXmlElement* markup_frame = new TiXmlElement("Frame");
	markup_dim->LinkEndChild(markup_frame);
	markup_frame->SetAttribute("m_eFrame", sMarkupData.m_eFrame);
	markup_frame->SetAttribute("m_usFrame", sMarkupData.m_usFrame);
	markup_frame->SetAttribute("m_usFrameGroup", sMarkupData.m_usFrameGroup);

	TiXmlElement* markup_score = new TiXmlElement("Score");
	markup_dim->LinkEndChild(markup_score);
	markup_score->SetAttribute("m_eScore", sMarkupData.m_eScore);
	markup_score->SetAttribute("m_usScore", sMarkupData.m_usScore);
	markup_score->SetAttribute("m_usScoreGroup", sMarkupData.m_usScoreGroup);

	traverseMarkupTextProperties(sMarkupData.m_pTextProperties, markup_dim, jsonData, index, pmiIndex);

	return A3DMarkupDimensionGet(NULL, &sMarkupData);
}

//######################################################################################################################
int traverseToleranceSizeValue(const A3DMDToleranceSizeValue* pToleranceSizeValue, TiXmlElement* setting, json* jsonData, int index, int pmiIndex)
{
	A3DMDToleranceSizeValueData sToleranceSizeValueData;
	A3D_INITIALIZE_DATA(A3DMDToleranceSizeValueData, sToleranceSizeValueData);

	TiXmlElement* markup_ToleranceSizeValue = new TiXmlElement("A3DMDToleranceSizeValueData");
	setting->LinkEndChild(markup_ToleranceSizeValue);

	A3DStatus iRet = A3DMDToleranceSizeValueGet(pToleranceSizeValue, &sToleranceSizeValueData);
	if (iRet != A3D_SUCCESS)
	{
		markup_ToleranceSizeValue->SetAttribute("error", iRet);
		return iRet;
	}
	sttraverseDimensionType(sToleranceSizeValueData.m_eType, markup_ToleranceSizeValue, jsonData, index, pmiIndex);
	markup_ToleranceSizeValue->SetDoubleAttribute("m_dValue", sToleranceSizeValueData.m_dValue);

	markup_ToleranceSizeValue->SetAttribute("m_pcSeparator", sToleranceSizeValueData.m_pcSeparator ? sToleranceSizeValueData.m_pcSeparator : "null");

	TiXmlElement* MainValue = new TiXmlElement("A3DMDDimensionValue");
	markup_ToleranceSizeValue->LinkEndChild(MainValue);
	if (sToleranceSizeValueData.m_sMainValue.m_iType == 0)
		MainValue->SetAttribute("m_iType", "true value");
	else if (sToleranceSizeValueData.m_sMainValue.m_iType == 1)
		MainValue->SetAttribute("m_iType", "fake num value");
	else if (sToleranceSizeValueData.m_sMainValue.m_iType == 2)
		MainValue->SetAttribute("m_iType", "fake alpha num value");
	else
		MainValue->SetAttribute("m_iType", "unexpected");

	MainValue->SetAttribute("m_bFormat", sToleranceSizeValueData.m_sMainValue.m_bFormat ? "decimal format" : "fractional");
	MainValue->SetDoubleAttribute("m_dAccuracy", sToleranceSizeValueData.m_sMainValue.m_dAccuracy);
	MainValue->SetDoubleAttribute("m_dToleranceAccuracy", sToleranceSizeValueData.m_sMainValue.m_dToleranceAccuracy);
	MainValue->SetAttribute("m_pcFakeValue", sToleranceSizeValueData.m_sMainValue.m_pcFakeValue ? sToleranceSizeValueData.m_sMainValue.m_pcFakeValue : "null");
	MainValue->SetDoubleAttribute("m_dTolNumSup", sToleranceSizeValueData.m_sMainValue.m_dTolNumSup);
	MainValue->SetDoubleAttribute("m_dTolNumInf", sToleranceSizeValueData.m_sMainValue.m_dTolNumInf);
	MainValue->SetAttribute("m_pcTolTxtSup", sToleranceSizeValueData.m_sMainValue.m_pcTolTxtSup ? sToleranceSizeValueData.m_sMainValue.m_pcTolTxtSup : "null");
	MainValue->SetAttribute("m_pcTolTxtInf", sToleranceSizeValueData.m_sMainValue.m_pcTolTxtInf ? sToleranceSizeValueData.m_sMainValue.m_pcTolTxtInf : "null");
	MainValue->SetAttribute("m_pcBeforeText", sToleranceSizeValueData.m_sMainValue.m_pcBeforeText ? sToleranceSizeValueData.m_sMainValue.m_pcBeforeText : "null");
	MainValue->SetAttribute("m_pcAfterText", sToleranceSizeValueData.m_sMainValue.m_pcAfterText ? sToleranceSizeValueData.m_sMainValue.m_pcAfterText : "null");
	MainValue->SetAttribute("m_pcAboveText", sToleranceSizeValueData.m_sMainValue.m_pcAboveText ? sToleranceSizeValueData.m_sMainValue.m_pcAboveText : "null");
	MainValue->SetAttribute("m_pcBelowText", sToleranceSizeValueData.m_sMainValue.m_pcBelowText ? sToleranceSizeValueData.m_sMainValue.m_pcBelowText : "null");
	MainValue->SetAttribute("m_usLastSeparDefinedNum", sToleranceSizeValueData.m_sMainValue.m_usLastSeparDefinedNum);

	MainValue->SetAttribute("m_iOption", sToleranceSizeValueData.m_sMainValue.m_iOption);
	MainValue->SetDoubleAttribute("m_dDeltaForMinMax", sToleranceSizeValueData.m_sMainValue.m_dDeltaForMinMax);

	sttraverseDimensionValueFormat(sToleranceSizeValueData.m_sMainValue.m_pDimValueFormat, MainValue);
	sttraverseDimensionToleranceFormat(sToleranceSizeValueData.m_sMainValue.m_pToleranceFormat, MainValue);

	if (sToleranceSizeValueData.m_sMainValue.m_pChamferDimSecondValue)
		sttraverseDimensionValue(sToleranceSizeValueData.m_sMainValue.m_pChamferDimSecondValue, MainValue);

	sttraverseDimensionValue(sToleranceSizeValueData.m_pDualValue, MainValue);

	return A3DMDToleranceSizeValueGet(NULL, &sToleranceSizeValueData);
}