$exec_name = "open-cl-test"
$sdk_install_root = "..\OpenCL-SDK\install"

$bin_path = "bin"
$src_path = "src"
$obj_path = "obj"

if (!(Test-Path -Path .\$bin_path)) {
    New-Item -Path . -Name $bin_path -ItemType "directory" 
}
if (!(Test-Path -Path .\$obj_path)) {
    New-Item -Path . -Name $obj_path -ItemType "directory" 
}

cl.exe /nologo /TC /W4 /DCL_TARGET_OPENCL_VERSION=100 /I$sdk_install_root\include\ $src_path\Main.c /Fe:"$bin_path\$exec_name" /Fo:"$obj_path\" /link /LIBPATH:$sdk_install_root\lib OpenCL.lib
