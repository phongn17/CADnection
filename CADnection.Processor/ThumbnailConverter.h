#ifndef __THUMBNAIL_CONVERTER_H__
#define __THUMBNAIL_CONVERTER_H__

#include "InputOutput.h"
#include "S3Storage.h"

#define DEFAULT_RES "320x240"

class ThumbnailConverter: public InputOutput {
private:
	string m_executor;
	string m_license;
	string m_resolution;
	string m_rootPath;

public:
	ThumbnailConverter();

	const char* GetRootPath();
	
	void SetExecutor(const char* executor);
	void SetLicense(const char* license);
	
	bool Execute(const char* input, const char* output, const char* sc);

private:
	bool GenerateRootPath();
};

#endif // !__THUMBNAIL_CONVERTER_H__
