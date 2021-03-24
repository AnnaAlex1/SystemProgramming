#ifndef __BLOOM__
#define __BLOOM__

#define NUM_OF_HASHES 16



struct BloomFilter{

    unsigned int* array;
	unsigned long num_of_pos;
	unsigned long num_of_bits;

};


struct BloomFilter* init_Bloom();
void insert_Bloom(struct BloomFilter bf, int K, char* element);
void set_bit(unsigned int* int_pos, int bit_pos);
void destroy_Bloom(struct BloomFilter *bf);
int search_Bloom(struct BloomFilter bf, int K, char* element);

void print_Bloom(struct BloomFilter bf);
////////////
unsigned long djb2(unsigned char *str);
unsigned long sdbm(unsigned char *str);
unsigned long hash_i(unsigned char *str, unsigned int i);


#endif