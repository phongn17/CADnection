#ifndef __JSON_WRITER_H__
#define __JSON_WRITER_H__

#include <cassert>
#include <stack>
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"

using namespace rapidjson;

class JsonWriter {
public:
	static const bool IsReader = false;
	static const bool IsWriter = !IsReader;

private:
	JsonWriter(const JsonWriter&);
	JsonWriter& operator=(const JsonWriter&);

	// PIMPL idiom
	void* mWriter;      ///< JSON writer.
	void* mStream;      ///< Stream buffer.

public:
	/// Constructor.
	JsonWriter();

	/// Destructor.
	~JsonWriter();

	/// Obtains the serialized JSON string.
	const char* GetString() const;

	// Archive concept

	operator bool() const { return true; }

	JsonWriter& StartObject();
	JsonWriter& Member(const char* name);
	bool HasMember(const char* name) const;
	JsonWriter& EndObject();

	JsonWriter& StartArray(size_t* size = 0);
	JsonWriter& EndArray();

	JsonWriter& operator&(bool& b);
	JsonWriter& operator&(unsigned& u);
	JsonWriter& operator&(int& i);
	JsonWriter& operator&(double& d);
	JsonWriter& operator&(std::string& s);
	JsonWriter& SetNull();
};

#endif // !__JSON_WRITER_H__
