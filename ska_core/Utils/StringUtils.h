#pragma once
#include <string>
#include <sstream>
#include <vector>

namespace ska {
	class StringUtils
	{
	private:
		StringUtils();

	public:
		static std::string intToStr(const int x);
		static int strToInt(const std::string& str);
		static unsigned int strToUint(const std::string& str);
		static float strToFloat(const std::string& str);
		static std::string uintToStr(const unsigned int x);
		static std::string floatToStr(const float f);
		static std::string charToStr(char letter);

		template <class T>
		static T fromString(const std::string& str) {
            std::istringstream iss(str);
            T result;
            iss >> result;
            return result;
		}

		template <class T>
		static std::string toString(const T& value) {
            std::ostringstream oss;
            oss << value;
            return oss.str();
		}

		static std::vector<std::string>& split(const std::string &s, const char delim, std::vector<std::string> &elems);
		static std::vector<std::string> split(const std::string &s, const char delim);
		static std::string ltrim(const std::string &s);
		static std::string rtrim(const std::string &s);
		static std::string trim(const std::string &s);

		template <class T>
		static T extractTo(const size_t start, const std::string& s, const char to) {
            const auto subString = s.substr(start);
            const auto positionCharSearched = subString.find_first_of(to);
            return positionCharSearched != std::string::npos ?
                subString.substr(0, positionCharSearched - start) : "";
        };

		static std::wstring toUTF8(const std::string& s);
		static std::string toANSI(const std::wstring& ws);

		static void replaceAll(std::string& str, const std::string& from, const std::string& to);
		static void replaceAll(std::string& str, const char from, const char to);
		static bool isInt(const std::string& s, const int base);
		static bool isDecimal(const std::string& s);
		~StringUtils();


	};
}
