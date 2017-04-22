'将多个工作簿合并到一个工作簿中
'https://www.extendoffice.com/documents/excel/zh-cn-excel/2805-combine-multiple-workbooks.html

'承载这个VBA code的Workbook本身不能跟待合并的Workbooks文件放在一个文件夹中，不然当Loop到ThisWorkbook时，会提示“"工作簿1.xls"已经打开。重新执行打开操作会造成前面所做的更改被放弃，是否重新打开"工作簿1.xls"?”

Sub GetSheets()
  Path = "C:\Users\dt\Desktop\dt kte\"
  '因为这个VBA代码涉及到对文件的读取与修改，因此一定要小心，注意路径名等不要写错。
  '最开始，曾报错“不能给只读属性赋值”，并将`Path =`标上黄色。分析原因，我猜是由于Path是Workbook object的一个只读属性，详见[Workbook.Path Property (Excel)](https://msdn.microsoft.com/en-us/library/office/ff840974.aspx)。但为何这个VBA代码的作者仍然用Path作为变量名而feel alright呢？我猜Path作为Workbook object的属性是不是始自Office 2013。不管怎样，我将`Path`换成另外一个不敏感的变量名，运行成功。
  Filename = Dir(Path & "*.xls")
  Do While Filename <> ""
    Workbooks.Open Filename:=Path & Filename, ReadOnly:=True
    For Each Sheet In ActiveWorkbook.Sheets
      Sheet.Copy After:=ThisWorkbook.Sheets(1)
    Next Sheet
    Workbooks(Filename).Close
  Filename = Dir() 'https://msdn.microsoft.com/en-us/library/gg278779.aspx 值得好好看看Dir函数：“In Microsoft Windows, Dir supports the use of multiple character (*) and single character (?) wildcards to specify multiple files.” “Dir returns the first file name that matches pathname. To get any additional file names that match pathname, call Dir again with no arguments.”
  Loop
End Sub


'将多个工作表合并到一个工作表中
'https://www.extendoffice.com/documents/excel/zh-cn-excel/2800-excel-merge-multiple-worksheets-into-one.html

Sub Combine()
  Dim J As Integer
  On Error Resume Next
  Sheets(1).Select
  Worksheets.Add 'https://msdn.microsoft.com/zh-cn/library/ff838966.aspx, "如果同时省略 Before 和 After，则新工作表插入到活动工作表之前。"
  Sheets(1).Name = "Combined" '注意这个Sheets(1)已经是新插入的空白sheet了
  Sheets(2).Activate '说点题外话：activate和select是有区别的，回想在Excel中，选定一片区域后，只有一个单元格被激活以输入内容
  Range("A1").EntireRow.Select
  Selection.Copy Destination:=Sheets(1).Range("A1")
  'Selection是Application或Window对象的方法，并且在这两个方法中往往表现一致。此外，理解Selection的关键在于，“The returned object type depends on the current selection (for example, if a cell is selected, this property returns a Range object).” "Using this property with no object qualifier is equivalent to using Application.Selection." https://msdn.microsoft.com/zh-cn/library/ff835019.aspx, https://msdn.microsoft.com/zh-cn/library/ff840834.aspx
  For J = 2 To Sheets.Count
    Sheets(J).Activate
    Range("A1").Select
    Selection.CurrentRegion.Select
    Selection.Offset(1, 0).Resize(Selection.Rows.Count - 1).Select
    Selection.Copy Destination:=Sheets(1).Range("A65536").End(xlUp)(2) '如何理解这个End(xlUp)(2)：根据网上贴子https://www.mrexcel.com/forum/excel-questions/336210-end-xlup-2-a.html，似乎跟Range对象的Item属性有关https://msdn.microsoft.com/zh-cn/library/ff841096.aspx
  Next
End Sub