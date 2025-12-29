import sys

if len(sys.argv) != 3:
    print("Usage: python convertFont.py <input.bdf> <output.h>")
    sys.exit(1)

fontWidth = 0
fontHeight = 0

index = 0

glyphs = []

#Initalizes glyphs array with 96 empty lists
for i in range(96):
    glyphs.append([])

currentEnc = None
bitmap = False

with open(sys.argv[1], "r") as f:
    contents = f.readlines()

    for line in contents:
        line = line.strip()

        #Gets fontWidth and fontHeight found in FONTBOUNDINGBOX
        if line.startswith("FONTBOUNDINGBOX"):
            line = line.split()
            fontWidth = int(line[1])
            fontHeight = int(line[2])

        #Finds ENCODING keyword and makes sure its in the ASCII range 32-127 (These are the only ones we need)
        elif line.startswith("ENCODING"):
            line = line.split()
            if int(line[1]) <= 127 and int(line[1]) >= 32:
                currentEnc = int(line[1])
            else:
                currentEnc = None
        
        #When we reach the BITMAP keyword and its an ASCII character we are looking for
        elif line.startswith("BITMAP") and currentEnc != None:
            bitmap = True
            currentRows = []

        #Continues looking under the BITMAP keyword
        elif bitmap:
            if line.startswith("ENDCHAR"): #When we hit ENDCHAR stop looking
                bitmap = False
                
                while len(currentRows) < fontHeight: #Initalizes the currentRows array to match the fontHeight (Number of hex values required in each glyph)
                    currentRows.append(0)

                #Puts all the hex values for the character in the glyphs array
                glyphs[currentEnc - 32] = currentRows #32 offset as we start with ASCII 32

                currentEnc = None
            else:
                currentRows.append(int(line, 16)) #Keeps adding the hex values under BITMAP converted to integers

with open(sys.argv[2], "w") as out:
    out.write("#pragma once\n")
    out.write("#include <stdint.h>\n\n")
    out.write(f"#define FONT_WIDTH {fontWidth}\n")
    out.write(f"#define FONT_HEIGHT {fontHeight}\n\n")
    out.write("static const uint16_t font[96][FONT_HEIGHT] = {\n")

    for i in glyphs:
        out.write(f"\n {{")
        for num in i:
            out.write(f" 0x{int(num):04X},")
    
        out.write(f"}},")
    
    out.write(f"\n}};")


                
    

        
            