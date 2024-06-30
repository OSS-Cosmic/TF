#pragma once

#include "Forge/Core/TF_Core.h"
#include "Forge/Core/TF_Logging.h"

typedef void* (*TFAllocatorMalloc)(void* self, size_t size);
typedef void* (*TFAllocatorFree)(void* self, void* ptr);

struct TFAllocator {
  void* mSelf;
  TFAllocatorMalloc mMalloc;
  TFAllocatorFree mFree;
};


struct TFFixedBufferAllocDesc
{
    void* mBuffer;
    size_t mSize;
    size_t mAlignment;
};

struct TFFixedBufferAllocator
{
    void*  mData;
    size_t mSize;
    size_t mPos;
    size_t mAlignment;
};

struct TFScratchAllocDesc
{
    size_t blockSize;
    size_t alignment;
};

struct TFScratchAllocator
{
    struct TFAllocScratchBlock* mFreeBlocks;
    struct TFAllocScratchBlock* mCurrent;
    size_t                      pos;
    size_t                      blockSize;
    size_t                      alignment;
};

struct TFStackAllocatorCheckpoint
{
    struct TFStackAllocBlock* mTop;
    size_t                    mPos;
};

struct TFStackAllocDesc
{
    size_t mBlockSize;
    size_t mAlignment;
};

struct TFStackAllocator
{
    struct TFStackAllocBlock* mFreeBlocks;
    struct TFStackAllocBlock* mTail;
    struct TFStackAllocBlock* mHead;
    size_t                    mTotalAllocated;
    size_t                    mPos;
    size_t                    mBlockSize;
    size_t                    mAlignment;
};


// stack allocator
void tfAddFixedAllocator(struct TFFixedBufferAllocator* alloc, struct TFFixedBufferAllocDesc* desc);
void tfAddStackAllocator(struct TFStackAllocator* alloc, struct TFStackAllocDesc* desc);
void tfAddScratchAllocator(struct TFScratchAllocator* alloc, TFScratchAllocDesc* desc);

void tfFreeScratchAllocator(struct TFScratchAllocator* alloc);
void tfFreeStackAllocator(struct TFStackAllocator* alloc);

// checkpoint allows for the allocation to restore to a checkpoint
struct TFStackAllocatorCheckpoint tfStackAllocGetCheckpoint(struct TFStackAllocator* alloc);
void tfStackAllocRestoreCheckpoint(struct TFStackAllocator* alloc, struct TFStackAllocatorCheckpoint* checkpoint);
void tfStackAllocReset(struct TFStackAllocator* alloc);
void tfStackAllocCompaction(struct TFStackAllocator* alloc, size_t reserveSize);

void* tfScratchAlloc(struct TFScratchAllocator* alloc, size_t size);
void* tfStackAlloc(struct TFStackAllocator* alloc, size_t size);
void* tfFixedBufferAlloc(struct TFFixedBufferAllocator* alloc, size_t size);

