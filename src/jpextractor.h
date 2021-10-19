#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#define HRSTART 0x5e
#define HREND 0xad
#define KTSTART 0xae
#define KTEND 0xfe
#define KJSTART 0x1200

typedef uint8_t byte;
typedef uint32_t word;

word convert_little_endian(byte a[], int n, int k);
word find_dialogue_section(FILE *f, word count, word *section_size);
void load_lookup_table(int n, char table[][n], FILE *source);
bool is_math_tbl(byte toc_header[]);
bool is_dialogue_section(byte toc_entry[]);
bool is_hiragana(byte a);
bool is_katakana(byte a);
bool is_kanji_start(byte a);
char *is_position(byte a);
char *is_punct(byte a);
char *is_color(byte a);
char *is_effect(byte a);
