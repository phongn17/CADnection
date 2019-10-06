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
#include <boost/algorithm/string.hpp>

//######################################################################################################################
int traverseMaterialProperties(const A3DEntity* pEntity, TiXmlElement* xmlfather, json* jsonData, int index)
{
	TiXmlElement* xml = new TiXmlElement("A3DMiscMaterialPropertiesData");
	A3DMiscMaterialPropertiesData sMaterialPropertiesData;
	A3D_INITIALIZE_DATA(A3DMiscMaterialPropertiesData, sMaterialPropertiesData);

	A3DStatus iRet = A3DMiscGetMaterialProperties(pEntity, &sMaterialPropertiesData);
	if(iRet == A3D_SUCCESS)
	{
		xml->SetDoubleAttribute("m_dDensity", sMaterialPropertiesData.m_dDensity);
		xml->SetAttribute("m_pcMaterialName", sMaterialPropertiesData.m_pcMaterialName ? sMaterialPropertiesData.m_pcMaterialName:"NULL");
		if (jsonData && sMaterialPropertiesData.m_pcMaterialName) {
			(*jsonData)["docs"][index]["MaterialName"] = sMaterialPropertiesData.m_pcMaterialName;
		}
		xml->SetAttribute("m_ePhysicType", sMaterialPropertiesData.m_ePhysicType);
		switch (sMaterialPropertiesData.m_ePhysicType)
		{
		case A3DPhysicType_None:
		case A3DPhysicType_Fiber:
		case A3DPhysicType_HoneyComb:
		case A3DPhysicType_Isotropic:
		case A3DPhysicType_Orthotropic2D:
		case A3DPhysicType_Orthotropic3D:
		case A3DPhysicType_Anisotropic:

		default:
			break;
		}
		A3DMiscGetMaterialProperties(NULL, &sMaterialPropertiesData);
	}

	xmlfather->LinkEndChild(xml);
	return A3D_SUCCESS;
}

//######################################################################################################################
static int stTraversePartDef(const A3DAsmPartDefinition* pPart, TiXmlElement* setting, json* jsonData, int index)
{
	A3DInt32 iRet = A3D_SUCCESS;
	A3DAsmPartDefinitionData sData;
	A3D_INITIALIZE_DATA(A3DAsmPartDefinitionData, sData);

	TiXmlElement* part = new TiXmlElement("A3DAsmPartDefinitionData");
	traverseSource(pPart, part, NULL, -1, -1);

	iRet = A3DAsmPartDefinitionGet(pPart, &sData);
	if(iRet == A3D_SUCCESS)
	{
		A3DUns32 ui;
		part->SetAttribute("m_bInactivateAnnotations", sData.m_bInactivateAnnotations ? "true" : "false");

		iRet = traverseBoundingBox(&sData.m_sBoundingBox, part);

		for(ui = 0; ui < sData.m_uiAnnotationsSize; ++ui)
			traverseAnnotation(sData.m_ppAnnotations[ui], part, jsonData, index, ui);

		for(ui = 0; ui < sData.m_uiViewsSize; ++ui)
			traverseView(sData.m_ppViews[ui], part);
		
		for(ui = 0; ui < sData.m_uiRepItemsSize; ++ui)
			traverseRepItem(sData.m_ppRepItems[ui], part);

		// traverse the drawing !!!!!!!
		for (ui = 0; ui < sData.m_uiDrawingModelsSize; ++ui)
			traverseDrawing(sData.m_ppDrawingModels[ui], part, jsonData, index);

		A3DAsmPartDefinitionGet(NULL, &sData);
	}
	else
	{
		part->SetAttribute("error", iRet);
	}

	traverseMaterialProperties(pPart, setting, NULL, -1);
	setting->LinkEndChild(part);
	return A3D_SUCCESS;
}

//######################################################################################################################
static int stTraverseFilter(const A3DAsmFilter* pFilter, TiXmlElement* setting)
{
	A3DInt32 iRet = A3D_SUCCESS;
	A3DAsmFilterData sData;
	A3D_INITIALIZE_DATA(A3DAsmFilterData, sData);

	TiXmlElement* filter = new TiXmlElement("A3DAsmFilterData");
	traverseSource(pFilter, filter, NULL, -1, -1);

	iRet = A3DAsmFilterGet(pFilter, &sData);
	if(iRet == A3D_SUCCESS)
	{
		filter->SetAttribute("active", (int) sData.m_bIsActive);
		A3DAsmFilterGet(NULL, &sData);
	}
	else
	{
		filter->SetAttribute("error", iRet);
	}

	setting->LinkEndChild(filter);
	return iRet;
}

//######################################################################################################################
int traverseEntityReference(const A3DMiscEntityReference* pEntityReference, TiXmlElement* setting)
{
	A3DInt32 iRet = A3D_SUCCESS;
	A3DMiscEntityReferenceData sData;
	A3D_INITIALIZE_DATA(A3DMiscEntityReferenceData, sData);

	TiXmlElement* entityreference = new TiXmlElement("A3DMiscEntityReferenceData");
	traverseSource(pEntityReference, entityreference, NULL, -1, -1);

	iRet = A3DMiscEntityReferenceGet(pEntityReference, &sData);
	if (iRet != A3D_SUCCESS || sData.m_pEntity == NULL)
	{
		entityreference->SetAttribute("error", iRet);
		setting->LinkEndChild(entityreference);
		return iRet;
	}
	TiXmlElement* entity = new TiXmlElement("m_pEntity");
	_SetAttributePtr(entity, "Address", (void*)sData.m_pEntity);
	entityreference->LinkEndChild(entity);

	if (sData.m_pCoordinateSystem != NULL)
		iRet = traverseCSys(sData.m_pCoordinateSystem, entityreference);

	A3DEEntityType eType = kA3DTypeUnknown;
	iRet = A3DEntityGetType(sData.m_pEntity, &eType);
	if (iRet != A3D_SUCCESS)
	{
		entityreference->SetAttribute("error_A3DEntityGetType", iRet);
		setting->LinkEndChild(entityreference);
		return iRet;
	}

	if (eType == kA3DTypeMiscReferenceOnTopology)
	{
		A3DMiscReferenceOnTopologyData sReferenceOnTopologyData;
		A3D_INITIALIZE_DATA(A3DMiscReferenceOnTopologyData, sReferenceOnTopologyData);
		if(A3DMiscReferenceOnTopologyGet((A3DMiscReferenceOnTopology*) sData.m_pEntity, &sReferenceOnTopologyData) != A3D_SUCCESS)
		{
			entityreference->SetAttribute("error_A3DMiscReferenceOnTopologyGet", iRet);
			setting->LinkEndChild(entityreference);
			return iRet;
		}

		switch(sReferenceOnTopologyData.m_eTopoItemType)
		{
		case kA3DTypeTopoEdge:
			entityreference->SetAttribute("topotype", "Edge");
			if(sReferenceOnTopologyData.m_uiSize != 3)
			{
				entityreference->SetAttribute("ref_count", "error");
				break;
			}
			entityreference->SetAttribute("face_indice", sReferenceOnTopologyData.m_puiAdditionalIndexes[0]);
			entityreference->SetAttribute("loop_indice", sReferenceOnTopologyData.m_puiAdditionalIndexes[1]);
			entityreference->SetAttribute("edge_indice", sReferenceOnTopologyData.m_puiAdditionalIndexes[2]);
			break;
		case kA3DTypeTopoCoEdge:
			entityreference->SetAttribute("topotype", "CoEdge");
			if(sReferenceOnTopologyData.m_uiSize != 3)
			{
				entityreference->SetAttribute("ref_count", "error");
				break;
			}
			entityreference->SetAttribute("face_indice", sReferenceOnTopologyData.m_puiAdditionalIndexes[0]);
			entityreference->SetAttribute("loop_indice", sReferenceOnTopologyData.m_puiAdditionalIndexes[1]);
			entityreference->SetAttribute("coedge_indice", sReferenceOnTopologyData.m_puiAdditionalIndexes[2]);
			break;
		case kA3DTypeTopoFace:
			entityreference->SetAttribute("topotype", "Face");
			if(sReferenceOnTopologyData.m_uiSize != 1)
			{
				entityreference->SetAttribute("ref_count", "error");
				break;
			}
			entityreference->SetAttribute("face_indice", sReferenceOnTopologyData.m_puiAdditionalIndexes[0]);
			break;
		case kA3DTypeTopoUniqueVertex:
			entityreference->SetAttribute("topotype", "Unique Vertex");
			if(sReferenceOnTopologyData.m_uiSize != 4)
			{
				entityreference->SetAttribute("ref_count", "error");
				break;
			}
			entityreference->SetAttribute("face_indice", sReferenceOnTopologyData.m_puiAdditionalIndexes[0]);
			entityreference->SetAttribute("loop_indice", sReferenceOnTopologyData.m_puiAdditionalIndexes[1]);
			entityreference->SetAttribute("edge_indice", sReferenceOnTopologyData.m_puiAdditionalIndexes[2]);
			entityreference->SetAttribute("vertex_indice", sReferenceOnTopologyData.m_puiAdditionalIndexes[3]);
			break;
		case kA3DTypeTopoMultipleVertex:
			entityreference->SetAttribute("topotype", "Multiple Vertex");
			if(sReferenceOnTopologyData.m_uiSize%3 != 0)
			{
				entityreference->SetAttribute("ref_count", "error");
				break;
			}
			entityreference->SetAttribute("ref_count", "todo");
			entityreference->SetAttribute("face_indice", sReferenceOnTopologyData.m_puiAdditionalIndexes[0]);
			entityreference->SetAttribute("loop_indice", sReferenceOnTopologyData.m_puiAdditionalIndexes[1]);
			entityreference->SetAttribute("edge_indice", sReferenceOnTopologyData.m_puiAdditionalIndexes[2]);
			entityreference->SetAttribute("vertex_indice", sReferenceOnTopologyData.m_puiAdditionalIndexes[3]);
			break;
		default:
			entityreference->SetAttribute("linked_item_topo_reference", "unexpected");
			break;
		}
	}
	else if(eType == kA3DTypeMiscReferenceOnCsysItem)
	{
		A3DMiscReferenceOnCsysItemData sA3DMiscReferenceOnCSYSITemData;
		A3D_INITIALIZE_DATA(A3DMiscReferenceOnCsysItemData, sA3DMiscReferenceOnCSYSITemData);
		iRet = A3DMiscReferenceOnCsysItemGet(sData.m_pEntity, &sA3DMiscReferenceOnCSYSITemData);
		if(iRet!=A3D_SUCCESS || !sA3DMiscReferenceOnCSYSITemData.m_pCoordinateSystem)
		{
			entityreference->SetAttribute("error_A3DMiscReferenceOnCsysItemGet", iRet);
			setting->LinkEndChild(entityreference);
			return iRet;
		}
		A3DRootBaseData sCSYSRootData;
		A3D_INITIALIZE_DATA(A3DRootBaseData, sCSYSRootData);
		if( A3DRootBaseGet(sA3DMiscReferenceOnCSYSITemData.m_pCoordinateSystem, &sCSYSRootData)== A3D_SUCCESS)
		{
			entityreference->SetAttribute("cys_name", (sCSYSRootData.m_pcName && sCSYSRootData.m_pcName[0] != '\0')?sCSYSRootData.m_pcName:"NULL");
			entityreference->SetAttribute("index", sA3DMiscReferenceOnCSYSITemData.m_uiIndex);
		}
	}
	else
	{
		A3DRootBaseData sBaseData;
		A3D_INITIALIZE_DATA(A3DRootBaseData, sBaseData);
		iRet = A3DRootBaseGet(sData.m_pEntity,&sBaseData);
		if(iRet == A3D_SUCCESS)
		{
			if(sBaseData.m_pcName && sBaseData.m_pcName[0] != '\0')
				entityreference->SetAttribute("name", sBaseData.m_pcName);
			A3DRootBaseGet(NULL,&sBaseData);
		}

		if (eType == kA3DTypeRiPolyBrepModel)
		{
			A3DMiscReferenceOnTessData sReferenceOnTessData;
			A3D_INITIALIZE_DATA(A3DMiscReferenceOnTessData, sReferenceOnTessData);
			if (A3DMiscReferenceOnTessGet(pEntityReference, &sReferenceOnTessData) == A3D_SUCCESS)
			{
				switch(sReferenceOnTessData.m_eTopoItemType)
				{
					case kA3DTypeTessFace:
						entityreference->SetAttribute("topotype", "Face");
						if (sReferenceOnTessData.m_uiSize == 0)
							entityreference->SetAttribute("ref_count", "error");

						for (A3DUns32 iFace=0;iFace<sReferenceOnTessData.m_uiSize;iFace++)
							entityreference->SetAttribute("face_indice", sReferenceOnTessData.m_puiAdditionalIndexes[iFace]);
						break;
					case kA3DTypeTessEdge:
						entityreference->SetAttribute("topotype", "Edge");
						if (sReferenceOnTessData.m_uiSize != 3)
							entityreference->SetAttribute("ref_count", "error");
						else
						{
							entityreference->SetAttribute("face_indice", sReferenceOnTessData.m_puiAdditionalIndexes[0]);
							entityreference->SetAttribute("loop_indice", sReferenceOnTessData.m_puiAdditionalIndexes[1]);
							entityreference->SetAttribute("edge_indice", sReferenceOnTessData.m_puiAdditionalIndexes[2]);
						}
						break;
					case kA3DTypeTessVertex:
						entityreference->SetAttribute("topotype", "Vertex");
						if (sReferenceOnTessData.m_uiSize != 4)
							entityreference->SetAttribute("ref_count", "error");
						else
						{
							entityreference->SetAttribute("face_indice"  , sReferenceOnTessData.m_puiAdditionalIndexes[0]);
							entityreference->SetAttribute("loop_indice"  , sReferenceOnTessData.m_puiAdditionalIndexes[1]);
							entityreference->SetAttribute("edge_indice"  , sReferenceOnTessData.m_puiAdditionalIndexes[2]);
							entityreference->SetAttribute("vertex_indice", sReferenceOnTessData.m_puiAdditionalIndexes[3]);
						}
						break;
					default: break;
				}

				A3DMiscReferenceOnTessGet(NULL, &sReferenceOnTessData);
			}
		}
		else if(eType == kA3DTypeRiPlane)
		{
			A3DMiscMarkupLinkedItem* pLinkedItem = NULL;
			A3DRiPlaneSupportGet((const A3DRiPlane*) sData.m_pEntity, &pLinkedItem);
			if( pLinkedItem)
			{
				TiXmlElement* entityreferencesupport = new TiXmlElement("SUPPORT");
				entityreference->LinkEndChild(entityreferencesupport);
				traverseEntityReference(pLinkedItem, entityreferencesupport);
			}
		}
		else if(eType == kA3DTypeRiCurve)
		{
			A3DMiscMarkupLinkedItem* pLinkedItem = NULL;
			A3DRiCurveSupportGet((const A3DRiCurve*) sData.m_pEntity, &pLinkedItem);
			if( pLinkedItem)
			{
				TiXmlElement* entityreferencesupport = new TiXmlElement("SUPPORT");
				entityreference->LinkEndChild(entityreferencesupport);
				traverseEntityReference(pLinkedItem, entityreferencesupport);
			}
		}
	}
	A3DMiscEntityReferenceGet(NULL, &sData);

	setting->LinkEndChild(entityreference);
	return iRet;
}

//######################################################################################################################
static int traverseMaterialAndVis(const A3DMaterialAndVisualisationInfos& oMatVis, TiXmlElement* setting)
{
	setting->SetAttribute("m_bIsSuppressed", oMatVis.m_bIsSuppressed);
	setting->SetAttribute("m_usLayer", oMatVis.m_usLayer);
	setting->SetAttribute("m_usBehaviour", oMatVis.m_usBehaviour);
	setting->SetAttribute("m_ucType", oMatVis.m_ucType);
	
	for (A3DUns32 i = 0; i < oMatVis.m_uiPathInAssemblyTreeSize; i++)
	{
		A3DUTF8Char* acValue = oMatVis.m_ppPathInAssemblyTree[i];
		char acBuffer[32];
		sprintf(acBuffer, "m_ppPathInAssemblyTree_%d", i);

		setting->SetAttribute(acBuffer, acValue ? acValue : "");
	}

	if (oMatVis.m_pMaterial)
		traverseMaterial(*oMatVis.m_pMaterial, setting);

	// Commented out
	// if (oMatVis.m_pTransform)
	// 	traverseTransfo3d(*oMatVis.m_pTransform, setting);
	return 0;
}

static void setProductOccurrenceAttributes(const A3DAsmProductOccurrenceData& sData, TiXmlElement* occurrence) {
	occurrence->SetAttribute("m_ucBehaviour", (int)(sData.m_ucBehaviour));
	occurrence->SetAttribute("m_eProductLoadStatus", (int)(sData.m_eProductLoadStatus));

	occurrence->SetAttribute("m_uiProductFlags", (int)(sData.m_uiProductFlags));
	occurrence->SetAttribute("m_bUnitFromCAD", (int)(sData.m_bUnitFromCAD));
	occurrence->SetDoubleAttribute("m_dUnit", (double)(sData.m_dUnit));
	occurrence->SetDoubleAttribute("m_dDensityVolumeUnit", sData.m_dDensityVolumeUnit);
	occurrence->SetDoubleAttribute("m_dDensityMassUnit", sData.m_dDensityMassUnit);
	occurrence->SetAttribute("m_eModellerType", sData.m_eModellerType);
	occurrence->SetAttribute("m_bInactivateAnnotations", sData.m_bInactivateAnnotations ? "true" : "false");
}

void static traverseLayers(const A3DAsmProductOccurrence* pOccurrence, TiXmlElement* occurrence) {
	A3DUns32 uiLayers = 0;
	A3DAsmLayer* asLayers = 0;
	if (A3DAsmProductOccurrenceGetLayerList(pOccurrence, &uiLayers, &asLayers) == A3D_SUCCESS) {
		if (uiLayers) {
			TiXmlElement* pLayers = new TiXmlElement("Layers");
			for (A3DUns32 i = 0; i < uiLayers; ++i) {
				TiXmlElement* Layer = new TiXmlElement("Layer");
				Layer->SetAttribute("Name", asLayers[i].m_pcLayerName);
				Layer->SetAttribute("Layer", asLayers[i].m_usLayer);
				pLayers->LinkEndChild(Layer);
			}
			occurrence->LinkEndChild(pLayers);
		}
		A3DAsmProductOccurrenceGetLayerList(0, &uiLayers, &asLayers);
	}
}

A3DInt32 productOccurrenceGetFilePathNames(const A3DAsmProductOccurrence* pOccurrence, TiXmlElement* occurrence, ThumbnailConverter* thumbnailConverter, json* jsonData, int index) {
	A3DUTF8Char* mPOPath = NULL;
	A3DUTF8Char acSCFileName[_MAX_PATH * 2];
	A3DUTF8Char acPNGFileName[_MAX_PATH * 2];

	A3DInt32 iRet = A3DAsmProductOccurrenceGetFilePathName(pOccurrence, &mPOPath);
	if (iRet == A3D_SUCCESS) {
		occurrence->SetAttribute("A3DAsmProductOccurrenceGetFilePathName", mPOPath != NULL ? mPOPath : "");
		if (jsonData && mPOPath) {
			std::string filename = mPOPath;
			if (boost::filesystem::exists(filename)) {
				std::replace(filename.begin(), filename.end(), '\\', '/');
				(*jsonData)["docs"][index]["FileName"] = filename.c_str();
				std::string ext = boost::filesystem::extension(filename);
				boost::algorithm::replace_last(filename, ext, "");
				(*jsonData)["docs"][index]["FileExtension"] = ext.substr(1).c_str();
				boost::algorithm::to_lower(ext);
				map<string, string> extMappings = thumbnailConverter->GetExtMappings();
				map<string, string>::iterator it = extMappings.find(ext);
				if (it != extMappings.end()) {
					(*jsonData)["docs"][index]["Application"] = it->second;
				}
				std::string id = (*jsonData)["docs"][index]["Id"].get<std::string>();
				if (id.length() > 0) {
					const char* generatedDataFolder = thumbnailConverter->GetGeneratedDataFolder();
					//size_t idx = filename.rfind('/');
					//if (idx != std::string::npos)
					//	filename = filename.substr(idx + 1);
					sprintf(acPNGFileName, "%s%s.png", generatedDataFolder, id.c_str());
					sprintf(acSCFileName, "%s%s.scs", generatedDataFolder, id.c_str());
					bool ret = true;
					if (thumbnailConverter->IsGenerateThumbnail()) {
						ret = thumbnailConverter->Execute(mPOPath, acPNGFileName, acSCFileName);
					}
					if (ret) {
						//const char* dataFolder = thumbnailConverter->GetDataFolder();
						const char* rootPath = thumbnailConverter->GetRootPath();
						const char* author = thumbnailConverter->GetAuthor();
						const char* scPrefix = thumbnailConverter->GetSCPrefix();
						//boost::algorithm::replace_last(filename, dataFolder, dataServer);
						sprintf(acPNGFileName, "%s%s.png", rootPath, id.c_str());
						sprintf(acSCFileName, "%s%s%s.scs", scPrefix, rootPath, id.c_str());
						(*jsonData)["docs"][index]["Thumbnail"] = acPNGFileName;
						(*jsonData)["docs"][index]["StreamCache"] = acSCFileName;
						(*jsonData)["docs"][index]["Author"] = author;
					}
					else {
						cout << "Thumbnail conversion is failed for the file: " << acPNGFileName << endl;
					}
				}
			}
		}
	}
	else
		occurrence->SetAttribute("A3DAsmProductOccurrenceGetFilePathName", iRet);
	A3DAsmProductOccurrenceGetFilePathName(NULL, &mPOPath);

	mPOPath = NULL;
	iRet = A3DAsmProductOccurrenceGetOriginalFilePathName(pOccurrence, &mPOPath);
	if (iRet == A3D_SUCCESS)
		occurrence->SetAttribute("A3DAsmProductOccurrenceGetOriginalFilePathName", mPOPath != NULL ? mPOPath : "");
	else
		occurrence->SetAttribute("A3DAsmProductOccurrenceGetOriginalFilePathName", iRet);
	A3DAsmProductOccurrenceGetOriginalFilePathName(NULL, &mPOPath);

	return iRet;
}

//######################################################################################################################
int traverseGraphScene(const A3DGraphSceneDisplayParametersData& sData, TiXmlElement* setting);
static int stTraversePOccurrence(const A3DAsmProductOccurrence* pOccurrence, TiXmlElement* setting, ThumbnailConverter* thumbnailConverter, json* jsonData, int index, int parentIndex) {
	A3DInt32 iRet = A3D_SUCCESS;
	A3DAsmProductOccurrenceData sData;
	A3D_INITIALIZE_DATA(A3DAsmProductOccurrenceData, sData);

	int occurIndex = index;
	(*jsonData)["docs"][index]["Type"] = "Product Occurrence";

	TiXmlElement* occurrence = new TiXmlElement("A3DAsmProductOccurrenceData");
	traverseSource(pOccurrence, occurrence, jsonData, index, parentIndex);

	A3DRootBaseData sRootBaseData;
	A3D_INITIALIZE_DATA(A3DRootBaseData, sRootBaseData);
	iRet = A3DRootBaseGet(pOccurrence, &sRootBaseData);

	if (iRet == A3D_SUCCESS) {
		if (sRootBaseData.m_pcName && sRootBaseData.m_pcName[0] != '\0') {
			occurrence->SetAttribute("m_pcName", sRootBaseData.m_pcName);
			(*jsonData)["docs"][index]["Name"] = sRootBaseData.m_pcName;
		}

		A3DRootBaseGet(NULL, &sRootBaseData);
	}

	iRet = A3DAsmProductOccurrenceGet(pOccurrence, &sData);
	if (iRet == A3D_SUCCESS) {
		setProductOccurrenceAttributes(sData, occurrence);
		traverseTransformation(sData.m_pLocation, occurrence);
		traverseLayers(pOccurrence, occurrence);

		if (sData.m_pPrototype) {
			TiXmlElement* occurrenceproto = new TiXmlElement("m_pPrototype");
			stTraversePOccurrence(sData.m_pPrototype, occurrenceproto, thumbnailConverter, jsonData, index, parentIndex);
			occurrence->LinkEndChild(occurrenceproto);
		}
		//else // this is commented but beware http://docs.techsoft3d.com/exchange/latest/build/group__a3d__productoccurrence.html#ga497615eb89393c2aba37cce2e2323f4d
		{
			iRet = productOccurrenceGetFilePathNames(pOccurrence, occurrence, thumbnailConverter, jsonData, index);
		}

		if (sData.m_pExternalData) {
			TiXmlElement* occurrenceexternaldata = new TiXmlElement("m_pExternalData");
			stTraversePOccurrence(sData.m_pExternalData, occurrenceexternaldata, thumbnailConverter, jsonData, ++index, occurIndex);
			occurrence->LinkEndChild(occurrenceexternaldata);
		}

		A3DUns32 ui;
		for (ui = 0; ui < sData.m_uiPOccurrencesSize; ++ui)
			stTraversePOccurrence(sData.m_ppPOccurrences[ui], occurrence, thumbnailConverter, jsonData, ++index, occurIndex);

		for (ui = 0; ui < sData.m_uiEntityReferenceSize; ++ui)
			traverseEntityReference(sData.m_ppEntityReferences[ui], occurrence);

		for (ui = 0; ui < sData.m_uiAnnotationsSize; ++ui)
			traverseAnnotation(sData.m_ppAnnotations[ui], occurrence, jsonData, index);

		for (ui = 0; ui < sData.m_uiSceneDisplayParameterSize; ++ui)
			traverseGraphScene(sData.m_psSceneDisplayParameters[ui], occurrence);

		for (ui = 0; ui < sData.m_uiViewsSize; ++ui)
			traverseView(sData.m_ppViews[ui], occurrence);

		if (sData.m_pPart)
			stTraversePartDef(sData.m_pPart, occurrence, jsonData, index);

		if (sData.m_pEntityFilter)
			stTraverseFilter(sData.m_pEntityFilter, occurrence);

		for (ui = 0; ui < sData.m_uiFeatureBasedEntitiesSize; ++ui)
			traverseFeatureTree(sData.m_ppFeatureBasedEntities[ui], occurrence);

		for (ui = 0; ui < sData.m_uiMaterialAndVisualisationSetupSize; ++ui) {
			TiXmlElement* materialandvis = new TiXmlElement("m_psMaterialAndVisualisationSetup");
			traverseMaterialAndVis(sData.m_psMaterialAndVisualisationSetup[ui], materialandvis);
			occurrence->LinkEndChild(materialandvis);
		}

		//----------- Export Specific information per CAD format -----------
		switch (sData.m_eModellerType) {
			case kA3DModellerUnigraphics: {
				A3DAsmProductOccurrenceDataUg sDataUg;
				A3D_INITIALIZE_DATA(A3DAsmProductOccurrenceDataUg, sDataUg);
				iRet = A3DAsmProductOccurrenceGetUg(pOccurrence, &sDataUg);
				if (iRet == A3D_SUCCESS) {
					TiXmlElement* occurrencedataug = new TiXmlElement("A3DAsmProductOccurrenceDataUg");

					occurrencedataug->SetAttribute("RootFilePath",		sDataUg.m_psRootFilePath ? sDataUg.m_psRootFilePath : "NULL");
					occurrencedataug->SetAttribute("FileName",			sDataUg.m_psFileName ? sDataUg.m_psFileName : "NULL");
					occurrencedataug->SetAttribute("PartUID",			sDataUg.m_psPartUID ? sDataUg.m_psPartUID : "NULL");
					occurrencedataug->SetAttribute("InstanceFileName",	sDataUg.m_psInstanceFileName ? sDataUg.m_psInstanceFileName : "NULL");
					occurrencedataug->SetAttribute("InstanceTag",		sDataUg.m_uiInstanceTag);

					// Treat RefSets
					if (sDataUg.m_uiChildrenByRefsetsSize || sDataUg.m_uiSolidsByRefsetsSize) {
						TiXmlElement* refsetList = new TiXmlElement("RefSets");

						// Children by RefSet
						for (A3DUns32 iRefSet = 0; iRefSet < sDataUg.m_uiChildrenByRefsetsSize; ++iRefSet) {
							const A3DElementsByRefsetUg &pRefsetData = sDataUg.m_asChildrenByRefsets[iRefSet];

							// Init Xml Element if first occurrence
							TiXmlElement* refset = new TiXmlElement("RefSet");
							refset->SetAttribute("Name", pRefsetData.m_psRefset);

							// Add children
							for (A3DUns32 i = 0; i < pRefsetData.m_uiElementsSize; ++i) {
								TiXmlElement * refsetChild = new TiXmlElement("Child");
								refsetChild->SetAttribute("id", pRefsetData.m_auiElements[i]);
								refset->LinkEndChild(refsetChild);
							}

							refsetList->LinkEndChild(refset);
						}

						// Solid by RefSet
						for (A3DUns32 iRefSet = 0; iRefSet < sDataUg.m_uiSolidsByRefsetsSize; ++iRefSet) {
							const A3DElementsByRefsetUg &pRefsetData = sDataUg.m_asSolidsByRefsets[iRefSet];

							// Init Xml Element if first occurrence
							TiXmlElement* refset = new TiXmlElement("RefSet");
							refset->SetAttribute("Name", pRefsetData.m_psRefset);

							// Add Solid
							for (A3DUns32 i = 0; i < pRefsetData.m_uiElementsSize; ++i) {
								TiXmlElement * refsetChild = new TiXmlElement("Solid");
								refsetChild->SetAttribute("id", pRefsetData.m_auiElements[i]);
								refset->LinkEndChild(refsetChild);
							}

							refsetList->LinkEndChild(refset);
						}

						occurrencedataug->LinkEndChild(refsetList);
					}

					occurrence->LinkEndChild(occurrencedataug);

					CHECK_RET(A3DAsmProductOccurrenceGetUg(NULL, &sDataUg));
				}
				break;
			}
			default:
				break;
		}

		CHECK_RET(A3DAsmProductOccurrenceGet(NULL, &sData));
	}
	else
	{
		occurrence->SetAttribute("error", iRet);
	}

	traverseMaterialProperties(pOccurrence, occurrence, jsonData, index);

	
	setting->LinkEndChild(occurrence);

	return iRet;
}

//######################################################################################################################
int traverseModel(const A3DAsmModelFile* pModelFile, Export2Json& export2Json, ThumbnailConverter* thumbnailConverter, TiXmlElement* setting)
{
	A3DInt32 iRet = A3D_SUCCESS;
	A3DAsmModelFileData sData;
	A3D_INITIALIZE_DATA(A3DAsmModelFileData, sData);

	TiXmlElement* model = new TiXmlElement("A3DAsmModelFileData");
	traverseSource(pModelFile, model, NULL, -1, -1);

	iRet = A3DAsmModelFileGet(pModelFile , &sData);
	if(iRet == A3D_SUCCESS)
	{
		A3DUns32 ui;

		model->SetAttribute("m_bUnitFromCAD", sData.m_bUnitFromCAD);
		model->SetDoubleAttribute("m_dUnit", sData.m_dUnit);
		model->SetAttribute("m_eModellerType", sData.m_eModellerType);

		for(ui = 0; ui < sData.m_uiPOccurrencesSize; ++ui)
			stTraversePOccurrence(sData.m_ppPOccurrences[ui], model, thumbnailConverter, export2Json.GetData(), 0, -1);

		CHECK_RET(A3DAsmModelFileGet(NULL, &sData));
	}
	else
	{
		model->SetAttribute("error", iRet);
	}

	setting->LinkEndChild(model);
	return iRet;
}
