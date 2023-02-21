// Copyright Microsoft and CHERIoT Contributors.
// SPDX-License-Identifier: MIT

#define TEST_NAME "Stack tests"
#include "stack_tests.h"
#include "tests.hh"
#include <cheri.hh>
#include <errno.h>

using namespace CHERI;

bool leakedSwitcherCapability = false;
bool threadStackTestFailed    = false;
bool inTrustedStackExhaustion = false;

extern "C" ErrorRecoveryBehaviour
compartment_error_handler(ErrorState *frame, size_t mcause, size_t mtval)
{
	if (holds_switcher_capability(frame))
	{
		TEST(false, "Leaked switcher capabilities to stack_test compartment");
	}

	/* It's bad practice to InstallContext by default. If there are
	 * unpredicted errors, we want to make them sound.
	 * Therefore, we ForceUnwind by default, and InstallContext if we
	 * know the inner compartments indicate unexpected failures.
	 */
	if (inTrustedStackExhaustion && !leakedSwitcherCapability)
	{
		return ErrorRecoveryBehaviour::InstallContext;
	}

	if (!inTrustedStackExhaustion && !threadStackTestFailed)
	{
		return ErrorRecoveryBehaviour::InstallContext;
	}

	TEST(false, "Should be unreachable");
	return ErrorRecoveryBehaviour::ForceUnwind;
}

__cheri_callback void test_trusted_stack_exhaustion()
{
	exhaust_trusted_stack(&test_trusted_stack_exhaustion,
	                      &leakedSwitcherCapability);
}

void test_stack_exhaustion()
{
	inTrustedStackExhaustion = true;
	leakedSwitcherCapability = false;
	test_trusted_stack_exhaustion();

	inTrustedStackExhaustion = false;
	threadStackTestFailed    = false;
	exhaust_thread_stack(&threadStackTestFailed);
}

/*
 * The stack tests should cover the edge-cases scenarios for both
 * the trusted and compartment stacks. We make sure the
 * switcher handle them correctly. We check:
 * 	- trusted stack exhaustion
 *  - compartment stack exhaustion
 *	- compartment stack with incorrect permissions
 *  - invalid compartment stack
 */
void test_stack()
{
	test_stack_exhaustion();
	test_stack_permissions();
	test_stack_invalid();
}