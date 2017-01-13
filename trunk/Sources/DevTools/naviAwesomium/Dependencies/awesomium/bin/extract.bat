set vc="%1"
set wd="%2"

cd %wd%
if not exist %wd%\%vc% ..\..\..\..\..\..\Common\unrar.exe e Awesomiumbin%vc%.rar %vc%\
if not exist %wd%\icudt38.dll ..\..\..\..\..\..\Common\unrar.exe e icudt38.rar
