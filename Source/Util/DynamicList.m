//  Copyright © 2019 Whelan Callahan. All rights reserved.
#import "DynamicList.h"

static const unsigned int START_SIZE = 10;

@implementation DynamicList {
    unsigned int mDataSize;
    unsigned int mMaxSize;
    unsigned int mCurrSize;
    void *mMemory;
}


- (instancetype)initWith:(unsigned int)dataSize
{
    if (self = [super init]) {
        mDataSize = dataSize;
        mMemory = malloc(dataSize * mMaxSize);
    }
    
    return self;
}


- (void)dealloc
{
    free(mMemory);
}


- (void)pushBack: (void *)data
{
    if (mCurrSize == mMaxSize)
        [self _resize];
    
    memcpy([self atIndex:mCurrSize], data, mDataSize);
    ++mCurrSize;
}


- (void)_resize
{
    mMaxSize *= 2;
    mMemory = realloc(mMemory, mMaxSize);
}


- (void*)atIndex: (unsigned int)i
{
    return mMemory + (mDataSize * i);
}


- (unsigned int)size
{
    return mCurrSize;
}


- (void)clear
{
    mCurrSize = 0;
}


@end
