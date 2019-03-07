#include "ccheap.h"
#include "common.h"
#include <malloc.h>
#include <stdio.h>
static bool HpMaxHeap(int a, int b) {
	return a > b;
}

static bool HpMinHeap(int a, int b) {
	return a < b;
}

int HpCreateMaxHeap(CC_HEAP **MaxHeap, CC_VECTOR* InitialElements)
{
	if (MaxHeap == NULL)
	{
		return CC_ERROR_INVALID_PARAMETER;
	}

	(*MaxHeap) = (CC_HEAP*)malloc(sizeof(CC_HEAP));
	if ((*MaxHeap) == NULL)
	{
		goto cleanup;
	}

	(*MaxHeap)->Capacity = 10;
	(*MaxHeap)->Vector = (int*)malloc(sizeof(int)*(*MaxHeap)->Capacity);
	if ((*MaxHeap)->Vector == NULL)
	{
		goto cleanup;
	}

	(*MaxHeap)->Dimension = 0;
	(*MaxHeap)->Relation = HpMaxHeap;

	if (InitialElements != NULL)
	{
		unsigned int i = 0;
		int value = -1;
		for (; i < InitialElements->Dimension; ++i)
		{
			VecGetValueByIndex(InitialElements, i, &value);
			HpInsert((*MaxHeap), value);
		}
	}
cleanup:
	if ((*MaxHeap) == NULL) {
		return CC_ERROR_MEMORY_ALLOCATION;
	}
	if ((*MaxHeap)->Vector == NULL) {
		free(*MaxHeap);
		*MaxHeap = NULL;
		return CC_ERROR_MEMORY_ALLOCATION;
	}
	return CC_SUCCESS;

}

int HpCreateMinHeap(CC_HEAP **MinHeap, CC_VECTOR* InitialElements)
{
	
	if (MinHeap == NULL)
	{
		return CC_ERROR_INVALID_PARAMETER;
	}

	(*MinHeap) = (CC_HEAP*)malloc(sizeof(CC_HEAP));
	if ((*MinHeap) == NULL) {
		goto cleanup;
	}

	(*MinHeap)->Capacity = 10;
	(*MinHeap)->Vector = (int*)malloc(sizeof(int)*(*MinHeap)->Capacity);
	if ((*MinHeap)->Vector == NULL) {
		goto cleanup;
	}
	(*MinHeap)->Dimension = 0;
	(*MinHeap)->Relation = HpMinHeap;
	
	if (InitialElements != NULL)
	{
		unsigned int i = 0;
		int value = -1;
		for (; i < InitialElements->Dimension; ++i) 
		{
			VecGetValueByIndex(InitialElements, i, &value);
			HpInsert((*MinHeap), value);
		}
	}
cleanup:
	if ((*MinHeap) == NULL) {
		return CC_ERROR_MEMORY_ALLOCATION;
	}
	if ((*MinHeap)->Vector == NULL) {
		free(*MinHeap);
		*MinHeap = NULL;
		return CC_ERROR_MEMORY_ALLOCATION;
	}
	return CC_SUCCESS;
}

int HpDestroy(CC_HEAP **Heap)
{
	if (Heap == NULL)
	{
		return CC_ERROR_INVALID_PARAMETER;
	}
	if ((*Heap)->Vector != NULL) {
		free((*Heap)->Vector);
	}
	free((*Heap));
	*Heap = NULL;
	return CC_SUCCESS;
}

static int HpGoTop(CC_HEAP* Heap, int Index)
{
	if (Heap == NULL || Index<1 || (unsigned int)Index>Heap->Dimension)
	{
		return CC_ERROR_INVALID_PARAMETER;
	}

	int element = Heap->Vector[Index];
	int currentPosition = Index;
	int parent = currentPosition / 2;
	while (parent >= 1 && !Heap->Relation(Heap->Vector[parent], element))
	{
		Heap->Vector[currentPosition] = Heap->Vector[parent];
		currentPosition = parent;
		parent /= 2;
	}
	Heap->Vector[currentPosition] = element;
	return CC_SUCCESS;

}

static int HpRedimension(CC_HEAP*Heap) {
	if (Heap == NULL)
	{
		return CC_ERROR_INVALID_PARAMETER;
	}
	int* newVector = (int*)malloc(sizeof(int)*Heap->Capacity * 2);
	if (newVector == NULL)
	{
		goto cleanup;
	}

	unsigned int i = 0;
	for (; i <= Heap->Dimension; ++i) {
		newVector[i] = Heap->Vector[i];
	}

	Heap->Capacity *= 2;
	free(Heap->Vector);
	Heap->Vector = newVector;

cleanup:
	if (newVector == NULL)
	{
		return CC_ERROR_MEMORY_ALLOCATION;
	}

	return CC_SUCCESS;
}

int HpInsert(CC_HEAP *Heap, int Value)
{
	if (Heap == NULL)
	{
		return CC_ERROR_INVALID_PARAMETER;
	}

	if (Heap->Dimension+1 == Heap->Capacity)
	{
		if (HpRedimension(Heap) != CC_SUCCESS)
		{
			return CC_ERROR_MEMORY_ALLOCATION;
		}
	}

	Heap->Vector[++Heap->Dimension] = Value;
	HpGoTop(Heap, Heap->Dimension);

	return CC_SUCCESS;
}

static int HpGoDown(CC_HEAP *Heap, int Index) {
	if (Heap == NULL || Index<1 || (unsigned int)Index>Heap->Dimension) {
		return CC_ERROR_INVALID_PARAMETER;
	}
	int element = Heap->Vector[Index];
	unsigned int currentPosition = Index;
	unsigned int childPosition = 2 * currentPosition;
	while (childPosition <= Heap->Dimension)
	{
		if (childPosition < Heap->Dimension)
		{
			if (!Heap->Relation(Heap->Vector[childPosition], Heap->Vector[childPosition + 1])) {
				childPosition++;
			}
		}
		if (!Heap->Relation(Heap->Vector[childPosition], element)) {
			break;
		}
		else
		{
			Heap->Vector[currentPosition] = Heap->Vector[childPosition];
			currentPosition = childPosition;
			childPosition *= 2;
		}
	}
	Heap->Vector[currentPosition] = element;
	return CC_SUCCESS;
}

int HpRemove(CC_HEAP *Heap, int Value)
{
	if (Heap == NULL)
	{
		return CC_ERROR_INVALID_PARAMETER;
	}

	unsigned int i = 1;
	for (; i <= Heap->Dimension; ++i)
	{
		if (Heap->Vector[i] == Value)
		{
			Heap->Vector[i] = Heap->Vector[Heap->Dimension];
			Heap->Dimension--;
			HpGoDown(Heap, i);
			i--;
		
		}
	}

	return CC_SUCCESS;
}

int HpGetExtreme(CC_HEAP *Heap, int* ExtremeValue)
{
	if (Heap == NULL || ExtremeValue == NULL)
	{
		return CC_ERROR_INVALID_PARAMETER;
	}
	*ExtremeValue = Heap->Vector[1];
	return CC_SUCCESS;
}

int HpPopExtreme(CC_HEAP *Heap, int* ExtremeValue)
{
	if (Heap == NULL || ExtremeValue == NULL)
	{
		return CC_ERROR_INVALID_PARAMETER;
	}
	HpGetExtreme(Heap, ExtremeValue);
	HpRemove(Heap, *ExtremeValue);
	return CC_SUCCESS;
}

int HpGetElementCount(CC_HEAP *Heap)
{
	if (Heap == NULL)
	{
		return CC_ERROR_INVALID_PARAMETER;
	}
	
	return Heap->Dimension;
}

static int HpPopOneAppearanceOfExtreme(CC_HEAP* Heap,int* ExtremeValue)
{
	if (Heap == NULL)
	{
		return CC_ERROR_INVALID_PARAMETER;
	}

	HpGetExtreme(Heap, ExtremeValue);
	Heap->Vector[1] = Heap->Vector[Heap->Dimension];
	Heap->Dimension--;
	HpGoDown(Heap, 1);

	return CC_SUCCESS;
}

int HpSortToVector(CC_HEAP *Heap, CC_VECTOR* SortedVector)
{
	if (Heap == NULL || SortedVector == NULL)
	{
		return CC_ERROR_INVALID_PARAMETER;
	}

	unsigned int i = 1;
	CC_HEAP* auxHeap;
	if (HpCreateMinHeap(&auxHeap,NULL) == CC_ERROR_MEMORY_ALLOCATION) {
		return CC_ERROR_MEMORY_ALLOCATION;
	}
	free(auxHeap->Vector);
	auxHeap->Vector = (int*)malloc(sizeof(int)*(Heap->Dimension+1));
	auxHeap->Capacity = Heap->Dimension + 1;
	if (auxHeap->Vector == NULL)
	{
		return CC_ERROR_MEMORY_ALLOCATION;
	}

	for (; i <= Heap->Dimension; ++i)
	{
		HpInsert(auxHeap, Heap->Vector[i]);
	}
	
	int extremeValue=0;
	VecClear(SortedVector);
	while(HpGetElementCount(auxHeap)!=0){
		HpPopOneAppearanceOfExtreme(auxHeap, &extremeValue);
		if (VecInsertTail(SortedVector, extremeValue) == CC_ERROR_MEMORY_ALLOCATION) 
		{
			return CC_ERROR_MEMORY_ALLOCATION;
		}
	}
	HpDestroy(&auxHeap);
	return CC_SUCCESS;
}
