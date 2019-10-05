#include "InputOutput.h"

#pragma warning(disable : 4267)			// warning occurs in cenv.hpp
#include <nowide/cenv.hpp>
using namespace nowide;

#ifdef _MSC_VER
#pragma warning(disable: 4996) // _CRT_SECURE_NO_WARNINGS equivalent
#endif

void InputOutput::SetInput(const wchar_t* input) {
	this->m_input = narrow(input);
}

void InputOutput::SetOutput(const wchar_t* output) {
	this->m_output = narrow(output);
}

void InputOutput::SetOutput(const char* output) {
	this->m_output = output;
}

void InputOutput::SetDataFolder(const char* dataFolder) {
	this->m_dataFolder = dataFolder;
}

void InputOutput::SetDataServer(const char* dataServer) {
	this->m_dataServer = dataServer;
}

void InputOutput::SetAuthor(const char* author) {
	this->m_author = author;
}

void InputOutput::SetReprocessed(bool isReprocessed) {
	this->m_isReprocessed = isReprocessed;
}

void InputOutput::SetGenerateThumbnail(bool isGenerateThumbnail) {
	this->m_isGenerateThumbnail = isGenerateThumbnail;
}

void InputOutput::SetS3Bucket(const char* s3Bucket) {
	this->m_s3Bucket = s3Bucket;
}

void InputOutput::SetS3Folder(const char* s3Folder) {
	this->m_s3Folder = s3Folder;
}

void InputOutput::SetSCPrefix(const char* scPrefix) {
	this->m_scPrefix = scPrefix;
}

void InputOutput::SetGeneratedDataFolder(const char* generatedDataFolder) {
	this->m_generatedDataFolder = generatedDataFolder;
}

void InputOutput::SetJsonFolder(const char* jsonFolder) {
	this->m_jsonFolder = jsonFolder;
}
