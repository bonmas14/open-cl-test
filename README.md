# OpenCL test


## Build

Build open-cl with [guide](https://github.com/KhronosGroup/OpenCL-Guide/blob/main/chapters/getting_started_windows.md). The OpenCL-SDK is in same root directory as this project.

```powershell
mkdir project-name
cd project-name
git clone --recursive https://github.com/KhronosGroup/OpenCL-SDK.git 
# compile opencl

# now git clone this project
git clone https://github.com/bonmas14/open-cl-test.git
.\build.ps1
.\bin\open-cl-test.exe
```
