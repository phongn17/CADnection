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
static int sttraverseSrfNurbsData(const A3DSurfNurbsData& sData, TiXmlElement* setting)
{
	setting->SetAttribute("m_eKnotType", (int) sData.m_eKnotType);
	setting->SetAttribute("m_eSurfaceForm", (int) sData.m_eSurfaceForm);
	setting->SetAttribute("m_uiUDegree", (int) sData.m_uiUDegree);
	setting->SetAttribute("m_uiVDegree", (int) sData.m_uiVDegree);

	traverseDoubles("m_pdUKnots", sData.m_uiUKnotSize, sData.m_pdUKnots, setting);
	traverseDoubles("m_pdVKnots", sData.m_uiVKnotSize, sData.m_pdVKnots, setting);
	traversePoints("m_pCtrlPts", sData.m_uiUCtrlSize*sData.m_uiVCtrlSize, sData.m_pCtrlPts, setting);

	if(sData.m_pdWeights)
		traverseDoubles("m_pdWeights", sData.m_uiUCtrlSize*sData.m_uiVCtrlSize, sData.m_pdWeights, setting);

	return A3D_SUCCESS;
}

//######################################################################################################################
static int sttraverseSrfNurbs(const A3DSurfNurbs* pSrf, TiXmlElement* setting)
{
	A3DSurfNurbsData sData;
	A3D_INITIALIZE_DATA(A3DSurfNurbsData, sData);

	TiXmlElement* srf = new TiXmlElement("A3DSurfNurbsData");
	traverseSource(pSrf, srf, NULL, -1, -1);

	A3DInt32 iRet = A3DSurfNurbsGet(pSrf, &sData);
	if(iRet == A3D_SUCCESS)
	{
		sttraverseSrfNurbsData(sData, srf);
		CHECK_RET(A3DSurfNurbsGet(NULL, &sData));
	}
	else
	{
		srf->SetAttribute("error", iRet);
	}

	setting->LinkEndChild(srf);
	return iRet;
}

//######################################################################################################################
static int sttraverseSrfAsNurbs(const A3DSurfBase* pSrf, TiXmlElement* setting)
{
	A3DSurfNurbsData sData;
	A3D_INITIALIZE_DATA(A3DSurfNurbsData, sData);

	A3DDouble dTolerance = 1e-3;
	A3DInt32 iRet = A3DSurfBaseGetAsNurbs(pSrf,dTolerance, false, &sData);
	if(iRet == A3D_SUCCESS)
	{
		sttraverseSrfNurbsData(sData, setting);
		CHECK_RET(A3DSurfNurbsGet(NULL, &sData));
	}
	else
	{
		setting->SetAttribute("error", iRet);
	}

	return iRet;
}

//######################################################################################################################
int traverseSrfPlane(const A3DSurfPlane* pSrf, TiXmlElement* setting)
{
	A3DSurfPlaneData sData;
	A3D_INITIALIZE_DATA(A3DSurfPlaneData, sData);

	TiXmlElement* srf = new TiXmlElement("A3DSurfPlaneData");
	traverseSource(pSrf, srf, NULL, -1, -1);

	A3DInt32 iRet = A3DSurfPlaneGet(pSrf, &sData);
	if(iRet == A3D_SUCCESS)
	{
		// Commented out
		// traverseTransfo3d(sData.m_sTrsf, srf);
		traverseUVParam(&sData.m_sParam, srf);

		CHECK_RET(A3DSurfPlaneGet(NULL, &sData));
	}
	else
	{
		srf->SetAttribute("error", iRet);
	}

	setting->LinkEndChild(srf);
	return iRet;
}

//######################################################################################################################
int traverseSurface(const A3DSurfBase* pSrf, TiXmlElement* setting)
{
	TiXmlElement* srf = new TiXmlElement("A3DSurfBase");
	traverseSource(pSrf, srf, NULL, -1, -1);

	A3DEEntityType eType;
	A3DInt32 iRet = A3DEntityGetType(pSrf, &eType);
	if(iRet == A3D_SUCCESS)
	{
		switch(eType)
		{
		case kA3DTypeSurfNurbs:
			iRet = sttraverseSrfNurbs(pSrf, srf);
			break;
		case kA3DTypeSurfPlane:
			iRet = traverseSrfPlane(pSrf, srf);
			break;
		default:
			break;
		}
	}
	else if(iRet == A3D_NOT_IMPLEMENTED)
		iRet = sttraverseSrfAsNurbs(pSrf, srf);

	A3DDomainData oDomain;
	A3D_INITIALIZE_DATA(A3DDomainData, oDomain);
	CHECK_RET(A3DSrfGetDomain(pSrf, &oDomain));
	_SetDoubleAttribute(srf, "m_sMin.m_dX", oDomain.m_sMin.m_dX);
	_SetDoubleAttribute(srf, "m_sMin.m_dY", oDomain.m_sMin.m_dY);
	_SetDoubleAttribute(srf, "m_sMax.m_dX", oDomain.m_sMax.m_dX);
	_SetDoubleAttribute(srf, "m_sMax.m_dY", oDomain.m_sMax.m_dY);

	setting->LinkEndChild(srf);

	return iRet;
}
