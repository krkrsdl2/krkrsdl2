

#ifndef __SIMDE_RENAMES_H__
#define __SIMDE_RENAMES_H__

#ifdef __cplusplus
#include <cmath>
#endif
#include <math.h>

#if defined(__vita__) || defined(__SWITCH__)
#include <simde/simde/simde-common.h>
#undef SIMDE_HAVE_FENV_H
#endif

// These defines are mainly for documentation purposes
#ifndef KRKRSDL2_SIMD_RENAME_SSE
#define KRKRSDL2_SIMD_RENAME_SSE
#endif
#ifndef KRKRSDL2_SIMD_RENAME_SSE2
#define KRKRSDL2_SIMD_RENAME_SSE2
#endif
#ifndef KRKRSDL2_SIMD_RENAME_SSE3
#define KRKRSDL2_SIMD_RENAME_SSE3
#endif
#ifndef KRKRSDL2_SIMD_RENAME_SSSE3
#define KRKRSDL2_SIMD_RENAME_SSSE3
#endif
#ifndef KRKRSDL2_SIMD_RENAME_SSE4_1
#define KRKRSDL2_SIMD_RENAME_SSE4_1
#endif
#ifndef KRKRSDL2_SIMD_RENAME_AVX
#define KRKRSDL2_SIMD_RENAME_AVX
#endif
#ifndef KRKRSDL2_SIMD_RENAME_AVX2
#define KRKRSDL2_SIMD_RENAME_AVX2
#endif
#ifndef KRKRSDL2_SIMD_RENAME_FMA
#define KRKRSDL2_SIMD_RENAME_FMA
#endif

#ifdef KRKRSDL2_SIMD_RENAME_SSE
#include <simde/x86/sse.h>
#endif
#ifdef KRKRSDL2_SIMD_RENAME_SSE2
#include <simde/x86/sse2.h>
#endif
#ifdef KRKRSDL2_SIMD_RENAME_SSE3
#include <simde/x86/sse3.h>
#endif
#ifdef KRKRSDL2_SIMD_RENAME_SSSE3
#include <simde/x86/ssse3.h>
#endif
#ifdef KRKRSDL2_SIMD_RENAME_SSE4_1
#include <simde/x86/sse4.1.h>
#endif
#ifdef KRKRSDL2_SIMD_RENAME_AVX
#include <simde/x86/avx.h>
#endif
#ifdef KRKRSDL2_SIMD_RENAME_AVX2
#include <simde/x86/avx2.h>
#endif
#ifdef KRKRSDL2_SIMD_RENAME_FMA
#include <simde/x86/fma.h>
#endif



#if defined(_CMP_GE_OS) && !defined(SIMDE_CMP_GE_OS)
#undef _CMP_GE_OS
#endif
#ifndef _CMP_GE_OS
#define _CMP_GE_OS SIMDE_CMP_GE_OS
#endif
#if defined(_CMP_GT_OS) && !defined(SIMDE_CMP_GT_OS)
#undef _CMP_GT_OS
#endif
#ifndef _CMP_GT_OS
#define _CMP_GT_OS SIMDE_CMP_GT_OS
#endif
#if defined(_CMP_LE_OS) && !defined(SIMDE_CMP_LE_OS)
#undef _CMP_LE_OS
#endif
#ifndef _CMP_LE_OS
#define _CMP_LE_OS SIMDE_CMP_LE_OS
#endif
#if defined(_CMP_LT_OS) && !defined(SIMDE_CMP_LT_OS)
#undef _CMP_LT_OS
#endif
#ifndef _CMP_LT_OS
#define _CMP_LT_OS SIMDE_CMP_LT_OS
#endif
#if defined(_MM_ROUND_NEAREST) && !defined(SIMDE_MM_ROUND_NEAREST)
#undef _MM_ROUND_NEAREST
#endif
#ifndef _MM_ROUND_NEAREST
#define _MM_ROUND_NEAREST SIMDE_MM_ROUND_NEAREST
#endif
#if defined(_MM_SET_ROUNDING_MODE) && !defined(SIMDE_MM_SET_ROUNDING_MODE)
#undef _MM_SET_ROUNDING_MODE
#endif
#ifndef _MM_SET_ROUNDING_MODE
#define _MM_SET_ROUNDING_MODE SIMDE_MM_SET_ROUNDING_MODE
#endif
#if defined(_MM_SHUFFLE) && !defined(SIMDE_MM_SHUFFLE)
#undef _MM_SHUFFLE
#endif
#ifndef _MM_SHUFFLE
#define _MM_SHUFFLE SIMDE_MM_SHUFFLE
#endif
#if defined(__m128) && !defined(simde__m128)
#undef __m128
#endif
#ifndef __m128
#define __m128 simde__m128
#endif
#if defined(__m128d) && !defined(simde__m128d)
#undef __m128d
#endif
#ifndef __m128d
#define __m128d simde__m128d
#endif
#if defined(__m128i) && !defined(simde__m128i)
#undef __m128i
#endif
#ifndef __m128i
#define __m128i simde__m128i
#endif
#if defined(__m256) && !defined(simde__m256)
#undef __m256
#endif
#ifndef __m256
#define __m256 simde__m256
#endif
#if defined(__m256i) && !defined(simde__m256i)
#undef __m256i
#endif
#ifndef __m256i
#define __m256i simde__m256i
#endif
#if defined(__m64) && !defined(simde__m64)
#undef __m64
#endif
#ifndef __m64
#define __m64 simde__m64
#endif
#if defined(_mm256_add_epi16) && !defined(simde_mm256_add_epi16)
#undef _mm256_add_epi16
#endif
#ifndef _mm256_add_epi16
#define _mm256_add_epi16 simde_mm256_add_epi16
#endif
#if defined(_mm256_add_epi32) && !defined(simde_mm256_add_epi32)
#undef _mm256_add_epi32
#endif
#ifndef _mm256_add_epi32
#define _mm256_add_epi32 simde_mm256_add_epi32
#endif
#if defined(_mm256_add_epi8) && !defined(simde_mm256_add_epi8)
#undef _mm256_add_epi8
#endif
#ifndef _mm256_add_epi8
#define _mm256_add_epi8 simde_mm256_add_epi8
#endif
#if defined(_mm256_add_ps) && !defined(simde_mm256_add_ps)
#undef _mm256_add_ps
#endif
#ifndef _mm256_add_ps
#define _mm256_add_ps simde_mm256_add_ps
#endif
#if defined(_mm256_adds_epi16) && !defined(simde_mm256_adds_epi16)
#undef _mm256_adds_epi16
#endif
#ifndef _mm256_adds_epi16
#define _mm256_adds_epi16 simde_mm256_adds_epi16
#endif
#if defined(_mm256_adds_epu16) && !defined(simde_mm256_adds_epu16)
#undef _mm256_adds_epu16
#endif
#ifndef _mm256_adds_epu16
#define _mm256_adds_epu16 simde_mm256_adds_epu16
#endif
#if defined(_mm256_adds_epu8) && !defined(simde_mm256_adds_epu8)
#undef _mm256_adds_epu8
#endif
#ifndef _mm256_adds_epu8
#define _mm256_adds_epu8 simde_mm256_adds_epu8
#endif
#if defined(_mm256_and_ps) && !defined(simde_mm256_and_ps)
#undef _mm256_and_ps
#endif
#ifndef _mm256_and_ps
#define _mm256_and_ps simde_mm256_and_ps
#endif
#if defined(_mm256_and_si256) && !defined(simde_mm256_and_si256)
#undef _mm256_and_si256
#endif
#ifndef _mm256_and_si256
#define _mm256_and_si256 simde_mm256_and_si256
#endif
#if defined(_mm256_andnot_ps) && !defined(simde_mm256_andnot_ps)
#undef _mm256_andnot_ps
#endif
#ifndef _mm256_andnot_ps
#define _mm256_andnot_ps simde_mm256_andnot_ps
#endif
#if defined(_mm256_andnot_si256) && !defined(simde_mm256_andnot_si256)
#undef _mm256_andnot_si256
#endif
#ifndef _mm256_andnot_si256
#define _mm256_andnot_si256 simde_mm256_andnot_si256
#endif
#if defined(_mm256_castsi256_ps) && !defined(simde_mm256_castsi256_ps)
#undef _mm256_castsi256_ps
#endif
#ifndef _mm256_castsi256_ps
#define _mm256_castsi256_ps simde_mm256_castsi256_ps
#endif
#if defined(_mm256_cmp_ps) && !defined(simde_mm256_cmp_ps)
#undef _mm256_cmp_ps
#endif
#ifndef _mm256_cmp_ps
#define _mm256_cmp_ps simde_mm256_cmp_ps
#endif
#if defined(_mm256_cmpeq_epi32) && !defined(simde_mm256_cmpeq_epi32)
#undef _mm256_cmpeq_epi32
#endif
#ifndef _mm256_cmpeq_epi32
#define _mm256_cmpeq_epi32 simde_mm256_cmpeq_epi32
#endif
#if defined(_mm256_cvtepi32_ps) && !defined(simde_mm256_cvtepi32_ps)
#undef _mm256_cvtepi32_ps
#endif
#ifndef _mm256_cvtepi32_ps
#define _mm256_cvtepi32_ps simde_mm256_cvtepi32_ps
#endif
#if defined(_mm256_cvtepu8_epi16) && !defined(simde_mm256_cvtepu8_epi16)
#undef _mm256_cvtepu8_epi16
#endif
#ifndef _mm256_cvtepu8_epi16
#define _mm256_cvtepu8_epi16 simde_mm256_cvtepu8_epi16
#endif
#if defined(_mm256_cvtps_epi32) && !defined(simde_mm256_cvtps_epi32)
#undef _mm256_cvtps_epi32
#endif
#ifndef _mm256_cvtps_epi32
#define _mm256_cvtps_epi32 simde_mm256_cvtps_epi32
#endif
#if defined(_mm256_cvttps_epi32) && !defined(simde_mm256_cvttps_epi32)
#undef _mm256_cvttps_epi32
#endif
#ifndef _mm256_cvttps_epi32
#define _mm256_cvttps_epi32 simde_mm256_cvttps_epi32
#endif
#if defined(_mm256_extracti128_si256) && !defined(simde_mm256_extracti128_si256)
#undef _mm256_extracti128_si256
#endif
#ifndef _mm256_extracti128_si256
#define _mm256_extracti128_si256 simde_mm256_extracti128_si256
#endif
#if defined(_mm256_fnmadd_ps) && !defined(simde_mm256_fnmadd_ps)
#undef _mm256_fnmadd_ps
#endif
#ifndef _mm256_fnmadd_ps
#define _mm256_fnmadd_ps simde_mm256_fnmadd_ps
#endif
#if defined(_mm256_hadd_ps) && !defined(simde_mm256_hadd_ps)
#undef _mm256_hadd_ps
#endif
#ifndef _mm256_hadd_ps
#define _mm256_hadd_ps simde_mm256_hadd_ps
#endif
#if defined(_mm256_load_ps) && !defined(simde_mm256_load_ps)
#undef _mm256_load_ps
#endif
#ifndef _mm256_load_ps
#define _mm256_load_ps simde_mm256_load_ps
#endif
#if defined(_mm256_loadu_ps) && !defined(simde_mm256_loadu_ps)
#undef _mm256_loadu_ps
#endif
#ifndef _mm256_loadu_ps
#define _mm256_loadu_ps simde_mm256_loadu_ps
#endif
#if defined(_mm256_loadu_si256) && !defined(simde_mm256_loadu_si256)
#undef _mm256_loadu_si256
#endif
#ifndef _mm256_loadu_si256
#define _mm256_loadu_si256 simde_mm256_loadu_si256
#endif
#if defined(_mm256_maskstore_epi32) && !defined(simde_mm256_maskstore_epi32)
#undef _mm256_maskstore_epi32
#endif
#ifndef _mm256_maskstore_epi32
#define _mm256_maskstore_epi32 simde_mm256_maskstore_epi32
#endif
#if defined(_mm256_max_ps) && !defined(simde_mm256_max_ps)
#undef _mm256_max_ps
#endif
#ifndef _mm256_max_ps
#define _mm256_max_ps simde_mm256_max_ps
#endif
#if defined(_mm256_min_ps) && !defined(simde_mm256_min_ps)
#undef _mm256_min_ps
#endif
#ifndef _mm256_min_ps
#define _mm256_min_ps simde_mm256_min_ps
#endif
#if defined(_mm256_mul_ps) && !defined(simde_mm256_mul_ps)
#undef _mm256_mul_ps
#endif
#ifndef _mm256_mul_ps
#define _mm256_mul_ps simde_mm256_mul_ps
#endif
#if defined(_mm256_mulhi_epi16) && !defined(simde_mm256_mulhi_epi16)
#undef _mm256_mulhi_epi16
#endif
#ifndef _mm256_mulhi_epi16
#define _mm256_mulhi_epi16 simde_mm256_mulhi_epi16
#endif
#if defined(_mm256_mulhi_epu16) && !defined(simde_mm256_mulhi_epu16)
#undef _mm256_mulhi_epu16
#endif
#ifndef _mm256_mulhi_epu16
#define _mm256_mulhi_epu16 simde_mm256_mulhi_epu16
#endif
#if defined(_mm256_mullo_epi16) && !defined(simde_mm256_mullo_epi16)
#undef _mm256_mullo_epi16
#endif
#ifndef _mm256_mullo_epi16
#define _mm256_mullo_epi16 simde_mm256_mullo_epi16
#endif
#if defined(_mm256_or_ps) && !defined(simde_mm256_or_ps)
#undef _mm256_or_ps
#endif
#ifndef _mm256_or_ps
#define _mm256_or_ps simde_mm256_or_ps
#endif
#if defined(_mm256_or_si256) && !defined(simde_mm256_or_si256)
#undef _mm256_or_si256
#endif
#ifndef _mm256_or_si256
#define _mm256_or_si256 simde_mm256_or_si256
#endif
#if defined(_mm256_packs_epi32) && !defined(simde_mm256_packs_epi32)
#undef _mm256_packs_epi32
#endif
#ifndef _mm256_packs_epi32
#define _mm256_packs_epi32 simde_mm256_packs_epi32
#endif
#if defined(_mm256_packus_epi16) && !defined(simde_mm256_packus_epi16)
#undef _mm256_packus_epi16
#endif
#ifndef _mm256_packus_epi16
#define _mm256_packus_epi16 simde_mm256_packus_epi16
#endif
#if defined(_mm256_packus_epi32) && !defined(simde_mm256_packus_epi32)
#undef _mm256_packus_epi32
#endif
#ifndef _mm256_packus_epi32
#define _mm256_packus_epi32 simde_mm256_packus_epi32
#endif
#if defined(_mm256_permute2f128_ps) && !defined(simde_mm256_permute2f128_ps)
#undef _mm256_permute2f128_ps
#endif
#ifndef _mm256_permute2f128_ps
#define _mm256_permute2f128_ps simde_mm256_permute2f128_ps
#endif
#if defined(_mm256_permute2x128_si256) && !defined(simde_mm256_permute2x128_si256)
#undef _mm256_permute2x128_si256
#endif
#ifndef _mm256_permute2x128_si256
#define _mm256_permute2x128_si256 simde_mm256_permute2x128_si256
#endif
#if defined(_mm256_permute4x64_epi64) && !defined(simde_mm256_permute4x64_epi64)
#undef _mm256_permute4x64_epi64
#endif
#ifndef _mm256_permute4x64_epi64
#define _mm256_permute4x64_epi64 simde_mm256_permute4x64_epi64
#endif
#if defined(_mm256_rcp_ps) && !defined(simde_mm256_rcp_ps)
#undef _mm256_rcp_ps
#endif
#ifndef _mm256_rcp_ps
#define _mm256_rcp_ps simde_mm256_rcp_ps
#endif
#if defined(_mm256_set1_epi16) && !defined(simde_mm256_set1_epi16)
#undef _mm256_set1_epi16
#endif
#ifndef _mm256_set1_epi16
#define _mm256_set1_epi16 simde_mm256_set1_epi16
#endif
#if defined(_mm256_set1_epi32) && !defined(simde_mm256_set1_epi32)
#undef _mm256_set1_epi32
#endif
#ifndef _mm256_set1_epi32
#define _mm256_set1_epi32 simde_mm256_set1_epi32
#endif
#if defined(_mm256_set1_ps) && !defined(simde_mm256_set1_ps)
#undef _mm256_set1_ps
#endif
#ifndef _mm256_set1_ps
#define _mm256_set1_ps simde_mm256_set1_ps
#endif
#if defined(_mm256_set_epi32) && !defined(simde_mm256_set_epi32)
#undef _mm256_set_epi32
#endif
#ifndef _mm256_set_epi32
#define _mm256_set_epi32 simde_mm256_set_epi32
#endif
#if defined(_mm256_set_ps) && !defined(simde_mm256_set_ps)
#undef _mm256_set_ps
#endif
#ifndef _mm256_set_ps
#define _mm256_set_ps simde_mm256_set_ps
#endif
#if defined(_mm256_setzero_ps) && !defined(simde_mm256_setzero_ps)
#undef _mm256_setzero_ps
#endif
#ifndef _mm256_setzero_ps
#define _mm256_setzero_ps simde_mm256_setzero_ps
#endif
#if defined(_mm256_setzero_si256) && !defined(simde_mm256_setzero_si256)
#undef _mm256_setzero_si256
#endif
#ifndef _mm256_setzero_si256
#define _mm256_setzero_si256 simde_mm256_setzero_si256
#endif
#if defined(_mm256_shuffle_epi32) && !defined(simde_mm256_shuffle_epi32)
#undef _mm256_shuffle_epi32
#endif
#ifndef _mm256_shuffle_epi32
#define _mm256_shuffle_epi32 simde_mm256_shuffle_epi32
#endif
#ifndef __AVX2__
#if defined(_mm256_shufflehi_epi16) && !defined(simde_mm256_shufflehi_epi16)
#undef _mm256_shufflehi_epi16
#endif
#ifndef _mm256_shufflehi_epi16
#define _mm256_shufflehi_epi16 simde_mm256_shufflehi_epi16
#endif
#if defined(_mm256_shufflelo_epi16) && !defined(simde_mm256_shufflelo_epi16)
#undef _mm256_shufflelo_epi16
#endif
#ifndef _mm256_shufflelo_epi16
#define _mm256_shufflelo_epi16 simde_mm256_shufflelo_epi16
#endif
#endif
#if defined(_mm256_slli_epi16) && !defined(simde_mm256_slli_epi16)
#undef _mm256_slli_epi16
#endif
#ifndef _mm256_slli_epi16
#define _mm256_slli_epi16 simde_mm256_slli_epi16
#endif
#if defined(_mm256_slli_epi32) && !defined(simde_mm256_slli_epi32)
#undef _mm256_slli_epi32
#endif
#ifndef _mm256_slli_epi32
#define _mm256_slli_epi32 simde_mm256_slli_epi32
#endif
#if defined(_mm256_srai_epi16) && !defined(simde_mm256_srai_epi16)
#undef _mm256_srai_epi16
#endif
#ifndef _mm256_srai_epi16
#define _mm256_srai_epi16 simde_mm256_srai_epi16
#endif
#if defined(_mm256_srli_epi16) && !defined(simde_mm256_srli_epi16)
#undef _mm256_srli_epi16
#endif
#ifndef _mm256_srli_epi16
#define _mm256_srli_epi16 simde_mm256_srli_epi16
#endif
#if defined(_mm256_srli_epi32) && !defined(simde_mm256_srli_epi32)
#undef _mm256_srli_epi32
#endif
#ifndef _mm256_srli_epi32
#define _mm256_srli_epi32 simde_mm256_srli_epi32
#endif
#if defined(_mm256_srli_epi64) && !defined(simde_mm256_srli_epi64)
#undef _mm256_srli_epi64
#endif
#ifndef _mm256_srli_epi64
#define _mm256_srli_epi64 simde_mm256_srli_epi64
#endif
#if defined(_mm256_store_ps) && !defined(simde_mm256_store_ps)
#undef _mm256_store_ps
#endif
#ifndef _mm256_store_ps
#define _mm256_store_ps simde_mm256_store_ps
#endif
#if defined(_mm256_store_si256) && !defined(simde_mm256_store_si256)
#undef _mm256_store_si256
#endif
#ifndef _mm256_store_si256
#define _mm256_store_si256 simde_mm256_store_si256
#endif
#if defined(_mm256_storeu_ps) && !defined(simde_mm256_storeu_ps)
#undef _mm256_storeu_ps
#endif
#ifndef _mm256_storeu_ps
#define _mm256_storeu_ps simde_mm256_storeu_ps
#endif
#if defined(_mm256_storeu_si256) && !defined(simde_mm256_storeu_si256)
#undef _mm256_storeu_si256
#endif
#ifndef _mm256_storeu_si256
#define _mm256_storeu_si256 simde_mm256_storeu_si256
#endif
#if defined(_mm256_sub_epi16) && !defined(simde_mm256_sub_epi16)
#undef _mm256_sub_epi16
#endif
#ifndef _mm256_sub_epi16
#define _mm256_sub_epi16 simde_mm256_sub_epi16
#endif
#if defined(_mm256_sub_epi32) && !defined(simde_mm256_sub_epi32)
#undef _mm256_sub_epi32
#endif
#ifndef _mm256_sub_epi32
#define _mm256_sub_epi32 simde_mm256_sub_epi32
#endif
#if defined(_mm256_sub_ps) && !defined(simde_mm256_sub_ps)
#undef _mm256_sub_ps
#endif
#ifndef _mm256_sub_ps
#define _mm256_sub_ps simde_mm256_sub_ps
#endif
#if defined(_mm256_subs_epu16) && !defined(simde_mm256_subs_epu16)
#undef _mm256_subs_epu16
#endif
#ifndef _mm256_subs_epu16
#define _mm256_subs_epu16 simde_mm256_subs_epu16
#endif
#if defined(_mm256_testc_si256) && !defined(simde_mm256_testc_si256)
#undef _mm256_testc_si256
#endif
#ifndef _mm256_testc_si256
#define _mm256_testc_si256 simde_mm256_testc_si256
#endif
#if defined(_mm256_testz_si256) && !defined(simde_mm256_testz_si256)
#undef _mm256_testz_si256
#endif
#ifndef _mm256_testz_si256
#define _mm256_testz_si256 simde_mm256_testz_si256
#endif
#if defined(_mm256_unpackhi_epi16) && !defined(simde_mm256_unpackhi_epi16)
#undef _mm256_unpackhi_epi16
#endif
#ifndef _mm256_unpackhi_epi16
#define _mm256_unpackhi_epi16 simde_mm256_unpackhi_epi16
#endif
#if defined(_mm256_unpackhi_epi32) && !defined(simde_mm256_unpackhi_epi32)
#undef _mm256_unpackhi_epi32
#endif
#ifndef _mm256_unpackhi_epi32
#define _mm256_unpackhi_epi32 simde_mm256_unpackhi_epi32
#endif
#if defined(_mm256_unpackhi_epi8) && !defined(simde_mm256_unpackhi_epi8)
#undef _mm256_unpackhi_epi8
#endif
#ifndef _mm256_unpackhi_epi8
#define _mm256_unpackhi_epi8 simde_mm256_unpackhi_epi8
#endif
#if defined(_mm256_unpackhi_ps) && !defined(simde_mm256_unpackhi_ps)
#undef _mm256_unpackhi_ps
#endif
#ifndef _mm256_unpackhi_ps
#define _mm256_unpackhi_ps simde_mm256_unpackhi_ps
#endif
#if defined(_mm256_unpacklo_epi16) && !defined(simde_mm256_unpacklo_epi16)
#undef _mm256_unpacklo_epi16
#endif
#ifndef _mm256_unpacklo_epi16
#define _mm256_unpacklo_epi16 simde_mm256_unpacklo_epi16
#endif
#if defined(_mm256_unpacklo_epi32) && !defined(simde_mm256_unpacklo_epi32)
#undef _mm256_unpacklo_epi32
#endif
#ifndef _mm256_unpacklo_epi32
#define _mm256_unpacklo_epi32 simde_mm256_unpacklo_epi32
#endif
#if defined(_mm256_unpacklo_epi8) && !defined(simde_mm256_unpacklo_epi8)
#undef _mm256_unpacklo_epi8
#endif
#ifndef _mm256_unpacklo_epi8
#define _mm256_unpacklo_epi8 simde_mm256_unpacklo_epi8
#endif
#if defined(_mm256_unpacklo_ps) && !defined(simde_mm256_unpacklo_ps)
#undef _mm256_unpacklo_ps
#endif
#ifndef _mm256_unpacklo_ps
#define _mm256_unpacklo_ps simde_mm256_unpacklo_ps
#endif
#if defined(_mm256_xor_ps) && !defined(simde_mm256_xor_ps)
#undef _mm256_xor_ps
#endif
#ifndef _mm256_xor_ps
#define _mm256_xor_ps simde_mm256_xor_ps
#endif
#if defined(_mm_) && !defined(simde_mm_)
#undef _mm_
#endif
#ifndef _mm_
#define _mm_ simde_mm_
#endif
#if defined(_mm_add_epi16) && !defined(simde_mm_add_epi16)
#undef _mm_add_epi16
#endif
#ifndef _mm_add_epi16
#define _mm_add_epi16 simde_mm_add_epi16
#endif
#if defined(_mm_add_epi32) && !defined(simde_mm_add_epi32)
#undef _mm_add_epi32
#endif
#ifndef _mm_add_epi32
#define _mm_add_epi32 simde_mm_add_epi32
#endif
#if defined(_mm_add_epi8) && !defined(simde_mm_add_epi8)
#undef _mm_add_epi8
#endif
#ifndef _mm_add_epi8
#define _mm_add_epi8 simde_mm_add_epi8
#endif
#if defined(_mm_add_horz) && !defined(simde_mm_add_horz)
#undef _mm_add_horz
#endif
#ifndef _mm_add_horz
#define _mm_add_horz simde_mm_add_horz
#endif
#if defined(_mm_add_horz_ss) && !defined(simde_mm_add_horz_ss)
#undef _mm_add_horz_ss
#endif
#ifndef _mm_add_horz_ss
#define _mm_add_horz_ss simde_mm_add_horz_ss
#endif
#if defined(_mm_add_pi32) && !defined(simde_mm_add_pi32)
#undef _mm_add_pi32
#endif
#ifndef _mm_add_pi32
#define _mm_add_pi32 simde_mm_add_pi32
#endif
#if defined(_mm_add_pi8) && !defined(simde_mm_add_pi8)
#undef _mm_add_pi8
#endif
#ifndef _mm_add_pi8
#define _mm_add_pi8 simde_mm_add_pi8
#endif
#if defined(_mm_add_ps) && !defined(simde_mm_add_ps)
#undef _mm_add_ps
#endif
#ifndef _mm_add_ps
#define _mm_add_ps simde_mm_add_ps
#endif
#if defined(_mm_add_ss) && !defined(simde_mm_add_ss)
#undef _mm_add_ss
#endif
#ifndef _mm_add_ss
#define _mm_add_ss simde_mm_add_ss
#endif
#if defined(_mm_adds_epi16) && !defined(simde_mm_adds_epi16)
#undef _mm_adds_epi16
#endif
#ifndef _mm_adds_epi16
#define _mm_adds_epi16 simde_mm_adds_epi16
#endif
#if defined(_mm_adds_epu16) && !defined(simde_mm_adds_epu16)
#undef _mm_adds_epu16
#endif
#ifndef _mm_adds_epu16
#define _mm_adds_epu16 simde_mm_adds_epu16
#endif
#if defined(_mm_adds_epu8) && !defined(simde_mm_adds_epu8)
#undef _mm_adds_epu8
#endif
#ifndef _mm_adds_epu8
#define _mm_adds_epu8 simde_mm_adds_epu8
#endif
#if defined(_mm_and_ps) && !defined(simde_mm_and_ps)
#undef _mm_and_ps
#endif
#ifndef _mm_and_ps
#define _mm_and_ps simde_mm_and_ps
#endif
#if defined(_mm_and_si128) && !defined(simde_mm_and_si128)
#undef _mm_and_si128
#endif
#ifndef _mm_and_si128
#define _mm_and_si128 simde_mm_and_si128
#endif
#if defined(_mm_and_si64) && !defined(simde_mm_and_si64)
#undef _mm_and_si64
#endif
#ifndef _mm_and_si64
#define _mm_and_si64 simde_mm_and_si64
#endif
#if defined(_mm_andnot_ps) && !defined(simde_mm_andnot_ps)
#undef _mm_andnot_ps
#endif
#ifndef _mm_andnot_ps
#define _mm_andnot_ps simde_mm_andnot_ps
#endif
#if defined(_mm_andnot_si128) && !defined(simde_mm_andnot_si128)
#undef _mm_andnot_si128
#endif
#ifndef _mm_andnot_si128
#define _mm_andnot_si128 simde_mm_andnot_si128
#endif
#if defined(_mm_andnot_si64) && !defined(simde_mm_andnot_si64)
#undef _mm_andnot_si64
#endif
#ifndef _mm_andnot_si64
#define _mm_andnot_si64 simde_mm_andnot_si64
#endif
#if defined(_mm_avg_epu8) && !defined(simde_mm_avg_epu8)
#undef _mm_avg_epu8
#endif
#ifndef _mm_avg_epu8
#define _mm_avg_epu8 simde_mm_avg_epu8
#endif
#if defined(_mm_avg_pu8) && !defined(simde_mm_avg_pu8)
#undef _mm_avg_pu8
#endif
#ifndef _mm_avg_pu8
#define _mm_avg_pu8 simde_mm_avg_pu8
#endif
#if defined(_mm_castps_si128) && !defined(simde_mm_castps_si128)
#undef _mm_castps_si128
#endif
#ifndef _mm_castps_si128
#define _mm_castps_si128 simde_mm_castps_si128
#endif
#if defined(_mm_castsi128_ps) && !defined(simde_mm_castsi128_ps)
#undef _mm_castsi128_ps
#endif
#ifndef _mm_castsi128_ps
#define _mm_castsi128_ps simde_mm_castsi128_ps
#endif
#if defined(_mm_cmp) && !defined(simde_mm_cmp)
#undef _mm_cmp
#endif
#ifndef _mm_cmp
#define _mm_cmp simde_mm_cmp
#endif
#if defined(_mm_cmp_ps) && !defined(simde_mm_cmp_ps)
#undef _mm_cmp_ps
#endif
#ifndef _mm_cmp_ps
#define _mm_cmp_ps simde_mm_cmp_ps
#endif
#if defined(_mm_cmpeq_epi32) && !defined(simde_mm_cmpeq_epi32)
#undef _mm_cmpeq_epi32
#endif
#ifndef _mm_cmpeq_epi32
#define _mm_cmpeq_epi32 simde_mm_cmpeq_epi32
#endif
#if defined(_mm_cmpeq_pi32) && !defined(simde_mm_cmpeq_pi32)
#undef _mm_cmpeq_pi32
#endif
#ifndef _mm_cmpeq_pi32
#define _mm_cmpeq_pi32 simde_mm_cmpeq_pi32
#endif
#if defined(_mm_cmpeq_ps) && !defined(simde_mm_cmpeq_ps)
#undef _mm_cmpeq_ps
#endif
#ifndef _mm_cmpeq_ps
#define _mm_cmpeq_ps simde_mm_cmpeq_ps
#endif
#if defined(_mm_cmpge_ps) && !defined(simde_mm_cmpge_ps)
#undef _mm_cmpge_ps
#endif
#ifndef _mm_cmpge_ps
#define _mm_cmpge_ps simde_mm_cmpge_ps
#endif
#if defined(_mm_cmpgt_epi16) && !defined(simde_mm_cmpgt_epi16)
#undef _mm_cmpgt_epi16
#endif
#ifndef _mm_cmpgt_epi16
#define _mm_cmpgt_epi16 simde_mm_cmpgt_epi16
#endif
#if defined(_mm_cmpgt_pi16) && !defined(simde_mm_cmpgt_pi16)
#undef _mm_cmpgt_pi16
#endif
#ifndef _mm_cmpgt_pi16
#define _mm_cmpgt_pi16 simde_mm_cmpgt_pi16
#endif
#if defined(_mm_cmpgt_ps) && !defined(simde_mm_cmpgt_ps)
#undef _mm_cmpgt_ps
#endif
#ifndef _mm_cmpgt_ps
#define _mm_cmpgt_ps simde_mm_cmpgt_ps
#endif
#if defined(_mm_cmple_ps) && !defined(simde_mm_cmple_ps)
#undef _mm_cmple_ps
#endif
#ifndef _mm_cmple_ps
#define _mm_cmple_ps simde_mm_cmple_ps
#endif
#if defined(_mm_cmplt_epi16) && !defined(simde_mm_cmplt_epi16)
#undef _mm_cmplt_epi16
#endif
#ifndef _mm_cmplt_epi16
#define _mm_cmplt_epi16 simde_mm_cmplt_epi16
#endif
#if defined(_mm_cmplt_epi32) && !defined(simde_mm_cmplt_epi32)
#undef _mm_cmplt_epi32
#endif
#ifndef _mm_cmplt_epi32
#define _mm_cmplt_epi32 simde_mm_cmplt_epi32
#endif
#if defined(_mm_cmplt_ps) && !defined(simde_mm_cmplt_ps)
#undef _mm_cmplt_ps
#endif
#ifndef _mm_cmplt_ps
#define _mm_cmplt_ps simde_mm_cmplt_ps
#endif
#if defined(_mm_cmpxx_ps) && !defined(simde_mm_cmpxx_ps)
#undef _mm_cmpxx_ps
#endif
#ifndef _mm_cmpxx_ps
#define _mm_cmpxx_ps simde_mm_cmpxx_ps
#endif
#if defined(_mm_cvt_ps2pi) && !defined(simde_mm_cvt_ps2pi)
#undef _mm_cvt_ps2pi
#endif
#ifndef _mm_cvt_ps2pi
#define _mm_cvt_ps2pi simde_mm_cvt_ps2pi
#endif
#if defined(_mm_cvtepi16_epi32) && !defined(simde_mm_cvtepi16_epi32)
#undef _mm_cvtepi16_epi32
#endif
#ifndef _mm_cvtepi16_epi32
#define _mm_cvtepi16_epi32 simde_mm_cvtepi16_epi32
#endif
#if defined(_mm_cvtepi32_ps) && !defined(simde_mm_cvtepi32_ps)
#undef _mm_cvtepi32_ps
#endif
#ifndef _mm_cvtepi32_ps
#define _mm_cvtepi32_ps simde_mm_cvtepi32_ps
#endif
#if defined(_mm_cvtepu8_epi16) && !defined(simde_mm_cvtepu8_epi16)
#undef _mm_cvtepu8_epi16
#endif
#ifndef _mm_cvtepu8_epi16
#define _mm_cvtepu8_epi16 simde_mm_cvtepu8_epi16
#endif
#if defined(_mm_cvtm64_si64) && !defined(simde_mm_cvtm64_si64)
#undef _mm_cvtm64_si64
#endif
#ifndef _mm_cvtm64_si64
#define _mm_cvtm64_si64 simde_mm_cvtm64_si64
#endif
#if defined(_mm_cvtpi32_ps) && !defined(simde_mm_cvtpi32_ps)
#undef _mm_cvtpi32_ps
#endif
#ifndef _mm_cvtpi32_ps
#define _mm_cvtpi32_ps simde_mm_cvtpi32_ps
#endif
#if defined(_mm_cvtpi32x2_ps) && !defined(simde_mm_cvtpi32x2_ps)
#undef _mm_cvtpi32x2_ps
#endif
#ifndef _mm_cvtpi32x2_ps
#define _mm_cvtpi32x2_ps simde_mm_cvtpi32x2_ps
#endif
#if defined(_mm_cvtps_epi32) && !defined(simde_mm_cvtps_epi32)
#undef _mm_cvtps_epi32
#endif
#ifndef _mm_cvtps_epi32
#define _mm_cvtps_epi32 simde_mm_cvtps_epi32
#endif
#if defined(_mm_cvtps_pi16) && !defined(simde_mm_cvtps_pi16)
#undef _mm_cvtps_pi16
#endif
#ifndef _mm_cvtps_pi16
#define _mm_cvtps_pi16 simde_mm_cvtps_pi16
#endif
#if defined(_mm_cvtsi128_si32) && !defined(simde_mm_cvtsi128_si32)
#undef _mm_cvtsi128_si32
#endif
#ifndef _mm_cvtsi128_si32
#define _mm_cvtsi128_si32 simde_mm_cvtsi128_si32
#endif
#if defined(_mm_cvtsi32_si128) && !defined(simde_mm_cvtsi32_si128)
#undef _mm_cvtsi32_si128
#endif
#ifndef _mm_cvtsi32_si128
#define _mm_cvtsi32_si128 simde_mm_cvtsi32_si128
#endif
#if defined(_mm_cvtsi32_si64) && !defined(simde_mm_cvtsi32_si64)
#undef _mm_cvtsi32_si64
#endif
#ifndef _mm_cvtsi32_si64
#define _mm_cvtsi32_si64 simde_mm_cvtsi32_si64
#endif
#if defined(_mm_cvtsi32_ss) && !defined(simde_mm_cvtsi32_ss)
#undef _mm_cvtsi32_ss
#endif
#ifndef _mm_cvtsi32_ss
#define _mm_cvtsi32_ss simde_mm_cvtsi32_ss
#endif
#if defined(_mm_cvtsi64_m64) && !defined(simde_mm_cvtsi64_m64)
#undef _mm_cvtsi64_m64
#endif
#ifndef _mm_cvtsi64_m64
#define _mm_cvtsi64_m64 simde_mm_cvtsi64_m64
#endif
#if defined(_mm_cvtsi64_si32) && !defined(simde_mm_cvtsi64_si32)
#undef _mm_cvtsi64_si32
#endif
#ifndef _mm_cvtsi64_si32
#define _mm_cvtsi64_si32 simde_mm_cvtsi64_si32
#endif
#if defined(_mm_cvtss_f32) && !defined(simde_mm_cvtss_f32)
#undef _mm_cvtss_f32
#endif
#ifndef _mm_cvtss_f32
#define _mm_cvtss_f32 simde_mm_cvtss_f32
#endif
#if defined(_mm_cvtt_ps2pi) && !defined(simde_mm_cvtt_ps2pi)
#undef _mm_cvtt_ps2pi
#endif
#ifndef _mm_cvtt_ps2pi
#define _mm_cvtt_ps2pi simde_mm_cvtt_ps2pi
#endif
#if defined(_mm_cvttps_epi32) && !defined(simde_mm_cvttps_epi32)
#undef _mm_cvttps_epi32
#endif
#ifndef _mm_cvttps_epi32
#define _mm_cvttps_epi32 simde_mm_cvttps_epi32
#endif
#if defined(_mm_cvttps_pi32) && !defined(simde_mm_cvttps_pi32)
#undef _mm_cvttps_pi32
#endif
#ifndef _mm_cvttps_pi32
#define _mm_cvttps_pi32 simde_mm_cvttps_pi32
#endif
#if defined(_mm_div_ps) && !defined(simde_mm_div_ps)
#undef _mm_div_ps
#endif
#ifndef _mm_div_ps
#define _mm_div_ps simde_mm_div_ps
#endif
#if defined(_mm_empty) && !defined(simde_mm_empty)
#undef _mm_empty
#endif
#ifndef _mm_empty
#define _mm_empty simde_mm_empty
#endif
#if defined(_mm_extract_epi16) && !defined(simde_mm_extract_epi16)
#undef _mm_extract_epi16
#endif
#ifndef _mm_extract_epi16
#define _mm_extract_epi16 simde_mm_extract_epi16
#endif
#if defined(_mm_free) && !defined(simde_mm_free)
#undef _mm_free
#endif
#ifndef _mm_free
#define _mm_free simde_mm_free
#endif
#if defined(_mm_hadd_epi16) && !defined(simde_mm_hadd_epi16)
#undef _mm_hadd_epi16
#endif
#ifndef _mm_hadd_epi16
#define _mm_hadd_epi16 simde_mm_hadd_epi16
#endif
#if defined(_mm_hadd_ps) && !defined(simde_mm_hadd_ps)
#undef _mm_hadd_ps
#endif
#ifndef _mm_hadd_ps
#define _mm_hadd_ps simde_mm_hadd_ps
#endif
#if defined(_mm_load1_ps) && !defined(simde_mm_load1_ps)
#undef _mm_load1_ps
#endif
#ifndef _mm_load1_ps
#define _mm_load1_ps simde_mm_load1_ps
#endif
#if defined(_mm_load_ps) && !defined(simde_mm_load_ps)
#undef _mm_load_ps
#endif
#ifndef _mm_load_ps
#define _mm_load_ps simde_mm_load_ps
#endif
#if defined(_mm_load_si128) && !defined(simde_mm_load_si128)
#undef _mm_load_si128
#endif
#ifndef _mm_load_si128
#define _mm_load_si128 simde_mm_load_si128
#endif
#if defined(_mm_load_ss) && !defined(simde_mm_load_ss)
#undef _mm_load_ss
#endif
#ifndef _mm_load_ss
#define _mm_load_ss simde_mm_load_ss
#endif
#if defined(_mm_loadh_pi) && !defined(simde_mm_loadh_pi)
#undef _mm_loadh_pi
#endif
#ifndef _mm_loadh_pi
#define _mm_loadh_pi simde_mm_loadh_pi
#endif
#if defined(_mm_loadl_epi64) && !defined(simde_mm_loadl_epi64)
#undef _mm_loadl_epi64
#endif
#ifndef _mm_loadl_epi64
#define _mm_loadl_epi64 simde_mm_loadl_epi64
#endif
#if defined(_mm_loadl_pi) && !defined(simde_mm_loadl_pi)
#undef _mm_loadl_pi
#endif
#ifndef _mm_loadl_pi
#define _mm_loadl_pi simde_mm_loadl_pi
#endif
#if defined(_mm_loadu_ps) && !defined(simde_mm_loadu_ps)
#undef _mm_loadu_ps
#endif
#ifndef _mm_loadu_ps
#define _mm_loadu_ps simde_mm_loadu_ps
#endif
#if defined(_mm_loadu_si128) && !defined(simde_mm_loadu_si128)
#undef _mm_loadu_si128
#endif
#ifndef _mm_loadu_si128
#define _mm_loadu_si128 simde_mm_loadu_si128
#endif
#if defined(_mm_malloc) && !defined(simde_mm_malloc)
#undef _mm_malloc
#endif
#ifndef _mm_malloc
#define _mm_malloc simde_mm_malloc
#endif
#if defined(_mm_maskmove_si64) && !defined(simde_mm_maskmove_si64)
#undef _mm_maskmove_si64
#endif
#ifndef _mm_maskmove_si64
#define _mm_maskmove_si64 simde_mm_maskmove_si64
#endif
#if defined(_mm_maskmoveu_si128) && !defined(simde_mm_maskmoveu_si128)
#undef _mm_maskmoveu_si128
#endif
#ifndef _mm_maskmoveu_si128
#define _mm_maskmoveu_si128 simde_mm_maskmoveu_si128
#endif
#if defined(_mm_max_epu8) && !defined(simde_mm_max_epu8)
#undef _mm_max_epu8
#endif
#ifndef _mm_max_epu8
#define _mm_max_epu8 simde_mm_max_epu8
#endif
#if defined(_mm_max_horz) && !defined(simde_mm_max_horz)
#undef _mm_max_horz
#endif
#ifndef _mm_max_horz
#define _mm_max_horz simde_mm_max_horz
#endif
#if defined(_mm_max_ps) && !defined(simde_mm_max_ps)
#undef _mm_max_ps
#endif
#ifndef _mm_max_ps
#define _mm_max_ps simde_mm_max_ps
#endif
#if defined(_mm_max_pu8) && !defined(simde_mm_max_pu8)
#undef _mm_max_pu8
#endif
#ifndef _mm_max_pu8
#define _mm_max_pu8 simde_mm_max_pu8
#endif
#if defined(_mm_max_ss) && !defined(simde_mm_max_ss)
#undef _mm_max_ss
#endif
#ifndef _mm_max_ss
#define _mm_max_ss simde_mm_max_ss
#endif
#if defined(_mm_min_epu8) && !defined(simde_mm_min_epu8)
#undef _mm_min_epu8
#endif
#ifndef _mm_min_epu8
#define _mm_min_epu8 simde_mm_min_epu8
#endif
#if defined(_mm_min_horz) && !defined(simde_mm_min_horz)
#undef _mm_min_horz
#endif
#ifndef _mm_min_horz
#define _mm_min_horz simde_mm_min_horz
#endif
#if defined(_mm_min_ps) && !defined(simde_mm_min_ps)
#undef _mm_min_ps
#endif
#ifndef _mm_min_ps
#define _mm_min_ps simde_mm_min_ps
#endif
#if defined(_mm_min_pu8) && !defined(simde_mm_min_pu8)
#undef _mm_min_pu8
#endif
#ifndef _mm_min_pu8
#define _mm_min_pu8 simde_mm_min_pu8
#endif
#if defined(_mm_min_ss) && !defined(simde_mm_min_ss)
#undef _mm_min_ss
#endif
#ifndef _mm_min_ss
#define _mm_min_ss simde_mm_min_ss
#endif
#if defined(_mm_movehl_ps) && !defined(simde_mm_movehl_ps)
#undef _mm_movehl_ps
#endif
#ifndef _mm_movehl_ps
#define _mm_movehl_ps simde_mm_movehl_ps
#endif
#if defined(_mm_movelh_ps) && !defined(simde_mm_movelh_ps)
#undef _mm_movelh_ps
#endif
#ifndef _mm_movelh_ps
#define _mm_movelh_ps simde_mm_movelh_ps
#endif
#if defined(_mm_movemask_epi8) && !defined(simde_mm_movemask_epi8)
#undef _mm_movemask_epi8
#endif
#ifndef _mm_movemask_epi8
#define _mm_movemask_epi8 simde_mm_movemask_epi8
#endif
#if defined(_mm_movepi64_pi64) && !defined(simde_mm_movepi64_pi64)
#undef _mm_movepi64_pi64
#endif
#ifndef _mm_movepi64_pi64
#define _mm_movepi64_pi64 simde_mm_movepi64_pi64
#endif
#if defined(_mm_mul_epu32) && !defined(simde_mm_mul_epu32)
#undef _mm_mul_epu32
#endif
#ifndef _mm_mul_epu32
#define _mm_mul_epu32 simde_mm_mul_epu32
#endif
#if defined(_mm_mul_ps) && !defined(simde_mm_mul_ps)
#undef _mm_mul_ps
#endif
#ifndef _mm_mul_ps
#define _mm_mul_ps simde_mm_mul_ps
#endif
#if defined(_mm_mul_ss) && !defined(simde_mm_mul_ss)
#undef _mm_mul_ss
#endif
#ifndef _mm_mul_ss
#define _mm_mul_ss simde_mm_mul_ss
#endif
#if defined(_mm_mulhi_epi16) && !defined(simde_mm_mulhi_epi16)
#undef _mm_mulhi_epi16
#endif
#ifndef _mm_mulhi_epi16
#define _mm_mulhi_epi16 simde_mm_mulhi_epi16
#endif
#if defined(_mm_mulhi_epu16) && !defined(simde_mm_mulhi_epu16)
#undef _mm_mulhi_epu16
#endif
#ifndef _mm_mulhi_epu16
#define _mm_mulhi_epu16 simde_mm_mulhi_epu16
#endif
#if defined(_mm_mullo_epi16) && !defined(simde_mm_mullo_epi16)
#undef _mm_mullo_epi16
#endif
#ifndef _mm_mullo_epi16
#define _mm_mullo_epi16 simde_mm_mullo_epi16
#endif
#if defined(_mm_mullo_epi32) && !defined(simde_mm_mullo_epi32)
#undef _mm_mullo_epi32
#endif
#ifndef _mm_mullo_epi32
#define _mm_mullo_epi32 simde_mm_mullo_epi32
#endif
#if defined(_mm_or_ps) && !defined(simde_mm_or_ps)
#undef _mm_or_ps
#endif
#ifndef _mm_or_ps
#define _mm_or_ps simde_mm_or_ps
#endif
#if defined(_mm_or_si128) && !defined(simde_mm_or_si128)
#undef _mm_or_si128
#endif
#ifndef _mm_or_si128
#define _mm_or_si128 simde_mm_or_si128
#endif
#if defined(_mm_or_si64) && !defined(simde_mm_or_si64)
#undef _mm_or_si64
#endif
#ifndef _mm_or_si64
#define _mm_or_si64 simde_mm_or_si64
#endif
#if defined(_mm_packs_epi32) && !defined(simde_mm_packs_epi32)
#undef _mm_packs_epi32
#endif
#ifndef _mm_packs_epi32
#define _mm_packs_epi32 simde_mm_packs_epi32
#endif
#if defined(_mm_packus_epi16) && !defined(simde_mm_packus_epi16)
#undef _mm_packus_epi16
#endif
#ifndef _mm_packus_epi16
#define _mm_packus_epi16 simde_mm_packus_epi16
#endif
#if defined(_mm_rcp_ps) && !defined(simde_mm_rcp_ps)
#undef _mm_rcp_ps
#endif
#ifndef _mm_rcp_ps
#define _mm_rcp_ps simde_mm_rcp_ps
#endif
#if defined(_mm_rcp_ss) && !defined(simde_mm_rcp_ss)
#undef _mm_rcp_ss
#endif
#ifndef _mm_rcp_ss
#define _mm_rcp_ss simde_mm_rcp_ss
#endif
#if defined(_mm_set1_epi16) && !defined(simde_mm_set1_epi16)
#undef _mm_set1_epi16
#endif
#ifndef _mm_set1_epi16
#define _mm_set1_epi16 simde_mm_set1_epi16
#endif
#if defined(_mm_set1_epi32) && !defined(simde_mm_set1_epi32)
#undef _mm_set1_epi32
#endif
#ifndef _mm_set1_epi32
#define _mm_set1_epi32 simde_mm_set1_epi32
#endif
#if defined(_mm_set1_pi32) && !defined(simde_mm_set1_pi32)
#undef _mm_set1_pi32
#endif
#ifndef _mm_set1_pi32
#define _mm_set1_pi32 simde_mm_set1_pi32
#endif
#if defined(_mm_set1_ps) && !defined(simde_mm_set1_ps)
#undef _mm_set1_ps
#endif
#ifndef _mm_set1_ps
#define _mm_set1_ps simde_mm_set1_ps
#endif
#if defined(_mm_set_epi32) && !defined(simde_mm_set_epi32)
#undef _mm_set_epi32
#endif
#ifndef _mm_set_epi32
#define _mm_set_epi32 simde_mm_set_epi32
#endif
#if defined(_mm_set_ps) && !defined(simde_mm_set_ps)
#undef _mm_set_ps
#endif
#ifndef _mm_set_ps
#define _mm_set_ps simde_mm_set_ps
#endif
#if defined(_mm_set_ss) && !defined(simde_mm_set_ss)
#undef _mm_set_ss
#endif
#ifndef _mm_set_ss
#define _mm_set_ss simde_mm_set_ss
#endif
#if defined(_mm_setr_epi8) && !defined(simde_mm_setr_epi8)
#undef _mm_setr_epi8
#endif
#ifndef _mm_setr_epi8
#define _mm_setr_epi8 simde_mm_setr_epi8
#endif
#if defined(_mm_setzero_ps) && !defined(simde_mm_setzero_ps)
#undef _mm_setzero_ps
#endif
#ifndef _mm_setzero_ps
#define _mm_setzero_ps simde_mm_setzero_ps
#endif
#if defined(_mm_setzero_si128) && !defined(simde_mm_setzero_si128)
#undef _mm_setzero_si128
#endif
#ifndef _mm_setzero_si128
#define _mm_setzero_si128 simde_mm_setzero_si128
#endif
#if defined(_mm_setzero_si64) && !defined(simde_mm_setzero_si64)
#undef _mm_setzero_si64
#endif
#ifndef _mm_setzero_si64
#define _mm_setzero_si64 simde_mm_setzero_si64
#endif
#if defined(_mm_shuffle_epi32) && !defined(simde_mm_shuffle_epi32)
#undef _mm_shuffle_epi32
#endif
#ifndef _mm_shuffle_epi32
#define _mm_shuffle_epi32 simde_mm_shuffle_epi32
#endif
#if defined(_mm_shuffle_epi8) && !defined(simde_mm_shuffle_epi8)
#undef _mm_shuffle_epi8
#endif
#ifndef _mm_shuffle_epi8
#define _mm_shuffle_epi8 simde_mm_shuffle_epi8
#endif
#if defined(_mm_shuffle_ps) && !defined(simde_mm_shuffle_ps)
#undef _mm_shuffle_ps
#endif
#ifndef _mm_shuffle_ps
#define _mm_shuffle_ps simde_mm_shuffle_ps
#endif
#if defined(_mm_shufflehi_epi16) && !defined(simde_mm_shufflehi_epi16)
#undef _mm_shufflehi_epi16
#endif
#ifndef _mm_shufflehi_epi16
#define _mm_shufflehi_epi16 simde_mm_shufflehi_epi16
#endif
#if defined(_mm_shufflelo_epi16) && !defined(simde_mm_shufflelo_epi16)
#undef _mm_shufflelo_epi16
#endif
#ifndef _mm_shufflelo_epi16
#define _mm_shufflelo_epi16 simde_mm_shufflelo_epi16
#endif
#if defined(_mm_slli_epi16) && !defined(simde_mm_slli_epi16)
#undef _mm_slli_epi16
#endif
#ifndef _mm_slli_epi16
#define _mm_slli_epi16 simde_mm_slli_epi16
#endif
#if defined(_mm_slli_epi32) && !defined(simde_mm_slli_epi32)
#undef _mm_slli_epi32
#endif
#ifndef _mm_slli_epi32
#define _mm_slli_epi32 simde_mm_slli_epi32
#endif
#if defined(_mm_slli_epi64) && !defined(simde_mm_slli_epi64)
#undef _mm_slli_epi64
#endif
#ifndef _mm_slli_epi64
#define _mm_slli_epi64 simde_mm_slli_epi64
#endif
#if defined(_mm_slli_pi32) && !defined(simde_mm_slli_pi32)
#undef _mm_slli_pi32
#endif
#ifndef _mm_slli_pi32
#define _mm_slli_pi32 simde_mm_slli_pi32
#endif
#if defined(_mm_slli_pi64) && !defined(simde_mm_slli_pi64)
#undef _mm_slli_pi64
#endif
#ifndef _mm_slli_pi64
#define _mm_slli_pi64 simde_mm_slli_pi64
#endif
#if defined(_mm_slli_si128) && !defined(simde_mm_bslli_si128)
#undef _mm_slli_si128
#endif
#ifndef _mm_slli_si128
#define _mm_slli_si128 simde_mm_bslli_si128
#endif
#if defined(_mm_slli_si64) && !defined(simde_mm_slli_si64)
#undef _mm_slli_si64
#endif
#ifndef _mm_slli_si64
#define _mm_slli_si64 simde_mm_slli_si64
#endif
#if defined(_mm_sqrt_ps) && !defined(simde_mm_sqrt_ps)
#undef _mm_sqrt_ps
#endif
#ifndef _mm_sqrt_ps
#define _mm_sqrt_ps simde_mm_sqrt_ps
#endif
#if defined(_mm_srai_epi16) && !defined(simde_mm_srai_epi16)
#undef _mm_srai_epi16
#endif
#ifndef _mm_srai_epi16
#define _mm_srai_epi16 simde_mm_srai_epi16
#endif
#if defined(_mm_srai_epi32) && !defined(simde_mm_srai_epi32)
#undef _mm_srai_epi32
#endif
#ifndef _mm_srai_epi32
#define _mm_srai_epi32 simde_mm_srai_epi32
#endif
#if defined(_mm_srai_pi32) && !defined(simde_mm_srai_pi32)
#undef _mm_srai_pi32
#endif
#ifndef _mm_srai_pi32
#define _mm_srai_pi32 simde_mm_srai_pi32
#endif
#if defined(_mm_srli_epi16) && !defined(simde_mm_srli_epi16)
#undef _mm_srli_epi16
#endif
#ifndef _mm_srli_epi16
#define _mm_srli_epi16 simde_mm_srli_epi16
#endif
#if defined(_mm_srli_epi32) && !defined(simde_mm_srli_epi32)
#undef _mm_srli_epi32
#endif
#ifndef _mm_srli_epi32
#define _mm_srli_epi32 simde_mm_srli_epi32
#endif
#if defined(_mm_srli_epi64) && !defined(simde_mm_srli_epi64)
#undef _mm_srli_epi64
#endif
#ifndef _mm_srli_epi64
#define _mm_srli_epi64 simde_mm_srli_epi64
#endif
#if defined(_mm_srli_pi32) && !defined(simde_mm_srli_pi32)
#undef _mm_srli_pi32
#endif
#ifndef _mm_srli_pi32
#define _mm_srli_pi32 simde_mm_srli_pi32
#endif
#if defined(_mm_srli_pi64) && !defined(simde_mm_srli_pi64)
#undef _mm_srli_pi64
#endif
#ifndef _mm_srli_pi64
#define _mm_srli_pi64 simde_mm_srli_pi64
#endif
#if defined(_mm_srli_si128) && !defined(simde_mm_bsrli_si128)
#undef _mm_srli_si128
#endif
#ifndef _mm_srli_si128
#define _mm_srli_si128 simde_mm_bsrli_si128
#endif
#if defined(_mm_srli_si64) && !defined(simde_mm_srli_si64)
#undef _mm_srli_si64
#endif
#ifndef _mm_srli_si64
#define _mm_srli_si64 simde_mm_srli_si64
#endif
#if defined(_mm_store_ps) && !defined(simde_mm_store_ps)
#undef _mm_store_ps
#endif
#ifndef _mm_store_ps
#define _mm_store_ps simde_mm_store_ps
#endif
#if defined(_mm_store_si128) && !defined(simde_mm_store_si128)
#undef _mm_store_si128
#endif
#ifndef _mm_store_si128
#define _mm_store_si128 simde_mm_store_si128
#endif
#if defined(_mm_store_ss) && !defined(simde_mm_store_ss)
#undef _mm_store_ss
#endif
#ifndef _mm_store_ss
#define _mm_store_ss simde_mm_store_ss
#endif
#if defined(_mm_storeh_pi) && !defined(simde_mm_storeh_pi)
#undef _mm_storeh_pi
#endif
#ifndef _mm_storeh_pi
#define _mm_storeh_pi simde_mm_storeh_pi
#endif
#if defined(_mm_storel_epi64) && !defined(simde_mm_storel_epi64)
#undef _mm_storel_epi64
#endif
#ifndef _mm_storel_epi64
#define _mm_storel_epi64 simde_mm_storel_epi64
#endif
#if defined(_mm_storel_pi) && !defined(simde_mm_storel_pi)
#undef _mm_storel_pi
#endif
#ifndef _mm_storel_pi
#define _mm_storel_pi simde_mm_storel_pi
#endif
#if defined(_mm_storeu_ps) && !defined(simde_mm_storeu_ps)
#undef _mm_storeu_ps
#endif
#ifndef _mm_storeu_ps
#define _mm_storeu_ps simde_mm_storeu_ps
#endif
#if defined(_mm_storeu_si128) && !defined(simde_mm_storeu_si128)
#undef _mm_storeu_si128
#endif
#ifndef _mm_storeu_si128
#define _mm_storeu_si128 simde_mm_storeu_si128
#endif
#if defined(_mm_stream_si128) && !defined(simde_mm_stream_si128)
#undef _mm_stream_si128
#endif
#ifndef _mm_stream_si128
#define _mm_stream_si128 simde_mm_stream_si128
#endif
#if defined(_mm_sub_epi16) && !defined(simde_mm_sub_epi16)
#undef _mm_sub_epi16
#endif
#ifndef _mm_sub_epi16
#define _mm_sub_epi16 simde_mm_sub_epi16
#endif
#if defined(_mm_sub_epi32) && !defined(simde_mm_sub_epi32)
#undef _mm_sub_epi32
#endif
#ifndef _mm_sub_epi32
#define _mm_sub_epi32 simde_mm_sub_epi32
#endif
#if defined(_mm_sub_epi8) && !defined(simde_mm_sub_epi8)
#undef _mm_sub_epi8
#endif
#ifndef _mm_sub_epi8
#define _mm_sub_epi8 simde_mm_sub_epi8
#endif
#if defined(_mm_sub_pi32) && !defined(simde_mm_sub_pi32)
#undef _mm_sub_pi32
#endif
#ifndef _mm_sub_pi32
#define _mm_sub_pi32 simde_mm_sub_pi32
#endif
#if defined(_mm_sub_pi8) && !defined(simde_mm_sub_pi8)
#undef _mm_sub_pi8
#endif
#ifndef _mm_sub_pi8
#define _mm_sub_pi8 simde_mm_sub_pi8
#endif
#if defined(_mm_sub_ps) && !defined(simde_mm_sub_ps)
#undef _mm_sub_ps
#endif
#ifndef _mm_sub_ps
#define _mm_sub_ps simde_mm_sub_ps
#endif
#if defined(_mm_sub_ss) && !defined(simde_mm_sub_ss)
#undef _mm_sub_ss
#endif
#ifndef _mm_sub_ss
#define _mm_sub_ss simde_mm_sub_ss
#endif
#if defined(_mm_subs_epu16) && !defined(simde_mm_subs_epu16)
#undef _mm_subs_epu16
#endif
#ifndef _mm_subs_epu16
#define _mm_subs_epu16 simde_mm_subs_epu16
#endif
#if defined(_mm_subs_epu8) && !defined(simde_mm_subs_epu8)
#undef _mm_subs_epu8
#endif
#ifndef _mm_subs_epu8
#define _mm_subs_epu8 simde_mm_subs_epu8
#endif
#if defined(_mm_testc_si128) && !defined(simde_mm_testc_si128)
#undef _mm_testc_si128
#endif
#ifndef _mm_testc_si128
#define _mm_testc_si128 simde_mm_testc_si128
#endif
#if defined(_mm_testz_si128) && !defined(simde_mm_testz_si128)
#undef _mm_testz_si128
#endif
#ifndef _mm_testz_si128
#define _mm_testz_si128 simde_mm_testz_si128
#endif
#if defined(_mm_union) && !defined(simde_mm_union)
#undef _mm_union
#endif
#ifndef _mm_union
#define _mm_union simde_mm_union
#endif
#if defined(_mm_unpackhi_epi16) && !defined(simde_mm_unpackhi_epi16)
#undef _mm_unpackhi_epi16
#endif
#ifndef _mm_unpackhi_epi16
#define _mm_unpackhi_epi16 simde_mm_unpackhi_epi16
#endif
#if defined(_mm_unpackhi_epi32) && !defined(simde_mm_unpackhi_epi32)
#undef _mm_unpackhi_epi32
#endif
#ifndef _mm_unpackhi_epi32
#define _mm_unpackhi_epi32 simde_mm_unpackhi_epi32
#endif
#if defined(_mm_unpackhi_epi8) && !defined(simde_mm_unpackhi_epi8)
#undef _mm_unpackhi_epi8
#endif
#ifndef _mm_unpackhi_epi8
#define _mm_unpackhi_epi8 simde_mm_unpackhi_epi8
#endif
#if defined(_mm_unpackhi_pi16) && !defined(simde_mm_unpackhi_pi16)
#undef _mm_unpackhi_pi16
#endif
#ifndef _mm_unpackhi_pi16
#define _mm_unpackhi_pi16 simde_mm_unpackhi_pi16
#endif
#if defined(_mm_unpackhi_ps) && !defined(simde_mm_unpackhi_ps)
#undef _mm_unpackhi_ps
#endif
#ifndef _mm_unpackhi_ps
#define _mm_unpackhi_ps simde_mm_unpackhi_ps
#endif
#if defined(_mm_unpacklo_epi16) && !defined(simde_mm_unpacklo_epi16)
#undef _mm_unpacklo_epi16
#endif
#ifndef _mm_unpacklo_epi16
#define _mm_unpacklo_epi16 simde_mm_unpacklo_epi16
#endif
#if defined(_mm_unpacklo_epi32) && !defined(simde_mm_unpacklo_epi32)
#undef _mm_unpacklo_epi32
#endif
#ifndef _mm_unpacklo_epi32
#define _mm_unpacklo_epi32 simde_mm_unpacklo_epi32
#endif
#if defined(_mm_unpacklo_epi64) && !defined(simde_mm_unpacklo_epi64)
#undef _mm_unpacklo_epi64
#endif
#ifndef _mm_unpacklo_epi64
#define _mm_unpacklo_epi64 simde_mm_unpacklo_epi64
#endif
#if defined(_mm_unpacklo_epi8) && !defined(simde_mm_unpacklo_epi8)
#undef _mm_unpacklo_epi8
#endif
#ifndef _mm_unpacklo_epi8
#define _mm_unpacklo_epi8 simde_mm_unpacklo_epi8
#endif
#if defined(_mm_unpacklo_pi16) && !defined(simde_mm_unpacklo_pi16)
#undef _mm_unpacklo_pi16
#endif
#ifndef _mm_unpacklo_pi16
#define _mm_unpacklo_pi16 simde_mm_unpacklo_pi16
#endif
#if defined(_mm_unpacklo_ps) && !defined(simde_mm_unpacklo_ps)
#undef _mm_unpacklo_ps
#endif
#ifndef _mm_unpacklo_ps
#define _mm_unpacklo_ps simde_mm_unpacklo_ps
#endif
#if defined(_mm_untnorm_ps) && !defined(simde_mm_untnorm_ps)
#undef _mm_untnorm_ps
#endif
#ifndef _mm_untnorm_ps
#define _mm_untnorm_ps simde_mm_untnorm_ps
#endif
#if defined(_mm_xor_ps) && !defined(simde_mm_xor_ps)
#undef _mm_xor_ps
#endif
#ifndef _mm_xor_ps
#define _mm_xor_ps simde_mm_xor_ps
#endif
#if defined(_mm_xor_si128) && !defined(simde_mm_xor_si128)
#undef _mm_xor_si128
#endif
#ifndef _mm_xor_si128
#define _mm_xor_si128 simde_mm_xor_si128
#endif

#endif
