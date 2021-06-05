//
//  CSMAudioStats.h
//  CStoreMediaEngineKit
//
//  Created by HZheng on 2021/4/17.
//  Copyright © 2021 Aestron Inc. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface CSMAudioStats : NSObject

/**加入频道后累计通话时长,单位为秒 */
@property (nonatomic, assign) int32_t totalDuration;

/**加入频道后累计发送音频字节数 */
@property (nonatomic, assign) int32_t txAudioBytes;

/**加入频道后累计接收音频字节数 */
@property (nonatomic, assign) int32_t rxAudioBytes;

/**音频包的实时的每秒发送码率，单位bps */
@property (nonatomic, assign) int32_t txAudioBitrate;

/**音频包的实时的每秒发送码率，单位bps */
@property (nonatomic, assign) int32_t rxAudioBitrate;

/**音频链路本地客户端到边缘服务器的延迟 */
@property (nonatomic, assign) int32_t lastmileDelay;

/**音频链路本地客户端到边缘服务器的上行丢包率（%） */
@property (nonatomic, assign) int32_t txPacketLossRate;

/**视频链路本地客户端到边缘服务器的下行丢包率（%） */
@property (nonatomic, assign) int32_t rxPacketLossRate;

@end

NS_ASSUME_NONNULL_END
