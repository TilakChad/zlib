#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#define MAXBITS 15
#define MAX_OUTDATA_SIZE (25 * 1024 * 1024) // Currently 25 MB
struct bit_reader
{
    unsigned int inlen;
    unsigned int cur_len;
    unsigned int outlen;
    unsigned char* data;
    unsigned int buffer;
    unsigned char* outdata;
    short avail_bit;
};

struct huffman
{
    short* count;
    short* code;
};

struct bit_reader bit_read; // Made global for the sake of easeness
int read_bit(int);
int deflate(void);
static int dynamic_huffman();
int construct_cannonical_huffman(int* f_table,struct huffman*,int length);
int decode(struct huffman *);
int decompressor(struct huffman *,struct huffman*);



unsigned char* zlib_stream_decompress(unsigned char *data, int len, int * outlen)
{
    bit_read.inlen = len;
    bit_read.outlen = 0;
    bit_read.cur_len = 0;

    // Here we are going to analyze the zlib format
    // read the first byte which is the CMF flag
    unsigned char CMF = *(data + bit_read.cur_len++);
    unsigned char FLG = *(data + bit_read.cur_len++);
    printf("\n\tCompression method used is : %d.\n", CMF & 0xf);
    printf("\t Compression info is : %d.\n",(CMF & 0xf0)>>4);

    // These are the check specified in the zlib RFC 1950 
    printf("--------------------------------- Flag Info -----------------------------\n");
    printf("\tFCHECK -> Value is -> %d.\n",FLG & 0x1f);
    printf("\tFDICT  -> %d.\n",(FLG & 0x20)>>4);
    printf("\tFLEVEL -> %d.\n",(FLG & 0xc0)>>6);
    printf("\n\t Validiting the FCHECK status .... \n");
    printf("MSB is %d.""\nLSB is %d.""\nProduct -> %d x %d.\n""Result is %d.",CMF,FLG,CMF,FLG,(CMF<<8) + FLG);
    printf("\n-> %d %% 31 -> %d.\n",(CMF<<8) + FLG,((CMF<<8)+FLG)%31);

    bit_read.data = data;
    bit_read.avail_bit = 0;
    // Start the deflate algorithm from now on
    // Allocate enough data to hold the output
    // Output is not written onto the file directly. It is stored in a buffer outdata before writing to the file as a whole.
    bit_read.outdata = malloc(sizeof(unsigned char) * MAX_OUTDATA_SIZE); // 25 MB

    // Lets check whether it works
    deflate();
    
    *outlen = bit_read.outlen;
    return bit_read.outdata;
}

//
int deflate()
{
    // Read the header of the current block speciifed in rfc 1951 deflate 
    int BFINAL = -1;
    short BTYPE = -1;

    do
    {
	BFINAL = read_bit(1);
	BTYPE = read_bit(2);

	assert(BFINAL!=-1);
	if(BFINAL==1)
	    printf("It is the last block of the compressed data... So be careful...");
	else
	    printf("It is not the last block ...");

	switch (BTYPE)
	{
	case 0 : printf("\nThis is uncompressed block....Enjoy..\n"); // Not implemented .. cause its too easy
	    break;
	case 1 : printf("\nIt is compressed with fixed Huffman Codes..."); // Not implemented .. Not interested
	    break;
	case 2 : printf("\nThis block is compressed with dynamic Huffman codes...Trynna break it .. ");
	    dynamic_huffman();
	    break;
	case 3 : printf("\nThis is reserved block .. So you might have some error there... ");
	    break;
	default : printf("What an idiot...");
	    break;
	}
    } while(BFINAL!=1);

    return 0;
}
	

int read_bit(int required)
{
    // read_bits in the reversed ordering
    // when required bits are greater than available bits. Next byte is appended infront of the available bits. buffer[1] << (avail_bits) | buffer[0] 
    int out_bit = 0;
    if(bit_read.cur_len > bit_read.inlen)
    {
	assert(!"cur_len exceeded total input length");
	return -1;
    }

    // Read new bits into the buffer if available bits are less than required bits
    
    while(required > bit_read.avail_bit)
    {
	
	bit_read.buffer |= *(bit_read.data + bit_read.cur_len++) << bit_read.avail_bit;
	bit_read.avail_bit += 8;
    }
    
    out_bit = bit_read.buffer & (( 1 << required ) - 1); // (1 << required) -1 -> Bitwise & with this value takes the last required bits e.g. 1 << 8 -> 0x100 - 1 = 0xFF
    
    bit_read.buffer >>= required;

    bit_read.avail_bit -= required;
    return out_bit;
}

int dynamic_huffman(void)
{
    // Read all the frequencies of the symbol used
    short hlit = read_bit(5) + 257;
    short hdist = read_bit(5) + 1;
    short hclen = read_bit(4) + 4;
    
    // It is the order of run length's code-length specified in rfc (Folks say this order is determined in order of decreasing probability)
    short order[] = {16, 17, 18, 0 , 8, 7 , 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15};

    #ifdef DEBUG
    printf("******************************************** INFO ********************************");
    printf("\nhlit -> %d.",hlit);
    printf("\nhdist -> %d.\nhclen -> %d.\n",hdist,hclen);
    #endif
    
    int run_length_count[19] = {0};
    for(int index = 0; index < hclen; ++index)
    {
	run_length_count[order[index]] = read_bit(3);
    }
    struct huffman lencode,distcode;
    
    // List all the frequencies of the symbols
#ifdef DEBUG
    printf("Code length of run length read were : \n");
    for (int i = 0; i < sizeof(order)/sizeof(short); ++i)
	printf("\n [%d] -> %d.",i,run_length_count[i]);
#endif
    
    construct_cannonical_huffman(run_length_count,&lencode,19);


    int all_codes[hlit+hdist];

    // use the above code to decode other literal and distance code
    // Above decoded run length are now used to fill the code length of literals and distance codes which are assumed to be laid out sequentially

    int index = 0;
    int prev = 0;
    while (index < hlit+hdist) // Decode both literal and distance codes
    {
    	int sym = decode(&lencode);
    	int repeat = 0;
    	switch (sym)
    	{
    	case 0 : case 1 : case 2 : case 3 : case 4: case 5: case 6: case 7: case 8 : case 9: // Lol it's funny
    	case 10: case 11: case 12: case 13: case 14: case 15:
    	    all_codes[index++] = sym;
    	    break;
    	case 16:
    	    prev = all_codes[index-1];
    	    repeat = 3 + read_bit(2);
    	    break;
    	case 17:
    	    prev = 0;
    	    repeat = 3 + read_bit(3);
    	    break;
    	case 18:
    	    prev = 0;
    	    repeat = 11 + read_bit(7);
    	    break;
    	default: assert(1!=0);
    	}
    	while(repeat--)
    	    all_codes[index++] = prev;
    }

    // destroy the len code
    free(lencode.count);
    free(lencode.code);

    #ifdef DEBUG
    for(int i = 0; i < hlit+hdist; ++i)
    {
	if (all_codes[i]!=0)
	    printf("\nAll_codes[%d] -> %d.",i,all_codes[i]);
    }
    #endif 

    
    // Build huffman code lookup table for lencode using first hlit members of all_codes[]
    int lit_err = construct_cannonical_huffman(all_codes,&lencode,hlit);
    
    // Now form the huffman codes of remaining distance codes for all_codes+hlit onward 
    int err = construct_cannonical_huffman(all_codes+hlit,&distcode,hdist);
    
    decompressor(&lencode,&distcode);
    return 0;
}

// construct huffman table from the above occurences of the symbols

int construct_cannonical_huffman(int* f_table,struct huffman* hman, int len)
{
    // This function along with decode() function are used to decode the encoded huffman code
    // It creates a lookup table for decoding huffman.
    
    int counter[MAXBITS+1] = {0};

    // counter counts the frequency of each code length 
    for(int i = 0; i < len; ++i)
    {
	counter[f_table[i]]++;
    }

    // construct the hman table
    hman->count = malloc(sizeof(int) * MAXBITS+1);
    hman->code = malloc(sizeof(int) * len);
    for(int i = 0; i <  len; ++i)
    {

	hman->code[i] = 0;
    }

    // same as abouve counter .. redundant code 
    for(int i = 0; i <= MAXBITS; ++i)
	hman->count[i] = counter[i];

    
    int offset[MAXBITS+1] = {0};

    // It calculates the offset of each code length .. it acts like a cumulative frequency table of the code length  
    for(int i = 1; i < MAXBITS+1; ++i)
	offset[i+1] = offset[i] + counter[i];

    // Its not that complicated once you understand huffman coding scheme used by deflate .. rfc 1951
    
    for(int symbol = 0; symbol < len; ++symbol)
    {
	// Depending upon the code length and offset value the huffman code are assigned the symbol
	// say you have literal 100 with code length 2 and 150 again with code length 2
	// And if offset of code_length (2) is 15 (say) then literal 100 would have huffman code 15 and 150 would have 16.
	// Rest is done by the decoder
	// A naive way could also be implemented but its half copied :D
	// All is well if you understand it enough.. No need to invent everything from ground for everything 
	if(f_table[symbol]!=0) // symbol with code length 0 doesn't take part in huffman code generation 
	    hman->code[offset[f_table[symbol]]++] = symbol;

    }

#ifdef DEBUG
    for(int i = 0; i < len; ++i)
    {
    	printf("\nhuff->count[%d] -> %d.",i,hman->code[i]);
    }
#endif
    putchar('\n');
    return 0;
}

// Now need to implement the decoding section of the PNG 
    
int decode(struct huffman* lencode)
{
    // Its a clever function used to decode the huffman code
    // It should be reasonably faster than any simple method.
    
    // Not difficult to understand but very very difficult to invent it on your own .. I couln't have done that
    
    int code = 0;
    int count = 0;
    int first = 0;
    int index = 0;
    for (int i = 1; i <= MAXBITS; ++i)
    {
	code |= read_bit(1);
	count = lencode->count[i]; // Read its length
	if(code-first < count)
	    return lencode->code[index + (code-first)];
	first+=count;
	index+=count;
	first <<= 1;
	code <<= 1;
    }
    return -2;
}

int decompressor(struct huffman* lencode,struct huffman* distcode)
{
    short lengths[] = {3,4,5,6,7,8,9,10,11,13,
		       15,17,19,23,27,31,35,43,51,59,
		       67,83,99,115,131,163,195,227,258};
    short lengths_bit[] = {0,0,0,0,0,0,0,0,1,1,
			   1,1,2,2,2,2,3,3,3,3,
			   4,4,4,4,5,5,5,5,0};
    static_assert(sizeof(lengths)/sizeof(short) == sizeof(lengths_bit)/sizeof(short)); // Just a quick check
    int distance[] = {1,2,3,4,5,7,9,13,17,25,
		      33,49,65,97,129,193,257,385,513,769,
		      1025,1537,2049,3073,4097,6145,8193,12289,16385,24577};

    short distance_extra[] = {0,0,0,0,1,1,2,2,3,3,
			      4,4,5,5,6,6,7,7,8,8,
			      9,9,10,10,11,11,12,12,13,13};
    int symbol;
    int dist;
    int dist_code;
    int len;
    int counter = 0;
    do
    {
	counter++;
	// used for debugging purpose to sync compressor and decompressor
	/* if(counter==20000000) */
	/*     break; */
	
	symbol = decode(lencode);

#ifdef DEBUG
	printf(const char *restrict __format, ...)        //printf("\nSymbol read counter[%d] -> is : %d with outlen -> %d.",counter,symbol,bit_read.outlen);
#endif
	    if(symbol<256)
		bit_read.outdata[bit_read.outlen++] = symbol;
	    else
	    {
		if (symbol == 256)
		{
		    printf("\nEnd of the block reached....");
		    break;
		}
		else
		{

		    assert(symbol > 256 && symbol < 286);
		    len = lengths[symbol-257] + read_bit(lengths_bit[symbol-257]);
		    dist_code = decode(distcode);
#ifdef DEBUG
                    printf("\Current read len is : %d",len);
		    printf("\nCurrent read dist code is : %d.",dist_code);
#endif
		    assert(dist_code < 30);

                    dist = distance[dist_code] + read_bit(distance_extra[dist_code]);

                    if(bit_read.outlen < dist)
		    {
			
			// write even incomplete data to file.txt and assert 
			FILE* fp = fopen("incomplete.txt","wb");
			fwrite(bit_read.outdata,sizeof(unsigned char), bit_read.outlen,fp);
			assert(!"Outlen smaller than go_back distance");
		    }
		    
		    for (int i = 0; i < len; ++i)
		    {
			bit_read.outdata[bit_read.outlen] = bit_read.outdata[bit_read.outlen-dist];
			bit_read.outlen++;
		    }

		}
	    }
    }
    while(symbol!=256);
	
    printf("\nOutlen after the total operation is : %d.",bit_read.outlen);
    return 0;
}
