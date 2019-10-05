#include "Export2Json.h"

bool Export2Json::PartExists(json* jsonData, int index, const char* partId) {
	json& parts = (*jsonData)["docs"][index]["ComposedOf"];
	if (parts.type() == detail::value_t::null)
		return false;

	if (parts.find(partId) != parts.end()) {
		return true;
	}

	return false;
}

Export2Json::Export2Json() {
}

void Export2Json::Save() {
	try {
		// json& po = data["/docs/0"_json_pointer];
		// if (po.type() == detail::value_t::null)
		// 	return;

		// std::string json_str = po.dump();
		std::string json_str = data.dump();
		const char* output = GetOutput();
		std::ofstream file(output);
		file << json_str;
	}
	catch (const exception& ex) {
		cout << ex.what() << endl;
	}
}
