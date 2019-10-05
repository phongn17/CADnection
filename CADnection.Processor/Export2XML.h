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

#ifndef __A3DTESTERREAD_H__
#define __A3DTESTERREAD_H__

#include <stdlib.h>

#ifdef WIN32
#	pragma warning (disable:4090)
#	define _CRT_SECURE_NO_DEPRECATE 1
#	ifdef _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES
#		undef _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES
#		define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 1
#	endif
#endif

#ifdef __APPLE__
#   include "TargetConditionals.h"
#   if TARGET_OS_IPHONE == 1 || TARGET_IPHONE_SIMULATOR == 1
#		include "tinyxml.h"
#		include "common.hpp"
#	else
#		include <tinyxml.h>
#		include <A3DSDKIncludes.h>
#		include "../common.hpp"
#	endif
#elif defined(__ANDROID__)
#	include <tinyxml.h>
#	include "common.hpp"
#else
#	include <tinyxml.h>
#	include <boost/filesystem.hpp>
#	include "Configuration.h"
#	include "ThumbnailConverter.h"
#	include "Export2Json.h"
#	include <A3DSDKIncludes.h>
#	include "../common.hpp"
#	include <boost/uuid/uuid.hpp>            // uuid class
#	include <boost/uuid/uuid_generators.hpp> // generators
#	include <boost/uuid/uuid_io.hpp>         // streaming operators
#	include <boost/lexical_cast.hpp>
#	include <boost/algorithm/string/replace.hpp>
#	include <algorithm>

#endif

//######################################################################################################################
A3DStatus ProcessFile(
	A3DSDKHOOPSExchangeLoader & sHoopsExchangeLoader,
	A3DImport & sImport,
	const MY_CHAR * pcPRCFile,
	const MY_CHAR * pcXMLFile, Export2Json& export2Json, ThumbnailConverter* thumbnailConverter);

//######################################################################################################################
void _SetAttributePtr(TiXmlElement* psElement, const A3DUTF8Char* pcAttribName, void* pValue);

//######################################################################################################################
void _SetDoubleAttribute(TiXmlElement* psElement, const char* name, double val);

//######################################################################################################################
int traverseDoubles(const A3DUTF8Char* name, const A3DUns32 uiSize, const A3DDouble* pd, TiXmlElement* setting);

//######################################################################################################################
int traverseUInts(const A3DUTF8Char* name, const A3DUns32 uiSize, const A3DUns32* pui, TiXmlElement* setting);

//######################################################################################################################
int traverseUChars(const A3DUTF8Char* name, const A3DUns32 uiSize, const A3DUns8* pui, TiXmlElement* setting);

//######################################################################################################################
int traverseBools(const A3DUTF8Char* name, const A3DUns32 uiSize, const A3DBool* pb, TiXmlElement* setting);

//######################################################################################################################
int traverseVoids(const A3DUTF8Char* name, const A3DUns32 uiSize, const A3DVoid** pp, TiXmlElement* setting);

//######################################################################################################################
int traversePoint2d(const A3DUTF8Char* name, const A3DVector2dData& sData, TiXmlElement* setting);

//######################################################################################################################
int traversePoint(const A3DUTF8Char* name, const A3DVector3dData& sData, TiXmlElement* setting);

//######################################################################################################################
int traversePoints(const A3DUTF8Char* name, const A3DUns32 uiSize, const A3DVector3dData* pv, TiXmlElement* setting);

//######################################################################################################################
int traverseInterval(const A3DIntervalData* pData, TiXmlElement* setting);

//######################################################################################################################
int traverseDomain(const A3DDomainData* pData, TiXmlElement* setting);

//######################################################################################################################
int traverseBoundingBox(const A3DBoundingBoxData* pData, TiXmlElement* setting);

//######################################################################################################################
int traverseParam(const A3DParameterizationData* pData, TiXmlElement* setting);

//######################################################################################################################
int traverseUVParam(const A3DUVParameterizationData* pData, TiXmlElement* setting);

//######################################################################################################################
A3DStatus traverseTransformation(const A3DMiscCartesianTransformation* pTrsf, TiXmlElement* setting);

//######################################################################################################################
A3DStatus traverseTransfo3d(const A3DMiscCartesianTransformationData& oTrsf, TiXmlElement* setting);

//######################################################################################################################
A3DStatus traverseTransfo3d(const A3DMiscCartesianTransformation* pTransfo3d, TiXmlElement* setting);

//######################################################################################################################
int traverseSource(const A3DEntity* pEntity, TiXmlElement* setting, json* json, int index, int parentIndex, int pmiIndex = -1);

//######################################################################################################################
int traverseGlobal(const A3DGlobal* pGlobal, TiXmlElement* setting);

//######################################################################################################################
int traverseModel(const A3DAsmModelFile* pModelFile, Export2Json& export2Json, ThumbnailConverter* thumbnailConverter, TiXmlElement* setting);

//######################################################################################################################
int traverseFonts(TiXmlElement* setting);

//######################################################################################################################
int traverseCurve(const A3DCrvBase* pCrv, TiXmlElement* setting);

//######################################################################################################################
int traverseSurface(const A3DSurfBase* pSrf, TiXmlElement* setting);

//######################################################################################################################
int traverseSrfPlane(const A3DSurfPlane* pSrf, TiXmlElement* setting);

//######################################################################################################################
int traverseAnnotation(const A3DMkpAnnotationEntity* pAnnot, TiXmlElement* setting, json* jsonData, int index, int pmiIndex = -1);

//######################################################################################################################
int traverseMarkup(const A3DMkpMarkup* pMarkup, TiXmlElement* setting, json* jsonData, int index, int pmiIndex);

//######################################################################################################################
int traverseLeader(const A3DMkpLeader* pLeader, TiXmlElement* setting);

//######################################################################################################################
int traverseLinkedItem(const A3DMiscMarkupLinkedItem* pMkpLinkedItem, TiXmlElement* setting);

//######################################################################################################################
int traverseEntityReference(const A3DMiscEntityReference* pEntityReference, TiXmlElement* setting);

//######################################################################################################################
int traverseSingleWireBody(const A3DTopoSingleWireBody* pSingleWireBody, TiXmlElement* setting);

//######################################################################################################################
int traverseTopoContext(const A3DTopoContext* pContext, TiXmlElement* setting);

//######################################################################################################################
int traverseBodyContent(const A3DTopoBody* pBody, TiXmlElement* setting, unsigned char& ucBehav);

//######################################################################################################################
int traverseBrepData(const A3DTopoBrepData* pBrepData, TiXmlElement* setting);

//######################################################################################################################
int traverseTessBase(const A3DTessBase* pTess, TiXmlElement* setting);

//######################################################################################################################
int traverseView(const A3DMkpView* pView, TiXmlElement* setting);

//######################################################################################################################
int traverseRepItem(const A3DRiRepresentationItem* pRepItem, TiXmlElement* setting);

//######################################################################################################################
int traverseCSys(const A3DRiCoordinateSystem* pCSys, TiXmlElement* setting);

//######################################################################################################################
int traverseTextureApplication(const A3DGraphTextureApplicationData& sData, TiXmlElement* setting);

//######################################################################################################################
int traverseTextureDefinition(const A3DGraphTextureDefinitionData& sData, TiXmlElement* setting);

//######################################################################################################################
int traverseMarkupTess(const A3DTessMarkup* pMarkupTess, TiXmlElement* setting);

//######################################################################################################################
int traverseMarkupDefinition(const A3DMkpMarkup* pMarkup, TiXmlElement* setting, json* jsonData, int index, int pmiIndex);

//######################################################################################################################
int traverseMarkupText(const A3DMarkupText* pA3DMarkup, TiXmlElement* setting, json* jsonData, int index, int pmiIndex);

//######################################################################################################################
int traverseMarkupRichText(const A3DMarkupRichText* pA3DMarkup, TiXmlElement* setting, json* jsonData, int index, int pmiIndex);

//######################################################################################################################
int traverseMarkupDatum(const A3DMarkupDatum* pA3DMarkup, TiXmlElement* setting, json* jsonData, int index, int pmiIndex);

//######################################################################################################################
int traverseMarkupTextProperties(const A3DMDTextProperties* pA3DTextProperties, TiXmlElement* setting, json* jsonData, int index, int pmiIndex);

//######################################################################################################################
int traverseMarkupGDT(const A3DMarkupGDT* psMarkup, TiXmlElement* setting, json* jsonData, int index, int pmiIndex);

//######################################################################################################################
int traverseMarkupDimension(const A3DMarkupDimension* pMarkup, TiXmlElement* setting, json* jsonData, int index, int pmiIndex);

//######################################################################################################################
int traverseDrawing(const A3DDrawingModel* pDrawing, TiXmlElement* setting, json* jsonData, int index);

//######################################################################################################################
int traverseMaterial(const A3DGraphMaterialData& sData, TiXmlElement* setting);

//######################################################################################################################
int traverseMaterialProperties(const A3DEntity* pEntity, TiXmlElement* xmlfather, json* jsonData, int index);

//######################################################################################################################
A3DVoid traverseFeatureTree(const A3DFRMFeatureTree* pFeatFeatureTree, TiXmlElement* xmlfather);

//######################################################################################################################
int traverseToleranceSizeValue(const A3DMDToleranceSizeValue* pToleranceSizeValue, TiXmlElement* setting, json* jsonData, int index, int pmiIndex);
#endif	/*	__A3DTESTERREAD_H__ */
