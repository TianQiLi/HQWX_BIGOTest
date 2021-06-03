//
//  CSMLiveTranscoding.h
//  CStoreMediaEngineKit
//
//  Created by 林小程 on 2020/9/4.
//  Copyright © 2020 BIGO Inc. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "CSMTranscodingUser.h"
#import "CSMTranscodingImage.h"

NS_ASSUME_NONNULL_BEGIN

/**
 合流转码参数
 */
@interface CSMLiveTranscoding : NSObject

/**
 推流视频的总尺寸（宽和高），单位为像素。（必须是4的倍数）
 
 - size.width，推流视频的总宽度，默认值 720。width 值不得低于 64，否则 Aestron会调整为 64，不得高于1080
 - size.height，推流视频的总高度，默认值 1280。 height 值不得低于 64，否则 Aestron 会调整为 64，不得高于1920
 */
@property (assign, nonatomic) CGSize size;///<  推流视频的总尺寸（宽和高，单位为像素，必须是4的倍数）

/**
 推流视频的码率
 
 单位为 Kbps，默认值为 1000 Kbps。
 如果设置的码率超出合理范围，Aestron 服务器会在合理区间内自动调整码率值
 */
@property (nonatomic, assign) uint16_t videoBitrate;///<  推流视频的码率（单位为 Kbps）

/**
 推流视频的帧率

 单位为 fps，取值范围为 [1, 30]，默认值为 15 fps。服务器会将高于 30 的帧率设置改为 30
 */
@property (nonatomic, assign) uint8_t videoFramerate;///<  推流视频的帧率（单位为 fps）

/**
 用于旁路直播的输出视频的 GOP

 单位为帧。默认值为 24 帧
 */
@property (nonatomic, assign) uint8_t videoGop;///<  旁路直播的输出视频的 GOP（单位为帧）

/**
 参与合流的全部用户信息，详见 CSMTranscodingUser
 
 用于存储参与合流的全部用户信息，用户区域设置范围不能完全超出画布范围(x >= width || y >= height)
 */
@property (nonatomic, strong) NSArray<CSMTranscodingUser *> *transcodingUsers;

/**
 用于旁路直播的输出视频上的背景图片

 添加后所有旁路直播的观众都可以看到背景图片
 */
@property (nonatomic, strong) CSMTranscodingImage *backgroundImage;///<  输出视频上的背景图片，详见 CSMTranscodingImage

@end

NS_ASSUME_NONNULL_END
