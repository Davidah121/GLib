#pragma once
#include "BinaryTree.h"

namespace glib
{
	template<typename T>
	struct RBNode
	{
		bool color = false; //false == black, true == red
		T data;
	};

	template<typename T>
	class BinarySearchTree
	{
	public:
		static const int NORMAL = 0;
		static const int RED_BLACK = 1;
		
		/**
		 * @brief Construct a new Binary Search Tree object
		 * 
		 * @param type 
		 * 		Specifies the type of Binary Search tree to use.
		 * 		Either a normal binary search tree can be used with no balancing or
		 * 		a self balancing tree can be used using red-black trees.
		 * 		With self balancing, searching is guaranteed to be O(LogN)
		 */
		BinarySearchTree(int type);
		~BinarySearchTree();
		
		BinaryTreeNode<RBNode<T>>* add(T data);
		void remove(T data);
		void remove(BinaryTreeNode<RBNode<T>>* node);
		BinaryTreeNode<RBNode<T>>* search(T data);
		BinaryTree<RBNode<T>>* getTree();

		size_t size();
		int getType();
	private:
		static const bool COLOR_BLACK = false;
		static const bool COLOR_RED = true;

		BinaryTree<RBNode<T>> tree;
		void redBlackReBalance(BinaryTreeNode<RBNode<T>>* n);
		void redBlackDeleteFix(BinaryTreeNode<RBNode<T>>* node);
		

		int type = 0;
		size_t count = 0;
	};

	template<typename T>
	inline BinarySearchTree<T>::BinarySearchTree(int type)
	{
		this->type = type;
	}

	template<typename T>
	inline BinarySearchTree<T>::~BinarySearchTree()
	{
		//tree will automatically be deleted.
	}

	template<typename T>
	inline BinaryTree<RBNode<T>>* BinarySearchTree<T>::getTree()
	{
		return &tree;
	}

	template<typename T>
	inline size_t BinarySearchTree<T>::size()
	{
		return count;
	}

	template<typename T>
	inline int BinarySearchTree<T>::getType()
	{
		return type;
	}

	template<typename T>
	inline BinaryTreeNode<RBNode<T>>* BinarySearchTree<T>::search(T data)
	{
		BinaryTreeNode<RBNode<T>>* node = tree.getRoot();
		while(node != nullptr)
		{
			if(node->data.data == data)
				return node;
			else if(data < node->data.data)
				node = node->leftChild;
			else
				node = node->rightChild;
		}
		return nullptr;
	}

	template<typename T>
	inline BinaryTreeNode<RBNode<T>>* BinarySearchTree<T>::add(T data)
	{
		//find place to insert it at.
		count++;

		BinaryTreeNode<RBNode<T>>* selectedNode = tree.getRoot();
		if(selectedNode == nullptr)
		{
			BinaryTreeNode<RBNode<T>>* p = new BinaryTreeNode<RBNode<T>>();
			//Set the RedBlackNode stuff
			p->data.color = COLOR_BLACK;
			p->data.data = data;

			//Set the BinaryTree stuff
			p->leftChild = nullptr;
			p->rightChild = nullptr;
			tree.setRootNode( p );
			return p;
		}

		int insertMode = 0; //0=do nothing, 1=left, 2=right
		while(selectedNode != nullptr)
		{
			if(data < selectedNode->data.data)
			{
				//go left
				if(selectedNode->leftChild == nullptr)
				{
					insertMode = 1;
					break;
				}
				selectedNode = selectedNode->leftChild;
			}
			else if(data == selectedNode->data.data)
			{
				//do nothing
				insertMode = 0;
				break;
			}
			else
			{
				//go right
				if(selectedNode->rightChild == nullptr)
				{
					insertMode = 2;
					break;
				}
				selectedNode = selectedNode->rightChild;
			}
		}

		BinaryTreeNode<RBNode<T>>* p = nullptr;
		if(insertMode == 0)
		{
			count--;
			return nullptr; //No insertion
		}
		else if(insertMode == 1)
		{
			//insert left
			p = new BinaryTreeNode<RBNode<T>>();
			//Set the RedBlackNode stuff
			p->data.color = COLOR_RED;
			p->data.data = data;

			//Set the BinaryTree stuff
			p->leftChild = nullptr;
			p->rightChild = nullptr;
			tree.setLeftNode(selectedNode, p);
		}
		else
		{
			//go right
			p = new BinaryTreeNode<RBNode<T>>();
			//Set the RedBlackNode stuff
			p->data.color = COLOR_RED;
			p->data.data = data;

			//Set the BinaryTree stuff
			p->leftChild = nullptr;
			p->rightChild = nullptr;
			tree.setRightNode(selectedNode, p);
		}
		
		if(type == RED_BLACK)
		{
			redBlackReBalance(p); //Height can change here
			
			if(tree.getRoot() != nullptr)
				tree.getRoot()->data.color = COLOR_BLACK;
		}

		return p;
	}

	template<typename T>
	inline void BinarySearchTree<T>::remove(T data)
	{
		//find the data
		BinaryTreeNode<RBNode<T>>* currNode = search(data);
		remove(currNode);
	}

	template<typename T>
	inline void BinarySearchTree<T>::remove(BinaryTreeNode<RBNode<T>>* n)
	{
		if(n != nullptr)
		{
			BinaryTreeNode<RBNode<T>>* parentNode = n->parent;
			BinaryTreeNode<RBNode<T>>* newNode = nullptr;
			bool origColor = n->data.color;

			if(n->leftChild == nullptr && n->rightChild == nullptr)
			{
				//do normal delete
				//case 1
				if(parentNode != nullptr)
				{
					newNode = parentNode;
					if(parentNode->leftChild == n)
						tree.setLeftNode(parentNode, nullptr);
					else
						tree.setRightNode(parentNode, nullptr);
				}
				else
				{
					//deleting root
					tree.setRootNode(nullptr);
				}
			}
			else
			{
				if(n->leftChild != nullptr && n->rightChild != nullptr)
				{
					//case 3. Find successor
					//go right then left as much as possible
					BinaryTreeNode<RBNode<T>>* successor = n->rightChild;
					
					while(true)
					{
						if(successor->leftChild != nullptr)
							successor = successor->leftChild;
						else
							break;
					}

					//swap n with successor then delete n
					tree.swapNodes(n, successor);

					parentNode = n->parent;
					if(parentNode != nullptr)
					{
						if(parentNode->leftChild == n)
							tree.setLeftNode(parentNode, nullptr);
						else
							tree.setRightNode(parentNode, nullptr);
					}
					
				}
				else
				{
					//case 2
					BinaryTreeNode<RBNode<T>>* child = n->leftChild;
					if(child == nullptr)
						child = n->rightChild;
					
					//replace n with child
					if(parentNode != nullptr)
					{
						if(parentNode->leftChild == n)
						{
							parentNode->leftChild = child;
							child->parent = parentNode;
						}
						else
						{
							parentNode->rightChild = child;
							child->parent = parentNode;
						}
					}
					else
					{
						//is root
						child->parent = nullptr;
						tree.setRootNode(child);
					}
					newNode = child;
					
					delete n;
				}
			}

			if(origColor == COLOR_BLACK)
				redBlackDeleteFix(newNode);
		}
	}

	template<typename T>
	inline void BinarySearchTree<T>::redBlackReBalance(BinaryTreeNode<RBNode<T>>* n)
	{
		//get parent, grandparent, uncle
		if(n != nullptr)
		{
			BinaryTreeNode<RBNode<T>>* parent = nullptr;
			BinaryTreeNode<RBNode<T>>* grandparent = nullptr;
			BinaryTreeNode<RBNode<T>>* uncle = nullptr;

			bool pColor = COLOR_BLACK;
			bool gpColor = COLOR_BLACK;
			bool uColor = COLOR_BLACK;
			
			
			parent = n->parent;
			if(parent != nullptr)
			{
				pColor = parent->data.color;
				grandparent = parent->parent;

				if(parent->data.color == COLOR_BLACK)
				{
					return; //No need to continue
				}
			}
			else
			{
				return; //No need to continue
			}

			if(grandparent != nullptr)
			{
				gpColor = grandparent->data.color;
				if(grandparent->leftChild == parent)
					uncle = grandparent->rightChild;
				else
					uncle = grandparent->leftChild;
				
				if(uncle != nullptr)
					uColor = uncle->data.color;
			}
			
			//do rotations including uncle
			if(uColor == COLOR_RED)
			{
				//case 1 and 4.
				grandparent->data.color = COLOR_RED;
				parent->data.color = COLOR_BLACK;
				uncle->data.color = COLOR_BLACK;

				//set n to grandparent
				redBlackReBalance(grandparent); //May be a nullptr
			}
			else
			{
				if(grandparent == nullptr)
				{
					//can't do anything
					return;
				}

				if(grandparent->leftChild == parent)
				{
					if(parent->rightChild == n)
					{
						//may be incorrect
						tree.leftRotate(parent); //rotate about parent. parent is now child of n. grandparent is the same

						n->data.color = COLOR_BLACK;
						grandparent->data.color = COLOR_RED;
						tree.rightRotate(grandparent); //rotate about grandparent. Makes n the parent of both of them
					}
					else
					{
						parent->data.color = COLOR_BLACK;
						grandparent->data.color = COLOR_RED;
						tree.rightRotate(grandparent);
					}
				}
				else
				{
					if(parent->leftChild == n)
					{
						//may be incorrect
						tree.rightRotate(parent);
						n->data.color = COLOR_BLACK;
						grandparent->data.color = COLOR_RED;
						tree.leftRotate(grandparent);
					}
					else
					{
						parent->data.color = COLOR_BLACK;
						grandparent->data.color = COLOR_RED;
						tree.leftRotate(grandparent);
					}
				}
			}

		}
	}

	template<typename T>
	inline void BinarySearchTree<T>::redBlackDeleteFix(BinaryTreeNode<RBNode<T>>* x)
	{
		if(x != nullptr)
		{
			BinaryTreeNode<RBNode<T>>* n = x;
			
			while(n->data.color == COLOR_BLACK)
			{
				BinaryTreeNode<RBNode<T>>* p = n->parent;
				BinaryTreeNode<RBNode<T>>* sib = nullptr;
				bool nColor, pColor, sibColor;
				if(p != nullptr)
					sib = (p->leftChild == n) ? p->rightChild : p->leftChild;
				else
					break; //Is root
				
				nColor = n->data.color;
				pColor = p->data.color;
				sibColor = COLOR_BLACK;
				if(sib != nullptr)
					sibColor = sib->data.color;
				
				if(sibColor == COLOR_RED)
				{
					//type 1
					sib->data.color = COLOR_BLACK;
					p->data.color = COLOR_RED;
					tree.leftRotate(p);
					sib = (p->leftChild == n) ? p->rightChild : p->leftChild;
				}
				else if(sibColor == COLOR_BLACK)
				{
					bool sibL = COLOR_BLACK;
					bool sibR = COLOR_BLACK;
					
					if(sib != nullptr)
					{
						//Setting colors to avoid additional nullptr checks
						if(sib->leftChild != nullptr)
							sibL = sib->leftChild->data.color;
						
						if(sib->rightChild != nullptr)
							sibR = sib->rightChild->data.color;
					}

					if(sibL == COLOR_BLACK && sibR == COLOR_BLACK)
					{
						//type 2
						if(sib != nullptr)
							sib->data.color = COLOR_RED;
						n = p;
					}
					else 
					{
						if(sibR == COLOR_BLACK)
						{
							//type 3
							sib->leftChild->data.color = COLOR_BLACK; //Left child is red so it exist
							sib->data.color = COLOR_RED;
							tree.rightRotate(sib);
							sib = p->rightChild;
						}

						//type 4
						sib->data.color = p->data.color;
						p->data.color = COLOR_BLACK;

						if(sib->rightChild != nullptr)
							sib->rightChild->data.color = COLOR_BLACK; //Right child may not exist still

						tree.leftRotate(p);
						n = tree.getRoot();
					}
				}
			}

			n->data.color = COLOR_BLACK;
		}
	}

}