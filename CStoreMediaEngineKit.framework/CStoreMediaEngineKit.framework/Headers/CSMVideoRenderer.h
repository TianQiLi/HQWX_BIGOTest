//
//  CSMVideoRenderer.h
//  CStoreEngineKit
//
//  Created by Dushu Ou on 2019/9/2.
//  Copyright © 2019 BIGO Inc. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "CStoreMediaEngineCoreEnums.h"
NS_ASSUME_NONNULL_BEGIN

/**
 表示一块视频渲染区域
 */
@interface CSMVideoRenderer : NSObject
///渲染区域坐标
@property (nonatomic, assign) CGRect renderFrame;
///该区域对应的主播 / 麦上用户 uid
@property (nonatomic, assign) uint64_t uid;
/// 座位号
@property (nonatomic, assign) int micNum;

///渲染模式
@property (nonatomic, assign) CSMVideoRenderMode renderMode;

@end

NS_ASSUME_NONNULL_END
