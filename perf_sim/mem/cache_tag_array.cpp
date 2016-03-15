#include <cassert>

#include <vector>
#include <deque>

#include <cache_tag_array.h>

CacheTagArray::CacheTagArray(
    unsigned int size_in_bytes,
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
    }
}

CacheTagArray::~CacheTagArray()
{
    delete[] way_deqs;
    delete[] tag_arrays;
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

    }
    else
    {
        uint64 tag = getTag( addr);
        uint64 set = getSet( addr);

        for (unsigned int way = 0; way < ways; ++way)
        {
            if (tag_arrays[way][set] == tag)
            {
                std::deque<unsigned int>::iterator it;
                for ( it = way_deqs[set].begin(); it != way_deqs[set].end(); ++it)
                {
                    if ( *it == way)
                    {
                        way_deqs[set].erase( it);
                        way_deqs[set].push_front( way);
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

    }
    else
    {
        uint64 tag = getTag( addr);
        uint64 set = getSet( addr);
        
        unsigned int way = way_deqs[set][0];
        way_deqs[set].pop_back();
        way_deqs[set].push_front( way); 
        
        tag_arrays[way][set] = tag;
    }
}
