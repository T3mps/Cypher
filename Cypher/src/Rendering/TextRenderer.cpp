#include "TextRenderer.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <ranges>
#include <tuple>
#include <vector>

Cypher::TextRenderer::TextRenderer(SHORT& screenWidth, SHORT& screenHeight, Buffer& screenBuffer) :
	m_screenBuffer(screenBuffer),
	m_screenWidth(screenWidth),
	m_screenHeight(screenHeight)
{
}

void Cypher::TextRenderer::Clear()
{
	Fill(static_cast<SHORT>(0), static_cast<SHORT>(0), m_screenWidth, m_screenHeight, Pixel::FULL, static_cast<SHORT>(0));
}

void Cypher::TextRenderer::Draw(SHORT x, SHORT y, SHORT glyph, SHORT color)
{
	if (x >= 0 && x < m_screenWidth && y >= 0 && y < m_screenHeight)
	{
		m_screenBuffer[y * m_screenWidth + x].Char.UnicodeChar = glyph;
		m_screenBuffer[y * m_screenWidth + x].Attributes = color;
	}
}

void Cypher::TextRenderer::Fill(SHORT x0, SHORT y0, SHORT x1, SHORT y1, SHORT glyph, SHORT color)
{
	Clip(x0, y0);
	Clip(x1, y1);

	for (SHORT x = x0; x < x1; ++x)
	{
		for (SHORT y = y0; y < y1; ++y)
		{
			Draw(x, y, glyph, color);
		}
	}
}

void Cypher::TextRenderer::DrawString(SHORT x, SHORT y, std::wstring string, SHORT color)
{
	for (size_t i = 0; i < string.size(); ++i)
	{
		m_screenBuffer[y * m_screenWidth + x + static_cast<SHORT>(i)].Char.UnicodeChar = string[i];
		m_screenBuffer[y * m_screenWidth + x + static_cast<SHORT>(i)].Attributes = color;
	}
}

void Cypher::TextRenderer::DrawLine(SHORT x0, SHORT y0, SHORT x1, SHORT y1, SHORT glyph, SHORT color)
{
	SHORT dx = std::abs(x1 - x0);
	SHORT dy = -std::abs(y1 - y0);
	SHORT sx = x0 < x1 ? 1 : -1;
	SHORT sy = y0 < y1 ? 1 : -1;
	SHORT err = dx + dy;
	SHORT e2;

	while (true)
	{
		Draw(x0, y0, glyph, color);

		if (x0 == x1 && y0 == y1)
			break;

		e2 = 2 * err;

		if (e2 >= dy)
		{
			err += dy;
			x0 += sx;
		}

		if (e2 <= dx)
		{
			err += dx;
			y0 += sy;
		}
	}
}

void Cypher::TextRenderer::DrawRectangle(SHORT x, SHORT y, SHORT width, SHORT height, SHORT glyph, SHORT color)
{
	const SHORT x1 = x + width - 1;
	const SHORT y1 = y + height - 1;

	const SHORT perimeter = 2 * (width + height) - 4;

	for (SHORT i = 0; i < perimeter; ++i)
	{
		if (i < width)
		{
			Draw(x + i, y, glyph, color);
		}
		else if (i < width + height - 1)
		{
			Draw(x1, y + i - width + 1, glyph, color);
		}
		else if (i < width * 2 + height - 2)
		{
			Draw(x1 - (i - (width + height - 2)), y1, glyph, color);
		}
		else
		{
			Draw(x, y1 - (i - (width * 2 + height - 3)), glyph, color);
		}
	}
}

void Cypher::TextRenderer::FillRectangle(SHORT x, SHORT y, SHORT width, SHORT height, SHORT glyph, SHORT color)
{
	for (SHORT yi = 0; yi < height; ++yi)
	{
		for (SHORT xi = 0; xi < width; ++xi)
		{
			Draw(x + xi, y + yi, glyph, color);
		}
	}
}

void  Cypher::TextRenderer::DrawCircle(SHORT centerX, SHORT centerY, SHORT radius, SHORT glyph, SHORT color)
{
	if (radius <= 0)
		return;

	SHORT x = 0;
	SHORT y = radius;
	SHORT p = 1 - radius;

	while (x <= y)
	{
		Draw(centerX + x, centerY + y, glyph, color);
		Draw(centerX - x, centerY + y, glyph, color);
		Draw(centerX + x, centerY - y, glyph, color);
		Draw(centerX - x, centerY - y, glyph, color);

		if (x != y)
		{
			Draw(centerX + y, centerY + x, glyph, color);
			Draw(centerX - y, centerY + x, glyph, color);
			Draw(centerX + y, centerY - x, glyph, color);
			Draw(centerX - y, centerY - x, glyph, color);
		}

		if (p < 0)
		{
			p += 2 * x + 3;
		}
		else
		{
			p += 2 * (x - y) + 5;
			--y;
		}

		++x;
	}
}

void Cypher::TextRenderer::FillCircle(SHORT centerX, SHORT centerY, SHORT radius, SHORT glyph, SHORT color)
{
	if (radius <= 0)
		return;

	SHORT x = 0;
	SHORT y = radius;
	SHORT p = 3 - 2 * radius;

	auto drawScanLine = [&](SHORT startX, SHORT endX, SHORT y)
	{
		for (SHORT x = startX; x <= endX; ++x)
		{
			Draw(x, y, glyph, color);
		}
	};

	while (y >= x)
	{
		drawScanLine(centerX - x, centerX + x, centerY - y);
		drawScanLine(centerX - y, centerX + y, centerY - x);
		drawScanLine(centerX - x, centerX + x, centerY + y);
		drawScanLine(centerX - y, centerX + y, centerY + x);

		if (p < 0)
			p += 4 * x++ + 6;
		else
			p += 4 * (x++ - y--) + 10;
	}
};

void Cypher::TextRenderer::DrawTriangle(SHORT x0, SHORT y0, SHORT x1, SHORT y1, SHORT x2, SHORT y2, SHORT glyph, SHORT color)
{
	auto interpolate = [](SHORT x0, SHORT y0, SHORT x1, SHORT y1) -> std::vector<std::pair<SHORT, SHORT>>
	{
		std::vector<std::pair<SHORT, SHORT>> points;

		SHORT dx = std::abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
		SHORT dy = -std::abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
		SHORT err = dx + dy, e2;

		while (true)
		{
			points.push_back(std::make_pair(x0, y0));
			if (x0 == x1 && y0 == y1)
				break;

			e2 = 2 * err;
			
			if (e2 >= dy)
			{
				err += dy;
				x0 += sx;
			}
			if (e2 <= dx)
			{
				err += dx;
				y0 += sy;
			}
		}

		return points;
	};

	auto edge1 = interpolate(x0, y0, x1, y1);
	auto edge2 = interpolate(x1, y1, x2, y2);
	auto edge3 = interpolate(x2, y2, x0, y0);

	edge1.insert(edge1.end(), edge2.begin(), edge2.end());
	edge1.insert(edge1.end(), edge3.begin(), edge3.end());

	for (const auto& [x, y] : edge1)
	{
		Draw(x, y, glyph, color);
	}
}

void Cypher::TextRenderer::FillTriangle(SHORT x0, SHORT y0, SHORT x1, SHORT y1, SHORT x2, SHORT y2, SHORT glyph, SHORT color)
{
	if (y0 > y1)
		std::tie(x0, y0, x1, y1) = std::make_tuple(x1, y1, x0, y0);
	if (y0 > y2)
		std::tie(x0, y0, x2, y2) = std::make_tuple(x2, y2, x0, y0);
	if (y1 > y2)
		std::tie(x1, y1, x2, y2) = std::make_tuple(x2, y2, x1, y1);

	auto drawScanLine = [&](SHORT startX, SHORT endX, SHORT y)
	{
		for (SHORT x = startX; x <= endX; ++x)
		{
			Draw(x, y, glyph, color);
		}
	};

	auto [dx1, signx1, dy1] = std::make_tuple(std::abs(x1 - x0), (x1 - x0) > 0 ? 1 : -1, y1 - y0);
	auto [dx2, signx2, dy2] = std::make_tuple(std::abs(x2 - x0), (x2 - x0) > 0 ? 1 : -1, y2 - y0);

	bool changed1 = dy1 > dx1;
	bool changed2 = dy2 > dx2;

	if (changed1)
		std::swap(dx1, dy1);
	if (changed2)
		std::swap(dx2, dy2);

	auto [e1, e2] = std::make_tuple(dx1 >> 1, dx2 >> 1);
	SHORT t1x = x0, t2x = x0, y = y0;

	auto drawHalf = [&]()
	{
		for (SHORT i = 0; i <= dx1; ++i)
		{
			SHORT t1xp = 0;
			SHORT t2xp = 0;
			SHORT minx = std::min<SHORT>(t1x, t2x);
			SHORT maxx = std::max<SHORT>(t1x, t2x);

			while (i < dx1)
			{
				e1 += dy1;
				if (e1 >= dx1)
				{
					e1 -= dx1;
					if (!changed1)
						break;
					t1xp = signx1;
				}
				if (changed1)
					break;
				t1x += signx1;
				++i;
			}
			while (t2x != x2)
			{
				e2 += dy2;
				if (e2 >= dx2)
				{
					e2 -= dx2;
					if (!changed2)
						break;
					t2xp = signx2;
				}
				if (changed2)
					break;
				t2x += signx2;
			}
			minx = std::min<SHORT>(minx, std::min<SHORT>(t1x, t2x));
			maxx = std::max<SHORT>(maxx, std::max<SHORT>(t1x, t2x));
			drawScanLine(minx, maxx, y);

			t1x += (changed1 ? t1xp : signx1 + t1xp);
			t2x += (changed2 ? t2xp : signx2 + t2xp);
			y += 1;

			if (y == y1)
				break;
		}
	};

	drawHalf();
	
	if (y0 != y1)
	{	
		dx1 = std::abs(x2 - x1);
		dy1 = y2 - y1;
		if (changed1 = dy1 > dx1)
			std::swap(dy1, dx1);
	
		signx1 = (x2 - x1) > 0 ? 1 : -1;
		t1x = x1;
		e1 = dx1 >> 1;

		drawHalf();
	}
}

void Cypher::TextRenderer::DrawPolygon(const std::vector<Vector2f>& vertices, SHORT x, SHORT y, float rotation, float scale, SHORT glyph, SHORT color)
{
	if (vertices.empty())
		return;

	std::vector<Vector2f> transformedVerticeList;
	const size_t verticeCount = vertices.size();
	transformedVerticeList.reserve(verticeCount);

	const float cosTheta = cosf(rotation);
	const float sinTheta = sinf(rotation);

	for (const auto& vertex : vertices)
	{
		const float transformedX = (vertex.x() * cosTheta - vertex.y() * sinTheta) * scale + x;
		const float transformedY = (vertex.y() * cosTheta + vertex.x() * sinTheta) * scale + y;
		transformedVerticeList.emplace_back(transformedX, transformedY);
	}

	for (size_t i = 0; i < verticeCount; ++i)
	{
		size_t nextIndex = (i + 1) % verticeCount;
		DrawLine(static_cast<SHORT>(transformedVerticeList[i].x()),
					static_cast<SHORT>(transformedVerticeList[i].y()),
					static_cast<SHORT>(transformedVerticeList[nextIndex].x()),
					static_cast<SHORT>(transformedVerticeList[nextIndex].y()),
					glyph, color);
	}
}

void Cypher::TextRenderer::FillPolygon(const std::vector<Vector2f>& verticeList, SHORT x, SHORT y, float rotation, float scale, SHORT glyph, SHORT color)
{
	const float cosTheta = cosf(rotation);
	const float sinTheta = sinf(rotation);

	std::vector<Vector2f> transformedVertices;
	for (const auto& vertex : verticeList)
	{
		Vector2f transformed;
		transformed.x() = (vertex.x() * cosTheta - vertex.y() * sinTheta) * scale + x;
		transformed.y() = (vertex.x() * sinTheta + vertex.y() * cosTheta) * scale + y;
		transformedVertices.push_back(transformed);
	}

	auto drawScanLine = [&](SHORT startX, SHORT endX, SHORT y)
	{
		for (SHORT x = startX; x <= endX; ++x)
		{
			Draw(x, y, glyph, color);
		}
	};

	SHORT minX = 0;
	SHORT maxX = 0;
	SHORT minY = 0;
	SHORT maxY = 0;
	for (const auto& vertex : transformedVertices)
	{
		minX = std::min<SHORT>(minX, static_cast<SHORT>(vertex.x()));
		maxX = std::max<SHORT>(maxX, static_cast<SHORT>(vertex.x()));
		minY = std::min<SHORT>(minY, static_cast<SHORT>(vertex.y()));
		maxY = std::max<SHORT>(maxY, static_cast<SHORT>(vertex.y()));
	}

	for (SHORT scanY = minY; scanY <= maxY; ++scanY)
	{
		std::vector<SHORT> nodeX;
		for (size_t i = 0, j = transformedVertices.size() - 1; i < transformedVertices.size(); j = i++)
		{
			const auto& vCurrent = transformedVertices[i];
			const auto& vPrevious = transformedVertices[j];

			if (vCurrent.y() < scanY && vPrevious.y() >= scanY || vPrevious.y() < scanY && vCurrent.y() >= scanY)
			{
				float slope = (vPrevious.x() - vCurrent.x()) / (vPrevious.y() - vCurrent.y());
				float deltaX = slope * (scanY - vCurrent.y());
				SHORT node = static_cast<SHORT>(vCurrent.x() + deltaX);
				nodeX.push_back(node);
			}
		}

		std::sort(nodeX.begin(), nodeX.end());

		for (size_t i = 0; i + 1 < nodeX.size(); i += 2)
		{
			const auto& vCurrent = nodeX[i];
			if (vCurrent > maxX)
				continue;
			const auto& vNext = nodeX[i + 1];
			if (vNext > minX)
			{
				SHORT startX = std::max<SHORT>(minX, vCurrent);
				SHORT startY = std::min<SHORT>(maxX, vNext);
				drawScanLine(startX, startY, scanY);
			}
		}
	}
}

void Cypher::TextRenderer::Clip(SHORT& x, SHORT& y)
{
	x = (x < 0) ? x = 0 :
		 (x >= m_screenWidth) ? x = m_screenWidth : x;
	y = (y < 0) ? y = 0 :
		 (y >= m_screenHeight) ? y = m_screenHeight : y;
}
