//  Copyright © 2019 Whelan Callahan. All rights reserved.

#import <Foundation/Foundation.h>

// for use with c structures
@interface DynamicList : NSObject

- (instancetype)initWith:(unsigned int)dataSize;

- (void)pushBack: (void *)data;
- (void*)atIndex: (unsigned int)i;
- (unsigned int)size;
- (void)clear;

@end
