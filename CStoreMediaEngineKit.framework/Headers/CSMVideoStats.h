//
//  CSMVideoStats.h
//  CStoreMediaEngineKit
//
//  Created by Peter Xi on 2021/4/10.
//  Copyright © 2021 Aestron Inc. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface CSMVideoStats : NSObject

/**实际发送码率，单位为 Kbps，不包含丢包后重传的视频码率。 */
@property (nonatomic) int32_t sentBitrate;

/**实际发送帧率，单位为 fps，不包含丢包后重传视频等的发送帧率。 */
@property (nonatomic) int32_t sentFrameRate;

/**本地编码器的输出帧率，单位为 fps。 */
@property (nonatomic) int32_t encoderOutputFrameRate;

/**视频编码码率（Kbps）。该参数不包含丢包后重传视频等的编码码率。 */
@property (nonatomic) int32_t encodedBitrate;

/**视频编码宽度（px）。 */
@property (nonatomic) int32_t encodedFrameWidth;

/**视频编码高度（px)。 */
@property (nonatomic) int32_t encodedFrameHeight;

/**弱网对抗前本地客户端到边缘服务器的视频丢包率 (%)。 */
@property (nonatomic) int32_t txPacketLossRate;

/**本地采集帧率 (fps)。 */
@property (nonatomic) int32_t captureFrameRate;

/**加入频道后累计通话时长,单位为秒 */
@property (nonatomic) int32_t totalDuration;

/**加入频道后累计发送视频字节数 */
@property (nonatomic) int32_t txVideoBytes;

/**加入频道后累计接收视频字节数 */
@property (nonatomic) int32_t rxVideoBytes;

/**视频包的发送码率，单位Kbps */
@property (nonatomic) int32_t txVideoBitrate;

/**视频包的接收码率，单位Kbps */
@property (nonatomic) int32_t rxVideoBitrate;

/**视频链路本地客户端到边缘服务器的延迟 */
@property (nonatomic) int32_t lastmileDelay;

/**视频链路本地客户端到边缘服务器的下行丢包率（%） */
@property (nonatomic) int32_t rxPacketLossRate;

@end

NS_ASSUME_NONNULL_END
