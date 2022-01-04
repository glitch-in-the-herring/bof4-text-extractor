#include "extractor.h"

word convert_little_endian(byte a[], int n, int bytes)
{
    word result = 0;
    for (int i = bytes - 1; i >= 0; i--)
    {
        result = a[n + i] << 8 * i | result;
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
            *section_size = convert_little_endian(toc_entry, 0, 4);
            return address;
        }

        tmp = convert_little_endian(toc_entry, 0, 4);
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

    table[i][j] = '\0';
}

bool is_math_tbl(byte toc_header[])
{
    byte magic[8] = {0x4d, 0x41, 0x54, 0x48, 0x5f, 0x54, 0x42, 0x4c};
    for (int i = 8; i < 16; i++)
    {
        if (toc_header[i] != magic[i - 8])
        {
            return false;
        }
    }
    return true;
}

bool is_dialogue_section(byte toc_entry[])
{
    return toc_entry[4] == 0x00 && toc_entry[5] == 0x00 && toc_entry[6] == 0x01 && toc_entry[7] == 0x80;
}

bool is_alphanum(byte a)
{
    return (a >= 65 && a <= 90) || (a >= 97 && a <= 122) || (a >= 48 && a <= 57);
}

bool is_punct(byte a)
{
    return (a >= 32 && a <= 47) || (a >= 58 && a <= 63);
}

char *is_position(byte a)
{
    static char buffer[20];
    byte position = a & 0x0f;
    byte style = (a & 0xf0) >> 4;

    switch (position)
    {
        case 0x00:
            strcpy(buffer, "[BOTTOMM");
            break;
        case 0x01:
            strcpy(buffer, "[MIDM");
            break;
        case 0x02:
            strcpy(buffer, "[TOPM");
            break;
        case 0x03:
            strcpy(buffer, "[TOPL");
            break;
        case 0x04:
            strcpy(buffer, "[TOPR");
            break;
        case 0x05:
            strcpy(buffer, "[BOTTOML");
            break;
        case 0x06:
            strcpy(buffer, "[BOTTOMR");
            break;
    }

    switch (style)
    {
        case 0x00:
            strcat(buffer, "] ");
            break;
        case 0x02:
            strcat(buffer, " FOCUS] ");
            break;
        case 0x04:
            strcat(buffer, " SMALL] ");
            break;
        case 0x08:
            strcat(buffer, " NOBOX] ");
            break;
    }

    return buffer;
}

char *is_symbol(byte a)
{
    switch (a)
    {
        case 0x00:
            return "◯";
            break;
        case 0x01:
            return "×";
            break; 
        case 0x02:
            return "△";
            break;
        case 0x03:
            return "□";
            break;
        case 0x06:
            return "★";
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
            return "GREY";
            break;
        case 0x02:
            return "RED";
            break;
        case 0x03:
            return "CYAN";
            break;
        case 0x04:
            return "GREEN";
            break;
        case 0x05:
            return "PINK";
            break;
        case 0x06:
            return "YELLOW";
            break;
        case 0x07:
            return "MAGENTA";
            break;
        case 0x08:
            return "WHITE";
            break;
        case 0x0A:
            return "PALETTE";
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
