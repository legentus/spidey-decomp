Option Explicit
Dim url, outPath, http, text, re, matches, fso, file
If WScript.Arguments.Count < 1 Then
  WScript.Echo "[ERROR] get_remote_sha.vbs requires an output path."
  WScript.Quit 2
End If
outPath = WScript.Arguments(0)
url = "https://api.github.com/repos/legentus/spidey-decomp/commits/dev"
On Error Resume Next
Set http = CreateObject("WinHttp.WinHttpRequest.5.1")
If Err.Number <> 0 Then
  Err.Clear
  Set http = CreateObject("MSXML2.ServerXMLHTTP.6.0")
End If
If Err.Number <> 0 Or (http Is Nothing) Then WScript.Quit 3
On Error GoTo 0
http.Open "GET", url, False
On Error Resume Next
http.Option(6) = True
On Error GoTo 0
http.SetRequestHeader "User-Agent", "Spider-Man-2000-Dev-Updater"
http.Send
If http.Status < 200 Or http.Status >= 300 Then WScript.Quit 4
text = http.ResponseText
Set re = New RegExp
re.Pattern = """sha""\s*:\s*""([0-9a-fA-F]{40})"""
re.IgnoreCase = True
re.Global = False
Set matches = re.Execute(text)
If matches.Count = 0 Then WScript.Quit 5
Set fso = CreateObject("Scripting.FileSystemObject")
Set file = fso.CreateTextFile(outPath, True, False)
file.WriteLine matches(0).SubMatches(0)
file.Close
WScript.Quit 0
