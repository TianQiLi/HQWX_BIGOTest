//
//  CStoreMediaEngineCore+Public.h
//  CStoreEngineKit
//
//  Created by Caimu Yang on 2019/8/20.
//  Copyright © 2019 BIGO Inc. All rights reserved.
//

#ifndef CStoreMediaEngineCore_Public_h
#define CStoreMediaEngineCore_Public_h

#import "CStoreMediaEngineCoreEnums.h"
#import <Foundation/Foundation.h>
#import <GLKit/GLKit.h>
#import <CoreVideo/CoreVideo.h>

@class CSMVideoRenderer;
@class CSMUserInfo;
@class CSMLiveTranscoding;
@class CSMVideoCanvas;
@class CSAppConfigure;
@class CSEWaterMarkOptions;

NS_ASSUME_NONNULL_BEGIN

/**
 提供所有可供 App 调用的方法
 
 CStoreMediaEngineCore 是 Bigo SDK 的入口类。它为 App 提供了快速搭建音视频通信的 API。
 */
@interface CStoreMediaEngineCore (Public)


/**
 创建 CStoreMediaEngineCore 实例
 @param appId 开放平台授权申请的appId
 
 @warning *Important:* 请确保在调用其他 API 前先调用该方法创建并初始化 CStoreMediaEngineCore
 
 */
+ (void)launchWithAppId:(NSString *)appId;

/**
 返回全局唯一的CStoreMediaEngineCore对象
 @return CStoreMediaEngineCore对象，在调用 launchWithAppId: 之前调用该方法会返回nil
 */
+ (instancetype)sharedSingleton;

/**
 销毁CStoreMediaEngineCore对象
 
 调用destrop后再调用sharedSingleton会返回nil，需要重新调用 launchWithAppId: 初始化SDK
 */
+ (void)destroy;

/**
 通知SDK应用变成活跃状态
 
 在实现UIApplicationDelegate的applicationDidBecomeActive:接口时调用
 */
- (void)onApplicationDidBecomeActive;
/**
 通知SDK应用即将变成非活跃状态
 
 在实现UIApplicationDelegate的applicationWillResignActive:接口时调用
 */
- (void)onApplicationWillResignActive;
/**
 通知SDK应用回到后台
 
 在实现UIApplicationDelegate的applicationDidEnterBackground:接口时调用
 */
- (void)onApplicationDidEnterBackground;

/**
 设置用户角色
 
 用户有两种角色，分别是观众和主播，本接口用来设置用户角色。
 由观众切换到主播或者由主播切换到观众可以设置此接口。
 进入频道前，应先调用该接口，设置自己进入频道时的用户角色（主播 or 观众）。如果未调用该接口设置角色，joinChannel时会以观众身份进入频道。但建议App都显式调用该接口来设置进入频道时的角色。
 进入频道后，App可以调用该接口切换用户的角色类型。
 当用户的角色类型发生变化时，会触发 [CStoreMediaEngineCoreDelegate mediaEngine:didClientRoleChanged:newRole:clientRoleInfo:channelName:]回调。第一次进入频道不是切换角色的情况，但也会触发一次该回调用以通知App。
 
 @param role 用户角色，详见：CSMClientRole
 
 @see CSMClientRole
 */
- (void)setClientRole:(CSMClientRole)role;

/**
 设置频道模式
 
 @param channelProfile 频道模式。详见 CSMChannelProfile
 */
- (void)setChannelProfile:(CSMChannelProfile)channelProfile;

/**
 设置媒体次要信息发送开关及相关类型
 
 开启媒体次要信息传输后，发送 SEI 数据接口 sendMediaSideInfo:info 才有效,同时发送的数据长度会受到 onlyAudioPublish 限制。
 
 @param start 开启/关闭媒体次要信息传输，YES 表示开启媒体次要信息传输，NO 表示关闭媒体次要信息传输。start 为 YES 时，onlyAudioPublish 参数开关才有效。
 @param onlyAudioPublish 是否为纯音频直播，YES 表示纯音频直播，sei为单帧发送，长度限制为(16,800)字节；false 表示音视频直播，sei为随帧发送，长度限制为(16,4096)字节；默认为 NO。
 @param seiSendType sei发送类型(预留参数，暂无作用)
*/
- (void)setMediaSideFlags:(BOOL)start onlyAudioPublish:(BOOL)onlyAudioPublish seiSendType:(int)seiSendType;

/**
发送媒体次要信息传输
 
发送时将会采用 H.264 的 SEI (nalu type = 6, payload type = 5) 类型打包，由于视频编码器自身会产生 payload type 为 5 的 SEI，或使用视频文件推流时本身已存在此类型 SEI，因此用户使用该接口发送 SEI 时需要填写业务特定的 uuid(长度为 16 字节)，把 uuid + context 作为 info 输入。其中 uuid 将用作 SDK 的 SEI 解析过滤，回调 uuid 相符的 SEI 到业务方。
 
@param info 需要传输的音视频次要信息数据，外部输入；注意：用户需要把 uuid(长度为 16 字节) + content 当作 info 输入；
*/
- (void)sendMediaSideInfo:(NSData *)info;

/**
 获取SDK版本号
 
 @return 示例:0.0.1.218
 */
+ (NSString *)getSdkVersion;

#pragma mark - Channel

/**
 提前建立连接
 
 提前建立连接，连接会在40s后自动超时断开，在 joinChannelWithUserAccount:token:channelName:completion: 之前提前调用此方法
 预先建立好连接,可以节省一次rtt
 */
- (void)preConnect;


/** 
 加入频道
 
 该方法让用户加入频道，在同一个频道内的用户可以互相通话，多个用户加入同一个频道，可以群聊。
 在直播模式下，调用该方法加入一个频道前，用户需要先通过setClientRole设置自己的角色是主播还是观众，如果以主播身份进入频道，且之前没有人在该频道，则会自动创建频道，如果已经有人存在该频道，则在进入频道后自动上麦。
 在通信模式下，调用该方法，如果之前没有人在该频道，则会自动创建频道，如果已经有人存在该频道，则在进入频道后自动上麦。
 如果已在频道中，用户必须调用 leaveChannel 退出当前频道，才能进入下一个频道。
 成功调用该方法加入频道后，会回调 completion block
 
 @param optionalUid 第三方用户自定义uid（需业务方自己保证唯一）。为0时，服务器将自动分配一个
 @param token 在 App 服务器端生成的校验Token，在进入频道时用于鉴权
 @param channelName 频道名称
                     标识通话的频道名称，长度在 64 字节以内的字符串。以下为支持的字符集范围（共 89 个字符）：
                     26 个小写英文字母 a-z
                     26 个大写英文字母 A-Z
                     10 个数字 0-9
                     空格
                     "!", "#", "$", "%", "&", "(", ")", "+", "-", ":", ";", "<", "=", ".", ">", "?", "@", "[", "]", "^", "_", " {", "}", "|", "~", ","
 @param extraInfo 进房所携带的额外数据
                    该数据长度最长不能超过800字节，超过800字节会被截断为800字节。
                    一般用于主播的进房通知信息，用户可自行扩展。
                    该信息是在 [CStoreMediaEngineCoreDelegate mediaEngine:userJoined:elapsed:] 回调中用户信息中携带
 @param completion 加入频道成功与否的回调通知。
                    App可根据该回调结果，做一些简单的处理工作。
                    APP不要在此回调中做耗时操作，以免卡住SDK内部线程
 */
- (void)joinChannelWithUid:(uint64_t)optionalUid
                     token:(NSString *)token
               channelName:(NSString *)channelName
                 extraInfo:(NSString *)extraInfo
                completion:(void(^)(BOOL success,
                                    CSMErrorCode resCode,
                                    NSString *channel,
                                    uint64_t uid,
                                    NSInteger useTime))completion;

/**
 使用 User Account 加入频道
 
 @param userAccount 用户名。不可为空，业务方需要保证唯一性
 @param token 在 App 服务器端生成的校验Token，在进入频道时用于鉴权
 @param channelName 频道名称
                     标识通话的频道名称，长度在 64 字节以内的字符串。以下为支持的字符集范围（共 89 个字符）：

                     26 个小写英文字母 a-z
                     26 个大写英文字母 A-Z
                     10 个数字 0-9
                     空格
                     "!", "#", "$", "%", "&", "(", ")", "+", "-", ":", ";", "<", "=", ".", ">", "?", "@", "[", "]", "^", "_", " {", "}", "|", "~", ","
 @param extraInfo 进房所携带的额外数据
                 该数据长度最长不能超过800字节，超过800字节会被截断为800字节。
                 一般用于主播的进房通知信息，用户可自行扩展。
                 该信息是在 [CStoreMediaEngineCoreDelegate mediaEngine:userJoined:elapsed:] 回调中用户信息中携带
 @param completion 加入频道成功与否的回调通知。
                 App可根据该回调结果，做一些简单的处理工作。
                 APP不要在此回调中做耗时操作，以免卡住SDK内部线程
 */
- (void)joinChannelWithUserAccount:(NSString *)userAccount
                             token:(NSString *)token
                       channelName:(NSString *)channelName
                         extraInfo:(NSString *)extraInfo
                        completion:(void(^)(BOOL success,
                                            CSMErrorCode resCode,
                                            NSString *channel,
                                            uint64_t uid,
                                            NSInteger useTime))completion;

/**
 离开频道
 
 离开频道，即挂断或退出通话。调用joinChannel后，必须调用leaveChannel结束通话，否则无法开始下一次通话。成功调用该方法离开频道后，SDK会释放频道相关资源。
 通信模式下的用户和直播模式下的主播离开频道后，远端会触发 [CStoreMediaEngineCoreDelegate mediaEngine:userOffline:reason:] 回调。
 进房可能会触发的错误：
 1. token校验失败：排查步骤 1）检查token是否过期，每个token都有一个有效期，如果过期了会校验失败。2）这种情况下需要对比生成的token的入参(appid、uid、channelName、cert)和你进入的房间信息(sdk create的时候传入的appid、自己的uid、进入channelName)是否匹配，如果不匹配，则会校验失败。
 2. 重复进入房间错误：如果之前进入过频道，在没有leavelChannel再次调用joinChannel会返回这个错误
 3. 超时：请检查自己网络是否正常
 4. 协议错误：服务器返回的错误，这种错误一般检查appid，token的值
 5. 不存在的房间：如果您以观众身份进入一个不存在的频道，则会出现该错误
 6. 进入一个私密房：如果进入一个私密房，且你没有在白名单内，会出现该错误，参考接口updateWhiteList
 7. 你被拉入黑名单：如果你的uid被主播加入黑名单，则无法进入该房间，参考接口updateBlackUids
 8. 你被主播踢出：你被主播踢出，一段时间内不能进房，踢出时间由主播指定，参考接口 kickUser、kickAll
 9. 其他，具体问题具体分析
 */
- (void)leaveChannel;

/**
 更新 Token
 
 该方法用于更新 Token。
 Token是用来做安全保障的，具有一定的有效期，如果Token即将失效了。发生 [CStoreMediaEngineCoreDelegate mediaEngine:tokenPrivilegeWillExpire:] 回调时 App 应重新获取 Token，然后调用该 API 更新 Token，否则 SDK 无法和服务器建立连接。
 更新token发生错误会通过 [CStoreMediaEngineCoreDelegate mediaEngine:didOccurError:]回调，收到CSMErrorCheckTokenFailed时可尝试重试。
 调用时机：该接口需要在进入频道成功后调用，在没有触发token告警的情况下，第三方也可以调用。
 
 @param token  在 App 服务器端生成的新Token
 */
- (void)renewToken:(NSString *)token;

/**
 主播踢出房间内指定用户
 
 被踢出的用户在 blockTime 指定的时间内进房会失败
 
 @param accessTokenVer token版本
 @param accessToken token
 @param kickUidList 被踢出的uid列表
 @param blockTime 被踢出的时长，单位秒
 @param completion 踢出的结果回调
 */
- (void)kickUser:(uint32_t)accessTokenVer
     accessToken:(NSString *)accessToken
     kickUidList:(nullable NSArray <NSNumber *>*)kickUidList
       blockTime:(uint32_t)blockTime
      completion:(void (^)(BOOL success, CSMErrorCode resCode))completion;

/**
 踢出房间内的所有用户
 
 被踢出的用户在 blockTime 指定的时间内进房会失败
 
 @param accessTokenVer token版本
 @param accessToken token
 @param blockTime 被踢出的时长，单位秒
 @param completion 踢出的结果回调
 */
- (void)kickAll:(uint32_t)accessTokenVer
    accessToken:(NSString *)accessToken
      blockTime:(uint32_t)blockTime
     completion:(void (^)(BOOL success, CSMErrorCode resCode))completion;

#pragma mark - Users

/**
 获取当前频道内的用户uid列表
 
 @param pageIdx 页面索引
 @param completion 回调用户uid列表
 */
- (void)queryChannelUsersWithPageIdx:(int32_t)pageIdx completion:(void (^)(int32_t resCode, int32_t pageIdx, int32_t totalPage, int64_t sid, NSArray<NSNumber *> *users))completion;

#pragma mark - User Account

/**
 注册本地用户 User Account
 
 该方法为本地用户注册一个 User Account。注册成功后，该 User Account 即可标识该本地用户的身份，用户可以使用它加入频道
 注意，使用同一 userAccount 多次调用该接口进行注册，会返回相同的uid。
 
 @param userAccount 第三方用户账号名，不可为空，业务方需要保证唯一
 @param completion 注册结果回调。回调中会给出注册结果：
 - 如果失败，则通过resCode提供对应的错误码；
 - 如果成功，则通过userInfo返回对应的用户uid。
 */
- (void)registerLocalUserAccount:(NSString *)userAccount
                      completion:(void (^)(BOOL success, CSMErrorCode resCode, CSMUserInfo *userInfo))completion;

/**
 通过 User Account 获取用户信息
 
 @param userAccount 第三方用户账号名，不可为空，业务方需要保证唯一
 @param completion 获取用户信息结果回调。回调中会给获取结果：
 - 如果失败，则通过resCode提供对应的错误码
 - 如果成功，则通过userInfo返回对应的用户uid
 */
- (void)getUserInfoByUserAccount:(NSString *)userAccount
                      completion:(void (^)(BOOL success, CSMErrorCode resCode, CSMUserInfo *userInfo))completion;

/**
 通过 UID 获取用户信息
 
 @param uid 用户uid
 @param completion 获取用户信息结果回调。回调中会给获取结果：
 - 如果失败，则通过resCode提供对应的错误码
 - 如果成功，则通过userInfo返回对应的用户uid
 */
- (void)getUserInfoByUid:(int64_t)uid
              completion:(void (^)(BOOL success, CSMErrorCode resCode, CSMUserInfo *userInfo))completion;


/**
 批量获取用户信息
 
 @param uid 用户自己的uid
 @param uids 要查询的用户uid列表
 @param completion 获取结果返回
 */
- (void)getUserInfoListByUid:(int64_t)uid
                        uids:(NSArray<NSNumber *> *)uids
                  completion:(void (^)(BOOL success, CSMErrorCode resCode, NSArray<CSMUserInfo *> *userInfos))completion;

#pragma mark - Core Audio

/**
 调节录音音量
 
 @param volume 录音信号音量，可在 0~200 范围内进行调节
 - 0：静音
 - 100：原始音量
 - 200：最大可为原始音量的 2 倍（自带溢出保护）
 */
- (void)adjustRecordingSignalVolume:(int)volume;

/**
 调节播放音量
 
 @param volume 播放信号音量，可在 0~200 范围内进行调节：
 - 0：静音
 - 100：原始音量
 - 200：最大可为原始音量的 2 倍（自带溢出保护）
 */
- (void)adjustPlaybackSignalVolume:(int)volume;

/**
 开/关本地音频采集
 
 当 App 加入频道时，它的语音功能默认是开启的。该方法可以关闭或重新开启本地语音功能，即停止或重新开始本地音频采集。
 该方法不影响接收或播放远端音频流，enableLocalAudio:NO 适用于只听不发的用户场景。
 语音功能关闭或重新开启后，会收到回调 [CStoreMediaEngineCoreDelegate mediaEngine:didMicrophoneEnabled:] 。
 该方法与 muteLocalAudioStream 的区别在于：
 - enableLocalAudio：开启或关闭本地语音采集及处理
 - muteLocalAudioStream：停止或继续发送本地音频流
 
 @param enabled 是否开启
 
 - YES：重新开启本地语音功能，即开启本地语音采集（默认）
 - NO：关闭本地语音功能，即停止本地语音采集
 */
- (void)enableLocalAudio:(BOOL)enabled;

/**
 停止/恢复发送本地音频流。
 
 - 静音/取消静音。该方法用于允许/禁止往网络发送本地音频流。
 - 成功调用该方法后，远端会触发  [CStoreMediaEngineCoreDelegate mediaEngine:didAudioMuted:byUid:]  回调。
 - 该方法不影响录音状态，并没有禁用麦克风
 
 @param mute 是否停止发送音视频
 - YES：停止发送本地音频流
 - NO：继续发送本地音频流（默认）
 */
- (void)muteLocalAudioStream:(BOOL)mute;

/**
 停止/恢复接收指定音频流。
 
 如果之前有调用过 muteAllRemoteAudioStreams: 停止接收所有远端音频流，在调用本 API 之前请确保你已调用 muteAllRemoteAudioStreams:NO
 muteAllRemoteAudioStreams: 是全局控制， muteRemoteAudioStream:mute: 是精细控制
 
 @param uid  指定的用户 ID
 @param mute 是否停止接收
 - YES：停止接收指定用户的音频流
 - NO：继续接收指定用户的音频流（默认）
 */
- (void)muteRemoteAudioStream:(uint64_t)uid mute:(BOOL)mute;

/**
 停止/恢复接收所有音频流。
 
 @param mute 是否停止接收
 - YES：停止接收所有远端音频流
 - NO：继续接收所有远端音频流（默认）
 */
- (void)muteAllRemoteAudioStreams:(BOOL)mute;


- (void)setIsTelephoneReceiverMode:(BOOL)isTelephoneReceiverMode; // Set YES to switch to the telephone receiver.

/**
 设置是否将语音路由设到扬声器（外放）
 
 @param enabled 是否将音频路由到外放
 - YES：切换到外放
 - NO：切换到听筒。如果设备连接了耳机，则语音路由走耳机
 */
- (void)setEnableSpeakerphone:(BOOL)enabled;

/**
 检查扬声器是否已开启。
 
 @return 是否开启
 */
- (BOOL)isSpeakerphoneEnabled;

/**
 获取当前网络连接状态
 
 @return 返回连接状态 CSMConnectionStateType
 */
- (CSMConnectionStateType)getConnectionState;




/**
 设置音频参数和场景
 @param profile  设置采样率，码率，编码模式和声道数，详见 BigoAudioProfile。
 @param scenario 设置音频应用场景。不同的音频场景下，设备的系统音量(媒体音量/通话音量)是不同的
 @return 调用成功
    - 0：设置成功
    - < 0：设置失败
*/
- (int)setAudioProfile:(BigoAudioProfile)profile scenario:(BigoAudioScenario)scenario;

/**
 注册语音数据监测对象，注册之后可以获取到采集的音频，播放的音频并对其修改
 @param observer 为 IAudioFrameObserver的C++实例，需要外部实现；如果传入 NULL，则表示取消注册；建议在leaveChannel 后调用，来释放语音观测器对象
 @return 调用成功
    - 0：设置成功
    - < 0：设置失败
*/
- (int)registerAudioFrameObserver:(long) observer;

#pragma mark - InEarMonitoring耳返
/**
是否启用耳返功能
@param enable 是否启用耳返
 
 - YES：启用耳返
 - NO：关闭耳返
*/
- (void)enableInEarMonitoring:(BOOL)enable;

/**
设置耳返音量大小
@param volume 音量大小0-200,不设置默认是100
*/
- (void)setInEarMonitoringVolume:(int)volume;

#pragma mark - Audio Effect Player
/**
获取音效文件的播放音量
@return volume 音量大小0-200.0
*/
- (double)getEffectsVolume;
/**
设置音效播放器的音量
@param volume 音量大小0-200.0，默认值100为原始音量
@return 调用成功
 
   - 0：设置成功
   - < 0：设置失败
*/
- (int)setEffectsVolume:(double)volume;
/**
设置soundId的音效文件的音量
@param soundId 音效文件的ID
@param volume 要设置的音量值
@return 调用成功
 
   - 0：设置成功
   - < 0：设置失败
*/
- (int)setVolumeOfEffect:(int)soundId withVolume:(double)volume;
/**
播放音效文件soundId
调用该接口后，会触发localAudioEffectStateChange回调
成功调用播放: state:BigoAudioMixingStatePlaying
播放失败:state:BigoAudioMixingStateStopped/BigoAudioMixingStateFailed
reasonCode:BigoAudioMixingErrorCanNotOpen
成功调用播放之后，会有文件播放进度回调
state:BigoAudioMixingStateProgress
reasonCode: 为进度时间（单位ms）
 
@param soundId 音效文件的ID
@param filePath 音效文件的绝对路径
@param loopCount 设置播放循环的次数；-1为无限循环；0为播放1次；1为播放2次；...；n为播放n+1次
@param pitch 需要改变的音高比例，范围[0.5,-2]，默认为1.0表示不改变； 目前尚不支持(设置为1.0)
@param pan 设定音效的空间位置。默认0.0，取值范围为 [-1.0, 1.0]， -1.0：音效出现在左边，0:音效出现在正前方，1.0：音效出现在右边；目前尚不支持(设置为0)
@param gain 设定音效的音量。默认100.0，取值范围为 [0, 100.0]
@param publish 是否将音效发给其他用户
@param progress 是否需要回调文件的播放进度
@return 调用成功
 
   - 0：设置成功
   - < 0：设置失败
*/
- (int)playEffect:(int)soundId filePath:(NSString *_Nullable)filePath loopCount:(int)loopCount pitch:(double)pitch pan:(double)pan gain:(double)gain publish:(BOOL)publish progress:(BOOL)progress;
/**
停止播放soundId的音效文件
调用该接口后，会触发localAudioEffectStateChange回调
@param soundId 音效文件的ID
@return 调用成功
   - 0：设置成功
   - < 0：设置失败
*/
- (int)stopEffect:(int)soundId;
/**
停止播放所有的音效文件
调用该接口后，会触发localAudioEffectStateChange回调
@return 调用成功
   - 0：设置成功
   - < 0：设置失败
*/
- (int)stopAllEffects;
/**
预加载的soundId的音效文件
@param soundId 音效文件的ID
@param filePath 音效文件的绝对路径
@return 调用成功
 
   - 0：设置成功
   - < 0：设置失败
*/
- (int)preloadEffect:(int)soundId filePath:(NSString *_Nullable)filePath;
/**
释放预加载的soundId的音效文件
@param soundId 音效文件的ID
@return 调用成功
 
   - 0：设置成功
   - < 0：设置失败
*/
- (int)unloadEffect:(int)soundId;
/**
暂停播放soundId的音效文件
调用该接口后，会触发localAudioEffectStateChange回调
@param soundId 音效文件的ID
@return 调用成功
 
   - 0：设置成功
   - < 0：设置失败
*/
- (int)pauseEffect:(int)soundId;
/**
暂停播放所有的音效文件
调用该接口后，会触发localAudioEffectStateChange回调
@return 调用成功
 
   - 0：设置成功
   - < 0：设置失败
*/
- (int)pauseAllEffects;
/**
恢复播放soundId的音效文件
调用该接口后，会触发localAudioEffectStateChange回调
@param soundId 音效文件的ID
@return 调用成功
 
   - 0：设置成功
   - < 0：设置失败
*/
- (int)resumeEffect:(int)soundId;
/**
恢复播放所有的音效文件
调用该接口后，会触发localAudioEffectStateChange回调
@return 调用成功
 
   - 0：设置成功
   - < 0：设置失败
*/
- (int)resumeAllEffects;
/**
获取音效文件的时长，单位为毫秒
@param soundId 音效文件的ID
@return 播放位置
*/
- (int)getEffectFileDuration:(int)soundId;
/**
获取当前音效文件的播放位置
@param soundId 音效文件的ID
@return 播放位置
*/
- (int)getCurrentEffectFilePlayPosition:(int)soundId;
/**
设置音效文件的播放位置
@param soundId 音效文件的ID
@param position 设置播放的位置
@return 调用成功
 
- 0：设置成功
- < 0：设置失败
*/
- (int)setCurrentEffectFilePlayPosition:(int)soundId currentPositon:(int) position;
//*************** 音效播放器 ***************//

#pragma mark - AudioMixing
//************* 伴奏播放器 ***************//
/**
开始播放音乐文件
 指定本地或在线音频文件来和麦克风采集的音频流进行混音或替换。替换是指用指定的音频文件替换麦克风采集到的音频流。该方法可以选择是否让对方听到本地播放的音频，并指定循环播放的次数。
 调用该接口后，会触发 localAudioMixingStateDidChanged回调。
 成功调用播放，
    state:BigoAudioMixingStatePlaying
 播放失败
    state:BigoAudioMixingStateStopped/BigoAudioMixingStateFailed
    reasonCode:BigoAudioMixingErrorCanNotOpen
成功调用播放之后，会有文件播放进度回调
    state:BigoAudioMixingStateProgress
    reasonCode: 播放文件的时间进度（单位ms）
@param filePath 需要播放的音频文件路径（目前支持的格式有mp3，m4a，aac，wav）
@param loopback YES: 只有本地可以听到混音或替换后的音频流//NO: 本地和对方都可以听到混音或替换后的音频流
@param replace  YES: 只推送设置的本地音频文件或者线上音频文件，不传输麦克风收录的音频。// NO: 音频文件内容将会和麦克风采集的音频流进行混音
@param cycle    指定音频文件循环播放的次数;  >0：循环次数//-1：无限循环
@return 调用成功
   - 0：设置成功
   - < 0：设置失败
*/
- (int)startAudioMixing:(NSString *_Nonnull)filePath loopback:(BOOL)loopback replace:(BOOL)replace cycle:(NSInteger)cycle;
/**
停止播放音乐文件
调用该接口后，会触发 localAudioMixingStateDidChanged回调。
注意
当回调state为BigoAudioMixingStateStopped停止时，
回调reasonCode为BigoAudioMixingStateStopped时表示文件播放结束
回调reasonCode为BigoAudioMixingPlayActiveStop时表示主动停止播放
@return 调用成功
   - 0：设置成功
   - < 0：设置失败
*/
- (int)stopAudioMixing;
/**
暂停播放音乐文件
调用该接口后，会触发localAudioMixingStateDidChanged回调
state:BigoAudioMixingStatePaused
 @return 调用成功
 
   - 0：设置成功
   - < 0：设置失败
*/
- (int)pauseAudioMixing;
/**
恢复播放音乐文件
调用该接口后，会触发localAudioMixingStateDidChanged回调
state:BigoAudioMixingStatePlaying
@return 调用成功
 
   - 0：设置成功
   - < 0：设置失败
*/
- (int)resumeAudioMixing;
/**
调节音乐文件的播放音量
@param volume  音乐文件播放音量范围为 0~200。默认 100 为原始文件音量
@return 调用成功
 
   - 0：设置成功
   - < 0：设置失败
*/
- (int)adjustAudioMixingVolume:(NSInteger)volume;
/**
调节音乐文件在本地播放的音量
@param volume  音乐文件播放音量范围为 0~200。默认 100 为原始文件音量
@return 调用成功
 
   - 0：设置成功
   - < 0：设置失败
*/
- (int)adjustAudioMixingPlayoutVolume:(NSInteger)volume;
/**
调节音乐文件在远端播放的音量
@param volume  音乐文件播放音量范围为 0~200。默认 100 为原始文件音量
@return 调用成功
 
  - 0：设置成功
  - < 0：设置失败
*/
- (int)adjustAudioMixingPublishVolume:(NSInteger)volume;
/**
获取音乐文件的本地播放音量
@return 返回音量值，范围为 [0,200]
*/
- (int)getAudioMixingPlayoutVolume;
/**
获取音乐文件的远端播放音量
@return 返回音量值，范围为 [0,200]
*/
- (int) getAudioMixingPublishVolume;
/**
获取音乐文件时长
@return 获取音乐文件时长，单位为毫秒
*/
- (int) getAudioMixingDuration;
/**
获取音乐文件当前播放位置，单位为毫秒。
*/
- (int) getAudioMixingCurrentPosition;
/**
设置音频文件的播放位置
@param pos 整数。进度条位置，单位为毫秒
@return 是否设置成功
 
 - 0：设置成功
 - < 0：设置失败
*/
- (int) setAudioMixingPosition:(NSInteger)pos;
//************* 伴奏播放器 ***************//

/**
 * 调整本地播放的音乐文件的音调<br/>
 * @param pitch 按半音音阶调整本地播放的音乐文件的音调，默认值为 0，即不调整音调。取值范围为 [-12,12]，每相邻两个值的音高距离相差半音。取值的绝对值越大，音调升高或降低得越多。
 * @return 0 ：调用成功 ；  < 0：调用失败
 */
- (int)setAudioMixingPitch:(NSInteger)pitch;

#pragma mark - audio volume
/**
 启用说话者音量提示
 
@param interval  指定音量提示的时间间隔. 启用该方法后，会在 reportAudioVolumeIndicationOfSpeakers 回调中按设置的时间间隔返回音量提示
 - <= 0： 禁用音量提示功能
 - > 0：提示间隔，单位为毫秒。建议设置到大于 200 毫秒。最小不得少于 20 毫秒。
@param smooth 指定音量提示的灵敏度。取值范围为 [0,10]，建议值为 3，数字越大，波动越灵敏；数字越小，波动越平滑。
@param report_vad 是否开启本地人声检测功能。除引擎自动进行本地人声检测的场景外，reportAudioVolumeIndicationOfSpeakers 回调的 vad 参数不会报告是否在本地检测到人声。
 - NO： （默认）关闭本地人声检测功能
 - YES：开启后， reportAudioVolumeIndicationOfSpeakers 回调的 vad 参数会报告是否在本地检测到人声。
*/
- (void)enableAudioVolumeIndication:(NSInteger)interval smooth:(NSInteger)smooth report_vad:(BOOL)report_vad;

//************* 变声功能 ***************//
#pragma mark - voice changer
/**
 设置本地语音音调， 现在不起作用
@param pitch  语音频率。可以在 [0.5,2.0] 范围内设置。取值越小，则音调越低。默认值为 1.0，表示不需要修改音调。
@return 是否设置成功
 - 0：设置成功
 - < 0：设置失败
*/
- (int)setLocalVoicePitch:(double)pitch;
/**
 设置本地语音音效均衡，现在不起作用
@param bandFrequency  频谱子带索引，取值范围是 [0,9]，分别代表 10 个 频带，对应的中心频率是 [31，62，125，250，500，1k，2k，4k，8k，16k] Hz，详见 AestronAudioEqualizationBandFrequency。
@param gain  每个 band 的增益，单位是 dB，每一个值的范围是 [-15,15]，默认值为 0。
@return 是否设置成功
 - 0：设置成功
 - < 0：设置失败
*/
- (int)setLocalVoiceEqualizationOfBandFrequency:(AestronAudioEqualizationBandFrequency)bandFrequency withGain:(NSInteger)gain;

/**
 设置本地音效混响，现在不起作用
@param reverbType  混响音效类型。参考AestronAudioReverbParamsType。
@param value 设置混响音效的效果数值。各混响音效对应的取值范围请参考AestronAudioReverbParamsType
@return 是否设置成功
 - 0：设置成功
 - < 0：设置失败
*/
- (int)setLocalVoiceReverbOfType:(AestronAudioReverbParamsType)reverbType withValue:(NSInteger)value;

/**
 设置变声器的类型
@param voiceChanger  本地语音的变声效果选项，默认原声。
@return 是否设置成功
 - 0：设置成功
 - < 0：设置失败
*/
- (int)setLocalVoiceChanger:(AestronAudioVoiceChanger)voiceChanger;

/**
 预设置本地语音混响
@param preset 本地语音混响选项，默认原声
@return 是否设置成功
 - 0：设置成功
 - < 0：设置失败
*/
- (int)setLocalVoiceReverbPreset:(AestronAudioReverbPreset)preset;

/**
 预设置本地语音均衡器，现在不起作用
@param preset 本地语音均衡器选项，默认原声
@return 是否设置成功
 - 0：设置成功
 - < 0：设置失败
*/
- (int)setLocalVoiceEqualizerPreset:(AestronAudioEqualizationPreset)preset;


#pragma mark - Core Video

/**
 设置播放渲染画面的主view
 
 设置播放渲染画面的主view，多次调用此接口只有最后一次设置设置的view有效。
 sdk支持两种view绘制模式
 单GLKView模式：设置一个渲染的GLKView，指定每个麦上用户显示的区域。参考接口为 [CStoreMediaEngineCore setVideoRenderers:]
 多UIView模式：每个麦上用户单独设置一个显示UIView，画面大小根据View的大小改变而改变。参考接口为setupLocalVideo()/setupRemoteVideo()
 设置视频播放的GLKView ，GLKView是用来绘制画面的容器。主播自己的预览画面和远端连麦用户的网络画面都绘制到该GLKView 上。每个用户通过CSMVideoRenderer来指定在该GLKView上的显示区域，参考CStoreMediaEngineCore#setupLocalVideo(CSMVideoCanvas)\CStoreMediaEngineCore#setupRemoteVideo(CSMVideoCanvas)分别设置本地画面区域和远端画面区域。
 
 @param rendererView 承载播放的主view
 */
- (void)attachRendererView:(GLKView *)rendererView;

/**
 打开多View绘制模式
 
 多View绘制模式使用[CStoreMediaEngineCore setupLocalVideo:]和[CStoreMediaEngineCore setupRemoteVideo:]进行视频绘制，打开该模式后，请不要再调用[CStoreMediaEngineCore attachRendererView:]和[CStoreMediaEngineCore setVideoRenderers:]接口
 请每次进房前先调用此接口，再调用以上接口(setupLocalVideo/setupRemoteVideo/attachRendererView/setVideoRenderers)，才能生效。
 
 @param enable 是否打开多View绘制模式
 - YES 打开
 - NO 关闭
 */
- (void)enableMultiViewRender:(BOOL)enable;

/**
 初始化本地视图
 
 该方法初始化本地视图并设置本地用户视频显示信息，只影响本地用户看到的视频画面，不影响本地发布视频。调用该方法绑定本地视频流的显示视窗(view)。
 注意：如果需要在进频道前进行视频预览，需要先把 [CSMVideoCanvas uid] 指定为0，进频道成功后再把uid改为真正的本地用户uid。
 
 @param local 设置本地视图属性
 */
- (void)setupLocalVideo:(CSMVideoCanvas *_Nullable)local;

/**
 初始化远端用户视图
 
 设置的CSMVideoCanvas参数里是UIView*类型，但SDK使用这个view时，会先检查有没有子view为GLKView，若有就用这个子view；若没有类型为GLKView的子view，那么SDK会创建一个GLKView并作为子view加入到这个view，之后渲染也有这个创建的子view。
 
 @param remote 远端画面的预览参数
 */
- (void)setupRemoteVideo:(CSMVideoCanvas *_Nonnull)remote;


/**
 划分视频渲染区域
 
 确保传入的矩形区域数组是正确的（width>0&&height>0，面积为正值）
 
 @param videoRenderers 表示渲染区域的数组
 */
- (void)setVideoRenderers:(NSArray<CSMVideoRenderer *> *)videoRenderers;

/**
 设置视频背景
 
 @param image 背景图
 */
- (void)setVideoBackgroundImage:(UIImage *)image;


/**
 设置镜像模式。
 设置远端是否镜像，不会使预览镜像
 对于前置摄像头，设置远端显示画面是否镜像；对于后置摄像头，不起作用
 @param mode 镜像模式
 */
- (void)setLocalVideoMirrorMode:(CSMVideoMirrorMode)mode;


/**
 打开本地预览
 
 打开本地预览（非自定义采集需要App在前台，在后台调用无效，对自定义采集则无此限制），并推送本地采集的视频到远端，SDK内部会执行以下操作：
 对自定义采集：开始自定义采集，会回调通知CStoreMediaEngineCoreDelegate#onStartCustomCaptureVideoWithMediaEngine(CStoreMediaEngineCore*)。多次调用此方法，会多次回调通知。
 对非自定义采集：开始摄像头采集，以及创建采集线程，预览美颜线程。多次连续调用此方法，只有第一次有效。
 只有调用了CStoreMediaEngineCore#enableLocalVideo(BOOL)参数为YES后，调用此方法才有效。
 */
- (void)startPreview;

/**
 关闭本地预览
 
 关闭本地预览，并停止推送本地采集的视频到远端，SDK内部会执行以下操作：
 对自定义采集：停止自定义采集，会回调通知CStoreMediaEngineCoreDelegate#onStopCustomCaptureVideoWithMediaEngine(CStoreMediaEngineCore*)。多次调用此接口，会多次回调通知。
 对非自定义采集：停止摄像头采集，以及停止采集线程，预览美颜线程。多次连续调用此接口，只有第一次有效。
 */
- (void)stopPreview;

/**
 开/关本地视频采集。
 
 该方法禁用/启用本地视频功能。enableLocalVideo:NO 适用于只看不发的视频场景。
 使用该方法可以开启或关闭本地视频，且不影响接收远端视频。
 调用时机：在进房前调用
 @param enabled 是否启用本地视频
 
 - YES：开启本地视频采集和渲染（默认）
 - NO：关闭使用本地摄像头设备。关闭后，远端用户会接收不到本地用户的视频流；但本地用户依然可以接收远端用户的视频流。设置为 NO 时，该方法不需要本地有摄像头。
 */
- (void)enableLocalVideo:(BOOL)enabled;

/**
 停止/恢复发送本地视频流。
 
 成功调用该方法后，远端会触发 [CStoreMediaEngineCoreDelegate mediaEngine:didVideoMuted:byUid:] 回调
 
 调用该方法时，SDK 不再发送本地视频流，但摄像头仍然处于工作状态。相比于 enableLocalVideo:NO 用于控制本地视频流发送的方法，该方法响应速度更快。
 
 该方法不影响本地视频流获取，没有禁用摄像头。
 
 @param mute 发送本地视频流
 
 - YES：不发送本地视频流
 - NO：发送本地视频流（默认）
 */
- (void)muteLocalVideoStream:(BOOL)mute;

/**
 停止/恢复接收指定视频流。
 
 如果之前有调用过 muteAllRemoteVideoStreams:YES 停止接收所有远端视频流，在调用本 API 之前请确保你已调用 muteAllRemoteVideoStreams:NO 。
 muteAllRemoteVideoStreams 是全局控制所有远端视频流的接收，muteRemoteVideoStream:mute 是精细控制某个用户视频流的接收。
 
 @param uid   指定的用户 ID
 @param mute 是否停止
 
 - YES：停止接收指定用户的视频流
 - NO：继续接收指定用户的视频流（默认）
 */
- (void)muteRemoteVideoStream:(uint64_t)uid mute:(BOOL)mute;


/**
 清除用户视频首帧回调标志位。
 
 在[CStoreMediaEngineCoreDelegate mediaEngine:didVideoMuted:byUid:] ，且muted=NO的回调中调用该接口，可清除首帧回调标记，用户下次接收到远端用户的视频帧会再次触发回调 [CStoreMediaEngineCoreDelegate mediaEngine:firstRemoteVideoFrameOfUid:size:elapsed:]
 
 @param uid   需要清除标志位的用户uid
 
 */
- (void)clearFirstVideoFrameFlagWithUid:(uint64_t)uid;

/**
 停止/恢复接收所有视频流。
 
 调用该方法成功后，SDK会发送PCS_ClientMute消息到服务器，远端会触发 [CStoreMediaEngineCoreDelegate mediaEngine:didVideoMuted:byUid:] 回调
 
 @param mute 是否停止
 
 - YES：停止接收所有远端视频流
 - NO：继续接收所有远端视频流（默认）
 */
- (void)muteAllRemoteVideoStreams:(BOOL)mute;



/**
 切换前置/后置摄像头。
 
 非自采集模式下的前/后置摄像头切换。
 此接口不需要先调用startPreview也可以生效，不存在时序关系；
 调用此方法时不需要App在前台也可以生效。
 */
- (void)switchCamera;

/**
 检测设备是否支持闪光灯常开。
 
 一般情况下，App 默认开启前置摄像头，因此如果你的前置摄像头不支持闪光灯常开，直接使用该方法会返回 NO。
 如果需要检查后置摄像头是否支持闪光灯常开，需要先使用 switchCamera 切换摄像头，再使用该方法。
 
 获取摄像头是否支持闪光灯常开的结果：
 对于非自采集模式：只有开始采集后，才能获取，否则都返回NO。
 对于自采集模式：返回NO。
 
 @return 是否支持闪光灯常开
 
 - YES: 设备支持闪光灯常开
 - NO：设备不支持闪光灯常开
 */
- (BOOL)isCameraTorchSupported;

/**
 设置非自采集摄像头是否打开闪光灯
 只有开始采集后，才能设置成功。
 
 @param isOn 是否打开闪光灯
 - YES：打开
 - NO：关闭
 */
- (void)setCameraTorchOn:(BOOL)isOn;

/**
 设置所有视频合流后的最大分辨率和帧率
 
 该方法在进入房间/预览前后皆可调用；
 在自采集模式下(用户自行管理摄像头)，进房前后调用没有区别；
 在sdk内部采集模式下有以下区别:
 在进房/预览前调用，则采集分辨率会根据设置的最大编码分辨率进行选择；
 在进房/预览后调用，则摄像头采集分辨率默认为480x640。
 在当前设备性能及网络状况良好的情况下，编码分辨率及帧率会达到设置的最大值；
 当设备性能及网络状况变差时，SDK拥有网络对抗性，会自适应调整编码分辨率及帧率以保证视频流畅。网络恢复后，将恢复至设置的最大编码分辨率及帧率。
 
 @param maxResolutionType 最大分辨率
 @param maxFrameRate 最大帧率
 */
- (void)setAllVideoMaxEncodeParamsWithMaxResolution:(CSMResolutionType)maxResolutionType maxFrameRate:(CSMFrameRate)maxFrameRate;

/**
 * 设置视频横竖屏模式，仅支持 AestronVideoOrientation 定义的枚举值
 * @param videoOrientation 横竖屏
 
 */
- (void)setLocalVideoOrientation:(AestronVideoOrientation)videoOrientation;

/**
 * 设置/取消本地开播视频流图片水印效果，当前仅支持png格式，多次设置会覆盖
 * 注：如果水印参数设置宽高与图片实际大小不一致时，会按设置尺寸压缩拉伸
 *
 * @param waterMarkOptions 水印参数设置选项，为nil时取消水印效果
 * @param completion 设置水印的回调通知
 * - 通过resCode回调水印设置结果
 *
 */
- (void)setVideoWaterMarkWithOptions:(CSEWaterMarkOptions * _Nullable)waterMarkOptions completion:(void (^)(AestronWaterMarkResCode resCode))completion;

#pragma mark - Basic Beauty
/**
 设置滤镜类型以及强度
 
 @param filterPath 滤镜素材路径，传入nil则关闭滤镜
 @param level 滤镜级别，范围[0,100]，0关闭，100强度最大
 */
- (void)setBeautifyFilterWithPath:(NSString * _Nullable)filterPath level:(int)level;

/**
 设置美白强度
 
 @param whiteResourcePath 美白素材路径，传入nil则取消美白
 @param level 美白强度，范围[0,100]，0关闭，100强度最大
 */
- (void)setBeautifyWhiteSkinWithPath:(NSString * _Nullable)whiteResourcePath level:(int)level;

/**
 设置磨皮强度
 
 @param level    磨皮强度[0,100]， 0关闭，100为最高磨皮强度
 */
- (void)setBeautifySmoothSkinWithLevel:(int)level;

#pragma mark - Advanced Beauty
/**
 初始化人脸识别引擎，大眼瘦脸和贴纸功能需要识别人脸，在使用这些功能前，请先初始化人脸识别引擎
 
 内部会在当前线程进行引擎初始化，第一次初始化较耗时，调用方需要考虑在工作线程中调用
 
 @param modelPath 模型文件目录
 */
- (void)enableVenusEngineWithModelPath:(NSString *)modelPath;

/**
 设置大眼等级
 
 调整大眼效果之前，务必先调用 CStoreMediaEngineCore#enableVenusEngineWithModelPath: 初始化人脸识别引擎，否则调整无效
 也要确保调用者有权限使用高级美颜功能，否则调用无效
 
 @param level 大眼级别，范围[0,100]，0关闭，100强度最大
 */
- (void)setBeautifyBigEyeWithLevel:(int)level;

/**
 设置瘦脸等级
 
 调整瘦脸效果之前，务必先调用 enableVenusEngineWithModelPath: 初始化人脸识别引擎，否则调整无效
 也要确保调用者有权限使用高级美颜功能，否则调用无效
 
 @param level 瘦脸级别，范围[0,100]，0关闭，100强度最大
 */
- (void)setBeautifyThinFaceWithLevel:(int)level;

/**
 设置贴纸
 
 设置贴纸之前，务必先调用 enableVenusEngineWithModelPath: 初始化人脸识别引擎，否则调整无效
 也要确保调用者有权限使用高级美颜功能，否则调用无效
 
 @param path 贴纸素材路径，传入nil则取消贴纸，传入无效的素材路径，也会取消已有的贴纸效果
 */
- (void)setStickerWithPath:(NSString * _Nullable)path;

/**
 获取当前帧的人脸数量
 
 调用一次该方法只会获取一次，SDK 会在本地触发  [CStoreMediaEngineCoreDelegate mediaEngine:onFaceNumGot:]  回调
 
 获取之前，务必先调用 enableVenusEngineWithModelPath: 初始化人脸识别引擎，否则获取无效
 */
- (void)getFaceNum;

#pragma mark CDN Live Streaming
/**
 增加旁路推流地址
 
 增加旁路推流地址，仅触发旁路推流，不会触发转码合流，触发转码合流请调用 [CStoreMediaEngineCore setLiveTranscoding:]
 该方法用于添加旁路推流地址，调用该方法后，SDK 会在本地触发  [CStoreMediaEngineCoreDelegate mediaEngine:rtmpStreamingChangedToState:state:errorCode:]  回调，报告增加旁路推流地址的状态。
 - 该方法仅适用于直播场景。
 - 请确保在成功加入频道后再调用该接口。
 - 该方法每次只能增加一路旁路推流地址。若需推送多路流，则需多次调用该方法。
 
 @param url CDN 推流地址，格式为 RTMP，不支持中文等特殊字符。
 
 @return 接口调用结果
 
 - 0：调用成功
 - < 0：调用失败
  - -1：在非直播场景调用
  - -2：url长度为0或者为nil
  - -3：在成功加入频道前调用
  - -4：未知错误
 */
- (int)addPublishStreamUrl:(NSString *_Nonnull)url;
/**
 删除旁路推流地址
 
 该方法用于删除旁路推流过程中已经设置的 RTMP 推流地址。调用该方法后，SDK 会在本地触发 [CStoreMediaEngineCoreDelegate mediaEngine:rtmpStreamingChangedToState:state:errorCode:] 回调，报告删除旁路推流地址的状态。
 
 - 该方法仅适用于直播场景。
 - 请确保在成功加入频道后再调用该接口。
 - 该方法每次只能删除一路旁路推流地址。若需删除多路流，则需多次调用该方法。
 
 @param url CDN 推流地址，格式为 RTMP，不支持中文等特殊字符。
 
 @return 接口调用结果
 
 - 0：调用成功
 - < 0：调用失败
    - -1：在非直播场景调用
    - -2：url长度为0或者为nil
    - -3：在成功加入频道前调用
    - -4：未知错误
 */
- (int)removePublishStreamUrl:(NSString *_Nonnull)url;

/**
 设置直播合流转码参数
 
 设置直播合流转码参数，并启动合流转码，如果已经启动转码合流，则会更新合流转码的相关参数
 该方法用于旁路推流的视图布局及音频设置。 该方法需要和addPublishStreamUrl旁路推流一起使用，将用户画面按照设置的合流参数进行合流后再旁路转推到指定的url，观看端可以通过旁路转推的地址观看合流后的画面。
 调用该方法更新转码设置后本地会触发 [CStoreMediaEngineCoreDelegate mediaEngineTranscodingUpdated:] 回调。
 调用该方法更新转码参数 LiveTranscoding成功时，SDK 会触发 onTranscodingUpdated 回调，代表合流参数触发和更新成功。
 推流状态的有更新会触发 onRtmpStreamingStateChanged回调，用户可以根据这个状态回调观察推流情况。
 调用时机：在 加入频道 成功之后调用，内部设置时需要带上 joinChannelServerTs 参数
 
 @param transcoding 一个 CSMLiveTranscoding 的对象，详细设置见 CSMLiveTranscoding。
 @return 接口调用结果
 
 - 0: 方法调用成功；
 - < 0: 方法调用失败。
    - -3：在成功加入频道前调用
    - -4：未知错误
    - -5：视频布局参数无效，请参照API文档检查[CSMLiveTranscoding size]，[CSMTranscodingUser rect]，[CSMTranscodingImage rect]几个参数是否有误
 */
- (int)setLiveTranscoding:(CSMLiveTranscoding *)transcoding;

/**
 停止转码合流
 
 调用建议：先停止旁路转推，再调用该接口停止合流
 
 @return 接口调用结果
 
 - 0: 方法调用成功；
 - < 0: 方法调用失败。目前没有调用返回 < 0 的情况，都是return 0
 */
- (int)stopLiveTranscoding;

/**
 加入频道PK
 
 如果您想和其他频道的主播跨房间PK，则调用该方法加入PK频道
  
 @param token 进房所携带的校验token
 @param channelName 要PK的频道名
 @param myUid 您的uid
 @param completion 加入后信息的回调通知
 */
- (void)joinPkChannelWithToken:(NSString *)token
                   channelName:(NSString *)channelName
                         myUid:(uint64_t)myUid
                    completion:(void(^)(BOOL success,
                                        CSMErrorCode resCode,
                                        NSString *channel,
                                        uint64_t uid,
                                        NSInteger useTime))completion;

/**
 退出PK频道
 */
- (void)leavePkChannel;

/**
是否启用通话模式

@param isUseCallMode 是否是通话模式
 - YES：启用通话模式(通话音量)
 - NO：不启用通话模式(媒体音量)
*/
- (void)setIsUseCallMode:(BOOL)isUseCallMode;

/**
 开始或停止自定义视频采集
 
 必须在引擎启动前设置，即在setClientRole、joinChannelWithUid等接口之前调用。
 调用leaveChannel退出频道时，SDK会重置状态，停止自定义视频采集，所以如果要保持自定义视频采集开启状态，需每次进入频道前调用该接口进行设置
 当开发者开启自定义采集时，通过CStoreMediaEngineCoreDelegate中相关回调接口接收开始采集、结束采集等通知事件
 
 @param enable 是否打开自定义视频采集
 - YES 打开自定义视频采集
 - NO 停止自定义视频采集
 */
- (void)enableCustomVideoCapture:(BOOL)enable;

/**
 向SDK 发送自定义采集的视频帧
 
 向 SDK 发送自定义采集的视频帧 CVPixelBuffer 数据，仅支持kCVPixelFormatType_420YpCbCr8BiPlanarFullRange格式
 该接口应该在 [CStoreMediaEngineCoreDelegate onStartCustomCaptureVideoWithMediaEngine:] 通知之后开始调用，在 [CStoreMediaEngineCoreDelegate onStopCustomCaptureVideoWithMediaEngine:] 通知之后结束调用。
 如果未开启自定义视频采集，调用该接口向SDK发送数据是不会生效的

 @param frameBuffer 要向 SDK 发送的视频帧数据
*/
- (void)sendCustomVideoCapturePixelBuffer:(CVPixelBufferRef)frameBuffer;

/**
 * 设置App配置信息，该接口需要在joinChannel前调用
 *
 * @param appConfigs App配置信息
 * CSAppConfigure 支持的配置信息如下：
    1、countryCode：用户所在地区的国家码,SDK能根据用户的国家码去接入更优的直播线路。国家码限定为2个字节，如果多余2个字节，SDK会截断。
 */
- (void)setAppConfigure:(CSAppConfigure*)appConfigs;

#pragma mark - Log
/**
 * 是否允许SDK进行log捞取操作。
 * @param enable YES的时候允许捞取log，NO的时候禁止捞取。默认是YES
*/
- (void)enableUploadLog:(BOOL)enable;


/**
 * 主动上传SDK log.
 * 该方法是异步执行的，不会阻塞主线程
 * @param completion 调用结果回调，错误码参考CSMErrorUploadLogErrorCode
*/
- (void)uploadLogWithCompletion:(void(^)(CSMErrorUploadLogErrorCode resCode))completion;


- (NSString *)mssdkDebugInfo;
@end

NS_ASSUME_NONNULL_END

#endif /* CStoreMediaEngineCore_Public_h */
