/**
 * API project 2020-2021
 * Author: Arturo Caliandro
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

int graphDimension;
int rankLength;
int actualRankSize = 0;
int *lastRankShot;
bool rankChanged;
bool rankLengthChanged;
char *nextLine;
size_t maxLineSize;
int **adjacencyMap;
int *distance;

int count = 0;                      //to maintain the count of iterations in recursive functions
int worstMetricValue = 0;
int graphIndex = 0;
const int INFINITY = -1;

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

// RB Tree for rank
//----------------------------------------------------------------------------------------------------------------------
/**
 * Node struct for RB tree implementation for rank.
 */
typedef struct treeNode {
    int graphId;
    int key;
    bool isRed;
    struct treeNode *father;
    struct treeNode *left;
    struct treeNode *right;
} rankNode;

/*typedef struct {
    int *outGoingStar;
} graphNode;*/

/*typedef struct {
    int id;
} graph;*/

/**
 * Node that represents a null value.
 */
rankNode *treeNil;
/**
 * Node that represents the treeRoot of the rank tree.
 */
rankNode *treeRoot;

/**
 *
 */
rankNode *createTreeNode(int graph, /*unsigned long long*/ int result) {
    rankNode *newNode = malloc(sizeof(rankNode));
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
rankNode *search(rankNode *T, /*unsigned long long*/ int key) {
    if (T == treeNil || T->key == key)
        return T;
    else if (T->key < key)
        return search(T->right, key);
    else return search(T->left, key);
}

void printInOrder(rankNode *subTree) {
    if (subTree->left != treeNil)
        printInOrder(subTree->left);
    printf("%d", subTree->graphId);
    count++;
    if (count != actualRankSize)
        fputc(' ', stdout);
    else return;
    if (subTree->right != treeNil)
        printInOrder(subTree->right);
}

/**
 * Returns the node with the lowest key, starting from a specified node.
 * @param subTree the treeRoot of the sub tree to use to start the research
 * @return the the node with the lowest key in the sub tree
 */
rankNode *minNode(rankNode *subTree) {
    rankNode *current = subTree;
    while (current->left != treeNil)
        current = current->left;
    return current;
}

/**
 * Returns the node with the highest key, starting from a specified node.
 * @param subTree the treeRoot of the sub tree to use to start the research
 * @return the the node with the highest key in the sub tree
 */
rankNode *maxNode(rankNode *subTree) {
    rankNode *current = subTree;
    while (current->right != treeNil)
        current = current->right;
    return current;
}

/**
 * Returns the node with the lowest key value that is higher than the node passed as parameter.
 * @param startNode the node to use as reference
 * @return the next node
 */
rankNode *successor(rankNode *startNode) {
    if (startNode->right != treeNil)
        return minNode(startNode->right);
    rankNode *father = startNode->father;
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
rankNode *predecessor(rankNode *startNode) {
    if (startNode->left != treeNil)
        return minNode(startNode->left);
    rankNode *father = startNode->father;
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
void leftRotate(rankNode *toRotate) {
    rankNode *rightSon = toRotate->right;
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
void rightRotate(rankNode *toRotate) {
    rankNode *leftSon = toRotate->left;
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
void RBInsertFixup(rankNode *newNode) {
    while (newNode != treeRoot && newNode->father->isRed) {
        rankNode *newNodeUncle;
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
void RBInsert(rankNode *toInsert) {
    rankNode *previousNode = treeNil;
    rankNode *currentNode = treeRoot;
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
void RBDeleteFixup(rankNode *x) {
    rankNode *w;
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
void RBDelete(rankNode *node) {
    rankNode *toDelete, *subTree;
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

// Binary heap to optimize Dijkstra's algorithm
//----------------------------------------------------------------------------------------------------------------------

/**
 * Representation of an heap node
 */
typedef struct heap_node {
    /**
     * the current calculated distance from the node 0. It is set by default to INFINITY, if the node id is != 0
     */
    int dist;
    /**
     * the identifier of the node
     */
    int vertexId;
    /**
     * an auxiliary boolean to check if the node key has to be increased or not
     */
    bool visited;
} heapNode;

/**
 * Creates a node that can be put into the heap.
 * @param dist the current distance from the node 0
 * @param vertexId the number of the vertex
 * @return a pointer to an heap node
 */
heapNode *createHeapNode(int dist, int vertexId) {
    heapNode *newNode = malloc(sizeof(heapNode));
    newNode->dist = dist;
    newNode->vertexId = vertexId;
    newNode->visited = false;
    return newNode;
}

/**
 * Representation of the heap
 */
typedef struct {
    /**
     * the heap array, with nodes
     */
    heapNode **nodes;
    /**
     * an array that tracks the current position of vertices into the heap
     */
    int *position;
    /**
     * the current allocated memory for the heap
     */
    int length;
    /**
     * the current number of elements into the heap
     */
    int heapSize;
} heap;

heap *binHeap;
heapNode **nodesArray;

/**
 * Finds the parent of a node.
 * @param i the index of the son
 * @return the index of the parent
 */
int parent(int i) {
    return i >> 1;
}

/**
 * Finds the left son.
 * @param i the index of the parent
 * @return the index of the left son
 */
int left(int i) {
    return i << 1;
}

/**
 * Finds the right son.
 * @param i the index of the parent
 * @return the index of the right son
 */
int right(int i) {
    return (i << 1) + 1;
}

/**
 * Turns the heap array into a correct heap.
 * @param H the heap
 * @param i an auxiliary index used for recursion
 */
void heapify(heap *H, int i) {
    int l = left(i);
    int r = right(i);
    int smallest;
    if (l <= H->heapSize && H->nodes[l]->dist != INFINITY &&
        (H->nodes[i]->dist == INFINITY || H->nodes[l]->dist < H->nodes[i]->dist))
        smallest = l;
    else smallest = i;
    if (r <= H->heapSize && H->nodes[r]->dist != INFINITY &&
        (H->nodes[smallest]->dist == INFINITY || H->nodes[r]->dist < H->nodes[smallest]->dist))
        smallest = r;
    if (smallest != i) {
        heapNode *temp = H->nodes[i];
        H->nodes[i] = H->nodes[smallest];
        H->nodes[smallest] = temp;
        H->position[smallest] = i;
        H->position[i] = smallest;
        heapify(H, smallest);
    }
}

/**
 * Builds an heap.
 * @param nodes the array of nodes to put into the heap
 * @param len the size of the allocated memory for the heap array
 * @param heapSize the number of nodes into the heap array
 * @return the built heap
 */
heap *buildHeap(heapNode **nodes, int len, int heapSize) {
    heap *H = (heap *) malloc(sizeof(heap));
    H->nodes = nodes;
    H->length = len;
    H->heapSize = heapSize;
    for (int i = heapSize >> 1; i >= 1; ++i) {
        heapify(H, i);
    }
    return H;
}

void heapsort(heap *H) {
    for (int i = H->length; i >= 2; ++i) {
        heapNode *temp = H->nodes[1];
        H->nodes[1] = H->nodes[i];
        H->nodes[i] = temp;
        H->heapSize--;
        heapify(H, 1);
    }
}

/**
 * Extracts the node with the minimum value of `dist` from the heap.
 * @param H the heap
 * @return the root
 */
heapNode *heapExtractMin(heap *H) {
    if (H->heapSize < 1)
        return NULL;
    heapNode *min = H->nodes[1];
    H->nodes[1] = H->nodes[H->heapSize];
    H->position[H->nodes[1]->vertexId] = 1;
    H->heapSize--;
    heapify(H, 1);
    return min;
}

/**
 * Inserts a node into the heap.
 * @param H the heap
 * @param toInsert the node to insert
 */
void heapInsert(heap *H, heapNode *toInsert) {
    H->heapSize++;
    int i = H->heapSize;
    while (i > 1 && toInsert->dist != INFINITY &&
           (H->nodes[parent(i)]->dist == INFINITY || H->nodes[parent(i)]->dist > toInsert->dist)) {
        H->nodes[i] = H->nodes[parent(i)];
        H->position[H->nodes[i]->vertexId] = i;
        i = parent(i);
    }
    H->nodes[i] = toInsert;
    H->position[toInsert->vertexId] = i;
}

/**
 * Retrains the specified node into the heap.
 * @param H the heap
 * @param vertexId the id of the node to retrain
 */
void heapDecreaseKey(heap *H, int vertexId) {
    int i = H->position[vertexId];
    while (i > 1 && H->nodes[i]->dist != INFINITY &&
           (H->nodes[parent(i)]->dist == INFINITY || H->nodes[i]->dist < H->nodes[parent(i)]->dist)) {
        H->position[H->nodes[i]->vertexId] = parent(i);
        H->position[H->nodes[parent(i)]->vertexId] = i;
        heapNode *temp = H->nodes[i];
        H->nodes[i] = H->nodes[parent(i)];
        H->nodes[parent(i)] = temp;
        i = parent(i);
    }
}

// Rank functions
//----------------------------------------------------------------------------------------------------------------------

/**
 * Adds the graph into the rank, if there is space for it.
 * @param graphToAdd the id of the graph
 * @param result the result of the metric of the graph
 */
void addGraphIntoRank(int graphToAdd, int result) {
    if (actualRankSize == rankLength) {
        if (result >= worstMetricValue) {
            return;
        } else {
            rankNode *max = maxNode(treeRoot);
            worstMetricValue = predecessor(max)->key;
            RBDelete(max);
        }
    }
    rankChanged = true;
    if (result > worstMetricValue)
        worstMetricValue = result;
    rankNode *newNode = createTreeNode(graphToAdd, result);
    RBInsert(newNode);
    if (actualRankSize != rankLength) {
        rankLengthChanged = true;
        actualRankSize++;
    }
}

// Add graph functions
//----------------------------------------------------------------------------------------------------------------------

/**
 * Calculates the shortest paths from a specified node to all the other vertices.
 * @param sourceId the initial vertex
 * @param adjacencyMap the adjacency map containing the weights of arcs between the vertices
 * @param distance the array in which distances of vertices from the source vertex are reported
 */
void dijkstra(int sourceId) {
    int temp;
    for (int i = 0; i < graphDimension; i++) {
        if (i != sourceId)
            distance[i] = INFINITY;
        else
            distance[sourceId] = 0;
    }
    for (int i = 0; i < graphDimension; i++)
        heapInsert(binHeap, nodesArray[i]);
    while (binHeap->heapSize != 0) {
        heapNode *current = heapExtractMin(binHeap);
        if (current->dist != INFINITY) {
            current->visited = true;
            for (int i = 0; i < graphDimension; i++) {
                if (current->vertexId != i && adjacencyMap[current->vertexId][i] != 0) {
                    temp = distance[current->vertexId] + adjacencyMap[current->vertexId][i];
                    if (distance[i] == INFINITY || distance[i] > temp) {
                        distance[i] = temp;
                        if (!nodesArray[i]->visited) {
                            nodesArray[i]->dist = temp;
                            heapDecreaseKey(binHeap, nodesArray[i]->vertexId);
                        }
                    }
                }
            }
        }
    }
    if (binHeap->heapSize > 0)
        binHeap->heapSize = 0;
    for (int i = 0; i < graphDimension; i++) {
        if (distance[i] == INFINITY)
            distance[i] = 0;
        if (i != sourceId) {
            nodesArray[i]->dist = INFINITY;
        }
        nodesArray[i]->visited = false;
    }
}

/**
 * Sums up all the distances.
 * @param distance the array containing the distances from the source vertex
 * @return the sum of all the elements into the distance array
 */
int fromDijkstraToMetric() {
    int totalDistance = 0;
    for (int i = 0; i < graphDimension; ++i) {
        totalDistance += distance[i];
    }
    return totalDistance;
}

/**
 * Calculates the metric of a graph.
 * @param adjacencyMap the adjacency matrix
 * @return the metric value
 */
int calcGraphMetric() {
    dijkstra(0);
    return fromDijkstraToMetric();
}

//
//----------------------------------------------------------------------------------------------------------------------
/**
 * Parse a string array containing digits and returns the
 * @param number the char array that contains digits
 * @return the number contained in the array, as an integer
 */
int parseInt(const char *number) {
    int value = 0;
    for (int i = 0; i < strlen(number); i++) {
        value = 10 * value + (number[i] - 48);
    }
    return value;
}

/**
 * Parse the next edge and adds weights into the array passed as parameter.
 * @param vertices the array in which to put weights
 */
void parseNextEdge(int vertices[], int vertexId) {
    int insertedNumbersPointer = 0;
    bool isFirstDigit = true;
    int number = 0;
    char nextDigit;
    size_t lineLen;
    lineLen = getline(&nextLine, &maxLineSize, stdin);
    for (int i = 0; i < lineLen; i++) {
        nextDigit = nextLine[i];
        if (nextDigit == ',' || nextDigit == '\r' || nextDigit == '\n') {
            vertices[insertedNumbersPointer] = number;
            if (nextDigit == '\r' || nextDigit == '\n')
                return;
            insertedNumbersPointer++;
            isFirstDigit = true;
            number = 0;
        } else {
            if (insertedNumbersPointer == 0 || insertedNumbersPointer == vertexId) {
                i += (int) strcspn(&nextLine[i], ",");
                vertices[insertedNumbersPointer] = 0;
                insertedNumbersPointer++;
            } else {
                if (isFirstDigit && nextDigit == '0') {
                    i++;
                    vertices[insertedNumbersPointer] = 0;
                    insertedNumbersPointer++;
                    if (insertedNumbersPointer == graphDimension)
                        return;
                } else {
                    number = number * 10 + nextDigit - 48;
                    isFirstDigit = false;
                }
            }
        }
    }
}

/**
 * Reads a graph.
 */
void analyzeGraph() {
    for (int i = 0; i < graphDimension; i++) {
        parseNextEdge(adjacencyMap[i], i);
        if (i == 0) {
            bool zeroNodeIsDisconnected = true;
            for (int j = 0; j < graphDimension; j++) {
                if (adjacencyMap[0][j] != 0)
                    zeroNodeIsDisconnected = false;
            }
            if (zeroNodeIsDisconnected) {
                addGraphIntoRank(graphIndex, 0);
                graphIndex++;
                return;
            }
        }
    }
    int result = calcGraphMetric();
    addGraphIntoRank(graphIndex, result);
    graphIndex++;
}

/**
 * Prints out the rank.
 */
void printRank() {
    if (actualRankSize == 0) {
        putc('\n', stdout);
        return;
    }
    /*if (rankChanged) {
        if (rankLengthChanged) {
            lastRankShot = (int *) realloc(lastRankShot, actualRankSize * sizeof(int));
            rankLengthChanged = false;
        }
        count = 0;
        printInOrder(treeRoot);
        rankChanged = false;
    }
    for (int i = 0; i <= actualRankSize - 1; ++i) {
        printf("%d", lastRankShot[i]);
        if (i < actualRankSize - 1)
            printf(" ");
    }*/
    printInOrder(treeRoot);
    count = 0;
    putc('\n', stdout);
}

/**
 * Initializes the program.
 */
void initialize() {
    char *initialization;
    char *next;
    size_t charDim;
    size_t maxDim = 23;
    initialization = malloc(sizeof(char) * maxDim);
    charDim = getline(&initialization, &maxDim, stdin);
    if (charDim) {
        graphDimension = strtol(initialization, &next, 10);
        rankLength = strtol(next, NULL, 10);
    }
    /*free(initialization);*/
    adjacencyMap = (int **) malloc(sizeof(int *) * graphDimension);
    distance = (int *) malloc(sizeof(int) * graphDimension);
    for (int i = 0; i < graphDimension; ++i) {
        adjacencyMap[i] = (int *) malloc(sizeof(int) * graphDimension);
    }
    maxLineSize = sizeof(char) * (11 * graphDimension + 11 + 3);
    nextLine = (char *) malloc(maxLineSize);
    nodesArray = malloc(sizeof(heapNode *) * graphDimension);
    for (int i = 0; i <= graphDimension; ++i) {
        if (i == 0)
            nodesArray[0] = createHeapNode(0, 0);
        else
            nodesArray[i] = createHeapNode(INFINITY, i);

    }
    heapNode **nodes = malloc(sizeof(heapNode *) * graphDimension + 1);
    binHeap = buildHeap(nodes, graphDimension, 0);
    binHeap->position = malloc(sizeof(int) * graphDimension + 1);
}

/**
 * Reads commands from input.
 */
void inputHandler() {
    char *command;
    size_t cmdLen;
    size_t argumentDim = 15;
    command = malloc(sizeof(char) * argumentDim);
    do {
        command[0] = '\0';
        cmdLen = getline(&command, &argumentDim, stdin);
        if (command[0] == 'A') {
            if (worstMetricValue != 0 || actualRankSize != rankLength)
                analyzeGraph();
        } else if (command[0] == 'T')
            printRank();
    } while (cmdLen != -1);
    free(command);
}

int main() {
    setbuf(stdout, NULL);
    rankChanged = false;
    rankLengthChanged = false;
    /*lastRankShot = malloc(sizeof(int));*/
    treeNil = malloc(sizeof(rankNode));
    treeNil->graphId = -1;
    treeNil->key = -1;
    treeNil->father = treeNil;
    treeNil->left = treeNil;
    treeNil->right = treeNil;
    treeNil->isRed = false;
    treeRoot = treeNil;
    initialize();
    inputHandler();
    /*free(treeNil);
    free(lastRankShot);*/
    return 0;
}
