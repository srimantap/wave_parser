#include <stdlib.h>
#include <check.h>

#include "../src/parse.h"
#include "../src/log.h"

#define WRONG_FILE "/wrong/path/to/a/file"
#define WAV_FILE TEST_FILE_PATH "/test.wav"


START_TEST(test_parse_wave_invalid_file)
{
  struct wave_format wf;

  ck_assert_int_eq(parse_wave(WRONG_FILE, &wf), -1);
}
END_TEST

START_TEST(test_parse_wave_valid)
{

  struct wave_format wf;
  uint32_t size;
  uint16_t size_16;

  ck_assert_int_eq(parse_wave(WAV_FILE, &wf), 0);

  //validate chunk id
  //ck_assert_str_eq(wf.chunk_id, "RIFF");

  //validate chunk size
  size = *(uint32_t *) &wf.chunk_size;
  ck_assert_int_eq(size, 32036);

  //validate format
  //ck_assert_str_eq(str_format, "WAVE");

  //validate SubChunk1Id
  //ck_assert_str_eq(str_sub_chunk1_id, "fmt ");

  //validate SubChunk1Size
  size = wf.sub_chunk1_size;
  ck_assert_int_eq(size, 16);

  //validate Audio Format
  size_16 = *(uint16_t *) &wf.audio_format;
  ck_assert_int_eq(size_16, 1);

  //validate NumChannel
  uint16_t nbr_channels= *(uint16_t *) &wf.nbr_channels;
  ck_assert_int_eq(nbr_channels, 1);

  //validate sample rate
  uint32_t sample_rate = *(uint32_t *) &wf.sample_rate;
  ck_assert_int_eq(sample_rate, 16000);

  //validate byte rate
  uint32_t byte_rate = *(uint32_t *) &wf.byte_rate;
  ck_assert_int_eq(byte_rate, 32000);

  //validate Block Align
  uint16_t block_align = *(uint16_t *) &wf.block_align;
  ck_assert_int_eq(block_align, 2);

  //validate Bits per Sample
  uint16_t bps = *(uint16_t *) &wf.bits_per_sample;
  ck_assert_int_eq(bps, 16);

  // Validate block align
  // BlockAlign = NumChannels * BitsPerSample/8
  ck_assert_int_eq(block_align, ((nbr_channels * bps) / 8));

  // ByteRate = SampleRate * NumChannels * BitsPerSample/8
  ck_assert_int_eq(byte_rate, ((sample_rate * nbr_channels * bps) / 8));

  //validate SubChunk2Id
  //ck_assert_str_eq(str_sub_chunk2_id, "data");

  //validate SubChunk2Size
  uint32_t sub_chunk2_size = *(uint32_t *) &wf.sub_chunk2_size;
  ck_assert_int_eq(sub_chunk2_size, 32000);

  // Subchunk2Size = NumSamples * NumChannels * BitsPerSample/8
  ck_assert_int_eq(sub_chunk2_size, ((sample_rate * nbr_channels * bps) / 8));
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
