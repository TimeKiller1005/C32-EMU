/*
 *--------------------------------------------------------------------------
 * Copyright (C) 2013, Joe Mruz
 * Contact email: mruz.joe@gmail.com
 *
 * This file is part of C32-EMU.
 *
 * C32-EMU is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * C32-EMU is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with C32-EMU.  If not, see <http://www.gnu.org/licenses/>.
 *--------------------------------------------------------------------------
 */

/* Assembler */

/* Code cleanup is needed */

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <map>
#include <utility>
#include <vector>
#include <iomanip>
#include <algorithm>

using std::vector;
using std::map;
using std::cout;
using std::endl;
using std::string;

extern "C"
{
	#include "emu_defs.h"
    #include "file_utils.h"
}

#define MAX_FILE_SIZE 		(1024 * 1024) 	// 1 MB
#define ENTRY_POINT_LABEL 	"_ASSEMBLER_GENERATED_ENTRY_POINT_"

/* first 4 bytes = asm generated DCALL, second 4 bytes = asm generated RET*/
#define HEADER_SIZE         (4 + 4)


typedef struct instr_s instr_t;

int read_source_file(string&, vector<string>&, vector<string>&, vector<string>&, vector<string>&, vector<string>&, uint32_t&);
string trim(string& str, const string& whitespace = " \t");
string reduce(string& str,const string& fill = "",const string& whitespace = " \t");
uint32_t create_db(std::string& src, string& constant, std::vector<uint8_t>& data_section);
uint8_t opcode_a2i (string& opcode);
uint8_t register_a2i (string& reg);
void parse_20bit(instr_t &, string &, unsigned long);
void parse_24bit(instr_t &, string &, unsigned long);
unsigned long htol(const char *p);
void instr_cpy(instr_t *ins, uint8_t* buffer, unsigned long adr);
uint32_t pack(instr_t *ins);
instr_t unpack(uint32_t& ins);

unsigned int            error_count = 0;
unsigned int            warning_count = 0;
vector<uint32_t>        reloc_tbl;
map<string, uint32_t>   labels;

int 
main(int argc, char** argv)
{
	string filename;
	bool code_dump_enabled = false;
    bool dasm = false;
	vector<string> includes;
	vector<string> opcode;
	vector<string> dst;
	vector<string> src;
	vector<string> constant;
	vector<instr_t> instructions;
	vector<uint8_t> data_section;
	uint8_t buffer[MAX_FILE_SIZE];
	uint32_t entry_point = 0;

	// Get the input filename from the command line
	if(argc < 2) {
		printf("Usage:\n %s <filename>\n",argv[0]);
		return 0;
	}
	else {
		filename = argv[1];

		if(std::string(argv[argc-1]) == std::string("--dasm")) {
			code_dump_enabled = true;
            dasm = true;
		}
	}
	
	printf("processing...\n");
	
	// read the main file
	read_source_file(filename, 
					 opcode, 
					 dst,
					 src,
					 constant,
					 includes,
					 entry_point);
	
	// put a DCALL to entry point at the beginning of the table
	if(entry_point) { // if an explicit entry point was found and non-zero, add the JMP
		opcode.insert(opcode.begin(),"DCALL");
		dst.insert(dst.begin(),std::string("$") + std::string(ENTRY_POINT_LABEL));
		src.insert(src.begin(),"N/A");
		constant.insert(constant.begin(),"N/A");
        // add corresponding RET
        opcode.insert(opcode.begin()+1,"RET");
        dst.insert(dst.begin()+1,"N/A");
        src.insert(src.begin()+1,"N/A");
        constant.insert(constant.begin()+1,"N/A");
	}
    else {
        error_count ++;
        printf ("Error: \"START\" label missing; no entry point defined\n");
    }
	
	// first pass -> handle data section.
	// then a label map can be created without needing adjustments
	for(unsigned long line = 0; line < dst.size(); line++)
	{
		if( dst[line] == "db" &&
			opcode[line][0] == ':' ) {
			int32_t adr = create_db(src [line], constant [line], data_section);
            
            // add a label entry with address adr + size of code tables
            string name = opcode [line].substr (1);
            labels.insert (std::pair<string, uint32_t>(name, adr + (opcode.size()*4)));
            
            // clear the code entry
            opcode [line] = "NOP";
		}
	}
	
	// second pass, look for label definitions, add to map
	for(unsigned long line = 0; line < dst.size(); line++)
	{
        unsigned int offset = HEADER_SIZE - 8;
		if (opcode[line][0] == ':' ) {
            // add a label entry
            string name = opcode [line].substr (1);
            labels.insert (std::pair<string, uint32_t>(name, line*4+offset));
            
            // clear the code entry
            opcode [line] = "NOP";
		}
	}
	
	// now our data section, and code is ready
	// fourth pass, build instructions
	for(unsigned long line = 0; line < opcode.size(); line++)
	{
		instr_t ins = {0};
		
		ins.opcode = opcode_a2i (opcode [line]);
		
		switch(ins.opcode)
		{
			// zero or no operands
			default:
			case NOP: case RET: case HLD: case HLT:
			case M32: case M16: case M8:  case DRET:
			
				// nothing to do
				
				break;
			// 1 operand -- dst is expected to be a register
			case NEG: case PUSH: case POP: case JR:
            case IN:  case OUT:  case DJR: case DCR: case CR:
			
				ins.dst_src 	= (register_a2i (dst [line]) << 4) & 0xF0;
				
				break;
			// 2 operand -- dst and src expected to be registers
			case ADD: case SUB: case MUL: case DIV:
			case MOD: case AND: case BOR: case XOR:
			case BSL: case BSR: case CMP: case LOAD:
			case MOV: case STORE:         case OPTIMIZED_CPY:
			
				ins.dst_src 	= (register_a2i (dst [line]) << 4) & 0xF0;
				ins.dst_src 	|= register_a2i (src [line]);
				
				break;
			// 2 operand -- dst is a register and src is a 20-bit value (src + constant)
			case SET: case DSET:
				
				ins.dst_src 	= (register_a2i (dst [line]) << 4) & 0xF0;
				parse_20bit (ins, src [line], line);
			
				break;
			// 1 operand -- dst is a 24 bit value (dst + src + constant)
			case CALL: case JMP: case JZ:    case JNZ:
			case JNS:  case JS:  case DCALL: case DJMP:
            case DJZ:  case DJNZ:case DJNS:  case DJS:
            case CZ:   case CNZ: case CNS:   case CS:   
            case DCZ:  case DCNZ:case DCNS:  case DCS:  
			
				parse_24bit (ins, dst [line], line);
				
				break;
		}
		instructions.push_back(ins);
	}
	
	// next step: write to binary
	
	// optional, debug: print out the prepped code
	if(code_dump_enabled)
	{
		printf("\n---ascii---\n");
		for (unsigned long i = 0; i < opcode.size(); i++)
		{
			printf("%ld -  %s %s %s %s\n",i,opcode[i].c_str(),dst[i].c_str(),src[i].c_str(),constant[i].c_str());
		}
		
		printf("\n---bin---\n");
		for (unsigned long i = 0; i < instructions.size(); i++)
		{
			printf("%ld : 0x%x 0x%x 0x%x\n",i+1,instructions[i].opcode,
											  instructions[i].dst_src,
											  instructions[i].constant);
		}
        printf("- : [data section]\n");
		printf("\n");
	}

	memset ((void *) buffer, 0, sizeof (buffer));

	// copy to binary buffer (little endian)
	for (unsigned long i = 0; i < instructions.size(); i++)
	{
		instr_cpy(&instructions[i], buffer, (i*4));
	}
	
	
    // data section
    for (unsigned long i = 0; i < data_section.size(); i++)
    {
    	buffer [i + (instructions.size()*4) + HEADER_SIZE] = (uint8_t) data_section [i];
    }

	if(warning_count > 0)
	{
		printf("Warnings found: %d\n",warning_count);
	}

	if(error_count > 0)
	{
		printf("Errors found: %d\n",error_count);
		return 0;
	}

	printf("writing binary...\n");
	// copy to new buffer of size "count"
	unsigned long total_size = (instructions.size()*4) + data_section.size() + HEADER_SIZE;
	uint8_t out_buffer [total_size];

	for(unsigned long i = 0; i < total_size; i++) {
		out_buffer[i] = buffer[i];
	}
	
	FILE *out_file;
	out_file = fopen("a.out", "wb");
	if(!out_file)
	{
		printf("Error: Could not open out file!\n");
		return 0;
	}
	
	fwrite(&out_buffer, sizeof(out_buffer), sizeof(uint8_t), out_file);
	fclose(out_file);
	printf("Done.\n");
	
    // optional -- disassemble the output file and dump
    if (dasm)
    {
        printf("\nDASM invoked...\n");
        char dasm_buff[total_size];
        file_utils_read (dasm_buff, sizeof(dasm_buff), "a.out");

        for(unsigned long i = 0; i <= sizeof(dasm_buff)-4; i+=4)
        {
        	uint32_t instruction = dasm_buff[i] << 24 | dasm_buff[i+1] << 16 | 
        							dasm_buff[i+2] << 8 | dasm_buff[i+3];
            instr_t is = unpack(instruction);
            
            printf("%ld : 0x%x 0x%x 0x%x\n", i/4, 
                                            is.opcode,
                                            is.dst_src,
                                            is.constant);
        }

        printf("Done.\n");
    }

	return 0;
}


int 
read_source_file(string& filename, 
				 vector<string>& opcode, 
				 vector<string>& dst, 
				 vector<string>& src, 
				 vector<string>& constant, 
				 vector<string>& includes, 
				 unsigned int& entry_point)
{
	std::ifstream fd;
	
	// Do not include the same file more than once.
	// Note: Ordering of included source files is not important.
	if (includes.empty() || find(includes.begin(), includes.end(), filename) == includes.end())
	{
		includes.push_back (filename);
	}
	else
	{
		return 1;
	}

	fd.open(filename.c_str());
	if(!fd.is_open())
	{
		printf("Error: Could not open \"%s\"\n",filename.c_str());
		return -1;
	}
	
	int line_num = -1;
	int code_num = -1;
	while (fd.is_open() && !fd.eof())
	{
		string line;
		getline (fd, line);
		line = trim(line);
		
		line_num ++;
		if(line.empty()) continue;
		if(line[0] == ';') continue;
		code_num ++;
		
		if(line == "start:" || line == "START:")
		{
			// entry point found
			entry_point = 1;
			line = std::string(":") + std::string(ENTRY_POINT_LABEL);
		}

        if(line[0] == '#' && line.find("INCLUDE") != string::npos)
        {
            // get the input file name and recursively load
            int first_quote     = line.find('\"');
            int last_quote      = line.rfind('\"');
            string inc_file = line.substr(first_quote+1,last_quote-first_quote-1);
            read_source_file(inc_file, opcode, dst, src, constant, includes, entry_point);
            continue;
        }
		
		// read the opcode
		string op = line.substr(0, line.find_first_of(" \t"));
		opcode.push_back (op);
		line = line.substr(line.find_first_of(" \t")+1);
        line = trim(line);
        if (! (line[0] == 'd' && line[1] == 'b'))
		    line = reduce(line);
		std::stringstream stream(line);
		std::string word;
		
		unsigned int col = 1;
		while (getline(stream,word,','))
		{
			if (!word.empty() && word[0] == ';')
				break; // ignore the rest as soon as we hit a comment in the line
			switch (col) 
			{
				case 1: dst.push_back(word); break;
				case 2: src.push_back(word); break;
				case 3: constant.push_back(word); break;

                // if there are more than 3 operands, keep appending to operand 3
				default: constant [code_num-1] += " " + word; break; 
			}
			col++;
		}
		if(col == 1) 
		{
			dst.push_back("N/A");
			src.push_back("N/A");
			constant.push_back("N/A");
		}
		else if(col == 2)
		{
			src.push_back("N/A");
			constant.push_back("N/A");
		}
		else if(col == 3)
			constant.push_back("N/A");
	}
	fd.close();
	return 0;
}


uint32_t create_db(std::string& src, string& constant, std::vector<uint8_t>& data_section)
{
    int32_t adr = (uint32_t)data_section.size();
    
    unsigned long num_bytes = (int)atoi(src.c_str());
    
    std::string str_data = "";
    src = trim(src);
    constant = trim(constant);
    int first_quote     = constant.find('\"');
	int last_quote      = constant.rfind('\"');

	// 32-bit integer
	if ((int)std::string::npos == first_quote && 
        (int)std::string::npos == last_quote)
	{
        uint32_t data32 = (uint32_t)atol(constant.c_str());
        data_section.push_back((uint8_t)data32 & 0xFF);
        data_section.push_back((uint8_t)(data32 >> 8) & 0xFF);
        data_section.push_back((uint8_t)(data32 >> 16) & 0xFF);
        data_section.push_back((uint8_t)(data32 >> 24) & 0xFF);
	}
	// string
	else
	{
        str_data = constant.substr(first_quote+1,last_quote-first_quote-1);
        for(unsigned long i = 0; i < num_bytes; i++)
        {
        	data_section.push_back((uint8_t)str_data[i]);
        }

        // keep 4 byte alignment
        unsigned int align = data_section.size() % 4;
        for(unsigned int i = 0; i < align; i++);
            data_section.push_back(0);
    }
    
    return adr + HEADER_SIZE;
}

uint8_t opcode_a2i (string& opcode)
{
	if(opcode == "NOP") 	return NOP;
	if(opcode == "SET") 	return SET;
	if(opcode == "MOV") 	return MOV;
	if(opcode == "LOAD") 	return LOAD;
	if(opcode == "STORE") 	return STORE;
	if(opcode == "ADD") 	return ADD;
	if(opcode == "SUB") 	return SUB;
	if(opcode == "MUL") 	return MUL;
	if(opcode == "DIV") 	return DIV;
	if(opcode == "MOD") 	return MOD;
	if(opcode == "NEG") 	return NEG;
	if(opcode == "AND") 	return AND;
	if(opcode == "BOR") 	return BOR;
	if(opcode == "XOR") 	return XOR;
	if(opcode == "BSL") 	return BSL;
	if(opcode == "BSR") 	return BSR;
	if(opcode == "CMP") 	return CMP;
	if(opcode == "IN") 	    return IN;
	if(opcode == "OUT") 	return OUT;
	if(opcode == "M32") 	return M32;
	if(opcode == "M16") 	return M16;
	if(opcode == "M8") 		return M8;
	if(opcode == "HLD") 	return HLD;
	if(opcode == "HLT") 	return HLT;
	if(opcode == "PUSH") 	return PUSH;
	if(opcode == "POP") 	return POP;
	if(opcode == "CALL") 	return CALL;
	if(opcode == "JMP") 	return JMP;
	if(opcode == "JR") 		return JR;
	if(opcode == "JZ") 		return JZ;
	if(opcode == "JNZ") 	return JNZ;
	if(opcode == "JS") 		return JS;
	if(opcode == "JNS") 	return JNS;
	if(opcode == "RET") 	return RET;
	if(opcode == "OPCPY") 	return OPTIMIZED_CPY;
    if(opcode == "CR")      return CR;
    if(opcode == "CZ")      return CZ;
    if(opcode == "CNZ")     return CNZ;
    if(opcode == "CS")      return CS;
    if(opcode == "CNS")     return CNS;
    if(opcode == "DSET")    return DSET;
    if(opcode == "DCALL")   return DCALL;
    if(opcode == "DJMP")    return DJMP;
    if(opcode == "DJR")     return DJR;
    if(opcode == "DJZ")     return DJZ;
    if(opcode == "DJNZ")    return DJNZ;
    if(opcode == "DJS")     return DJS;
    if(opcode == "DJNS")    return DJNS;
    if(opcode == "DRET")    return DRET;
    if(opcode == "DCR")     return DCR;
    if(opcode == "DCZ")     return DCZ;
    if(opcode == "DCNZ")    return DCNZ;
    if(opcode == "DCS")     return DCS;
    if(opcode == "DCNS")    return DCNS;
	
	error_count ++;
	printf("Error: unknown opcode \"%s\"\n", opcode.c_str());
	return NOP;
}


uint8_t register_a2i (string& reg)
{
	if(reg == "%A")			return REGISTER_A;
	if(reg == "%B")			return REGISTER_B;
	if(reg == "%C")			return REGISTER_C;
    if(reg == "%D")         return REGISTER_D;
    if(reg == "%W")         return REGISTER_W;
	if(reg == "%X")			return REGISTER_X;
	if(reg == "%Y")			return REGISTER_Y;
	if(reg == "%Z")			return REGISTER_Z;
    if(reg == "%RTA")       return REGISTER_RTA;
	if(reg == "%CFLAGS")	return REGISTER_CFLAGS;
	if(reg == "%PORT")		return REGISTER_PORT;
	if(reg == "%OFFSET")	return REGISTER_OFFSET;
	if(reg == "%SB")		return REGISTER_STACKBASE;
	if(reg == "%SP")		return REGISTER_STACKPOINTER;
	if(reg == "%PC")		return REGISTER_PC;
	if(reg == "%ZERO")		return REGISTER_ZERO;
	
	error_count ++;
	printf("Error: unknown register \"%s\"\n", reg.c_str());
	return REGISTER_ZERO;
}

void parse_20bit(instr_t &ins, string &src, unsigned long line)
{
	// check for $ or [...] --> lookup in label map, replace with found address
	// else assume a literal value
	// if symbol not found = error
	// split value to src | constant
	
	// make sure to |= to dst_src field
    
    uint32_t adr = 0;

    if (src.find_first_of("$[]") == string::npos) 
    {
        adr = (uint32_t) htol (src.c_str());
    }
    else if (src.find_first_of("$") != string::npos) 
    {
        string name = trim (src, "$");
        
        // lookup in map
        adr = labels [name];
        
        if(!adr)
        {
            error_count ++;
            printf ("Error: unknown label \"%s\"\n", name.c_str());
            return;
        }
    }
    else
    {
        string address = trim(src, "[]");
        adr = (uint32_t)  htol (src.c_str());
    }
	
	uint8_t high4 	= (uint8_t) (adr >> 16) & 0xF;
	uint16_t low16 	= (uint16_t) (adr & 0xFFFF);
	
    ins.dst_src |= high4;
    ins.constant = low16;
}

void parse_24bit(instr_t &ins, string &dst, unsigned long line)
{
	// check for $ or [...] --> lookup in label map, replace with found address
	// else assume a literal value
	// if symbol not found = error
	// split value to dst_src | constant
	
	uint32_t adr = 0;

    if (dst.find_first_of("$[]") == string::npos) 
    {
        adr = (uint32_t) htol (dst.c_str());
    }
    else if (dst.find_first_of("$") != string::npos) 
    {
        string name = trim(dst, "$");
        
        // lookup in map
        adr = labels [name];
        
        if(!adr)
        {
            error_count ++;
            printf ("Error: unknown label \"%s\"\n", name.c_str());
            return;
        }
    }
    else
    {
        string address = trim(dst, "[]");
        adr = (uint32_t) htol (address.c_str());
    }
	
	uint8_t high8 	= (uint8_t) (adr >> 16) & 0xFF;
	uint16_t low16 	= (uint16_t) (adr & 0xFFFF);
	
    ins.dst_src 	= high8;
    ins.constant 	= low16;
}

string trim(string& str, const string& whitespace )
{
    int strBegin = str.find_first_not_of(whitespace);
    if (strBegin == (int)std::string::npos)
        return ""; // no content

    int strEnd = str.find_last_not_of(whitespace);
    int strRange = strEnd - strBegin + 1;

    return str.substr(strBegin, strRange);
}


string reduce(string& str, const string& fill, const string& whitespace)
{
    // trim first
    std::string result = trim(str, whitespace);

    // replace sub ranges
    int beginSpace = result.find_first_of(whitespace);
    while (beginSpace != (int)std::string::npos)
    {
        const int endSpace = result.find_first_not_of(whitespace, beginSpace);
        const int range = endSpace - beginSpace;

        result.replace(beginSpace, range, fill);

        const int newStart = beginSpace + fill.length();
        beginSpace = result.find_first_of(whitespace, newStart);
    }

    return result;
}

unsigned long
htol(const char *p)
{
        /*
         * Look for 'x' as second character as in '0x' format
         */
        if ((p[1] == 'x')  || (p[1] == 'X'))
                return(strtol(&p[2], (char **)0, 16));
        else
                return(strtol(p, (char **)0, 16));
}

// little endian
void
instr_cpy(instr_t *ins, uint8_t* buffer, unsigned long adr)
{	
	uint32_t instruction = pack(ins);
	
	buffer[adr]   = (uint8_t)(instruction) & 0xFF;
	buffer[adr+1] = (uint8_t)((instruction) >> 8) & 0xFF;
	buffer[adr+2] = (uint8_t)((instruction) >> 16) & 0xFF;
	buffer[adr+3] = (uint8_t)((instruction) >> 24) & 0xFF;
}

uint32_t
pack(instr_t *ins)
{
	uint32_t instruction;
	instruction = ((ins->opcode << 24) & 0xFF000000 ) | 
				  ((ins->dst_src << 16) & 0x00FF0000) | 
				  ((ins->constant) & 0x0000FFFF);
	return instruction;
}

instr_t
unpack(uint32_t& ins)
{
	instr_t instruction;
	instruction.opcode = (ins >> 24) & 0xFF;
	instruction.dst_src = (ins >> 16) & 0xff;
	instruction.constant = (ins & 0xFFFF);
	return instruction;
}
