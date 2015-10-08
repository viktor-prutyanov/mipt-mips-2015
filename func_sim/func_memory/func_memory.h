/**
 * func_memory.h - Header of module implementing the concept of 
 * programer-visible memory space accesing via memory address.
 * @author Alexander Titov <alexander.igorevich.titov@gmail.com>
 * Copyright 2012 uArchSim iLab project
 */

// protection from multi-include
#ifndef FUNC_MEMORY__FUNC_MEMORY_H
#define FUNC_MEMORY__FUNC_MEMORY_H

// Generic C++
#include <string>
#include <cassert>
#include <map>
#include <utility>

// uArchSim modules
#include <types.h>
#include <elf_parser.h>

using namespace std;

struct Addr
{
    uint64 setNum;
    uint64 pageNum;
    uint64 offset;
};

class FuncMemory
{
    // You could not create the object
    // using this default constructor
    FuncMemory(){}

    uint64 getSetNumByAddr( uint64 addr) const;
    uint64 getPageNumByAddr( uint64 addr) const;
    uint64 getOffsetByAddr( uint64 addr) const;
    void splitAddr( uint64 addr, Addr *addrToSplit) const;

    uint64 text_sect_start;

    uint64 addr_bits;
    uint64 set_num_bits;
    uint64 page_num_bits;
    uint64 offset_bits;

    vector< pair< uint64, uint64> > mem_bounds;
    mutable map< uint64, map< uint64, map< uint64, uint8> > > mem;

public:

    FuncMemory ( const char* executable_file_name,
                 uint64 addr_size = 32,
                 uint64 page_num_size = 10,
                 uint64 offset_size = 12);
    
    virtual ~FuncMemory();
    
    uint64 read( uint64 addr, unsigned short num_of_bytes = 4) const;
    void   write( uint64 value, uint64 addr, unsigned short num_of_bytes = 4);
    
    uint64 startPC() const;
    
    string dump( string indent = "") const;
};

#endif // #ifndef FUNC_MEMORY__FUNC_MEMORY_H
