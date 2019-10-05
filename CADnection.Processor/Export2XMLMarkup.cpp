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
static int stTraverseAnnotationSet(const A3DMkpAnnotationSet* pAnnot, TiXmlElement* setting, json* jsonData, int index, int pmiIndex)
{
	A3DMkpAnnotationSetData sData;
	A3D_INITIALIZE_DATA(A3DMkpAnnotationSetData, sData);

	TiXmlElement* annotset = new TiXmlElement("A3DMkpAnnotationSetData");
	traverseSource(pAnnot, annotset, NULL, -1, -1);

	A3DInt32 iRet = A3DMkpAnnotationSetGet(pAnnot, &sData);
	if(iRet == A3D_SUCCESS)
	{
		for(A3DUns32 ui = 0; ui < sData.m_uiAnnotationsSize; ++ui)
			traverseAnnotation(sData.m_ppAnnotations[ui], annotset, jsonData, index, ui);

		A3DMkpAnnotationSetGet(NULL, &sData);
	}
	else
	{
		annotset->SetAttribute("error", iRet);
	}
	setting->LinkEndChild(annotset);
	return iRet;
}

//######################################################################################################################
static int stTraverseAnnotationReference(const A3DMkpAnnotationReference* pAnnot, TiXmlElement* setting, json* jsonData, int index)
{
	A3DMkpAnnotationReferenceData sData;
	A3D_INITIALIZE_DATA(A3DMkpAnnotationReferenceData, sData);

	TiXmlElement* ref = new TiXmlElement("A3DMkpAnnotationReferenceData");
	traverseSource(pAnnot, ref, NULL, -1, -1);

	A3DInt32 iRet = A3DMkpAnnotationReferenceGet(pAnnot, &sData);
	if(iRet == A3D_SUCCESS)
	{
		for(A3DUns32 ui = 0; ui < sData.m_uiLinkedItemsSize; ++ui)
			traverseLinkedItem(sData.m_ppLinkedItems[ui], ref); // => very very useful

		A3DMkpAnnotationReferenceGet(NULL, &sData);
	}
	else
	{
		ref->SetAttribute("error", iRet);
	}
	setting->LinkEndChild(ref);
	return iRet;
}

//######################################################################################################################
static int stTraverseAnnotationItem(const A3DMkpAnnotationItem* pAnnot, TiXmlElement* setting, json* jsonData, int index, int pmiIndex)
{
	A3DMkpAnnotationItemData sData;
	A3D_INITIALIZE_DATA(A3DMkpAnnotationItemData, sData);

	TiXmlElement* annotitem = new TiXmlElement("A3DMkpAnnotationItemData");
	traverseSource(pAnnot, annotitem, jsonData, index, -1, pmiIndex);

	A3DInt32 iRet = A3DMkpAnnotationItemGet(pAnnot, &sData);
	if(iRet == A3D_SUCCESS)
	{
		traverseMarkup(sData.m_pMarkup, annotitem, jsonData, index, pmiIndex);
		A3DMkpAnnotationItemGet(NULL, &sData);
	}
	else
	{
		annotitem->SetAttribute("error", iRet);
	}
	setting->LinkEndChild(annotitem);
	return iRet;
}

//######################################################################################################################
int traverseAnnotation(const A3DMkpAnnotationEntity* pAnnot, TiXmlElement* setting, json* jsonData, int index, int pmiIndex)
{
	A3DEEntityType eType;
	A3DInt32 iRet = A3DEntityGetType(pAnnot, &eType);
	if(iRet == A3D_SUCCESS)
	{
		switch(eType)
		{
		case kA3DTypeMkpAnnotationSet:
			return stTraverseAnnotationSet(pAnnot,setting, jsonData, index, pmiIndex);
		case kA3DTypeMkpAnnotationReference:
			return stTraverseAnnotationReference(pAnnot,setting, jsonData, index);
		case kA3DTypeMkpAnnotationItem:
			return stTraverseAnnotationItem(pAnnot,setting, jsonData, index, pmiIndex);
		default: 
			break;
		}
	}
	return A3D_NOT_IMPLEMENTED;
}

//######################################################################################################################
int traverseAmbientLight(const A3DGraphAmbientLight *pLight, TiXmlElement* setting)
{
	if (!pLight)
	{
		setting->SetAttribute("Light", "Bad cast");
		return -1;
	}
	TiXmlElement* light = new TiXmlElement("A3DGraphAmbientLightData");
	A3DStatus iRet;
	A3DGraphAmbientLightData oLight;
	A3D_INITIALIZE_DATA(A3DGraphAmbientLightData, oLight);
	if ((iRet = A3DGraphAmbientLightGet(pLight, &oLight)) == A3D_SUCCESS)
	{
		
		light->SetAttribute("m_uiAmbientColorIndex", oLight.m_uiAmbientColorIndex);		/*!< Ambient color index. */
		light->SetAttribute("m_uiDiffuseColorIndex", oLight.m_uiDiffuseColorIndex);		/*!< Diffuse color index. */
		light->SetAttribute("m_uiSpecularColorIndex", oLight.m_uiSpecularColorIndex);	/*!< Specular color index. */
		A3DGraphAmbientLightGet(NULL, &oLight);
	}
	else
	{
		light->SetAttribute("Error", iRet);
	}
	setting->LinkEndChild(light);
	return 0;
}

//######################################################################################################################
int traversePointLight(const A3DGraphPointLight* pLight, TiXmlElement* setting)
{
	if (!pLight)
	{
		setting->SetAttribute("Light", "Bad cast");
		return -1;
	}

	TiXmlElement* light = new TiXmlElement("A3DGraphPointLightData");
	A3DStatus iRet;
	A3DGraphPointLightData oLight;
	A3D_INITIALIZE_DATA(A3DGraphPointLightData, oLight);
	if ((iRet = A3DGraphPointLightGet(pLight, &oLight)) == A3D_SUCCESS)
	{
		light->SetAttribute("m_uiAmbientColorIndex", oLight.m_uiAmbientColorIndex);		/*!< Ambient color index. */
		light->SetAttribute("m_uiDiffuseColorIndex", oLight.m_uiDiffuseColorIndex);		/*!< Diffuse color index. */
		light->SetAttribute("m_uiSpecularColorIndex", oLight.m_uiSpecularColorIndex);	/*!< Specular color index. */
		traversePoint("m_sLocation", oLight.m_sLocation, light);			/*!< Location. \version 2.1 */
		light->SetDoubleAttribute("m_dConstantAttenuation", oLight.m_dConstantAttenuation);		/*!< Constant attenuation. \version 2.1 */
		light->SetDoubleAttribute("m_dLinearAttenuation", oLight.m_dLinearAttenuation);			/*!< Linear attenuation. \version 2.1 */
		light->SetDoubleAttribute("m_dQuadraticAttenuation", oLight.m_dQuadraticAttenuation);		/*!< Quadratic attenuation. \version 2.1 */
		A3DGraphPointLightGet(NULL, &oLight);
	}
	else
	{
		light->SetAttribute("Error", iRet);
	}
	setting->LinkEndChild(light);
	return 0;
}

//######################################################################################################################
int traverseSpotLight(const A3DGraphSpotLight* pLight, TiXmlElement* setting)
{
	if (!pLight)
	{
		setting->SetAttribute("Light", "Bad cast");
		return -1;
	}

	TiXmlElement* light = new TiXmlElement("A3DGraphSpotLightData");
	A3DStatus iRet;
	A3DGraphSpotLightData oLight;
	A3D_INITIALIZE_DATA(A3DGraphSpotLightData, oLight);
	if ((iRet = A3DGraphSpotLightGet(pLight, &oLight)) == A3D_SUCCESS)
	{
		light->SetAttribute("m_uiAmbientColorIndex", oLight.m_uiAmbientColorIndex);		/*!< Ambient color index. */
		light->SetAttribute("m_uiDiffuseColorIndex", oLight.m_uiDiffuseColorIndex);		/*!< Diffuse color index. */
		light->SetAttribute("m_uiSpecularColorIndex", oLight.m_uiSpecularColorIndex);	/*!< Specular color index. */
		traversePoint("m_sDirection", oLight.m_sDirection, light);		/*!< Direction. */
		light->SetDoubleAttribute("m_dFallOffAngle", oLight.m_dFallOffAngle);				/*!< Fall-off angle.*/
		light->SetDoubleAttribute("m_dFallOffExponent", oLight.m_dFallOffExponent);			/*!< Fall-off exponent. */
		traversePoint("m_sLocation", oLight.m_sLocation, light);			/*!< Location. \version 2.1 */
		light->SetDoubleAttribute("m_dConstantAttenuation", oLight.m_dConstantAttenuation);		/*!< Constant attenuation. \version 2.1 */
		light->SetDoubleAttribute("m_dLinearAttenuation", oLight.m_dLinearAttenuation);			/*!< Linear attenuation. \version 2.1 */
		light->SetDoubleAttribute("m_dQuadraticAttenuation", oLight.m_dQuadraticAttenuation);		/*!< Quadratic attenuation. \version 2.1 */
		A3DGraphSpotLightGet(NULL, &oLight);
	}
	else
	{
		light->SetAttribute("Error", iRet);
	}
	setting->LinkEndChild(light);
	return 0;
}

//######################################################################################################################
int traverseDirectionalLight(const A3DGraphDirectionalLight* pLight, TiXmlElement* setting)
{
	if (!pLight)
	{
		setting->SetAttribute("Light", "Bad cast");
		return -1;
	}

	TiXmlElement* light = new TiXmlElement("A3DGraphDirectionalLightData");
	A3DStatus iRet;
	A3DGraphDirectionalLightData oLight;
	A3D_INITIALIZE_DATA(A3DGraphDirectionalLightData, oLight);
	if ((iRet = A3DGraphDirectionalLightGet(pLight, &oLight)) == A3D_SUCCESS)
	{
		light->SetAttribute("m_uiAmbientColorIndex", oLight.m_uiAmbientColorIndex);		/*!< Ambient color index. */
		light->SetAttribute("m_uiDiffuseColorIndex", oLight.m_uiDiffuseColorIndex);		/*!< Diffuse color index. */
		light->SetAttribute("m_uiSpecularColorIndex", oLight.m_uiSpecularColorIndex);	/*!< Specular color index. */
		light->SetDoubleAttribute("m_dIntensity", oLight.m_dIntensity);				/*!< Density. */
		traversePoint("m_sDirection", oLight.m_sDirection, light);		/*!< Direction. */
		A3DGraphDirectionalLightGet(NULL, &oLight);
	}
	else
	{
		light->SetAttribute("Error", iRet);
	}
	setting->LinkEndChild(light);
	return 0;
}

//######################################################################################################################
int traverseLight(const A3DGraphLight* pLight, TiXmlElement* setting)
{
	
	if (!pLight)
	{
		setting->SetAttribute("Light", "empty");
		return -1;
	}
	
	A3DEEntityType eType = kA3DTypeUnknown;
	A3DEntityGetType(pLight, &eType);
	switch (eType)
	{
	case kA3DTypeGraphAmbientLight:
		traverseAmbientLight(pLight, setting);
		break;
	case kA3DTypeGraphPointLight:
		traversePointLight(pLight, setting);
		break;
	case kA3DTypeGraphSpotLight:
		traverseSpotLight(pLight, setting);
		break;
	case kA3DTypeGraphDirectionalLight:
		traverseDirectionalLight(pLight, setting);
		break;
	default:
		setting->SetAttribute("Light", "Unknown Type");
		break;
	}
	return 0;
}

//######################################################################################################################
int traverseCamera(const A3DGraphCamera* pCamera, TiXmlElement* setting)
{
	TiXmlElement* camera = new TiXmlElement("A3DGraphCameraData");
	if (pCamera)
	{
		traverseSource(pCamera, camera, NULL, -1, -1);
		A3DGraphCameraData oData;
		A3D_INITIALIZE_DATA(A3DGraphCameraData, oData);
		A3DStatus iRet = A3DGraphCameraGet(pCamera, &oData);

		if (iRet == A3D_SUCCESS)
		{
			camera->SetAttribute("m_bOrthographic", oData.m_bOrthographic ? "true" : "false");
			traversePoint("m_sLocation", oData.m_sLocation, camera);
			traversePoint("m_sLookAt", oData.m_sLookAt, camera);
			traversePoint("m_sUp", oData.m_sUp, camera);
			camera->SetDoubleAttribute("m_dXFovy", oData.m_dXFovy);
			camera->SetDoubleAttribute("m_dYFovy", oData.m_dYFovy);
			camera->SetDoubleAttribute("m_dAspectRatio", oData.m_dAspectRatio);
			camera->SetDoubleAttribute("m_dZNear", oData.m_dZNear);
			camera->SetDoubleAttribute("m_dZFar", oData.m_dZFar);
			camera->SetDoubleAttribute("m_dZoomFactor", oData.m_dZoomFactor);
		}
		else
		{
			camera->SetAttribute("Error", iRet);
		}
		A3DGraphCameraGet(NULL, &oData);
	}
	setting->LinkEndChild(camera);
	return A3D_SUCCESS;
}

//######################################################################################################################
int traverseGraphScene(const A3DGraphSceneDisplayParametersData& sData, TiXmlElement* setting)
{
	A3DInt32 iRet = A3D_SUCCESS;
	TiXmlElement* graphscene = new TiXmlElement("A3DGraphSceneDisplayParametersData");
	graphscene->SetAttribute("NumberOfPlanes", sData.m_uiPlaneSize);

	for (A3DUns32 ui = 0; ui < sData.m_uiPlaneSize; ui++)
		traverseSrfPlane(sData.m_ppClippingPlanes[ui], graphscene);

	graphscene->SetAttribute("m_bActive", sData.m_bIsActive);
	traverseCamera(sData.m_pCamera, graphscene);

	graphscene->SetAttribute("NumberOfLights", sData.m_uiLightSize);
	for (A3DUns32 ui = 0; ui < sData.m_uiLightSize; ui++)
		traverseLight(sData.m_ppLights[ui], graphscene);

	graphscene->SetAttribute("m_bHasRotationCenter", sData.m_bHasRotationCenter ? "true" : "false");
	if (sData.m_bHasRotationCenter)
		traversePoint("m_sRotationCenter", sData.m_sRotationCenter, graphscene);

	graphscene->SetAttribute("m_uiBackgroundStyleIndex", sData.m_uiBackgroundStyleIndex);
	graphscene->SetAttribute("m_uiDefaultStyleIndex", sData.m_uiDefaultStyleIndex);

	A3DUns32 i, iMax = sData.m_uiDefaultPerTypeIndexSize;
	graphscene->SetAttribute("NumberOfDefaultStyleIndexesPerType", iMax);
	for (i = 0; i < iMax; i++)
	{
		graphscene->SetAttribute("TypesOfDefaultStyleIndexes", sData.m_puiTypesOfDefaultStyleIndexes[i]);
		graphscene->SetAttribute("DefaultStyleIndexesPerType", sData.m_puiDefaultStyleIndexesPerType[i]);
	}

	setting->LinkEndChild(graphscene);
	return iRet;
}

//######################################################################################################################
int traverseGraphScene(const A3DGraphSceneDisplayParameters * pGraphSceneParam, TiXmlElement* setting)
{
	A3DGraphSceneDisplayParametersData sData;
	A3D_INITIALIZE_DATA(A3DGraphSceneDisplayParametersData, sData);
	A3DInt32 iRet = A3DGraphSceneDisplayParametersGet(pGraphSceneParam, &sData);

	traverseGraphScene(sData, setting);

	A3DGraphSceneDisplayParametersGet(NULL, &sData);
	return iRet;
}

//######################################################################################################################
int traverseDisplayFilter(A3DAsmFilter* pFilter, TiXmlElement* setting)
{
	A3DInt32 iRet = A3D_SUCCESS;
	TiXmlElement* filter = new TiXmlElement("A3DAsmFilterData");
	setting->LinkEndChild(filter);
	if (!pFilter)
	{
		filter->SetAttribute("Filter", "empty");
		return -1;
	}

	A3DAsmFilterData oData;
	A3D_INITIALIZE_DATA(A3DAsmFilterData, oData);
	CHECK_RET(A3DAsmFilterGet(pFilter, &oData));

	filter->SetAttribute("m_bIsActive", oData.m_bIsActive);
	
	filter->SetAttribute("m_sLayerFilterItem.m_bIsInclusive", oData.m_sLayerFilterItem.m_bIsInclusive);

	A3DUns32 i, iMax = oData.m_sLayerFilterItem.m_uiSize;
	for (i = 0; i < iMax; i++)
		filter->SetAttribute("m_sLayerFilterItem.m_puiLayerIndexes", oData.m_sLayerFilterItem.m_puiLayerIndexes[i]);
		
	filter->SetAttribute("m_sEntityFilterItem.m_bIsInclusive", oData.m_sEntityFilterItem.m_bIsInclusive);
	iMax = oData.m_sEntityFilterItem.m_uiSize;
	for (i = 0; i < iMax; i++)
		traverseEntityReference(oData.m_sEntityFilterItem.m_ppEntities[i], filter);

	A3DAsmFilterGet(NULL, &oData);
	return iRet;
}

//######################################################################################################################
int traverseDisplayFilters(A3DUns32 uiSize,A3DAsmFilter** ppFilters , TiXmlElement* setting)
{
	for (A3DUns32 uk = 0; uk < uiSize; ++uk)
		traverseDisplayFilter(ppFilters[uk], setting);
	return 0;
}

//######################################################################################################################
int traverseView(const A3DMkpView* pView, TiXmlElement* setting)
{
	A3DMkpViewData sData;
	A3D_INITIALIZE_DATA(A3DMkpViewData, sData);

	TiXmlElement* view = new TiXmlElement("A3DMkpViewData");
	traverseSource(pView, view, NULL, -1, -1);

	A3DInt32 iRet = A3DMkpViewGet(pView, &sData);
	if(iRet == A3D_SUCCESS)
	{
		view->SetAttribute("m_bIsAnnotationView", sData.m_bIsAnnotationView);
		view->SetAttribute("m_bIsDefaultView", sData.m_bIsDefaultView);
		traverseVoids("m_ppAnnotations", sData.m_uiAnnotationsSize, (const A3DVoid**)sData.m_ppAnnotations, view);
		traverseSrfPlane(sData.m_pPlane, view);
		traverseGraphScene(sData.m_pSceneDisplayParameters, view);

		TiXmlElement* displayfilters = new TiXmlElement("DisplayFilters");
		traverseDisplayFilters(sData.m_uiDisplayFilterSize, sData.m_ppDisplayFilters, displayfilters);
		view->LinkEndChild(displayfilters);

		TiXmlElement* markupref = new TiXmlElement("LinkedItems");
		for (A3DUns32 uk = 0; uk < sData.m_uiLinkedItemsSize; ++uk)
			traverseLinkedItem(sData.m_ppLinkedItems[uk], markupref);
		view->LinkEndChild(markupref);

		A3DMkpViewGet(NULL, &sData);
	}
	setting->LinkEndChild(view);

	return A3D_SUCCESS;
}

//######################################################################################################################
#define EnumValueToXmlAttribute(ENUM_VAL, ATT_TYPE, ATT_VALUE) \
	case ENUM_VAL: \
	setting->SetAttribute(ATT_TYPE, ATT_VALUE); \
	break;

//######################################################################################################################
static void stTraverseMarkupTypeAndSubType(const A3DEMarkupType& eType, const A3DEMarkupSubType& eSubType, TiXmlElement* setting)
{
	switch(eType)
	{
		EnumValueToXmlAttribute(kA3DMarkupTypeUnknown, "m_eType", "Unknown");
		EnumValueToXmlAttribute(kA3DMarkupTypeText, "m_eType", "Plain text");
		EnumValueToXmlAttribute(kA3DMarkupTypeDimension, "m_eType", "Dimension");
		EnumValueToXmlAttribute(kA3DMarkupTypeArrow, "m_eType", "Arrow");
		EnumValueToXmlAttribute(kA3DMarkupTypeBalloon, "m_eType", "Balloon");
		EnumValueToXmlAttribute(kA3DMarkupTypeCircleCenter, "m_eType", "Center of circle");
		EnumValueToXmlAttribute(kA3DMarkupTypeCoordinate, "m_eType", "Coordinate");
		EnumValueToXmlAttribute(kA3DMarkupTypeDatum, "m_eType", "Datum");
		EnumValueToXmlAttribute(kA3DMarkupTypeFastener, "m_eType", "Fastener");
		EnumValueToXmlAttribute(kA3DMarkupTypeGdt, "m_eType", "GDT");
		EnumValueToXmlAttribute(kA3DMarkupTypeLocator, "m_eType", "Locator");
		EnumValueToXmlAttribute(kA3DMarkupTypeMeasurementPoint, "m_eType", "Point");
		EnumValueToXmlAttribute(kA3DMarkupTypeRoughness, "m_eType", "Roughness");
		EnumValueToXmlAttribute(kA3DMarkupTypeWelding, "m_eType", "Welding");
		EnumValueToXmlAttribute(kA3DMarkupTypeTable, "m_eType", "Table");
		EnumValueToXmlAttribute(kA3DMarkupTypeOther, "m_eType", "Other");
	default:
		setting->SetAttribute("m_eType", "Unexpected");
		break;
	}

	if(eType == kA3DMarkupTypeDatum)
	{
		switch(eSubType)
		{
			EnumValueToXmlAttribute(kA3DMarkupSubTypeDatumIdent, "m_eSubType", "Datum subtype");
			EnumValueToXmlAttribute(kA3DMarkupSubTypeDatumTarget, "m_eSubType", "Datum subtype");
			EnumValueToXmlAttribute(kA3DMarkupSubTypeDatumEnumMax, "m_eSubType", "Datum max value. \version 2.2");
		default:
			setting->SetAttribute("Datum_m_eSubType", "Unexpected");
			break;
		}
	}
	else if(eType == kA3DMarkupTypeDimension)
	{
		switch(eSubType)
		{
			EnumValueToXmlAttribute(kA3DMarkupSubTypeDimensionDistance, "m_eSubType", "Dimension distance subtype");
			EnumValueToXmlAttribute(kA3DMarkupSubTypeDimensionDistanceOffset, "m_eSubType", "Dimension distance subtype");
			EnumValueToXmlAttribute(kA3DMarkupSubTypeDimensionDistanceCumulate, "m_eSubType", "Dimension distance subtype");
			EnumValueToXmlAttribute(kA3DMarkupSubTypeDimensionChamfer, "m_eSubType", "Dimension chamfer subtype");
			EnumValueToXmlAttribute(kA3DMarkupSubTypeDimensionSlope, "m_eSubType", "Dimension slope subtype");
			EnumValueToXmlAttribute(kA3DMarkupSubTypeDimensionOrdinate, "m_eSubType", "Dimension ordinate subtype");
			EnumValueToXmlAttribute(kA3DMarkupSubTypeDimensionRadius, "m_eSubType", "Dimension radius subtype");
			EnumValueToXmlAttribute(kA3DMarkupSubTypeDimensionRadiusTangent, "m_eSubType", "Dimension radius subtype");
			EnumValueToXmlAttribute(kA3DMarkupSubTypeDimensionRadiusCylinder, "m_eSubType", "Dimension radius subtype");
			EnumValueToXmlAttribute(kA3DMarkupSubTypeDimensionRadiusEdge, "m_eSubType", "Dimension radius subtype");
			EnumValueToXmlAttribute(kA3DMarkupSubTypeDimensionDiameter, "m_eSubType", "Dimension diameter subtype");
			EnumValueToXmlAttribute(kA3DMarkupSubTypeDimensionDiameterTangent, "m_eSubType", "Dimension diameter subtype");
			EnumValueToXmlAttribute(kA3DMarkupSubTypeDimensionDiameterCylinder, "m_eSubType", "Dimension diameter subtype");
			EnumValueToXmlAttribute(kA3DMarkupSubTypeDimensionDiameterEdge, "m_eSubType", "Dimension diameter subtype");
			EnumValueToXmlAttribute(kA3DMarkupSubTypeDimensionDiameterCone, "m_eSubType", "Dimension diameter subtype");
			EnumValueToXmlAttribute(kA3DMarkupSubTypeDimensionLength, "m_eSubType", "Dimension length subtype");
			EnumValueToXmlAttribute(kA3DMarkupSubTypeDimensionLengthCurvilinear, "m_eSubType", "Dimension length subtype");
			EnumValueToXmlAttribute(kA3DMarkupSubTypeDimensionLengthCircular, "m_eSubType", "Dimension length subtype");
			EnumValueToXmlAttribute(kA3DMarkupSubTypeDimensionAngle, "m_eSubType", "Dimension angle subtype");
			EnumValueToXmlAttribute(kA3DMarkupSubTypeDimensionEnumMax, "m_eSubType", "Dimension max value. \version 2.2");
		default:
			setting->SetAttribute("Dimension_m_eSubType", "Unexpected");
			break;
		}
	}
	else if(eType == kA3DMarkupTypeGdt)
	{
		switch(eSubType)
		{
			EnumValueToXmlAttribute(kA3DMarkupSubTypeGdtFcf, "m_eSubType", "GDT feature control frame subtype");
			EnumValueToXmlAttribute(kA3DMarkupSubTypeGdtEnumMax, "m_eSubType", "GDT max value. \version 2.2");
		default:
			setting->SetAttribute("Gdt_m_eSubType", "Unexpected");
			break;
		}
	}
	else if(eType == kA3DMarkupTypeWelding)
	{
		switch(eSubType)
		{
			EnumValueToXmlAttribute(kA3DMarkupSubTypeWeldingLine, "m_eSubType", "Line welding subtype");
			EnumValueToXmlAttribute(kA3DMarkupSubTypeWeldingSpot, "m_eSubType", "Spot welding subtype");
			EnumValueToXmlAttribute(kA3DMarkupSubTypeWeldingEnumMax, "m_eSubType", "welding max value. \version 2.2");
		default:
			setting->SetAttribute("welding_m_eSubType", "Unexpected");
			break;
		}
	}
	else if(eType == kA3DMarkupTypeOther)
	{
		switch(eSubType)
		{
			EnumValueToXmlAttribute(kA3DMarkupSubTypeOtherSymbolUser, "m_eSubType", "SymbolUser other subtype");
			EnumValueToXmlAttribute(kA3DMarkupSubTypeOtherSymbolUtility, "m_eSubType", "SymbolUtility other subtype");
			EnumValueToXmlAttribute(kA3DMarkupSubTypeOtherSymbolCustom, "m_eSubType", "SymbolCustom other subtype");
			EnumValueToXmlAttribute(kA3DMarkupSubTypeOtherGeometricReference, "m_eSubType", "GeometricReference other subtype");
			EnumValueToXmlAttribute(kA3DMarkupSubTypeOtherRegion, "m_eSubType", "Region");
		default:
			setting->SetAttribute("other_symbol_m_eSubType", "Unexpected");
			break;
		}
	}
}

//######################################################################################################################
int traverseMarkup(const A3DMkpMarkup* pMarkup, TiXmlElement* setting, json* jsonData, int index, int pmiIndex)
{
	A3DMkpMarkupData sData;
	A3D_INITIALIZE_DATA(A3DMkpMarkupData, sData);

	TiXmlElement* markup = new TiXmlElement("A3DMkpMarkupData");
	traverseSource(pMarkup, markup, jsonData, index, -1, pmiIndex);

	A3DInt32 iRet = A3DMkpMarkupGet(pMarkup, &sData);
	A3DRootBaseData sBaseData;
	A3D_INITIALIZE_DATA(A3DRootBaseData, sBaseData);
	iRet = A3DRootBaseGet(pMarkup, &sBaseData);
	iRet = A3DRootBaseGet(NULL, &sBaseData);
	if(iRet == A3D_SUCCESS)
	{
		stTraverseMarkupTypeAndSubType(sData.m_eType, sData.m_eSubType, markup);
		traverseMarkupDefinition(pMarkup, markup, jsonData, index, pmiIndex);

		TiXmlElement* leaders = new TiXmlElement("Leaders");
		leaders->SetAttribute("size", (int)sData.m_uiLeadersSize);
		for(A3DUns32 ui = 0; ui < sData.m_uiLeadersSize; ++ui)
			traverseLeader(sData.m_ppLeaders[ui], leaders);
		markup->LinkEndChild(leaders);

		TiXmlElement* linkedItems = new TiXmlElement("LinkedItems");
		linkedItems->SetAttribute("size", (int)sData.m_uiLinkedItemsSize);
		for(A3DUns32 uj = 0; uj < sData.m_uiLinkedItemsSize; ++uj)
			traverseLinkedItem(sData.m_ppLinkedItems[uj], linkedItems);
		markup->LinkEndChild(linkedItems);

        // parse the references
        A3DUns32 uiLinkedItemsSize = 0;
        A3DMiscMarkupLinkedItem** ppLinkedItems = NULL;
        iRet = A3DMkpLinkForMarkupReferenceGet(pMarkup, &uiLinkedItemsSize, &ppLinkedItems);
        if (iRet == A3D_SUCCESS && ppLinkedItems)
        {
            TiXmlElement* markupref = new TiXmlElement("LinksForMarkupReference");
            linkedItems->SetAttribute("size", (int) uiLinkedItemsSize);
            for (A3DUns32 uk = 0; uk < uiLinkedItemsSize; ++uk)
                traverseLinkedItem(ppLinkedItems[uk], markupref);
			markup->LinkEndChild(markupref);
			A3DMkpLinkForMarkupReferenceGet(NULL, &uiLinkedItemsSize, &ppLinkedItems);
        }

        // parse the additional references
        A3DUns32 uiAdditionalLinkedItemsSize = 0;
        A3DMiscMarkupLinkedItem** ppAdditionnalLinkedItems = NULL;
        iRet = A3DMkpLinkForAdditionalMarkupReferenceGet(pMarkup, &uiAdditionalLinkedItemsSize, &ppAdditionnalLinkedItems);
        if (iRet == A3D_SUCCESS && ppAdditionnalLinkedItems)
        {
            TiXmlElement* additionalMarkupref = new TiXmlElement("LinksForAdditionalMarkupReference");
            linkedItems->SetAttribute("size", (int) uiAdditionalLinkedItemsSize);
            for (A3DUns32 uk = 0; uk < uiAdditionalLinkedItemsSize; ++uk)
                traverseLinkedItem(ppAdditionnalLinkedItems[uk], additionalMarkupref);
			markup->LinkEndChild(additionalMarkupref);
			A3DMkpLinkForAdditionalMarkupReferenceGet(NULL, &uiAdditionalLinkedItemsSize, &ppAdditionnalLinkedItems);
        }

		// Commented out
		// traverseTessBase(sData.m_pTessellation, markup);

		A3DMkpMarkupGet(NULL, &sData);
	}
	else
		markup->SetAttribute("error", iRet);

	setting->LinkEndChild(markup);

	return iRet;
}

//######################################################################################################################
void setLeaderSymbolType(TiXmlElement* setting, const char* name, const A3DMDLeaderSymbolType value)
{
	switch(value)
	{
		EnumValueToXmlAttribute(KA3DMDLeaderSymbolNotUsed,											name, "Not used");
		EnumValueToXmlAttribute(KA3DMDLeaderSymbolCross,											name, "Cross");
		EnumValueToXmlAttribute(KA3DMDLeaderSymbolPlus,												name, "Plus");
		EnumValueToXmlAttribute(KA3DMDLeaderSymbolConcentric,										name, "Concentric");
		EnumValueToXmlAttribute(KA3DMDLeaderSymbolCoincident,										name, "Coincident");
		EnumValueToXmlAttribute(KA3DMDLeaderSymbolFullCircle,										name, "FullCircle");
		EnumValueToXmlAttribute(KA3DMDLeaderSymbolFullSquare,										name, "FullSquare");
		EnumValueToXmlAttribute(KA3DMDLeaderSymbolStar,												name, "Star");
		EnumValueToXmlAttribute(KA3DMDLeaderSymbolDot,												name, "Dot");
		EnumValueToXmlAttribute(KA3DMDLeaderSymbolSmallDot,											name, "SmallDot");
		EnumValueToXmlAttribute(KA3DMDLeaderSymbolMisc1,											name, "Misc1");
		EnumValueToXmlAttribute(KA3DMDLeaderSymbolMisc2,											name, "Misc2");
		EnumValueToXmlAttribute(KA3DMDLeaderSymbolFullCircle2,										name, "FullCircle2");
		EnumValueToXmlAttribute(KA3DMDLeaderSymbolFullSquare2,										name, "FullSquare2");
		EnumValueToXmlAttribute(KA3DMDLeaderSymbolOpenArrow,										name, "OpenArrow");
		EnumValueToXmlAttribute(KA3DMDLeaderSymbolUnfilledArrow,									name, "UnfilledArrow");
		EnumValueToXmlAttribute(KA3DMDLeaderSymbolBlankedArrow,										name, "BlankedArrow");
		EnumValueToXmlAttribute(KA3DMDLeaderSymbolFilledArrow,										name, "FilledArrow");
		EnumValueToXmlAttribute(KA3DMDLeaderSymbolUnfilledCircle,									name, "UnfilledCircle");
		EnumValueToXmlAttribute(KA3DMDLeaderSymbolBlankedCircle,									name, "BlankedCircle");
		EnumValueToXmlAttribute(KA3DMDLeaderSymbolFilledCircle,										name, "FilledCircle");
		EnumValueToXmlAttribute(KA3DMDLeaderSymbolCrossedCircle,									name, "CrossedCircle");
		EnumValueToXmlAttribute(KA3DMDLeaderSymbolBlankedSquare,									name, "BlankedSquare");
		EnumValueToXmlAttribute(KA3DMDLeaderSymbolFilledSquare,										name, "FilledSquare");
		EnumValueToXmlAttribute(KA3DMDLeaderSymbolBlankedTriangle,									name, "BlankedTriangle");
		EnumValueToXmlAttribute(KA3DMDLeaderSymbolFilledTriangle,									name, "FilledTriangle");
		EnumValueToXmlAttribute(KA3DMDLeaderSymbolManipulatorSquare,								name, "ManipulatorSquare");
		EnumValueToXmlAttribute(KA3DMDLeaderSymbolMamipulatorDiamond,								name, "MamipulatorDiamond");
		EnumValueToXmlAttribute(KA3DMDLeaderSymbolManipulatorCircle,								name, "ManipulatorCircle");
		EnumValueToXmlAttribute(KA3DMDLeaderSymbolManipulatorTriangle,								name, "ManipulatorTriangle");
		EnumValueToXmlAttribute(KA3DMDLeaderSymbolDoubleOpenArrow,									name, "DoubleOpenArrow");
		EnumValueToXmlAttribute(KA3DMDLeaderSymbolWave,												name, "Wave");
		EnumValueToXmlAttribute(KA3DMDLeaderSymbolSegment,											name, "Segment");
		EnumValueToXmlAttribute(KA3DMDLeaderSymbolDoubleFilledArrow,								name, "DoubleFilledArrow");
		EnumValueToXmlAttribute(KA3DMDLeaderSymbolDoubleClosedArrow,								name, "DoubleClosedArrow");
		EnumValueToXmlAttribute(KA3DMDLeaderSymbolHalfOpenArrowUp,									name, "HalfOpenArrowUp");
		EnumValueToXmlAttribute(KA3DMDLeaderSymbolHalfOpenArrowDown,								name, "HalfOpenArrowDown");
		EnumValueToXmlAttribute(KA3DMDLeaderSymbolHalfFilledArrowUp,								name, "HalfFilledArrowUp");
		EnumValueToXmlAttribute(KA3DMDLeaderSymbolHalfFilledArrowDown,								name, "HalfFilledArrowDown");
		EnumValueToXmlAttribute(KA3DMDLeaderSymbolSlash,											name, "Slash");
		EnumValueToXmlAttribute(KA3DMDLeaderSymbolDoubleBlankedArrow,								name, "DoubleBlankedArrow");
		EnumValueToXmlAttribute(KA3DMDLeaderSymbolIntegral,											name, "Integral");
		EnumValueToXmlAttribute(KA3DMDLeaderSymbolZoneGlobalAllAround,								name, "ZoneGlobalAllAround");
		EnumValueToXmlAttribute(KA3DMDLeaderSymbolZonePartialAllAround,								name, "ZonePartialAllAround");
		EnumValueToXmlAttribute(KA3DMDLeaderSymbolZoneGlobalAllAboutWithHorizontalAxisIndicator,	name, "ZoneGlobalAllAboutWithHorizontalAxisIndicator");
		EnumValueToXmlAttribute(KA3DMDLeaderSymbolZoneGlobalAllAboutWithVerticalAxisIndicator,		name, "ZoneGlobalAllAboutWithVerticalAxisIndicator");
		EnumValueToXmlAttribute(KA3DMDLeaderSymbolZonePartialAllAboutWithHorizontalAxisIndicator,	name, "ZonePartialAllAboutWithHorizontalAxisIndicator");
		EnumValueToXmlAttribute(KA3DMDLeaderSymbolZonePartialAllAboutWithVerticalAxisIndicator,		name, "ZonePartialAllAboutWithVerticalAxisIndicator");
		EnumValueToXmlAttribute(KA3DMDLeaderSymbolZoneGlobalAllOver,								name, "ZoneGlobalAllOver");
		EnumValueToXmlAttribute(KA3DMDLeaderSymbolZonePartialAllOver,								name, "ZonePartialAllOver");
	default:
		setting->SetAttribute(name, "Unexpected");
		break;
	}
}

//######################################################################################################################
int stTraverseLeaderSymbol(const A3DMDLeaderSymbol* pSymbol, TiXmlElement* setting)
{
	A3DMDLeaderSymbolData sData;
	A3D_INITIALIZE_DATA(A3DMDLeaderSymbolData, sData);

	TiXmlElement* symbol = new TiXmlElement("A3DMDLeaderSymbolData");

	A3DInt32 iRet = A3DMDLeaderSymbolGet(pSymbol, &sData);
	if(iRet == A3D_SUCCESS)
	{
		symbol->SetDoubleAttribute("m_dAdditionalParameter", sData.m_dAdditionalParameter);
		symbol->SetDoubleAttribute("m_dLength"             , sData.m_dLength);
		setLeaderSymbolType(symbol, "m_eHeadSymbol", sData.m_eHeadSymbol);
		A3DMDLeaderSymbolGet(NULL, &sData);
	}
	setting->LinkEndChild(symbol);
	return iRet;
}

//######################################################################################################################
int stTraverseLeaderStub(const A3DMDMarkupLeaderStub* pStub, TiXmlElement* setting)
{
	A3DMDMarkupLeaderStubData sData;
	A3D_INITIALIZE_DATA(A3DMDMarkupLeaderStubData, sData);

	TiXmlElement* stub = new TiXmlElement("A3DMDMarkupLeaderStubData");

	A3DInt32 iRet = A3DMDMarkupLeaderStubGet(pStub, &sData);
	if(iRet == A3D_SUCCESS)
	{
		traverseDoubles("m_pdValues"     , sData.m_uiValuesAndAnchorTypesSize, sData.m_pdValues     , stub);
		traverseUInts  ("m_piAnchorTypes", sData.m_uiValuesAndAnchorTypesSize, sData.m_piAnchorTypes, stub);
		A3DMDMarkupLeaderStubGet(NULL, &sData);
	}
	setting->LinkEndChild(stub);
	return iRet;
}

//######################################################################################################################
int stTraverseMDPosition2D(const A3DMDPosition2D* pPos, TiXmlElement* setting)
{
	A3DMDPosition2DData sData;
	A3D_INITIALIZE_DATA(A3DMDPosition2DData, sData);

	TiXmlElement* pos = new TiXmlElement("A3DMDPosition2DData");

	A3DInt32 iRet = A3DMDPosition2DGet(pPos, &sData);
	if(iRet == A3D_SUCCESS)
	{
		pos->SetDoubleAttribute("m_dOffset", sData.m_dOffset);
		traverseSrfPlane(sData.m_pPlane, pos);
		traversePoint2d("m_sPosition", sData.m_sPosition, pos);
		A3DMDPosition2DGet(NULL, &sData);
	}
	setting->LinkEndChild(pos);
	return iRet;
}

//######################################################################################################################
int stTraverseMDPosition3D(const A3DMDPosition3D* pPos, TiXmlElement* setting)
{
	A3DMDPosition3DData sData;
	A3D_INITIALIZE_DATA(A3DMDPosition3DData, sData);

	TiXmlElement* pos = new TiXmlElement("A3DMDPosition3DData");

	A3DInt32 iRet = A3DMDPosition3DGet(pPos, &sData);
	if(iRet == A3D_SUCCESS)
	{
		traversePoint("m_sPosition", sData.m_sPosition, pos);
		A3DMDPosition3DGet(NULL, &sData);
	}
	setting->LinkEndChild(pos);
	return iRet;
}

//######################################################################################################################
int stTraverseMDPositionReference(const A3DMDPositionReference* pPos, TiXmlElement* setting)
{
	A3DMDPositionReferenceData sData;
	A3D_INITIALIZE_DATA(A3DMDPositionReferenceData, sData);

	TiXmlElement* pos = new TiXmlElement("A3DMDPositionReferenceData");

	A3DInt32 iRet = A3DMDPositionReferenceGet(pPos, &sData);
	if(iRet == A3D_SUCCESS)
	{
		// traverseLinkedItem(sData.m_psLinkedItem, pos); // avoid recursive visitation...
		pos->SetAttribute("m_eAttachType", sData.m_eAttachType);
		traversePoint2d("m_sOffsetToReference", sData.m_sOffsetToReference, pos);
		A3DMDPositionReferenceGet(NULL, &sData);
	}
	setting->LinkEndChild(pos);
	return iRet;
}

//######################################################################################################################
int stTraverseMDPosition(const A3DMDPosition* pPos, TiXmlElement* setting)
{
	A3DEEntityType eType = kA3DTypeUnknown;
	A3DInt32       iRet  = A3DEntityGetType(pPos, &eType);
	if(iRet == A3D_SUCCESS)
	{
		switch(eType)
		{
			case kA3DTypeMDPosition2D:
				iRet = stTraverseMDPosition2D(pPos, setting);
				break;
			case kA3DTypeMDPosition3D:
				iRet = stTraverseMDPosition3D(pPos, setting);
				break;
			case kA3DTypeMDPositionReference:
				iRet = stTraverseMDPositionReference(pPos, setting);
				break;
			default:
				iRet = A3D_INVALID_ENTITY_TYPE;
				break;
		}
	}
	return iRet;
}

//######################################################################################################################
int stTraverseLeaderDefinition(const A3DMDLeaderDefinition* pLeaderDef, TiXmlElement* setting)
{
	A3DMDLeaderDefinitionData sData;
	A3D_INITIALIZE_DATA(A3DMDLeaderDefinitionData, sData);

	TiXmlElement* definition = new TiXmlElement("A3DMDLeaderDefinitionData");

	A3DInt32 iRet = A3DMDLeaderDefinitionGet(pLeaderDef, &sData);
	if(iRet == A3D_SUCCESS)
	{
		setLeaderSymbolType(definition, "m_eTailSymbol", sData.m_eTailSymbol);
		traverseDoubles("m_pdGapList", sData.m_uiNbGapsElements, sData.m_pdGapList, definition);

		if(sData.m_pHeadSymbol != NULL)
			stTraverseLeaderSymbol(sData.m_pHeadSymbol, definition);

		// Commented out
		// TiXmlElement* positions = new TiXmlElement("PathLeaderPositions");
		// positions->SetAttribute("size", (int)sData.m_uiNumberOfPathLeaderPositions);
		// for(A3DUns32 ui = 0; ui < sData.m_uiNumberOfPathLeaderPositions; ++ui)
		// 	stTraverseMDPosition(sData.m_ppsPathLeaderPositions[ui], positions);
		// definition->LinkEndChild(positions);

		if(sData.m_pStub != NULL)
			stTraverseLeaderStub(sData.m_pStub, definition);

		definition->SetAttribute("m_uAnchorFrame", sData.m_uAnchorFrame);
		definition->SetAttribute("m_uAnchorPoint", sData.m_uAnchorPoint);

		if(sData.m_pNextLeader != NULL)
			stTraverseLeaderDefinition(sData.m_pNextLeader, definition);

		A3DMDLeaderDefinitionGet(NULL, &sData);
	}
	setting->LinkEndChild(definition);
	return iRet;
}

//######################################################################################################################
int traverseLeader(const A3DMkpLeader* pLeader, TiXmlElement* setting)
{
	A3DMkpLeaderData sData;
	A3D_INITIALIZE_DATA(A3DMkpLeaderData, sData);

	TiXmlElement* leader = new TiXmlElement("A3DMkpLeaderData");
	traverseSource(pLeader, leader, NULL, -1, -1);

	A3DInt32 iRet = A3DMkpLeaderGet(pLeader, &sData);
	if(iRet == A3D_SUCCESS)
	{
		traverseLinkedItem(sData.m_pLinkedItem, leader);
		// Commented out
		// traverseTessBase(sData.m_pTessellation, leader);
		stTraverseLeaderDefinition(pLeader, leader);
		A3DMkpLeaderGet(NULL, &sData);
	}
	else
		leader->SetAttribute("error", iRet);

	setting->LinkEndChild(leader);
	return iRet;
}

//######################################################################################################################
int traverseLinkedItem(const A3DMiscMarkupLinkedItem* pMkpLinkedItem, TiXmlElement* setting)
{
	A3DMiscMarkupLinkedItemData sData;
	A3D_INITIALIZE_DATA(A3DMiscMarkupLinkedItemData, sData);

	TiXmlElement* mkplinkeditem = new TiXmlElement("A3DMiscMarkupLinkedItemData");
	traverseSource(pMkpLinkedItem, mkplinkeditem, NULL, -1, -1);

	A3DInt32 iRet = A3DMiscMarkupLinkedItemGet(pMkpLinkedItem, &sData);
	if(pMkpLinkedItem && iRet == A3D_SUCCESS)
	{
		traverseEntityReference((A3DMiscEntityReference*)pMkpLinkedItem, mkplinkeditem);
		mkplinkeditem->SetAttribute("m_bMarkupShowControl", sData.m_bMarkupShowControl);
		mkplinkeditem->SetAttribute("m_bMarkupDeleteControl", sData.m_bMarkupDeleteControl);
		mkplinkeditem->SetAttribute("m_bLeaderShowControl", sData.m_bLeaderShowControl);
		mkplinkeditem->SetAttribute("m_bLeaderDeleteControl", sData.m_bLeaderDeleteControl);
		if(sData.m_pTargetProductOccurrence != NULL)
			_SetAttributePtr(mkplinkeditem, "m_pTargetProductOccurrence", (void*) sData.m_pTargetProductOccurrence);	
		A3DMiscMarkupLinkedItemGet(NULL, &sData);
	}
	else
		mkplinkeditem->SetAttribute("error", pMkpLinkedItem ? iRet : A3D_INVALID_ENTITY_NULL);

	setting->LinkEndChild(mkplinkeditem);
	return iRet;
}
