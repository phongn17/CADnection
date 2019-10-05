#ifndef __INPUT_OUTPUT_H__
#define __INPUT_OUTPUT_H__

#include <string>
using namespace std;

class InputOutput {
private:
	string m_input;
	string m_output;
	string m_dataFolder;
	string m_dataServer;
	string m_author;
	bool m_isReprocessed;
	bool m_isGenerateThumbnail;
	string m_s3Bucket;
	string m_s3Folder;
	string m_scPrefix;
	string m_generatedDataFolder;
	string m_jsonFolder;

public:
	const char* GetInput() const { return m_input.c_str(); }
	void SetInput(const wchar_t* input);

	const char* GetOutput() const { return m_output.c_str(); }
	void SetOutput(const wchar_t* output);
	void SetOutput(const char* output);

	const char* GetDataFolder() const { return m_dataFolder.c_str(); }
	void SetDataFolder(const char* dataFolder);

	const char* GetDataServer() const { return m_dataServer.c_str(); }
	void SetDataServer(const char* dataServer);

	const char* GetAuthor() const { return m_author.c_str(); }
	void SetAuthor(const char* author);

	bool IsReprocessed() const { return m_isReprocessed; }
	void SetReprocessed(bool isReprocessed);

	bool IsGenerateThumbnail() const { return m_isGenerateThumbnail; }
	void SetGenerateThumbnail(bool isGenerateThumbnail);

	const char* GetS3Bucket() const { return m_s3Bucket.c_str(); }
	void SetS3Bucket(const char* s3Bucket);

	const char* GetS3Folder() const { return m_s3Folder.c_str(); }
	void SetS3Folder(const char* s3Folder);

	const char* GetSCPrefix() const { return m_scPrefix.c_str(); }
	void SetSCPrefix(const char* scPrefix);

	const char* GetGeneratedDataFolder() const { return m_generatedDataFolder.c_str(); }
	void SetGeneratedDataFolder(const char* generatedDataFolder);

	const char* GetJsonFolder() const { return m_jsonFolder.c_str(); }
	void SetJsonFolder(const char* jsonFolder);
};

#endif // !__INPUT_OUTPUT_H__

