# zlib

Implements the compression and decompression of inflate and deflate stream specified by rfc 1951 <br>
It technically is zlib stream that contains zlib header followed by deflate stream (its header and other) followed by adler32 checksum.
CRC check is not done. <br>
In comparison to decompression, compression was hard work for me and took around a week (around 3-4 hrs a day and classes). <br>
Deflate stream is used by most of the popular programs like zlib, gzip, png and tar and might be one of the complex program to write correctly. <br>
I've written a minimum working version that compresses the given file using dynamic huffman coding and run_length coding. <br> 
Other formats of deflate are currently not supported. Actually there are two left, fixed huffman code should be a little work to implement and another one is uncompessed .. A little change in header should be done to accomplish it. <br>
It was a worth writing this program, since I learned about : 
<br> Run length encoding , lz77 and its variants
<br> Entropy and dictionary based encoding 
<br> Working with raw binary data with little help from debug
<br> Writing a moderately large program in C lang
<br> A good knowledge about the most popular compression technique
<br> <br>
This program currently works for any binary data and can re_construct without fail. <br>
Extension are not stored or recovered after compression and decompression and should be manually managed. This might change if I get will to work a little on it. <br>
A large numbers of header and source files are needed. It might not have been needed if I could manage the file properly but I tried to be as modular as I could. Name of headers and source files except some should be self explanatory while some maybe not cause I didn't think of a better name to give to them while I even wasn't sure what I would be writing on them when I created them. 
<br>
A number of things could be changed or made simpler but I didn't go through those stuffs. I read the rfc 1950 & 1951 and went to work. <br>
I might write a Chip-8 emulator next. <br>

<br> 
<br>
TL;DR -> It is still incomplete. It works for certain files only. The huffman code generation procedure need to be modified by some high intellect alogrithm. The naive method doesn't work for larger files.
<br>
But when it works, it can be decompressed by any standard gzip compressor/decompressor.
<br>

