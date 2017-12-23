

import sys
import argparse


class ConverterState:
    none = 0
    enum = 1
    struct = 2
    structInit = 3

parser = argparse.ArgumentParser()
parser.add_argument("file", help = "input C file")

args = parser.parse_args()

state = ConverterState.none

fileName = args.file.split(".");

builtInTypes = ["uint8_t", "uint16_t", "uint32_t", "int8_t", "int16_t", "int32_t", "uint64_t", "protobuf_c_boolean"]

if fileName[0]:
    fileOut = fileName[0]
else:
    fileOut = "." + fileName[1]

def fixName(str):
    return str[1:].strip("\n")

tabMod = 0

with open(fileOut + ".proto", "w") as fo:
    with open(args.file, "r") as f:
        for line in f:
            if line.find("typedef enum") >= 0:
                state = ConverterState.enum
                spline = line.split(" ")
                #print spline[2]
                fo.write("enum " + fixName(spline[2]) + "\n{\n")
                continue
            elif line.find("struct  ") >= 0:
                state = ConverterState.struct
                structName = fixName(line.split("  ")[1])
                #print structName
                structDef = []
                index = 1
                opFiledName = ""
                tabMod = 0
                continue
            
            if (state == ConverterState.enum):
                if line.find("=") >= 0:
                    spline = line.split("__")
                    fo.write("\t" + spline[1].replace(",","").strip().strip("\n") + ";\n")
                elif line.find("}") >= 0:
                    state = ConverterState.none
                    fo.write("}\n\n")
            elif state == ConverterState.structInit:
                if line.find("}") >= 0:
                    spline = line.split(",")
                    #print "init: "
                    #print spline
                    #print structDef
                    index = 0
                    stop = 0
                    for item in spline:
                        if (item.find("__") >= 0):
                            #print item.split("__")
                            #print index
                            structDef[index] = structDef[index].replace(";", " [default = " + item.split("__")[1].replace("}", "").strip("\n").strip() + "];")
                        elif item.find("}") and stop > 0:
                            stop = stop - 1
                        elif item.find("{") >= 0:
                            stop = stop + 1
                        
                        if stop == 0 and item.strip():
                            index = index + 1
                    #print structDef
                    fo.write("message " + structName + "\n{\n")
                    for item in structDef:
                        fo.write("\t" + item + "\n")
                    fo.write("}\n\n")
            elif state == ConverterState.struct:
                if line.find("}") >= 0:
                    state = ConverterState.structInit
                    continue
                
                if line.find("ProtobufCMessage") >= 0 or line.find("{") >= 0:
                    continue
                elif line.find("protobuf_c_boolean") >= 0 and line.find("has_") >= 0:
                    opFiledName = line.split()[1].strip().strip(";")[4:]
                    continue
                elif line.find("size_t") >= 0:
                    opFiledName = line.split()[1].strip().strip(";")[2:]
                    tabMod = 1
                    continue

                if tabMod == 1 and line.find(opFiledName) >= 0:
                    tabMod = 0
                    modifier = "repeated "
                    line = line.replace("*", "")
                else:
                    if opFiledName and line.find(opFiledName) >= 0:
                        modifier = "optional "
                        opFiledName = ""
                    else:
                        modifier = "required "
                
                spline = modifier + line.replace("_t ", " ").replace(";", " = "+str(index)+";").strip().replace("*", "")
                spline = spline.replace("ProtobufCBinaryData", "bytes")
                spline = spline.replace("protobuf_c_boolean", "bool")
                spline = spline.replace("char ", "string ")
                structDef.append(spline)
                index = index + 1
                