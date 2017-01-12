set dst="..\..\..\Common\bin\navigator\Release"
set vc="%1"

if not exist "..\..\..\Dependencies\navigatorModule\include" mkdir ..\..\..\Dependencies\navigatorModule\include
copy .\NavigatorModule.h ..\..\..\Dependencies\navigatorModule\include\. /y
copy .\Tools\Event.h ..\..\..\Dependencies\navigatorModule\include\. /y
copy .\MainApplication\IApplication.h ..\..\..\Dependencies\navigatorModule\include\. /y
copy .\MainApplication\IInstance.h ..\..\..\Dependencies\navigatorModule\include\. /y
copy .\MainApplication\IWindow.h ..\..\..\Dependencies\navigatorModule\include\. /y
copy .\OgreTools\OgreHelpers.h ..\..\..\Dependencies\navigatorModule\include\. /y
copy .\OgreTools\ExternalTextureSourceEx.h ..\..\..\Dependencies\navigatorModule\include\. /y

if not exist ..\..\..\Dependencies\navigatorModule\lib\Release mkdir ..\..\..\Dependencies\navigatorModule\lib\Release
copy ReleaseNCS\Navigator.lib ..\..\..\Dependencies\navigatorModule\lib\Release\. /y
copy ReleaseNCS\Navigator.dll ..\..\..\Dependencies\navigatorModule\lib\Release\. /y

if not exist "%dst%" mkdir "%dst%"
copy ReleaseNCS\Navigator.dll "%dst%" /y
if exist "%dst%\OgreMain.dll" goto OgreMain_exist
 if exist "%OGRE_HOME%\lib\OgreMain.dll" xcopy "%OGRE_HOME%\lib\OgreMain.dll" "%dst%" /s /y
 if exist "%OGRE_HOME%\bin\release\OgreMain.dll" xcopy "%OGRE_HOME%\bin\release\OgreMain.dll" "%dst%" /s /y
:OgreMain_exist
if exist "%dst%\Plugin_BSPSceneManager.dll" goto Plugin_BSPSceneManager_exist
 if exist "%OGRE_HOME%\lib\Plugin_BSPSceneManager.dll" xcopy "%OGRE_HOME%\lib\Plugin_BSPSceneManager.dll" "%dst%" /s /y
 if exist "%OGRE_HOME%\bin\release\Plugin_BSPSceneManager.dll" xcopy "%OGRE_HOME%\bin\release\Plugin_BSPSceneManager.dll" "%dst%" /s /y
:Plugin_BSPSceneManager_exist
if exist "%dst%\Plugin_OctreeSceneManager.dll" goto Plugin_OctreeSceneManager_exist
 if exist "%OGRE_HOME%\lib\Plugin_OctreeSceneManager.dll" xcopy "%OGRE_HOME%\lib\Plugin_OctreeSceneManager.dll" "%dst%" /s /y
 if exist "%OGRE_HOME%\bin\release\Plugin_OctreeSceneManager.dll" xcopy "%OGRE_HOME%\bin\release\Plugin_OctreeSceneManager.dll" "%dst%" /s /y
:Plugin_OctreeSceneManager_exist
if exist "%dst%\Plugin_OctreeZone.dll" goto Plugin_OctreeZone_exist
 if exist "%OGRE_HOME%\lib\Plugin_OctreeZone.dll" xcopy "%OGRE_HOME%\lib\Plugin_OctreeZone.dll" "%dst%" /s /y
 if exist "%OGRE_HOME%\bin\debug\Plugin_OctreeZone.dll" xcopy "%OGRE_HOME%\bin\debug\Plugin_OctreeZone.dll" "%dst%" /s /y
:Plugin_OctreeZone_exist
if exist "%dst%\Plugin_PCZSceneManager.dll" goto Plugin_PCZSceneManager_exist
 if exist "%OGRE_HOME%\lib\Plugin_PCZSceneManager.dll" xcopy "%OGRE_HOME%\lib\Plugin_PCZSceneManager.dll" "%dst%" /s /y
 if exist "%OGRE_HOME%\bin\debug\Plugin_PCZSceneManager.dll" xcopy "%OGRE_HOME%\bin\debug\Plugin_PCZSceneManager.dll" "%dst%" /s /y
:Plugin_PCZSceneManager_exist
if exist "%dst%\Plugin_ParticleFX.dll" goto Plugin_ParticleFX_exist
 if exist "%OGRE_HOME%\lib\Plugin_ParticleFX.dll" xcopy "%OGRE_HOME%\lib\Plugin_ParticleFX.dll" "%dst%" /s /y
 if exist "%OGRE_HOME%\bin\release\Plugin_ParticleFX.dll" xcopy "%OGRE_HOME%\bin\release\Plugin_ParticleFX.dll" "%dst%" /s /y
:Plugin_ParticleFX_exist
if exist "%dst%\Plugin_CgProgramManager.dll" goto Plugin_CgProgramManager_exist
 if exist "%OGRE_HOME%\lib\Plugin_CgProgramManager.dll" xcopy "%OGRE_HOME%\lib\Plugin_CgProgramManager.dll" "%dst%" /s /y
 if exist "%OGRE_HOME%\bin\release\Plugin_CgProgramManager.dll" xcopy "%OGRE_HOME%\bin\release\Plugin_CgProgramManager.dll" "%dst%" /s /y
:Plugin_CgProgramManager_exist
if exist "%dst%\RenderSystem_Direct3D9.dll" goto RenderSystem_Direct3D9_exist
 if exist "%OGRE_HOME%\lib\RenderSystem_Direct3D9.dll" xcopy "%OGRE_HOME%\lib\RenderSystem_Direct3D9.dll" "%dst%" /s /y
 if exist "%OGRE_HOME%\bin\release\RenderSystem_Direct3D9.dll" xcopy "%OGRE_HOME%\bin\release\RenderSystem_Direct3D9.dll" "%dst%" /s /y
:RenderSystem_Direct3D9_exist
if exist "%dst%\RenderSystem_GL.dll" goto RenderSystem_GL_exist
 if exist "%OGRE_HOME%\lib\RenderSystem_GL.dll" xcopy "%OGRE_HOME%\lib\RenderSystem_GL.dll" "%dst%" /s /y
 if exist "%OGRE_HOME%\bin\release\RenderSystem_GL.dll" xcopy "%OGRE_HOME%\bin\release\RenderSystem_GL.dll" "%dst%" /s /y
:RenderSystem_GL_exist
if exist "%dst%\cg.dll" goto cg_exist
 if exist "%OGRE_HOME%\Samples\Common\bin\Release\cg.dll" xcopy "%OGRE_HOME%\Samples\Common\bin\Release\cg.dll" "%dst%" /s /y
 if exist "%OGRE_HOME%\bin\release\cg.dll" xcopy "%OGRE_HOME%\bin\release\cg.dll" "%dst%" /s /y
:cg_exist
if exist "%dst%\OIS.dll" goto OIS_exist
 if exist "%OGRE_HOME%\Samples\Common\bin\Release\OIS.dll" xcopy "%OGRE_HOME%\Samples\Common\bin\Release\OIS.dll" "%dst%" /s /y
 if exist "%OGRE_HOME%\bin\release\OIS.dll" xcopy "%OGRE_HOME%\bin\release\OIS.dll" "%dst%" /s /y
:OIS_exist
if not exist "%dst%\pthreadVC.dll" xcopy "..\..\..\Dependencies\pthreads\lib\pthreadVC.dll" "%dst%" /s /y
if not exist "%dst%\xul.dll" xcopy "..\..\..\Dependencies\llmozlib\win32_%vc%\runtime\Release\*.*" "%dst%" /s /y
xcopy "..\..\..\Dependencies\protocols\XmlDatas\lib\ReleaseDLL\XmlDatas.dll" "%dst%" /s /y
xcopy "..\..\..\Dependencies\protocols\P2NNullClientServer\ReleaseDLL\P2NNullClientServer.dll" "%dst%" /s /y
xcopy "..\..\..\Dependencies\peerModule\lib\Release\Peer.dll" "%dst%" /s /y
if exist "..\..\..\Dependencies\expat\lib\ReleaseDLL\libexpat.dll" xcopy "..\..\..\Dependencies\expat\lib\ReleaseDLL\libexpat.dll" "%dst%" /s /y
if not exist "%dst%\NaviLocal\" xcopy "..\..\..\Media\NaviLocal\*.*" "%dst%\NaviLocal" /s /y /i
if not exist "%dst%\lua\" xcopy "..\..\..\Media\lua\*.*" "%dst%\lua" /s /y /i
if not exist "%dst%\fmodex.dll" xcopy "%FMOD_PATH%\fmodex.dll" "%dst%" /s /y
if not exist "%dst%\plugin_3ds.dll" xcopy "..\..\..\Dependencies\plugin_3ds\lib\plugin_3ds.dll" "%dst%" /s /y
if not exist "%dst%\plugin_skp.dll" xcopy "..\..\..\Dependencies\plugin_skp\lib\Release\*.dll" "%dst%" /s /y
if not exist "%dst%\RakNet.dll" xcopy "%RAKNET_PATH%\Lib\RakNet.dll" "%dst%" /s /y
if not exist "%dst%\libcurl.dll" xcopy "..\..\..\Dependencies\curl\lib\ReleaseDLL\libcurl.dll" "%dst%" /s /y
xcopy "..\..\..\Dependencies\navigatorConfigDialog\lib\WIN32\Release\navigatorConfigDialog.dll" "%dst%" /s /y
xcopy "..\..\..\Dependencies\VoiceEngine\lib\Release\voiceEngine.dll" "%dst%" /s /y
xcopy "..\..\..\Dependencies\Phonetizer\lib\Release\Phonetizer.dll" "%dst%" /s /y
xcopy "..\..\..\Dependencies\FaceController\lib\Release\FaceController.dll" "%dst%" /s /y
xcopy "..\..\..\Dependencies\libiconv\lib\libiconv2.dll" "%dst%" /s /y
xcopy "..\..\..\Dependencies\gecode-2.2.0\bin\*.dll" "%dst%" /s /y
xcopy "..\..\..\Dependencies\navi\bin\Navi.dll" "%dst%" /s /y
xcopy "..\..\..\Dependencies\navi\bin\Awesomium.dll" "%dst%" /s /y
xcopy "..\..\..\Dependencies\navi\bin\icudt38.dll" "%dst%" /s /y

if exist "%dst%\Plugins.cfg" goto Plugins_exist
 if exist "%OGRE_HOME%\Samples\Common\bin\Release\Plugins.cfg" xcopy "%OGRE_HOME%\Samples\Common\bin\Release\Plugins.cfg" "%dst%" /s /y
 if exist "%OGRE_HOME%\bin\release\Plugins.cfg" xcopy "%OGRE_HOME%\bin\release\Plugins.cfg" "%dst%" /s /y
:Plugins_exist
if exist "%dst%\quake3settings.cfg" goto quake3settings_exist
 if exist "%OGRE_HOME%\Samples\Common\bin\Release\quake3settings.cfg" xcopy "%OGRE_HOME%\Samples\Common\bin\Release\quake3settings.cfg" "%dst%" /s /y
 if exist "%OGRE_HOME%\bin\release\quake3settings.cfg" xcopy "%OGRE_HOME%\bin\release\quake3settings.cfg" "%dst%" /s /y
:quake3settings_exist
xcopy "..\..\..\Media\resources.cfg" "%dst%" /s /y
xcopy "..\..\..\Media\declarativemodeling.cfg" "%dst%" /s /y

call copyDocumentation.bat
call copyNCSPhysXFiles.bat
