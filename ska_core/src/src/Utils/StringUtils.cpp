#include "StringUtils.h"
#include "../Exceptions/NumberFormatException.h"
#include <string>
#include <sstream>

#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>
#include <iomanip>
#include <codecvt>

// utility wrapper to adapt locale-bound facets for wstring/wbuffer convert
template<class Facet>
class deletable_facet : Facet {
    public:
    template<class ...Args>
    deletable_facet(Args&& ...args) : Facet(std::forward<Args>(args)...) {}
    ~deletable_facet() {}
};

std::string ska::StringUtils::intToStr(const int x) {
	std::ostringstream oss;
	oss << x;
	return oss.str();
}

bool ska::StringUtils::isInt(const std::string& s, const int base) {
	if (s.empty() || std::isspace(s[0])) {
		return false;
	}
	char * p;
	strtol(s.c_str(), &p, base);
	return (*p == 0);
}

int ska::StringUtils::strToInt(const std::string& str) {
	return stoi(str);
}

unsigned int ska::StringUtils::strToUint(const std::string& str) {
	return stoul(str);
}

bool ska::StringUtils::isDecimal(const std::string& s) {
	if (s.empty()) {
		return false;
	}

	std::istringstream iss(s);
	float f;
	iss >> std::noskipws >> f; // noskipws considers leading whitespace invalid
						  // Check the entire string was consumed and if either failbit or badbit is set
	return iss.eof() && !iss.fail();
}

float ska::StringUtils::strToFloat(const std::string& str) {
	return static_cast<float>(atof(str.c_str()));
}

std::string ska::StringUtils::uintToStr(const unsigned int x) {
	std::ostringstream oss;
	oss << x;
	return oss.str();
}

std::vector<std::string>& ska::StringUtils::split(const std::string &s, const char delim, std::vector<std::string> &elems) {
	std::stringstream ss(s);
	std::string item;
	while (getline(ss, item, delim)) {
		elems.push_back(item);
	}
	return elems;
}


std::vector<std::string> ska::StringUtils::split(const std::string &s, const char delim) {
	std::vector<std::string> elems;
	split(s, delim, elems);
	return elems;
}

// trim from start
std::string ska::StringUtils::ltrim(const std::string &s) {
	auto tmp = std::string (s.c_str());
	auto firstCharIt = find_if(tmp.begin(), tmp.end(), [](const auto& c) {return !std::isspace(c); });
	tmp.erase(tmp.begin(), firstCharIt);
	return tmp;
}

// trim from end
std::string ska::StringUtils::rtrim(const std::string &s) {
	std::string tmp(s.c_str());
	auto lastCharIt = std::find_if(tmp.rbegin(), tmp.rend(), [](const auto& c) {return !std::isspace(c); }).base();
	tmp.erase(lastCharIt, tmp.end());
	return tmp;
}

// trim from both ends
std::string ska::StringUtils::trim(const std::string &s) {
	return ltrim(rtrim(s));
}

void ska::StringUtils::replaceAll(std::string& str, const std::string& from, const std::string& to) {
	size_t start_pos = 0;
	while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
		str.replace(start_pos, from.length(), to);
		start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
	}
}

void ska::StringUtils::replaceAll(std::string& str, const char from, const char to) {
	replaceAll(str, std::string(1, from), std::string(1, to));
}

std::wstring ska::StringUtils::toUTF8(const std::string& s) {
	if (s.empty()) {
		return L"";
	}

	auto sNat = s.c_str();
	std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
	return converter.from_bytes(sNat);
}

std::string ska::StringUtils::floatToStr(const float f) {
	std::ostringstream oss;
	oss << f;
	return oss.str();
}

std::string ska::StringUtils::charToStr(char letter) {
	std::ostringstream oss;
	oss << letter;
	return oss.str();
}

template <>
std::string ska::StringUtils::fromString<std::string>(const std::string& str) {
	return str;
}

std::string ska::StringUtils::toANSI(const std::wstring& ws) {
	if (ws.empty()) {
		return "";
	}

	auto sNat = ws.c_str();
	std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
	return converter.to_bytes(sNat);
}
