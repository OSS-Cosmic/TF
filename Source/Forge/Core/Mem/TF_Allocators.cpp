#include "Forge/Core/Mem/TF_Allocators.h"

#include "Forge/Core/TF_Logging.h"
#include "Forge/Core/TF_Math.h"

#include "Forge/Core/Mem/TF_Memory.h"

struct TFStackAllocBlock
{
    struct TFStackAllocBlock* mNext;
    size_t                    mSize;
    void*                     mData[];
};

struct TFAllocScratchBlock
{
    struct TFAllocScratchBlock* mNext;
    void*                       mData[];
};

void tfAddFixedAllocator(struct TFFixedBufferAllocator* alloc, struct TFFixedBufferAllocDesc* desc)
{
    ASSERT(alloc);
    ASSERT(desc);
    memset(alloc, 0, sizeof(struct TFScratchAllocator));
    alloc->mData = desc->mBuffer;
    alloc->mSize = desc->mSize;
}

void tfAddScratchAllocator(struct TFScratchAllocator* alloc, TFScratchAllocDesc* desc)
{
    ASSERT(alloc);
    ASSERT(desc);
    memset(alloc, 0, sizeof(struct TFScratchAllocator));
    alloc->blockSize = desc->blockSize;
    alloc->alignment = max(desc->alignment, sizeof(uint16_t));
}

void tfAddStackAllocator(struct TFStackAllocator* alloc, struct TFStackAllocDesc* desc)
{
    ASSERT(alloc);
    ASSERT(desc);
    memset(alloc, 0, sizeof(struct TFStackAllocator));
    alloc->mBlockSize = desc->mBlockSize;
    alloc->mAlignment = max(desc->mAlignment, sizeof(uint16_t));
}

void* tfScratchAlloc(TFScratchAllocator* alloc, size_t size)
{
    const size_t reqSize = round_up_64(size, alloc->alignment);
    if (reqSize > alloc->blockSize)
    {
        struct TFAllocScratchBlock* block =
            (struct TFAllocScratchBlock*)tf_malloc(sizeof(struct TFAllocScratchBlock) + size + alloc->alignment);
        block->mNext = alloc->mFreeBlocks;
        alloc->mFreeBlocks = block;

        size_t offset = ((size_t)block->mData) % alloc->alignment;
        return ((uint8_t*)block->mData) + offset;
    }

    if (alloc->mCurrent && (alloc->pos + reqSize) > alloc->blockSize)
    {
        struct TFAllocScratchBlock* block =
            (struct TFAllocScratchBlock*)tf_malloc(sizeof(struct TFAllocScratchBlock) + alloc->blockSize + alloc->alignment);
        alloc->pos = ((size_t)block->mData) % alloc->alignment;

        block->mNext = alloc->mFreeBlocks;
        alloc->mFreeBlocks = block;
        alloc->mCurrent = block;
    }
    void* result = ((uint8_t*)alloc->mCurrent->mData) + alloc->pos;
    alloc->pos += reqSize;
    return result;
}

void* tfFixedBufferAlloc(struct TFFixedBufferAllocator* alloc, size_t size)
{
    const size_t reqSize = round_up_64(size, alloc->mAlignment);
    if (alloc->mPos == 0)
        alloc->mPos = ((size_t)alloc->mData) % alloc->mAlignment;
    if ((alloc->mPos + reqSize) > alloc->mSize)
    {
        return NULL;
    }
    void* result = ((uint8_t*)alloc->mData) + alloc->mPos;
    alloc->mPos += reqSize;
    return result;
}

void* tfStackAlloc(struct TFStackAllocator* alloc, size_t size)
{
    const size_t reqSize = round_up_64(size, alloc->mAlignment);
    if (reqSize > alloc->mBlockSize)
    {
        struct TFStackAllocBlock* block = (struct TFStackAllocBlock*)tf_malloc(sizeof(struct TFStackAllocBlock) + size + alloc->mAlignment);
        block->mSize = size + alloc->mAlignment;
        block->mNext = alloc->mTail;
        alloc->mTail = block;
        if (alloc->mHead == NULL)
        {
            alloc->mHead = block;
            alloc->mPos = block->mSize;
        }
        alloc->mTotalAllocated += block->mSize;
        size_t offset = ((size_t)block->mData) % alloc->mAlignment;
        return ((uint8_t*)block->mData) + offset;
    }

    if (alloc->mHead && (alloc->mPos + reqSize) > alloc->mHead->mSize)
    {
        if (alloc->mFreeBlocks)
        {
            struct TFStackAllocBlock* block = alloc->mFreeBlocks;
            alloc->mFreeBlocks = alloc->mFreeBlocks->mNext;
            alloc->mPos = ((size_t)block->mData) % alloc->mAlignment;

            alloc->mHead->mNext = block;
            alloc->mHead = block;
            if (alloc->mTail == NULL)
                alloc->mTail = block;
        }
        else
        {
            struct TFStackAllocBlock* block =
                (struct TFStackAllocBlock*)tf_malloc(sizeof(struct TFAllocScratchBlock) + alloc->mBlockSize + alloc->mAlignment);
            block->mSize = alloc->mBlockSize + alloc->mAlignment;
            alloc->mTotalAllocated += block->mSize;
            alloc->mPos = ((size_t)block->mData) % alloc->mAlignment;

            alloc->mHead->mNext = block;
            alloc->mHead = block;
            if (alloc->mTail == NULL)
                alloc->mTail = block;
        }
    }
    void* result = ((uint8_t*)alloc->mHead->mData) + alloc->mPos;
    alloc->mPos += reqSize;
    return result;
}

struct TFStackAllocatorCheckpoint tfStackAllocGetCheckpoint(struct TFStackAllocator* alloc)
{
    struct TFStackAllocatorCheckpoint checkpoint = { .mTop = alloc->mHead, .mPos = alloc->mPos };
    return checkpoint;
}

void tfStackAllocRestoreCheckpoint(struct TFStackAllocator* alloc, struct TFStackAllocatorCheckpoint* checkpoint)
{
    if (checkpoint->mTop == NULL)
    {
        return;
    }
    alloc->mHead = checkpoint->mTop;
    alloc->mPos = checkpoint->mPos;
    if (checkpoint->mTop->mNext)
    {
        TFStackAllocBlock* blk = checkpoint->mTop->mNext;
        while (blk)
        {
            struct TFStackAllocBlock* current = blk;
            blk = blk->mNext;
            current->mNext = alloc->mFreeBlocks;
            alloc->mFreeBlocks = current;
        }
    }
}

void tfFreeStackAllocator(struct TFStackAllocator* alloc)
{
    struct TFStackAllocBlock* blk = alloc->mTail;
    while (blk)
    {
        struct TFStackAllocBlock* current = blk;
        blk = blk->mNext;
        tf_free(current);
    }
    blk = alloc->mFreeBlocks;
    while (blk)
    {
        struct TFStackAllocBlock* current = blk;
        blk = blk->mNext;
        tf_free(current);
    }
}

void tfStackAllocCompaction(struct TFStackAllocator* alloc, size_t reserveSize)
{
    struct TFStackAllocBlock* freeBlocks = NULL;
    struct TFStackAllocBlock* blk = alloc->mFreeBlocks;
    while (blk)
    {
        struct TFStackAllocBlock* current = blk;
        blk = blk->mNext;
        if (alloc->mTotalAllocated > reserveSize)
        {
            alloc->mTotalAllocated -= current->mSize;
            tf_free(current);
        }
        else
        {
            current->mNext = freeBlocks;
            freeBlocks = current;
        }
    }
    alloc->mFreeBlocks = freeBlocks;
}

void tfStackAllocReset(struct TFStackAllocator* alloc)
{
    struct TFStackAllocBlock* freeBlocks = NULL;
    size_t                    calcSize = 0;
    struct TFStackAllocBlock* blk = alloc->mTail;
    while (blk)
    {
        struct TFStackAllocBlock* current = blk;
        blk = blk->mNext;
        calcSize += current->mSize;

        current->mNext = freeBlocks;
        freeBlocks = current;
    }
    blk = alloc->mFreeBlocks;
    while (blk)
    {
        struct TFStackAllocBlock* current = blk;
        blk = blk->mNext;
        calcSize += current->mSize;

        current->mNext = freeBlocks;
        freeBlocks = current;
    }
    alloc->mFreeBlocks = freeBlocks;
    alloc->mTail = NULL;
    alloc->mHead = NULL;
}

void tfFreeScratchAllocator(TFScratchAllocator* alloc)
{
    struct TFAllocScratchBlock* blk = alloc->mFreeBlocks;
    while (blk)
    {
        struct TFAllocScratchBlock* current = blk;
        blk = blk->mNext;
        tf_free(current);
    }
}
