#include "StringUtil.h"

#include <algorithm>
#include <cctype>
#include <cwctype>
#include <locale>
#include <string>
#include <vector>
#ifdef _WIN32
   #include <windows.h>
#endif

std::string Cypher::StringUtil::ToLower(const std::string& str)
{
   std::string lowerStr = str;
   std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), ::tolower);
   return lowerStr;
}

std::wstring Cypher::StringUtil::ToLower(const std::wstring& str)
{
   std::wstring lowerStr = str;
   std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), ::towlower);
   return lowerStr;
}

std::string Cypher::StringUtil::ToUpper(const std::string& str)
{
   std::string upperStr = str;
   std::transform(upperStr.begin(), upperStr.end(), upperStr.begin(), ::toupper);
   return upperStr;
}

std::wstring Cypher::StringUtil::ToUpper(const std::wstring& str)
{
   std::wstring upperStr = str;
   std::transform(upperStr.begin(), upperStr.end(), upperStr.begin(), ::towupper);
   return upperStr;
}

int32_t Cypher::StringUtil::Compare(const std::string& str1, const std::string& str2)
{
   return str1.compare(str2);
}

int32_t Cypher::StringUtil::Compare(const std::wstring& str1, const std::wstring& str2)
{
   return str1.compare(str2);
}

std::string Cypher::StringUtil::Trim(const std::string& str)
{
   size_t first = str.find_first_not_of(' ');
   if (first == std::string::npos)
      return EMPTY_STRING;
   size_t last = str.find_last_not_of(' ');
   return str.substr(first, (last - first + 1));
}

std::wstring Cypher::StringUtil::Trim(const std::wstring& str)
{
   size_t first = str.find_first_not_of(L' ');
   if (first == std::wstring::npos)
      return EMPTY_WSTRING;
   size_t last = str.find_last_not_of(L' ');
   return str.substr(first, (last - first + 1));
}

char* Cypher::StringUtil::ToChar(const std::string& str)
{
   char* cstr = new char[str.length() + 1];
   std::strcpy(cstr, str.c_str());
   return cstr;
}

wchar_t* Cypher::StringUtil::ToWChar(const std::wstring& str)
{
   wchar_t* wcstr = new wchar_t[str.length() + 1];
   std::wcscpy(wcstr, str.c_str());
   return wcstr;
}

std::string Cypher::StringUtil::FromChar(const char* str)
{
   return std::string(str);
}

std::wstring Cypher::StringUtil::FromWChar(const wchar_t* str)
{
   return std::wstring(str);
}

std::vector<std::string> Cypher::StringUtil::Split(const std::string& str, const std::string& delimiters)
{
   std::vector<std::string> tokens;
   size_t start = 0, end = 0;
   while ((end = str.find_first_of(delimiters, start)) != std::string::npos)
   {
      if (end != start)
         tokens.push_back(str.substr(start, end - start));
      start = end + 1;
   }
   if (end != start)
      tokens.push_back(str.substr(start));
   return tokens;
}

std::vector<std::wstring> Cypher::StringUtil::Split(const std::wstring& str, const std::wstring& delimiters)
{
   std::vector<std::wstring> tokens;
   size_t start = 0, end = 0;
   while ((end = str.find_first_of(delimiters, start)) != std::wstring::npos)
   {
      if (end != start)
         tokens.push_back(str.substr(start, end - start));
      start = end + 1;
   }
   if (end != start)
      tokens.push_back(str.substr(start));
   return tokens;
}

std::string Cypher::StringUtil::Replace(const std::string& str, const std::string& find, const std::string& replace)
{
   std::string result = str;
   size_t pos = 0;
   while ((pos = result.find(find, pos)) != std::string::npos)
   {
      result.replace(pos, find.length(), replace);
      pos += replace.length();
   }
   return result;
}

std::wstring Cypher::StringUtil::Replace(const std::wstring& str, const std::wstring& find, const std::wstring& replace)
{
   std::wstring result = str;
   size_t pos = 0;
   while ((pos = result.find(find, pos)) != std::wstring::npos)
   {
      result.replace(pos, find.length(), replace);
      pos += replace.length();
   }
   return result;
}

std::string Cypher::StringUtil::ToString(const std::wstring& str)
{
#ifdef _WIN32
   int32_t size = WideCharToMultiByte(CP_UTF8, 0, str.c_str(), -1, NULL, 0, NULL, NULL);
   char* buffer = new char[size];
   WideCharToMultiByte(CP_UTF8, 0, str.c_str(), -1, buffer, size, NULL, NULL);
   std::string result(buffer);
   delete[] buffer;
#else
   std::mbstate_t state = std::mbstate_t();
   std::string result(str.size() * sizeof(wchar_t), '\0');
   const wchar_t* wcstr = str.c_str();
   char* mbstr = &result[0];
   std::wcsrtombs(mbstr, &wcstr, result.size(), &state);
#endif
   return result;
}

std::wstring Cypher::StringUtil::ToWString(const std::string& str)
{
#ifdef _WIN32
   // Windows-specific conversion using MultiByteToWideChar
   int32_t size = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
   wchar_t* buffer = new wchar_t[size];
   MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, buffer, size);
   std::wstring result(buffer);
   delete[] buffer;
#else
   std::mbstate_t state = std::mbstate_t();
   std::wstring result(str.size(), L'\0');
   const char* mbstr = str.c_str();
   wchar_t* wcstr = &result[0];
   std::mbsrtowcs(wcstr, &mbstr, result.size(), &state);
#endif
   return result;
}
