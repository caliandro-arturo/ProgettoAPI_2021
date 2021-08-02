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
int graphIndex = 0;
const int INFINITY = -1;

// Max heap for rank
//----------------------------------------------------------------------------------------------------------------------
/**
 * Node struct for RB tree implementation for rank.
 */
typedef struct {
    char *graphId;
    int key;
} rankNode;

typedef struct {
    rankNode **rankArray;
    int heapSize;
} rankHeap;

rankHeap *rank;

void printRankHeap(rankHeap *H) {
    for (int i = 1; i <= H->heapSize; i++) {
        fputs(H->rankArray[i]->graphId, stdout);
        if (i < H->heapSize)
            fputs(" ", stdout);
    }
    fputs("\n", stdout);
}

rankNode *createRankNode(int id, int key) {
    rankNode *newNode = malloc(sizeof(rankNode));
    newNode->key = key;
    char buf[12], *p = buf + 10;
    buf[11] = '\0';
    int numLen = 1;
    do {
        numLen++;
        p--;
        *p = id % 10 + '0';
        id /= 10;
    } while (id);
    newNode->graphId = malloc(sizeof(char) * numLen);
    memcpy(newNode->graphId, p, numLen);
    return newNode;
}

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

void swap(rankHeap *H, int pos1, int pos2) {
    rankNode *temp = H->rankArray[pos1];
    H->rankArray[pos1] = H->rankArray[pos2];
    H->rankArray[pos2] = temp;
}

void rankHeapify(rankHeap *H, int i) {
    int l = left(i);
    int r = right(i);
    int largest;
    if (l <= H->heapSize && H->rankArray[l]->key > H->rankArray[i]->key)
        largest = l;
    else
        largest = i;
    if (r <= H->heapSize && H->rankArray[r]->key > H->rankArray[largest]->key)
        largest = r;
    if (largest != i) {
        swap(H, i, largest);
        rankHeapify(H, largest);
    }
}

void rankHeapRemoveMax(rankHeap *H) {
    if (H->heapSize == 0)
        return;
    rankNode *max = H->rankArray[1];
    H->rankArray[1] = H->rankArray[H->heapSize];
    H->heapSize--;
    rankHeapify(H, 1);
    free(max);
}

void rankHeapInsert(rankHeap *H, rankNode *toInsert) {
    H->heapSize++;
    int i = H->heapSize;
    int key = toInsert->key;
    while (i > 1 && H->rankArray[parent(i)]->key < key) {
        H->rankArray[i] = H->rankArray[parent(i)];
        i = parent(i);
    }
    H->rankArray[i] = toInsert;
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
    bool inserted;
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
    newNode->inserted = false;
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

/*void check(int i) {
    if (binHeap->nodes[i]->vertexId != binHeap->nodes[binHeap->position[binHeap->nodes[i]->vertexId]]->vertexId)
        printf("error");
}*/

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
        H->position[H->nodes[i]->vertexId] = i;
        H->position[H->nodes[smallest]->vertexId] = smallest;
        /*check(i);
        check(smallest);*/
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
        /*check(i);*/
        i = parent(i);
    }
    H->nodes[i] = toInsert;
    H->position[toInsert->vertexId] = i;
    /*check(i);*/
    toInsert->inserted = true;
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
        /*check(i);
        check(parent(i));*/
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
        if (result >= rank->rankArray[1]->key) {
            return;
        } else {
            rankHeapRemoveMax(rank);
        }
    }
    rankNode *newNode = createRankNode(graphToAdd, result);
    rankHeapInsert(rank, newNode);
    if (actualRankSize != rankLength) {
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
    heapInsert(binHeap, nodesArray[0]);
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
                            if (!nodesArray[i]->inserted) {
                                heapInsert(binHeap, nodesArray[i]);
                            } else {
                                heapDecreaseKey(binHeap, nodesArray[i]->vertexId);
                            }
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
        nodesArray[i]->inserted = false;
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
int graphWeightNumber = 0;
bool allTheWeightsAreEquals = true;

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
            if (allTheWeightsAreEquals)
                allTheWeightsAreEquals = graphWeightNumber == number;
            if (nextDigit == '\r' || nextDigit == '\n')
                return;
            insertedNumbersPointer++;
            isFirstDigit = true;
            number = 0;
        } else {
            if (insertedNumbersPointer == 0 || insertedNumbersPointer == vertexId) {
                if (vertexId == 0) {
                    while (nextLine[i] != ',' && nextLine[i] != '\r' && nextLine[i] != '\n') {
                        graphWeightNumber = graphWeightNumber * 10 + nextLine[i] - 48;
                        i++;
                    }
                } else
                    i += (int) strcspn(&nextLine[i], ",");
                vertices[insertedNumbersPointer] = 0;
                insertedNumbersPointer++;
            } else {
                if (isFirstDigit && nextDigit == '0') {
                    i++;
                    vertices[insertedNumbersPointer] = 0;
                    if (allTheWeightsAreEquals)
                        allTheWeightsAreEquals = graphWeightNumber == 0;
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
    if (allTheWeightsAreEquals) {
        addGraphIntoRank(graphIndex, graphWeightNumber * (graphDimension - 1));
    } else {
        addGraphIntoRank(graphIndex, calcGraphMetric());
    }
    allTheWeightsAreEquals = true;
    graphWeightNumber = 0;
    graphIndex++;
}

/**
 * Prints out the rank.
 */
void printRank() {
    printRankHeap(rank);
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
    rank = malloc(sizeof(rankHeap));
    rank->rankArray = malloc(sizeof(rankNode *) * rankLength + 1);
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
            if (rank->heapSize != rankLength || (rank->heapSize > 0 && rank->rankArray[1]->key > 0))
                analyzeGraph();
        } else if (command[0] == 'T')
            printRank();
    } while (cmdLen != -1);
    free(command);
}

int main() {
    setbuf(stdout, NULL);
    initialize();
    inputHandler();
    /*free(treeNil);
    free(lastRankShot);*/
    return 0;
}
