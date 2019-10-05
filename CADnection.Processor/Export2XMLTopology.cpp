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
static int sttraverseMultipleVertex(const A3DTopoMultipleVertex* pVertex, TiXmlElement* setting)
{
	A3DTopoMultipleVertexData sData;
	A3D_INITIALIZE_DATA(A3DTopoMultipleVertexData, sData);

	TiXmlElement* vertex = new TiXmlElement("A3DTopoMultipleVertexData");
	traverseSource(pVertex, vertex, NULL, -1, -1);

	A3DInt32 iRet = A3DTopoMultipleVertexGet(pVertex, &sData);
	if(iRet == A3D_SUCCESS)
	{
		traversePoints("m_pPts",sData.m_uiSize, sData.m_pPts, vertex);
		A3DTopoMultipleVertexGet(NULL, &sData);
	}
	else
	{
		vertex->SetAttribute("error", iRet);
	}

	setting->LinkEndChild(vertex);
	return iRet;
}

//######################################################################################################################
static int sttraverseUniqueVertex(const A3DTopoUniqueVertex* pVertex, TiXmlElement* setting)
{
	A3DTopoUniqueVertexData sData;
	A3D_INITIALIZE_DATA(A3DTopoUniqueVertexData, sData);

	TiXmlElement* vertex = new TiXmlElement("A3DTopoUniqueVertexData");
	traverseSource(pVertex, vertex, NULL, -1, -1);

	A3DInt32 iRet = A3DTopoUniqueVertexGet(pVertex, &sData);
	if(iRet == A3D_SUCCESS)
	{
		traversePoint("m_sPoint", sData.m_sPoint, vertex);
		_SetDoubleAttribute(vertex, "m_dTolerance", sData.m_dTolerance);
		A3DTopoUniqueVertexGet(NULL, &sData);
	}
	else
	{
		vertex->SetAttribute("error", iRet);
	}

	setting->LinkEndChild(vertex);
	return iRet;
}

//######################################################################################################################
static int sttraverseVertex(const A3DTopoVertex* pVertex, TiXmlElement* setting)
{
	A3DEEntityType eType;
	A3DInt32 iRet = A3DEntityGetType(pVertex, &eType);
	if(iRet == A3D_SUCCESS)
	{
		switch(eType)
		{
		case kA3DTypeTopoMultipleVertex:
			iRet = sttraverseMultipleVertex(pVertex, setting);
			break;
		case kA3DTypeTopoUniqueVertex:
			iRet = sttraverseUniqueVertex(pVertex, setting);
			break;
		default:
			break;
		}
	}
	return iRet;
}

//######################################################################################################################
static int sttraverseEdge(const A3DTopoEdge* pEdge, TiXmlElement* setting)
{
	A3DTopoEdgeData sData;
	A3D_INITIALIZE_DATA(A3DTopoEdgeData, sData);

	TiXmlElement* edge = new TiXmlElement("A3DTopoEdgeData");
	traverseSource(pEdge, edge, NULL, -1, -1);

	A3DInt32 iRet = A3DTopoEdgeGet(pEdge, &sData);
	if(iRet == A3D_SUCCESS)
	{
		_SetDoubleAttribute(edge, "m_dTolerance", sData.m_dTolerance);
		edge->SetAttribute("m_bHasTrimDomain", (int) sData.m_bHasTrimDomain);

		// Commented out
		// if(sData.m_p3dCurve)
		// 	traverseCurve(sData.m_p3dCurve, edge);
		if(sData.m_bHasTrimDomain)
			traverseInterval(&sData.m_sInterval, edge);

		if(sData.m_pStartVertex)
			sttraverseVertex(sData.m_pStartVertex, edge);
		if(sData.m_pEndVertex)
			sttraverseVertex(sData.m_pEndVertex, edge);

		A3DTopoEdgeGet(NULL, &sData);
	}
	else
	{
		edge->SetAttribute("error", iRet);
	}

	setting->LinkEndChild(edge);
	return iRet;
}

//######################################################################################################################
static int sttraverseCoEdge(const A3DTopoCoEdge* pCoEdge, TiXmlElement* setting)
{	
	A3DTopoCoEdgeData sData;
	A3D_INITIALIZE_DATA(A3DTopoCoEdgeData, sData);

	TiXmlElement* coedge = new TiXmlElement("A3DTopoCoEdgeData");
	traverseSource(pCoEdge, coedge, NULL, -1, -1);

	A3DInt32 iRet = A3DTopoCoEdgeGet(pCoEdge, &sData);
	if(iRet == A3D_SUCCESS)
	{
		if(sData.m_pEdge)
			sttraverseEdge(sData.m_pEdge, coedge);
		// Commented out
		// if(sData.m_pUVCurve)
		// 	traverseCurve(sData.m_pUVCurve, coedge);

		//_SetAttributePtr(coedge, "m_pNeighbor", sData.m_pNeighbor);
		coedge->SetAttribute("m_pNeighbor", sData.m_pNeighbor ? "true" : "false");
		coedge->SetAttribute("m_ucOrientationUVWithLoop", (int) sData.m_ucOrientationUVWithLoop);
		coedge->SetAttribute("m_ucOrientationWithLoop", (int) sData.m_ucOrientationWithLoop);
		A3DTopoCoEdgeGet(NULL, &sData);
	}
	else
	{
		coedge->SetAttribute("error", iRet);
	}

	setting->LinkEndChild(coedge);
	return iRet;
}

//######################################################################################################################
static int sttraverseLoop(const A3DTopoLoop* pLoop, TiXmlElement* setting)
{	
	A3DTopoLoopData sData;
	A3D_INITIALIZE_DATA(A3DTopoLoopData, sData);

	TiXmlElement* loop = new TiXmlElement("A3DTopoLoopData");
	traverseSource(pLoop, loop, NULL, -1, -1);

	A3DInt32 iRet = A3DTopoLoopGet(pLoop, &sData);
	if(iRet == A3D_SUCCESS)
	{
		loop->SetAttribute("m_ucOrientationWithSurface", (int)(sData.m_ucOrientationWithSurface));
		for(A3DUns32 ui = 0; ui < sData.m_uiCoEdgeSize; ++ui)
			iRet = sttraverseCoEdge(sData.m_ppCoEdges[ui], loop);
		A3DTopoLoopGet(NULL, &sData);
	}
	else
	{
		loop->SetAttribute("error", iRet);
	}

	setting->LinkEndChild(loop);
	return iRet;
}

//######################################################################################################################
static int sttraverseFace(const A3DTopoFace* pFace, TiXmlElement* setting)
{	
	A3DTopoFaceData sData;
	A3D_INITIALIZE_DATA(A3DTopoFaceData, sData);

	TiXmlElement* face = new TiXmlElement("A3DTopoFaceData");
	traverseSource(pFace, face, NULL, -1, -1);

	A3DInt32 iRet = A3DTopoFaceGet(pFace, &sData);
	if(iRet == A3D_SUCCESS)
	{
		face->SetAttribute("m_bHasTrimDomain", (int)(sData.m_bHasTrimDomain));
		face->SetAttribute("m_uiOuterLoopIndex", (int)(sData.m_uiOuterLoopIndex));
		_SetDoubleAttribute(face, "m_dTolerance", sData.m_dTolerance);

		traverseSurface(sData.m_pSurface, face);

		if(sData.m_bHasTrimDomain)
			traverseDomain(&sData.m_sSurfaceDomain, face);

		for(A3DUns32 ui = 0; ui < sData.m_uiLoopSize; ++ui)
			iRet = sttraverseLoop(sData.m_ppLoops[ui], face);

		A3DTopoFaceGet(NULL, &sData);
	}
	else
	{
		face->SetAttribute("error", iRet);
	}

	setting->LinkEndChild(face);
	return iRet;
}

//######################################################################################################################
static int sttraverseShell(const A3DTopoShell* pShell, TiXmlElement* setting)
{	
	A3DTopoShellData sData;
	A3D_INITIALIZE_DATA(A3DTopoShellData, sData);

	TiXmlElement* shell = new TiXmlElement("A3DTopoShellData");
	traverseSource(pShell, shell, NULL, -1, -1);

	A3DInt32 iRet = A3DTopoShellGet(pShell, &sData);
	if(iRet == A3D_SUCCESS)
	{
		shell->SetAttribute("closed", (int)(sData.m_bClosed));

		for(A3DUns32 ui = 0; ui < sData.m_uiFaceSize; ++ui)
			iRet = sttraverseFace(sData.m_ppFaces[ui], shell);

		traverseUChars("m_pucOrientationWithShell",sData.m_uiFaceSize,sData.m_pucOrientationWithShell,shell);

		A3DTopoShellGet(NULL, &sData);
	}
	else
	{
		shell->SetAttribute("error", iRet);
	}

	setting->LinkEndChild(shell);
	return iRet;
}

//######################################################################################################################
static int sttraverseConnex(const A3DTopoConnex* pConnex, TiXmlElement* setting)
{	
	A3DTopoConnexData sData;
	A3D_INITIALIZE_DATA(A3DTopoConnexData, sData);

	TiXmlElement* connex = new TiXmlElement("A3DTopoConnexData");
	traverseSource(pConnex, connex, NULL, -1, -1);

	A3DInt32 iRet = A3DTopoConnexGet(pConnex, &sData);
	if(iRet == A3D_SUCCESS)
	{
		for(A3DUns32 ui = 0; ui < sData.m_uiShellSize; ++ui)
			iRet = sttraverseShell(sData.m_ppShells[ui], connex);
		A3DTopoConnexGet(NULL, &sData);
	}
	else
	{
		connex->SetAttribute("error", iRet);
	}

	setting->LinkEndChild(connex);
	return iRet;
}

//######################################################################################################################
int traverseBrepData(const A3DTopoBrepData* pBrepData, TiXmlElement* setting)
{	
	A3DTopoBrepDataData sData;
	A3D_INITIALIZE_DATA(A3DTopoBrepDataData, sData);

	TiXmlElement* brepdata = new TiXmlElement("A3DTopoBrepDataData");
	traverseSource(pBrepData, brepdata, NULL, -1, -1);
	unsigned char ucBehavior;
	traverseBodyContent(pBrepData, brepdata,ucBehavior);

	A3DInt32 iRet = A3DTopoBrepDataGet(pBrepData, &sData);
	if(iRet == A3D_SUCCESS)
	{
		for(A3DUns32 ui = 0; ui < sData.m_uiConnexSize; ++ui)
			iRet = sttraverseConnex(sData.m_ppConnexes[ui], brepdata);

		if(ucBehavior !=0)
			iRet = traverseBoundingBox(&sData.m_sBoundingBox, brepdata);

		A3DTopoBrepDataGet(NULL, &sData);
	}
	else
	{
		brepdata->SetAttribute("error", iRet);
	}

	setting->LinkEndChild(brepdata);
	return iRet;
}

//######################################################################################################################
static int sttraverseWireEdge(const A3DTopoWireEdge* pWireEdge,	TiXmlElement* setting)
{	
	A3DTopoWireEdgeData sData;
	A3D_INITIALIZE_DATA(A3DTopoWireEdgeData, sData);

	TiXmlElement* wireedge = new TiXmlElement("A3DTopoWireEdgeData");
	traverseSource(pWireEdge, wireedge, NULL, -1, -1);

	A3DInt32 iRet = A3DTopoWireEdgeGet(pWireEdge, &sData);
	if(iRet == A3D_SUCCESS)
	{
		wireedge->SetAttribute("m_bHasTrimDomain", (int) sData.m_bHasTrimDomain);

		// Commented out
		// if(sData.m_p3dCurve)
		// 	traverseCurve(sData.m_p3dCurve, wireedge);

		if(sData.m_bHasTrimDomain)
			traverseInterval(&sData.m_sInterval, wireedge);

		A3DTopoWireEdgeGet(NULL, &sData);
	}
	else
	{
		wireedge->SetAttribute("error", iRet);
	}

	setting->LinkEndChild(wireedge);
	return iRet;
}

//######################################################################################################################
int traverseSingleWireBody(const A3DTopoSingleWireBody* pSingleWireBody, TiXmlElement* setting)
{	
	A3DTopoSingleWireBodyData sData;
	A3D_INITIALIZE_DATA(A3DTopoSingleWireBodyData, sData);

	TiXmlElement* singlewirebody = new TiXmlElement("A3DTopoSingleWireBodyData");
	traverseSource(pSingleWireBody, singlewirebody, NULL, -1, -1);
	unsigned char ucBehavior;
	traverseBodyContent(pSingleWireBody, singlewirebody, ucBehavior);

	A3DInt32 iRet = A3DTopoSingleWireBodyGet(pSingleWireBody, &sData);
	if(iRet == A3D_SUCCESS)
	{
		iRet = sttraverseWireEdge(sData.m_pWireEdge, singlewirebody);
		A3DTopoSingleWireBodyGet(NULL, &sData);
	}
	else
	{
		singlewirebody->SetAttribute("error", iRet);
	}

	setting->LinkEndChild(singlewirebody);
	return iRet;
}

//######################################################################################################################
int traverseBodyContent(const A3DTopoBody* pBody, TiXmlElement* setting, unsigned char &ucBehavior)
{
	A3DInt32 iErr = A3D_SUCCESS;
	A3DTopoBodyData sData;
	A3D_INITIALIZE_DATA(A3DTopoBodyData, sData);

	iErr = A3DTopoBodyGet(pBody, &sData);
	if(iErr == A3D_SUCCESS)
	{
		TiXmlElement* bodydata = new TiXmlElement("A3DTopoBodyData");

		ucBehavior = (unsigned char) sData.m_ucBehaviour;
		bodydata->SetAttribute("m_ucBehaviour", (int) sData.m_ucBehaviour);

		if(sData.m_pContext)
		{
			traverseTopoContext(sData.m_pContext, bodydata);
			setting->LinkEndChild(bodydata);
		}

		A3DTopoBodyGet(NULL, &sData);
	}

	return A3D_SUCCESS;
}

//######################################################################################################################
int traverseTopoContext(const A3DTopoContext* pContext, TiXmlElement* setting)
{
	A3DInt32 iErr = A3D_SUCCESS;
	A3DTopoContextData sData;
	A3D_INITIALIZE_DATA(A3DTopoContextData, sData);

	TiXmlElement* topocontext = new TiXmlElement("A3DTopoContextData");
	traverseSource(pContext, topocontext, NULL, -1, -1);

	iErr = A3DTopoContextGet(pContext, &sData);
	if(iErr == A3D_SUCCESS)
	{
		topocontext->SetAttribute("m_ucBehaviour", (int) sData.m_ucBehaviour);
		topocontext->SetAttribute("m_dGranularity", (int) sData.m_dGranularity);
		topocontext->SetAttribute("m_dTolerance", (int) sData.m_dTolerance);
		topocontext->SetAttribute("m_bHaveSmallestFaceThickness", (int) sData.m_bHaveSmallestFaceThickness);
		topocontext->SetAttribute("m_dSmallestThickness", (int) sData.m_dSmallestThickness);
		topocontext->SetAttribute("m_bHaveScale", (int) sData.m_bHaveScale);
		topocontext->SetAttribute("m_dScale", (int) sData.m_dScale);

		A3DTopoContextGet(NULL, &sData);
	}
	else
	{
		topocontext->SetAttribute("error", iErr);
	}

	setting->LinkEndChild(topocontext);
	return iErr;
}
