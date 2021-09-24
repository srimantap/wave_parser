#include <stdlib.h>
#include <check.h>

#include "../src/parse.h"
#include "../src/log.h"

#define WRONG_FILE "/wrong/path/to/a/file"
#define WAV_FILE TEST_FILE_PATH "/test.wav"
#define WAV_FILE_LIST_CHUNK TEST_FILE_PATH "/test_list_chunk.wav"


START_TEST(test_parse_wave_invalid_file)
{
  struct wave_format wf;

  ck_assert_int_eq(parse_wave(WRONG_FILE, &wf), -1);
}
END_TEST

static void validate_basic_part(struct wave_format *wfmt, int chunk_size,
    int smpl_rate, int byterate)
{

  log_info("wfmt->chunk_id, %p", wfmt->chunk_id);
  //validate chunk id
  ck_assert_str_eq(wfmt->chunk_id, "RIFF");

  //validate chunk size
  ck_assert_int_eq(wfmt->chunk_size, chunk_size);

  //validate format
  ck_assert_str_eq(wfmt->format, "WAVE");

  //validate SubChunk1Id
  ck_assert_str_eq(wfmt->sub_chunk1_id, "fmt ");

  //validate SubChunk1Size
  ck_assert_int_eq(wfmt->sub_chunk1_size, 16);

  //validate Audio Format
  ck_assert_int_eq(wfmt->audio_format, 1);

  //validate NumChannel
  uint16_t nbr_channels= wfmt->nbr_channels;
  ck_assert_int_eq(nbr_channels, 1);

  //validate sample rate
  uint32_t sample_rate = wfmt->sample_rate;
  ck_assert_int_eq(sample_rate, smpl_rate);

  //validate byte rate
  uint32_t byte_rate = wfmt->byte_rate;
  ck_assert_int_eq(byte_rate, byterate);

  //validate Block Align
  uint16_t block_align = wfmt->block_align;
  ck_assert_int_eq(block_align, 2);

  //validate Bits per Sample
  uint16_t bps = wfmt->bits_per_sample;
  ck_assert_int_eq(bps, 16);

  // Validate block align
  // BlockAlign = NumChannels * BitsPerSample/8
  ck_assert_int_eq(block_align, ((nbr_channels * bps) / 8));

  // ByteRate = SampleRate * NumChannels * BitsPerSample/8
  ck_assert_int_eq(byte_rate, ((sample_rate * nbr_channels * bps) / 8));

  return;
}

START_TEST(test_parse_wave_valid_data_chunk)
{

  struct wave_format wf;

  //log_set_level(LOG_INFO);
  ck_assert_int_eq(parse_wave(WAV_FILE, &wf), 0);

  validate_basic_part(&wf, 32036, 16000, 32000);

  //validate SubChunk2Id
  ck_assert_str_eq(wf.chunks->sub_chunk2_id, "data");

  //validate SubChunk2Size
  uint32_t sub_chunk2_size = wf.chunks->sub_chunk2_size;
  ck_assert_int_eq(sub_chunk2_size, 32000);

  // Subchunk2Size = NumSamples * NumChannels * BitsPerSample/8
  ck_assert_int_eq(wf.chunks->sub_chunk2_size, ((wf.sample_rate * wf.nbr_channels * wf.bits_per_sample) / 8));

  parse_finalize(&wf);
}
END_TEST

START_TEST(test_parse_wave_valid_list_chunk)
{

  struct wave_format wf;

  //log_set_level(LOG_INFO);
  ck_assert_int_eq(parse_wave(WAV_FILE_LIST_CHUNK, &wf), 0);

  validate_basic_part(&wf, 32838, 16384, 32768);

  //validate SubChunk2Id
  ck_assert_str_eq(wf.chunks->sub_chunk2_id, "LIST");

  //validate SubChunk2Size
  uint32_t sub_chunk2_size = wf.chunks->sub_chunk2_size;
  ck_assert_int_eq(sub_chunk2_size, 26);

  parse_finalize(&wf);
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
  tcase_add_test(tc_core, test_parse_wave_valid_data_chunk);
  tcase_add_test(tc_core, test_parse_wave_valid_list_chunk);
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
