#include "extractor.h"

int main(int argc, char *argv[])
{
    /*
     * Command-line argument related checks
     */

    if (argc != 3)
    {
        printf("Usage: %s input_file output_file\n", argv[0]);
        return 1;
    }

    /*
     * File validation
     */    

    FILE *area_file = fopen(argv[1], "rb");
    if (area_file == NULL)
    {
        printf("Error opening input file\n");
        return 2;
    }

    byte toc_header[16];
    if (fread(toc_header, 1, sizeof(toc_header), area_file) != sizeof(toc_header))
    {
        printf("Error reading file!\n");
        fclose(area_file);
        return 3;
    }
    
    if (!is_math_tbl(toc_header))
    {
        printf("Not an .EMI file!\n");
        fclose(area_file);
        return 4;
    }

    word file_count = convert_little_endian(toc_header, 0, 4);
    word address;
    word section_size;

    if ((address = find_dialogue_section(area_file, file_count, &section_size)) == 0)
    {
        printf("Dialogue section not found!\n");
        fclose(area_file);
        return 5;
    }
    fseek(area_file, address, SEEK_SET);

    /*
     * Load tables
     */

    FILE *acc_source = fopen("acc_en.src", "r");
    if (acc_source == NULL)
    {
        printf("Error reading accessory table!");
        fclose(area_file);
        return 6;
    }

    FILE *abils_source = fopen("abils_en.src", "r");
    if (abils_source == NULL)
    {
        printf("Error reading accessory table!");
        fclose(area_file);
        fclose(acc_source);
        return 6;
    }    

    FILE *armor_source = fopen("armor_en.src", "r");
    if (armor_source == NULL)
    {
        printf("Error reading armor table!");
        fclose(area_file);
        fclose(acc_source);
        fclose(abils_source);
        return 6;
    }

    FILE *items_source = fopen("items_en.src", "r");
    if (items_source == NULL)
    {
        printf("Error reading items table!");
        fclose(area_file);
        fclose(acc_source);
        fclose(abils_source);
        fclose(armor_source);
        return 6;
    }

    FILE *weapons_source = fopen("weapons_en.src", "r");
    if (weapons_source == NULL)
    {
        printf("Error reading weapons table!");
        fclose(area_file);
        fclose(acc_source);
        fclose(abils_source);
        fclose(armor_source);
        fclose(items_source);
        return 6;
    }    

    FILE *output_file = fopen(argv[2], "w");
    if (output_file == NULL)
    {
        printf("Error opening output file\n");
        fclose(area_file);
        fclose(acc_source);
        fclose(abils_source);
        fclose(armor_source);
        fclose(items_source);
        fclose(weapons_source);
        return 6;
    }    

    byte dialogue_section[section_size];
    if (fread(dialogue_section, 1, sizeof(dialogue_section), area_file) != sizeof(dialogue_section))
    {
        printf("Error reading input file!\n");
        fclose(area_file);
        fclose(output_file);
        fclose(acc_source);
        fclose(abils_source);
        fclose(armor_source);
        fclose(items_source);
        fclose(weapons_source);
        return 7;
    }

    word pointer_size = convert_little_endian(dialogue_section, 0, 2);
    word pointer;

    char acc_table[86][13];
    char abils_table[255][13];
    char armor_table[69][13];
    char items_table[118][13];
    char weapons_table[88][13];
    load_lookup_table(13, acc_table, acc_source);
    load_lookup_table(13, abils_table, abils_source);
    load_lookup_table(13, armor_table, armor_source);
    load_lookup_table(13, items_table, items_source);
    load_lookup_table(13, weapons_table, weapons_source);
    fclose(acc_source);
    fclose(abils_source);
    fclose(armor_source);
    fclose(items_source);
    fclose(weapons_source);

    char last_color[8];
    char symbol[4];
    char box_position[20];
    int j;

    for (int i = 0; i < pointer_size; i++)
    {
        pointer = convert_little_endian(dialogue_section, i * 2, 2);
        j = pointer;

        if (pointer == pointer_size && i != 0)
        {
            continue;
        }
        else if (pointer + 1 == section_size)
        {
            break;
        }

        while (dialogue_section[j] != 0x00 && dialogue_section[j] != 0x16)
        {
            if (is_alphanum(dialogue_section[j]) || is_punct(dialogue_section[j]))
            {
                fprintf(output_file, "%c", dialogue_section[j]);
            }        
            else if (dialogue_section[j] == 0x0c)
            {
                strcpy(box_position, is_position(dialogue_section[j + 1]));
                fprintf(output_file, "%s", box_position);
                j++;
            }
            else if (dialogue_section[j] == 0x0d)
            {
                fprintf(output_file, "[EFFECT]");
            }
            else if (dialogue_section[j] == 0x01)
            {
                fprintf(output_file, "\n");
            }
            else if (dialogue_section[j] == 0x0b)
            {
                fprintf(output_file, "--");
            }
            else if (dialogue_section[j] == 0x02)
            {
                fprintf(output_file, "\n        ▼        \n");
            }
            else if (dialogue_section[j] == 0x0f && dialogue_section[j - 1] == 0x0e)
            {
                fprintf(output_file, "[/EFFECT %s]", is_effect(dialogue_section[j + 1]));
                j += 2;    
            }
            else if (dialogue_section[j] == 0x04)
            {
                switch (dialogue_section[j + 1])
                {
                    case 0x00:
                        fprintf(output_file, "Ryu");
                        j++;
                        break;
                    case 0x01:
                        fprintf(output_file, "Nina");
                        j++;
                        break;
                    case 0x02:
                        fprintf(output_file, "Cray");
                        j++;
                        break;
                    case 0x03:
                        fprintf(output_file, "Scias");
                        j++;
                        break;
                    case 0x04:
                        fprintf(output_file, "Ursula");
                        j++;
                        break;
                    case 0x05:
                        fprintf(output_file, "Ershin");
                        j++;
                        break;
                    case 0x06:
                        fprintf(output_file, "Fou-Lu");
                        j++;
                        break;
                    default:
                        break;
                }
            }
            else if (dialogue_section[j] == 0x05)
            {
                strcpy(last_color, is_color(dialogue_section[j + 1]));
                fprintf(output_file, "[%s]", last_color);
                j++;            
            }
            else if (dialogue_section[j] == 0x06)
            {
                fprintf(output_file, "[/%s]", last_color);
            }
            else if (dialogue_section[j] == 0x09)
            {
                byte id = dialogue_section[j + 1];
                switch (dialogue_section[j + 2])
                {
                    case 0x00:
                        fprintf(output_file, "%s", items_table[id]);
                        break;
                    case 0x01:
                        fprintf(output_file, "%s", weapons_table[id]);
                        break;
                    case 0x02:
                        fprintf(output_file, "%s", armor_table[id]);
                        break;
                    case 0x03:
                        fprintf(output_file, "%s", acc_table[id]);
                        break;
                    case 0x04:
                        fprintf(output_file, "%s", abils_table[id]);
                        break;
                    default:
                        fprintf(output_file, "[PLACEHOLDER]");
                        break;
                }
                
                j += 2;
            }
            else if (dialogue_section[j] == 0x14)
            {
                fprintf(output_file, "\n[OPTIONS]\n");
                j += 2;
            }
            else if (dialogue_section[j] == 0x15)
            {
                strcpy(symbol, is_symbol(dialogue_section[j + 1]));
                fprintf(output_file, "%s", symbol);
                j++;
            }
            else if (dialogue_section[j] == 0x17)
            {
                fprintf(output_file, "\n[PORTRAIT %x %x]\n", dialogue_section[j + 1], dialogue_section[j + 2]);
                j += 2;
            }
            else if (dialogue_section[j] == 0x18)
            {
                fprintf(output_file, "\n");
                j++;
            }
            else if (dialogue_section[j] == 0x07)
            {
                fprintf(output_file, " [PLACEHOLDER] ");
                j++;
            }

            j++;
        }

        fprintf(output_file, "\n==================================================== [%i (0x%04x)]\n", i, pointer);
    }

    fclose(area_file);
    fclose(output_file);
    return 0;
}
