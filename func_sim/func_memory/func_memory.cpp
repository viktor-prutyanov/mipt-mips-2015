/**
 * func_memory.cpp - the module implementing the concept of
 * programer-visible memory space accesing via memory address.
 * @author Alexander Titov <alexander.igorevich.titov@gmail.com>
 * Copyright 2012 uArchSim iLab project
 */

// Generic C
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
// Generic C++

// uArchSim modules
#include <func_memory.h>

FuncMemory::FuncMemory( const char* executable_file_name,
                        uint64 addr_size,
                        uint64 page_bits,
                        uint64 offset_size)
{
    addr_bits = addr_size;
    set_num_bits = addr_size - page_bits - offset_size;
    page_num_bits = page_bits;
    offset_bits = offset_size;

    assert( executable_file_name != NULL);
    vector<ElfSection> sections_array;
    ElfSection::getAllElfSections( executable_file_name, sections_array);

    for ( unsigned int i = 0; i < sections_array.size(); ++i)
    {
        //printf("{ %s\n", sections_array[i].name);
        //printf("  start = %d (0x%x)\n", sections_array[i].start_addr, sections_array[i].start_addr);
        //printf("  size = %d }\n", sections_array[i].size);
        for ( uint64 j = 0; j < sections_array[i].size; ++j)
        {
            Addr cur_addr;
            splitAddr( sections_array[i].start_addr + j, &cur_addr);
            //printf("0x%.8x: 0x%.2x to [%d|%d|%d]\n", j, sections_array[i].content[j], cur_addr.setNum, cur_addr.pageNum, cur_addr.offset);
            mem[cur_addr.setNum][cur_addr.pageNum][cur_addr.offset] = sections_array[i].content[j];
        }
        //printf("\n");
        if ( strcmp( sections_array[i].name, ".text") == 0)
        {
            text_sect_start = sections_array[i].start_addr;
            Addr text = {};
            splitAddr(text_sect_start, &text);
        }
    }
}

FuncMemory::~FuncMemory()
{
    // put your code here
}

uint64 FuncMemory::startPC() const
{
    return text_sect_start;
}

uint64 FuncMemory::read( uint64 addr, unsigned short num_of_bytes) const
{
    if (num_of_bytes == 0)
    {
        abort();
    }
    uint64 value = 0;

    for (int i = 0; i < num_of_bytes; ++i)
    {
        Addr cur_addr;
        splitAddr( addr + i, &cur_addr);

        value |= ( ( uint64)( mem[cur_addr.setNum][cur_addr.pageNum][cur_addr.offset]) << ( i * 8));
        //printf("{0x%x}\n", value);
    }

    //printf("res: 0x%x\n", value);
    return value;
}

void FuncMemory::write( uint64 value, uint64 addr, unsigned short num_of_bytes)
{
    if (num_of_bytes == 0)
    {
        abort();
    }
    
    for ( int i = 0; i < num_of_bytes; ++i)
    {   
        Addr cur_addr;
        splitAddr( addr + i, &cur_addr);

        mem[cur_addr.setNum][cur_addr.pageNum][cur_addr.offset] = ( value >> ( i * 8)) & 0xFF;
    }
}

string FuncMemory::dump( string indent) const
{
    return string("ERROR: You need to implement FuncMemory!");
}

uint64 FuncMemory::getSetNumByAddr( uint64 addr) const
{
    return ( addr >> ( page_num_bits + offset_bits)) 
        & ( ( 1 << set_num_bits) - 1);
}

uint64 FuncMemory::getPageNumByAddr( uint64 addr) const
{
    return ( addr >> offset_bits) & ( ( 1 << page_num_bits) - 1);
}

uint64 FuncMemory::getOffsetByAddr( uint64 addr) const
{
    return  addr & ( ( 1 << offset_bits) - 1);
}

void FuncMemory::splitAddr( uint64 addr, Addr *addrToSplit) const
{
    assert ( addrToSplit != NULL);
    addrToSplit->setNum = getSetNumByAddr( addr);
    addrToSplit->pageNum = getPageNumByAddr( addr);
    addrToSplit->offset = getOffsetByAddr( addr);
}