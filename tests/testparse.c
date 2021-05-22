#include <stdlib.h>
#include <check.h>

#include "../src/parse.h"

START_TEST(test_test_func)
{

  ck_assert_int_eq(test_func(), 0);
}
END_TEST

Suite * money_suite(void)
{
  Suite *s;
  TCase *tc_core;

  s = suite_create("Wave Parse");

  /* Core test case */
  tc_core = tcase_create("Parse");

  tcase_add_test(tc_core, test_test_func);
  suite_add_tcase(s, tc_core);

  return s;
}

int main(void)
{
  int number_failed;
  Suite *s;
  SRunner *sr;

  s = money_suite();
  sr = srunner_create(s);

  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
 }
