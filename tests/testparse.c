#include <stdlib.h>
#include <check.h>

#include "../src/parse.h"
#include "../src/log.h"

#define WRONG_FILE "/wrong/path/to/a/file"
#define WAV_FILE TEST_FILE_PATH "/test.wav"

START_TEST(test_parse_wave_invalid_file)
{

  ck_assert_int_eq(parse_wave(WRONG_FILE), -1);
}
END_TEST

START_TEST(test_parse_wave_valid)
{

  ck_assert_int_eq(parse_wave(WAV_FILE), 0);
}
END_TEST

Suite * wave_parse_suite(void)
{
  Suite *s;
  TCase *tc_core;

  s = suite_create("Wave Parse");

  /* Core test case */
  tc_core = tcase_create("Parse");

  tcase_add_test(tc_core, test_parse_wave_invalid_file);
  tcase_add_test(tc_core, test_parse_wave_valid);
  suite_add_tcase(s, tc_core);

  return s;
}

int main(void)
{
  int number_failed;
  Suite *s;
  SRunner *sr;

  s = wave_parse_suite();
  sr = srunner_create(s);

  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
 }
