# X4Playthroughs

X4 Foundations Playthroughs by Nostrogo.
This program enables multiple save folders (ideally one per playthrough) for X4 foundations by Egosoft.
This way, each playthrough/run can benefit from all possible saves (10 permitted by the game).

## Environment
This program is developped for Windows and rely on the win 32 API.
It links with comdlg32 and Comctl32.
It is build with msys64/mingw64/g++

###### Compilation
It is complied from Visual Studio Code, the resulting compilation line is :
>C:\msys64\mingw64\bin\g++.exe -fdiagnostics-color=always -g -std=c++20 F:\Projects\Code\X4Playthroughs\src\*.cpp -lcomdlg32 -lComctl32 -o F:\Projects\Code\X4Playthroughs\src\..\bin\X4Playthroughs.exe


## About this program:

###### How to run it?
Please note that this program creates symbolic link on Windows. 
As a result **this program need to run with administrator privileges.**

###### Precaution of use
**Please back up your X4 foundation save folder prior to using this software.**
At first use, it should try to backup your X4 save folder to the backup folder but better carefull than sorry, back up your saves.

This software will replace the X4 foundation save folder by a symbolic link to the selected playthrough save folder.
Each managed playthrough will generate a folder in the data folder of this program (next to the configuration file).
Each of these folder will contain a save folder that will become the target of the link from the game save folder.

**Do not forget to deactivate Steam Cloud for the save files (unless you want steam cloud to overwrite all your playthrough saves)...**
This is done in the General tab of the game Properties on steam.

###### Do i install in in my X4 Foundation folder ? No!
This software works on the OS level with folders. It is independent from your game and can be installed anywhere.

###### Will my X4 saves be tagged as *modified* ? No!
Please note that this does not modify the X4 Foundations game in any way, your saves will not be *modified*.

