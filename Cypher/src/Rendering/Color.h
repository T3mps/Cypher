#pragma once

#include <type_traits>

#include "Common.h"

namespace Cypher
{
	namespace Color
	{
		enum class Foreground : short
		{
			BLACK			 = 0x0000,
			DARK_BLUE	 = 0x0001,
			DARK_GREEN	 = 0x0002,
			DARK_CYAN	 = 0x0003,
			DARK_RED		 = 0x0004,
			PURPLE		 = 0x0005,
			DARK_YELLOW	 = 0x0006,
			GREY			 = 0x0007,
			DARK_GREY	 = 0x0008,
			BLUE			 = 0x0009,
			GREEN			 = 0x000A,
			CYAN			 = 0x000B,
			RED			 = 0x000C,
			MAGENTA		 = 0x000D,
			YELLOW		 = 0x000E,
			WHITE			 = 0x000F,
		};
		using FG = Foreground;

		enum class Background : short
		{
			BLACK			 = 0x0000,
			DARK_BLUE	 = 0x0010,
			DARK_GREEN	 = 0x0020,
			DARK_CYAN	 = 0x0030,
			DARK_RED		 = 0x0040,
			PURPLE		 = 0x0050,
			DARK_YELLOW	 = 0x0060,
			GREY			 = 0x0070,
			DARK_GREY	 = 0x0080,
			BLUE			 = 0x0090,
			GREEN			 = 0x00A0,
			CYAN			 = 0x00B0,
			RED			 = 0x00C0,
			MAGENTA		 = 0x00D0,
			YELLOW		 = 0x00E0,
			WHITE			 = 0x00F0
		};
		using BG = Background;

		inline short operator+(Foreground fg) { return static_cast<short>(fg); }
		inline short operator+(Background bg) { return static_cast<short>(bg); }

		inline Foreground& operator++(Foreground& fg)
		{
			fg = static_cast<Foreground>((static_cast<short>(fg) + 1) % 16); // 16 is the number of values in Foreground
			return fg;
		}
		inline Background& operator++(Background& bg)
		{
			bg = static_cast<Background>((static_cast<short>(bg) + 1) % 16); // 16 is the number of values in Foreground
			return bg;
		}

		inline Foreground& operator--(Foreground& fg)
		{
			if (fg == Foreground::BLACK)
				fg = Foreground::WHITE;
			else
				fg = static_cast<Foreground>(static_cast<short>(fg) - 1);
			return fg;
		}
		inline Background& operator--(Background& bg)
		{
			if (bg == Background::BLACK)
				bg = Background::WHITE;
			else
				bg = static_cast<Background>(static_cast<short>(bg) - 1);
			return bg;
		}

		struct Compound
		{
			Foreground foreground;
			Background background;

			Compound(Foreground foreground = Foreground::WHITE, Background background = Background::BLACK) :
				foreground(foreground),
				background(background)
			{
			}

			operator short() const { return static_cast<short>(foreground) | static_cast<short>(background); }
		
			Compound& operator|(const Foreground& rhs)
			{
				foreground = rhs;
				return *this;
			}

			Compound& operator|(const Background& rhs)
			{
				background = rhs;
				return *this;
			}

			Foreground GetForeground() const { return foreground; }
			Background GetBackground() const { return background; }
		};

		inline Compound operator|(Foreground lhs, Background rhs) { return Compound(lhs, rhs); }
		inline Compound operator|(Background lhs, Foreground rhs) { return Compound(rhs, lhs); }

		template<typename T>
		concept ValidType = std::same_as<T, Compound> || std::same_as<T, Color::Foreground> || std::same_as<T, Color::Background> || std::same_as<T, int> || std::same_as<T, short>;
	}
} // namespace Cypher
