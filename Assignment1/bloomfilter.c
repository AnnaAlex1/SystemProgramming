#include "bloomfilter.h"



#include <stdlib.h>
#include <math.h>
#include <stdio.h>



/*
int main(void){


    struct BloomFilter *bf;
    bf = init_Bloom();

    insert_Bloom(*bf, 3, "hello");
    insert_Bloom(*bf, 3, "anna");

    print_Bloom(*bf);

    search_Bloom(*bf, 3, "hello");
    search_Bloom(*bf, 3, "helo");
    search_Bloom(*bf, 3, "anna");
    search_Bloom(*bf, 3, "32ew");




    destroy_Bloom(bf);

}

*/

struct BloomFilter* init_Bloom(){


    struct BloomFilter* bf;
    bf = malloc(sizeof(struct BloomFilter));

    bf->num_of_bits = SIZE_IN_BITS;
    bf->num_of_pos = (SIZE_IN_BITS/8) / sizeof(int) + 1;       //100kb / sizeof(int) -> table size
    bf->array = malloc( (bf->num_of_pos)*sizeof(int) );
    //bf->array = malloc( (SIZE_IN_BITS/8) / sizeof(int) + 1);

    for (int i = 0; i < bf->num_of_pos; i++)
    {
        bf->array[i] = 0;
    }
    

    return bf;
}




void insert_Bloom(struct BloomFilter bf, int K, char* element){

    unsigned long pos, int_num, bit_num;

    printf("\nBloomInsert: %s\n", element);


    for (int i = 0; i < K; i++)
    {
        pos =  hash_i( (unsigned char*)element, i) % SIZE_IN_BITS;
        //printf("Hash value: %ld\n", pos);
        int_num = (float)pos/(8*(float)sizeof(int));
        //printf("Int: %ld\n", int_num);
        bit_num = pos % sizeof(int);
        //printf("Bit in int: %ld\n", bit_num);
        set_bit( &(bf.array[int_num]), bit_num);

    }



}

void set_bit(unsigned int* int_pos, int bit_pos){

    *int_pos = *int_pos | 1<<bit_pos;

   // *int_pos = *int_pos | (unsigned int)pow(2, bit_pos);
}



int search_Bloom(struct BloomFilter bf, int K, char* element){
    
    unsigned long pos, int_num, bit_num;

    printf("\nSearching for element: %s\n", element);


    for (int i = 0; i < K; i++)
    {
        pos =  hash_i( (unsigned char*)element, i) % SIZE_IN_BITS;
        //printf("Hash value: %ld\n", pos);
        int_num = (float)pos/(8*(float)sizeof(int));
        //printf("Int: %ld\n", int_num);
        bit_num = pos % sizeof(int);
        //printf("Bit in int: %ld\n", bit_num);

        
        if ( ( bf.array[int_num] & 1<<bit_num) == 0 ){
            printf("Definetely not in the structure\n");
            return 0;
            
        }
        //set_bit( &(bf.array[int_num]), bit_num);

    }

    printf("Maybe in the structure.\n");
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

