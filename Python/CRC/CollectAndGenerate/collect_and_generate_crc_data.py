####################################################################################################
# Import
####################################################################################################

import getopt
import selenium.webdriver
import selenium.webdriver.common.by
import sys

####################################################################################################
# Function
####################################################################################################

def collect_crc_data(bit_width):
    # Driver
    driver = selenium.webdriver.Chrome()

    # Set Up
    if bit_width == 8:
        url = "https://reveng.sourceforge.io/crc-catalogue/1-15.htm#crc.cat-bits.8"
    elif bit_width == 16:
        url = "https://reveng.sourceforge.io/crc-catalogue/16.htm#crc.cat-bits.16"
    elif bit_width == 32:
        url = "https://reveng.sourceforge.io/crc-catalogue/17plus.htm#crc.cat-bits.32"
    else:
        return

    # Collect (https://reveng.sourceforge.io/crc-catalogue/)
    driver.get(url)
    page = driver.find_element(selenium.webdriver.common.by.By.XPATH, "/html/body").text.split("\n")
    crc_list = []
    for i, line in enumerate(page):
        if line.find(f"width={bit_width}") != -1:
            crc = {}
            key_value_pair_list = line.split(" ")
            for key_value_pair in key_value_pair_list:
                key, value = key_value_pair.split("=")
                if value.find("0x") != -1:
                    crc[f"{key}"] = f"0x{value[2:].upper()}"
                else:
                    crc[f"{key}"] = value
            if page[i+2].find("Alias: ") != -1:
                alias_list = []
                for alias in page[i+2][len("Alias: "):].split(", "):
                    alias_list.append(f"\"{alias}\"")
                crc["alias"] = alias_list
            else:
                crc["alias"] = []
            crc_list.append(crc)

    # Collect (https://crccalc.com/)
    for crc in crc_list:
        argument = crc["name"].replace("\"", "")
        url = f"https://crccalc.com/?crc=123456789&method={argument}&datatype=ascii&outtype=hex"
        driver.get(url)
        page = driver.find_element(selenium.webdriver.common.by.By.XPATH, "/html/body").text
        start = page.find("Comma delimiters\n") + len("Comma delimiters\n")
        end = page.find("Share your result:\n")
        lookup_table = page[start:end].replace(",", " ").replace("\n", " ").split()
        crc["lookup_table"] = lookup_table

    # Driver
    driver.quit()

    return sorted(crc_list, key=lambda item: item["name"])

def display_usage():
    print(f"Usage: {sys.argv[0]} [OPTION]", flush=True)
    print("  -b[8,16,32,64], --bit=[8,16,32,64]", flush=True)
    print("  -h, --help", flush=True)

def generate_crc_data(crc_list, bit_width):
    # Header Template
    with open(f"Template/crc{bit_width}.h", "r") as file:
        template = file.read()

    # Replacement
    replacement = 0
    for crc in crc_list:
        replacement = max(replacement, len(crc["alias"]))
    header = template.replace("##### REPLACEMENT #####", f"{replacement}")

    # Header
    with open(f"../../../C/Eclectic/crc{bit_width}.h", "w") as file:
        file.write(header)

    # Source Template
    with open(f"Template/crc{bit_width}.c", "r") as file:
        template = file.read()

    # Replacement
    replacement = ""
    for crc in crc_list:
        replacement += f"    {get_crc_data_string(bit_width, crc, False)},\n"
    replacement = replacement[:-2] # Remove Last ",\n"
    source = template.replace("##### REPLACEMENT #####", replacement)

    # Source
    with open(f"../../../C/Eclectic/crc{bit_width}.c", "w") as file:
        file.write(source)

    # Test Template
    with open(f"Template/test_crc{bit_width}.c", "r") as file:
        template = file.read()

    # Replacement 1
    crc_data_list = []
    string = ""
    for crc in crc_list:
        crc_data_list.append(f"{crc['name']}")
        for alias in crc["alias"]:
            crc_data_list.append(f"{alias}")
    crc_data_list.sort()
    string += f"{{{', '.join(crc_data_list)}}}"
    template = template.replace("##### REPLACEMENT 1 #####", string)

    # Replacement 2
    if bit_width == 8:
        check = crc_list[0]["check"]
    elif bit_width in [16, 32]:
        check = split_hex_string(crc_list[0]["check"])
        if crc_list[0]["refout"] == "true":
            check.reverse()
            check = ", ".join(check)
        else:
            check = ", ".join(check)
    else:
        return
    template = template.replace("##### REPLACEMENT 2 #####", check)

    # Replacement 3
    template = template.replace("##### REPLACEMENT 3 #####", crc_list[0]["name"])

    # Replacement 4
    template = template.replace("##### REPLACEMENT 4 #####", get_crc_data_string(bit_width, crc_list[0], False))

    # Replacement 5
    crc_data_list = []
    indentation = " " * 8
    for crc in crc_list:
        crc_data_list.append(f"{indentation}{{{crc['name']}, {get_crc_data_string(bit_width, crc, False)}}}")
        for alias in crc["alias"]:
            crc_data_list.append(f"{indentation}{{{alias}, {get_crc_data_string(bit_width, crc, False)}}}")
    crc_data_list.sort()
    string = ",\n".join(crc_data_list)
    template = template.replace("##### REPLACEMENT 5 #####", string)

    # Replacement 6
    crc_data_list = []
    indentation = " " * 8
    for crc in crc_list:
        crc_data_list.append(f"{indentation}{{{crc['name']}, {get_crc_data_string(bit_width, crc, True)}}}")
        for alias in crc["alias"]:
            crc_data_list.append(f"{indentation}{{{alias}, {get_crc_data_string(bit_width, crc, True)}}}")
    crc_data_list.sort()
    string = ",\n".join(crc_data_list)
    test = template.replace("##### REPLACEMENT 6 #####", string)

    # Test
    with open(f"../../../C/Ceedling/test_crc{bit_width}.c", "w") as file:
        file.write(test)

def get_crc_data_string(bit_width, crc, lookup_table):
    alias_list = ""
    for alias in crc["alias"]:
        alias_list += f"{alias}, "
    if len(alias_list) > 0:
        alias_list = alias_list[:-2]  # Remove Last ",\n"
    string = ""
    string += f"{{{{{alias_list}}}, "
    string += f"{len(crc['alias'])}, "
    string += f"{crc['check']}, "
    string += f"{crc['init']}, "
    if lookup_table == True:
        string += f"(uint{bit_width}_t []){{{', '.join(crc['lookup_table'])}}}, "
    else:
        string += "NULL, "
    string += f"{crc['name']}, "
    string += f"{crc['poly']}, "
    string += f"{crc['refin']}, "
    string += f"{crc['refout']}, "
    string += f"{crc['residue']}, "
    string += f"{crc['xorout']}}}"

    return string

def split_hex_string(hex_string):
    hex_string = hex_string[2:] # Remove "0x"
    return ["0x" + hex_string[i:i + 2] for i in range(0, len(hex_string), 2)]

####################################################################################################
# Program Start
####################################################################################################

# Options And Arguments
try:
    short_options = "b:h"
    long_options = [
        "bit=",
        "help",
    ]
    opts, args = getopt.getopt(sys.argv[1:], short_options, long_options)
except:
    display_usage()
    sys.exit()

# Default
bit_width = 0

# Parse Arguments
for option, argument in opts:
    if option in ("-b", "--bit"):
        try:
            bit_width = int(argument)
            if bit_width not in [8, 16, 32, 64]:
                raise Exception()
        except:
            print("Error: Bit Width Must Be 8, 16, 32, Or 64", flush=True)
            exit()
    elif option in ("-h", "--help"):
        display_usage()
        sys.exit()

# Error Check
if bit_width == 0:
    print("Error: Bit Width Must Be Specified", flush=True)
    exit()

# Collect
crc_list = collect_crc_data(bit_width)

# Generate
generate_crc_data(crc_list, bit_width)
