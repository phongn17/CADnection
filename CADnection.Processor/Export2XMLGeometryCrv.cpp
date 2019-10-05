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
static int sttraverseCrvLine(const A3DCrvLine* pCrv, TiXmlElement* setting)
{
	A3DCrvLineData sData;
	A3D_INITIALIZE_DATA(A3DCrvLineData, sData);

	TiXmlElement* crv = new TiXmlElement("A3DCrvLineData");
	traverseSource(pCrv, crv, NULL, -1, -1);

	A3DInt32 iRet = A3DCrvLineGet(pCrv, &sData);
	if(iRet == A3D_SUCCESS)
	{
		setting->SetAttribute("m_bIs2D", (int)sData.m_bIs2D);
		// Commented out
		// traverseTransfo3d(sData.m_sTrsf, crv);
		traverseParam(&sData.m_sParam, crv);
		A3DCrvLineGet(NULL, &sData);
	}
	else
	{
		crv->SetAttribute("error", iRet);
	}

	setting->LinkEndChild(crv);
	return iRet;
}

//######################################################################################################################
static int sttraverseCrvCircle(const A3DCrvCircle* pCrv, TiXmlElement* setting)
{
	A3DCrvCircleData sData;
	A3D_INITIALIZE_DATA(A3DCrvCircleData, sData);

	TiXmlElement* crv = new TiXmlElement("A3DCrvCircleData");
	traverseSource(pCrv, crv, NULL, -1, -1);

	A3DInt32 iRet = A3DCrvCircleGet(pCrv, &sData);
	if(iRet == A3D_SUCCESS)
	{
		setting->SetAttribute("m_bIs2D", (int)sData.m_bIs2D);
		// Commented out
		// traverseTransfo3d(sData.m_sTrsf, crv);
		traverseParam(&sData.m_sParam, crv);
		_SetDoubleAttribute(crv,"m_dRadius", sData.m_dRadius);
		A3DCrvCircleGet(NULL, &sData);
	}
	else
	{
		crv->SetAttribute("error", iRet);
	}

	setting->LinkEndChild(crv);
	return iRet;
}

//######################################################################################################################
static int sttraverseCrvEllipse(const A3DCrvEllipse* pCrv, TiXmlElement* setting)
{
	A3DCrvEllipseData sData;
	A3D_INITIALIZE_DATA(A3DCrvEllipseData, sData);

	TiXmlElement* crv = new TiXmlElement("A3DCrvEllipseData");
	traverseSource(pCrv, crv, NULL, -1, -1);

	A3DInt32 iRet = A3DCrvEllipseGet(pCrv, &sData);
	if(iRet == A3D_SUCCESS)
	{
		setting->SetAttribute("m_bIs2D", (int)sData.m_bIs2D);
		// Commented out
		// traverseTransfo3d(sData.m_sTrsf, crv);
		traverseParam(&sData.m_sParam, crv);
		_SetDoubleAttribute(crv,"m_dXRadius", sData.m_dXRadius);
		_SetDoubleAttribute(crv,"m_dYRadius", sData.m_dYRadius);
		A3DCrvEllipseGet(NULL, &sData);
	}
	else
	{
		crv->SetAttribute("error", iRet);
	}

	setting->LinkEndChild(crv);
	return iRet;
}

//######################################################################################################################
static int sttraverseCrvParabola(const A3DCrvParabola* pCrv, TiXmlElement* setting)
{
	A3DCrvParabolaData sData;
	A3D_INITIALIZE_DATA(A3DCrvParabolaData, sData);

	TiXmlElement* crv = new TiXmlElement("A3DCrvParabolaData");
	traverseSource(pCrv, crv, NULL, -1, -1);

	A3DInt32 iRet = A3DCrvParabolaGet(pCrv, &sData);
	if(iRet == A3D_SUCCESS)
	{
		setting->SetAttribute("m_bIs2D", (int)sData.m_bIs2D);
		// Commented out
		// traverseTransfo3d(sData.m_sTrsf, crv);
		traverseParam(&sData.m_sParam, crv);
		_SetDoubleAttribute(crv,"m_dFocal", sData.m_dFocal);
		crv->SetAttribute("m_cParamType", (int) sData.m_cParamType);
		A3DCrvParabolaGet(NULL, &sData);
	}
	else
	{
		crv->SetAttribute("error", iRet);
	}

	setting->LinkEndChild(crv);
	return iRet;
}

//######################################################################################################################
static int sttraverseCrvHyperbola(const A3DCrvHyperbola* pCrv, TiXmlElement* setting)
{
	A3DCrvHyperbolaData sData;
	A3D_INITIALIZE_DATA(A3DCrvHyperbolaData, sData);

	TiXmlElement* crv = new TiXmlElement("A3DCrvHyperbolaData");
	traverseSource(pCrv, crv, NULL, -1, -1);

	A3DInt32 iRet = A3DCrvHyperbolaGet(pCrv, &sData);
	if(iRet == A3D_SUCCESS)
	{
		setting->SetAttribute("m_bIs2D", (int)sData.m_bIs2D);
		// Commented out
		// traverseTransfo3d(sData.m_sTrsf, crv);
		traverseParam(&sData.m_sParam, crv);
		_SetDoubleAttribute(crv,"m_dSemiAxis", sData.m_dSemiAxis);
		_SetDoubleAttribute(crv,"m_dSemiImageAxis", sData.m_dSemiImageAxis);
		crv->SetAttribute("m_cParamType", (int) sData.m_cParamType);
		A3DCrvHyperbolaGet(NULL, &sData);
	}
	else
	{
		crv->SetAttribute("error", iRet);
	}

	setting->LinkEndChild(crv);
	return iRet;
}

//######################################################################################################################
static int sttraverseCrvHelix(const A3DCrvHelix* pCrv, TiXmlElement* setting)
{
	A3DCrvHelixData sData;
	A3D_INITIALIZE_DATA(A3DCrvHelixData, sData);

	TiXmlElement* crv = new TiXmlElement("A3DCrvHelixData");
	traverseSource(pCrv, crv, NULL, -1, -1);

	A3DInt32 iRet = A3DCrvHelixGet(pCrv, &sData);
	if(iRet == A3D_SUCCESS)
	{
		setting->SetAttribute("m_bIs2D", (int)sData.m_bIs2D);
		A3DCrvHelixGet(NULL, &sData);
	}
	else
	{
		crv->SetAttribute("error", iRet);
	}

	setting->LinkEndChild(crv);
	return iRet;
}

//######################################################################################################################
static int sttraverseCrvPolyLine(const A3DCrvPolyLine* pCrv, TiXmlElement* setting)
{
	A3DCrvPolyLineData sData;
	A3D_INITIALIZE_DATA(A3DCrvPolyLineData, sData);

	TiXmlElement* crv = new TiXmlElement("A3DCrvPolyLineData");
	traverseSource(pCrv, crv, NULL, -1, -1);

	A3DInt32 iRet = A3DCrvPolyLineGet(pCrv, &sData);
	if(iRet == A3D_SUCCESS)
	{
		setting->SetAttribute("m_bIs2D", (int)sData.m_bIs2D);
		// Commented out
		// traverseTransfo3d(sData.m_sTrsf, crv);
		traverseParam(&sData.m_sParam, crv);
		traversePoints("m_pPts", sData.m_uiSize, sData.m_pPts, crv);

		A3DCrvPolyLineGet(NULL, &sData);
	}
	else
	{
		crv->SetAttribute("error", iRet);
	}

	setting->LinkEndChild(crv);
	return iRet;
}

//######################################################################################################################
static int sttraverseCrvComposite(const A3DCrvComposite* pCrv, TiXmlElement* setting)
{
	A3DCrvCompositeData sData;
	A3D_INITIALIZE_DATA(A3DCrvCompositeData, sData);

	TiXmlElement* crv = new TiXmlElement("A3DCrvCompositeData");
	traverseSource(pCrv, crv, NULL, -1, -1);

	A3DInt32 iRet = A3DCrvCompositeGet(pCrv, &sData);
	if(iRet == A3D_SUCCESS)
	{
		setting->SetAttribute("m_bIs2D", (int)sData.m_bIs2D);
		// Commented out
		// traverseTransfo3d(sData.m_sTrsf, crv);
		traverseParam(&sData.m_sParam, crv);
		crv->SetAttribute("m_bClosed", (int) sData.m_bClosed);
		traverseBools("m_pbSenses", sData.m_uiSize, sData.m_pbSenses, crv);

		// Commented out
		// A3DUns32 ui, uiSize = sData.m_uiSize;
		// for(ui = 0; ui < uiSize; ++ui)
		//  	traverseCurve(sData.m_ppCurves[ui], crv);

		A3DCrvCompositeGet(NULL, &sData);
	}
	else
	{
		crv->SetAttribute("error", iRet);
	}

	setting->LinkEndChild(crv);
	return iRet;
}

//######################################################################################################################
static int sttraverseCrvNurbsData(const A3DCrvNurbsData& sData, TiXmlElement* setting)
{
	setting->SetAttribute("m_bIs2D", (int) sData.m_bIs2D);
	setting->SetAttribute("m_bRational", (int) sData.m_bRational);
	setting->SetAttribute("m_eKnotType", (int) sData.m_eKnotType);
	setting->SetAttribute("m_eCurveForm", (int) sData.m_eCurveForm);
	setting->SetAttribute("m_uiDegree", (int) sData.m_uiDegree);
	traverseDoubles("m_pdKnots",sData.m_uiKnotSize, sData.m_pdKnots, setting);
	traversePoints("m_pCtrlPts",sData.m_uiCtrlSize, sData.m_pCtrlPts, setting);

	if(sData.m_bRational)
		traverseDoubles("m_pdWeights", sData.m_uiWeightSize, sData.m_pdWeights, setting);

	return A3D_SUCCESS;
}

//######################################################################################################################
static int sttraverseCrvNurbs(const A3DCrvNurbs* pCrv, TiXmlElement* setting)
{
	A3DCrvNurbsData sData;
	A3D_INITIALIZE_DATA(A3DCrvNurbsData, sData);

	TiXmlElement* crv = new TiXmlElement("A3DCrvNurbsData");
	traverseSource(pCrv, crv, NULL, -1, -1);

	A3DInt32 iRet = A3DCrvNurbsGet(pCrv, &sData);
	if(iRet == A3D_SUCCESS)
	{
		sttraverseCrvNurbsData(sData, crv);
		CHECK_RET(A3DCrvNurbsGet(NULL, &sData));
	}
	else
	{
		crv->SetAttribute("error", iRet);
	}

	setting->LinkEndChild(crv);
	return iRet;
}

//######################################################################################################################
static int sttraverseCrvAsNurbs(const A3DCrvBase* pCrv, TiXmlElement* setting)
{
	A3DCrvNurbsData sData;
	A3D_INITIALIZE_DATA(A3DCrvNurbsData, sData);

	A3DDouble dTolerance=1e-3;
	A3DInt32 iRet = A3DCrvBaseGetAsNurbs(pCrv, dTolerance, true, &sData);
	if(iRet == A3D_SUCCESS)
	{
		sttraverseCrvNurbsData(sData, setting);
		CHECK_RET(A3DCrvNurbsGet(NULL, &sData));
	}
	else
	{
		setting->SetAttribute("error", iRet);
	}

	return iRet;
}

//######################################################################################################################
int traverseCurve(const A3DCrvBase* pCrv, TiXmlElement* setting)
{
	TiXmlElement* crv = new TiXmlElement("A3DCrvBase");
	traverseSource(pCrv, crv, NULL, -1, -1);

	A3DEEntityType eType;
	A3DInt32 iRet = A3DEntityGetType(pCrv,&eType);
	if(iRet == A3D_SUCCESS)
	{
		switch(eType)
		{
		case kA3DTypeCrvNurbs:
			iRet = sttraverseCrvNurbs(pCrv, crv);	
			break;
		case kA3DTypeCrvLine:
			iRet = sttraverseCrvLine(pCrv, crv);
			break;
		case kA3DTypeCrvCircle:
			iRet = sttraverseCrvCircle(pCrv, crv);
			break;
		case kA3DTypeCrvEllipse:
			iRet = sttraverseCrvEllipse(pCrv, crv);
			break;
		case kA3DTypeCrvParabola:
			iRet = sttraverseCrvParabola(pCrv, crv);
			break;
		case kA3DTypeCrvHyperbola:
			iRet = sttraverseCrvHyperbola(pCrv, crv);
			break;
		case kA3DTypeCrvHelix:
			iRet = sttraverseCrvHelix(pCrv, crv);
			break;
		case kA3DTypeCrvPolyLine:
			iRet = sttraverseCrvPolyLine(pCrv, crv);
			break;
		case kA3DTypeCrvComposite:
			iRet = sttraverseCrvComposite(pCrv, crv);
			break;
		default:
			iRet = sttraverseCrvAsNurbs(pCrv, crv);
			break;
		}
	}
	A3DIntervalData oInterval;
	A3D_INITIALIZE_DATA(A3DIntervalData, oInterval);
	A3DCrvGetInterval(pCrv, &oInterval);
	_SetDoubleAttribute(crv, "m_dMin", oInterval.m_dMin);
	_SetDoubleAttribute(crv, "m_dMax", oInterval.m_dMax);

	setting->LinkEndChild(crv);
	return iRet;
}
