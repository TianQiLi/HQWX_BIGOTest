Pod::Spec.new do |s|

 s.name = 'HQWX_BIGOTest'
 s.version = '1.0.0'
 s.summary = 'BiGO test hqwx '
 s.homepage = 'https://github.com/TianQiLi/HQWX_BIGOTest.git'
 s.license = { :'type' => 'Copyright', :'text' => ' Copyright 2020 tql '}
 s.authors = {"litianqi" => "litianqi@hqwx.com"}
 s.description  = <<-DESC
                          WKwebview
                         DESC
 
 
 s.source = { :path => "."}
 s.platform = :ios, '8.0'
 s.requires_arc  = true
 s.vendored_frameworks = 'CStoreMediaEngineKit.framework'
 s.frameworks = 'UIKit','Foundation','CoreGraphics','WebKit','CoreLocation','SystemConfiguration'
 #s.libraries = 'sqlite3' , 'c++', 'z'
 s.resources = 'CStoreMediaEngineKit.framework/*bundle'
 

end 
