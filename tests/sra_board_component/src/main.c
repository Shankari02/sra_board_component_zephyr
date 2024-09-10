/*
 * Copyright (c) 2020 Friedt Professional Engineering Services, Inc
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <ztest.h>

#include <sra_board_component.h>

static void test_bar_graph(void)
{
	int err;

    err = bar_graph_init();
    zassert_equal(err, 0, "bar_graph_init() failed");

    err = bar_graph_toggle();
    zassert_equal(err, 0, "bar_graph_toggle() failed");
}

void test_main(void)
{
	ztest_test_suite(
		sra_board_component,
		ztest_unit_test(test_bar_graph)
	);

	ztest_run_test_suite(sra_board_component);
}
