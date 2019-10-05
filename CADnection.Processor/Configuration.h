#ifndef __CADNECTION_PROCESSOR_CONFIGURATION_H__
#define __CADNECTION_PROCESSOR_CONFIGURATION_H__

#include <iostream>
#include <fstream>
#include <cstddef>
#include <map>
#include <string>

class Configuration {
public:
	static void Read(const char* filename, std::map<std::string, std::string>& props) {
		std::ifstream ins(filename);
		if (ins.is_open()) {
			std::string s, key, value;

			// For each (key, value) pair in the file
			while (std::getline(ins, s)) {
				std::string::size_type begin = s.find_first_not_of(" \f\t\v");
				// Skip blank lines
				if (begin == std::string::npos)
					continue;

				// Skip commentary
				if (std::string("#").find(s[begin]) != std::string::npos)
					continue;

				// Extract the key value
				std::string::size_type end = s.find('=', begin);
				key = s.substr(begin, end - begin);

				// (No leading or trailing whitespace allowed)
				key.erase(key.find_last_not_of(" \f\t\v") + 1);

				// No blank keys allowed
				if (key.empty()) 
					continue;

				// Extract the value (no leading or trailing whitespace allowed)
				begin = s.find_first_not_of(" \f\n\r\t\v", end + 1);
				end = s.find_last_not_of(" \f\n\r\t\v") + 1;

				value = s.substr(begin, end - begin);

				// Insert the properly extracted (key, value) pair into the map
				props[key] = value;
			}

			ins.close();
		}
	}
};

#endif // !__CADNECTION_PROCESSOR_CONFIGURATION_H__
