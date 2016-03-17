#ifndef CACHE_TAG_ARRAY_H
#define CACHE_TAG_ARRAY_H

#include <vector>
#include <deque>

#include <types.h>

class CacheTagArray
{
public:
    CacheTagArray( 
        unsigned long int size_in_bytes,
        unsigned int ways,
        unsigned short block_size_in_bytes, 
        unsigned short addr_size_in_bits);
    ~CacheTagArray();

    bool read( uint64 addr); 
    void write( uint64 addr);

private: 
    unsigned int size_in_bytes;
    unsigned int ways;
    unsigned short block_size_in_bytes;
    unsigned short addr_size_in_bits;
    
    bool is_fully;
    unsigned int tag_array_len;
    uint64 offset_width;
    uint64 set_width;
   
    unsigned int log2( unsigned int x) const;
    uint64 getTag( uint64 addr) const;
    uint64 getSet( uint64 addr) const;

    std::vector<uint64>* tag_arrays;
    std::deque<unsigned int>* way_deqs;
    std::vector<uint64> tag_array; //only for fully-associative cache
    std::vector<bool> mru_bits; //only for fiully-associative cache
    uint64 mru_bits_num; 
};

#endif //CACHE_TAG_ARRAY_H
