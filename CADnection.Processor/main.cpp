// CADnection.Processor.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#define INITIALIZE_A3D_API
#include "Export2XML.h"
#include <vector>
#include <string>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string.hpp>

#pragma warning(disable : 4267)			// warning occurs in cenv.hpp
#include <nowide/cenv.hpp>
using namespace nowide;
using namespace boost::filesystem;

void Size2String(long size, char* fileSize) {
	memset((void*)fileSize, 0, 50);
	long sz = size == 1024 ? 1 : size / 1024 + 1;
	if (sz >= 1024) {
		long mb = sz == 1024 ? 1 : sz / 1024 + 1;
		if (mb >= 1024) {
			sprintf(fileSize, "%d GB", mb == 1024 ? 1 : mb / 1024 + 1);
		}
		else {
			sprintf(fileSize, "%d MB", mb);
		}
	}
	else {
		sprintf(fileSize, "%d KB", sz);
	}
}

void Time2String(std::time_t t, char* modifiedTime) {
	std::tm* ptm = std::localtime(&t);
	memset((void*)modifiedTime, 0, 50);
	std::strftime(modifiedTime, 32, "%m/%d/%Y %H:%M:%S", ptm);
}

A3DStatus GenerateFiles(A3DSDKHOOPSExchangeLoader& sHoopsExchangeLoader, const MY_CHAR* acSrcFileName, bool isGenerateXml, ThumbnailConverter* thumbnailConverter) {
	MY_CHAR acXmlFileName[_MAX_PATH * 2];
	A3DUTF8Char acJsonFileName[_MAX_PATH * 2];
	A3DUTF8Char acSCFileName[_MAX_PATH * 2];
	A3DUTF8Char acPNGFileName[_MAX_PATH * 2];
	A3DUTF8Char fileSize[50];
	A3DUTF8Char modifiedTime[50];

	string filename = narrow(acSrcFileName);
	string filenameNoExt = filename;
	std::string ext = boost::filesystem::extension(filenameNoExt);
	boost::algorithm::replace_last(filenameNoExt, ext, "");
	if (isGenerateXml) {
		MY_SPRINTF(acXmlFileName, "%s.xml", filenameNoExt.c_str());
	}

	size_t idx = filenameNoExt.rfind('/');
	string filenameOnly = (idx != std::string::npos) ? filenameNoExt.substr(idx + 1) : filenameNoExt;
	const char* jsonFolder = thumbnailConverter->GetJsonFolder();
	sprintf(acJsonFileName, "%s%s.json", jsonFolder, filenameOnly.c_str());

	if (!thumbnailConverter->IsReprocessed() && boost::filesystem::exists(acJsonFileName)) {
		cout << filename << " already processed" << endl;
		return A3D_SUCCESS;
	}

	Export2Json export2Json;
	export2Json.SetOutput(acJsonFileName);

	try {
		A3DImport sImport(acSrcFileName); // see A3DSDKInternalConvert.hxx for import and export detailed parameters

		cout << "Processing " << filename << endl;

		A3DStatus iRet = ProcessFile(sHoopsExchangeLoader, sImport, acSrcFileName, isGenerateXml ? acXmlFileName : NULL, export2Json, thumbnailConverter);
		if (iRet != A3D_SUCCESS && iRet != A3D_LOAD_MULTI_MODELS_CADFILE && iRet != A3D_LOAD_MISSING_COMPONENTS) {
			return iRet;
		}

		json* jsonData = export2Json.GetData();

		struct stat stat_buf;
		int rc = stat(filename.c_str(), &stat_buf);
		if (rc == 0) {
			Size2String(stat_buf.st_size, fileSize);
			(*jsonData)["docs"][0]["Size"] = fileSize;
			std::time_t t = boost::filesystem::last_write_time(filename.c_str());
			Time2String(t, modifiedTime);
			(*jsonData)["docs"][0]["DateModified"] = modifiedTime;
		}

		std::replace(filenameNoExt.begin(), filenameNoExt.end(), '\\', '/');
		std::string id = (*jsonData)["docs"][0]["Id"].get<std::string>();
		if (id.length() > 0) {
			const char* generatedDataFolder = thumbnailConverter->GetGeneratedDataFolder();
			//size_t idx = filenameNoExt.rfind('/');
			//if (idx != std::string::npos)
			//	filenameNoExt = filenameNoExt.substr(0, idx + 1);
			sprintf(acPNGFileName, "%s%s.png", generatedDataFolder, id.c_str());
			sprintf(acSCFileName, "%s%s.scs", generatedDataFolder, id.c_str());
			if (!boost::filesystem::exists(acPNGFileName)) {
				(*jsonData)["docs"][0]["FileName"] = filename.c_str();
				(*jsonData)["docs"][0]["FileExtension"] = ext.substr(1).c_str();
				boost::algorithm::to_lower(ext);
				map<string, string> extMappings = thumbnailConverter->GetExtMappings();
				map<string, string>::iterator it = extMappings.find(ext);
				if (it != extMappings.end()) {
					(*jsonData)["docs"][0]["Application"] = it->second;
				}
				bool ret = true;
				if (thumbnailConverter->IsGenerateThumbnail()) {
					ret = thumbnailConverter->Execute(filename.c_str(), acPNGFileName, acSCFileName);
				}
				if (ret) {
					//const char* dataFolder = thumbnailConverter->GetDataFolder();
					const char* rootPath = thumbnailConverter->GetRootPath();
					const char* dataServer = thumbnailConverter->GetDataServer();
					const char* author = thumbnailConverter->GetAuthor();
					const char* scPrefix = thumbnailConverter->GetSCPrefix();
					//boost::algorithm::replace_last(filenameNoExt, dataFolder, dataServer);
					sprintf(acPNGFileName, "%s%s.png", rootPath, id.c_str());
					sprintf(acSCFileName, "%s%s%s.scs", scPrefix, rootPath, id.c_str());
					(*jsonData)["docs"][0]["Thumbnail"] = acPNGFileName;
					(*jsonData)["docs"][0]["StreamCache"] = acSCFileName;
					(*jsonData)["docs"][0]["Author"] = author;
				}
				else {
					cout << "Thumbnail conversion is failed for the file: " << acPNGFileName << endl;
				}
			}
		}

		//
		// ### Export to JSON file
		//
		export2Json.Save();
		
		cout << "Succeeded processing " << filename << endl;
	}
	catch (const exception& ex) {
		cout << ex.what() << endl;
		return A3D_ERROR;
	}

	// Check memory allocations
	//if (siCheckMallocCounter != 0)
	//	fprintf(GetLogFile(), "stiMallocCounter=%d", siCheckMallocCounter);

	return A3D_SUCCESS;
}

void ProcessFolder(const char* folder, A3DSDKHOOPSExchangeLoader& sHoopsExchangeLoader, bool isGenerateXml, ThumbnailConverter* thumbnailConverter) {
	std::vector<wstring> folders;
	std::locale loc;
	try {
		assert(is_directory(folder));
		A3DStatus ret;
		vector<string>& supportedExts = thumbnailConverter->GetSupportedExts();
		for (directory_entry& x : directory_iterator(folder)) {
			wstring path = x.path().generic_wstring();
			if (is_regular_file(path)) {
				const std::string extension = boost::algorithm::to_lower_copy(boost::filesystem::extension(path));
				vector<string>::iterator it = std::find(supportedExts.begin(), supportedExts.end(), extension);
				if (it != supportedExts.end()) {
					ret = GenerateFiles(sHoopsExchangeLoader, path.c_str(), isGenerateXml, thumbnailConverter);
					if (ret != A3D_SUCCESS) {
						cout << "Something went wrong with the file " << path << endl;
					}
				}
			}
			else if (is_directory(path)) {
				folders.push_back(path);
			} else {
				cout << path << " is not a regular file or directory" << endl;
			}
		}
		if (!folders.empty()) {
			for (std::vector<wstring>::iterator it = folders.begin(); it != folders.end(); ++it) {
				ProcessFolder(narrow(*it).c_str(), sHoopsExchangeLoader, isGenerateXml, thumbnailConverter);
			}
			folders.clear();
		}
	}
	catch (const filesystem_error& ex) {
		cout << ex.what() << endl;
	}
}

// Main function
#ifdef _MSC_VER
int wmain(A3DInt32 iArgc, wchar_t** ppcArgv)
#else
int main(A3DInt32 iArgc, A3DUTF8Char** ppcArgv)
#endif
{
	MY_CHAR acLogFileName[_MAX_PATH * 2];
	MY_SPRINTF(acLogFileName, "%s.log", L"CADnection.Processor");
	GetLogFile(acLogFileName); // Initialize log file

	string configFile = (iArgc == 2) ? narrow(ppcArgv[1]) : "";
	configFile += "CADnection.Processor.properties";
	std::map<std::string, std::string> props;
	Configuration::Read(configFile.c_str(), props);
	if (props.empty()) {
		cout << "Please review and correct the CADnection.Processor.properties file!" << endl;
		return A3D_ERROR;
	}
	std::string source = props["Source"];
	bool isGenerateXml = props["IsGenerateXml"] == "true";
	bool isGenerateThumbnail = props["IsGenerateThumbnail"] == "true";
	std::string dataServer = props["DataServer"];
	std::string author = props["Author"];
	bool isReprocessed = props["IsReProcessed"] == "true";
	std::string s3Bucket = props["S3Bucket"];
	std::string s3Folder = props["S3Folder"];
	std::string scPrefix = props["SCPrefix"];
	std::string generatedDataFolder = props["GeneratedDataFolder"];
	std::string jsonFolder = props["JsonFolder"];
	vector<string> exts;
	boost::split(exts, props["SupportedExtensions"], boost::is_any_of(","), boost::token_compress_on);

	struct stat info;
	if (stat(source.c_str(), &info) != 0) {
		cout << "The Source does not exists!" << endl;
		return A3D_ERROR;
	}
	else if (!(info.st_mode & S_IFDIR)) {
		cout << "The Source is not a directory!" << endl;
		return A3D_ERROR;
	}

	//
	// ### INITIALIZE HOOPS EXCHANGE
	//

	A3DSDKHOOPSExchangeLoader sHoopsExchangeLoader(_T(HOOPS_BINARY_DIRECTORY));
	CHECK_RET(sHoopsExchangeLoader.m_eSDKStatus);

	// Uncomment these lines to track memory leaks
	//CHECK_RET(A3DDllSetCallbacksMemory(CheckMalloc, CheckFree));
	CHECK_RET(A3DDllSetCallbacksReport(PrintLogMessage, PrintLogWarning, PrintLogError));

	ThumbnailConverter thumbnailConverter;
	thumbnailConverter.SetExecutor(props["ThumbnailConverterExe"].c_str());
	thumbnailConverter.SetLicense("4BUHBgRv6ga23wvz0Fjn2eMY4hYz0waWvSIU2uN$4BN05CzmzIpLjARk3yyZ9DYjDhJxrDVoCdEXBxZ2C9EMCiB4B7DF1yr96gEWwjfvCBjw8jqX1yNl2FeICU754EvowCf5xTM3CEz6xji10irx3vnu2zeLAiEQ4QFo5VmK2gE4BCeZABY19TY2DgiW3TiY5SaSADmXvCuZ9Ev50uF$0SVpCUj8ByaTAUM34Ezu5Sa94ib45iET2bDFj7Fy1hUJj7DFjCY26ghFj7DF8vM7wcQYkNDFwTM12sQYkNDFDQFl1y6XDvaM3AZl0BQH8UNx3RUH3wf03iv$5CeY5CjvxQJtDUmI0wZ6wEyXBRRa3Ui84vNvBTM22hQN2fVcCCmMAEbaowQECP1Fj7DsjNDFvUF9wQF22BJ18yJ8BeUPCgr4wyMO9SVr2vb8DVmVvVr48DV$2hQ68iB9BfNc4DRkEfU53TUX8hQzwgeU4Cu1DVryxBeWBBa58Vbo2yn3xwYK5yfvARM5xReOAfQIwwn86hZvBVaGvDU0DwR$2Tn27UJv0yeS0Drl1eRpvTeGwQMZ8fYWBgyHDEfswBUJAgno5iB5DieTBiqz8hIZwDnb3BJu5BJ64U7bBxiIzUY1CTbc2iySvBZo9geZEiAL7SjovTV65Sf2DRI_DCj31zrv5gEL1vny9iNaxyUZ3jn5vCqRDhJs9QRx0VaPEiiSByr08QUL4QMP3gMSAye13CAJvENlDeJqwia14inbwCY4Dg6ikrDF2RQ_8d5Fj7E79eM0xybo7RfpvDQ38xUzwxNpxQY79ffnxQE8jDJlwxJn7TJovDQ48TM18uZnxeQ2xAZm8ya89eZpxAFkwBi0wTQ5xQI3vDU68QZnwhU6wRfo8uU5wRe7wia4xhIz8uJl7QUzxhQ3xeY08hI87RizvAU37TU4xRa48AM67TM6vTI4vRjm8xU2xhZl8ffm9fm6wBez8QM48ia3weVn8eU8wAIzwuIzwuRl8QM3xQM6wfm2vDQ49eQz8TNovRjmxUazxfa5xffnweY4wQE07QFkxRez8eNl7QU4xQFm7QQ58TZkwQJkxQU68vnlxuU07Ri5wAY3xxM39hM1xuI8wBi2weRo8vi68eZpxvm88eY3xAM28uJn8Eaz7TJo8vno8QM4wxM79hQ58hVl8fbpwEazwRjl8TU78vblvARo8TNmwTRnwRi78eQ18uY8xRnl8eU2xANmwxU1weQzwAJnxQQ7vBe48ANpvQU4wBnnwTJn7Ra5xQM4wDU58DRpxRi18eQ29hJk8eI0wDM8vEa48Ba4vEa0xQE6wAZpxDZp8ya7vARkxhUz8AEz9fa68AYzvTY38fbpvDY0wuQ2wDVl7TI3wfnk9hRpvDZp8eRowhRkxhYzwuI1wuZo8DZp9eE5vBi88TM1wvi57TQ19hQz8DU7xDY4xAE89eE3vDQ78Rm88eUz8QU5jCJz4UL");
	std::string dataFolder = source;
	std::replace(dataFolder.begin(), dataFolder.end(), '\\', '/');
	thumbnailConverter.SetDataFolder(dataFolder.c_str());
	thumbnailConverter.SetDataServer(dataServer.c_str());
	thumbnailConverter.SetAuthor(author.c_str());
	thumbnailConverter.SetReprocessed(isReprocessed);
	thumbnailConverter.SetGenerateThumbnail(isGenerateThumbnail);
	thumbnailConverter.SetS3Bucket(s3Bucket.c_str());
	thumbnailConverter.SetS3Folder(s3Folder.c_str());
	thumbnailConverter.SetSCPrefix(scPrefix.c_str());
	thumbnailConverter.SetGeneratedDataFolder(generatedDataFolder.c_str());
	thumbnailConverter.SetJsonFolder(jsonFolder.c_str());
	thumbnailConverter.GenerateExtensionInfo(exts);

	ProcessFolder(source.c_str(), sHoopsExchangeLoader, isGenerateXml, &thumbnailConverter);

	return A3D_SUCCESS;
}

