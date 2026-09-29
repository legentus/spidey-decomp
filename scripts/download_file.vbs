Option Explicit
Dim url, outPath, http, stream
If WScript.Arguments.Count < 2 Then
  WScript.Echo "[ERROR] download_file.vbs requires URL and output path."
  WScript.Quit 2
End If
url = WScript.Arguments(0)
outPath = WScript.Arguments(1)
On Error Resume Next
Set http = CreateObject("WinHttp.WinHttpRequest.5.1")
If Err.Number <> 0 Then
  Err.Clear
  Set http = CreateObject("MSXML2.ServerXMLHTTP.6.0")
End If
If Err.Number <> 0 Or (http Is Nothing) Then
  WScript.Echo "[ERROR] Windows HTTP COM component is unavailable."
  WScript.Quit 3
End If
On Error GoTo 0
http.Open "GET", url, False
On Error Resume Next
http.Option(6) = True
On Error GoTo 0
http.SetRequestHeader "User-Agent", "Spider-Man-2000-Dev-Updater"
http.Send
If http.Status < 200 Or http.Status >= 300 Then
  WScript.Echo "[ERROR] HTTP " & http.Status & " while downloading: " & url
  WScript.Quit 4
End If
Set stream = CreateObject("ADODB.Stream")
stream.Type = 1
stream.Open
stream.Write http.ResponseBody
stream.SaveToFile outPath, 2
stream.Close
WScript.Quit 0
