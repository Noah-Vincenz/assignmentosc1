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
        /*
        char * const memory = new char[sizeIn];
        mcb = new (memory) MemControlBlock(true, );
         */
        cout << "Initial requested: " << requested << endl;
        int mod = requested % 4;
        if (mod == 1) {
            requested -= 1;
        }
        if (mod == 2) {
            requested -= 2;
        }
        if (mod == 3) {
            requested += 1;
        }
        cout << "This is the requested space:  "<< requested << endl;
        print();
        MemControlBlock * curr = startOfHeap;
        MemControlBlock * bestSoFar = nullptr;
        int currMinSize = -1;
        int minSize = 0;
        int index = 0;
        char * currAddressMCB = reinterpret_cast<char*>(memory);
        char * bestAddress;
        for (int i = 0; curr; ++i, curr = curr->next) {
            cout << "Loop " << i << endl;
            if (curr->available && curr->size >= requested) {
                cout << "Curr.size is > than requested. It is " << curr->size << endl;
                cout << "Prev of curr: " << curr->previous << endl;
                cout << "Next of curr: " << curr->next << endl;
                minSize = curr->size - requested;
                if (currMinSize == -1 || minSize < currMinSize) {
                    currMinSize = minSize;
                    bestSoFar = curr;
                    bestAddress = currAddressMCB;
                }
            }
            currAddressMCB += curr->size;
        }
        if (bestSoFar == nullptr) {
            cout << "no best so far found" << endl;
            return nullptr;
        }
        else {
            bestSoFar->available = false;
            int oldSpace = bestSoFar->size;
            int newSpace = oldSpace - requested;
            bestSoFar->size = newSpace;
            if (newSpace > 16) {
                char * x = bestAddress + requested + 16;
                MemControlBlock * newMCB = new(x) MemControlBlock(true, newSpace);
                MemControlBlock * oldPrev;
                MemControlBlock * oldNext;
                if (bestSoFar->next) {
                    oldNext = bestSoFar->next;
                    newMCB->next = oldNext;
                    oldNext->previous = newMCB;
                }
                newMCB->previous = bestSoFar;
                bestSoFar->next = newMCB;
                bestSoFar->size = requested;
                cout << "jsjfksdf" << endl;
            }
            //char * x = new char[4];
            //return x;
            return bestAddress + 16;
            //return new (bestAddress + 16)char[requested];
            //return y;
//            return new (c) MemControlBlock(true, );
            //return new char[bestAddress + 16];
        }
    }
    
    /** @brief Deallocate the memory used by the object at the given address */
    void deallocateMemory(char * toDeallocate) {
        // TODO: your code for deallocateMemory memory goes here

        
        
    }
};

#endif