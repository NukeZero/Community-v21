#pragma once
#include <string>

namespace std
{
	inline std::string to_string(const std::string& s) { return s; }
	inline std::string to_string(const char* s) { return s; }
}

class Lexer
{
	public:
	template<typename... Args>
	static std::string build(Args const&... args)
	{
		std::string result;
		const int unpack[]{ 0, (result += std::to_string(args), 0)... };
		static_cast<void>(unpack);
		return result;
	}
};