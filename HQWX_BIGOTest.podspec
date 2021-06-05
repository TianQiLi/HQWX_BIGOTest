Pod::Spec.new do |s|

 s.name = 'HQWX_BIGOTest'
 s.version = '0.0.11'
 s.summary = 'this is a  BiGO test hqwx  '
 s.homepage = 'https://github.com/TianQiLi/HQWX_BIGOTest.git'
 s.license = { :'type' => 'Copyright', :'text' => ' Copyright 2020 tql '}
 s.authors = {"litianqi" => "litianqi@hqwx.com"}
 s.description  = <<-DESC
                          WKwebview  test testsetstsetstsetstsstest
                         DESC
 
 
 s.source = {:git => "https://github.com/TianQiLi/HQWX_BIGOTest.git", :tag =>"#{s.version}"}
 s.platform = :ios, '9.0'
 s.requires_arc  = true
 s.vendored_frameworks = 'CStoreMediaEngineKit.framework'
 s.frameworks = 'UIKit','Foundation','CoreGraphics','WebKit','CoreLocation','SystemConfiguration','GLKit','Security'
  s.libraries = 'c++' ,'iconv' , 'z'
  s.resources =  'CStoreMediaEngineKit.framework/*.bundle'
 
    s.xcconfig = { 'VALID_ARCHS' => 'armv7 arm64 x86_64' }
end 
