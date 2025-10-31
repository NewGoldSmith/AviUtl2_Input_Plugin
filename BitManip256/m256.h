/**
 * @file m256.h
 * @brief m256共用体の宣言
 * @author Gold Smith
 * @date 2024
 * SPDX-License-Identifier: MIT
 * Released under the MIT license
 * https: //opensource.org/licenses/mit-license.php
 *
 * このファイル内のすべてのコードは、特に明記されていない限り、MITライセンスに従います。
 */
#pragma once
#define NOMINMAX
#include <immintrin.h>
#include <emmintrin.h>
#include <stdint.h>
#include <ranges>
#if !defined(_M_X64) && !defined(__amd64__)
#error "This code must be compiled in x64 configuration."
#endif
#ifndef __AVX2__
#error "This code requires AVX2."
#endif
namespace bit_manip
{
   inline bool operator==(const __m256i a, const __m256i b) noexcept {
      __m256i xor_result = _mm256_xor_si256(a, b);
      return _mm256_testz_si256(xor_result, xor_result);
   }

   inline bool operator!=(const __m256i a, const __m256i b) noexcept {
      return !(a == b);
   }

   inline bool IsNonZero256(const __m256i a) noexcept{
       return !_mm256_testz_si256(a, a);
   }

   inline bool IsZero256(const __m256i a) noexcept{
      return _mm256_testz_si256(a, a);
   }

   union [[maybe_unused]] __declspec(intrin_type) __declspec(align(32)) m256{
      __m256i m256i;
      struct{
         /// @brief 黒固定。
         uint64_t B;
         /// @brief 白固定。
         uint64_t W;
         /// @brief move固定。複数の選択肢が入る場合もある。
         uint64_t M;
         int16_t score;
         uint16_t ID;
         uint8_t depth;
         uint8_t turn;
         /// @brief 現在の手番の判別用。
         bool bIsCurrentTurnWhite : 1;
         /// @brief Ponderで探索されたボードか否か。
         bool bIsPonderBoard : 1;
      };

      // コンストラクタと演算子
      inline m256() noexcept = default;
      inline constexpr m256(const m256& m) noexcept = default;
      inline constexpr m256(m256&&) noexcept = default;
      inline constexpr m256(const __m256i& m) noexcept{ m256i = m; }
      inline constexpr m256& operator=(const m256& m) noexcept = default;
      inline constexpr m256& operator=(m256&& m) noexcept = default;
      inline constexpr m256& operator=(__m256i m) noexcept{
         m256i = m;
         return *this;
      }

      // m256同士の比較演算子
      friend inline bool operator==(const m256& a,const m256& b) noexcept{
         return _mm256_testz_si256(_mm256_xor_si256(a.m256i,b.m256i),_mm256_xor_si256(a.m256i,b.m256i));
      }

      // キャスト演算子
      inline constexpr operator const __m256i ()const noexcept{ return m256i; }

      inline operator bool() const noexcept{
         return !_mm256_testz_si256(m256i,m256i);
      }
   };

   template <typename T>
   concept IsM256Container =
      std::ranges::range<T>
      && std::same_as<typename T::value_type,bit_manip::m256>;

   template <typename T>
   concept HasScore = requires(T u){
      { u.score }->std::convertible_to<decltype(bit_manip::m256::score)>;
   };

   template <typename T>
   concept HasIsCurrentTurnWhite = requires(T u){
      { u.bIsCurrentTurnWhite }->std::convertible_to<decltype(bit_manip::m256::bIsCurrentTurnWhite)>;
   };

   template <typename T>
   concept HasTurn = requires(T u) {
      { u.turn } -> std::convertible_to<decltype(bit_manip::m256::turn)>;
   };

   template <typename T>
   concept HasDepth = requires(T t) {
      {t.depth}-> std::convertible_to<decltype(bit_manip::m256::depth)>;
   };

   union m128{
      __m128i m{};
      struct{
         uint64_t B;
         uint64_t W;
      };
   };


}

