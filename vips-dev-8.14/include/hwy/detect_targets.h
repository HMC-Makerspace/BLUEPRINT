// Copyright 2021 Google LLC
// SPDX-License-Identifier: Apache-2.0
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef HIGHWAY_HWY_DETECT_TARGETS_H_
#define HIGHWAY_HWY_DETECT_TARGETS_H_

// Defines targets and chooses which to enable.

#include "hwy/detect_compiler_arch.h"

//------------------------------------------------------------------------------
// Optional configuration

// See g3doc/quick_reference.md for documentation of these macros.

// Uncomment to override the default baseline determined from predefined macros:
// #define HWY_BASELINE_TARGETS (HWY_SSE4 | HWY_SCALAR)

// Uncomment to override the default blocklist:
// #define HWY_BROKEN_TARGETS HWY_AVX3

// Uncomment to definitely avoid generating those target(s):
// #define HWY_DISABLED_TARGETS HWY_SSE4

// Uncomment to avoid emitting BMI/BMI2/FMA instructions (allows generating
// AVX2 target for VMs which support AVX2 but not the other instruction sets)
// #define HWY_DISABLE_BMI2_FMA

// Uncomment to enable SSSE3/SSE4 on MSVC even if AVX is not enabled
// #define HWY_WANT_SSSE3
// #define HWY_WANT_SSE4

//------------------------------------------------------------------------------
// Targets

// Unique bit value for each target. A lower value is "better" (e.g. more lanes)
// than a higher value within the same group/platform - see HWY_STATIC_TARGET.
//
// All values are unconditionally defined so we can test HWY_TARGETS without
// first checking the HWY_ARCH_*.
//
// The C99 preprocessor evaluates #if expressions using intmax_t types. This
// holds at least 64 bits in practice (verified 2022-07-18 via Godbolt on
// 32-bit clang/GCC/MSVC compilers for x86/Arm7/AArch32/RISC-V/WASM). We now
// avoid overflow when computing HWY_TARGETS (subtracting one instead of
// left-shifting 2^62), but still do not use bit 63 because it is the sign bit.

// --------------------------- x86: 15 targets (+ one fallback)
// Bits 0..5 reserved (6 targets)
// Currently HWY_AVX3_DL plus a special case for CompressStore (10x as fast).
// We may later also use VPCONFLICT.
#define HWY_AVX3_ZEN4 (1LL << 6)
// Currently satisfiable by Ice Lake (VNNI, VPCLMULQDQ, VPOPCNTDQ, VBMI, VBMI2,
// VAES, BITALG). Later to be added: BF16 (Cooper Lake). VP2INTERSECT is only in
// Tiger Lake? We do not yet have uses for GFNI.
#define HWY_AVX3_DL (1LL << 7)  // see HWY_WANT_AVX3_DL below
#define HWY_AVX3 (1LL << 8)
#define HWY_AVX2 (1LL << 9)
// Bit 10: reserved for AVX
#define HWY_SSE4 (1LL << 11)
#define HWY_SSSE3 (1LL << 12)
// Bit 13: reserved for SSE3
#define HWY_SSE2 (1LL << 14)
// The highest bit in the HWY_TARGETS mask that a x86 target can have. Used for
// dynamic dispatch. All x86 target bits must be lower or equal to
// (1 << HWY_HIGHEST_TARGET_BIT_X86) and they can only use
// HWY_MAX_DYNAMIC_TARGETS in total.
#define HWY_HIGHEST_TARGET_BIT_X86 14

// --------------------------- Arm: 15 targets (+ one fallback)
// Bits 15..23 reserved (9 targets)
#define HWY_SVE2_128 (1LL << 24)  // specialized target (e.g. Arm N2)
#define HWY_SVE_256 (1LL << 25)   // specialized target (e.g. Arm V1)
#define HWY_SVE2 (1LL << 26)
#define HWY_SVE (1LL << 27)
#define HWY_NEON (1LL << 28)  // Implies support for AES
#define HWY_NEON_WITHOUT_AES (1LL << 29)
#define HWY_HIGHEST_TARGET_BIT_ARM 29

// --------------------------- RISC-V: 9 targets (+ one fallback)
// Bits 30..36 reserved (7 targets)
#define HWY_RVV (1LL << 37)
// Bit 38 reserved
#define HWY_HIGHEST_TARGET_BIT_RVV 38

// --------------------------- Future expansion: 4 targets
// Bits 39..42 reserved


// --------------------------- IBM Power: 9 targets (+ one fallback)
// Bits 43..46 reserved (4 targets)
#define HWY_PPC10 (1LL << 47)  // v3.1
#define HWY_PPC9 (1LL << 48)   // v3.0
#define HWY_PPC8 (1LL << 49)   // v2.07
// Bits 50..51 reserved for prior VSX/AltiVec (2 targets)
#define HWY_HIGHEST_TARGET_BIT_PPC 51

// --------------------------- WebAssembly: 9 targets (+ one fallback)
// Bits 52..57 reserved (6 targets)
#define HWY_WASM_EMU256 (1LL << 58)  // Experimental
#define HWY_WASM (1LL << 59)
// Bits 60 reserved
#define HWY_HIGHEST_TARGET_BIT_WASM 60

// --------------------------- Emulation: 2 targets

#define HWY_EMU128 (1LL << 61)
// We do not add/left-shift, so this will not overflow to a negative number.
#define HWY_SCALAR (1LL << 62)
#define HWY_HIGHEST_TARGET_BIT_SCALAR 62

// Do not use bit 63 - would be confusing to have negative numbers.

//------------------------------------------------------------------------------
// Set default blocklists

// Disabled means excluded from enabled at user's request. A separate config
// macro allows disabling without deactivating the blocklist below.
#ifndef HWY_DISABLED_TARGETS
#define HWY_DISABLED_TARGETS 0
#endif

// Broken means excluded from enabled due to known compiler issues. We define
// separate HWY_BROKEN_* and then OR them together (more than one might apply).

// x86 clang-6: we saw multiple AVX2/3 compile errors and in one case invalid
// SSE4 codegen (possibly only for msan), so disable all those targets.
#if HWY_ARCH_X86 && (HWY_COMPILER_CLANG != 0 && HWY_COMPILER_CLANG < 700)

#define HWY_BROKEN_CLANG6 \
  (HWY_SSE4 | HWY_AVX2 | HWY_AVX3 | HWY_AVX3_DL | HWY_AVX3_ZEN4)
// This entails a major speed reduction, so warn unless the user explicitly
// opts in to scalar-only.
#if !defined(HWY_COMPILE_ONLY_SCALAR)
#pragma message("x86 Clang <= 6: define HWY_COMPILE_ONLY_SCALAR or upgrade.")
#endif

#else
#define HWY_BROKEN_CLANG6 0
#endif

// 32-bit may fail to compile AVX2/3.
#if HWY_ARCH_X86_32
#define HWY_BROKEN_32BIT (HWY_AVX2 | HWY_AVX3 | HWY_AVX3_DL | HWY_AVX3_ZEN4)
#else
#define HWY_BROKEN_32BIT 0
#endif

// MSVC AVX3 support is buggy: https://github.com/Mysticial/Flops/issues/16
#if HWY_COMPILER_MSVC != 0
#define HWY_BROKEN_MSVC (HWY_AVX3 | HWY_AVX3_DL | HWY_AVX3_ZEN4)
#else
#define HWY_BROKEN_MSVC 0
#endif

// AVX3_DL and AVX3_ZEN4 require clang >= 7 (ensured above) or gcc >= 8.1.
#if (HWY_COMPILER_GCC_ACTUAL && HWY_COMPILER_GCC_ACTUAL < 801)
#define HWY_BROKEN_AVX3_DL_ZEN4 (HWY_AVX3_DL | HWY_AVX3_ZEN4)
#else
#define HWY_BROKEN_AVX3_DL_ZEN4 0
#endif

// armv7be has not been tested and is not yet supported.
#if HWY_ARCH_ARM_V7 &&            \
    (defined(__ARM_BIG_ENDIAN) || \
     (defined(__BYTE_ORDER) && __BYTE_ORDER == __BIG_ENDIAN))
#define HWY_BROKEN_ARM7_BIG_ENDIAN (HWY_NEON | HWY_NEON_WITHOUT_AES)
#else
#define HWY_BROKEN_ARM7_BIG_ENDIAN 0
#endif

// armv7-a without a detected vfpv4 is not supported
// (for example Cortex-A8, Cortex-A9)
// vfpv4 always have neon half-float _and_ FMA.
#if HWY_ARCH_ARM_V7 && (__ARM_ARCH_PROFILE == 'A') && \
    !defined(__ARM_VFPV4__) &&                        \
    !((__ARM_NEON_FP & 0x2 /* half-float */) && (__ARM_FEATURE_FMA == 1))
#define HWY_BROKEN_ARM7_WITHOUT_VFP4 (HWY_NEON | HWY_NEON_WITHOUT_AES)
#else
#define HWY_BROKEN_ARM7_WITHOUT_VFP4 0
#endif

// SVE[2] require recent clang or gcc versions.
#if (HWY_COMPILER_CLANG && HWY_COMPILER_CLANG < 1100) || \
    (HWY_COMPILER_GCC_ACTUAL && HWY_COMPILER_GCC_ACTUAL < 1000)
#define HWY_BROKEN_SVE (HWY_SVE | HWY_SVE2 | HWY_SVE_256 | HWY_SVE2_128)
#else
#define HWY_BROKEN_SVE 0
#endif

// RVV compiles with current clang but not GCC.
// TODO(janwas): re-enable for new GCC once the required version is known.
#if HWY_COMPILER_GCC_ACTUAL
#define HWY_BROKEN_RVV (HWY_RVV)
#else
#define HWY_BROKEN_RVV 0
#endif

// There are GCC/Clang compiler bugs on big-endian PPC with the -mcpu=power10
// option if optimizations are enabled
#if HWY_ARCH_PPC && defined(__BYTE_ORDER__) && \
    defined(__ORDER_LITTLE_ENDIAN__) &&        \
    __BYTE_ORDER__ != __ORDER_LITTLE_ENDIAN__
#define HWY_BROKEN_PPC10 (HWY_PPC10)
#else
#define HWY_BROKEN_PPC10 0
#endif

// Allow the user to override this without any guarantee of success.
#ifndef HWY_BROKEN_TARGETS

#define HWY_BROKEN_TARGETS                                          \
  (HWY_BROKEN_CLANG6 | HWY_BROKEN_32BIT | HWY_BROKEN_MSVC |         \
   HWY_BROKEN_AVX3_DL_ZEN4 | HWY_BROKEN_ARM7_BIG_ENDIAN |           \
   HWY_BROKEN_ARM7_WITHOUT_VFP4 | HWY_BROKEN_SVE | HWY_BROKEN_RVV | \
   HWY_BROKEN_PPC10)

#endif  // HWY_BROKEN_TARGETS

// Enabled means not disabled nor blocklisted.
#define HWY_ENABLED(targets) \
  ((targets) & ~((HWY_DISABLED_TARGETS) | (HWY_BROKEN_TARGETS)))

// Opt-out for EMU128 (affected by a GCC bug on multiple arches, fixed in 12.3:
// see https://gcc.gnu.org/bugzilla/show_bug.cgi?id=106322). This is separate
// from HWY_BROKEN_TARGETS because it affects the fallback target, which must
// always be enabled. If 1, we instead choose HWY_SCALAR even without
// HWY_COMPILE_ONLY_SCALAR being set.
#if !defined(HWY_BROKEN_EMU128)  // allow overriding
#if (HWY_COMPILER_GCC_ACTUAL && HWY_COMPILER_GCC_ACTUAL < 1203) || \
    defined(HWY_NO_LIBCXX)
#define HWY_BROKEN_EMU128 1
#else
#define HWY_BROKEN_EMU128 0
#endif
#endif  // HWY_BROKEN_EMU128

//------------------------------------------------------------------------------
// Detect baseline targets using predefined macros

// Baseline means the targets for which the compiler is allowed to generate
// instructions, implying the target CPU would have to support them. This does
// not take the blocklist into account.

#if defined(HWY_COMPILE_ONLY_SCALAR) || HWY_BROKEN_EMU128
#define HWY_BASELINE_SCALAR HWY_SCALAR
#else
#define HWY_BASELINE_SCALAR HWY_EMU128
#endif

// Also check HWY_ARCH to ensure that simulating unknown platforms ends up with
// HWY_TARGET == HWY_BASELINE_SCALAR.

#if HWY_ARCH_WASM && defined(__wasm_simd128__)
#if defined(HWY_WANT_WASM2)
#define HWY_BASELINE_WASM HWY_WASM_EMU256
#else
#define HWY_BASELINE_WASM HWY_WASM
#endif  // HWY_WANT_WASM2
#else
#define HWY_BASELINE_WASM 0
#endif

// GCC or Clang.
#if HWY_ARCH_PPC && HWY_COMPILER_GCC && defined(__ALTIVEC__) && \
    defined(__VSX__) && defined(__POWER8_VECTOR__) &&           \
    (defined(__CRYPTO__) || defined(HWY_DISABLE_PPC8_CRYPTO))
#define HWY_BASELINE_PPC8 HWY_PPC8
#else
#define HWY_BASELINE_PPC8 0
#endif

#if HWY_BASELINE_PPC8 != 0 && defined(__POWER9_VECTOR__)
#define HWY_BASELINE_PPC9 HWY_PPC9
#else
#define HWY_BASELINE_PPC9 0
#endif

#if HWY_BASELINE_PPC9 != 0 && \
    (defined(_ARCH_PWR10) || defined(__POWER10_VECTOR__))
#define HWY_BASELINE_PPC10 HWY_PPC10
#else
#define HWY_BASELINE_PPC10 0
#endif

#define HWY_BASELINE_SVE2 0
#define HWY_BASELINE_SVE 0
#define HWY_BASELINE_NEON 0

#if HWY_ARCH_ARM

#if defined(__ARM_FEATURE_SVE2)
#undef HWY_BASELINE_SVE2  // was 0, will be re-defined
// If user specified -msve-vector-bits=128, they assert the vector length is
// 128 bits and we should use the HWY_SVE2_128 (more efficient for some ops).
#if defined(__ARM_FEATURE_SVE_BITS) && __ARM_FEATURE_SVE_BITS == 128
#define HWY_BASELINE_SVE2 HWY_SVE2_128
// Otherwise we're not sure what the vector length will be. The baseline must be
// unconditionally valid, so we can only assume HWY_SVE2. However, when running
// on a CPU with 128-bit vectors, user code that supports dynamic dispatch will
// still benefit from HWY_SVE2_128 because we add it to HWY_ATTAINABLE_TARGETS.
#else
#define HWY_BASELINE_SVE2 HWY_SVE2
#endif  // __ARM_FEATURE_SVE_BITS
#endif  // __ARM_FEATURE_SVE2

#if defined(__ARM_FEATURE_SVE)
#undef HWY_BASELINE_SVE  // was 0, will be re-defined
// See above. If user-specified vector length matches our optimization, use it.
#if defined(__ARM_FEATURE_SVE_BITS) && __ARM_FEATURE_SVE_BITS == 256
#define HWY_BASELINE_SVE HWY_SVE_256
#else
#define HWY_BASELINE_SVE HWY_SVE
#endif  // __ARM_FEATURE_SVE_BITS
#endif  // __ARM_FEATURE_SVE

// GCC 4.5.4 only defines __ARM_NEON__; 5.4 defines both.
#if defined(__ARM_NEON__) || defined(__ARM_NEON)
#undef HWY_BASELINE_NEON
#if defined(__ARM_FEATURE_AES)
#define HWY_BASELINE_NEON (HWY_NEON | HWY_NEON_WITHOUT_AES)
#else
#define HWY_BASELINE_NEON (HWY_NEON_WITHOUT_AES)
#endif
#endif

#endif  // HWY_ARCH_ARM

// Special handling for MSVC because it has fewer predefined macros:
#if HWY_COMPILER_MSVC

#if HWY_ARCH_X86_32
#if _M_IX86_FP >= 2
#define HWY_CHECK_SSE2 1
#else
#define HWY_CHECK_SSE2 0
#endif
#elif HWY_ARCH_X86_64
#define HWY_CHECK_SSE2 1
#else
#define HWY_CHECK_SSE2 0
#endif

// 1) We can only be sure SSSE3/SSE4 are enabled if AVX is:
//    https://stackoverflow.com/questions/18563978/.
#if defined(__AVX__)
#define HWY_CHECK_SSSE3 1
#define HWY_CHECK_SSE4 1
#else
#define HWY_CHECK_SSSE3 0
#define HWY_CHECK_SSE4 0
#endif

// 2) Cannot check for PCLMUL/AES and BMI2/FMA/F16C individually; we assume
//    PCLMUL/AES are available if SSE4 is, and BMI2/FMA/F16C if AVX2 is.
#define HWY_CHECK_PCLMUL_AES 1
#define HWY_CHECK_BMI2_FMA 1
#define HWY_CHECK_F16C 1

#else  // non-MSVC

#if defined(__SSE2__)
#define HWY_CHECK_SSE2 1
#else
#define HWY_CHECK_SSE2 0
#endif

#if defined(__SSSE3__)
#define HWY_CHECK_SSSE3 1
#else
#define HWY_CHECK_SSSE3 0
#endif

#if defined(__SSE4_1__) && defined(__SSE4_2__)
#define HWY_CHECK_SSE4 1
#else
#define HWY_CHECK_SSE4 0
#endif

// If these are disabled, they should not gate the availability of SSE4/AVX2.
#if defined(HWY_DISABLE_PCLMUL_AES) || (defined(__PCLMUL__) && defined(__AES__))
#define HWY_CHECK_PCLMUL_AES 1
#else
#define HWY_CHECK_PCLMUL_AES 0
#endif

#if defined(HWY_DISABLE_BMI2_FMA) || (defined(__BMI2__) && defined(__FMA__))
#define HWY_CHECK_BMI2_FMA 1
#else
#define HWY_CHECK_BMI2_FMA 0
#endif

#if defined(HWY_DISABLE_F16C) || defined(__F16C__)
#define HWY_CHECK_F16C 1
#else
#define HWY_CHECK_F16C 0
#endif

#endif  // non-MSVC

#if HWY_ARCH_X86 && (HWY_WANT_SSE2 || HWY_CHECK_SSE2)
#define HWY_BASELINE_SSE2 HWY_SSE2
#else
#define HWY_BASELINE_SSE2 0
#endif

#if HWY_ARCH_X86 && (HWY_WANT_SSSE3 || HWY_CHECK_SSSE3)
#define HWY_BASELINE_SSSE3 HWY_SSSE3
#else
#define HWY_BASELINE_SSSE3 0
#endif

#if HWY_ARCH_X86 && (HWY_WANT_SSE4 || (HWY_CHECK_SSE4 && HWY_CHECK_PCLMUL_AES))
#define HWY_BASELINE_SSE4 HWY_SSE4
#else
#define HWY_BASELINE_SSE4 0
#endif

#if HWY_BASELINE_SSE4 != 0 && HWY_CHECK_BMI2_FMA && HWY_CHECK_F16C && \
    defined(__AVX2__)
#define HWY_BASELINE_AVX2 HWY_AVX2
#else
#define HWY_BASELINE_AVX2 0
#endif

// Require everything in AVX2 plus AVX-512 flags (also set by MSVC)
#if HWY_BASELINE_AVX2 != 0 && defined(__AVX512F__) && defined(__AVX512BW__) && \
    defined(__AVX512DQ__) && defined(__AVX512VL__)
#define HWY_BASELINE_AVX3 HWY_AVX3
#else
#define HWY_BASELINE_AVX3 0
#endif

// TODO(janwas): not yet known whether these will be set by MSVC
#if HWY_BASELINE_AVX3 != 0 && defined(__AVX512VNNI__) && defined(__VAES__) && \
    defined(__VPCLMULQDQ__) && defined(__AVX512VBMI__) &&                  \
    defined(__AVX512VBMI2__) && defined(__AVX512VPOPCNTDQ__) &&            \
    defined(__AVX512BITALG__)
#define HWY_BASELINE_AVX3_DL HWY_AVX3_DL
#define HWY_BASELINE_AVX3_ZEN4 HWY_AVX3_ZEN4
#else
#define HWY_BASELINE_AVX3_DL 0
#define HWY_BASELINE_AVX3_ZEN4 0
#endif

#if HWY_ARCH_RVV && defined(__riscv_vector)
#define HWY_BASELINE_RVV HWY_RVV
#else
#define HWY_BASELINE_RVV 0
#endif

// Allow the user to override this without any guarantee of success.
#ifndef HWY_BASELINE_TARGETS
#define HWY_BASELINE_TARGETS                                     \
  (HWY_BASELINE_SCALAR | HWY_BASELINE_WASM | HWY_BASELINE_PPC8 | \
   HWY_BASELINE_PPC9 | HWY_BASELINE_PPC10 | HWY_BASELINE_SVE2 |  \
   HWY_BASELINE_SVE | HWY_BASELINE_NEON | HWY_BASELINE_SSE2 | \
   HWY_BASELINE_SSSE3 | HWY_BASELINE_SSE4 | HWY_BASELINE_AVX2 | \
   HWY_BASELINE_AVX3 | HWY_BASELINE_AVX3_DL | HWY_BASELINE_AVX3_ZEN4 | \
   HWY_BASELINE_RVV)
#endif  // HWY_BASELINE_TARGETS

//------------------------------------------------------------------------------
// Choose target for static dispatch

#define HWY_ENABLED_BASELINE HWY_ENABLED(HWY_BASELINE_TARGETS)
#if HWY_ENABLED_BASELINE == 0
#error "At least one baseline target must be defined and enabled"
#endif

// Best baseline, used for static dispatch. This is the least-significant 1-bit
// within HWY_ENABLED_BASELINE and lower bit values imply "better".
#define HWY_STATIC_TARGET (HWY_ENABLED_BASELINE & -HWY_ENABLED_BASELINE)

// Start by assuming static dispatch. If we later use dynamic dispatch, this
// will be defined to other targets during the multiple-inclusion, and finally
// return to the initial value. Defining this outside begin/end_target ensures
// inl headers successfully compile by themselves (required by Bazel).
#define HWY_TARGET HWY_STATIC_TARGET

//------------------------------------------------------------------------------
// Choose targets for dynamic dispatch according to one of four policies

#if 1 < (defined(HWY_COMPILE_ONLY_SCALAR) + defined(HWY_COMPILE_ONLY_EMU128) + \
         defined(HWY_COMPILE_ONLY_STATIC))
#error "Can only define one of HWY_COMPILE_ONLY_{SCALAR|EMU128|STATIC} - bug?"
#endif
// Defining one of HWY_COMPILE_ONLY_* will trump HWY_COMPILE_ALL_ATTAINABLE.

// Clang, GCC and MSVC allow runtime dispatch on x86.
#if HWY_ARCH_X86
#define HWY_HAVE_RUNTIME_DISPATCH 1
// On Arm/PPC, currently only GCC does, and we require Linux to detect CPU
// capabilities.
#elif (HWY_ARCH_ARM || HWY_ARCH_PPC) && HWY_COMPILER_GCC_ACTUAL && \
      HWY_OS_LINUX && !defined(TOOLCHAIN_MISS_SYS_AUXV_H)
#define HWY_HAVE_RUNTIME_DISPATCH 1
#else
#define HWY_HAVE_RUNTIME_DISPATCH 0
#endif

// AVX3_DL is not widely available yet. To reduce code size and compile time,
// only include it in the set of attainable targets (for dynamic dispatch) if
// the user opts in, OR it is in the baseline (we check whether enabled below).
#if defined(HWY_WANT_AVX3_DL) || (HWY_BASELINE & HWY_AVX3_DL)
#define HWY_ATTAINABLE_AVX3_DL (HWY_AVX3_DL)
#else
#define HWY_ATTAINABLE_AVX3_DL 0
#endif

#if HWY_ARCH_ARM_A64 && HWY_HAVE_RUNTIME_DISPATCH
#define HWY_ATTAINABLE_NEON (HWY_NEON | HWY_NEON_WITHOUT_AES)
#elif HWY_ARCH_ARM  // static dispatch, or HWY_ARCH_ARM_V7
#define HWY_ATTAINABLE_NEON (HWY_BASELINE_NEON)
#else
#define HWY_ATTAINABLE_NEON 0
#endif

#if HWY_ARCH_ARM_A64 && (HWY_HAVE_RUNTIME_DISPATCH || \
                         (HWY_ENABLED_BASELINE & (HWY_SVE | HWY_SVE_256)))
#define HWY_ATTAINABLE_SVE (HWY_SVE | HWY_SVE_256)
#else
#define HWY_ATTAINABLE_SVE 0
#endif

#if HWY_ARCH_ARM_A64 && (HWY_HAVE_RUNTIME_DISPATCH || \
                         (HWY_ENABLED_BASELINE & (HWY_SVE2 | HWY_SVE2_128)))
#define HWY_ATTAINABLE_SVE2 (HWY_SVE2 | HWY_SVE2_128)
#else
#define HWY_ATTAINABLE_SVE2 0
#endif

#if HWY_ARCH_PPC && !HWY_DISABLE_PPC8_CRYPTO
#define HWY_ATTAINABLE_PPC (HWY_PPC8 | HWY_PPC9 | HWY_PPC10)
#else
#define HWY_ATTAINABLE_PPC 0
#endif

// Attainable means enabled and the compiler allows intrinsics (even when not
// allowed to autovectorize). Used in 3 and 4.
#if HWY_ARCH_X86
#define HWY_ATTAINABLE_TARGETS                                        \
  HWY_ENABLED(HWY_BASELINE_SCALAR | HWY_SSE2 | HWY_SSSE3 | HWY_SSE4 | \
              HWY_AVX2 | HWY_AVX3 | HWY_ATTAINABLE_AVX3_DL | HWY_AVX3_ZEN4)
#elif HWY_ARCH_ARM
#define HWY_ATTAINABLE_TARGETS                                                 \
  HWY_ENABLED(HWY_BASELINE_SCALAR | HWY_ATTAINABLE_NEON | HWY_ATTAINABLE_SVE | \
              HWY_ATTAINABLE_SVE2)
#elif HWY_ARCH_PPC
#define HWY_ATTAINABLE_TARGETS \
  HWY_ENABLED(HWY_BASELINE_SCALAR | HWY_ATTAINABLE_PPC)
#else
#define HWY_ATTAINABLE_TARGETS (HWY_ENABLED_BASELINE)
#endif  // HWY_ARCH_*

// 1) For older compilers: avoid SIMD intrinsics, but still support all ops.
#if defined(HWY_COMPILE_ONLY_EMU128) && !HWY_BROKEN_EMU128
#undef HWY_STATIC_TARGET
#define HWY_STATIC_TARGET HWY_EMU128  // override baseline
#define HWY_TARGETS HWY_EMU128

// 1b) HWY_SCALAR is less capable than HWY_EMU128 (which supports all ops), but
// we currently still support it for backwards compatibility.
#elif defined(HWY_COMPILE_ONLY_SCALAR) || \
    (defined(HWY_COMPILE_ONLY_EMU128) && HWY_BROKEN_EMU128)
#undef HWY_STATIC_TARGET
#define HWY_STATIC_TARGET HWY_SCALAR  // override baseline
#define HWY_TARGETS HWY_SCALAR

// 2) For forcing static dispatch without code changes (removing HWY_EXPORT)
#elif defined(HWY_COMPILE_ONLY_STATIC)
#define HWY_TARGETS HWY_STATIC_TARGET

// 3) For tests: include all attainable targets (in particular: scalar)
#elif defined(HWY_COMPILE_ALL_ATTAINABLE) || defined(HWY_IS_TEST)
#define HWY_TARGETS HWY_ATTAINABLE_TARGETS

// 4) Default: attainable WITHOUT non-best baseline. This reduces code size by
// excluding superseded targets, in particular scalar. Note: HWY_STATIC_TARGET
// may be 2^62 (HWY_SCALAR), so we must not left-shift/add it. Subtracting one
// sets all lower bits (better targets), then we also include the static target.
#else
#define HWY_TARGETS \
  (HWY_ATTAINABLE_TARGETS & ((HWY_STATIC_TARGET - 1LL) | HWY_STATIC_TARGET))

#endif  // target policy

// HWY_ONCE and the multiple-inclusion mechanism rely on HWY_STATIC_TARGET being
// one of the dynamic targets. This also implies HWY_TARGETS != 0 and
// (HWY_TARGETS & HWY_ENABLED_BASELINE) != 0.
#if (HWY_TARGETS & HWY_STATIC_TARGET) == 0
#error "Logic error: best baseline should be included in dynamic targets"
#endif

#endif  // HIGHWAY_HWY_DETECT_TARGETS_H_
