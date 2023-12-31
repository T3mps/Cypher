#include "Sprite.h"

#include "Color.h"

Cypher::Sprite::Sprite() :
   m_width(0),
   m_height(0),
   m_glyphs(nullptr),
   m_colors(nullptr)
{
}

Cypher::Sprite::Sprite(SHORT width, SHORT height) :
   m_width(width),
   m_height(height),
   m_glyphs(new SHORT[static_cast<size_t>(width) * height]),
   m_colors(new SHORT[static_cast<size_t>(width) * height])
{
   const SHORT area = width * height;
   for (SHORT i = 0; i < area; ++i)
   {
      m_glyphs[i] = ' ';
      m_colors[i] = 0;
   }
}

Cypher::Sprite::Sprite(const std::wstring& path) :
   m_width(0),
   m_height(0),
   m_glyphs(nullptr),
   m_colors(nullptr)
{
}

bool Cypher::Sprite::operator==(const Sprite& other) const
{
   return m_width == other.m_width &&
      m_height == other.m_height &&
      m_glyphs == other.m_glyphs &&
      m_colors == other.m_colors;
}

bool Cypher::Sprite::operator!=(const Sprite& other) const
{
   return !(*this == other);
}

SHORT Cypher::Sprite::SampleGlyph(float x, float y)
{
   SHORT sx = static_cast<SHORT>(x * static_cast<float>(m_width));
   SHORT sy = static_cast<SHORT>(y * static_cast<float>(m_height) - 1.0f);
   if (sx < 0 || sx >= m_width || sy < 0 || sy >= m_height)
      return ' ';
   return m_glyphs[sy * m_width + sx];
}

SHORT Cypher::Sprite::SampleColor(float x, float y)
{
   SHORT sx = static_cast<SHORT>(x * static_cast<float>(m_width));
   SHORT sy = static_cast<SHORT>(y * static_cast<float>(m_height) - 1.0f);
   if (sx < 0 || sx >= m_width || sy < 0 || sy >= m_height)
      return 0;
   return m_colors[sy * m_width + sx];
}

bool Cypher::Sprite::Save(const std::wstring& path) const
{
   FILE* file = nullptr;
   _wfopen_s(&file, path.c_str(), L"wb");
   if (file)
   {
      fwrite(&m_width, sizeof(SHORT), 1, file);
      fwrite(&m_height, sizeof(SHORT), 1, file);
      fwrite(m_glyphs, sizeof(SHORT), static_cast<size_t>(m_width) * m_height, file);
      fwrite(m_colors, sizeof(SHORT), static_cast<size_t>(m_width) * m_height, file);
      fclose(file);
   }

   return file != nullptr;
}

bool Cypher::Sprite::Load(const std::wstring& path)
{
   FILE* file = nullptr;
   _wfopen_s(&file, path.c_str(), L"rb");
   if (file)
   {
      delete[] m_glyphs;
      delete[] m_colors;
      m_width = 0;
      m_height = 0;
      
      std::fread(&m_width, sizeof(SHORT), 1, file);
      std::fread(&m_height, sizeof(SHORT), 1, file);

      const size_t area = static_cast<size_t>(m_width) * m_height;
      m_glyphs = new SHORT[area];
      m_colors = new SHORT[area];
      std::fread(m_glyphs, sizeof(SHORT), area, file);
      std::fread(m_colors, sizeof(SHORT), area, file);

      std::fclose(file);
   }

   return file != nullptr;
}

SHORT Cypher::Sprite::GetGlyph(SHORT x, SHORT y)
{
   if (x < 0 || x >= m_width || y < 0 || y >= m_height)
      return ' ';
   return m_glyphs[y * m_width + x];
}

void Cypher::Sprite::SetGlyph(SHORT x, SHORT y, SHORT glyph)
{
   if (x < 0 || x >= m_width || y < 0 || y >= m_height)
      return;
   m_glyphs[y * m_width + x] = glyph;
}

SHORT Cypher::Sprite::GetColor(SHORT x, SHORT y)
{
   if (x < 0 || x >= m_width || y < 0 || y >= m_height)
      return 0;
   return m_colors[y * m_width + x];
}

void Cypher::Sprite::SetColor(SHORT x, SHORT y, SHORT color)
{
   if (x < 0 || x >= m_width || y < 0 || y >= m_height)
      return;
   m_colors[y * m_width + x] = color;
}
