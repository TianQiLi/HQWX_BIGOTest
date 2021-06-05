//
//  CSEWaterMarkOptions.h
//  CStoreMediaEngineKit
//
//  Created by Bo Yu on 2021/4/14.
//  Copyright © 2021 BIGO Inc. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface CSEWaterMarkOptions : NSObject

@property (nonatomic, strong) UIImage *waterMarkImage;  /// 水印图片
@property (nonatomic, assign) int x;                    /// 以预览视图左上角为原点，水印的横坐标。单位是像素
@property (nonatomic, assign) int y;                    /// 以预览视图左上角为原点，水印的纵坐标。单位是像素
@property (nonatomic, assign) int width;                /// 水印宽度。单位是像素。值为0表示以图片尺寸为准。
@property (nonatomic, assign) int height;               /// 水印高度。单位是像素。值为0表示以图片尺寸为准。

@end

NS_ASSUME_NONNULL_END
