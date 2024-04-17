#pragma once

#include <string>
#include <vector>

#include "Types.h"

namespace Cypher
{
   namespace StringUtil
   {
      const std::string EMPTY_STRING = "";
      const std::wstring EMPTY_WSTRING = L"";

      std::string ToLower(const std::string& str);
      std::wstring ToLower(const std::wstring& str);

      std::string ToUpper(const std::string& str);
      std::wstring ToUpper(const std::wstring& str);

      int32_t Compare(const std::string& str1, const std::string& str2);
      int32_t Compare(const std::wstring& str1, const std::wstring& str2);

      std::string Trim(const std::string& str);
      std::wstring Trim(const std::wstring& str);

      char* ToChar(const std::string& str);
      wchar_t* ToWChar(const std::wstring& str);

      std::string FromChar(const char* str);
      std::wstring FromWChar(const wchar_t* str);

      std::vector<std::string> Split(const std::string& str, const std::string& delimiters);
      std::vector<std::wstring> Split(const std::wstring& str, const std::wstring& delimiters);

      std::string Replace(const std::string& str, const std::string& find, const std::string& replace);
      std::wstring Replace(const std::wstring& str, const std::wstring& find, const std::wstring& replace);

      std::string ToString(const std::wstring& str);
      std::wstring ToWString(const std::string& str);
   } // namespace StringUtil
} // namespace Cypher
