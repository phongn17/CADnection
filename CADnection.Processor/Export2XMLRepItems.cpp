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

#include <memory>

//######################################################################################################################
int traverseCSys(const A3DRiCoordinateSystem* pCSys, TiXmlElement* setting)
{	
	A3DRiCoordinateSystemData sData;
	A3D_INITIALIZE_DATA(A3DRiCoordinateSystemData, sData);

	TiXmlElement* csys = new TiXmlElement("A3DRiCoordinateSystemData");
	traverseSource(pCSys, csys, NULL, -1, -1);

	A3DInt32 iRet = A3DRiCoordinateSystemGet(pCSys, &sData);
	if(iRet == A3D_SUCCESS)
	{
		// Commented out
		// iRet = traverseTransfo3d(sData.m_pTransformation, csys);
		A3DRiCoordinateSystemGet(NULL, &sData);
	}
	else
	{
		csys->SetAttribute("error", iRet);
	}

	setting->LinkEndChild(csys);
	return iRet;
}

//######################################################################################################################
int traverseRepItemContent(const A3DRiRepresentationItem* pRi, TiXmlElement* setting)
{
	A3DInt32 iRet = A3D_SUCCESS;
	A3DRiRepresentationItemData sData;
	A3D_INITIALIZE_DATA(A3DRiRepresentationItemData, sData);

	iRet = A3DRiRepresentationItemGet(pRi, &sData);
	if(iRet == A3D_SUCCESS)
	{
		std::unique_ptr<TiXmlElement> repitemdata(new TiXmlElement("A3DRiRepresentationItemData"));

		if(sData.m_pCoordinateSystem)
			traverseCSys(sData.m_pCoordinateSystem, repitemdata.get());

		// Commented out
		// if(sData.m_pTessBase)
		// 	traverseTessBase(sData.m_pTessBase, repitemdata.get());

		if(sData.m_pCoordinateSystem || sData.m_pTessBase)
			setting->LinkEndChild(repitemdata.release());

		A3DRiRepresentationItemGet(NULL, &sData);
	}

	return A3D_SUCCESS;
}

//######################################################################################################################
static int traverseBrepModel(const A3DRiBrepModel* pBrepModel, TiXmlElement* setting)
{
	A3DRiBrepModelData sData;
	A3D_INITIALIZE_DATA(A3DRiBrepModelData, sData);

	TiXmlElement* brep = new TiXmlElement("A3DRiBrepModelData");
	traverseSource(pBrepModel, brep, NULL, -1, -1);
	// Commented out
	// traverseRepItemContent(pBrepModel, brep);
	traverseMaterialProperties(pBrepModel, brep, NULL, -1);

	TiXmlElement* physicalprops = new TiXmlElement("A3DPhysicalPropertiesData");

	A3DVector3dData psScale;
	A3D_INITIALIZE_DATA(A3DVector3dData, psScale);
	psScale.m_dX = 1;
	psScale.m_dY = 1;
	psScale.m_dZ = 1;
	A3DPhysicalPropertiesData physicalPropertiesData;
	A3D_INITIALIZE_A3DPhysicalPropertiesData(physicalPropertiesData);
	A3DInt32 iRet = A3DComputePhysicalProperties(pBrepModel, &psScale, &physicalPropertiesData);

	physicalprops->SetAttribute("m_bVolumecomputed", physicalPropertiesData.m_bVolumeComputed);
	physicalprops->SetDoubleAttribute("m_dSurface", (double)physicalPropertiesData.m_dSurface);
	physicalprops->SetDoubleAttribute("m_dVolume", (double)physicalPropertiesData.m_dVolume);

	physicalprops->SetDoubleAttribute("m_sGravityCenter.m_dX",(double) physicalPropertiesData.m_sGravityCenter.m_dX); 
	physicalprops->SetDoubleAttribute("m_sGravityCenter.m_dY", (double)physicalPropertiesData.m_sGravityCenter.m_dY);
	physicalprops->SetDoubleAttribute("m_sGravityCenter.m_dZ",(double) physicalPropertiesData.m_sGravityCenter.m_dZ);

	setting->LinkEndChild(physicalprops);
	// Commented out
	// iRet = A3DRiBrepModelGet(pBrepModel, &sData);
	// if(iRet == A3D_SUCCESS)
	// {
	// 	traverseBrepData(sData.m_pBrepData, brep);
	// 	A3DRiBrepModelGet(NULL, &sData);
	// }

	setting->LinkEndChild(brep);
	return iRet;
}

//######################################################################################################################
static int traversePolyBrepModel(const A3DRiPolyBrepModel* pPolyBrepModel, TiXmlElement* setting)
{
	A3DRiPolyBrepModelData sData;
	A3D_INITIALIZE_DATA(A3DRiPolyBrepModelData, sData);

	TiXmlElement* polybrep = new TiXmlElement("A3DRiPolyBrepModelData");
	traverseSource(pPolyBrepModel, polybrep, NULL, -1, -1);
	traverseRepItemContent(pPolyBrepModel, polybrep);
	traverseMaterialProperties(pPolyBrepModel, polybrep, NULL, -1);

	A3DPhysicalPropertiesData physicalPropertiesData;
	A3D_INITIALIZE_DATA(A3DPhysicalPropertiesData, physicalPropertiesData);
	A3DVector3dData psScale;
	A3D_INITIALIZE_DATA(A3DVector3dData, psScale);
	psScale.m_dX = 1;
	psScale.m_dY = 1;
	psScale.m_dZ = 1;
	A3DInt32 iRet = A3DComputePolyBrepPhysicalProperties(pPolyBrepModel, &psScale, &physicalPropertiesData);

	TiXmlElement* physicalprops = new TiXmlElement("A3DPhysicalPropertiesData");
	physicalprops->SetAttribute("m_bVolumecomputed", physicalPropertiesData.m_bVolumeComputed);
	physicalprops->SetDoubleAttribute("m_dSurface", (double)physicalPropertiesData.m_dSurface);
	physicalprops->SetDoubleAttribute("m_dVolume", (double)physicalPropertiesData.m_dVolume);

	physicalprops->SetDoubleAttribute("m_sGravityCenter.m_dX",(double) physicalPropertiesData.m_sGravityCenter.m_dX); 
	physicalprops->SetDoubleAttribute("m_sGravityCenter.m_dY", (double)physicalPropertiesData.m_sGravityCenter.m_dY);
	physicalprops->SetDoubleAttribute("m_sGravityCenter.m_dZ",(double) physicalPropertiesData.m_sGravityCenter.m_dZ);

	setting->LinkEndChild(physicalprops);

	iRet = A3DRiPolyBrepModelGet(pPolyBrepModel, &sData);
	if(iRet == A3D_SUCCESS)
	{
		polybrep->SetAttribute("m_bIsClosed", (int) sData.m_bIsClosed);
		A3DRiPolyBrepModelGet(NULL, &sData);
	}

	setting->LinkEndChild(polybrep);
	return iRet;
}

//######################################################################################################################
static int traverseRICurve(const A3DRiCurve* pRICrv, TiXmlElement* setting)
{
	A3DRiCurveData sData;
	A3D_INITIALIZE_DATA(A3DRiCurveData, sData);

	TiXmlElement* ricrv = new TiXmlElement("A3DRiCurveData");
	traverseSource(pRICrv, ricrv, NULL, -1, -1);
	traverseRepItemContent(pRICrv, ricrv);
	traverseMaterialProperties(pRICrv, ricrv, NULL, -1);

	A3DInt32 iRet = A3DRiCurveGet(pRICrv, &sData);
	if(iRet == A3D_SUCCESS)
	{
		traverseSingleWireBody(sData.m_pBody, ricrv);
		A3DRiCurveGet(NULL, &sData);
	}

	setting->LinkEndChild(ricrv);
	return iRet;
}

//######################################################################################################################
static int traverseRIPlane(const A3DRiPlane* pRIPlane, TiXmlElement* setting)
{
	A3DRiPlaneData sData;
	A3D_INITIALIZE_DATA(A3DRiPlaneData, sData);

	TiXmlElement* riplane = new TiXmlElement("A3DRiPlaneData");
	traverseSource(pRIPlane, riplane, NULL, -1, -1);
	traverseRepItemContent(pRIPlane, riplane);
	traverseMaterialProperties(pRIPlane, riplane, NULL, -1);

	A3DInt32 iRet = A3DRiPlaneGet(pRIPlane, &sData);
	if(iRet == A3D_SUCCESS)
	{
		traverseBrepData(sData.m_pBrepData, riplane);
		A3DRiPlaneGet(NULL, &sData);
	}

	setting->LinkEndChild(riplane);
	return iRet;
}

//######################################################################################################################
static int traverseDirection(const A3DRiDirection* pDirection, TiXmlElement* setting)
{
	A3DRiDirectionData sData;
	A3D_INITIALIZE_DATA(A3DRiDirectionData, sData);

	TiXmlElement* dir = new TiXmlElement("A3DRiDirectionData");
	traverseSource(pDirection, dir, NULL, -1, -1);
	traverseRepItemContent(pDirection, dir);

	A3DInt32 iRet = A3DRiDirectionGet(pDirection, &sData);
	if(iRet == A3D_SUCCESS)
	{
		traversePoint("m_sOrigin"   , sData.m_sOrigin   , dir);
		traversePoint("m_sDirection", sData.m_sDirection, dir);
		A3DRiDirectionGet(NULL, &sData);
	}
	else
	{
		dir->SetAttribute("error", iRet);
	}

	setting->LinkEndChild(dir);
	return iRet;
}			

//######################################################################################################################
static int traversePointSet(const A3DRiPointSet* pPointSet, TiXmlElement* setting)
{
	A3DRiPointSetData sData;
	A3D_INITIALIZE_DATA(A3DRiPointSetData, sData);

	TiXmlElement* pointset = new TiXmlElement("A3DRiPointSetData");
	traverseSource(pPointSet, pointset, NULL, -1, -1);
	traverseRepItemContent(pPointSet, pointset);
	traverseMaterialProperties(pPointSet, pointset, NULL, -1);

	A3DInt32 iRet = A3DRiPointSetGet(pPointSet, &sData);
	if(iRet == A3D_SUCCESS)
	{
		for(A3DUns32 ui = 0; ui < sData.m_uiSize; ++ui)
			traversePoint("m_pPts", sData.m_pPts[ui], pointset);
		A3DRiPointSetGet(NULL, &sData);
	}
	else
	{
		pointset->SetAttribute("error", iRet);
	}

	setting->LinkEndChild(pointset);
	return iRet;
}

//######################################################################################################################
static int traverseSet(const A3DRiSet* pSet, TiXmlElement* setting)
{	
	A3DInt32 iRet = A3D_SUCCESS;
	A3DRiSetData sData;
	A3D_INITIALIZE_DATA(A3DRiSetData, sData);

	TiXmlElement* set = new TiXmlElement("A3DRiSetData");
	traverseSource(pSet, set, NULL, -1, -1);
	traverseRepItemContent(pSet, set);
	traverseMaterialProperties(pSet, set, NULL, -1);

	iRet = A3DRiSetGet(pSet, &sData);
	if(iRet == A3D_SUCCESS)
	{
		for(A3DUns32 ui = 0; ui < sData.m_uiRepItemsSize; ++ui)
			iRet = traverseRepItem(sData.m_ppRepItems[ui], set);

		A3DRiSetGet(NULL, &sData);
	}
	else
	{
		set->SetAttribute("error", iRet);
	}

	setting->LinkEndChild(set);
	return iRet;
}

//######################################################################################################################
int traverseDrawingBlock(const A3DDrawingBlock *pDrwBlock, TiXmlElement* setting)
{
	A3DInt32 iRet = A3D_SUCCESS;
	
	A3DDrawingBlockBasicData sDrwBlockData;
	A3D_INITIALIZE_DATA(A3DDrawingBlockBasicData, sDrwBlockData);
	
	CHECK_RET( A3DDrawingBlockBasicGet( (A3DDrawingBlockBasic const*) pDrwBlock, &sDrwBlockData));
	
	TiXmlElement* drwBlock = new TiXmlElement("A3DDrawingBlockData");
	
	drwBlock->SetAttribute("Nb_Entities", sDrwBlockData.m_uiDrwEntitiesSize);
	drwBlock->SetAttribute("Nb_Markups", sDrwBlockData.m_uiMarkupsSize);
	drwBlock->SetAttribute("Nb_SubBlocks", sDrwBlockData.m_uiDrwBlocksSize);

	for (A3DUns32 ui = 0; ui < sDrwBlockData.m_uiMarkupsSize; ++ui)
	{
		traverseMarkup(sDrwBlockData.m_ppMarkups[ui], drwBlock, NULL, -1, -1);
	}
	
	setting->LinkEndChild(drwBlock);
	
	return iRet;
}

//######################################################################################################################
int traverseDrawingView(const A3DDrawingView *pDrwView, TiXmlElement* setting, json* jsonData, int index, int viewIndex)
{

	A3DDrawingViewData sDrwViewData;
	A3D_INITIALIZE_DATA(A3DDrawingViewData, sDrwViewData);


	TiXmlElement* drwView = new TiXmlElement("A3DDrawingViewData");

	// Get the type of the drawing view
	A3DInt32 iRet = A3D_SUCCESS;

	iRet = A3DDrawingViewGet(pDrwView, &sDrwViewData);

	A3DEDrawingViewType eDrwType = sDrwViewData.m_eType;

	if (jsonData != NULL && viewIndex != -1) {
		(*jsonData)["docs"][index]["DrawingViews"][viewIndex]["Id"] = boost::lexical_cast<std::string>(boost::uuids::random_generator()());
	}

	switch(eDrwType)
	{
		case kA3DDrawingViewTypeIso: 
			drwView->SetAttribute("ISO view",1);
			if (jsonData != NULL && viewIndex != -1) {
				(*jsonData)["docs"][index]["DrawingViews"][viewIndex]["ISOView"] = 1;
			}
			break;
		case kA3DDrawingViewTypeTop:
			drwView->SetAttribute("Top view",1);
			if (jsonData != NULL && viewIndex != -1) {
				(*jsonData)["docs"][index]["DrawingViews"][viewIndex]["TopView"] = 1;
			}
			break;
		case kA3DDrawingViewTypeBottom:
			drwView->SetAttribute("Bottom view",1);
			if (jsonData != NULL && viewIndex != -1) {
				(*jsonData)["docs"][index]["DrawingViews"][viewIndex]["BottomView"] = 1;
			}
			break;
		case kA3DDrawingViewTypeLeft:
			drwView->SetAttribute("Left view",1);
			if (jsonData != NULL && viewIndex != -1) {
				(*jsonData)["docs"][index]["DrawingViews"][viewIndex]["ISOView"] = 1;
			}
			break;
		case kA3DDrawingViewTypeRight:
			drwView->SetAttribute("Right view",1);
			if (jsonData != NULL && viewIndex != -1) {
				(*jsonData)["docs"][index]["DrawingViews"][viewIndex]["RightView"] = 1;
			}
			break;
		case kA3DDrawingViewTypeFront:
			drwView->SetAttribute("Front view",1);
			if (jsonData != NULL && viewIndex != -1) {
				(*jsonData)["docs"][index]["DrawingViews"][viewIndex]["FrontView"] = 1;
			}
			break;
		case kA3DDrawingViewTypeBack:
			drwView->SetAttribute("Back view",1);
			if (jsonData != NULL && viewIndex != -1) {
				(*jsonData)["docs"][index]["DrawingViews"][viewIndex]["BackView"] = 1;
			}
			break;
		case kA3DDrawingViewTypeBackground:
			drwView->SetAttribute("Background view",1);
			if (jsonData != NULL && viewIndex != -1) {
				(*jsonData)["docs"][index]["DrawingViews"][viewIndex]["BackgroundView"] = 1;
			}
			break;
		case kA3DDrawingViewTypeWorking:
			drwView->SetAttribute("Working view",1);
			if (jsonData != NULL && viewIndex != -1) {
				(*jsonData)["docs"][index]["DrawingViews"][viewIndex]["WorkingView"] = 1;
			}
			break;
		case kA3DDrawingViewTypeProjected:
			drwView->SetAttribute("Projected view",1);
			if (jsonData != NULL && viewIndex != -1) {
				(*jsonData)["docs"][index]["DrawingViews"][viewIndex]["ProjectedView"] = 1;
			}
			break;
		case kA3DDrawingViewTypeAuxiliary:
			drwView->SetAttribute("Auxiliary view",1);
			if (jsonData != NULL && viewIndex != -1) {
				(*jsonData)["docs"][index]["DrawingViews"][viewIndex]["AuxiliaryView"] = 1;
			}
			break;
		case kA3DDrawingViewTypeSection:
			drwView->SetAttribute("Section view",1);
			if (jsonData != NULL && viewIndex != -1) {
				(*jsonData)["docs"][index]["DrawingViews"][viewIndex]["SectionView"] = 1;
			}
			break;
		case kA3DDrawingViewTypeDetail :
			drwView->SetAttribute("Detail view",1);
			if (jsonData != NULL && viewIndex != -1) {
				(*jsonData)["docs"][index]["DrawingViews"][viewIndex]["DetailView"] = 1;
			}
			break;
		default:
			drwView->SetAttribute("Unknown",1);
			if (jsonData != NULL && viewIndex != -1) {
				(*jsonData)["docs"][index]["DrawingViews"][viewIndex]["Unknown"] = 1;
			}
			break;
	}

	// get the name of the drawing view
	A3DRootBaseData sData;
	A3D_INITIALIZE_DATA(A3DRootBaseData, sData);
	CHECK_RET(A3DRootBaseGet(pDrwView, &sData));

	if (sData.m_pcName && sData.m_pcName[0] != '\0') {
		drwView->SetAttribute("name", sData.m_pcName);
		if (jsonData != NULL && viewIndex != -1) {
			(*jsonData)["docs"][index]["DrawingViews"][viewIndex]["Name"] = sData.m_pcName;
		}
	}
	else {
		if (jsonData != NULL && viewIndex != -1) {
			(*jsonData)["docs"][index]["DrawingViews"][viewIndex]["Name"] = "View " + std::to_string(viewIndex + 1);
		}
	}
	if (jsonData != NULL && viewIndex != -1) {
		(*jsonData)["docs"][index]["DrawingViews"][viewIndex]["Type"] = "Drawing View";
	}
	A3DRootBaseGet(NULL,&sData);

	if (sDrwViewData.m_pLocalBlocks)
	{
		traverseDrawingBlock(sDrwViewData.m_pLocalBlocks, drwView);
	}

	for (A3DUns32 ui = 0; ui < sDrwViewData.m_uiDrwBlocksSize; ++ui)
	{
		traverseDrawingBlock(sDrwViewData.m_ppDrwBlocks[ui], drwView);
	}

	setting->LinkEndChild(drwView);

	return iRet;
}

//######################################################################################################################
int traverseDrawingSheet(const A3DDrawingSheet * pDrwSheet, TiXmlElement* setting, json* jsonData, int index)
{
	A3DInt32 iRet = A3D_SUCCESS;

	A3DDrawingSheetData sDrwSheetData;
	A3D_INITIALIZE_DATA(A3DDrawingSheetData, sDrwSheetData);

	iRet = A3DDrawingSheetGet(pDrwSheet, &sDrwSheetData);


	TiXmlElement* pSheet = new TiXmlElement("A3DDrawingSheetdata");

	for (A3DUns32 ui = 0; ui < sDrwSheetData.m_uiDrwViewsSize; ++ui)
	{
		traverseDrawingView(sDrwSheetData.m_ppDrwViews[ui], pSheet, jsonData, index, ui);
	}

	setting->LinkEndChild(pSheet);

	return iRet;

}

//######################################################################################################################
int traverseDrawing(const A3DDrawingModel * pDrawing, TiXmlElement* setting, json* jsonData, int index)
{
	A3DInt32 iRet = A3D_SUCCESS;

	A3DDrawingModelData sDrwModelData;
	A3D_INITIALIZE_DATA(A3DDrawingModelData, sDrwModelData);

	A3DRootBaseData sBaseData;
	A3D_INITIALIZE_DATA(A3DRootBaseData, sBaseData);

	A3DRootBaseGet(pDrawing, &sBaseData);

	iRet = A3DDrawingModelGet(pDrawing, &sDrwModelData);

	TiXmlElement* drawing = new TiXmlElement("A3DDrawingModeldata");

	// traverse the drawing sheet
	for (A3DUns32 ui = 0; ui < sDrwModelData.m_uiDrwSheetsSize; ++ui)
	{
		traverseDrawingSheet(sDrwModelData.m_ppDrwSheets[ui], drawing, jsonData, index);
	}

	setting->LinkEndChild(drawing);

	return iRet;
}


//######################################################################################################################
int traverseRepItem(const A3DRiRepresentationItem* pRepItem, TiXmlElement* setting)
{
	A3DInt32 iRet = A3D_SUCCESS;
	A3DEEntityType eType;

	CHECK_RET(A3DEntityGetType(pRepItem, &eType));

	switch(eType)
	{
	case kA3DTypeRiSet:
		iRet = traverseSet(pRepItem, setting);
		break;
	case kA3DTypeRiPointSet:
		iRet = traversePointSet(pRepItem, setting);
		break;
	case kA3DTypeRiDirection:
		iRet = traverseDirection(pRepItem, setting);
		break;
	case kA3DTypeRiCurve:
		iRet = traverseRICurve(pRepItem, setting);
		break;
	case kA3DTypeRiCoordinateSystem:
		iRet = traverseCSys(pRepItem, setting);
		break;
	case kA3DTypeRiPlane:
		iRet = traverseRIPlane(pRepItem, setting);
		break;
	case kA3DTypeRiBrepModel:
		iRet = traverseBrepModel(pRepItem, setting);
		break;
	case kA3DTypeRiPolyBrepModel:
		iRet = traversePolyBrepModel(pRepItem, setting);
		break;
	default:
		iRet = A3D_NOT_IMPLEMENTED;
		break;
	}
	return iRet;
}
