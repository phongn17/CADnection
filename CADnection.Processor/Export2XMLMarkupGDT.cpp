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
static void stvAddModifier(int iModifier, TiXmlElement* setting)
{
	switch(iModifier)
	{
	case KEA3DGDTModifierNone: setting->SetAttribute("m_eModifier", "none"); break;
	case KEA3DGDTModifierMax: setting->SetAttribute("m_eModifier", "maximum material condition"); break;
	case KEA3DGDTModifierMin: setting->SetAttribute("m_eModifier", "least material condition"); break;
	case KEA3DGDTModifierProj: setting->SetAttribute("m_eModifier", "projected tolerance zone"); break;
	case KEA3DGDTModifierFree: setting->SetAttribute("m_eModifier", "free state"); break;
	case KEA3DGDTModifierRfs: setting->SetAttribute("m_eModifier", "regardless of feature size"); break;
	case KEA3DGDTModifierTangent: setting->SetAttribute("m_eModifier", "tangent plane"); break;
	case KEA3DGDTModifierST: setting->SetAttribute("m_eModifier", "statical"); break;
	default: setting->SetAttribute("m_eModifier", "unexpected"); break;
	}
}

//######################################################################################################################
static void stvAddGDTType(int itype, TiXmlElement* setting)
{
	switch(itype)
	{
	case KEA3DGDTTypeNone: setting->SetAttribute("m_eType", "none"); break;
	case KEA3DGDTTypeStraightness: setting->SetAttribute("m_eType", "Straightness"); break;
	case KEA3DGDTTypeFlatness: setting->SetAttribute("m_eType", "Flatness"); break;
	case KEA3DGDTTypeCircularity: setting->SetAttribute("m_eType", "Circularity"); break;
	case KEA3DGDTTypeCylindricity: setting->SetAttribute("m_eType", "Cylindricity"); break;
	case KEA3DGDTTypeLineProfile: setting->SetAttribute("m_eType", "Profile of a line"); break;
	case KEA3DGDTTypeSurfaceProfile: setting->SetAttribute("m_eType", "Profile of a surface"); break;
	case KEA3DGDTTypeAngularity: setting->SetAttribute("m_eType", "Angularity"); break;
	case KEA3DGDTTypePerpendicularity: setting->SetAttribute("m_eType", "Perpendicularity"); break;
	case KEA3DGDTTypeParallelism: setting->SetAttribute("m_eType", "Parallel"); break;
	case KEA3DGDTTypePosition: setting->SetAttribute("m_eType", "Positional"); break;
	case KEA3DGDTTypeConcentricity: setting->SetAttribute("m_eType", "Concentricity"); break;
	case KEA3DGDTTypeSymmetry: setting->SetAttribute("m_eType", "Symmetric"); break;
	case KEA3DGDTTypeCircularRunout: setting->SetAttribute("m_eType", "Circular run out"); break;
	case KEA3DGDTTypeTotalRunout: setting->SetAttribute("m_eType", "Total run out"); break;
	default: setting->SetAttribute("m_eType", "unexpected"); break;
	}
}

//######################################################################################################################
static int sttraverseDraftingRow(const A3DMDFCFDraftingRow *psRow, TiXmlElement* setting)
{
	A3DMDFCFDraftingRowData sData;
	A3D_INITIALIZE_DATA(A3DMDFCFDraftingRowData, sData);

	TiXmlElement* drft_row = new TiXmlElement("A3DMDFCFDraftingRowData");
	setting->LinkEndChild(drft_row);

	A3DStatus iRet = A3DMDFCFDraftingRowGet(psRow, &sData);
	if(iRet != A3D_SUCCESS)
	{
		drft_row->SetAttribute("error", iRet);
		return iRet;
	}

	stvAddGDTType(sData.m_eType, drft_row);

	TiXmlElement* row_boxes = new TiXmlElement("A3DMDFCFDraftingRowData");
	drft_row->LinkEndChild(row_boxes);
	if(sData.m_uiNumberOfTextsInBoxes == 0)
		row_boxes->SetAttribute("error", "no box");

	A3DUns32 i;
	char sName[20];
	for(i = 0; i < sData.m_uiNumberOfTextsInBoxes; ++i)
	{
		sprintf(sName, "box_%u", i);
		if(sData.m_ppcTextsInBoxes[i] == NULL)
		{
			row_boxes->SetAttribute("error", "no text in box");
			continue;
		}
		row_boxes->SetAttribute(sName, sData.m_ppcTextsInBoxes[i]);
	}
	return A3DMDFCFDraftingRowGet(NULL, &sData);
}

//######################################################################################################################
static int sttraverseDatumReference(const char* pcTitle, A3DMDFCFRowDatum* psDatum, TiXmlElement* drw_row)
{
	TiXmlElement* datum_ref = new TiXmlElement(pcTitle);
	drw_row->LinkEndChild(datum_ref);

	A3DMDFCFRowDatumData sDatumData;
	A3D_INITIALIZE_DATA(A3DMDFCFRowDatumData, sDatumData);

	A3DStatus iRet = A3DMDFCFRowDatumGet(psDatum, &sDatumData);
	if(iRet != A3D_SUCCESS)
	{
		drw_row->SetAttribute("error", iRet);
		return A3D_SUCCESS;
	}

	datum_ref->SetAttribute("m_pcDatum", sDatumData.m_pcDatum?sDatumData.m_pcDatum : "null");
	stvAddModifier(sDatumData.m_eModifier, datum_ref);

	if(sDatumData.m_pLinkedItem)
		traverseLinkedItem(sDatumData.m_pLinkedItem, datum_ref);
	if(sDatumData.m_pNext)
		return sttraverseDatumReference("next", sDatumData.m_pNext, datum_ref);

		return A3D_SUCCESS;
}

//######################################################################################################################
static int sttraverseDrawingRow(const A3DMDFCFDrawingRowData& sData, TiXmlElement* drw_row)
{
	A3DStatus iRet;

	// row type
	stvAddGDTType(sData.m_eType, drw_row);

	// value
	if(sData.m_eValueType == KEA3DGDTValueNone)
		drw_row->SetAttribute("m_eValueType", "none");
	else if(sData.m_eValueType == KEA3DGDTValueDiameter)
		drw_row->SetAttribute("m_eValueType", "diameter");
	else if(sData.m_eValueType == KEA3DGDTValueRadius)
		drw_row->SetAttribute("m_eValueType", "radius");
	else if(sData.m_eValueType == KEA3DGDTValueSpherical)
		drw_row->SetAttribute("m_eValueType", "spherical");
	else
		drw_row->SetAttribute("m_eValueType", "unexpected");

	drw_row->SetAttribute("m_pcValue", sData.m_pcValue ? sData.m_pcValue : "null");

	// modifier
	stvAddModifier(sData.m_eModifier, drw_row);
	drw_row->SetAttribute("m_bFreeState", sData.m_bFreeState ? "true" : "false");
	drw_row->SetAttribute("m_bStatistical", sData.m_bStatistical ? "true" : "false");

	// references
	if(sData.m_psPrimaryRef)
		sttraverseDatumReference("m_psPrimaryRef", sData.m_psPrimaryRef, drw_row);
	if(sData.m_psSecondaryRef)
		sttraverseDatumReference("m_psSecondaryRef", sData.m_psSecondaryRef, drw_row);
	if(sData.m_psTertiaryRef)
		sttraverseDatumReference("m_psTertiaryRef", sData.m_psTertiaryRef, drw_row);

	if(sData.m_psSemanticValue == NULL)
	{
		return A3D_SUCCESS;
	}

	// semantic tolerance value
	TiXmlElement* semantic_value = new TiXmlElement("m_psSemanticValue");
	drw_row->LinkEndChild(semantic_value);

	A3DMDFCFToleranceValueData sSemanticData;
	A3D_INITIALIZE_DATA(A3DMDFCFToleranceValueData, sSemanticData);

	iRet = A3DMDFCFToleranceValueGet(sData.m_psSemanticValue, &sSemanticData);
	if(iRet != A3D_SUCCESS)
	{
		semantic_value->SetAttribute("error", iRet);
		return iRet;
	}

	// main value
	if(sSemanticData.m_sValue.m_bIsValue)
	{
		if(sSemanticData.m_sValue.m_pcValue == NULL)
			semantic_value->SetAttribute("m_sValue.m_pcValue", "NULL");
		else
			semantic_value->SetAttribute("m_sValue.m_pcValue", sSemanticData.m_sValue.m_pcValue);
	}
	else
		semantic_value->SetDoubleAttribute("m_sValue.m_dValue", sSemanticData.m_sValue.m_dValue);

	// tolerance per unit
	if(sSemanticData.m_psTolerancePerUnit)
	{
		TiXmlElement* tol_per_unit = new TiXmlElement("m_psTolerancePerUnit");
		semantic_value->LinkEndChild(tol_per_unit);

		A3DMDFCTolerancePerUnitData sTolerancePerUnitData;
		A3D_INITIALIZE_DATA(A3DMDFCTolerancePerUnitData, sTolerancePerUnitData);

		iRet = A3DMDFCTolerancePerUnitGet(sSemanticData.m_psTolerancePerUnit, &sTolerancePerUnitData);
		if(iRet != A3D_SUCCESS)
		{
			tol_per_unit->SetAttribute("error", iRet);
			return iRet;
		}

		tol_per_unit->SetDoubleAttribute("m_dUnitLengthOrAreaFirstValue", sTolerancePerUnitData.m_dUnitLengthOrAreaFirstValue);
		if(sTolerancePerUnitData.m_pdUnitAreaSecondLength)
			tol_per_unit->SetDoubleAttribute("m_pdUnitAreaSecondLength", *sTolerancePerUnitData.m_pdUnitAreaSecondLength);

		if(sTolerancePerUnitData.m_psRefinementZone)
		{
			TiXmlElement* tol_per_unit_ref = new TiXmlElement("m_psRefinementZone");
			tol_per_unit->LinkEndChild(tol_per_unit_ref);

			A3DMDFCValueData sValueData;
			A3D_INITIALIZE_DATA(A3DMDFCValueData, sValueData);

			iRet = A3DMDFCValueGet(sTolerancePerUnitData.m_psRefinementZone, &sValueData);
			if(iRet != A3D_SUCCESS)
			{
				tol_per_unit_ref->SetAttribute("refinement zone error", iRet);
				return iRet;
			}

			if(sValueData.m_bIsValue)
			{
				if(sValueData.m_pcValue == NULL)
					tol_per_unit_ref->SetAttribute("m_sValue.m_pcValue", "NULL");
				else
					tol_per_unit_ref->SetAttribute("m_sValue.m_pcValue", sValueData.m_pcValue);
			}
			else
				tol_per_unit_ref->SetDoubleAttribute("m_sValue.m_dValue", sValueData.m_dValue);

			CHECK_RET(A3DMDFCValueGet(NULL, &sValueData));
		}

		CHECK_RET(A3DMDFCTolerancePerUnitGet(NULL, &sTolerancePerUnitData));
	}

	// projected zone
	if(sSemanticData.m_psProjectedZone)
	{
		TiXmlElement* proj_zone = new TiXmlElement("m_psProjectedZone");
		semantic_value->LinkEndChild(proj_zone);

		A3DMDFCProjectedZoneData sProjectedZoneData;
		A3D_INITIALIZE_DATA(A3DMDFCProjectedZoneData, sProjectedZoneData);

		iRet = A3DMDFCProjectedZoneGet(sSemanticData.m_psProjectedZone, &sProjectedZoneData);
		if(iRet != A3D_SUCCESS)
		{
			proj_zone->SetAttribute("error", iRet);
			return iRet;
		}

		if(sProjectedZoneData.m_sLength.m_bIsValue)
		{
			if(sProjectedZoneData.m_sLength.m_pcValue == NULL)
				proj_zone->SetAttribute("m_sLength.m_pcValue", "NULL");
			else
				proj_zone->SetAttribute("m_sLength.m_pcValue", sProjectedZoneData.m_sLength.m_pcValue);
		}
		else
			proj_zone->SetDoubleAttribute("m_sLength.m_dValue", sProjectedZoneData.m_sLength.m_dValue);

		if(sProjectedZoneData.m_pdPosition)
			proj_zone->SetDoubleAttribute("m_pdPosition", *sProjectedZoneData.m_pdPosition);
		CHECK_RET(A3DMDFCProjectedZoneGet(NULL, &sProjectedZoneData));
	}

	// profile tolerance offset
	if(sSemanticData.m_psProfileTolerance)
	{
		TiXmlElement* prof_tolerance = new TiXmlElement("m_psProfileTolerance");
		semantic_value->LinkEndChild(prof_tolerance);

		A3DMDFCValueData sValueData;
		A3D_INITIALIZE_DATA(A3DMDFCValueData, sValueData);

		iRet = A3DMDFCValueGet(sSemanticData.m_psProfileTolerance, &sValueData);
		if(iRet != A3D_SUCCESS)
		{
			prof_tolerance->SetAttribute("profile tolerance error", iRet);
			return iRet;
		}

		if(sValueData.m_bIsValue)
		{
			if(sValueData.m_pcValue == NULL)
				prof_tolerance->SetAttribute("m_sValue.m_pcValue", "NULL");
			else
				prof_tolerance->SetAttribute("m_sValue.m_pcValue", sValueData.m_pcValue);
		}
		else
			prof_tolerance->SetDoubleAttribute("m_dValue", sValueData.m_dValue);

		CHECK_RET(A3DMDFCValueGet(NULL, &sValueData));
	}

	// maximum bonus
	if(sSemanticData.m_psMaximumBonus)
	{
		TiXmlElement* max_bonus = new TiXmlElement("m_psMaximumBonus");
		semantic_value->LinkEndChild(max_bonus);
		A3DMDFCValueData sValueData;
		A3D_INITIALIZE_DATA(A3DMDFCValueData, sValueData);
		iRet = A3DMDFCValueGet(sSemanticData.m_psMaximumBonus, &sValueData);
		if(iRet != A3D_SUCCESS)
		{
			max_bonus->SetAttribute("maximum bonnus error", iRet);
			return iRet;
		}
		if(sValueData.m_bIsValue)
		{
			if(sValueData.m_pcValue == NULL)
				max_bonus->SetAttribute("m_sValue.m_pcValue", "NULL");
			else
				max_bonus->SetAttribute("m_sValue.m_pcValue", sValueData.m_pcValue);
		}
		else
			max_bonus->SetDoubleAttribute("m_dValue", sValueData.m_dValue);

		CHECK_RET(A3DMDFCValueGet(NULL, &sValueData));
	}

	CHECK_RET(A3DMDFCFToleranceValueGet(NULL, &sSemanticData));
	return iRet;
}

//######################################################################################################################
static int sttraverseFeatureControlFrameData(const A3DMDFeatureControlFrameData& sFeatureControlFrameData,
											  TiXmlElement* setting)
{
	A3DUns32 r;
	A3DEEntityType eType = kA3DTypeUnknown;
	A3DStatus iRet;

	TiXmlElement* fcf = new TiXmlElement("A3DMDFeatureControlFrameData");
	setting->LinkEndChild(fcf);

	TiXmlElement* rows = new TiXmlElement("m_ppRows");
	fcf->LinkEndChild(rows);
	for(r = 0; r < sFeatureControlFrameData.m_uiNumberOfRows; ++r)
	{
		iRet = A3DEntityGetType(sFeatureControlFrameData.m_ppRows[r], &eType);
		if(iRet != A3D_SUCCESS)
		{
			rows->SetAttribute("error A3DEntityGetType", iRet);
			continue;
		}

		A3DMDFCFDrawingRowData sDrawingRowData;
		A3D_INITIALIZE_DATA(A3DMDFCFDrawingRowData, sDrawingRowData);

		iRet = A3DMDFCFSemanticRowGet(sFeatureControlFrameData.m_ppRows[r], &sDrawingRowData);
		if(iRet == A3D_SUCCESS)
		{
			TiXmlElement* drw_row = new TiXmlElement("A3DMDFCFDrawingRowData");
			rows->LinkEndChild(drw_row);
			sttraverseDrawingRow(sDrawingRowData, drw_row);
			A3DMDFCFDrawingRowGet(NULL, &sDrawingRowData);
		}
		else
		{
			if(eType != kA3DTypeMDFCFDraftingRow)
				rows->SetAttribute("error", "FCFRow");
			else
				sttraverseDraftingRow((const A3DMDFCFDraftingRow*) sFeatureControlFrameData.m_ppRows[r], rows);
		}
	}
	return A3D_SUCCESS;
}

//######################################################################################################################
static int sttraverseToleranceSize(const A3DMDToleranceSize* pToleranceSize, TiXmlElement* setting, json* jsonData, int index, int pmiIndex)
{
	A3DMDToleranceSizeData sToleranceSizeData;
	A3D_INITIALIZE_DATA(A3DMDToleranceSizeData, sToleranceSizeData);

	TiXmlElement* toleranceSize = new TiXmlElement("A3DMDToleranceSizeData");
	setting->LinkEndChild(toleranceSize);

	TiXmlElement* values = new TiXmlElement("m_ppTolSizeValue");
	toleranceSize->LinkEndChild(values);

	A3DStatus iRet = A3DMDToleranceSizeGet(pToleranceSize, &sToleranceSizeData);
	if (iRet != A3D_SUCCESS)
	{
		toleranceSize->SetAttribute("error", iRet);
		return iRet;
	}
	
	for (A3DUns32 i = 0; i < sToleranceSizeData.m_uiNumberOfSizeValues; ++i)
	{
		traverseToleranceSizeValue(sToleranceSizeData.m_ppTolSizeValue[i], toleranceSize, jsonData, index, pmiIndex);
	}
			
	return iRet;
}

//######################################################################################################################
int traverseMarkupGDT(const A3DMarkupGDT* psMarkup, TiXmlElement* setting, json* jsonData, int index, int pmiIndex)
{
	A3DMarkupGDTData sData;
	A3D_INITIALIZE_DATA(A3DMarkupGDTData, sData);

	TiXmlElement* markup_gdt = new TiXmlElement("A3DMarkupGDTData");
	setting->LinkEndChild(markup_gdt);

	A3DStatus iRet = A3DMarkupGDTGet(psMarkup, &sData);
	if(iRet != A3D_SUCCESS)
	{
		markup_gdt->SetAttribute("error", iRet);
		return iRet;
	}

	markup_gdt->SetAttribute("m_pcAboveText", sData.m_pcAboveText ? sData.m_pcAboveText : "null");
	markup_gdt->SetAttribute("m_pcBelowText", sData.m_pcBelowText ? sData.m_pcBelowText : "null");
	markup_gdt->SetAttribute("m_pcBeforeText", sData.m_pcBeforeText ? sData.m_pcBeforeText : "null");
	markup_gdt->SetAttribute("m_pcAfterText", sData.m_pcAfterText ? sData.m_pcAfterText : "null");

	if (pmiIndex != -1) {
		if (sData.m_pcAboveText)
			(*jsonData)["docs"][index]["PMI"][pmiIndex]["AboveText"] = sData.m_pcAboveText;
		if (sData.m_pcBelowText)
			(*jsonData)["docs"][index]["PMI"][pmiIndex]["BelowText"] = sData.m_pcBelowText;
		if (sData.m_pcBeforeText)
			(*jsonData)["docs"][index]["PMI"][pmiIndex]["BeforeText"] = sData.m_pcBeforeText;
		if (sData.m_pcAfterText)
			(*jsonData)["docs"][index]["PMI"][pmiIndex]["AfterText"] = sData.m_pcAfterText;
	}

	A3DUns32 i;
	char sName[20];
	A3DEEntityType eType = kA3DTypeUnknown;
	TiXmlElement* tolerance = NULL;
	for (i = 0; i < sData.m_uiNumberOfMarkupTolerances; ++i)
	{
		sprintf(sName, "A3DMDTolerance_%u", i);
		tolerance = new TiXmlElement(sName);
		markup_gdt->LinkEndChild(tolerance);
		iRet = A3DEntityGetType(sData.m_ppsMarkupTolerances[i], &eType);
		if (iRet != A3D_SUCCESS)
		{
			tolerance->SetAttribute("error", iRet);
			continue;
		}
		if (eType == kA3DTypeMDFeatureControlFrame)
		{
			// semantic data
			A3DMDFeatureControlFrameData sFCFData;
			A3D_INITIALIZE_DATA(A3DMDFeatureControlFrameData, sFCFData);

			iRet = A3DMDSemanticFeatureControlFrameGet(sData.m_ppsMarkupTolerances[i], &sFCFData);
			if (iRet == A3D_SUCCESS)
			{
				TiXmlElement* semantic_data = new TiXmlElement("SEMANTIC_DATA");
				tolerance->LinkEndChild(semantic_data);
				sttraverseFeatureControlFrameData(sFCFData, semantic_data);
				A3DMDSemanticFeatureControlFrameGet(NULL, &sFCFData);
			}
			else if (A3DMDFeatureControlFrameGet(sData.m_ppsMarkupTolerances[i], &sFCFData) == A3D_SUCCESS)
			{
				sttraverseFeatureControlFrameData(sFCFData, tolerance);
				A3DMDFeatureControlFrameGet(NULL, &sFCFData);
			}
			else
				tolerance->SetAttribute("error", iRet);
		}
		else if (eType == kA3DTypeMDToleranceSize)
		{
			sttraverseToleranceSize(sData.m_ppsMarkupTolerances[i], tolerance, jsonData, index, pmiIndex);
		}
		else
		{
			tolerance->SetAttribute("eType", "unexpected");
			continue;
		}
	}

	// text properties
	traverseMarkupTextProperties(sData.m_pTextProperties, setting, jsonData, index, pmiIndex);

	return A3DMarkupGDTGet(NULL, &sData);
}

//######################################################################################################################
int traverseMarkupDatum(const A3DMarkupDatum* psMarkup, TiXmlElement* setting, json* jsonData, int index, int pmiIndex)
{
	A3DMarkupDatumData sMarkupData;
	A3D_INITIALIZE_DATA(A3DMarkupDatumData, sMarkupData);
	TiXmlElement* markup_datum = new TiXmlElement("A3DMarkupDatumData");
	setting->LinkEndChild(markup_datum);
	A3DStatus iRet = A3DMarkupDatumGet(psMarkup, &sMarkupData);
	if(iRet != A3D_SUCCESS)
	{
		markup_datum->SetAttribute("error", iRet);
		return iRet;
	}
	markup_datum->SetAttribute("m_pcUpText", sMarkupData.m_pcUpText?sMarkupData.m_pcUpText:"NULL");
	markup_datum->SetAttribute("m_pcDownText", sMarkupData.m_pcDownText?sMarkupData.m_pcDownText:"NULL");

	markup_datum->SetAttribute("m_bTarget", sMarkupData.m_bTarget ? "true" : "false");
	markup_datum->SetAttribute("m_bDiameter", sMarkupData.m_bDiameter ? "true" : "false");
	markup_datum->SetAttribute("m_bIndividual", sMarkupData.m_bIndividual ? "true" : "false");

	if (pmiIndex != -1) {
		if (sMarkupData.m_pcUpText)
			(*jsonData)["docs"][index]["PMI"][pmiIndex]["UpText"] = sMarkupData.m_pcUpText;
		if (sMarkupData.m_pcDownText)
			(*jsonData)["docs"][index]["PMI"][pmiIndex]["DownText"] = sMarkupData.m_pcDownText;
		(*jsonData)["docs"][index]["PMI"][pmiIndex]["Target"] = sMarkupData.m_bTarget ? "true" : "false";
		(*jsonData)["docs"][index]["PMI"][pmiIndex]["Diameter"] = sMarkupData.m_bDiameter ? "true" : "false";
		(*jsonData)["docs"][index]["PMI"][pmiIndex]["Individual"] = sMarkupData.m_bIndividual ? "true" : "false";
	}

	traverseMarkupTextProperties(sMarkupData.m_pTextProperties, markup_datum, jsonData, index, pmiIndex);
	return A3DMarkupDatumGet(NULL, &sMarkupData);
}
