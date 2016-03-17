#include <cstdlib>

#include <fstream>
#include <iostream>

#include <cache_tag_array.h>

#define BLOCK_SIZE 4
#define ADDR_SIZE 32

int main( int argc, char** argv)
{
    if ( argc != 3)
    {
        std::cerr << "miss_rate_sim <input memory trace> <output csv table>\n";
        return EXIT_FAILURE;
    }

    std::ofstream out_stream;
    out_stream.open( argv[2]);
    //std::cerr << argv[2] << std::endl;
    out_stream << ",1KB,2KB,4KB,8KB,16KB,32KB,64KB,128KB,256KB,512KB,1024KB\n";

    std::ifstream in_stream;

    for ( unsigned int ways_num = 0; ways_num <= 16; ways_num <<= 1)
    {
        //std::cerr << "ways=" << ways_num << "\n";
        out_stream << ways_num  << " way(s),";

        for ( unsigned long int size = 1024; size <= 1024*1024; size <<= 1)
        {
            //std::cerr << "size=" << size << " ways=" << ways_num << "\n";
            CacheTagArray cacheTagArray( size, ways_num, BLOCK_SIZE, ADDR_SIZE);
            //std::cerr << "ok"; 
            in_stream.open( argv[1]);
            size_t total = 0;
            size_t hits = 0;
            uint32 addr;

            while ( !in_stream.eof())
            {
                in_stream >> std::hex >> addr;
                //std::cerr << "addr=" << addr << "\n";
                if ( cacheTagArray.read( addr))
                    ++hits;
                else
                    cacheTagArray.write( addr);
                ++total;
            }

            in_stream.close();
            out_stream << ( 1. * ( total - hits) / total);
            if (size != 1024*1024)
                out_stream << ",";
            std::cerr << "Size " << size / 1024 << "KB done.\n";
        }

        out_stream << "\n";
        std::cerr << "\t" << ways_num << " way(s) done.\n";
    }

    out_stream.close();

    return 0;
}
