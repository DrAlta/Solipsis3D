set vc="%1"

if not exist "..\..\..\Dependencies\peerModule\include" mkdir ..\..\..\Dependencies\peerModule\include
copy PeerModule.h ..\..\..\Dependencies\peerModule\include\. /y
copy IPeer.h ..\..\..\Dependencies\peerModule\include\. /y
copy OgreHelpers.h ..\..\..\Dependencies\peerModule\include\. /y
copy IPhysics*.h ..\..\..\Dependencies\peerModule\include\. /y
copy PhysicsEngineManager.h ..\..\..\Dependencies\peerModule\include\. /y

if not exist "..\..\..\Dependencies\peerModule\lib\Release" mkdir ..\..\..\Dependencies\peerModule\lib\Release
copy ReleaseNCS\Peer.lib "..\..\..\Dependencies\peerModule\lib\Release\Peer.lib" /y
copy ReleaseNCS\Peer.dll "..\..\..\Dependencies\peerModule\lib\Release\Peer.dll" /y

copy "..\..\..\Dependencies\peerModule\lib\Release\Peer.dll" "..\..\..\Common\bin\navigator\Release\Peer.dll" /y

pause