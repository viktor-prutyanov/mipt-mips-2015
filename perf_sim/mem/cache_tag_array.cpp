#include <cassert>

#include <iostream>
#include <algorithm>
#include <vector>
#include <deque>

#include <cache_tag_array.h>

CacheTagArray::CacheTagArray(
    unsigned long int size_in_bytes,
    unsigned int ways,
    unsigned short block_size_in_bytes,
    unsigned short addr_size_in_bits)
   :size_in_bytes ( size_in_bytes),
    ways ( ways),
    block_size_in_bytes ( block_size_in_bytes),
    addr_size_in_bits ( addr_size_in_bits)
{
    offset_width = log2( block_size_in_bytes);
    is_fully = ( ways == 0);
    if ( is_fully)
    {
        tag_array_len = size_in_bytes / block_size_in_bytes; 
        set_width = 0;
        tag_array = std::vector<uint64>(tag_array_len, 0);
        //std::fill( tag_array.begin(), tag_array.begin() + tag_array_len, 0);
        mru_bits = std::vector<bool>(tag_array_len, false);
        //std::fill( mru_bits.begin(), mru_bits.begin() + tag_array_len, false);
        mru_bits_num = 0;
    }
    else
    {
        tag_array_len = size_in_bytes / block_size_in_bytes / ways;
        set_width = log2( tag_array_len);
        tag_arrays = new std::vector<uint64>[ways];
        way_deqs = new std::deque<unsigned int>[tag_array_len];

        for ( unsigned int way = 0; way < ways; ++way)
            for ( uint64 set = 0; set < tag_array_len; ++set)
                way_deqs[set].push_back( way); 
        
        std::fill( tag_arrays, tag_arrays + ways, std::vector<uint64>(tag_array_len));
    }
}

CacheTagArray::~CacheTagArray()
{
    if ( !is_fully)
    {
        delete[] tag_arrays;
        delete[] way_deqs;
    }
}

unsigned int CacheTagArray::log2( unsigned int x) const
{
    assert( x != 0);
    if (x == 1) 
        return 0;
    unsigned int ret = 0;
    while (x > 1) 
    {
        x >>= 1;
        ret++;
    }
    return ret;
}

uint64 CacheTagArray::getTag( uint64 addr) const
{
    return addr >> set_width >> offset_width;
}

uint64 CacheTagArray::getSet( uint64 addr) const
{
    return (addr >> offset_width) & ((1 << set_width) - 1);
}

bool CacheTagArray::read( uint64 addr)
{
    if ( is_fully)
    {
        uint64 tag = getTag( addr);

        for ( size_t i = 0; i < tag_array_len; ++i)
        {
            if ( tag_array[i] == tag)
            {
                if ( mru_bits_num + 1 == tag_array_len) 
                {
                    mru_bits_num = 0;
                    std::fill( mru_bits.begin(), mru_bits.begin() + tag_array_len, false);
                }
                mru_bits[i] = true;
                ++mru_bits_num; 
                return true;
            }
        }

        return false;
    }
    else
    {
        uint64 tag = getTag( addr);
        uint64 set = getSet( addr);

        for ( unsigned int way = 0; way < ways; ++way)
        {
            if ( tag_arrays[way][set] == tag)
            {
                std::deque<unsigned int>::iterator it;
                for ( it = way_deqs[set].begin(); it != way_deqs[set].end(); ++it)
                {
                    if ( *it == way)
                    {
                        way_deqs[set].erase( it);
                        way_deqs[set].push_back( way);
                        break;
                    }
                }
                
                return true;
            }
        }

        return false;
    }
}

void CacheTagArray::write( uint64 addr)
{
    if ( is_fully)
    {
        for ( size_t i = 0; i < tag_array_len; ++i)
        {
            if ( !mru_bits[i])
            {
                tag_array[i] = getTag( addr);
                if ( mru_bits_num + 1 == tag_array_len) 
                {
                    mru_bits_num = 0;
                    std::fill( mru_bits.begin(), mru_bits.begin() + tag_array_len, false);
                }
                mru_bits[i] = true;
                ++mru_bits_num;
                return;
            }
        }
    }
    else
    {
        uint64 tag = getTag( addr);
        uint64 set = getSet( addr);
        
        unsigned int way = way_deqs[set].front();
        way_deqs[set].pop_front();
        way_deqs[set].push_back( way); 
        
        tag_arrays[way][set] = tag;
    }
}
