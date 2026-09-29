Option Explicit
Dim zipPath, destPath, marker, fso, shell, zipNs, destNs, i
If WScript.Arguments.Count < 2 Then
  WScript.Echo "[ERROR] extract_zip.vbs requires ZIP and destination paths."
  WScript.Quit 2
End If
zipPath = WScript.Arguments(0)
destPath = WScript.Arguments(1)
marker = ""
If WScript.Arguments.Count >= 3 Then marker = WScript.Arguments(2)
Set fso = CreateObject("Scripting.FileSystemObject")
zipPath = fso.GetAbsolutePathName(zipPath)
destPath = fso.GetAbsolutePathName(destPath)
If Not fso.FileExists(zipPath) Then
  WScript.Echo "[ERROR] ZIP not found: " & zipPath
  WScript.Quit 3
End If
If Not fso.FolderExists(destPath) Then fso.CreateFolder(destPath)
Set shell = CreateObject("Shell.Application")
Set zipNs = shell.NameSpace(zipPath)
Set destNs = shell.NameSpace(destPath)
If zipNs Is Nothing Or destNs Is Nothing Then
  WScript.Echo "[ERROR] Windows Shell could not open the ZIP archive."
  WScript.Quit 4
End If
destNs.CopyHere zipNs.Items, 20
If marker <> "" Then
  For i = 1 To 240
    If fso.FileExists(fso.BuildPath(destPath, marker)) Or fso.FolderExists(fso.BuildPath(destPath, marker)) Then
      WScript.Sleep 1000
      WScript.Quit 0
    End If
    WScript.Sleep 500
  Next
  WScript.Echo "[ERROR] ZIP extraction timed out waiting for: " & marker
  WScript.Quit 5
End If
WScript.Sleep 2000
WScript.Quit 0
