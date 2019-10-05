#include "ThumbnailConverter.h"
#pragma warning(disable : 4267)			// warning occurs in cenv.hpp
#include <nowide/cenv.hpp>
using namespace nowide;

#ifdef _MSC_VER
#pragma warning(disable: 4996) // _CRT_SECURE_NO_WARNINGS equivalent
#endif

ThumbnailConverter::ThumbnailConverter() : m_resolution(DEFAULT_RES) {
}

void ThumbnailConverter::SetExecutor(const char* executor) {
	this->m_executor = executor;
}

void ThumbnailConverter::SetLicense(const char* license) {
	this->m_license = license;
}

const char* ThumbnailConverter::GetRootPath() {
	if (this->m_rootPath.empty()) {
		GenerateRootPath();
	}

	return this->m_rootPath.c_str();
}

bool ThumbnailConverter::GenerateRootPath() {
	char rootPath[1024];
	memset((void*)rootPath, 0, 1024);
	sprintf(rootPath, "https://s3-%s.amazonaws.com/%s/%s/", "us-west-1", GetS3Bucket(), GetS3Folder());
	this->m_rootPath = rootPath;

	return true;
}

bool ThumbnailConverter::Execute(const char* source, const char* dest, const char* sc) {
	char command[3024];
	char s3Object[_MAX_PATH];
	char bgColor[100];
	memset((void*)bgColor, 0, 100);
	sprintf(bgColor, "%f,%f,%f", 255.0f, 255.0f, 255.0f);

	const char* input = (source != NULL) ? source : GetInput();
	const char* output = (dest != NULL) ? dest : GetOutput();

	// this will 'fill' the string command with the right stuff,
	// assuming myFile and convertedFile are strings themselves
	sprintf(command, "%s --background_color \"%s\" --output_png_resolution %s --license %s --input \"%s\" --output_png \"%s\" --output_scs \"%s\"", m_executor.c_str(), bgColor, m_resolution.c_str(), m_license.c_str(), input, output, sc);

	// system call
	int ret = system(command);
	if (ret == 0) {
		const char* bucketName = GetS3Bucket();
		const char* s3Folder = GetS3Folder();
		//Aws::SDKOptions options;
		//Aws::InitAPI(options);
		{
			S3Storage s3;
			//if (s3.CreateBucketIfAny(bucketName)) {
				memset((void*)s3Object, 0, _MAX_PATH);
				string key = output;
				std::replace(key.begin(), key.end(), '/', '.');
				sprintf(s3Object, "%s/%s", s3Folder, key.c_str());
				//if (s3.PutObject(bucketName, s3Object, output)) {
				//	ret = -1;
				//}
				memset((void*)s3Object, 0, _MAX_PATH);
				key = sc;
				std::replace(key.begin(), key.end(), '/', '.');
				sprintf(s3Object, "%s/%s", s3Folder, key.c_str());
				//if (s3.PutObject(bucketName, s3Object, sc)) {
				//	ret = -1;
				//}
			//}
			//else {
			//	cout << "Can't create/access to the s3 bucket named " << GetS3Bucket() << endl;
			//	ret = -1;
			//}
		}
		//Aws::ShutdownAPI(options);
	}

	return (ret == 0);
}
