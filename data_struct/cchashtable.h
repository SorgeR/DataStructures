#pragma once
typedef struct _CC_HASHTABLE_NODE {
	int Value;
	char* Key;
}CC_HASHTABLE_NODE;
typedef struct _CC_HASH_TABLE { 
    // Members
	CC_HASHTABLE_NODE**Vector;
	unsigned int Capacity;
	unsigned int Dimension;
	int(*HtDispersionFunction)(struct _CC_HASH_TABLE*, char*, int);
} CC_HASH_TABLE; 
  
int HtCreate(CC_HASH_TABLE** HashTable); 
int HtDestroy(CC_HASH_TABLE** HashTable); 
 
int HtSetKeyValue(CC_HASH_TABLE* HashTable, char* Key, int Value); 
int HtGetKeyValue(CC_HASH_TABLE* HashTable, char* Key, int *Value); 
int HtRemoveKey(CC_HASH_TABLE* HashTable, char* Key); 
int HtHasKey(CC_HASH_TABLE* HashTable, char* Key); 
int HtGetNthKey(CC_HASH_TABLE* HashTable, int Index, char** Key); 
int HtClear(CC_HASH_TABLE* HashTable); 
int HtGetKeyCount(CC_HASH_TABLE* HashTable);  
