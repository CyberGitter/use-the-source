Sub Splitbook()
'Updateby20140612, https://www.extendoffice.com/documents/excel/628-excel-split-workbook.html
Dim xPath As String
xPath = Application.ActiveWorkbook.Path
Application.ScreenUpdating = False
Application.DisplayAlerts = False
For Each xWs In ThisWorkbook.Sheets
    xWs.Copy
    Application.ActiveWorkbook.SaveAs Filename:=xPath & "\" & xWs.Name & "分行.et"
    Application.ActiveWorkbook.Close False
Next
Application.DisplayAlerts = True
Application.ScreenUpdating = True
End Sub
