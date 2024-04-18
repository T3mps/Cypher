#pragma once

#include <string>
#include <Windows.h>

namespace Cypher
{
   struct Window
   {
      SHORT width;
      SHORT height;
      std::wstring title;
      SMALL_RECT extents;
      bool hasFocus;

      Window() :
         width(0),
         height(0),
         extents({ 0U, 0U, 1U, 1U }),
         hasFocus(true)
      {
      }

      Window(SHORT w, SHORT h, const std::wstring& t) :
         width(w),
         height(h),
         title(t),
         extents({ 0U, 0U, 1U, 1U }),
         hasFocus(true)
      {
      }

      SHORT Area() const { return width * height; }
   };
}