rem カレントドライブ移動
%~d0
rem カレントディレクトリ移動
cd %~dp0
rem パック
#__TbfPack #__FileList_sourcerer.csv sourcerer\enlight1010i.dat sourcerer\ -i V01 > FilePackLog_sourcerer_iOS_V01.txt
