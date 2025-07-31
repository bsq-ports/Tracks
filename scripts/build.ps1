Param(
    [Parameter(Mandatory=$false)]
    [Switch]$clean,
    [Parameter(Mandatory=$false)]
    [Switch]$release
)

# if user specified clean, remove all build files
if ($clean.IsPresent)
{
    if (Test-Path -Path "build")
    {
        remove-item build -R
    }
}

if (($clean.IsPresent) -or (-not (Test-Path -Path "build")))
{
    $out = new-item -Path build -ItemType Directory
}

# build the rust code
cd ./tracks_rs_link
if ($release.IsPresent) {
    cargo ndk --no-strip -t arm64-v8a -o build build --release
} else {
    cargo ndk --no-strip -t arm64-v8a -o build build
}
cd ..

# Set build type based on release flag
$buildType = if ($release.IsPresent) { "RelWithDebInfo" } else { "Debug" }

& cmake -B ./build -G "Ninja" -DCMAKE_BUILD_TYPE="$buildType" .
& cmake --build ./build 
