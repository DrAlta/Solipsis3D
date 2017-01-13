@echo off

echo,
echo This will clean up the cache.xml.
echo,
echo *********************************************************
echo ** You must close the Server and all Navigators, then  **
echo ** run this batch script on each Navigator client !    **
echo *********************************************************
echo,
pause

echo ========== Cleaning up... ==========

move cache_NoScene_ref.xml cache_NoScene_ref.xm_ >NUL
move cache_ref.xml cache_ref.xm_ >NUL
del *.ssf 2>NUL
del *.sif 2>NUL
del *.sof 2>NUL
del *.xml 2>NUL

copy cache_NoScene_ref.xm_ cache.xml >NUL
move cache_NoScene_ref.xm_ cache_NoScene_ref.xml >NUL
move cache_ref.xm_ cache_ref.xml >NUL


echo ============== Done! ===============
pause
