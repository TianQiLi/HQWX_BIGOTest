//
//  CSAppConfigure.h
//  CStoreMediaEngineKit
//
//  Created by 周好冲 on 2020/12/12.
//  Copyright © 2020 BIGO Inc. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface CSAppConfigure : NSObject

/**
 * 用户所在地区的国家码
 * SDK能根据用户国家码去接入更优的直播线路
 */
@property (nonatomic, copy) NSString* countryCode;


/**
 * 开启音视频无延时模式
 * 开启该模式后，无论是主播还是观众看到远端的画面都是极小延时的，网络条件好的情况下无延时。
 * 开启该模式后请确保网络质量良好的情况下观看视频。
 * 默认是关闭
*/
@property (nonatomic, assign) BOOL enableNoDelayMode;

- (void)setCountryCode:(NSString * _Nonnull)countryCode;

@end

NS_ASSUME_NONNULL_END
