#pragma once

#ifndef UNICODE
   #error Please enable the Unicode Character Set for your compiler.
#endif

#include <string>
#include <vector>

#include "Color.h"
#include "Sprite.h"
#include "Math/Vector.h"

#ifdef _WIN32
   #include "windows.h"
#else
   #error TextRenderer may only be executed on Windows
#endif

namespace Cypher
{
   class TextRenderer
   {
   public:
      using Buffer = CHAR_INFO*;

      TextRenderer(SHORT& screenWidth, SHORT& screenHeight, Buffer& screenBuffer);
      ~TextRenderer() = default;

      void Clear();

      template <typename Coord_t, typename Glyph_t = Pixel, Color::ValidType Color_t = Color::Foreground>
      inline void Draw(Coord_t x, Coord_t y, Glyph_t glyph = Pixel::FULL, Color_t color = Color::Foreground::WHITE) { Draw(static_cast<SHORT>(x), static_cast<SHORT>(y), static_cast<SHORT>(glyph), static_cast<SHORT>(color)); }

      template <typename Coord_t, typename Glyph_t = Pixel, Color::ValidType Color_t = Color::Foreground>
      inline void Fill(Coord_t x0, Coord_t y0, Coord_t x1, Coord_t y1, Glyph_t glyph = Pixel::FULL, Color_t color = Color::Foreground::WHITE) { Fill(static_cast<SHORT>(x0), static_cast<SHORT>(y0), static_cast<SHORT>(x1), static_cast<SHORT>(y1), static_cast<SHORT>(glyph), static_cast<SHORT>(color)); }

      template <typename Coord_t, Color::ValidType Color_t = Color::Foreground>
      inline void DrawString(Coord_t x, Coord_t y, std::wstring string, Color_t color = Color::Foreground::WHITE) { DrawString(static_cast<SHORT>(x), static_cast<SHORT>(y), string, static_cast<SHORT>(color)); }
  
      template <typename Coord_t, typename Glyph_t = Pixel, typename Color_t = Color::Foreground>
      void DrawLine(Coord_t x0, Coord_t y0, Coord_t x1, Coord_t y1, Glyph_t glyph = Pixel::FULL, Color_t color = Color::Foreground::WHITE) { DrawLine(static_cast<SHORT>(x0), static_cast<SHORT>(y0), static_cast<SHORT>(x1), static_cast<SHORT>(y1), static_cast<SHORT>(glyph), static_cast<SHORT>(color)); }

      template <typename Coord_t, typename Glyph_t = Pixel, typename Color_t = Color::Foreground>
      void DrawRectangle(Coord_t x, Coord_t y, SHORT width, SHORT height, Glyph_t glyph = Pixel::FULL, Color_t color = Color::Foreground::WHITE) { DrawRectangle(static_cast<SHORT>(x), static_cast<SHORT>(y), width, height, static_cast<SHORT>(glyph), static_cast<SHORT>(color)); }

      template <typename Coord_t, typename Glyph_t = Pixel, typename Color_t = Color::Foreground>
      void FillRectangle(Coord_t x, Coord_t y, SHORT width, SHORT height, Glyph_t glyph = Pixel::FULL, Color_t color = Color::Foreground::WHITE) { FillRectangle(static_cast<SHORT>(x), static_cast<SHORT>(y), width, height, static_cast<SHORT>(glyph), static_cast<SHORT>(color)); }

      template <typename Coord_t, typename Glyph_t = Pixel, typename Color_t = Color::Foreground>
      void DrawCircle(Coord_t centerX, Coord_t centerY, SHORT radius, Glyph_t glyph = Pixel::FULL, Color_t color = Color::Foreground::WHITE) { DrawCircle(static_cast<SHORT>(centerX), static_cast<SHORT>(centerY), radius, static_cast<SHORT>(glyph), static_cast<SHORT>(color)); }

      template <typename Coord_t, typename Glyph_t = Pixel, typename Color_t = Color::Foreground>
      void FillCircle(Coord_t centerX, Coord_t centerY, SHORT radius, Glyph_t glyph = Pixel::FULL, Color_t color = Color::Foreground::WHITE) { FillCircle(static_cast<SHORT>(centerX), static_cast<SHORT>(centerY), radius, static_cast<SHORT>(glyph), static_cast<SHORT>(color)); }

      template <typename Coord_t, typename Glyph_t = Pixel, typename Color_t = Color::Foreground>
      void DrawTriangle(Coord_t x0, Coord_t y0, Coord_t x1, Coord_t y1, Coord_t x2, Coord_t y2, Glyph_t glyph = Pixel::FULL, Color_t color = Color::Foreground::WHITE) { DrawTriangle(static_cast<SHORT>(x0), static_cast<SHORT>(y0), static_cast<SHORT>(x1), static_cast<SHORT>(y1), static_cast<SHORT>(x2), static_cast<SHORT>(y2), static_cast<SHORT>(glyph), static_cast<SHORT>(color)); }

      template <typename Coord_t, typename Glyph_t = Pixel, typename Color_t = Color::Foreground>
      void FillTriangle(Coord_t x0, Coord_t y0, Coord_t x1, Coord_t y1, Coord_t x2, Coord_t y2, Glyph_t glyph = Pixel::FULL, Color_t color = Color::Foreground::WHITE) { FillTriangle(static_cast<SHORT>(x0), static_cast<SHORT>(y0), static_cast<SHORT>(x1), static_cast<SHORT>(y1), static_cast<SHORT>(x2), static_cast<SHORT>(y2), static_cast<SHORT>(glyph), static_cast<SHORT>(color)); }

      template <typename Coord_t, typename Glyph_t = Pixel, typename Color_t = Color::Foreground>
      void DrawPolygon(const std::vector<Vector2f>& vertices, Coord_t x, Coord_t y, float rotation = 0.0f, float scale = 1.0f, Glyph_t glyph = Pixel::FULL, Color_t color = Color::Foreground::WHITE) { DrawPolygon(vertices, static_cast<SHORT>(x), static_cast<SHORT>(y), rotation, scale, static_cast<SHORT>(glyph), static_cast<SHORT>(color)); }

      template <typename Coord_t, typename Glyph_t = Pixel, typename Color_t = Color::Foreground>
      void FillPolygon(const std::vector<Vector2f>& vertices, Coord_t x, Coord_t y, float rotation = 0.0f, float scale = 1.0f, Glyph_t glyph = Pixel::FULL, Color_t color = Color::Foreground::WHITE) { FillPolygon(vertices, static_cast<SHORT>(x), static_cast<SHORT>(y), rotation, scale, static_cast<SHORT>(glyph), static_cast<SHORT>(color)); }

      template<typename Coord_t>
      void DrawSprite(Coord_t x, Coord_t y, Sprite sprite)
      {
         for (SHORT yp = 0; yp < sprite.GetHeight(); yp++)
         {
            for (SHORT xp = 0; xp < sprite.GetWidth(); xp++)
            {
               if (sprite.GetGlyph(xp, yp) != ' ')
                  Draw(x + xp, y + yp, sprite.GetGlyph(xp, yp), sprite.GetColor(xp, yp));
            }
         }
      }

   private:
      void Draw(SHORT x, SHORT y, SHORT glyph, SHORT color);
      void Fill(SHORT x0, SHORT y0, SHORT x1, SHORT y1, SHORT glyph, SHORT color);
   
      void DrawString(SHORT x, SHORT y, std::wstring string, SHORT color);

      void DrawLine(SHORT x0, SHORT y0, SHORT x1, SHORT y1, SHORT glyph, SHORT color);
   
      void DrawRectangle(SHORT x, SHORT y, SHORT width, SHORT height, SHORT glyph, SHORT color);
      void FillRectangle(SHORT x, SHORT y, SHORT width, SHORT height, SHORT glyph, SHORT color);

      void DrawCircle(SHORT centerX, SHORT centerY, SHORT radius, SHORT glyph, SHORT color);
      void FillCircle(SHORT centerX, SHORT centerY, SHORT radius, SHORT glyph, SHORT color);

      void DrawTriangle(SHORT x0, SHORT y0, SHORT x1, SHORT y1, SHORT x2, SHORT y2, SHORT glyph, SHORT color);
      void FillTriangle(SHORT x0, SHORT y0, SHORT x1, SHORT y1, SHORT x2, SHORT y2, SHORT glyph, SHORT color);

      void DrawPolygon(const std::vector<Vector2f>& vertices, SHORT x, SHORT y, float rotation, float scale, SHORT glyph, SHORT color);
      void FillPolygon(const std::vector<Vector2f>& vertices, SHORT x, SHORT y, float rotation, float scale, SHORT glyph, SHORT color);

      void Clip(SHORT& x, SHORT& y);

      SHORT& m_screenWidth;
      SHORT& m_screenHeight;
      Buffer& m_screenBuffer;
   };
} // namespace Cypher
