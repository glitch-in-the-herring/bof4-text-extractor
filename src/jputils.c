#include "jpextractor.h"

word convert_little_endian(byte a[], int n, int k)
{
    word result = 0;
    for (int i = n; i >= k; i--)
    {
        result = a[i] << 8 * i | result;
    }
    return result;
}

word find_dialogue_section(FILE *f, word count, word *section_size)
{
    byte toc_entry[16];
    word tmp = 0;
    word address = 0x0800;
    do
    {
        address += tmp;
        if (fread(toc_entry, 1, sizeof(toc_entry), f) != sizeof(toc_entry))
        {
            return -1;
        }

        if (is_dialogue_section(toc_entry))
        {
            *section_size = convert_little_endian(toc_entry, 3, 0);
            return address;
        }

        tmp = convert_little_endian(toc_entry, 3, 0);
        if (tmp % 0x0800 != 0)
        {
            tmp = 0x0800 * ((tmp + 0x0800) / 0x0800);
        }
    }
    while (count--);
    return 0;
}

void load_lookup_table(int n, char table[][n], FILE *source)
{
    int i = 0;
    int j = 0;
    char c;
    while((c = fgetc(source)) != EOF)
    {
        if (c != '\n')
        {
            table[i][j] = c;
            j++;
        }
        else
        {
            table[i][j] = '\0';
            j = 0;
            i++;
        }
    }
}

bool is_math_tbl(byte toc_header[])
{
    byte magic[8] = {0x4d, 0x41, 0x54, 0x48, 0x5f, 0x54, 0x42, 0x4c};
    for (int i = 8; i < 16; i++)
    {
        if(toc_header[i] != magic[i - 8])
        {
            return false;
        }
    }
    return true;
}

bool is_dialogue_section(byte toc_entry[])
{
    return toc_entry[8] == 0x00 && toc_entry[9] == 0x02 && toc_entry[10] != 0x16 && toc_entry[11] == 0x02;
}

bool is_hiragana(byte a)
{
    return a >= HRSTART && a <= HREND;
}

bool is_katakana(byte a)
{
    return a >= KTSTART && a <= KTEND;
}

bool is_kanji_start(byte a)
{
    return a == 0x12 || a == 0x13;
}

char *is_punct(byte a)
{
    switch (a)
    {
        case 0x2A:
            return "「";
            break;
        case 0x3E:
            return "‥";
            break;
        case 0x3F:
            return "？";
            break;
        case 0x40:
            return "！";
            break;
        case 0xFC:
            return "ー";
            break;
        case 0xFD:
            return "。";
            break;
        case 0xFE:
            return "、";
            break;
        default:
            return "";
            break;
    }
}

char *is_color(byte a)
{
    switch (a)
    {
        case 0x01:
            return "PURPLE";
            break;
        case 0x02:
            return "RED";
            break;
        case 0x03:
            return "CYAN";
            break;
        case 0x04:
            return "YELLOW";
            break;
        case 0x05:
            return "PINK";
            break;
        case 0x06:
            return "GREEN";
            break;
        case 0x07:
            return "BLACK";
            break;
        default:
            return "";
            break;            
    }    
}

char *is_effect(byte a)
{
    switch (a)
    {
        case 0x00:
            return "type=shake persistent=no";
            break;
        case 0x01:
            return "type=shake persistent=yes";
            break;
        case 0x02:
            return "type=big1 persistent=no";
            break;
        case 0x03:
            return "type=big1 persistent=yes";
            break;
        case 0x04:
            return "size=big2 persistent=no";
            break;
        case 0x05:
            return "size=big2 persistent=yes";
            break;
        case 0x06:
            return "size=small persistent=no";
            break;
        case 0x07:
            return "size=small persistent=yes";
            break;
        case 0x08:
            return "type=wave_ascend";
            break;
        case 0x09:
            return "type=jump";
            break;
        default:
            return "";
            break;
    }    
}
