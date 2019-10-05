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
#define READMAXBIGSTRING 65536
A3DUTF8Char gbigstring[READMAXBIGSTRING]; // 64 kB buffer is OK to store big strings like array of doubles
A3DUns32 gnAllocatedStrings = 0;

//######################################################################################################################
void _SetDoubleAttribute(TiXmlElement* psElement, const char* name, double val)
{	
	// max size of one double: "%.15 " 13+1+15+1=30
	A3DUTF8Char* pc = gbigstring;

	size_t iwritten = sprintf(pc, "%.15f", val);
	for(size_t i = iwritten - 1; i > 0; i--)
	{
		if((pc)[i] == '0')
			(pc)[i] = 0;
		else
			break;
	}

	psElement->SetAttribute(name, pc);
}

//######################################################################################################################
const TiXmlElement* _FirstChildByAttribute(TiXmlElement* psElement, const A3DUTF8Char* pcAttribName, void* pValue)
{
	A3DUTF8Char buffer[64];
	sprintf(buffer, "%p", pValue);

	const TiXmlElement* child = psElement->FirstChildElement();
	for(; child; child = child->NextSiblingElement())
	{
		const A3DUTF8Char* pcValue = child->Attribute(pcAttribName);
		if(pcValue != NULL && strcmp(pcValue, buffer) == 0)
			return child;
	}
	return NULL;
}

//######################################################################################################################
void _SetAttributePtr(TiXmlElement* psElement, const A3DUTF8Char* pcAttribName, void* pValue)
{
	A3DUTF8Char buffer[64];
	sprintf(buffer, "%p", pValue);
	psElement->SetAttribute(pcAttribName, buffer);
}

//######################################################################################################################
int traverseDoubles(const A3DUTF8Char* name, const A3DUns32 uiSize, const A3DDouble* pd, TiXmlElement* setting)
{
	if(uiSize > 0)
	{
		A3DUns32 ui = 0;
		TiXmlElement* doubles = new TiXmlElement(name);

		doubles->SetAttribute("size", (int)uiSize);

		// max size of one double: "%.15 " 13+1+15+1=30
		A3DUns32 uiFormat = 30;
		A3DUTF8Char* pc = NULL;
		if(uiFormat*uiSize > READMAXBIGSTRING)
		{
			pc = (A3DUTF8Char*) malloc(size_t(uiSize * uiFormat) * sizeof(A3DUTF8Char));
			pc[0] = 0;
			gnAllocatedStrings++;
		}
		else 
			pc = gbigstring;

		size_t uiNext = 0;
		for(ui = 0; ui < uiSize; ++ui)
		{
			int iwritten = sprintf(pc+uiNext, "%.15f", pd[ui]);
			size_t i = size_t(iwritten - 1);
			for(; i > 0; i--)
			{
				if((pc+uiNext)[i] == '0')
					(pc+uiNext)[i] = 0;
				else
					break;
			}
			(pc+uiNext)[i+1] = L' ';
			uiNext += i+2;
		}
		pc[uiNext-1] = 0;

		doubles->SetAttribute("data", pc);
		setting->LinkEndChild(doubles);

		if(uiFormat*uiSize > READMAXBIGSTRING)
			free(pc);
	}
	return A3D_SUCCESS;
}

//######################################################################################################################
int traverseUInts(const A3DUTF8Char* name, const A3DUns32 uiSize, const A3DUns32* pui, TiXmlElement* setting)
{
	if(uiSize > 0)
	{
		A3DUns32 ui = 0;
		TiXmlElement* uints = new TiXmlElement(name);
		uints->SetAttribute("size", (int)uiSize);

		// max size of one uint: "%d " 14+1=15
		A3DUns32 uiFormat = 15;
		A3DUTF8Char* pc = NULL;
		if(uiFormat*uiSize > READMAXBIGSTRING)
		{
			pc = (A3DUTF8Char*) malloc(size_t(uiSize * uiFormat) * sizeof(A3DUTF8Char));
			pc[0] = 0;
			gnAllocatedStrings++;
		}
		else 
			pc = gbigstring;

		size_t uiNext = 0;
		for(ui = 0; ui < uiSize; ++ui)
		{
			int iwritten = sprintf(pc+uiNext, "%u ", pui[ui]);
			uiNext += size_t(iwritten);
		}
		pc[uiNext] = 0;

		uints->SetAttribute("data", pc);
		setting->LinkEndChild(uints);

		if(uiFormat*uiSize > READMAXBIGSTRING)
			free(pc);
	}
	return A3D_SUCCESS;
}

//######################################################################################################################
int traverseUChars(const A3DUTF8Char* name, const A3DUns32 uiSize, const A3DUns8* pui, TiXmlElement* setting)
{
	if(uiSize > 0)
	{
		A3DUns32 ui = 0;
		TiXmlElement* uints = new TiXmlElement(name);
		uints->SetAttribute("size", (int)uiSize);

		// max size of one uchar: "%d ": 6
		A3DUns32 uiFormat = 6;
		A3DUTF8Char* pc = NULL;
		if(uiFormat*uiSize > READMAXBIGSTRING)
		{
			pc = (A3DUTF8Char*) malloc(size_t(uiSize * uiFormat) * sizeof(A3DUTF8Char));
			pc[0] = 0;
			gnAllocatedStrings++;
		}
		else 
			pc = gbigstring;

		size_t uiNext = 0;
		for(ui = 0; ui < uiSize; ++ui)
		{
			int iwritten = sprintf(pc+uiNext, "%u ", pui[ui]);
			uiNext += size_t(iwritten);
		}
		pc[uiNext] = 0;

		uints->SetAttribute("data", pc);
		setting->LinkEndChild(uints);

		if(uiFormat*uiSize > READMAXBIGSTRING)
			free(pc);
	}
	return A3D_SUCCESS;
}

//######################################################################################################################
int traverseBools(const A3DUTF8Char* name, const A3DUns32 uiSize, const A3DBool* pb, TiXmlElement* setting)
{
	if(uiSize > 0)
	{
		A3DUns32 ui = 0;
		TiXmlElement* bools = new TiXmlElement(name);
		bools->SetAttribute("size", (int) uiSize);

		// max size of one bool: 2
		A3DUns32 uiFormat = 2;
		A3DUTF8Char* pc = NULL;
		if(uiFormat*uiSize > READMAXBIGSTRING)
		{
			pc = (A3DUTF8Char*) malloc(size_t(uiSize * uiFormat) * sizeof(A3DUTF8Char));
			pc[0] = 0;
			gnAllocatedStrings++;
		}
		else 
			pc = gbigstring;

		size_t uiNext = 0;
		for(ui = 0; ui < uiSize; ++ui)
		{
			int iwritten = sprintf(pc+uiNext, "%u ", pb[ui] ? 1 : 0);
			uiNext += size_t(iwritten);
		}
		pc[uiNext] = 0;

		bools->SetAttribute("data", pc);
		setting->LinkEndChild(bools);

		if(uiFormat*uiSize > READMAXBIGSTRING)
			free(pc);
	}
	return A3D_SUCCESS;
}

//######################################################################################################################
int traverseVoids(const A3DUTF8Char* name, const A3DUns32 uiSize, const A3DVoid** pp, TiXmlElement* setting)
{
	if(uiSize > 0)
	{
		A3DUns32 ui = 0;
		TiXmlElement* uints = new TiXmlElement(name);
		uints->SetAttribute("size", (int)uiSize);

		// max size of one void: "%p ":8
		A3DUns32 uiFormat = 8;
		A3DUTF8Char* pc = NULL;
		if(uiFormat*uiSize > READMAXBIGSTRING)
		{
			pc = (A3DUTF8Char*) malloc(size_t(uiSize * uiFormat) * sizeof(A3DUTF8Char));
			pc[0] = 0;
			gnAllocatedStrings++;
		}
		else 
			pc = gbigstring;

		size_t uiNext = 0;
		for(ui = 0; ui < uiSize; ++ui)
		{
			const A3DVoid* p = pp[ui];
			int iwritten = sprintf(pc+uiNext, "%p ", (void*) p);
			uiNext += size_t(iwritten);
		}
		pc[uiNext]=0;

		uints->SetAttribute("data", pc);
		setting->LinkEndChild(uints);

		if(uiFormat*uiSize > READMAXBIGSTRING)
			free(pc);
	}
	return A3D_SUCCESS;
}

//######################################################################################################################
int traversePoint(const A3DUTF8Char* name, const A3DVector3dData& sData, TiXmlElement* setting)
{
	TiXmlElement* pt = new TiXmlElement(name);
	_SetDoubleAttribute(pt, "m_dX", sData.m_dX);
	_SetDoubleAttribute(pt, "m_dY", sData.m_dY);
	_SetDoubleAttribute(pt, "m_dZ", sData.m_dZ);
	setting->LinkEndChild(pt);
	return A3D_SUCCESS;
}

//######################################################################################################################
int traversePoint2d(const A3DUTF8Char* name, const A3DVector2dData& sData, TiXmlElement* setting)
{
	TiXmlElement* pt = new TiXmlElement(name);
	_SetDoubleAttribute(pt, "m_dX", sData.m_dX);
	_SetDoubleAttribute(pt, "m_dY", sData.m_dY);
	setting->LinkEndChild(pt);
	return A3D_SUCCESS;
}

//######################################################################################################################
int traversePoints(const A3DUTF8Char* name, const A3DUns32 uiSize, const A3DVector3dData* pv, TiXmlElement* setting)
{
	if(uiSize > 0)
	{
		A3DDouble* pd = (A3DDouble*) malloc(size_t(uiSize) * 3 * sizeof(A3DDouble));
		if(pd == NULL)
			return A3D_ALLOC_FATAL_ERROR;

		for(A3DUns32 ui = 0; ui < uiSize; ++ui)
		{
			pd[3*ui] = pv[ui].m_dX;
			pd[3*ui+1] = pv[ui].m_dY;
			pd[3*ui+2] = pv[ui].m_dZ;
		}
		traverseDoubles(name, 3*uiSize, pd, setting);
		free(pd);
	}
	return A3D_SUCCESS;
}

//######################################################################################################################
int traverseInterval(const A3DIntervalData* pData, TiXmlElement* setting)
{
	TiXmlElement* extent1d = new TiXmlElement("A3DIntervalData");
	_SetDoubleAttribute(extent1d, "m_dMin", pData->m_dMin);
	_SetDoubleAttribute(extent1d, "m_dMax", pData->m_dMax);
	setting->LinkEndChild(extent1d);
	return A3D_SUCCESS;
}

//######################################################################################################################
int traverseDomain(const A3DDomainData* pData, TiXmlElement* setting)
{
	TiXmlElement* extent2d = new TiXmlElement("A3DDomainData");
	_SetDoubleAttribute(extent2d, "m_sMin.m_dX", pData->m_sMin.m_dX);
	_SetDoubleAttribute(extent2d, "m_sMin.m_dY", pData->m_sMin.m_dY);
	_SetDoubleAttribute(extent2d, "m_sMax.m_dX", pData->m_sMax.m_dX);
	_SetDoubleAttribute(extent2d, "m_sMax.m_dY", pData->m_sMax.m_dY);
	setting->LinkEndChild(extent2d);
	return A3D_SUCCESS;
}

//######################################################################################################################
int traverseBoundingBox(const A3DBoundingBoxData* pData, TiXmlElement* setting)
{
	TiXmlElement* extent3d = new TiXmlElement("A3DBoundingBoxData");
	_SetDoubleAttribute(extent3d, "m_sMin.m_dX", pData->m_sMin.m_dX);
	_SetDoubleAttribute(extent3d, "m_sMin.m_dY", pData->m_sMin.m_dY);
	_SetDoubleAttribute(extent3d, "m_sMin.m_dZ", pData->m_sMin.m_dZ);
	_SetDoubleAttribute(extent3d, "m_sMax.m_dX", pData->m_sMax.m_dX);
	_SetDoubleAttribute(extent3d, "m_sMax.m_dY", pData->m_sMax.m_dY);
	_SetDoubleAttribute(extent3d, "m_sMax.m_dZ", pData->m_sMax.m_dZ);
	setting->LinkEndChild(extent3d);
	return A3D_SUCCESS;
}

//######################################################################################################################
int traverseParam(const A3DParameterizationData* pData, TiXmlElement* setting)
{
	TiXmlElement* param = new TiXmlElement("A3DParameterizationData");
	_SetDoubleAttribute(param, "m_sInterval.m_dMin", pData->m_sInterval.m_dMin);
	_SetDoubleAttribute(param, "m_sInterval.m_dMax", pData->m_sInterval.m_dMax);
	_SetDoubleAttribute(param, "m_dCoeffA", pData->m_dCoeffA);
	_SetDoubleAttribute(param, "m_dCoeffB", pData->m_dCoeffB);
	setting->LinkEndChild(param);
	return A3D_SUCCESS;
}

//######################################################################################################################
int traverseUVParam(const A3DUVParameterizationData* pData, TiXmlElement* setting)
{
	TiXmlElement* param = new TiXmlElement("A3DUVParameterizationData");
	traverseDomain(&pData->m_sUVDomain, param);
	_SetDoubleAttribute(param, "m_dUCoeffA", pData->m_dUCoeffA);
	_SetDoubleAttribute(param, "m_dUCoeffB", pData->m_dUCoeffB);
	_SetDoubleAttribute(param, "m_dVCoeffA", pData->m_dVCoeffA);
	_SetDoubleAttribute(param, "m_dVCoeffB", pData->m_dVCoeffB);
	_SetDoubleAttribute(param, "m_bSwapUV", (int)pData->m_bSwapUV);
	setting->LinkEndChild(param);
	return A3D_SUCCESS;
}

//######################################################################################################################
A3DStatus traverseTransfo3d(const A3DMiscCartesianTransformationData& oTrsf, TiXmlElement* setting)
{
	TiXmlElement* trsf = new TiXmlElement("A3DMiscCartesianTransformationData");
	trsf->SetAttribute("m_ucBehaviour", (int)(oTrsf.m_ucBehaviour));
	traversePoint("m_sOrigin" , oTrsf.m_sOrigin , trsf);
	traversePoint("m_sXVector", oTrsf.m_sXVector, trsf);
	traversePoint("m_sYVector", oTrsf.m_sYVector, trsf);
	traversePoint("m_sScale"  , oTrsf.m_sScale  , trsf);
	setting->LinkEndChild(trsf);
	return A3D_SUCCESS;
}


//######################################################################################################################
A3DStatus traverseTransfo3d(const A3DMiscCartesianTransformation* pTrsf, TiXmlElement* setting)
{
	if (pTrsf)
	{
		A3DMiscCartesianTransformationData sData;
		A3D_INITIALIZE_DATA(A3DMiscCartesianTransformationData, sData);
		A3DStatus iRet = A3DMiscCartesianTransformationGet(pTrsf, &sData);
		if (iRet == A3D_SUCCESS)
		{
			traverseTransfo3d(sData, setting);
			A3DMiscCartesianTransformationGet(NULL, &sData);
		}
		else
		{
			setting->SetAttribute("error", iRet);
		}
	}
	return A3D_SUCCESS;
}

//######################################################################################################################
A3DStatus traverseGeneralTransformation(const A3DMiscGeneralTransformation* pTrsf, TiXmlElement* setting)
{
	TiXmlElement* trsf = new TiXmlElement("A3DMiscGeneralTransformationData");
	A3DMiscGeneralTransformationData sData;
	A3D_INITIALIZE_DATA(A3DMiscGeneralTransformationData, sData);
	A3DStatus iRet = A3DMiscGeneralTransformationGet(pTrsf, &sData);
	if (iRet == A3D_SUCCESS)
	{
		for (int i = 0; i < 16; ++i)
		{
			A3DUTF8Char indexBuffer[33];
			sprintf(indexBuffer, "%d", i);
			_SetDoubleAttribute(setting, indexBuffer, sData.m_adCoeff[i]);
		}

		A3DMiscGeneralTransformationGet(NULL, &sData);
	}
	else
	{
		trsf->SetAttribute("error", iRet);
	}
	setting->LinkEndChild(trsf);
	return A3D_SUCCESS;
}

//######################################################################################################################
A3DStatus traverseTransformation(const A3DMiscCartesianTransformation* pTransfo3d, TiXmlElement* setting)
{	
	if(pTransfo3d == NULL)
		return A3D_SUCCESS;
		
	A3DStatus iRet;
	TiXmlElement* trsf = new TiXmlElement("A3DMiscTransformation");
	traverseSource(pTransfo3d, trsf, NULL, -1, -1);
	A3DEEntityType eType = kA3DTypeUnknown;
	iRet = A3DEntityGetType(pTransfo3d, &eType);
	if (eType == kA3DTypeMiscCartesianTransformation)
	{
		// Commented out
		// iRet = traverseTransfo3d(pTransfo3d, trsf);
	}
	else if (eType == kA3DTypeMiscGeneralTransformation)
	{
		iRet = traverseGeneralTransformation(pTransfo3d, trsf);
	}
	else
	{
		trsf->SetAttribute("error", iRet);
	}

	setting->LinkEndChild(trsf);
	return iRet;
}

