#include <cstdint>
#include <winsock.h>

#define SKIP_BYTES(mem, bytes_to_skip) (mem += bytes_to_skip)

int32_t sh_abs(int32_t number)  {
    return (number > 0) ? number : -1*number;
}

uint8_t base_length_extra_bit[] = {
    0, 0, 0, 0, 0, 0, 0, 0, //257 - 264
    1, 1, 1, 1, //265 - 268
    2, 2, 2, 2, //269 - 273 
    3, 3, 3, 3, //274 - 276
    4, 4, 4, 4, //278 - 280
    5, 5, 5, 5, //281 - 284
    0           //285
};

uint32_t base_lengths[] = {
    3, 4, 5, 6, 7, 8, 9, 10, //257 - 264
    11, 13, 15, 17,          //265 - 268
    19, 23, 27, 31,          //269 - 273 
    35, 43, 51, 59,          //274 - 276
    67, 83, 99, 115,         //278 - 280
    131, 163, 195, 227,      //281 - 284
    258                      //285
};

uint32_t dist_bases[] = {
    /*0*/ 1, 2, 3, 4,    //0-3
    /*1*/ 5, 7,          //4-5
    /*2*/ 9, 13,         //6-7
    /*3*/ 17, 25,        //8-9
    /*4*/ 33, 49,        //10-11
    /*5*/ 65, 97,        //12-13
    /*6*/ 129, 193,      //14-15
    /*7*/ 257, 385,      //16-17
    /*8*/ 513, 769,      //18-19
    /*9*/ 1025, 1537,    //20-21
    /*10*/ 2049, 3073,   //22-23
    /*11*/ 4097, 6145,   //24-25
    /*12*/ 8193, 12289,  //26-27
    /*13*/ 16385, 24577, //28-29
             0   , 0      //30-31, error, shouldn't occur
};

uint32_t dist_extra_bits[] = {
    /*0*/ 0, 0, 0, 0, //0-3
    /*1*/ 1, 1,       //4-5
    /*2*/ 2, 2,       //6-7
    /*3*/ 3, 3,       //8-9
    /*4*/ 4, 4,       //10-11
    /*5*/ 5, 5,       //12-13
    /*6*/ 6, 6,       //14-15
    /*7*/ 7, 7,       //16-17
    /*8*/ 8, 8,       //18-19
    /*9*/ 9, 9,       //20-21
    /*10*/ 10, 10,    //22-23
    /*11*/ 11, 11,    //24-25
    /*12*/ 12, 12,    //26-27
    /*13*/ 13, 13,     //28-29
            0 , 0      //30-31 error, they shouldn't occur
};

struct sh_png_chunk {
    uint32_t data_length;
    uint8_t type[4];
    uint8_t *data;
    uint32_t crc32;
};

struct sh_zlib_block {
    uint8_t cmf;
    uint8_t extra_flags;
    uint8_t *data;
    uint16_t check_value;
};

struct sh_png_bit_stream {
    uint8_t *data_stream;
    uint32_t bit_buffer;
    uint32_t bits_remaining;
};

enum sh_png_filters {
    sh_no_filter,
    sh_sub_filter,
    sh_up_filter,
    sh_avg_filter,
    sh_paeth_filter
};

uint8_t* sh_memalloc(uint32_t bytes_to_allocate) {
    return (uint8_t *) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, bytes_to_allocate);
}

uint8_t sh_memfree(uint8_t *mem_pointer) {
    return HeapFree(GetProcessHeap(), 0, (LPVOID) mem_pointer);        
}

void sh_memcpy(uint8_t *from, uint8_t *to, uint32_t bytes_to_copy) { //copy some bytes from "from" to "to" 
    while(bytes_to_copy-- > 0) {
        *(to + bytes_to_copy) = *(from + bytes_to_copy);
    }
}

void sh_memset(uint8_t *mem, uint8_t value_to_use, uint32_t bytes_to_set) {
    while(bytes_to_set-- > 0) {
        *mem++ = value_to_use;
    } 
}

uint16_t sh_get_uint16be(uint8_t *mem) {
    uint16_t result = 0;
    for(uint32_t i = 0; i < 2; ++i) {
        result <<= 8;
        result |= *(mem + i);
    }

    return result;
}

uint32_t sh_get_uint32be(uint8_t *mem) {
    uint32_t result = 0;
    for(uint32_t i = 0; i < 4; ++i) {
        result <<= 8;
        result |= *(mem + i);
    }

    return result;
}

uint8_t* sh_read_file(LPCWSTR file_name) {
    uint8_t *result = NULL;
    HANDLE file = CreateFile(
            file_name,
            GENERIC_READ,
            FILE_SHARE_READ,
            0,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            0
            );

    DWORD size = GetFileSize(file, 0);

    result = sh_memalloc(size);
    ReadFile(file, (void *) result, size, 0, 0);
    CloseHandle(file);
    return result;
}

sh_png_chunk sh_png_read_chunk(uint8_t *mem) {
    sh_png_chunk chunk = {};
    chunk.data_length = sh_get_uint32be(mem);
    SKIP_BYTES(mem, 4); //we move 4 bytes over because we read the length, which is 4 bytes

    *( (uint32_t *)&chunk.type) = *((uint32_t *) mem);
    SKIP_BYTES(mem, 4);

    chunk.data = sh_memalloc(chunk.data_length);

    sh_memcpy(mem, chunk.data, chunk.data_length);
    SKIP_BYTES(mem, chunk.data_length);

    chunk.crc32 = sh_get_uint32be(mem);

    return chunk;
}

sh_zlib_block sh_read_zlib_block(uint8_t *mem, uint32_t length) {
    sh_zlib_block zlib_block = {};
    zlib_block.cmf = *mem;
    SKIP_BYTES(mem, 1);

    zlib_block.extra_flags = *mem;
    SKIP_BYTES(mem, 1);

    //Length is the sum of all the data, we consumed two bytes, and the last two bytes are for the check value
    zlib_block.data = sh_memalloc(length - 2 - 2); //2 for cmf, and flag, 2 for check value

    //Remember we already skipped 2 bytes pointer wise, they were for the cmf and flag bytes
    sh_memcpy(mem, zlib_block.data, length - 2); 
    SKIP_BYTES(mem, length - 2);

    zlib_block.check_value = sh_get_uint16be(mem);

    return zlib_block;
}

void sh_png_get_bits(sh_png_bit_stream *bits, uint32_t bits_required) {
    //this is an extremely stupid way to make sure the unsigned integer doesn't underflow, this is just a replacement for abs() but on unsigned integers.
    uint32_t extra_bits_needed = (bits->bits_remaining > bits_required) ? (bits->bits_remaining - bits_required) : (bits_required - bits->bits_remaining);
    uint32_t bytes_to_read = extra_bits_needed/8;

    //because the above is integer division, there is a possiblity of bits to be remaining, i.e: imagine extra_bits_needed is 14, if you do integer division by 8, you get 1, but an extra 6 bits remain 
    if(extra_bits_needed%8) { //do we have any remaining bits?
        bytes_to_read++; //if we do have extra bits they won't be more than 8 bits, so we will add one extra byte for those bits and we are good to go
    }

    for(uint32_t i = 0; i < bytes_to_read; ++i) {
        uint32_t byte = *bits->data_stream++;
        bits->bit_buffer |= byte << (i*8 + bits->bits_remaining); //we need to becareful to not overwrite the remaining bits if any
    }

    bits->bits_remaining += bytes_to_read*8;
}

uint32_t sh_png_read_bits(sh_png_bit_stream *bits, uint32_t bits_to_read) {
    uint32_t result = 0;

    if(bits_to_read > bits->bits_remaining) {
        sh_png_get_bits(bits, bits_to_read);
    }

    for(uint32_t i = 0; i < bits_to_read; ++i) {
        uint32_t bit = bits->bit_buffer & (1 << i);
        result |= bit;
    }

    bits->bit_buffer >>= bits_to_read;
    bits->bits_remaining -= bits_to_read;

    return result;
}

uint32_t sh_peak_bits_reverse(sh_png_bit_stream *bits, uint32_t bits_to_peak) {
    if(bits_to_peak > bits->bits_remaining) {
        sh_png_get_bits(bits, bits_to_peak);
    }

    uint32_t result = 0; //this could potentially cause problems, 
    for(uint32_t i = 0; i < bits_to_peak; ++i) {
        result <<= 1;
        uint32_t bit = bits->bit_buffer & (1 << i);
        result |= (bit > 0) ? 1 : 0;
    }

    return result;
}

int32_t sh_png_paeth_predict(int32_t a, int32_t b, int32_t c) { 
    int32_t p = a + b - c;
    int32_t pa = sh_abs(p - a);
    int32_t pb = sh_abs(p - b);
    int32_t pc = sh_abs(p - c);

    if(pa <= pb && pa <= pc) return a;
    if(pb <= pc) return b;
    return c;
}

uint32_t sh_decode_huffman(sh_png_bit_stream *bits, uint32_t *assigned_codes, uint8_t *code_bit_lengths, uint32_t assigned_code_length) {
    for(uint32_t i = 0; i < assigned_code_length; ++i) {
        if(code_bit_lengths[i] == 0) continue;
        uint32_t code = sh_peak_bits_reverse(bits, code_bit_lengths[i]);
        if(assigned_codes[i] == code) {
            bits->bit_buffer >>= code_bit_lengths[i];
            bits->bits_remaining -= code_bit_lengths[i];
            return i;
        }
    }

    return 0;
}

uint8_t sh_get_maximum_bit_length(uint8_t *code_bit_lengths, uint32_t len_of_array) {
    uint8_t max_bit_length = 0;
    for(uint32_t i = 0; i < len_of_array; ++i) {
        if(max_bit_length < code_bit_lengths[i]) {
            max_bit_length = code_bit_lengths[i];
        }
    }

    return max_bit_length;
}

void sh_get_bit_length_count(uint32_t *code_count, uint8_t *code_bit_length, uint32_t bit_len_array_len) {
    for(uint32_t i = 0; i < bit_len_array_len; ++i) {
        code_count[code_bit_length[i]]++;
    }
}

void sh_first_code_for_bitlen(uint32_t *first_codes, uint32_t *code_count, uint32_t max_bit_length) {
    uint32_t code = 0;
    for(uint32_t i = 1; i <= max_bit_length; ++i) {
        code = ( code + code_count[i-1]) << 1; 

        if(code_count[i] > 0) {
            first_codes[i] = code;
        }
    }
}

void sh_assign_huffman_code(uint32_t *assigned_codes, uint32_t *first_codes, uint8_t *code_bit_lengths, uint32_t len_assign_code) {
    for(uint32_t i = 0; i < len_assign_code; ++i) {
        if(code_bit_lengths[i]) {
            assigned_codes[i] = first_codes[code_bit_lengths[i]]++;
        }
    }
}

uint32_t* sh_build_huffman_code(uint8_t *code_bit_lengths, uint32_t len_code_bit_lengths) {
    uint32_t max_bit_length = sh_get_maximum_bit_length(code_bit_lengths, len_code_bit_lengths);

    uint32_t *code_counts = (uint32_t *)sh_memalloc(sizeof(uint32_t)*( max_bit_length + 1 ));
    uint32_t *first_codes = (uint32_t *)sh_memalloc(sizeof(uint32_t)*(max_bit_length + 1));
    uint32_t *assigned_codes = (uint32_t *)sh_memalloc(sizeof(uint32_t)*(len_code_bit_lengths));//we have to assign code to every element in the alphabet, even if we have to assign zero

    sh_get_bit_length_count(code_counts,  code_bit_lengths, len_code_bit_lengths);
    code_counts[0] = 0; //in the real world, when a code of the alphabet has zero bit length, it means it doesn't occur in the data thus we have to reset the count for the zero bit length codes to 0.

    sh_first_code_for_bitlen(first_codes, code_counts, max_bit_length);
    sh_assign_huffman_code(assigned_codes, first_codes, code_bit_lengths, len_code_bit_lengths);

    return assigned_codes;
}

uint8_t* sh_zlib_deflate_block(
        sh_png_bit_stream *bits,
        uint32_t *literal_tree, uint8_t *lit_code_bit_len, uint32_t lit_arr_len,
        uint32_t *distance_tree, uint8_t *dist_tree_bit_len, uint32_t dist_arr_len,
        uint32_t *bytes_read)
{
    //1 MB data for the uncompressed block, you can pre allocte a giant memory
    // that you can pass in, the size of the memory would probaly be something like
    // bytes_per_pixel * width * height + height*filter_byte
    //each row has a filter byte(s) in the beginning that you have to account for
    //when you decompress

    uint8_t *decompressed_data = sh_memalloc(1024*1024);
    uint32_t data_index = 0;
    while(true) {
        uint32_t decoded_value = sh_decode_huffman(bits, literal_tree, lit_code_bit_len, lit_arr_len);

        if(decoded_value == 256) break;
        if(decoded_value < 256) { //its a literal so just output it
            decompressed_data[data_index++] = decoded_value;
            continue;
        }

        if(decoded_value < 286 && decoded_value > 256) {
            uint32_t base_index = decoded_value - 257;
            uint32_t duplicate_length = base_lengths[base_index] + sh_png_read_bits(bits, base_length_extra_bit[base_index]);;

            uint32_t distance_index = sh_decode_huffman(bits, distance_tree, dist_tree_bit_len, dist_arr_len);
            uint32_t distance_length = dist_bases[distance_index] + sh_png_read_bits(bits, dist_extra_bits[distance_index]);

            uint32_t back_pointer_index = data_index - distance_length;
            while(duplicate_length--) {
                decompressed_data[data_index++] = decompressed_data[back_pointer_index++];
            }
        }
    }

    *bytes_read = data_index;
    uint8_t *fit_image = sh_memalloc(data_index);
    sh_memcpy(decompressed_data, fit_image, data_index);

    sh_memfree(decompressed_data);

    return fit_image;
}

uint8_t* sh_zlib_decompress(uint8_t *zlib_data, uint32_t *decompressed_size) {
    uint8_t *decompressed_data = sh_memalloc(1024*1024*4); //4 MB free space
    uint32_t data_read = 0;
    uint32_t final;
    uint32_t type;
    sh_png_bit_stream bit_stream = {zlib_data, 0, 0};
    sh_png_bit_stream *bits = &bit_stream;
    do {
        final = sh_png_read_bits(bits, 1);
        type = sh_png_read_bits(bits, 2);

        uint32_t hlit = sh_png_read_bits(bits, 5) + 257;
        uint32_t hdist = sh_png_read_bits(bits, 5) + 1;
        uint32_t hclen = sh_png_read_bits(bits, 4) + 4;

        uint8_t code_lengths_of_code_length_order[] = {16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15};
        uint8_t code_length_of_code_length[19]; //maximum alphabet symbol is 18
        sh_memset(code_length_of_code_length, 0, 19);

        for(uint8_t i = 0; i < hclen; ++i) {
            code_length_of_code_length[code_lengths_of_code_length_order[i]] = sh_png_read_bits(bits, 3);
        }

        uint32_t *huffman_codes_of_tree_of_trees = sh_build_huffman_code(code_length_of_code_length, 19);
        uint8_t *two_trees_code_bit_lengths = sh_memalloc(hlit + hdist);

        //because we have repetition, we won't necessarly have the exact bit lengths for each symbol if we just loop one increment at a time
        uint32_t code_index = 0;
        while(code_index < (hdist+hlit)) {
            uint32_t decoded_value = sh_decode_huffman(bits, huffman_codes_of_tree_of_trees, code_length_of_code_length, 19);
            if(decoded_value < 16) {
                two_trees_code_bit_lengths[code_index++] = decoded_value;
                continue;
            }

            uint32_t repeat_count = 0;
            uint8_t code_length_to_repeat = 0; 

            switch(decoded_value) {
                case 16:
                    repeat_count = sh_png_read_bits(bits, 2) + 3;// 3 - 6 repeat count
                    code_length_to_repeat = two_trees_code_bit_lengths[code_index - 1];
                    break;
                case 17:
                    repeat_count = sh_png_read_bits(bits, 3) + 3;// 3 - 10 repeat count
                    break;
                case 18:
                    repeat_count = sh_png_read_bits(bits, 7) + 11;// 3 - 10 repeat count
                    break;
            }

            sh_memset(two_trees_code_bit_lengths + code_index, code_length_to_repeat, repeat_count);
            code_index += repeat_count;
        }

        uint32_t *literal_length_huff_tree = sh_build_huffman_code(two_trees_code_bit_lengths, hlit);
        uint32_t *distance_huff_tree = sh_build_huffman_code(two_trees_code_bit_lengths + hlit, hdist);
        uint32_t block_size = 0;
        uint8_t *decompressed_block = sh_zlib_deflate_block(
                bits,
                literal_length_huff_tree, two_trees_code_bit_lengths, hlit,
                distance_huff_tree, two_trees_code_bit_lengths + hlit, hdist,
                &block_size);
        
        sh_memcpy(decompressed_block, decompressed_data + data_read, block_size);
        data_read += block_size;
        sh_memfree(decompressed_block);

    } while(!final);

    *decompressed_size = data_read;

    return decompressed_data;
}

uint8_t* sh_png_defilter(uint8_t *decompressed_image, uint32_t size, sh_png_chunk *ihdr) {
    uint32_t x = sh_get_uint32be(ihdr->data);
    uint32_t y = sh_get_uint32be(ihdr->data+4);
    uint8_t bit_depth = *( ihdr->data + 4 + 4 );//count the bytes out
    uint8_t byte_per_pixel = 1;//this is usually determined by checking color type, the picture I'm using is only greyscale, its only one byte per pixel

    uint8_t *row = decompressed_image;
    uint32_t stride = x*byte_per_pixel;

    uint8_t *image = sh_memalloc(x*y*byte_per_pixel); //this is even smaller than the filter but just being safe
    uint8_t *working = image;
    for(uint32_t i = 0; i < y; ++i) {
        working = image + i*stride;
        uint8_t filter = *row++;

        switch(filter) {
            case sh_no_filter: {
                for(uint32_t j = 0; j < x; ++j) {
                    working[j] = row[j];
                }
            } break;
            case sh_sub_filter: {
                for(uint32_t j = 0; j < x; ++j) {
                    uint8_t a = 0;
                    if(j != 0) {
                        a = working[j-1];
                    }
                    uint8_t value = row[j] + a;
                    working[j] = value;
                }
            } break;
            case sh_up_filter: {
                uint8_t *prev_row = working - stride;
                for(uint32_t j = 0; j < x; ++j) {
                    uint8_t b = prev_row[j];
                    uint8_t value = row[j] + b;
                    working[j] = value;
                }
           } break;
            case sh_avg_filter: {
                uint8_t *prev_row = working - stride;
                for(uint32_t j = 0; j < x; ++j) {
                    uint8_t a = 0;
                    uint8_t b = prev_row[j];
                    if(j) {
                        a = working[j - 1];
                    }

                    uint8_t value = row[j] + ( (a + b) >> 1 );
                    working[j] = value;
                }
            } break;
            case sh_paeth_filter: {
                uint8_t *prev_row = working - stride;
                for(uint32_t j = 0; j < x; ++j) {
                    uint8_t a = 0;
                    uint8_t b = prev_row[j];
                    uint8_t c = 0;
                    if(j) { 
                        a = working[j - 1];
                        c = prev_row[j - 1];
                    }

                    uint8_t value = row[j] + sh_png_paeth_predict((int32_t) a, (int32_t) b, (int32_t) c);
                    working[j] = value;
                }
            } break;
        }

        row += stride;
    }

    return image;
}

void parsetest(LPCWSTR path) {
    uint8_t *mem = sh_read_file(path);
    SKIP_BYTES(mem, 8); //skip signature, you can read it and check for stuff

    sh_png_chunk chunks[3];
    
    uint8_t *for_reading = mem; //we don't want to lose the pointer of the file
    for(uint8_t i = 0; i < 3; ++i) {
        chunks[i] = sh_png_read_chunk(for_reading);
        SKIP_BYTES(for_reading, 4 + 4 + chunks[i].data_length + 4); //Length bytes, type, actual data of the chunk, crc value 
    }
    //First chunk is IHDR, 2nd is one IDATA chunk, last one is IEND
    sh_zlib_block zlib_block = sh_read_zlib_block(chunks[1].data, chunks[1].data_length);

    uint32_t decompressed_data_size = 0;
    uint8_t *decompressed_png = sh_zlib_decompress(zlib_block.data, &decompressed_data_size);

    uint8_t *image = sh_png_defilter(decompressed_png, decompressed_data_size, &chunks[0]);
}