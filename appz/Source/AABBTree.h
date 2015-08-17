#pragma once
#include "AABBTreeLeaf.h"

class AABBTree
{
public:
	AABBTree(const unsigned& size = 0);
	~AABBTree();
	AABBTreeNode* GetBegin();
	AABBTreeNode* GetEnd();
	void IncreaseCapacityTo(const unsigned& size);
	void Sort(const AABBBox& box, const unsigned& size);
	void GetContacts(AABBTree* tree, Contact* buffer, Contact** end);
	void GetContacts(Contact** iterator);
private:
	unsigned capacity;
	unsigned size;
	AABBTreeNode* nodes;

	AABBTreeLeaf mainLeaf;
};