cmake_minimum_required(VERSION 3.10)

project(zlib)

add_executable(zlib_compress src/main.c src/compress.c src/hash_table.c src/huffman_tree.c src/inflate.c src/lz77.c src/priority_minheap.c src/binary_IO.c)

add_executable(zlib_decompress src/decompressor.c src/decompress_main.c)
include_directories(./includes)




