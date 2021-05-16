#include "bloomfilter.h"



#include <stdlib.h>
#include <math.h>
#include <stdio.h>

int size_in_bytes;


struct BloomFilter* init_Bloom(){

    struct BloomFilter* bf;
    bf = malloc(sizeof(struct BloomFilter));

    bf->num_of_bits = size_in_bytes*8;
    bf->num_of_pos = size_in_bytes / sizeof(int) + 1;       //100kb / sizeof(int) -> table size
    bf->array = malloc( (bf->num_of_pos)*sizeof(int) );

    for (int i = 0; i < bf->num_of_pos; i++)
    {
        bf->array[i] = 0;
    }
    

    return bf;
}




void insert_Bloom(struct BloomFilter bf, int K, char* element){

    unsigned long pos, int_num, bit_num;

    //printf("\nBloomInsert: %s\n", element);


    for (int i = 0; i < K; i++)
    {
        pos =  hash_i( (unsigned char*)element, i) % bf.num_of_bits;
        int_num = (float)pos/(8*(float)sizeof(int));
        bit_num = pos % sizeof(int);
        set_bit( &(bf.array[int_num]), bit_num);

    }



}

void set_bit(unsigned int* int_pos, int bit_pos){

    *int_pos = *int_pos | 1<<bit_pos;

}



int search_Bloom(struct BloomFilter bf, int K, char* element){
    
    unsigned long pos, int_num, bit_num;

    //printf("\nSearching for element: %s\n", element);


    for (int i = 0; i < K; i++)
    {
        pos =  hash_i( (unsigned char*)element, i) % bf.num_of_bits;
        int_num = (float)pos/(8*(float)sizeof(int));
        bit_num = pos % sizeof(int);
        
        if ( ( bf.array[int_num] & 1<<bit_num) == 0 ){
            //printf("Definetely not in the structure\n");
            return 0;
            
        }

    }

    //printf("Maybe in the structure.\n");
    return 1;


}






void destroy_Bloom(struct BloomFilter *bf){
	free(bf->array);
    free(bf);
    bf = NULL;
	//printf("Bloom Filter destroyed.\n");
}




void print_Bloom(struct BloomFilter bf){
	printf("\n");
	int i,j;
    int bitcount = 1;

	for (i=0;i< bf.num_of_pos; i++){

        bitcount = 1;

        for (j=0; j<sizeof(int)*8; j++){     //for every bit in this position
		    printf("%u  ", ((bf.array)[i]) & bitcount);
            bitcount = bitcount<<1;

        }

        printf("\n");
	}
	printf("\n");
	
	printf("bits:%lu bytes:%lu\n", bf.num_of_bits, bf.num_of_pos);
}


//////////////////////////////////////////////////////////////////
////////HASH FUNCTIONS

unsigned long djb2(unsigned char *str) {
	unsigned long hash = 5381;
	int c; 
	while (c = *str++ ) {
		hash = ((hash << 5) + hash) + c; 
	}
	return hash;
}


unsigned long sdbm(unsigned char *str) {
	unsigned long hash = 0;
	int c;

	while (c = *str++) {
		hash = c + (hash << 6) + (hash << 16) - hash;
	}

	return hash;
}



unsigned long hash_i(unsigned char *str, unsigned int i) {
	return djb2(str) + i*sdbm(str) + i*i;
}

