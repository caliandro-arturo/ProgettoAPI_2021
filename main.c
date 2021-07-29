/**
 * API project 2020-2021
 * Author: Arturo Caliandro
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

int graphDimension;
int rankingLength;
int actualRankingSize = 0;
int *lastRankingShot;
bool rankingChanged;
bool rankingLengthChanged;
int count = 0;                      //to maintain the count of iterations in recursive functions
/*unsigned long long*/ int worstMetricValue = 0;
int graphIndex = 0;
const /*unsigned long long*/ int INFINITY = -1;

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
    /*unsigned long long*/ int key;
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
 *
 */
rankingNode *createTreeNode(int graph, /*unsigned long long*/ int result) {
    rankingNode *newNode = malloc(sizeof(rankingNode));
    newNode->graphId = graph;
    newNode->key = result;
    newNode->isRed = false;
    return newNode;
}

/**
 * Returns the node containing the required key.
 * @param T the father to consider to do the research
 * @param key the value that identifies the searched node
 */
rankingNode *search(rankingNode *T, /*unsigned long long*/ int key) {
    if (T == treeNil || T->key == key)
        return T;
    else if (T->key < key)
        return search(T->right, key);
    else return search(T->left, key);
}

void printInOrder(rankingNode *subTree) {
    if (subTree->left != treeNil)
        printInOrder(subTree->left);
    lastRankingShot[count] = subTree->graphId;
    count++;
    if (subTree->right != treeNil)
        printInOrder(subTree->right);
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
 * Returns the node with the highest key, starting from a specified node.
 * @param subTree the treeRoot of the sub tree to use to start the research
 * @return the the node with the highest key in the sub tree
 */
rankingNode *maxNode(rankingNode *subTree) {
    rankingNode *current = subTree;
    while (current->right != treeNil)
        current = current->right;
    return current;
}

/**
 * Returns the node with the lowest key value that is higher than the node passed as parameter.
 * @param startNode the node to use as reference
 * @return the next node
 */
rankingNode *successor(rankingNode *startNode) {
    if (startNode->right != treeNil)
        return minNode(startNode->right);
    rankingNode *father = startNode->father;
    while (father != treeNil && startNode == father->right) {
        startNode = father;
        father = father->father;
    }
    return father;
}

/**
 * Returns the node with the highest key value that is lower than the node passed as parameter.
 * @param startNode the node to use as reference
 * @return the next node
 */
rankingNode *predecessor(rankingNode *startNode) {
    if (startNode->left != treeNil)
        return minNode(startNode->left);
    rankingNode *father = startNode->father;
    while (father != treeNil && startNode == father->left) {
        startNode = father;
        father = father->father;
    }
    return father;
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
    else if (toRotate == toRotate->father->right)
        toRotate->father->right = leftSon;
    else toRotate->father->left = leftSon;
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
        if (newNode->father == newNode->father->father->left) {
            newNodeUncle = newNode->father->father->right;
            if (newNodeUncle->isRed) {
                newNode->father->isRed = false;
                newNodeUncle->isRed = false;
                newNode->father->father->isRed = true;
                newNode = newNode->father->father;
            } else {
                if (newNode == newNode->father->right) {
                    newNode = newNode->father;
                    leftRotate(newNode);
                }
                newNode->father->isRed = false;
                newNode->father->father->isRed = true;
                rightRotate(newNode->father->father);
            }
        } else {
            newNodeUncle = newNode->father->father->left;
            if (newNodeUncle->isRed) {
                newNode->father->isRed = false;
                newNodeUncle->isRed = false;
                newNode->father->father->isRed = true;
                newNode = newNode->father->father;
            } else {
                if (newNode == newNode->father->left) {
                    newNode = newNode->father;
                    rightRotate(newNode);
                }
                newNode->father->isRed = false;
                newNode->father->father->isRed = true;
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
 * Auxiliary function that fixes the RB tree after a deletion of a node, if the the deleted node was black.
 * @param x the node that has taken the place of the old removed node
 */
void RBDeleteFixup(rankingNode *x) {
    rankingNode *w;
    while (x != treeRoot && !x->isRed) {
        if (x == x->father->left) {
            w = x->father->right;
            if (w->isRed) {
                w->isRed = false;
                x->father->isRed = true;
                leftRotate(x->father);
                w = x->father->right;
            }
            if (!w->left->isRed && !w->right->isRed) {
                w->isRed = true;
                x = x->father;
            } else {
                if (!w->right->isRed) {
                    w->left->isRed = false;
                    w->isRed = true;
                    rightRotate(w);
                    w = x->father->right;
                }
                w->isRed = x->father->isRed;
                x->father->isRed = false;
                w->right->isRed = false;
                leftRotate(x->father);
                x = treeRoot;
            }
        } else {
            w = x->father->left;
            if (w->isRed) {
                w->isRed = false;
                x->father->isRed = true;
                rightRotate(x->father);
                w = x->father->left;
            }
            if (!w->right->isRed && !w->left->isRed) {
                w->isRed = true;
                x = x->father;
            } else {
                if (!w->left->isRed) {
                    w->right->isRed = false;
                    w->isRed = true;
                    leftRotate(w);
                    w = x->father->left;
                }
                w->isRed = x->father->isRed;
                x->father->isRed = false;
                w->left->isRed = false;
                rightRotate(x->father);
                x = treeRoot;
            }
        }
    }
    x->isRed = false;
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
    subTree->father = toDelete->father;
    if (toDelete->father == treeNil)
        treeRoot = node;
    else {
        if (toDelete->father->left == toDelete)
            toDelete->father->left = subTree;
        else
            toDelete->father->right = subTree;
    }
    if (toDelete != node) {
        node->key = toDelete->key;
        node->graphId = toDelete->graphId;
    }
    if (!toDelete->isRed)
        RBDeleteFixup(subTree);
    free(toDelete);
}

// Fibonacci heap to optimize Dijkstra's algorithm
//----------------------------------------------------------------------------------------------------------------------

typedef struct heap_node {
    /*unsigned long long*/ int key;
    int graphNode;
    int degree;
    struct heap_node *leftSibling;
    struct heap_node *rightSibling;
    struct heap_node *parent;
    struct heap_node *child;
    bool mark;
    bool visited;
} heapNode;

/**
 * A representation of the NULL value for the heap. I know it's not mandatory, but it's to maintain consistency between
 * the heap and the RB tree implementation.
 */
/*heapNode *heapNil;*/

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
    H->min = /*heapNil*/ NULL;
    return H;
}

/**
 * Creates a node with the specified key as attribute.
 * @param value the key to use as value
 * @param graphNode the graph node index
 * @return a new node containing the indicated key
 */
heapNode *fibHeapCreateNode(/*unsigned long long*/ int value, int graphNode) {
    heapNode *newNode = (heapNode *) malloc(sizeof(heapNode));
    newNode->key = value;
    newNode->graphNode = graphNode;
    newNode->degree = 0;
    newNode->parent = /*heapNil*/ NULL;
    newNode->child = /*heapNil*/ NULL;
    newNode->leftSibling = newNode;
    newNode->rightSibling = newNode;
    newNode->mark = false;
    newNode->visited = false;
    return newNode;
}

/**
 * Inserts a newNode node into the heap.
 * @param H the fibonacci heap
 * @param newNode the node to add
 */
void fibHeapInsert(fibonacciHeap *H, heapNode *newNode) {
    if (H->min != /*heapNil*/ NULL) {
        //adds the new node to the root list
        H->min->leftSibling->rightSibling = newNode;
        newNode->rightSibling = H->min;
        newNode->leftSibling = H->min->leftSibling;
        H->min->leftSibling = newNode;
        if (newNode->key != INFINITY && newNode->key < H->min->key)
            H->min = newNode;
    } else
        H->min = newNode;
    H->n++;
}

/**
 * Removes a child from the parent child list and adds it to the parent root list.
 * @param child the child to insert into H root list
 * @param parent the parent of the child
 */
void fibHeapLink(heapNode *child, heapNode *parent) {
    // removes the child from the H root list
    child->rightSibling->leftSibling = child->leftSibling;
    child->leftSibling->rightSibling = child->rightSibling;
    child->leftSibling = child;
    child->rightSibling = child;
    child->parent = parent;
    if (parent->child == /*heapNil*/ NULL)
        parent->child = child;
    else {
        child->rightSibling = parent->child;
        child->leftSibling = parent->child->leftSibling;
        parent->child->leftSibling->rightSibling = child;
        parent->child->leftSibling = child;
        if (parent->child->key == INFINITY || child->key < parent->child->key)
            parent->child = child;
    }
    parent->degree++;
    child->mark = false;
}

/**
 * Calculates the integer part of log2(n) + 1.
 */
int calculateDegree(int n) {
    int degreeCount = 1;
    while (n > 0) {
        n = n / 2;
        degreeCount++;
    }
    return degreeCount;
}

/**
 * Consolidates the heap.
 * @param H the fibonacci Heap to consolidate
 */
void consolidate(fibonacciHeap *H) {
    int degree = calculateDegree(H->n);
    heapNode *A[degree];
    for (int i = 0; i < degree; i++) {
        A[i] = /*heapNil*/ NULL;
    }
    int tempDegree;
    heapNode *iterator = H->min;
    int hRootListCounter = 0;
    do {
        iterator->visited = true;
        hRootListCounter++;
        iterator = iterator->rightSibling;
    } while (iterator != H->min);
    do {
        if (!iterator->visited) {
            iterator = iterator->rightSibling;
            continue;
        }
        iterator->visited = false;
        tempDegree = iterator->degree;
        while (A[tempDegree] != /*heapNil*/ NULL) {
            heapNode *y = A[tempDegree];
            if (iterator->key == INFINITY || (y->key != INFINITY && iterator->key > y->key)) {
                heapNode *temp = iterator;
                iterator = y;
                y = temp;
            }
            fibHeapLink(y, iterator);
            A[tempDegree] = /*heapNil*/ NULL;
            tempDegree++;
        }
        A[tempDegree] = iterator;
        hRootListCounter--;
        iterator = iterator->rightSibling;
    } while (hRootListCounter > 0);
    H->min = /*heapNil*/ NULL;
    for (int i = 0; i < degree; i++) {
        if (A[i] != /*heapNil*/ NULL) {
            // adds A[i] to the H root list
            A[i]->leftSibling = A[i];
            A[i]->rightSibling = A[i];
            if (H->min == /*heapNil*/ NULL) {
                H->min = A[i];
            } else {
                H->min->leftSibling->rightSibling = A[i];
                A[i]->rightSibling = H->min;
                A[i]->leftSibling = H->min->leftSibling;
                H->min->leftSibling = A[i];
                if (H->min->key == INFINITY || A[i]->key < H->min->key) {
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
    if (minNode != /*heapNil*/ NULL) {
        if (minNode->child != /*heapNil*/ NULL) {
            // takes min node children out of the min child list to put them on the root list of H
            heapNode *childHead = minNode->child;
            heapNode *childTail = childHead->leftSibling;
            heapNode *oldTail = minNode->leftSibling;
            childTail->rightSibling = minNode;
            minNode->leftSibling = childTail;
            childHead->leftSibling = oldTail;
            oldTail->rightSibling = childHead;
            heapNode *iterator = childHead;
            while (iterator != minNode) {
                iterator->parent = NULL;
                iterator = iterator->rightSibling;
            }
            minNode->child = /*heapNil*/ NULL;
        }
        // removes the min node from the H root list
        minNode->leftSibling->rightSibling = minNode->rightSibling;
        minNode->rightSibling->leftSibling = minNode->leftSibling;
        if (minNode == minNode->rightSibling)
            H->min = /*heapNil*/ NULL;
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
        parent->child = /*heapNil*/ NULL;
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
    nodeToDecrease->parent = /*heapNil*/ NULL;
    nodeToDecrease->mark = false;
}

/**
 * Applies recursively the cut function to parent's children.
 * @param H the fibonacci Heap
 * @param parent the parent of the children to cut
 */
void cascadingCut(fibonacciHeap *H, heapNode *parent) {
    heapNode *grandparent;
    grandparent = parent->parent;
    if (grandparent != /*heapNil*/ NULL) {
        if (parent->mark == false) {
            parent->mark = true;
        } else {
            cut(H, parent, grandparent);
            cascadingCut(H, grandparent);
        }
    }
}

/**
 * Substitutes the key of the specified node with the specified new key value.
 * @param H the fibonacci heap
 * @param nodeToDecrease the node to change
 * @param newKey the new value of the key
 */
void fibHeapDecreaseKey(fibonacciHeap *H, heapNode *nodeToDecrease, /*unsigned long long*/ int newKey) {
    if (nodeToDecrease == /*heapNil*/ NULL) {
        /*printf("Node is not in the heap");*/
        return;
    }
    nodeToDecrease->key = newKey;
    heapNode *parentNode = nodeToDecrease->parent;
    if ((parentNode != /*heapNil*/ NULL) && (parentNode->key == INFINITY || nodeToDecrease->key < parentNode->key)) {
        cut(H, nodeToDecrease, parentNode);
        cascadingCut(H, parentNode);
    }
    if (H->min->key == INFINITY || nodeToDecrease->key < H->min->key) {
        H->min = nodeToDecrease;
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

// Ranking functions
//----------------------------------------------------------------------------------------------------------------------

void addGraphIntoRanking(int graphToAdd, /*unsigned long long*/ int result) {
    if (actualRankingSize == rankingLength) {
        if (result > worstMetricValue) {
            return;
        } else {
            rankingNode *max = maxNode(treeRoot);
            worstMetricValue = predecessor(max)->key;
            RBDelete(max);
        }
    }
    rankingChanged = true;
    if (result > worstMetricValue)
        worstMetricValue = result;
    rankingNode *newNode = createTreeNode(graphToAdd, result);
    RBInsert(newNode);
    if (actualRankingSize != rankingLength) {
        rankingLengthChanged = true;
        actualRankingSize++;
    }
}

// Add graph functions
//----------------------------------------------------------------------------------------------------------------------

void dijkstraFromZero(/*unsigned long*/ int adjacencyMap[][graphDimension],
        /*unsigned long long*/ int distance[graphDimension],
                                        int previous[graphDimension]) {
    fibonacciHeap *queue = makeFibHeap();
    /*unsigned long long*/ int temp;
    bool visited[graphDimension];
    for (int i = 0; i < graphDimension; i++) {
        visited[i] = false;
    }
    heapNode *nodes[graphDimension];
    distance[0] = 0;
    for (int i = 0; i < graphDimension; i++) {
        if (i != 0)
            distance[i] = INFINITY;
        else
            distance[i] = 0;
        previous[i] = -1;
        heapNode *newNode = fibHeapCreateNode(distance[i], i);
        fibHeapInsert(queue, newNode);
        nodes[i] = newNode;
    }
    while (queue->min != /*heapNil*/ NULL) {
        heapNode *current = fibHeapExtractMin(queue);
        if (current->key != INFINITY) {
            visited[current->graphNode] = true;
            for (int i = 0; i < graphDimension; i++) {
                if (current->graphNode != i && adjacencyMap[current->graphNode][i] != 0) {
                    temp = distance[current->graphNode] + adjacencyMap[current->graphNode][i];
                    if (distance[i] == INFINITY || distance[i] > temp) {
                        distance[i] = temp;
                        previous[i] = current->graphNode;
                        if (!visited[i])
                            fibHeapDecreaseKey(queue, nodes[i], temp);
                    }
                }
            }
        }
        free(current);
    }
    for (int i = 0; i < graphDimension; i++) {
        if (distance[i] == INFINITY)
            distance[i] = 0;
    }
    free(queue);
}

/*unsigned long long*/ int fromDijkstraToMetric(const /*unsigned long long*/ int distance[graphDimension]) {
    /*unsigned long long*/ int totalDistance = 0;
    for (int i = 1; i < graphDimension; ++i) {
        totalDistance += distance[i];
    }
    return totalDistance;
}

/*unsigned long long*/ int calcGraphMetric(/*unsigned long*/ int adjacencyMap[][graphDimension]) {
    /*unsigned long long*/ int distance[graphDimension];
    int previous[graphDimension];
    dijkstraFromZero(adjacencyMap, distance, previous);
    return fromDijkstraToMetric(distance);
}

//
//----------------------------------------------------------------------------------------------------------------------
/**
 * Parse a string array containing digits and returns the
 * @param number the char array that contains digits
 * @return the number contained in the array, as an integer
 */
/*unsigned long*/ int parseInt(const char *number) {
    return strtoul(number, NULL, 10);
}

/**
 * Parse the next edge and adds weights into the array passed as parameter.
 * @param vertices the array in which to put weights
 */
void parseNextEdge(/*unsigned long*/ int vertices[]) {
    int insertedNumbersPointer = 0;
    int insertedDigitsPointer = 0;
    char numberDigits[11];
    char nextDigit;
    do {
        nextDigit = (char) getchar_unlocked();
        if (nextDigit == ',' || nextDigit == '\r' || nextDigit == '\n') {
            numberDigits[insertedDigitsPointer] = '\0';
            vertices[insertedNumbersPointer] = parseInt(numberDigits);
            insertedDigitsPointer = 0;
            insertedNumbersPointer++;
            if (nextDigit == '\r' || nextDigit == '\n') {
                if (nextDigit == '\r')
                    getchar_unlocked();
                return;
            }
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
    /*unsigned long*/ int adjacencyMap[graphDimension][graphDimension];
    int i = 0;
    for (; i < graphDimension; i++) {
        parseNextEdge(adjacencyMap[i]);
    }
    /*unsigned long long*/ int result = calcGraphMetric(adjacencyMap);
    /*printf("\nThe sum of shortest paths is: %llu\n\n", result);*/
    addGraphIntoRanking(graphIndex, result);
    graphIndex++;
}

/**
 * Prints out the ranking.
 */
void printRanking() {
    if (rankingLength == 0)
        return;
    if (rankingChanged) {
        if (rankingLengthChanged) {
            lastRankingShot = (int *) realloc(lastRankingShot, actualRankingSize * sizeof(int));
            rankingLengthChanged = false;
        }
        count = 0;
        printInOrder(treeRoot);
        rankingChanged = false;
    }
    for (int i = 0; i <= actualRankingSize - 1; ++i) {
        printf("%d", lastRankingShot[i]);
        if (i < actualRankingSize - 1)
            printf(" ");
    }
    putc('\n', stdout);
}

/**
 * Initializes the program, setting dimension of graphs to classify and length of the ranking.
 */
void initialize() {
    char *initialization;
    char *next;
    int charDim;
    size_t maxDim = 23;
    initialization = malloc(sizeof(char) * maxDim);
    charDim = getline(&initialization, &maxDim, stdin);
    if (charDim != 0) {
        graphDimension = strtoull(initialization, &next, 10);
        rankingLength = strtoull(next, NULL, 10);
    }
    free(initialization);
}

/**
 * Reads commands from input.
 */
void inputHandler() {
    char *command;
    int charDim;
    size_t argumentDim = 15;
    command = malloc(sizeof(char) * argumentDim);
    do {
        command[0] = '\0';
        charDim = getline(&command, &argumentDim, stdin);
        command[strcspn(command, "\n")] = '\0';
        if (!strcmp("AggiungiGrafo", command)) {
            analyzeGraph();
        } else if (!strcmp("TopK", command))
            printRanking();
        else if (charDim == -1) break;
    } while (strcmp("", command) != 0);
    /*printf("Closing the program...");*/
    free(command);
}

int main() {
    /*heapNil = fibHeapCreateNode(0, 0);
    heapNil->child = heapNil*/
    setbuf(stdout, NULL);
    rankingChanged = false;
    rankingLengthChanged = false;
    lastRankingShot = malloc(sizeof(int));
    treeNil = malloc(sizeof(rankingNode));
    treeNil->graphId = -1;
    treeNil->key = -1;
    treeNil->father = treeNil;
    treeNil->left = treeNil;
    treeNil->right = treeNil;
    treeNil->isRed = false;
    treeRoot = treeNil;
    initialize();
    inputHandler();
    /*free(heapNil);*/
    free(treeNil);
    free(lastRankingShot);
    return 0;
}
