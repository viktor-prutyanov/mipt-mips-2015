/**
 * func_memory.cpp - the module implementing the concept of
 * programer-visible memory space accesing via memory address.
 * @author Alexander Titov <alexander.igorevich.titov@gmail.com>
 * Copyright 2012 uArchSim iLab project
 */

// Generic C
#include <cassert>
#include <cstring>
#include <cstdio>
#include <cstdlib>
// Generic C++

// uArchSim modules
#include <func_memory.h>

#define DUMP_STR_LINE_SIZE 54

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
        mem_bounds.push_back( make_pair( sections_array[i].start_addr, sections_array[i].size));
        for ( uint64 j = 0; j < sections_array[i].size; ++j)
        {
            Addr cur_addr;
            splitAddr( sections_array[i].start_addr + j, &cur_addr);
            mem[cur_addr.setNum][cur_addr.pageNum][cur_addr.offset] = sections_array[i].content[j];
        }
        if ( strcmp( sections_array[i].name, ".text") == 0)
        {
            text_sect_start = sections_array[i].start_addr;
            Addr text = {};
            splitAddr( text_sect_start, &text);
        }
    }
}

FuncMemory::~FuncMemory()
{
    // Nothing to do
}

uint64 FuncMemory::startPC() const
{
    return text_sect_start;
}

uint64 FuncMemory::read( uint64 addr, unsigned short num_of_bytes) const
{
    if ( num_of_bytes == 0)
    {
        abort();
    }

    bool isBoundsErr = true;

    for ( int i = 0; i < mem_bounds.size(); ++i)
    {
        if ( ( mem_bounds[i].first <= addr) && ( mem_bounds[i].second >= num_of_bytes))
        {
            isBoundsErr = false;
            break;
        }
    }

    if ( isBoundsErr)
    {
        abort();
    }

    uint64 value = 0;

    for ( int i = 0; i < num_of_bytes; ++i)
    {
        Addr cur_addr;
        splitAddr( addr + i, &cur_addr);

        value |= ( ( uint64)( mem[cur_addr.setNum][cur_addr.pageNum][cur_addr.offset]) << ( i * 8));
    }

    return value;
}

void FuncMemory::write( uint64 value, uint64 addr, unsigned short num_of_bytes)
{
    if ( num_of_bytes == 0)
    {
        abort();
    }

    for ( int i = 0; i < num_of_bytes; ++i)
    {   
        Addr cur_addr;
        splitAddr( addr + i, &cur_addr);

        mem[cur_addr.setNum][cur_addr.pageNum][cur_addr.offset] = ( value >> ( i * 8)) & 0xFF;
    }

    mem_bounds.push_back( make_pair( addr, num_of_bytes));
}

string FuncMemory::dump( string indent) const
{
    char *str = new char[DUMP_STR_LINE_SIZE * mem_bounds.size() + 1];
    for ( int i = 0; i < mem_bounds.size(); ++i)
    {
        sprintf( str + DUMP_STR_LINE_SIZE * i, "Bounds %.2d: start_addr = 0x%.8x, size = %10d\n", 
            i, mem_bounds[i].first, mem_bounds[i].second);
    }
    string dump_str = string( str);
    delete[] str;
    return dump_str;
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