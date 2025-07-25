param(
    [Parameter(ValueFromRemainingArguments=$true)]
    [string[]]$args,
    [switch]$log = $false,
    [switch]$release = $false
)

if ($release) {
    & $PSScriptRoot/build.ps1 -release
} else {
    & $PSScriptRoot/build.ps1
}
if ($?) {
    adb push build/libtracks.so /sdcard/ModData/com.beatgames.beatsaber/Modloader/mods/libtracks.so
    if ($?) {
        adb shell am force-stop com.beatgames.beatsaber
        adb shell am start com.beatgames.beatsaber/com.unity3d.player.UnityPlayerActivity
#         adb logcat -c; adb logcat > test.log
        if ($log.IsPresent) {
            $timestamp = Get-Date -Format "MM-dd HH:mm:ss.fff"
#             adb logcat -c
#             adb logcat -T "$timestamp" main-modloader:W QuestHook[Tracks`|v0.1.0]:* QuestHook[UtilsLogger`|v1.0.12]:* AndroidRuntime:E *:S
        }
    }
}
