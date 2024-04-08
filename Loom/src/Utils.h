#pragma once

#include <string>
#include <iostream>
#include <memory>
#include <Windows.h>

namespace Utils
{
   std::string WStringToString(const std::wstring& wstr)
   {
      if (wstr.empty())
         return std::string();
      int sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
      std::string strTo(sizeNeeded, 0);
      WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], sizeNeeded, NULL, NULL);
      return strTo;
   }
}