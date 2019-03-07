#include "cctree.h"
#include "common.h"
#include <malloc.h>
#include <assert.h>

#define CC_ERROR -1
#define CC_NOT_EXISTENT_VALUE -1
#define CC_ALREADY_EXISTENT_VALUE -1
#define CC_EMPTY_TREE -1

static int TreeCreateNode(CC_TREE_NODE** Node,int Value)
{
	if (Node == NULL)
	{
		return CC_ERROR_INVALID_PARAMETER;
	}
	(*Node) = (CC_TREE_NODE*)malloc(sizeof(CC_TREE_NODE));
	if ((*Node) == NULL)
	{
		goto cleanup;
	}
	(*Node)->Left = NULL;
	(*Node)->Right = NULL;
	(*Node)->Value = Value;
	(*Node)->Height = 0;
cleanup:
	if ((*Node) == NULL)
	{
		return CC_ERROR_MEMORY_ALLOCATION;
	}
	return CC_SUCCESS;
}

 static int TreeDestroyNode(CC_TREE_NODE** Node) {
	
	if (Node == NULL) {
		return CC_ERROR_INVALID_PARAMETER;
	}
	free((*Node));
	(*Node) = NULL;
	return CC_SUCCESS;
}

static int TreeGetHeightOfNode(CC_TREE_NODE*node)
{
	if (node == NULL) {
		return -1;
	}
	else {
		return node->Height;
	}
}

static int TreeRecalculateHeight(CC_TREE_NODE* node)
{
	int height = -1;
	if (node == NULL)
	{
		height = -1;
	}
	else
	{
		height = TreeGetHeightOfNode(node->Left) > TreeGetHeightOfNode(node->Right) ? TreeGetHeightOfNode(node->Left) + 1 : TreeGetHeightOfNode(node->Right) + 1;
	}
	return height;
}

static int TreeSingleRotationLeft(CC_TREE_NODE* node,CC_TREE_NODE**newRoot)
{
	CC_TREE_NODE* rightNode = node->Right;
	node->Right = rightNode->Left;
	rightNode->Left = node;
	node->Height = TreeRecalculateHeight(node);
	rightNode->Height = TreeRecalculateHeight(rightNode);
	(*newRoot) = rightNode;
	return CC_SUCCESS;
}

static int TreeSingleRotationRight(CC_TREE_NODE* node,CC_TREE_NODE**newRoot)
{
	CC_TREE_NODE* leftNode = node->Left;
	node->Left = leftNode->Right;
	leftNode->Right = node;
	node->Height = TreeRecalculateHeight(node);
	leftNode->Height = TreeRecalculateHeight(leftNode);
	(*newRoot) = leftNode;
	return CC_SUCCESS;
}

static int TreeDoubleRotationLeft(CC_TREE_NODE*node,CC_TREE_NODE**newRoot)
{
	CC_TREE_NODE *newRoot1;
	TreeSingleRotationRight(node->Right, &newRoot1);
	node->Right = newRoot1;
	TreeSingleRotationLeft(node, newRoot);
	return CC_SUCCESS;

}

static int TreeDoubleRotationRight(CC_TREE_NODE*node, CC_TREE_NODE**newRoot)
{
	CC_TREE_NODE *newRoot1;
	TreeSingleRotationLeft(node->Left, &newRoot1);
	node->Left = newRoot1;
	TreeSingleRotationRight(node, newRoot);
	return CC_SUCCESS;
}

int TreeCreate(CC_TREE **Tree)
{
	if (Tree == NULL) {
		return CC_ERROR_INVALID_PARAMETER;
	}
	(*Tree) = (CC_TREE*)malloc(sizeof(CC_TREE));
	if ((*Tree) == NULL) {
		return CC_ERROR_MEMORY_ALLOCATION;
	}
	(*Tree)->Root = NULL;
	(*Tree)->Dimension = 0;
	return CC_SUCCESS;
}

static int TreeDestroySubtree(CC_TREE_NODE**Node) {
	
	if ((*Node) == NULL) {
		return CC_SUCCESS;
	}
	if ((*Node)->Left!=NULL) {
		TreeDestroySubtree(&((*Node)->Left));
	}
	if ((*Node)->Right != NULL) {
		TreeDestroySubtree(&((*Node)->Right));
	}
	TreeDestroyNode(Node);
	return CC_SUCCESS;
}

int TreeDestroy(CC_TREE **Tree)
{
	if (Tree == NULL) 
	{
		return CC_ERROR_INVALID_PARAMETER;
	}
	if (TreeDestroySubtree(&((*Tree)->Root)) == CC_SUCCESS) {
			free((*Tree));
			(*Tree) = NULL;
			return CC_SUCCESS;
	}
	return CC_ERROR;
}

static CC_TREE_NODE* TreeAddInTreeOfGivenNode(CC_TREE_NODE*Root, int Value)
{
	if (Root == NULL)
	{
		CC_TREE_NODE*newNode;
		TreeCreateNode(&newNode, Value);
		Root = newNode;
	}
	else
	{
		if (Value >= Root->Value) {
			Root->Right = TreeAddInTreeOfGivenNode(Root->Right, Value);
			if (TreeGetHeightOfNode(Root->Right) - TreeGetHeightOfNode(Root->Left) == 2)
			{
				if (Value >= Root->Right->Value)
				{
					TreeSingleRotationLeft(Root, &Root);
				}
				else {
					TreeDoubleRotationLeft(Root, &Root);
				}
			}
			else {
				Root->Height = TreeRecalculateHeight(Root);
			}
		}
		else {
			if (Value <= Root->Value) {
				Root->Left = TreeAddInTreeOfGivenNode(Root->Left, Value);
				if (TreeGetHeightOfNode(Root->Left) -TreeGetHeightOfNode(Root->Right) == 2)
				{
					if (Value <= Root->Left->Value)
					{
						TreeSingleRotationRight(Root, &Root);
					}
					else {
						TreeDoubleRotationRight(Root, &Root);
					}
				}
				else {
					Root->Height = TreeRecalculateHeight(Root);
				}
			}
		}
	}
	return Root;
}

int TreeInsert(CC_TREE *Tree, int Value)
{
	if (Tree == NULL)
	{
		return CC_ERROR_INVALID_PARAMETER;
    }

	
	Tree->Root = TreeAddInTreeOfGivenNode(Tree->Root, Value);
	Tree->Dimension++;
	return CC_SUCCESS;
}

static CC_TREE_NODE* TreeGetMinFromSubtree(CC_TREE_NODE*Root)
{
	CC_TREE_NODE* current=Root;
	while (current->Left != NULL)
	{
		current = current->Left;
	}
	return current;
}

static CC_TREE_NODE* TreeRemoveFromSubtree(CC_TREE_NODE*Root, int Value)
{
	if (Root == NULL) {
		return Root;
	}
	if (Root->Value > Value)
	{
		
		Root->Left= TreeRemoveFromSubtree(Root->Left, Value);
		
	}
	
	if (Root->Value < Value)
	{
		
		Root->Right= TreeRemoveFromSubtree(Root->Right, Value);
		
	}

	if (Root->Value == Value)
	{
		if (Root->Left == NULL || Root->Right==NULL){
			CC_TREE_NODE* temp = Root->Left != NULL ? Root->Left : Root->Right;

			if (temp == NULL)
			{
				temp = Root;
				Root = NULL;
			}
			else {
				*Root = *temp;
			}
			TreeDestroyNode(&temp);
		}
		else {

			CC_TREE_NODE* temp = TreeGetMinFromSubtree(Root->Right);
			Root->Value = temp->Value;
			Root->Right = TreeRemoveFromSubtree(Root->Right, temp->Value);
		}
	}

	if (Root == NULL) {
		return Root;
	}

	Root->Height = TreeGetHeightOfNode(Root->Left) > TreeGetHeightOfNode(Root->Right) ? TreeGetHeightOfNode(Root->Left) + 1 : TreeGetHeightOfNode(Root->Right) + 1;
	
	if (TreeGetHeightOfNode(Root->Left) - TreeGetHeightOfNode(Root->Right) > 1 && TreeGetHeightOfNode(Root->Left->Left)-TreeGetHeightOfNode(Root->Left->Right)>=0)
		
	{
		TreeDoubleRotationRight(Root, &Root);
		return Root;
	}

	if ( TreeGetHeightOfNode(Root->Left) - TreeGetHeightOfNode(Root->Right) > 1 && TreeGetHeightOfNode(Root->Left->Left) - TreeGetHeightOfNode(Root->Left->Right) <0 )
	{
		TreeDoubleRotationRight(Root, &Root);
		return Root;
	}

	if (TreeGetHeightOfNode(Root->Right)- TreeGetHeightOfNode(Root->Left) > 1  && TreeGetHeightOfNode(Root->Left->Left) - TreeGetHeightOfNode(Root->Left->Right)>0)
	{
		TreeDoubleRotationLeft(Root, &Root);
		return Root;
	}
	if (TreeGetHeightOfNode(Root->Right) - TreeGetHeightOfNode(Root->Left) > 1 && TreeGetHeightOfNode(Root->Left->Left) - TreeGetHeightOfNode(Root->Left->Right) < 0)
	{
		TreeSingleRotationLeft(Root, &Root);
		return Root;
	}
	return Root;

}

int TreeRemove(CC_TREE *Tree, int Value)
{
	if (Tree == NULL)
	{
		return CC_ERROR_INVALID_PARAMETER;
	}
	if (TreeContains(Tree, Value) == 0)
	{
		return CC_NOT_EXISTENT_VALUE;
	}

	while (TreeContains(Tree, Value) ==1)
	{
		Tree->Root = TreeRemoveFromSubtree(Tree->Root, Value);
		Tree->Dimension--;
	}
	
	return CC_SUCCESS;
}

static int SearchInSubtree(CC_TREE_NODE* node, int Value)
{
	if (node == NULL)
	{
		return 0;
	}
	else {
		if (node->Value == Value)
		{
			return 1;
		}
		if (Value < node->Value)
		{
			return SearchInSubtree(node->Left, Value);
		}
		else {
			return SearchInSubtree(node->Right, Value);
		}
	}
}

int TreeContains(CC_TREE *Tree, int Value)
{
	if (Tree == NULL)
	{
		return CC_ERROR_INVALID_PARAMETER;
    }
	return SearchInSubtree(Tree->Root, Value);
}

int TreeGetCount(CC_TREE *Tree)
{
	if (Tree == NULL)
	{
		return CC_ERROR_INVALID_PARAMETER;
	}
	return Tree->Dimension;
}

int TreeGetHeight(CC_TREE *Tree)
{
	if (Tree == NULL)
	{
		return CC_ERROR_INVALID_PARAMETER;
    }
	if (Tree->Root != NULL) {
		return Tree->Root->Height;
	}
	else {
		return CC_NOT_EXISTENT_VALUE;
	}
    
}

int TreeClear(CC_TREE *Tree)
{
	if (Tree == NULL)
	{
		return CC_ERROR_INVALID_PARAMETER;
	}

	if (TreeDestroySubtree(&Tree->Root) == CC_SUCCESS) {
		Tree->Dimension = 0;
		return CC_SUCCESS;
	}
	return CC_ERROR;
}

static void TreeGetNthPreorderFromSubtree(CC_TREE_NODE *Root, int *Index,int *Value)
{

	if (Root == NULL) 
	{
		return;
	}
	
	if ((*Index)==0) {
		*Value = Root->Value;
		
	}
	(*Index)--;
	TreeGetNthPreorderFromSubtree(Root->Left, Index, Value);
	TreeGetNthPreorderFromSubtree(Root->Right, Index, Value);

}
int TreeGetNthPreorder(CC_TREE *Tree, int Index, int *Value)
{
	if (Tree == NULL || Value == NULL || Index<0 || (unsigned int)Index>Tree->Dimension-1)
	{
		return CC_ERROR_INVALID_PARAMETER;
	}
	if (Tree->Dimension == 0)
	{
		return CC_EMPTY_TREE;
	}
	int *copyIndex = (int*)malloc(sizeof(int));
	*copyIndex = Index;

	TreeGetNthPreorderFromSubtree(Tree->Root, copyIndex, Value);
	free(copyIndex);
    return CC_SUCCESS;
}

static void TreeGetNthInorderFromSubtree(CC_TREE_NODE *Root, int*Index, int *Value)
{
	static int inorderCount = 0;

	if (Root == NULL)
	{
		return;
	}

	TreeGetNthInorderFromSubtree(Root->Left, Index, Value);
	if ((*Index)==0) {
		*Value = Root->Value;
	}
	(*Index)--;
	TreeGetNthInorderFromSubtree(Root->Right, Index, Value);

}

int TreeGetNthInorder(CC_TREE *Tree, int Index, int *Value)
{
	
	if (Tree == NULL || Value == NULL || Index<0 || (unsigned int)Index>Tree->Dimension - 1)
	{
		return CC_ERROR_INVALID_PARAMETER;
	}

	if (Tree->Dimension == 0)
	{
		return CC_EMPTY_TREE;
	}
	int *copyIndex = (int*)malloc(sizeof(int));
	*copyIndex = Index;

	TreeGetNthInorderFromSubtree(Tree->Root, copyIndex, Value);
	free(copyIndex);
	return CC_SUCCESS;
}

static void TreeGetNthPostorderFromSubtree(CC_TREE_NODE *Root, int *Index, int *Value)
{
	static int postorderCount = 0;
	if (Root == NULL)
	{
		return;
	}

	TreeGetNthPostorderFromSubtree(Root->Left, Index, Value);
	TreeGetNthPostorderFromSubtree(Root->Right, Index, Value);
	if ((*Index)==0) {
		*Value = Root->Value;
	}
	(*Index)--;
	

}

int TreeGetNthPostorder(CC_TREE *Tree, int Index, int *Value)
{
	if (Tree == NULL || Value == NULL || Index<0 || (unsigned int)Index>Tree->Dimension - 1)
	{
		return CC_ERROR_INVALID_PARAMETER;
	}

	if (Tree->Dimension == 0)
	{
		return CC_EMPTY_TREE;
	}
	int *copyIndex = (int*)malloc(sizeof(int));
	*copyIndex = Index;

	TreeGetNthPostorderFromSubtree(Tree->Root, copyIndex, Value);
	free(copyIndex);
	return CC_SUCCESS;
 
}

