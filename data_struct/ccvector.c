#include "ccvector.h"
#include "common.h"
#include <malloc.h>
#include <limits.h>

#define CC_INITIAL_CAPACITY 10;

int VecCreate(CC_VECTOR **Vector)
{
	if (Vector == NULL)
	{
		return CC_ERROR_INVALID_PARAMETER;
	}

	(*Vector) = (CC_VECTOR*)malloc(sizeof(CC_VECTOR));

	if ((*Vector) == NULL)
	{
		goto cleanUp;
	}

	(*Vector)->Capacity = CC_INITIAL_CAPACITY;
	(*Vector)->Dimension = 0;
	(*Vector)->Vector = (int*)malloc(sizeof(int)*(*Vector)->Capacity);

	if ((*Vector)->Vector == NULL)
	{
		goto cleanUp;
	}

cleanUp:
	if ((*Vector) == NULL)
	{
		return CC_ERROR_MEMORY_ALLOCATION;
	}
	if ((*Vector)->Vector == NULL)
	{
		free(*Vector);
		*Vector = NULL;
		return CC_ERROR_MEMORY_ALLOCATION;
	}

	return CC_SUCCESS;
}

int VecDestroy(CC_VECTOR **Vector)
{
	if (Vector == NULL)
	{
		return CC_ERROR_INVALID_PARAMETER;
	}

	if ((*Vector)->Vector != NULL)
	{
		free((*Vector)->Vector);
	}
	free((*Vector));
	(*Vector) = NULL;
	return CC_SUCCESS;
}

static int VecRedimension(CC_VECTOR *Vector) {

	if (Vector == NULL)
	{
		return CC_ERROR_INVALID_PARAMETER;
	}

	unsigned int dimensionToMalloc = 0;
	if (Vector->Capacity == UINT_MAX-1)
	{
		return CC_ERROR_MEMORY_ALLOCATION;
	}
	if (Vector->Capacity * 2 < UINT_MAX)
	{
		dimensionToMalloc = Vector->Capacity * 2;
	}
	else 
	{
		dimensionToMalloc = UINT_MAX-1;
	}

	int* newVector = (int*)malloc(sizeof(int)*dimensionToMalloc);
	if (newVector == NULL)
	{
		goto cleanup;
	}

	unsigned int i = 0;
	for (; i < Vector->Dimension; ++i) {
		newVector[i] = Vector->Vector[i];
	}

	Vector->Capacity = dimensionToMalloc;
	free(Vector->Vector);
	Vector->Vector = newVector;

cleanup:
	if (newVector == NULL)
	{
		return CC_ERROR_MEMORY_ALLOCATION;
	}
	return CC_SUCCESS;
}
	
int VecInsertTail(CC_VECTOR *Vector, int Value)
{
	if (Vector == NULL)
	{
		return CC_ERROR_INVALID_PARAMETER;
	}

	if (Vector->Dimension == Vector->Capacity)
	{
		if (VecRedimension(Vector) != CC_SUCCESS)
		{
			return CC_ERROR_MEMORY_ALLOCATION;
		}
	}

	Vector->Vector[Vector->Dimension++] = Value;
	return CC_SUCCESS;
}

int VecInsertHead(CC_VECTOR *Vector, int Value)
{
	if (Vector == NULL)
	{
		return CC_ERROR_INVALID_PARAMETER;
	}

	if (Vector->Dimension == Vector->Capacity)
	{
		if (VecRedimension(Vector) != CC_SUCCESS)
		{
			return CC_ERROR_MEMORY_ALLOCATION;
		}

	}

	Vector->Dimension++;
	unsigned int i = Vector->Dimension - 1;
	for (; i > 0; --i)
	{
		Vector->Vector[i] = Vector->Vector[i - 1];
	}
	Vector->Vector[0] = Value;

	return CC_SUCCESS;
}

int VecInsertAfterIndex(CC_VECTOR *Vector, int Index, int Value)
{
	if (Vector == NULL || Index<0 || Vector->Dimension==0 || (unsigned int)Index>Vector->Dimension - 1)
	{
		return CC_ERROR_INVALID_PARAMETER;
	}

	if (Vector->Dimension == Vector->Capacity)
	{
		if (VecRedimension(Vector) != CC_SUCCESS)
		{
			return CC_ERROR_MEMORY_ALLOCATION;
		}
	}

	Vector->Dimension++;
	int i = Vector->Dimension - 1;
	for (; i > Index + 1; --i)
	{
		Vector->Vector[i] = Vector->Vector[i - 1];
	}
	Vector->Vector[Index + 1] = Value;

	return CC_SUCCESS;
}

int VecRemoveByIndex(CC_VECTOR *Vector, int Index)
{
	if (Vector == NULL || Index<0 || Vector->Dimension==0 || (unsigned int)Index>Vector->Dimension - 1)
	{
		return CC_ERROR_INVALID_PARAMETER;
	}
	Vector->Dimension--;
	unsigned int i = Index;
	for (; i < Vector->Dimension; ++i)
	{
		Vector->Vector[i] = Vector->Vector[i + 1];
	}
	return CC_SUCCESS;
}

int VecGetValueByIndex(CC_VECTOR *Vector, int Index, int *Value)
{
	if (Vector == NULL || Index<0 || (unsigned int)Index>Vector->Dimension - 1 || Value == NULL)
	{
		return CC_ERROR_INVALID_PARAMETER;
	}
	*Value = Vector->Vector[Index];
	return CC_SUCCESS;
}

int VecGetCount(CC_VECTOR *Vector)
{
	if (Vector == NULL)
	{
		return CC_ERROR_INVALID_PARAMETER;
	}

	return Vector->Dimension;
}

int VecClear(CC_VECTOR *Vector)
{
	if (Vector == NULL)
	{
		return CC_ERROR_INVALID_PARAMETER;
	}
	
	Vector->Dimension = 0;
	
	return CC_SUCCESS;
}

int VecSort(CC_VECTOR *Vector)
{
	if (Vector == NULL)
	{
		return CC_ERROR_INVALID_PARAMETER;
	}
	
	unsigned int i = 0, j = 0;
	for (; i < Vector->Dimension - 1; ++i)
	{
		for (j = i + 1; j < Vector->Dimension; ++j) {
			if (Vector->Vector[i] > Vector->Vector[j])
			{
				int aux = Vector->Vector[i];
				Vector->Vector[i] = Vector->Vector[j];
				Vector->Vector[j] = aux;
			}
		}
	}
	return CC_SUCCESS;
}
