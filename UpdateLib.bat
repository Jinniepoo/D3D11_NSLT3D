

// // xcopy .복사해야할 파일이 있는 경로		.복사받을 경로 

xcopy /y/s  .\Engine\Public\*.*										.\EngineSDK\Inc\
xcopy /y	.\Engine\Bin\Engine.lib									.\EngineSDK\Lib\
xcopy /y	.\Engine\ThirdPartyLib\*.lib							.\EngineSDK\Lib\
xcopy /y	.\Engine\Bin\Engine.dll									.\Client\Bin\
xcopy /y	.\Engine\Bin\ShaderFiles\*.*							.\Client\Bin\ShaderFiles\
