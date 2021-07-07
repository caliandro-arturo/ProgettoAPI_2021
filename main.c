/**
 * API project 2020-2021
 * Author: Arturo Caliandro
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#define INFINITY -2

int graphDimension;
int rankingLength;
int actualRankingSize = 0;
int graphIndex = 0;

/**
 * Returns the lowest value.
 * @param val1 the first value
 * @param val2 the second value
 * @return the lowest value
 */
int minInt(int val1, int val2) {
    if (val1 <= val2)
        return val1;
    else return val2;
}

/**
 * Returns the higher value.
 * @param val1 the first value
 * @param val2 the second value
 * @return the higher value
 */
int maxInt(int val1, int val2) {
    if (val1 >= val2)
        return val1;
    else return val2;
}

// RB Tree for ranking
//----------------------------------------------------------------------------------------------------------------------
/**
 * Node struct for RB tree implementation for ranking.
 */
typedef struct treeNode {
    int graphId;
    unsigned long long int key;
    bool isRed;
    struct treeNode *father;
    struct treeNode *left;
    struct treeNode *right;
} rankingNode;

/*typedef struct {
    int *outGoingStar;
} graphNode;*/

/*typedef struct {
    int id;
} graph;*/

/**
 * Node that represents a null value.
 */
rankingNode *treeNil;
/**
 * Node that represents the treeRoot of the ranking tree.
 */
rankingNode *treeRoot;

/**
 * Returns the node containing the required key.
 * @param T the father to consider to do the research
 * @param key the value that identifies the searched node
 */
rankingNode *search(rankingNode *T, int key) {
    if (T == treeNil || T->key == key)
        return T;
    else if (T->key < key)
        return search(T->right, key);
    else return search(T->left, key);
}

/**
 * Returns the node with the lowest key, starting from a specified node.
 * @param subTree the treeRoot of the sub tree to use to start the research
 * @return the the node with the lowest key in the sub tree
 */
rankingNode *minNode(rankingNode *subTree) {
    rankingNode *current = subTree;
    while (current->left != treeNil)
        current = current->left;
    return current;
}

/**
 * Returns the node with the lowest key value that is higher than the node passed as parameter.
 * @param startNode the node to use as reference
 * @return the next node
 */
rankingNode *successor(rankingNode *startNode) {
    if (startNode == treeNil)
        return treeNil;
    if (startNode->right != treeNil)
        return minNode(startNode->right);
    rankingNode *startNodeFather = startNode->father;
    while (startNodeFather != treeNil && startNodeFather->right == startNode) {
        startNode = startNodeFather;
        startNodeFather = startNodeFather->father;
    }
    return startNodeFather;
}

/**
 * Applies a counterclockwise rotation between a node and his right son.
 * The son becomes the father of the node, and the node becomes his left son.
 * @param toRotate the father
 */
void leftRotate(rankingNode *toRotate) {
    rankingNode *rightSon = toRotate->right;
    toRotate->right = rightSon->left;
    if (rightSon->left != treeNil)
        rightSon->left->father = toRotate;
    rightSon->father = toRotate->father;
    if (toRotate->father == treeNil)
        treeRoot = rightSon;
    else if (toRotate == toRotate->father->left)
        toRotate->father->left = rightSon;
    else toRotate->father->right = rightSon;
    rightSon->left = toRotate;
    toRotate->father = rightSon;
}

/**
 * Applies a clockwise rotation between a node and his left son.
 * The son becomes the father of the node, and the node becomes his right son.
 * @param toRotate the father
 */
void rightRotate(rankingNode *toRotate) {
    rankingNode *leftSon = toRotate->left;
    toRotate->left = leftSon->right;
    if (leftSon->right != treeNil)
        leftSon->right->father = toRotate;
    leftSon->father = toRotate->father;
    if (toRotate->father == treeNil)
        treeRoot = leftSon;
    else if (toRotate == toRotate->father->left)
        toRotate->father->left = leftSon;
    else toRotate->father->right = leftSon;
    leftSon->right = toRotate;
    toRotate->father = leftSon;
}

/**
 * Fixes the RB Tree property after the specified node has been inserted.
 * @param newNode the new node added into the tree
 */
void RBInsertFixup(rankingNode *newNode) {
    while (newNode != treeRoot && newNode->father->isRed) {
        rankingNode *newNodeUncle;
        if (newNode->father == newNode->father->father->left)
            newNodeUncle = newNode->father->father->right;
        else
            newNodeUncle = newNode->father->father->left;
        if (newNodeUncle->isRed) {
            newNodeUncle->isRed = false;
            newNode->father->isRed = false;
            newNode->father->father->isRed = true;
            newNode = newNode->father->father;
        } else {
            if (newNode->father == newNode->father->father->left &&
                newNode == newNode->father->left) {
                bool color = newNode->father->isRed;
                newNode->father->isRed = newNode->father->father->isRed;
                newNode->father->father->isRed = color;
                rightRotate(newNode->father->father);
            }
            if (newNode->father == newNode->father->father->left &&
                newNode == newNode->father->right) {
                bool color = newNode->isRed;
                newNode->isRed = newNode->father->father->isRed;
                newNode->father->father->isRed = color;
                leftRotate(newNode->father);
                rightRotate(newNode->father->father);
            }
            if (newNode->father == newNode->father->father->right &&
                newNode == newNode->father->right) {
                bool color = newNode->father->isRed;
                newNode->father->isRed = newNode->father->father->isRed;
                newNode->father->father->isRed = color;
                leftRotate(newNode->father->father);
            }
            if (newNode->father == newNode->father->father->right &&
                newNode == newNode->father->left) {
                bool color = newNode->isRed;
                newNode->isRed = newNode->father->father->isRed;
                newNode->father->father->isRed = color;
                rightRotate(newNode->father);
                leftRotate(newNode->father->father);
            }
        }
    }
    /* treeRoot must always be black */
    treeRoot->isRed = false;
}

/**
 * Inserts a new node inside the RB Tree.
 * @param toInsert the node to add
 */
void RBInsert(rankingNode *toInsert) {
    rankingNode *previousNode = treeNil;
    rankingNode *currentNode = treeRoot;
    while (currentNode != treeNil) {
        previousNode = currentNode;
        if (toInsert->key < currentNode->key)
            currentNode = currentNode->left;
        else currentNode = currentNode->right;
    }
    toInsert->father = previousNode;
    if (previousNode == treeNil)
        treeRoot = toInsert;
    else if (toInsert->key < previousNode->key)
        previousNode->left = toInsert;
    else previousNode->right = toInsert;
    toInsert->left = treeNil;
    toInsert->right = treeNil;
    toInsert->isRed = true;
    RBInsertFixup(toInsert);
}

/**
 * Auxiliary method that fixes the RB tree after a deletion of a node, if the the deleted node was black.
 * @param substitute the node that has taken the place of the old removed node
 */
void RBDeleteFixup(rankingNode *substitute) {
    rankingNode *w;
    while (substitute != treeRoot && !substitute->isRed) {
        if (substitute == substitute->father->left) {
            w = substitute->father->right;
            if (w->isRed) {
                w->isRed = false;
                substitute->father->isRed = true;
                leftRotate(substitute->father);
                w = substitute->father->right;
            }
            if (!w->left->isRed && !w->right->isRed) {
                w->isRed = true;
                substitute = substitute->father;
            } else if (!w->right->isRed) {
                w->left->isRed = false;
                w->isRed = true;
                rightRotate(w);
                w = substitute->father->right;
            }
            w->isRed = substitute->father->isRed;
            substitute->father->isRed = false;
            w->right->isRed = false;
            leftRotate(substitute->father);
            substitute = treeRoot;
        } else {
            w = substitute->father->left;
            if (w->isRed) {
                w->isRed = false;
                substitute->father->isRed = true;
                rightRotate(substitute->father);
                w = substitute->father->left;
            }
            if (!w->right->isRed && !w->left->isRed) {
                w->isRed = true;
                substitute = substitute->father;
            } else if (!w->left->isRed) {
                w->right->isRed = false;
                w->isRed = true;
                leftRotate(w);
                w = substitute->father->left;
            }
            w->isRed = substitute->father->isRed;
            substitute->father->isRed = false;
            w->left->isRed = false;
            rightRotate(substitute->father);
            substitute = treeRoot;
        }
    }
    substitute->isRed = false;
}

/**
 * Removes a node from the RB Tree.
 * @param node the node to remove
 */
void RBDelete(rankingNode *node) {
    rankingNode *toDelete, *subTree;
    if (node->left == treeNil || node->right == treeNil)
        toDelete = node;
    else toDelete = successor(node);
    if (toDelete->left != treeNil)
        subTree = toDelete->left;
    else subTree = toDelete->right;
    if (subTree != treeNil)
        subTree->father = toDelete->father;
    if (toDelete->father == treeNil)
        treeRoot = subTree;
    else if (toDelete == toDelete->father->left)
        toDelete->father->left = subTree;
    else toDelete->father->right = subTree;
    if (toDelete != node) {
        node->key = toDelete->key;
    }
    free(toDelete);
    if (!node->isRed)
        RBDeleteFixup(node);
}

// Graph metric calculation system
//----------------------------------------------------------------------------------------------------------------------

typedef struct heap_node {
    int key;
    int degree;
    struct heap_node *leftSibling;
    struct heap_node *rightSibling;
    struct heap_node *parent;
    struct heap_node *child;
    bool mark;
} heapNode;

/**
 * A representation of the NULL value for the heap. I know it's not mandatory, but it's to maintain consistency between
 * the heap and the RB tree implementation.
 */
heapNode *heapNil;

typedef struct fibonacci_heap {
    int n;              // number of nodes contained in the heap
    heapNode *min;      // the node with the minimum key
} fibonacciHeap;

/**
 * Creates a fibonacci Heap.
 * @return a new empty fibonacci Heap
 */
fibonacciHeap *makeFibHeap() {
    fibonacciHeap *H;
    H = (fibonacciHeap *) malloc(sizeof(fibonacciHeap));
    H->n = 0;
    H->min = heapNil;
    return H;
}

/**
 * Creates a node with the specified key as attribute.
 * @param value the key to use as value
 * @return a new node containing the indicated key
 */
heapNode *fibHeapCreateNode(int value) {
    heapNode *newNode = (heapNode *) malloc(sizeof(heapNode));
    newNode->key = value;
    newNode->degree = 0;
    newNode->parent = heapNil;
    newNode->child = heapNil;
    newNode->leftSibling = newNode;
    newNode->rightSibling = newNode;
    newNode->mark = false;
    return newNode;
}

/**
 * Inserts a newNode node into the heap.
 * @param H the fibonacci heap
 * @param newNode the node to add
 */
void fibHeapInsert(fibonacciHeap *H, heapNode *newNode) {
    if (H->min != heapNil) {
        //adds the new node to the root list
        H->min->leftSibling->rightSibling = newNode;
        newNode->rightSibling = H->min;
        newNode->leftSibling = H->min->leftSibling;
        H->min->leftSibling = newNode;
        if (newNode->key < H->min->key)
            H->min = newNode;
    } else
        H->min = newNode;
    H->n++;
}

/**
 * Merges two heaps into a new single heap, and frees them.
 * @param H1 the first heap
 * @param H2 the second heap
 * @return the merge result
 */
fibonacciHeap *fibHeapUnion(fibonacciHeap *H1, fibonacciHeap *H2) {
    if (H1 != NULL) {
        if (H1->min == heapNil) {
            free(H1);
            H1 = NULL;
        }
    }
    if (H2 != NULL) {
        if (H2->min == heapNil) {
            free(H2);
            H2 = NULL;
        }
    }
    if (H1 == NULL || H2 == NULL) {
        if (H1 == NULL && H2 == NULL)
            return makeFibHeap();
        else if (H1 == NULL)
            return H2;
        else return H1;
    }
    // at this point, H1 and H2 must contain at least one node.
    fibonacciHeap *H = makeFibHeap();
    H->min = H1->min;
    H->min->leftSibling->rightSibling = H2->min;
    H2->min->leftSibling = H->min->leftSibling;
    H->min->leftSibling = H2->min;
    H2->min->leftSibling->rightSibling = H->min;
    if (H1->min == heapNil || (H2->min != heapNil && H2->min->key < H1->min->key))
        H->min = H2->min;
    H->n = H1->n + H2->n;
    free(H1);
    free(H2);
    return H;
}

/**
 * Removes a child from the parent child list and adds it to the H root list.
 * @param H the fibonacci Heap
 * @param child the child to insert into H root list
 * @param parent the parent of the child
 */
void fibHeapLink(fibonacciHeap *H, heapNode *child, heapNode *parent) {
    // removes the child from the H root list
    child->rightSibling->leftSibling = child->leftSibling;
    child->leftSibling->rightSibling = child->rightSibling;
    child->leftSibling = child;
    child->rightSibling = child;
    child->parent = parent;
    if (parent->child == heapNil)
        parent->child = child;
    else {
        child->rightSibling = parent->child;
        child->leftSibling = parent->child->leftSibling;
        parent->child->leftSibling->rightSibling = child;
        parent->child->leftSibling = child;
        if (child->key < parent->child->key)
            parent->child = child;
    }
    parent->degree++;
    child->mark = false;
}

/**
 * Calculates the integer part of log2(n) + 1.
 */
int calculateDegree(int n) {
    int count = 0;
    while (n > 0) {
        n = n / 2;
        count++;
    }
    return count;
}

/**
 * Consolidates the heap.
 * @param H the fibonacci Heap to consolidate
 */
void consolidate(fibonacciHeap *H) {
    int degree = calculateDegree(H->n);
    heapNode *A[degree];
    for (int i = 0; i < degree; i++) {
        A[i] = heapNil;
    }
    int tempDegree;
    heapNode *iterator = H->min;
    do {
        tempDegree = iterator->degree;
        while (A[tempDegree] != heapNil) {
            heapNode *y = A[tempDegree];
            if (iterator->key > y->key) {
                heapNode *temp = iterator;
                iterator = y;
                y = temp;
            }
            fibHeapLink(H, y, iterator);
            A[tempDegree] = heapNil;
            tempDegree++;
        }
        A[tempDegree] = iterator;
        iterator = iterator->rightSibling;
    } while (iterator != H->min);
    H->min = heapNil;
    for (int i = 0; i < degree; i++) {
        if (A[i] != heapNil) {
            // adds A[i] to the H root list
            A[i]->leftSibling = A[i];
            A[i]->rightSibling = A[i];
            if (H->min == heapNil) {
                H->min = A[i];
            } else {
                H->min->leftSibling->rightSibling = A[i];
                A[i]->rightSibling = H->min;
                A[i]->leftSibling = H->min->leftSibling;
                H->min->leftSibling = A[i];
                if (A[i]->key < H->min->key) {
                    H->min = A[i];
                }
            }
        }
    }
}

/**
 * Returns and remove the min node.
 * @param H the fibonacci Heap
 * @return the minimum node
 */
heapNode *fibHeapExtractMin(fibonacciHeap *H) {
    heapNode *minNode = H->min;
    if (minNode != heapNil) {
        if (minNode->child != heapNil) {
            // takes min node children out of the min child list to put them on the root list of H
            heapNode *childList = minNode->child;
            while (minNode->degree == 0) {
                minNode->degree--;
                heapNode *nodeToTakeOut = childList->rightSibling;
                nodeToTakeOut->parent = heapNil;
                nodeToTakeOut->leftSibling->rightSibling = nodeToTakeOut->rightSibling;
                nodeToTakeOut->rightSibling->leftSibling = nodeToTakeOut->leftSibling;
                nodeToTakeOut->leftSibling = minNode->rightSibling;
                minNode->leftSibling->rightSibling = nodeToTakeOut;
                nodeToTakeOut->rightSibling = minNode;
                minNode->leftSibling = nodeToTakeOut;
            }
            minNode->child = heapNil;
        }
        // removes the min node from the H root list
        minNode->leftSibling->rightSibling = minNode->rightSibling;
        minNode->rightSibling->leftSibling = minNode->leftSibling;
        if (minNode == minNode->rightSibling)
            H->min = heapNil;
        else {
            H->min = minNode->rightSibling;
            consolidate(H);
        }
        H->n--;
    }
    return minNode;
}

/**
 * Subtracts the node to decrease from the parent child list and puts it into the H root list.
 * @param H the fibonacci heap
 * @param nodeToDecrease the node to add to the H root list
 * @param parent the parent of the node to decrease
 */
void cut(fibonacciHeap *H, heapNode *nodeToDecrease, heapNode *parent) {
    if (nodeToDecrease == nodeToDecrease->rightSibling)
        parent->child = heapNil;
    nodeToDecrease->leftSibling->rightSibling = nodeToDecrease->rightSibling;
    nodeToDecrease->rightSibling->leftSibling = nodeToDecrease->leftSibling;
    if (nodeToDecrease == parent->child)
        parent->child = nodeToDecrease->rightSibling;
    parent->degree--;
    nodeToDecrease->leftSibling = nodeToDecrease;
    nodeToDecrease->rightSibling = nodeToDecrease;
    H->min->leftSibling->rightSibling = nodeToDecrease;
    nodeToDecrease->rightSibling = H->min;
    nodeToDecrease->leftSibling = H->min->leftSibling;
    H->min->leftSibling = nodeToDecrease;
    nodeToDecrease->parent = heapNil;
    nodeToDecrease->mark = false;
}

/**
 * Applies recursively the cut function to parent's children.
 * @param H the fibonacci Heap
 * @param parent the parent of the children to cut
 */
void cascadingCut(fibonacciHeap *H, heapNode *parent) {
    heapNode *aux;
    aux = parent->parent;
    if (aux != heapNil) {
        if (parent->mark == false) {
            parent->mark = true;
        } else {
            cut(H, parent, aux);
            cascadingCut(H, aux);
        }
    }
}

/**
 * Substitutes the key of the specified node with the specified new key value.
 * @param H the fibonacci heap
 * @param nodeToDecrease the node to change
 * @param newKey the new value of the key
 */
void fibHeapDecreaseKey(fibonacciHeap *H, heapNode *nodeToDecrease, int newKey) {
    if (nodeToDecrease == heapNil) {
        /*printf("Node is not in the heap");*/
        return;
    } else {
        if (nodeToDecrease->key < newKey) {
            /*printf("n Invalid new key for decrease key operation n ");*/
            return;
        } else {
            nodeToDecrease->key = newKey;
            heapNode *parentNode = nodeToDecrease->parent;
            if ((parentNode != heapNil) && (nodeToDecrease->key < parentNode->key)) {
                cut(H, nodeToDecrease, parentNode);
                cascadingCut(H, parentNode);
            }
            if (nodeToDecrease->key < H->min->key) {
                H->min = nodeToDecrease;
            }
        }
    }
}

/**
 * Deletes the specified node from the heap.
 * @param H the fibonacci Heap
 * @param toDelete the node to delete
 */
void deleteNode(fibonacciHeap *H, heapNode *toDelete) {
    fibHeapDecreaseKey(H, toDelete, -5);    // -5 represents -inf, it is used to set the node as min
    fibHeapExtractMin(H);
}

//
//----------------------------------------------------------------------------------------------------------------------

void dijkstraQueue(unsigned long int adjacencyMap[][graphDimension]) {
    fibonacciHeap *queue = makeFibHeap();
    unsigned long long int distance[graphDimension - 1];
    unsigned int previous[graphDimension - 1];
    for (int i = 0; i < graphDimension - 1; i++) {
        distance[i] = INFINITY;
        previous[i] = -1;
        heapNode *newNode = fibHeapCreateNode(INFINITY);
        fibHeapInsert(queue, newNode);
    }
    while (queue->min == heapNil) {
        heapNode *current = fibHeapExtractMin(queue);
        // other code here, starting from successor
    }
}

unsigned long long int calcGraphMetric(unsigned long int adjacencyMap[][graphDimension]) {
    return 0;
}

//
//----------------------------------------------------------------------------------------------------------------------
/**
 * Parse a string array containing digits and returns the
 * @param number the char array that contains digits
 * @return the number contained in the array, as an integer
 */
unsigned long int parseInt(const char *number) {
    return strtoul(number, NULL, 10);
}

/**
 * Parse the next edge and adds weights into the array passed as parameter.
 * @param vertices the array in which to put weights
 */
void parseNextEdge(unsigned long int vertices[]) {
    int insertedNumbersPointer = 0;
    int insertedDigitsPointer = 0;
    char numberDigits[10];
    char nextDigit;
    do {
        nextDigit = (char) getchar();
        if (nextDigit == ',' || nextDigit == '\n') {
            vertices[insertedNumbersPointer] = parseInt(numberDigits);
            insertedDigitsPointer = 0;
            if (nextDigit == '\n')
                return;
        } else if (nextDigit != ' ') {
            numberDigits[insertedDigitsPointer] = nextDigit;
            insertedDigitsPointer++;
        }
    } while (1);
}

/**
 * Reads a graph.
 */
void analyzeGraph() {
    /*printf("I should parse a graph here\n");*/
    unsigned long int adjacencyMap[graphDimension][graphDimension];
    int i = 0;
    for (; i < graphDimension; i++) {
        parseNextEdge(adjacencyMap[i]);
    }
    calcGraphMetric(adjacencyMap);
}

/**
 * Prints out the ranking.
 */
void printRanking() {
    /*printf("I should print the ranking here\n");*/
}

/**
 * Initializes the program, setting dimension of graphs to classify and length of the ranking.
 */
void initialize() {
    scanf("%d %d", &graphDimension, &rankingLength);
    /* removes the '\n' from the buffer. */
    getc(stdin);
}

/**
 * Reads commands from input.
 */
void inputHandler() {
    char command[15];
    do {
        fgets(command, 15, stdin);
        command[strcspn(command, "\n")] = '\0';
        if (!strcmp("AggiungiGrafo", command)) {
            analyzeGraph();
            graphIndex++;
        } else if (!strcmp("TopK", command))
            printRanking();
    } while (strcmp("", command) != 0);
    /*printf("Closing the program...");*/
}

int main() {
    setbuf(stdout, NULL);
    heapNil = fibHeapCreateNode(-1);
    heapNil->child = heapNil;
    initialize();
    inputHandler();
    return 0;
}
