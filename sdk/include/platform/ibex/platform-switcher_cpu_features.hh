// Copyright SCI Semiconductor and CHERIoT Contributors.
// SPDX-License-Identifier: MIT

#pragma once

#include "ibex-csr.h"

/**
 * Flag to pass to switcher_invocation_cpu_features_set to enable or disable the
 * Ibex I$.
 */
#define SWITCHER_CPU_FEATURE_CACHE_INSTRUCTION_ENABLED                         \
	IbexCSR_Cpuctrlsts_IcacheEnable

/**
 * Flag to pass to switcher_invocation_cpu_features_set to enable or disable
 * Ibex's "data independent timing" behaviors.
 */
#define SWITCHER_CPU_FEATURE_PLATFORM_DATA_INDEPENDENT_TIMING                  \
	IbexCSR_Cpuctrlsts_DataIndependentTiming

/// Default to I$ on and "data independent timing" off.
#define SWITCHER_CPU_FEATURE_DEFAULT IbexCSR_Cpuctrlsts_IcacheEnable

#if defined(__ASSEMBLER__) && !defined(CLANG_TIDY)

/// A mask of all bits that can be set in cpuFeatures
#	define SWITCHER_CPU_FEATURE_PLATFORM_MASK                                 \
		(IbexCSR_Cpuctrlsts_IcacheEnable |                                     \
		 IbexCSR_Cpuctrlsts_DataIndependentTiming)

// clang-format off

.macro platform_switcher_cpu_features_activate newValue, scratch
	csrr               \scratch, IbexCSR_Cpuctrlsts_index
	andi               \scratch, \scratch, SWITCHER_CPU_FEATURE_PLATFORM_MASK
	or                 \scratch, \scratch, \newValue
	csrw               IbexCSR_Cpuctrlsts_index, \scratch
.endm

.macro platform_switcher_cpu_features_mask newValue, scratch
	andi               \newValue, \newValue, SWITCHER_CPU_FEATURE_PLATFORM_MASK
.endm

// clang-format on

#endif
