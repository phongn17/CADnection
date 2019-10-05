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
int traverseMarkupDefinition(const A3DMkpMarkup* pMarkup, TiXmlElement* setting, json* jsonData, int index, int pmiIndex)
{
	A3DMarkupDefinitionData sData;
	A3D_INITIALIZE_DATA(A3DMarkupDefinitionData, sData);

	A3DEEntityType eType = kA3DTypeUnknown;
	A3DStatus iRet = A3DEntityGetType(pMarkup, &eType);
	

	TiXmlElement* markup = NULL;
	iRet = A3DMarkupDefinitionGet((A3DMarkupDefinition*)pMarkup, &sData);
	if(iRet != A3D_SUCCESS)
	{
		markup->SetAttribute("error", iRet);
		return iRet;
	}

	A3DMDPosition2DData sPosition;
	A3DVector2dData sOffsetAnchorPoint;
	A3DBool bIsScreenLocation=FALSE;
	A3D_INITIALIZE_DATA(A3DMDPosition2DData, sPosition);
	A3D_INITIALIZE_DATA(A3DVector2dData, sOffsetAnchorPoint);
	A3DMarkupPositionIsScreenLocation((A3DMarkupDefinition*)pMarkup,
		&bIsScreenLocation,
		&sPosition,
		&sOffsetAnchorPoint);
	if(bIsScreenLocation)
	{
		markup = new TiXmlElement("A3DMkpMarkupDefinitionData");
		setting->LinkEndChild(markup);
		markup->SetAttribute("PositionIsScreenLocation", bIsScreenLocation);
		markup->SetDoubleAttribute("position2Dx ", sPosition.m_sPosition.m_dX);
		markup->SetDoubleAttribute("position2Dy ", sPosition.m_sPosition.m_dY);
		markup->SetDoubleAttribute("offsetAnchorPoint2Dx ", sOffsetAnchorPoint.m_dX);
		markup->SetDoubleAttribute("offsetAnchorPoint2Dy ", sOffsetAnchorPoint.m_dY);
	}
	A3DMarkupDefinitionGet(NULL, &sData);

	if(eType == kA3DTypeMkpMarkup)
	{
		setting->SetAttribute("data", "no definition");
		return A3D_SUCCESS;
	}

	if(!bIsScreenLocation)
	{
		markup = new TiXmlElement("A3DMkpMarkupDefinitionData");
		setting->LinkEndChild(markup);
	}

	if(eType == kA3DTypeMarkupText)
	{
		if (pmiIndex != -1)
			(*jsonData)["docs"][index]["PMI"][pmiIndex]["PMIType"] = "Text";
		return traverseMarkupText(pMarkup, markup, jsonData, index, pmiIndex);
	}
	else if(eType == kA3DTypeMarkupRichText)
	{
		if (pmiIndex != -1)
			(*jsonData)["docs"][index]["PMI"][pmiIndex]["PMIType"] = "Rich Text";
		return traverseMarkupRichText(pMarkup, markup, jsonData, index, pmiIndex);
	}
	else if(eType == kA3DTypeMarkupDatum)
	{
		if (pmiIndex != -1)
			(*jsonData)["docs"][index]["PMI"][pmiIndex]["PMIType"] = "Datum";
		return traverseMarkupDatum(pMarkup, markup, jsonData, index, pmiIndex);
	}
	else if(eType == kA3DTypeMarkupGDT)
	{
		if (pmiIndex != -1)
			(*jsonData)["docs"][index]["PMI"][pmiIndex]["PMIType"] = "GDT";
		return traverseMarkupGDT(pMarkup, markup, jsonData, index, pmiIndex);
	}
	else if(eType == kA3DTypeMarkupDimension)
	{
		if (pmiIndex != -1)
			(*jsonData)["docs"][index]["PMI"][pmiIndex]["PMIType"] = "Dimension";
		return traverseMarkupDimension(pMarkup, markup, jsonData, index, pmiIndex);
	}
	else
	{
		markup->SetAttribute("specific_data", "not yet implemented");
		return A3D_NOT_IMPLEMENTED;
	}
}
