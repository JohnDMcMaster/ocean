/* SccsId = "@(#)BinaryTree.C 1.3 () 11/03/93"   -*- c++ -*- */
/**********************************************************

Name/Version      : binaryTree/1.3

Language          : C
Operating system  : UNIX SYSTEM V
Host machine      : any

Author(s)         : Viorica Simion
Creation date     : May 18, 1993
Modified by       :
Modification date :


        Delft University of Technology
        Department of Electrical Engineering
        Mekelweg 4 - P.O.Box 5031
        2600 GA DELFT
        The Netherlands

        Phone : 015 - 786202
        email : viorica@muresh.et.tudelft.nl

        COPYRIGHT (C) 1993 , All rights reserved
**********************************************************/
   

#include<stream.h>
#include<iostream.h>
#include<string.h>
#include<stdlib.h>           // abort ()
#include "BinaryTree.h"
#define NIL 0

// constraction of the BinaryNode
void BinaryNode::setNode ()
{
  left=right=p=NIL;
  key=NIL;
  treeMemberMode=notTreeMember;
}

// set the key
void BinaryNode::setKey (void* k)
{
   if ( treeMemberMode == notTreeMember )
      key=k;
   else
   {
      treeError("Attempt to change the key of a node in the tree.");
      abort();
   }
}

void BinaryNode::setLeft (BinaryNode* node)
{
   left=node;
}

void BinaryNode::setRight (BinaryNode* node)
{
   right=node;
}

void BinaryNode::setP (BinaryNode* node)
{
   p=node;
}

void BinaryNode::setMode (TreeMemberMode mode)
{
   treeMemberMode=mode;
}

// BinaryTree destructor
BinaryTree::~BinaryTree ()
{
   deleteTreeNode(root);
   delete nilNode;
}

// constraction of the NIL node in a BinaryTree:
void BinaryTree::setNil (BinaryNode* node)
{
   root=NIL;
   tree_size=NIL;
   nilNode=node;
}

void BinaryTree::setRoot (BinaryNode* node)
{
   root=node;
   root->treeMemberMode=treeMember;
}

// getNil returns the pointer to the NIL node of the tree
BinaryNode* BinaryTree::getNil ()
{
   return nilNode;
}

// getRoot returns the pointer to the key root of the tree
BinaryNode* BinaryTree::getRoot ()
{
   return root;
}

// set error pointer
void BinaryTree::setError (ErrorMode mode)
{
   error=mode;
}

// get error pointer
ErrorMode BinaryTree::getError ()
{
   return error;
}

// binaryTree error messeges
void treeError (const char* s)
{
   cout << flush;
   cerr << "\nInternal tree error: " << s << endl;
}

// delete tree
void BinaryTree::deleteTreeNode (BinaryNode* node)
{
   if ( node != nilNode )
   {
      deleteTreeNode (node->left);
      deleteTreeNode (node->right);
      delete node;
   }
}

// inorderTreeWalk prints out all the keys so that the root key
// is printed between the values in its left subtree and those in
// its right subtree
void BinaryTree::inorderTreeWalk (BinaryNode* node)
{
   if ( node != nilNode )
  {
     inorderTreeWalk (node->left);
     node->printKey(node->key);
     inorderTreeWalk (node->right);
  }
}

//treeSize(node) returns the size of the subtree
int BinaryTree::treeSize (BinaryNode* node)
{
   if ( (node == NIL) || (node->treeMemberMode == notTreeMember) 
	|| (node == nilNode) )
   {
      treeError("node does not exist");
      return subtree_size=0;  //error node does not exist
   }
   else 
   {
      subtree_size=0;
      subtreeSize(node);
      return subtree_size;
   }
}

//subtreeSize(node,int) returns the size of the subtree
void BinaryTree::subtreeSize (BinaryNode* node)
{
   if ( node != nilNode )
  {
     subtreeSize (node->left);
     subtree_size++;
     subtreeSize (node->right);
  }
}

// treeSize returns the size of the tree
int BinaryTree::treeSize ()
{
   return tree_size;
} 

// treeHeight returns the height of the tree 
int BinaryTree::treeHeight ()
{
   int pathCounter=NIL;
   maxHeight=NIL;
   getHeight (root, pathCounter);
   return maxHeight;
}

// treeHeight(node) returns the height of the subtree 
int BinaryTree::treeHeight (BinaryNode* node)
{
   if ( (node == NIL) || (node->treeMemberMode == notTreeMember) 
	|| (node == nilNode) )
   {
      treeError("node does not exist");
      return maxHeight=0;  //error node does not exist
   }
   else 
   {
      int pathCounter=NIL;
      maxHeight=NIL;
      getHeight (node, pathCounter);
      return maxHeight;
   }
}

void BinaryTree::getHeight (BinaryNode* node, int pathCounter)
{
   if ( node != nilNode )
   {
      ++pathCounter;
      getHeight (node->left, pathCounter);
      getHeight (node->right, pathCounter);
   }
   else
   {
      if ( pathCounter > maxHeight )
	 maxHeight=pathCounter;
   }
}

// given a tree and a pointer to a node with key k, treeSearch
// returns a pointer to a node with the key k if one exists

BinaryNode*  BinaryTree::search (BinaryNode* node, void* key)
{
   if ( node == nilNode )
      return NIL;
   if ( node->compare(key) == NIL ) 
      return node;
   if ( node->compare(key) > NIL )
     return search (node->left, key);
   else
      return search (node->right, key);      
}

BinaryNode* BinaryTree::treeSearch (BinaryNode* node)
{
   return search (root, node->key);
}

// treeMin returns a pointer to the minimum element in the
// subtree rooted at a given node "node"
BinaryNode* BinaryTree::treeMin (BinaryNode* node)
{
   if ( node->left == nilNode )
         return node;
   else 
         return treeMin (node->left);
}
       
// treeMax returns a pointer to the maximum element in the
// subtree rooted at a given node "node"
BinaryNode* BinaryTree::treeMax (BinaryNode* node)
{
   if ( node->right == nilNode )
         return node;
   else 
         return treeMax (node->right);
}

// treeSucc returns the successor of a node in a binary
// search tree if it exist, and NIL if the node has the
// largest key in the tree
BinaryNode* BinaryTree::treeSucc (BinaryNode* node)
{
   BinaryNode* ytemp;
   if ( node->right != nilNode )
      return treeMin (node->right);
   ytemp = node->p;
   while ( (ytemp != NIL) && (node == ytemp->right) )
   {
      node = ytemp;
      ytemp = ytemp->p;
   }
   return ytemp;
}

// treePred returns the predecessor of a node in a binary
// search tree if it exist, and NIL if the node has the
// smallest key in the tree
BinaryNode* BinaryTree::treePred (BinaryNode* node)
{
   BinaryNode* ytemp;
   if ( node->left != nilNode )   
      return treeMax (node->left);
   ytemp = node->p;
   while ( (ytemp != NIL) && (node == ytemp->left) )
   {
      node = ytemp;
      ytemp = ytemp->p;
   }
   return ytemp;
}

// treeInsert inserts a new value into a binary search tree
// in such a way that the tree property continues to hold
BinaryNode* BinaryTree::treeInsert (BinaryNode* node)
{
   if ( root == NIL)
   {
      root=node;
      root->left=nilNode; root->right=nilNode;
      root->treeMemberMode=treeMember;
      tree_size=1;
      return root;
   }
   else
   {
       return insert (root, node);
    }
}

BinaryNode* BinaryTree::insert (BinaryNode* node, BinaryNode* next)
{
   BinaryNode* ytemp;
   setError(nodeDoesNotExist);
   while ( node != nilNode )
   {
      ytemp=node;
      if ( node->compare(next->key) == NIL )
      {
	 treeError("node already exist");
	 setError(nodeExist);
         return next;          // error node next already exist
      }
      if ( node->compare(next->key) > NIL )
         node=node->left;
      else
         node=node->right;
   }
   next->p=ytemp;
   if ( ytemp->compare(next->key) > NIL )
      ytemp->left=next;
   else
      ytemp->right=next;
   next->left=next->right=nilNode;
   next->treeMemberMode=treeMember;
   ++tree_size;
   return next;
}

BinaryNode* BinaryTree::treeDelete (BinaryNode* node)
{
   BinaryNode *xtemp, *ytemp;
   if ( node->treeMemberMode == notTreeMember || node == nilNode ) 
   {
      treeError("node does not exist");
      return node;      // sorry node does not exist
   }
   if ( (node->left == nilNode) || (node->right == nilNode) )
      ytemp=node;
   else
      ytemp=treeSucc (node);
   if ( ytemp->left != nilNode )
      xtemp=ytemp->left;
   else
      xtemp=ytemp->right;
   if ( xtemp != nilNode )
      xtemp->p=ytemp->p;
   if ( ytemp->p == NIL )
      root=xtemp;
   else
   {
      if ( ytemp == ytemp->p->left )
         ytemp->p->left=xtemp;
      else
         ytemp->p->right=xtemp;
   }
   if ( ytemp != node )
   {
      ytemp->p=node->p;
      ytemp->left=node->left;
      ytemp->right=node->right;
      node->left->p=ytemp;
      node->right->p=ytemp;
      if ( node->p != NIL )
      {
	 if ( node == node->p->left )
	    node->p->left=ytemp;
	 else
	    node->p->right=ytemp;
      }
      else
	 root=ytemp; 
   }
   node->p=node->left=node->right=NIL;
   node->treeMemberMode=notTreeMember;
   --tree_size;
   return node;
}





