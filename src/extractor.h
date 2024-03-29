#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

typedef uint8_t byte;
typedef uint32_t word;

word convert_little_endian(byte a[], int n, int k);
word find_dialogue_section(FILE *f, word count, word *section_size);
void load_lookup_table(int n, char table[][n], FILE *source);
bool is_math_tbl(byte toc_header[]);
bool is_dialogue_section(byte toc_entry[]);
bool is_alphanum(byte a);
bool is_punct(byte a);
char *is_position(byte a);
char *is_zenny_position(byte a);
char *is_symbol(byte a);
char *is_color(byte a);
char *is_effect(byte a);
