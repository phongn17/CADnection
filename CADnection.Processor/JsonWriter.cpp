#include "JsonWriter.h"

#define WRITER reinterpret_cast<PrettyWriter<StringBuffer>*>(mWriter)
#define STREAM reinterpret_cast<StringBuffer*>(mStream)

JsonWriter::JsonWriter() : mWriter(), mStream() {
	mStream = new StringBuffer;
	mWriter = new PrettyWriter<StringBuffer>(*STREAM);
}

JsonWriter::~JsonWriter() {
	delete WRITER;
	delete STREAM;
}

const char* JsonWriter::GetString() const {
	return STREAM->GetString();
}

JsonWriter& JsonWriter::StartObject() {
	WRITER->StartObject();
	return *this;
}

JsonWriter& JsonWriter::EndObject() {
	WRITER->EndObject();
	return *this;
}

JsonWriter& JsonWriter::Member(const char* name) {
	WRITER->String(name, static_cast<SizeType>(strlen(name)));
	return *this;
}

bool JsonWriter::HasMember(const char*) const {
	// This function is for JsonReader only.
	assert(false);
	return false;
}

JsonWriter& JsonWriter::StartArray(size_t*) {
	WRITER->StartArray();
	return *this;
}

JsonWriter& JsonWriter::EndArray() {
	WRITER->EndArray();
	return *this;
}

JsonWriter& JsonWriter::operator&(bool& b) {
	WRITER->Bool(b);
	return *this;
}

JsonWriter& JsonWriter::operator&(unsigned& u) {
	WRITER->Uint(u);
	return *this;
}

JsonWriter& JsonWriter::operator&(int& i) {
	WRITER->Int(i);
	return *this;
}

JsonWriter& JsonWriter::operator&(double& d) {
	WRITER->Double(d);
	return *this;
}

JsonWriter& JsonWriter::operator&(std::string& s) {
	WRITER->String(s.c_str(), static_cast<SizeType>(s.size()));
	return *this;
}

JsonWriter& JsonWriter::SetNull() {
	WRITER->Null();
	return *this;
}

#undef STREAM
#undef WRITER
