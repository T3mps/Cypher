#pragma once

#include <cmath>
#include <limits>
#include <stdexcept>
#include <tuple>
#include <type_traits>

#include "Common.h"

namespace Cypher
{
   template <typename T = double>
   class Vector2
   {
   public:
      static_assert(std::is_arithmetic<T>::value, "Vector2 requires a numerical type");

      static constexpr size_t ELEMENT_COUNT = 2;

      T x;
      T y;

      Vector2() = default;
      Vector2(T xVal, T yVal) : x(xVal), y(yVal) {}

      Vector2<T> operator+(const Vector2<T>& rhs) const { return Vector2<T>(x + rhs.x, y + rhs.y); }
      Vector2<T> operator+(T scalar) const { return Vector2<T>(x + scalar, y + scalar); }
      Vector2<T> operator-(const Vector2<T>& rhs) const { return Vector2<T>(x - rhs.x, y - rhs.y); }
      Vector2<T> operator-(T scalar) const { return Vector2<T>(x - scalar, y - scalar); }
      Vector2<T> operator*(const Vector2<T>& rhs) const { return Vector2<T>(x * rhs.x, y * rhs.y); }
      Vector2<T> operator*(T scalar) const { return Vector2<T>(x * scalar, y * scalar); }
      Vector2<T> operator/(const Vector2<T>& rhs) const { return Vector2<T>(x / rhs.x, y / rhs.y); }
      Vector2<T> operator/(T scalar) const { return Vector2<T>(x / scalar, y / scalar); }

      Vector2<T>& operator+=(const Vector2<T>& rhs)
      {
         x += rhs.x;
         y += rhs.y;
         return *this;
      }

      Vector2<T>& operator+=(T scalar)
      {
         x += scalar;
         y += scalar;
         return *this;
      }

      Vector2<T>& operator-=(const Vector2<T>& rhs)
      {
         x -= rhs.x;
         y -= rhs.y;
         return *this;
      }

      Vector2<T>& operator-=(T scalar)
      {
         x -= scalar;
         y -= scalar;
         return *this;
      }

      Vector2<T>& operator*=(const Vector2<T>& rhs)
      {
         x *= rhs.x;
         y *= rhs.y;
         return *this;
      }

      Vector2<T>& operator*=(T scalar)
      {
         x *= scalar;
         y *= scalar;
         return *this;
      }

      Vector2<T>& operator/=(const Vector2<T>& rhs)
      {
         x /= rhs.x;
         y /= rhs.y;
         return *this;
      }

      Vector2<T>& operator/=(T scalar)
      {
         x /= scalar;
         y /= scalar;
         return *this;
      }

      Vector2<T> operator-() { return Vector2<T>(-x, -y); }
      
      T operator[](int index)
      {
         switch (index)
         {
            case 0: return x;
            case 1: return y;
            default: throw std::out_of_range("Index out of range for Vector2");
         }
      }

      const T operator[](int index) const
      {
         switch (index)
         {
            case 0: return x;
            case 1: return y;
            default: throw std::out_of_range("Index out of range for Vector2");
         }
      }

      bool operator==(const Vector2<T>& rhs) const
      {
         if constexpr (std::is_integral<T>::value)
         {
            return x == rhs.x && y == rhs.y;
         }
         else
         {
            const T epsilon = static_cast<T>(1e-5);
            return std::abs(x - rhs.x) < epsilon && std::abs(y - rhs.y) < epsilon;
         }
      }

      bool operator!=(const Vector2<T>& rhs) const { return !(*this == rhs); }

      Vector2<T>& Negate()
      {
         x = -x;
         y = -y;
         return *this;
      }

      T Dot(const Vector2<T>& rhs) const { return Dot(rhs.x, rhs.y); }
      T Dot(T xVal, T yVal) const { return x * xVal + y * yVal; }

      T Cross(const Vector2<T>& rhs) const { return Cross(rhs.x, rhs.y); }
      T Cross(T xVal, T yVal) const { return x * yVal - y * xVal; }

      Vector2<T> TripleProduct(const Vector2<T>& b, const Vector2<T>& c) const
      {
         // Compute the determinant (or "2D cross product") of this vector and b
         T dot = this->x * b.y - b.x * this->y;

         // Compute the result vector based on the formula
         Vector2<T> result;
         result.x = -c.y * dot;
         result.y = c.x * dot;

         return result;
      }

      T Magnitude() const
      {
         using IntermediateType = std::conditional_t<std::is_integral<T>::value, double, T>;
         IntermediateType squaredSum = static_cast<IntermediateType>(x) * x + static_cast<IntermediateType>(y) * y;
         return static_cast<T>(std::sqrt(squaredSum));
      }

      constexpr T MagnitudeSquared() const { return x * x + y * y; }

      Vector2<T> Normalized() const
      {
         Vector2 temp = *this;
         temp.Normalize();
         return temp;
      }

      void Normalize()
      {
         using IntermediateType = std::conditional_t<std::is_integral<T>::value, double, T>;
         IntermediateType mag2 = static_cast<IntermediateType>(MagnitudeSquared());
         if (mag2 > 0)
         {
            IntermediateType mag = std::sqrt(mag2);
            x = static_cast<T>(static_cast<IntermediateType>(x) / mag);
            y = static_cast<T>(static_cast<IntermediateType>(y) / mag);
         }
      }

      T Distance(T xVal, T yVal)
      {
         using IntermediateType = std::conditional_t<std::is_integral<T>::value, double, T>;
         IntermediateType dx = static_cast<IntermediateType>(x) - xVal;
         IntermediateType dy = static_cast<IntermediateType>(y) - yVal;
         IntermediateType dist = std::sqrt(dx * dx + dy * dy);
         return static_cast<T>(dist);
      }

      T Distance(const Vector2<T>& rhs) { return Distance(rhs.x, rhs.y); }

      T DistanceSquared(T xVal, T yVal) const
      {
         T dx = x - xVal;
         T dy = y - yVal;
         return dx * dx + dy * dy;
      }

      T DistanceSquared(const Vector2<T>& rhs) const { return DistanceSquared(rhs.x, rhs.y); }
      
      static Vector2<T> Zero() { return Vector2<T>(0, 0); }
      static Vector2<T> One() { return Vector2<T>(1, 1); }
      
      static Vector2<T> Up() { return Vector2<T>(0, 1); }
      static Vector2<T> Down() { return Vector2<T>(0, -1); }
      static Vector2<T> Left() { return Vector2<T>(1, 0); }
      static Vector2<T> Right() { return Vector2<T>(-1, 0); }
   };

   using Vector2i = Vector2<int>;
   using Vector2f = Vector2<float>;
   using Vector2d = Vector2<double>;
} // namespace Cypher

namespace std
{
   template <typename T>
   struct tuple_size<Cypher::Vector2<T>> : integral_constant<size_t, Cypher::Vector2<T>::ELEMENT_COUNT> {};

   template <size_t N, typename T>
   struct tuple_element<N, Cypher::Vector2<T>> { using type = T; };

   template <size_t N, typename T>
   constexpr T& get(Cypher::Vector2<T>& vector)
   {
      static_assert(N < Cypher::Vector2<T>::ELEMENT_COUNT, "Index out of bounds");
      if constexpr (N == 0)
         return vector.x;
      else
         return vector.y;
   }

   template <size_t N, typename T>
   constexpr const T& get(const Cypher::Vector2<T>& vector)
   {
      static_assert(N < Cypher::Vector2<T>::ELEMENT_COUNT, "Index out of bounds");
      if constexpr (N == 0)
         return vector.x;
      else
         return vector.y;
   }
} // namespace std
