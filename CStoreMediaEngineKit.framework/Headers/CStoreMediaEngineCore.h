//
//  CStoreMediaEngineCore.h
//  CStoreEngineKit
//
//  Created by Caimu Yang on 2019/8/20.
//  Copyright © 2019 BIGO Inc. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "CStoreMediaEngineCoreDelegate.h"

NS_ASSUME_NONNULL_BEGIN

@interface CStoreMediaEngineCore : NSObject

/**
 设置/获取 CStoreMediaEngineCoreDelegate
 
 App 可根据自己需要，选择性实现 CStoreMediaEngineCoreDelegate 中方法，并为该变量进行赋值，用以监听回调事件
 */
@property (nonatomic, weak) id<CStoreMediaEngineCoreDelegate> delegate;

@end

NS_ASSUME_NONNULL_END
