#ifndef HEAP_H
#define HEAP_H

#include <cstddef>
#include <new>

#include <iostream>
using std::cout;
using std::endl;

class MemControlBlock {
  
public:
    
    /** @brief True if the MemControlBlock is before a block of available memory */
    bool available;
    
    /** @brief The size of the following block of memory, in bytes */
    size_t size;
    
    MemControlBlock * previous;
    MemControlBlock * next;
    
    MemControlBlock(const bool availableIn, const int sizeIn)
        : available(availableIn), size(sizeIn),
          previous(nullptr), next(nullptr)
    {
    }
    
};

class Heap {

private:
    char * const memory;
    
    /** @brief The first MemControlBlock for the heap -- the head of the list */
    MemControlBlock * startOfHeap;
    
public:
    
    ~Heap() {
        delete [] memory;
    }
    
    // no copy constructor, move constructor, assignment operator, ...
    Heap(const Heap &) = delete;
    Heap(Heap &&) = delete;    
    Heap & operator=(const Heap &) = delete;
    Heap & operator=(Heap &&) = delete;
    
    Heap(const size_t sizeIn)
        : memory(new char[sizeIn]) {

        // make a MemControlBlock at the start of the reserved memory
        startOfHeap = new(memory) MemControlBlock(// true = is available
                                                  true, 
                                                  // size = the size of the heap - the size of the MemControlBlock a the start of the heap
                                                  sizeIn - sizeof(MemControlBlock)
                                                 );
    }

    /** @brief Used for debugging - get the address of the start of the heap */
    char* getStartOfHeap() const {
        return memory;
    }
    
    /** @brief Used for debugging -- print out the details of the MemControlBlocks on the heap */
    void print() const {
        MemControlBlock * curr = startOfHeap;
        
        for (int i = 0; curr; ++i, curr = curr->next) {
            cout << "Block " << i << ": ";
            if (curr->available) {
                cout << " free, ";
            } else {
                cout << " in use, ";
            }
            cout << curr->size << " bytes\n";
        }
            
    }
    
    
    /** @brief Request a block of memory of the given size
     * 
     * Uses the 'Best Fit' algorithm to choose a suitable block of available memory
     *
     * Returns the memory address of the start of the requested memory
     * If no block is big enough, it returns nullptr.
     */
    char * allocateMemoryBestFit(size_t requested) {
        int mod = requested % 4;
        if (mod != 0) {
            requested = requested + 4 - mod;
        }
        MemControlBlock * curr = startOfHeap;
        MemControlBlock * bestSoFar = nullptr;
        int currMinSize = -1;
        int minSize = 0;
        char * currAddressMCB = reinterpret_cast<char*>(memory);
        char * bestAddress;
        for (int i = 0; curr; ++i, curr = curr->next) {
            if (curr->available && curr->size >= requested) {
                minSize = curr->size - requested;
                if (currMinSize == -1 || minSize < currMinSize) {
                    currMinSize = minSize;
                    bestSoFar = curr;
                    bestAddress = currAddressMCB;
                }
            }
            currAddressMCB = currAddressMCB + curr->size + 16;
        }
        if (bestSoFar == nullptr) {
            return nullptr;
        }
        else {
            bestSoFar->available = false;
            int oldSpace = bestSoFar->size;
            int newSpace = oldSpace - requested - 16; //sure about -16?
            if (newSpace > 16) {
                char * x = bestAddress + requested + 16;
                MemControlBlock * newMCB = new(x) MemControlBlock(true, newSpace);
                if (bestSoFar->next) {
                    MemControlBlock * oldNext = bestSoFar->next;
                    newMCB->next = oldNext;
                    oldNext->previous = newMCB;
                }
                bestSoFar->next = newMCB;
                newMCB->previous = bestSoFar;
                bestSoFar->size = requested;
            }
            return bestAddress + 16;
        }
    }

    /** @brief Deallocate the memory used by the object at the given address */
    void deallocateMemory(char * toDeallocate) {
        // TODO: your code for deallocateMemory memory goes here
        char * addressOfMCB = reinterpret_cast<char*>(toDeallocate);
        addressOfMCB -= 16;
        MemControlBlock * curr = startOfHeap;
        char * currAddressMCB = reinterpret_cast<char*>(memory);
        for (int i = 0; curr; ++i, curr = curr->next) {
            if (currAddressMCB == addressOfMCB) {
                curr->available = true;
                if (curr->previous && curr->previous->available == true) { //merge together
                    curr->previous->size = curr->previous->size + curr->size + 16;
                    if (curr->next) {
                        curr->previous->next = curr->next;
                        curr->next->previous = curr->previous;
                    }
                    else {
                        curr->previous->next = nullptr;
                    }
                    curr->next = nullptr;
                    curr->previous = nullptr;
                }
                //not sure about this one
                if (curr->next && curr->next->available == true) { //merge together
                    curr->size = curr->size + 16 + curr->next->size;
                    if (curr->next->next) {
                        curr->next = curr->next->next;
                        curr->next->next->previous = curr;
                        curr->next->previous = nullptr;
                        curr->next->next = nullptr;
                    }
                }
            }
            currAddressMCB = currAddressMCB + curr->size + 16;
        }
    }
};

#endif
