#pragma once

#include <string>

#include "Color.h"
#include "Common.h"

#ifdef _WIN32
   #include "windows.h"
#else
   #error Sprite may only be used on Windows
#endif

namespace Cypher
{
   enum class Pixel : SHORT
   {
      FULL = 0x2588,
      THREE_FOURTHS = 0x2593,
      HALF = 0x2592,
      ONE_FOURTH = 0x2591,
   };

   class Sprite
   {
   public:
      Sprite();
      Sprite(SHORT width, SHORT height);
      Sprite(const std::wstring& path);

      ~Sprite() = default;

      bool operator==(const Sprite& other) const;
      bool operator!=(const Sprite& other) const;

      inline SHORT SampleGlyph(float x, float y);
      inline SHORT SampleColor(float x, float y);

      bool Save(const std::wstring& path) const;
      bool Load(const std::wstring& path);

      SHORT GetGlyph(SHORT x, SHORT y);

      template <typename Coord_t, typename Glyph_t>
      inline void SetGlyph(Coord_t x, Coord_t y, Glyph_t glyph) { SetGlyph(static_cast<SHORT>(x), static_cast<SHORT>(y), static_cast<SHORT>(glyph)); }
   
      SHORT GetColor(SHORT x, SHORT y);

      template <typename Coord_t, Color::ValidType Color_t>
      inline void SetColor(Coord_t x, Coord_t y, Color_t color) { SetColor(static_cast<SHORT>(x), static_cast<SHORT>(y), static_cast<SHORT>(color)); }

      inline SHORT GetWidth() const { return m_width; }
      inline SHORT GetHeight() const { return m_height; }

   private:
      void SetGlyph(SHORT x, SHORT y, SHORT glyph);
      void SetColor(SHORT x, SHORT y, SHORT color);

      SHORT m_width;
      SHORT m_height;
      SHORT* m_glyphs;
      SHORT* m_colors;
   };
} // namespace Cypher
