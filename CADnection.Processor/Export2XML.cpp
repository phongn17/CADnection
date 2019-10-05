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

#include <memory>
#include <new> // for exception-throwing new

#ifdef __ANDROID__
#	include <android/log.h>
#	include <dlfcn.h>

#	define INITIALIZE_A3D_API
#	include <A3DSDKIncludes.h>
#	undef INITIALIZE_A3D_API

#	define JNI_LOG_TAG "SAMPLE PRC 2 XML"
#	define LOGI(...) __android_log_print(ANDROID_LOG_INFO,  JNI_LOG_TAG, __VA_ARGS__)
#	define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, JNI_LOG_TAG, __VA_ARGS__)
#endif // __ANDROID__

#ifdef __APPLE__
#   include "TargetConditionals.h"
#   if TARGET_OS_IPHONE == 1 || TARGET_IPHONE_SIMULATOR == 1
#		define APPLE_IOS
#		include <A3DSDKIncludes.h>
#	endif
#endif

#include "Export2XML.h"

#ifndef _MAX_PATH
#	ifdef _MSC_VER
#		define _MAX_PATH 260
#	else
#		define _MAX_PATH 4096
#	endif
#endif

// Retrieve physical properties
A3DStatus RetrievePhysicalProperties(A3DAsmModelFile* pModelFile, TiXmlElement * treatment) {
	A3DPhysicalPropertiesData sPhysPropsData;
	A3D_INITIALIZE_DATA(A3DPhysicalPropertiesData, sPhysPropsData);
	CHECK_RET(A3DComputeModelFilePhysicalProperties(pModelFile, &sPhysPropsData));

	std::unique_ptr<TiXmlElement> physicalprops(new TiXmlElement("ComputePhysicalProps"));
	physicalprops->SetDoubleAttribute("m_sGravityCenter.m_dX", sPhysPropsData.m_sGravityCenter.m_dX);
	physicalprops->SetDoubleAttribute("m_sGravityCenter.m_dY", sPhysPropsData.m_sGravityCenter.m_dY);
	physicalprops->SetDoubleAttribute("m_sGravityCenter.m_dZ", sPhysPropsData.m_sGravityCenter.m_dZ);
	physicalprops->SetDoubleAttribute("m_dSurface", sPhysPropsData.m_dSurface);
	physicalprops->SetDoubleAttribute("m_bVolumeComputed", sPhysPropsData.m_bVolumeComputed);
	physicalprops->SetDoubleAttribute("m_dVolume", sPhysPropsData.m_dVolume);
	treatment->LinkEndChild(physicalprops.release());

	return A3D_SUCCESS;
}

void ProcessError(A3DStatus status) {
	switch (status) {
		case A3D_LOAD_INVALID_FILE_NAME:
			cout << "Input file does not correspond to a valid file name." << endl;
			break;
		case A3D_LOAD_CANNOT_ACCESS_CADFILE:
			cout << "Cannot access input CAD file." << endl;
			break;
		case A3D_LOAD_CANNOT_READ_SETTINGS:
			cout << "Cannot access CAD reading settings file." << endl;
			break;
		case A3D_LOAD_INVALID_FILE_FORMAT:
			cout << "Input file seems invalid." << endl;
			break;
		case A3D_LOAD_READER_NOT_IMPLEMENTED:
			cout << "The reader is not implemented." << endl;
			break;
		case A3D_LOAD_CANNOT_LOAD_MODEL:
			cout << "An error occurred during parsing." << endl;
			break;
		case A3D_LOAD_CANNOT_LOAD_MULTIENTRY:
			cout << "Cannot load multi-entry file." << endl;
			break;
		case A3D_LOAD_EMPTY_MULTI_MODEL:
			cout << "Empty model." << endl;
			break;
		case A3D_LOAD_INVALID_READER_FOR_INCREMENTAL_LOAD:
			cout << "Invalid reader for incremental load." << endl;
			break;
		case A3D_LOAD_INVALID_PARAMETERS_FOR_INCREMENTAL_LOAD:
			cout << "Invalid parameters for incremental load." << endl;
			break;
		case A3D_LOAD_CANNOT_USE_MULTIPROCESS:
			cout << "Error while launching multiprocess." << endl;
			break;
		case A3D_LOAD_FILE_TOO_OLD:
			cout << "The minimum version requirement is not satisfied for the input format." << endl;
			break;
		case A3D_LOAD_FILE_TOO_RECENT:
			cout << "The maximum version requirement is not satisfied for the input format." << endl;
			break;
	}
}

//######################################################################################################################
// reads the PRC file and dumps information into XML format
A3DStatus ReadFile(A3DSDKHOOPSExchangeLoader & sHoopsExchangeLoader, A3DImport & sImport, Export2Json& export2Json, ThumbnailConverter* thumbnailConverter, TiXmlElement * treatment) {
	try {
		A3DStatus iRet = sHoopsExchangeLoader.Import(sImport);
		if (iRet != A3D_SUCCESS && iRet != A3D_LOAD_MULTI_MODELS_CADFILE && iRet != A3D_LOAD_MISSING_COMPONENTS) {
			ProcessError(iRet);
			return iRet;
		}

		// Model file: There is only one model file per assembly tree. The model file structure contains references to the product occurrences as well as 
		// global information such as the unit scale and the modelling system used to create the file.
		A3DAsmModelFile* pModelFile = sHoopsExchangeLoader.m_psModelFile;

		// Retrieve physical properties
		iRet = RetrievePhysicalProperties(pModelFile, treatment);
		if (iRet != A3D_SUCCESS)
			return iRet;

		// Retrieve files path from model files
#ifndef APPLE_IOS // the following code triggers a SIGSTOP on iOS
		A3DUns32 nbFiles = 0, nbAssemblyFiles = 0, nbMissingFiles = 0;
		A3DUTF8Char** ppPaths = NULL, ** ppAssemblyPaths = NULL, ** ppMissingPaths = NULL;

		struct ModelFileGuard
		{
			A3DAsmModelFile*& m_psModelFile;
			explicit ModelFileGuard(A3DAsmModelFile*& pModelFile) : m_psModelFile(pModelFile) {}
			~ModelFileGuard()
			{
				A3DAsmModelFileDelete(m_psModelFile);
				m_psModelFile = NULL;
			}
		} const sGuard(sHoopsExchangeLoader.m_psModelFile);

#ifdef __ANDROID__
		try {
#endif
			CHECK_RET(A3DAsmGetFilesPathFromModelFile(sHoopsExchangeLoader.m_psModelFile, &nbFiles, &ppPaths, &nbAssemblyFiles, &ppAssemblyPaths, &nbMissingFiles, &ppMissingPaths));
#ifdef __ANDROID__
		}
		catch (std::exception const& e) {
			LOGE("%s\n", e.what());
			return iRet;
		}
#endif
		std::unique_ptr<TiXmlElement> allFilesPaths(new TiXmlElement("A3DAsmFilesPathFromModelFile"));
		allFilesPaths->SetAttribute("nbFiles", (int)nbFiles);
		allFilesPaths->SetAttribute("nbAssemblyFiles", (int)nbAssemblyFiles);
		allFilesPaths->SetAttribute("nbMissingFiles", (int)nbMissingFiles);

		for (A3DUns32 ui = 0; ui < nbFiles; ui++) {
			std::unique_ptr<TiXmlElement> filePaths(new TiXmlElement("Filepaths"));
			filePaths->SetAttribute("m_FilePaths", ppPaths[ui]);
			allFilesPaths->LinkEndChild(filePaths.release());
		}

		for (A3DUns32 uj = 0; uj < nbAssemblyFiles; uj++) {
			std::unique_ptr<TiXmlElement> assemblyFilePaths(new TiXmlElement("AssemblyFilePaths"));
			assemblyFilePaths->SetAttribute("m_AssemblyFilePaths", ppAssemblyPaths[uj]);
			allFilesPaths->LinkEndChild(assemblyFilePaths.release());
		}

		for (A3DUns32 uk = 0; uk < nbMissingFiles; uk++) {
			std::unique_ptr<TiXmlElement> missingFilePaths(new TiXmlElement("MissingFilePaths"));
			missingFilePaths->SetAttribute("m_nbMissingFiles", ppMissingPaths[uk]);
			allFilesPaths->LinkEndChild(missingFilePaths.release());
		}

		treatment->LinkEndChild(allFilesPaths.release());
#endif // !defined(APPLE_IOS)

		A3DGlobal* pGlobal = NULL;
		CHECK_RET(A3DGlobalGetPointer(&pGlobal));

		// Commented out
		// traverseGlobal(pGlobal, treatment);
		traverseModel(pModelFile, export2Json, thumbnailConverter, treatment);
		// traverseFonts(treatment);
		return iRet;
	}
	catch (exception ex) {
		cout << ex.what() << endl;
		return A3D_EXCEPTION;
	}
}

//######################################################################################################################
A3DVoid _InitializeFontsArray();

//######################################################################################################################
A3DVoid _TerminateFontsArray();

//######################################################################################################################
A3DStatus ProcessFile(
	A3DSDKHOOPSExchangeLoader& sHoopsExchangeLoader,
	A3DImport& sImport,
	const MY_CHAR* pcPRCFile,
	const MY_CHAR* pcXMLFile,
	Export2Json& export2Json, ThumbnailConverter* thumbnailConverter)
{
	A3DUTF8Char sPRCFileUTF8[_MAX_PATH];
	A3DUTF8Char sXMLFileUTF8[_MAX_PATH];

	A3DMiscUTF16ToUTF8(pcPRCFile, sPRCFileUTF8);

	std::unique_ptr<TiXmlDocument> doc;
	try
	{
		doc.reset(new TiXmlDocument);
		doc->SetCondenseWhiteSpace(false);

		std::unique_ptr<TiXmlComment> comment(new TiXmlComment("XML dump from PRC file"));
		doc->LinkEndChild(comment.release());

		A3DInt32 iMajorVersion = 0, iMinorVersion = 0;
		CHECK_RET(A3DDllGetVersion(&iMajorVersion, &iMinorVersion));

		std::unique_ptr<TiXmlElement> treatment(new TiXmlElement("TREATMENT"));
		treatment->SetAttribute("MajorVersion", iMajorVersion);
		treatment->SetAttribute("MinorVersion", iMinorVersion);
		treatment->SetAttribute("PRCFile", sPRCFileUTF8);

		_InitializeFontsArray();

		CHECK_RET(ReadFile(sHoopsExchangeLoader, sImport, export2Json, thumbnailConverter, treatment.get()));

		doc->LinkEndChild(treatment.release());

		if (pcXMLFile != NULL) {
#ifdef _MSC_VER
			A3DMiscUTF16ToUTF8(pcXMLFile, sXMLFileUTF8);
#else
			MY_STRCPY(sPRCFileUTF8, pcPRCFile);
			MY_STRCPY(sXMLFileUTF8, pcXMLFile);
#endif

			doc->SaveFile(sXMLFileUTF8);
		}

		_TerminateFontsArray();
		return A3D_SUCCESS;
	}
	catch (const std::bad_alloc&)
	{
		PrintLogError(NULL, (A3DUTF8Char*)"std::bad_alloc exception caught\n");
		if (doc.get())
			// try to save what has already been generated
			doc->SaveFile(sXMLFileUTF8);
		return A3D_ERROR;
	}
}
