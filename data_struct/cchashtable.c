#include "cchashtable.h"
#include "common.h"
#include <malloc.h>

#define HASHTABLE_INITIAL_CAPACITY 10
#define CC_NOT_EXISTENT_ELEMENT -1
#define CC_ELEMENT_ALREADY_EXISTS -1

//first hash function
static int HtDispersionFunctionOne(char* Key)
{
	if (Key == NULL)
	{
		return CC_ERROR_INVALID_PARAMETER;
	}
	int sumAscii = 0;
	char* start = Key;
	while (Key[0] != '\0') {
		sumAscii += Key[0];
		Key++;
	}
	Key = start;
	
	return sumAscii;
}

//second has function
static int HtDispersionFunctionTwo(char* Key)
{
	if (Key == NULL)
	{
		return CC_ERROR_INVALID_PARAMETER;
	}

	int sumAsciiVocals = 0;
	char* start = Key;
	while (Key[0]!='\0' ) {
		if ((Key[0] == 'a' || Key[0] == 'e' || Key[0] == 'i' || Key[0] == 'o' || Key[0] == 'u'))
		{
			sumAsciiVocals += Key[0];
		}
		Key++;
	}
	Key = start;

	return sumAsciiVocals;
}

//the main hash function
static int HtDispersionFunction(CC_HASH_TABLE* HashTable,char* Key,int Index)
{
	if (Key == NULL)
	{
		return CC_ERROR_INVALID_PARAMETER;
	}


	return (HtDispersionFunctionOne(Key)+Index*HtDispersionFunctionTwo(Key)) % HashTable->Capacity;
}

//calculates the size of a string
static int HtSizeofString(char*String)
{
	if (String == NULL)
	{
		return CC_ERROR_INVALID_PARAMETER;
	}
	unsigned int count = 0;
	while (*String != '\0')
	{
		count++;
		String++;
	}
	return count;
}

//copies a string
static int HtCopyString(char*Original, char**Result)
{
	if (Original == NULL || Result == NULL)
	{
		return CC_ERROR_INVALID_PARAMETER;
	}
	unsigned int sizeOfOriginal = HtSizeofString(Original);
	(*Result) = (char*)malloc(sizeOfOriginal+1);
	if ((*Result) == NULL)
	{
		return CC_ERROR_MEMORY_ALLOCATION;
	}
	unsigned int index = 0;
	while (*Original != '\0')
	{
		(*Result)[index] = *Original;
		index++;
		Original++;
	}
	(*Result)[index] = '\0';
	return CC_SUCCESS;
}

//compares two strings by values
static int HtCmpStrings(char* str1, char* str2)
{
	while (*str1 != '\0' && str2 != '\0') {
		if (*str1 != *str2)
		{
			return 0;
		}
		str1++;
		str2++;
	}

	if (*str1 == '\0' && *str2 == '\0') {
		return 1;
	}
	return 0;
}

//creates a hashtable node
static int HtCreateNode(CC_HASHTABLE_NODE**Node, char*Key, int Value)
{
	if (Node == NULL || Key == NULL)
	{
		return CC_ERROR_INVALID_PARAMETER;
	}
	(*Node) = (CC_HASHTABLE_NODE*)malloc(sizeof(CC_HASHTABLE_NODE));
	if ((*Node) == NULL)
	{
		return CC_ERROR_MEMORY_ALLOCATION;
	}
	HtCopyString(Key, &(*Node)->Key);
	(*Node)->Value = Value;
	return CC_SUCCESS;
}

//destroys a hashtbale node
static int HtDestroyNode(CC_HASHTABLE_NODE**Node)
{
	if (Node == NULL)
	{
		return CC_ERROR_INVALID_PARAMETER;
	}
	if ((*Node)->Key != NULL) 
	{
		free((*Node)->Key);
	}
	free(*Node);
	*Node = NULL;
	return CC_SUCCESS;
}


int HtCreate(CC_HASH_TABLE** HashTable)
{
	if (HashTable == NULL) 
	{
		return CC_ERROR_INVALID_PARAMETER;
	}
	(*HashTable) = (CC_HASH_TABLE*)malloc(sizeof(CC_HASH_TABLE));
	if ((*HashTable) == NULL) {
		goto cleanup;
	}
	(*HashTable)->Vector = (CC_HASHTABLE_NODE**)malloc(sizeof(CC_HASHTABLE_NODE*)*HASHTABLE_INITIAL_CAPACITY);
	if((*HashTable)->Vector==NULL)
	{
		goto cleanup;
	}
	(*HashTable)->Capacity = HASHTABLE_INITIAL_CAPACITY;
	(*HashTable)->Dimension = 0;
	(*HashTable)->HtDispersionFunction = HtDispersionFunction;
	unsigned int i = 0;
	for(;i<(*HashTable)->Capacity;++i)
	{
		(*HashTable)->Vector[i] = NULL;
	}

cleanup:
	if ((*HashTable) == NULL) 
	{
		return CC_ERROR_MEMORY_ALLOCATION;
	}

	if ((*HashTable)->Vector == NULL)
	{
		free(*HashTable);
		return CC_ERROR_MEMORY_ALLOCATION;
	}

    return CC_SUCCESS;
}

//destroys the hashtable
int HtDestroy(CC_HASH_TABLE** HashTable)
{
	if (HashTable == NULL)
	{
		return CC_ERROR_INVALID_PARAMETER;
	}
	if ((*HashTable)->Vector != NULL)
	{
		unsigned int i = 0;
		for(;i<(*HashTable)->Capacity;++i)
		{
			if ((*HashTable)->Vector[i] != NULL)
			{
				HtDestroyNode(&(*HashTable)->Vector[i]);
				(*HashTable)->Vector[i] = NULL;
			}
		}
		free((*HashTable)->Vector);
	}
	free((*HashTable));
	*HashTable = NULL;
    return CC_SUCCESS;
}

//redimensions the hashtable (doubles it's capacity)
static int HtRedimension(CC_HASH_TABLE* HashTable)
{
	if (HashTable == NULL)
	{
		return CC_ERROR_INVALID_PARAMETER;
	}
	CC_HASHTABLE_NODE** newVector = (CC_HASHTABLE_NODE**)malloc(HashTable->Capacity * 2 * sizeof(CC_HASHTABLE_NODE*));
	if (newVector == NULL)
	{
		goto cleanup;
	}


	unsigned int i = 0;
	for (; i < HashTable->Capacity*2; ++i) {
		newVector[i] = NULL;
	}

	CC_HASHTABLE_NODE** hashVector=HashTable->Vector;
	HashTable->Vector = newVector;
	HashTable->Dimension = 0;
	HashTable->Capacity *= 2;
	for (i=0; i < HashTable->Capacity/2; ++i)
	{
		if (hashVector[i] != NULL && hashVector[i]->Key!=NULL)
		{
			HtSetKeyValue(HashTable, hashVector[i]->Key, hashVector[i]->Value);
		}
	}

	for (i = 0; i < HashTable->Capacity/2; ++i) {
		HtDestroyNode(&hashVector[i]);
	}
	free(hashVector);
	hashVector = NULL;
	
	
	

cleanup:
	if (newVector == NULL)
	{
		return CC_ERROR_MEMORY_ALLOCATION;
	}
	
	return CC_SUCCESS;
}

//sets the value of a key
int HtSetKeyValue(CC_HASH_TABLE* HashTable, char* Key, int Value)
{
	if (HashTable == NULL || Key == NULL)
	{
		return CC_ERROR_INVALID_PARAMETER;
	}
	int value = 0;
	if (HtGetKeyValue(HashTable, Key, &value) != CC_NOT_EXISTENT_ELEMENT)
	{
		return CC_ELEMENT_ALREADY_EXISTS;
	}
	if (HashTable->Capacity == HashTable->Dimension)
	{
		if (HtRedimension(HashTable)==CC_ERROR_MEMORY_ALLOCATION) {
			return CC_ERROR_MEMORY_ALLOCATION;
		}
	}
	unsigned int index = 0;
	unsigned int position = 0;
	CC_HASHTABLE_NODE* newNode;
	if (HtCreateNode(&newNode, Key, Value) == CC_ERROR_MEMORY_ALLOCATION)
	{
		return CC_ERROR_MEMORY_ALLOCATION;
	}

	while (index <= HashTable->Capacity)
	{
		position = HashTable->HtDispersionFunction(HashTable,Key, index);
		if (HashTable->Vector[position] == NULL)
		{
			HashTable->Vector[position] = newNode;
			HashTable->Dimension++;
			return CC_SUCCESS;
		}
		if (HashTable->Vector[position]->Key == NULL) 
		{
			HtDestroyNode(&HashTable->Vector[position]);
			HashTable->Vector[position] = newNode;
			HashTable->Dimension++;
			return CC_SUCCESS;
		}
		index++;
	}
    return CC_SUCCESS;
}

//gets the value from a key
int HtGetKeyValue(CC_HASH_TABLE* HashTable, char* Key, int *Value)
{
	if (HashTable == NULL || Key == NULL || Value == NULL)
	{
		return CC_ERROR_INVALID_PARAMETER;
	}
	unsigned int index = 0;
	unsigned int position = 0;
	while (index <= HashTable->Capacity)
	{
		position = HashTable->HtDispersionFunction(HashTable,Key, index);

		if (HashTable->Vector[position] == NULL) 
		{
			return CC_NOT_EXISTENT_ELEMENT;
		}

		if(HashTable->Vector[position]->Key!=NULL && HtCmpStrings(HashTable->Vector[position]->Key,Key)==1)
		{
			*Value= HashTable->Vector[position]->Value;
			return CC_SUCCESS;
		}
		index++;
	}
    return CC_NOT_EXISTENT_ELEMENT;
}


//removes all appearances of a key
int HtRemoveKey(CC_HASH_TABLE* HashTable, char* Key)
{
	if (HashTable == NULL || Key == NULL)
	{
		return CC_ERROR_INVALID_PARAMETER;
	}

	unsigned int index = 0;
	int position = -1;
	while (index <= HashTable->Capacity)
	{
		position = HashTable->HtDispersionFunction(HashTable,Key, index);
		if (HashTable->Vector[position] == NULL)
		{
			return CC_NOT_EXISTENT_ELEMENT;
		}
		if (HashTable->Vector[position]->Key!=NULL && HtCmpStrings(HashTable->Vector[position]->Key, Key) == 1)
		{
			free(HashTable->Vector[position]->Key);
			HashTable->Vector[position]->Key = NULL;
			HashTable->Dimension--;
			return CC_SUCCESS;
		
		}
		index++;
	}
	
	
		return CC_NOT_EXISTENT_ELEMENT;
	
}

int HtHasKey(CC_HASH_TABLE* HashTable, char* Key)
{
	if (HashTable == NULL || Key == NULL)
	{
		return CC_ERROR_INVALID_PARAMETER;
	}
	unsigned int index = 0;
	int position = -1;
	while (index <= HashTable->Capacity)
	{
		position = HashTable->HtDispersionFunction(HashTable, Key, index);
		if(HashTable->Vector[position]==NULL)
		{
			return 0;
		}
		if (HashTable->Vector[position]->Key != NULL && HtCmpStrings(HashTable->Vector[position]->Key, Key) == 1)
		{
			return 1;
		}
		index++;
	}
    return 0;
}

int HtGetNthKey(CC_HASH_TABLE* HashTable, int Index, char** Key)
{
	if (HashTable == NULL || Key == NULL || Index<0 || HashTable->Dimension==0 || (unsigned int)Index>HashTable->Dimension - 1) {
		return CC_ERROR_INVALID_PARAMETER;
	}
	unsigned int i = 0;
	for (; i < HashTable->Capacity; ++i) {
		
		if(HashTable->Vector[i]!=NULL && HashTable->Vector[i]->Key != NULL)
		{
			if (Index == 0) {
				(*Key)=HashTable->Vector[i]->Key;
				return CC_SUCCESS;
			}
			--Index;

		}
	}
	return CC_NOT_EXISTENT_ELEMENT;
}

int HtClear(CC_HASH_TABLE* HashTable)
{
	if (HashTable == NULL)
	{
		return CC_ERROR_INVALID_PARAMETER;
	}

	HashTable->Dimension = 0;
	unsigned int i=0;
	for (; i < HashTable->Capacity; ++i)
	{
		if (HashTable->Vector[i] != NULL)
		{
			CC_HASHTABLE_NODE* oldNode = HashTable->Vector[i];
			HashTable->Vector[i] = NULL;
			HtDestroyNode(&oldNode);
		}
	}

    return CC_SUCCESS;
}

int HtGetKeyCount(CC_HASH_TABLE* HashTable)
{
	if (HashTable == NULL) {
		return CC_ERROR_INVALID_PARAMETER;
	}
	unsigned int i = 0;
	unsigned int countOfKeys = 0;
	for (; i < HashTable->Capacity; ++i) 
	{
		if (HashTable->Vector[i] != NULL && HashTable->Vector[i]->Key != NULL)
		{
			countOfKeys++;
		}
	}
	return countOfKeys;
}
