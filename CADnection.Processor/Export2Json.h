#ifndef __EXPORT_2_JSON_H__
#define __EXPORT_2_JSON_H__
#include <fstream>
#include <iostream>

#include "InputOutput.h"
#include "json.hpp"

using namespace nlohmann;

class Export2Json: public InputOutput {
private:
	json data = "{ \"docs\": [] }"_json;

public:
	Export2Json();

	static bool PartExists(json* jsonData, int index, const char* partId);

	json* GetData() { return &data; }

	void Save();
};

#endif // !__EXPORT_2_JSON_H__

