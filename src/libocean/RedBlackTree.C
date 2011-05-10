/* SccsId = "@(#)RedBlackTree.C 1.2 () 11/03/93"   -*- c++ -*- */
/**********************************************************

Name/Version      : redBlackTree/1.2

Language          : C
Operating system  : UNIX SYSTEM V
Host machine      : any

Author(s)         : Viorica Simion
Creation date     : Jul 4, 1993
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
   
#include "RedBlackTree.h"
#define NIL 0

RedBlackNode::RedBlackNode () 
{
   BinaryNode::setNode();
   color=Black;
}

RedBlackNode* RedBlackNode::left ()
{
   return (RedBlackNode*)BinaryNode::getLeft ();
}

void RedBlackNode::left (RedBlackNode* node)
{
   BinaryNode::setLeft ((RedBlackNode*)node);
}

RedBlackNode* RedBlackNode::right ()
{
   return (RedBlackNode*)BinaryNode::getRight ();
}

void RedBlackNode::right (RedBlackNode* node)
{
   BinaryNode::setRight ((RedBlackNode*)node);
}

RedBlackNode* RedBlackNode::p ()
{
   return (RedBlackNode*)BinaryNode::getP ();
}

void RedBlackNode::p (RedBlackNode* node)
{
   BinaryNode::setP ((RedBlackNode*)node);
}

RedBlackTree::RedBlackTree(RedBlackNode* node):BinaryTree((RedBlackNode*)node)
{
}

void RedBlackTree::setNil (RedBlackNode* node)
{
   BinaryTree::setNil ((RedBlackNode*)node);
   getNil()->color=Black;
}

RedBlackNode* RedBlackTree::getNil ()
{
   return (RedBlackNode*)BinaryTree::getNil ();
}

void RedBlackTree::setRoot (RedBlackNode* node)
{
   BinaryTree::setRoot ((RedBlackNode*)node);
}

RedBlackNode* RedBlackTree::getRoot ()
{
   return (RedBlackNode*)BinaryTree::getRoot ();
}

void RedBlackTree::inorderTreeWalk (RedBlackNode* node)
{
   BinaryTree::inorderTreeWalk ((RedBlackNode*)node);
}

void RedBlackTree::deleteTreeNode (RedBlackNode* node)
{
   BinaryTree::deleteTreeNode ((RedBlackNode*)node);
}

RedBlackNode* RedBlackTree::treeSearch (RedBlackNode* node)
{
   return (RedBlackNode*)BinaryTree::treeSearch ((RedBlackNode*)node);
}

RedBlackNode* RedBlackTree::treeMin (RedBlackNode* node)
{
   return (RedBlackNode*)BinaryTree::treeMin ((RedBlackNode*)node);
}

RedBlackNode* RedBlackTree::treeMax (RedBlackNode* node)
{
   return (RedBlackNode*)BinaryTree::treeMax ((RedBlackNode*)node);
}

RedBlackNode* RedBlackTree::treeSucc (RedBlackNode* node)
{
   return (RedBlackNode*)BinaryTree::treeSucc ((RedBlackNode*)node);
}

RedBlackNode* RedBlackTree::treePred (RedBlackNode* node)
{
   return (RedBlackNode*)BinaryTree::treePred ((RedBlackNode*)node);
}

int RedBlackTree::treeSize ()
{
   return BinaryTree::treeSize ();
}

int RedBlackTree::treeSize (RedBlackNode* node)
{
   return BinaryTree::treeSize ((RedBlackNode*)node);
}

int RedBlackTree::treeHeight ()
{
   return BinaryTree::treeHeight ();
}

int RedBlackTree::treeHeight (RedBlackNode* node)
{
   return BinaryTree::treeHeight ((RedBlackNode*)node);
}

void RedBlackTree::leftRotate (RedBlackNode* node)
{
   RedBlackNode* rbtemp;
   rbtemp=node->right();
   node->right(rbtemp->left());
   if ( rbtemp->left() != getNil() )
      rbtemp->left()->p(node);
   rbtemp->p(node->p());
   if ( node->p() == NIL )
      setRoot(rbtemp);
   else
   {
      if ( node == node->p()->left() )
         node->p()->left(rbtemp);
      else
         node->p()->right(rbtemp);
   }
   rbtemp->left(node);
   node->p(rbtemp);
}

void RedBlackTree::rightRotate (RedBlackNode* node)
{
   RedBlackNode* rbtemp;
   rbtemp=node->left();
   node->left(rbtemp->right());
   if ( rbtemp->right() != getNil() )
      rbtemp->right()->p(node);
   rbtemp->p(node->p());
   if ( node->p() == NIL )
      setRoot(rbtemp);
   else
   {
      if ( node == node->p()->left() )
         node->p()->left(rbtemp);
      else
         node->p()->right(rbtemp);
   }  
   rbtemp->right(node);
   node->p(rbtemp);
}

RedBlackNode* RedBlackTree::insert (RedBlackNode* node, RedBlackNode* next)
{
   return (RedBlackNode*)BinaryTree::insert ((RedBlackNode*)node, 
                                             (RedBlackNode*)next);
}

RedBlackNode* RedBlackTree::rbInsert (RedBlackNode* node)
{
   if ( getRoot() == NIL)
   {
      setRoot(node);
      getRoot()->left(getNil());
      getRoot()->right(getNil());
      getRoot()->color=Black;
      getRoot()->setMode(treeMember);
      tree_size=1;
      return getRoot();
   }
   else
   {
      RedBlackNode *xtemp, *ytemp;
      xtemp=insert(getRoot(),node);
      if ( getError() == nodeExist )
         return node;
      xtemp->color=Red;
      while ( (xtemp != getRoot()) && (xtemp->p()->color == Red) )
      {
         if ( xtemp->p() == xtemp->p()->p()->left() )
         {
            ytemp=xtemp->p()->p()->right();
            if ( ytemp->color == Red )
            {
               xtemp->p()->color=Black;
               ytemp->color=Black;
               xtemp->p()->p()->color=Red;
               xtemp=xtemp->p()->p();
	    }
            else
            {
               if ( xtemp == xtemp->p()->right() )
               {
                  xtemp=xtemp->p();
                  leftRotate(xtemp);
	       }
               xtemp->p()->color=Black;
               xtemp->p()->p()->color=Red;
               rightRotate(xtemp->p()->p());
	    }
	 }
         else
         {
            ytemp=xtemp->p()->p()->left();
            if ( ytemp->color == Red )
            {
               xtemp->p()->color=Black;
               ytemp->color=Black;
               xtemp->p()->p()->color=Red;
               xtemp=xtemp->p()->p();
	    }
            else
            {
               if ( xtemp == xtemp->p()->left() )      
               {
                  xtemp=xtemp->p();
                  rightRotate(xtemp);
	       }       
               xtemp->p()->color=Black;
               xtemp->p()->p()->color=Red;
               leftRotate(xtemp->p()->p());
	    }
         }
      }
      getRoot()->color=Black;
      return node;
   }
}

RedBlackNode* RedBlackTree::rbDelete (RedBlackNode* node)
{
   RedBlackNode* xtemp, *ytemp;
   Color inColor;
   if ( node->getMode() == notTreeMember )
   {
      treeError("node does not exist");
      return node;      // sorry node does not exist
   }
   if ( (node->left() == getNil()) || (node->right() == getNil()) )
      ytemp=node;
   else
      ytemp=treeSucc (node);
   inColor=ytemp->color;
   if ( ytemp->left() != getNil() )
      xtemp=ytemp->left();
   else
      xtemp=ytemp->right();
   xtemp->p(ytemp->p());
   if ( ytemp->p() == NIL )
      setRoot(xtemp);
   else
   {
      if ( ytemp == ytemp->p()->left() )
         ytemp->p()->left(xtemp);
      else
         ytemp->p()->right(xtemp);
   }
   if ( ytemp != node )
   {
      ytemp->p(node->p());
      ytemp->left(node->left());
      ytemp->right(node->right());
      ytemp->color=node->color;
      if ( node->p() != NIL )
      {
	 if ( node == node->p()->left() )
	    node->p()->left(ytemp);
	 else
	    node->p()->right(ytemp);
      }
      else
	 setRoot(ytemp); 
   node->left()->p(ytemp);
   node->right()->p(ytemp);
   }
   if ( inColor == Black )
      rbDeleteFixup (xtemp);
   node->p(NIL); node->left(NIL); node->right(NIL);
   node->setMode(notTreeMember);
   --tree_size;
   xtemp->p(NIL);
   return node;
}

void RedBlackTree::rbDeleteFixup (RedBlackNode* node)
{
   RedBlackNode* xtemp;
   while ( (node != getRoot()) && (node->color == Black) )
   {
      if ( node == node->p()->left() )
      {
	 xtemp=node->p()->right();
	 if ( xtemp->color == Red )
	 {
	    xtemp->color=Black;
	    node->p()->color=Red;
	    leftRotate(node->p());
	    xtemp=node->p()->right();
	 }
	 if ( (xtemp->left()->color == Black) && 
                    (xtemp->right()->color == Black) )
	 {
	    xtemp->color=Red;
	    node=node->p();
	 }
	 else
	 {
	    if ( xtemp->right()->color == Black )
	    {
	       xtemp->left()->color=Black;
	       xtemp->color=Red;
	       rightRotate(xtemp);
	       xtemp=node->p()->right();
	    }
	    xtemp->color=node->p()->color;
	    node->p()->color=Black;
	    xtemp->right()->color=Black;
	    leftRotate(node->p());
	    node=getRoot();
	 }
      }
      else
      {
	 xtemp=node->p()->left();
	 if ( xtemp->color == Red )
	 {
	    xtemp->color=Black;
	    node->p()->color=Red;
	    rightRotate(node->p());
	    xtemp=node->p()->left();
	 }
	 if ( (xtemp->right()->color == Black) && 
                    (xtemp->left()->color == Black) )
	 {
	    xtemp->color=Red;
	    node=node->p();
	 }
	 else
	 {
	    if ( xtemp->left()->color == Black )
	    {
	       xtemp->right()->color=Black;
	       xtemp->color=Red;
	       leftRotate(xtemp);
	       xtemp=node->p()->left();
	    }
	    xtemp->color=node->p()->color;
	    node->p()->color=Black;
	    xtemp->left()->color=Black;
	    rightRotate(node->p());
	    node=getRoot();
	 }
      }
   }
   node->color=Black;
}

      

