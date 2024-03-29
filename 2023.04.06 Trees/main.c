
#include <malloc.h>
#include <stdio.h>
#include <strings.h>


const char NOTHING_TO_SEARCH = 0x0; /* 0b00000000 */
const char USE_VALUE = 0x1;         /* 0b00000001 (1 - use, 0 - do not use) */
const char DEPPEST = 0x2;           /* 0b00000010 (1 - lowerest/deepest, 0 - highest */
const char USE_DEPTH = 0x4;         /* 0b00000100 (1 - use, 0 - do not use) */
const char LEFT_RIGHT = 0x8;        /* 0b00001000 (1 - right child, 0 - left child */
const char USE_CHILD_NODE = 0x10;   /* 0b00010000 (1 - use node pointer as child, 0 - do not use */
const char USE_PARENT = 0x20;       /* 0b00100000 (1 - use node pointer as parent, 0 - do not use) */
const char WITHOUT_CHILD = 0x40;    /* 0b01000000 (1- search for node without child, 0 - do not use) */
const char WITH_CHILD = 0x80;       /* 0b10000000 (1 - search for node with child, 0 - do not use) */

struct node {
  struct node *left;
  struct node *right;
  int value;
};

typedef struct node nnode;

/*
 * allocate node with given value, left and right pointers will set on NULL
 * @param value int value to create node with
 * @return node allocated strcut node node with given value
 */
nnode *allocateNode(int value) {
  nnode *node = (nnode *)malloc(sizeof(nnode));
  node->value = value;
  node->left = NULL;
  node->right = NULL;
  return node;
}

/*
 * calculate tree heigth from given pointer
 * @param tree tree to calculate height of
 * @return tree height
*/
int nHeight(nnode *tree) {
  if (tree == NULL) return 0;
  int left = nHeight(tree->left);
  int right = nHeight(tree->right);
  return (left > right) ? left + 1 : right + 1;
}

/*
 * calculate node distance to nearest NULL child of this node
 * @param tree - subtree to begin search in
 * @param counter distance to nearest node, ! set 0 if starting at root
 * @return left:right distance to nearest NULL child of this node
 */
int nearestNULL(nnode *tree, int counter) {
  if (tree == NULL)
    return counter;
  int left = nearestNULL(tree->left, counter + 1);
  int right = nearestNULL(tree->right, counter + 1);
  return (left <= right) ? left : right;
}

/*
 * rightmost insert node into the tree with given value
 * @param tree nnode* tree to insert value in
 * @param value value to insert node in tree with
 * @return tree - ! assign return value only if youre creating tree, otherwise
 * it probably be subtree
 */
nnode *insert(nnode *tree, int value) {
  if (tree == NULL)
    return allocateNode(value);
  /* check if left tree is not null, if so, insert node at left */
  else if (tree->left == NULL) {
    tree->left = allocateNode(value);
    return tree;
  }
  /* check if right tree is not null, if so, insert node at right */
  else if (tree->right == NULL) {
    tree->right = allocateNode(value);
    return tree;
  }
  (nearestNULL(tree->left, 0) < nearestNULL(tree->right, 0))
      ? insert(tree->left, value)
      : insert(tree->right, value);
  return tree;
}

/*
 * rightmost insert array of values into the tree
 * @param tree nnode* tree to insert value in
 * @param array array to insert in tree
 * @param len lenght of array that will be inserted in tree
 * @return tree - ! assign return value only if youre creating tree, otherwise
 * it probably be subtree
 */
nnode *nInsertArray(nnode *tree, int *array, int len) {
  for (int i = 0; i < len; i++)
    if (tree == NULL)
      tree = insert(tree, array[i]);
    else
      insert(tree, array[i]);
  return tree;
}

nnode *min(nnode *tree) {
  if (tree != NULL) {

    nnode *minNode = tree;
    nnode *buffer = min(tree->left);
    if (buffer != NULL && minNode->value > buffer->value)
      minNode = buffer;
    buffer = min(tree->right);
    if (buffer != NULL && minNode->value > buffer->value)
      minNode = buffer;
    return minNode;

  } else
    return tree;
}

/*
 * find parent of given node
 * @param tree search parent of givent nodeToFind in that tree
 * @param nodeToFind parent of that node will be found if exists
 * @return tree parent of nodeToFInd
 */
nnode *nodeParent(nnode *tree, nnode *nodeToFind) {
  if (tree != NULL) {
    if (tree->left == nodeToFind || tree->right == nodeToFind)
      return tree;
    nnode *buffer = nodeParent(tree->left, nodeToFind);
    if (buffer != NULL &&
        (buffer->left == nodeToFind || buffer->right == nodeToFind))
      return buffer;
    buffer = nodeParent(tree->right, nodeToFind);
    if (buffer != NULL &&
        (buffer->left == nodeToFind || buffer->right == nodeToFind))
      return buffer;
    return NULL;
  } else
    return tree;
}

/*
 * remove lass node by given value from tree
 * @param tree tree to remove node from
 * @param value value of node to remove
 * @return tree - pointer to tree  
*/
nnode *nRemoveByValue(nnode *tree, int value) {
  // 1. find node with given value 
  nnode *nodePtr = nFindByValue(tree, value);

  // 2. find minimum value of it's subtrees 
  nnode *minNode = min(nodePtr->left);
  nnode *bufferNode = min(nodePtr->right);

  // 3. assign minimum value to minNode 
  if ( minNode != NULL && bufferNode != NULL) 
    minNode = (minNode->value < bufferNode->value) ? minNode : bufferNode;
  else if (minNode == NULL && bufferNode != NULL)
    minNode = bufferNode;
  else if ( minNode == NULL && bufferNode == NULL)
    minNode = nodePtr; 
  
  // 4.find parent of minNode
  if (minNode != NULL)
    bufferNode = nodeParent(tree, minNode);

  // 5. assign value of minimum node to current node 
  if ( nodePtr != minNode )
    nodePtr->value = minNode->value;

  // 6. free and delete pointer to buffer node ( can be current node or can be minimum node in subtrees of current node )
  if (bufferNode != NULL && bufferNode->left == minNode) {
    free(bufferNode->left);
    bufferNode->left = NULL;
  } 
  else if (bufferNode != NULL) {
    free(bufferNode->right);
    bufferNode->right = NULL;
  } 

  return tree;
}

/*
 * print binary tree in array look using pre     order traverse
 * @param tree tree to print
 */
void nPrintPreOrder(nnode *tree) {
  if (tree == NULL)
    return;
  nPrintPreOrder(tree->left);
  printf("%d ", tree->value);
  nPrintPreOrder(tree->right);
}

/*
 * print binary tree in array look using in order traverse
 * @param tree tree to print
 */
void nPrintInOrder(nnode *tree) {
  if (tree == NULL)
    return;
  printf("%d ", tree->value);
  nPrintInOrder(tree->left);
  nPrintInOrder(tree->right);
}

/*
 * print binary tree in array look using post order traverse
 * @param tree tree to print
 */
void nPrintPostOrder(nnode *tree) {
  if (tree == NULL)
    return;
  printf("%d ", tree->value);
  nPrintPostOrder(tree->right);
  nPrintPostOrder(tree->left);
}

/*
 * print binary tree in tree look
 * @param tree tree to print
 * @param space spacing between nodes in print (heigher spacing, taller the tree
 * will be)
 */
void nFancyPrint(nnode *tree, int space) {
  if (tree == NULL)
    return;
  space += 5;
  nFancyPrint(tree->right, space);
  printf("\n");
  for (int i = 5; i < space; i++) {
    printf(" ");
  }
  printf("[%d]<\n", tree->value);
  nFancyPrint(tree->left, space);
}

/*
 * calculate max sum of node values path to leaf node in tree 
 * @param tree tree to calculate path in 
 * @return max sum of node values path to leaf node in tree 
*/
int nCalculateMaxTreePath(nnode* tree) {
  if (tree == NULL) return 0;
  int left = nCalculateMaxTreePath(tree->left);
  int right = nCalculateMaxTreePath(tree->right);
  return tree->value + ((left > right) ? left : right);
}

/*
DFS З корня до нащадків:
      -> Нам потрібно зберігати поточний шлях ( елементи які ми відвідали )
      -> Нам потрібно зберігати значення суми ( після віднімання від неї значення вузла ) 
      -> Якщо ми вже дістались термінального вузла 
            -> Перевірити, чи сума == 0, ( якщо так, то ми знайшли потрібний шлях ) 
      -> Інакше викликати DFS на лівий та правий нащадок
*/
//int *nCalculateTreePath(nnode* tree, int sum) {
//  sum -= tree->value;
//  if (tree->left == NULL && tree->right == NULL && sum == 0) return path;
//  else if (sum != 0) { free(path); return NULL; }
//  else {
//    nCalculateMaxTreePath(tree->right, sum, path, index+1);
//    nCalculateMaxTreePath(tree->left, sum, path, index+1);
//  }
//}

/*
 * * * Algorythm * * * 
  Якщо обидва дерева порожні, то повернути 1
  Інакше - якщо обидва дерева не порожні
    –> Перевірити значення кореня (tree1->data == tree2->data)
    –> Рекурсивно перевірити ліві піддерева
    –> Рекурсивно перевірити праве піддерево
  Якщо вищенаведені три оператори істинні, то повернути 1
  Інакше повернути 0 (одне з них порожнє, а інше ні)
 * * * * * * * * * * * 
 * check if two trees are identical
 * @param tree1 first tree
 * @param tree2 second tree
 * @returns int 1 - identical, 0 - not identical
*/
int nisTreeIdentical(nnode* tree1, nnode* tree2) {
  if (!tree1 && !tree2) return 1;
  if (tree1 && tree2) {
    if (tree1->value == tree2->value
       && nisTreeIdentical(tree1->left, tree2->left)
       && nisTreeIdentical(tree1->right, tree2->right)) 
      return 1;
  }
  return 0;
}


void heightTest() {
  nnode *tree1 = NULL;
  int array[] =  {0,1,2,3,4,5,6,7,8,9,10,11,12,0,1,2,3,4,5,6,7,8,9,10,11,120,1,2,3,4,5,6,7,8,9,10,11,12,0,1,2,3,4,5,6,7,8,9,10,11,12};
  tree1 = nInsertArray(tree1, array, 13*4); 
  nFancyPrint(tree1, 1);
  printf(" Height of tree is %d\n", nHeight(tree1));

}

void testIsTreeIdentical() {
  nnode *tree1 = NULL;
  nnode *tree2 = NULL;
  int array[] =  {0,  1,  100,  3,  4,  5, 70};
  int array1[] = {0,  1,  100,  3,  1,  5, 70};
  tree1 = nInsertArray(tree1, array, 6); 
  tree2 = nInsertArray(tree2, array1, 6); 
  printf(" is trees identical? %s\n", nisTreeIdentical(tree1, tree2) ? "yes" : "no" );
}

void testInsert() {
  nnode *tree = NULL;
  int array[] = {0,  1,  2,  3,  4,  5,   6,     7,    8,   -100,
                 9,  10, 11, 12, 13, -90, 14,    15,   16,  17,
                 18, 19, 20, 21, 22, 23,  -2000, -900, -400};
  tree = nInsertArray(tree, array, 28);

  printf("Tree print: ");
  nFancyPrint(tree, 1);
  printf("\nTree traverses print: ");
  printf("\ntree pre order traversal:  ");
  nPrintPreOrder(tree);
  printf("\ntree in order traversal:   ");
  nPrintInOrder(tree);
  printf("\ntree post order traversal: ");
  nPrintPostOrder(tree);
  printf("\n node with value 3 [post order search]: %d",
  nFindByValue(tree, 3)->value);
  printf("\nminimum of the tree: %d\n", min(tree)->value);
  printf("\n parent of min value: %d",
  nodeParent(tree, min(tree))->value);
}

void testRemove() {
nnode *tree = NULL;
  int array[] = {1, 2, 3, 4, 5, 6};
  tree = nInsertArray(tree, array, 6); 

  // two child case
nRemoveByValue(tree, 3); 
  // one child case
nRemoveByValue(tree, 4);
  // terminal node case 
nRemoveByValue(tree, 5);
  // root case 
nRemoveByValue(tree, 1);

  printf("Tree print: ");
  nFancyPrint(tree, 1);
}

void testMaxPath() {
  nnode *tree = NULL;
  int array[] = {0,  1,  100,  3,  4, 5, 70};
  tree = nInsertArray(tree, array, 7);
  printf("max path: %d \n ", nCalculateMaxTreePath(tree));
  
  printf("Tree print: ");
  nFancyPrint(tree, 1);
}

int main() {
  //testInsert();
  //testRemove();
  //testMaxPath();
  testIsTreeIdentical();
  heightTest();
  return 0;
}