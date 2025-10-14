// ExcelCtrl.h: interface for the CExcelCtrl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EXCELCTRL_H__226A65A9_279F_42B9_911E_CB923BC77CB2__INCLUDED_)
#define AFX_EXCELCTRL_H__226A65A9_279F_42B9_911E_CB923BC77CB2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


//*****************************************************************************
//*                                                                           *
//*                                                                           *
//*             Hwang ChiHwan's Game Classes based on MFC version 6.0         *
//*                       ExcelCtrl Classes header file                       *
//*                                                                           *
//*                                                                           *
//*    ExcelCtrl.h: Interface for the ExcelCtrl class.                        *
//*                                                                           *
//*                                                                           *
//*                                                                           *
//*                                                                           *
//*  This Program is programmed by Hwang ChiHwan. (C)sai79	                  *
//*  Tab Size of this text is 4. It is edited with Visual C++ 6.0 Enviroment  *
//*                                                                           *
//*                                                                           *
//*****************************************************************************


#include <odbcinst.h>
#include <afxdb.h>

//*****************************************************************************
//*                                                                           *
//*                             Class CExcelCtrl                              *
//*                                                                           *
//*                                                                           *
//*                                                                           *
//*  CExcelCtrl                                                               *
//*                                                                           *
//*****************************************************************************

class CExcelCtrl  
{
public:
	CExcelCtrl(CString File, CString SheetOrSeparator, bool Backup = true);
	virtual ~CExcelCtrl();

			// Add header row to spreadsheet
			bool		AddHeaders(CStringArray &FieldNames, bool replace = false);
			// Clear text delimited file content
			bool		DeleteSheet(); 
			// Clear entire Excel spreadsheet content. The sheet itself is not deleted
			bool		DeleteSheet(CString SheetName); 
			// Insert or replace a row into spreadsheet. Default is add new row. 
			bool		AddRow(CStringArray &RowValues, long row = 0, bool replace = false); 
			// Replace or add a cell into Excel spreadsheet using header row or column alphabet.
			// Default is add cell into new row. Set Auto to false if want to force column to be used as header name
			bool		AddCell(CString CellValue, CString column, long row = 0, bool Auto = true); 
			// Replace or add a cell into spreadsheet using column number. Default is add cell into new row. 
			bool		AddCell(CString CellValue, short column, long row = 0); 
			// Search and replace rows in Excel spreadsheet
			bool		ReplaceRows(CStringArray &NewRowValues, CStringArray &OldRowValues); 
			// Read a row from spreadsheet. Default is read the next row
			bool		ReadRow(CStringArray &RowValues, long row = 0); 
			// Read a column from Excel spreadsheet using header row or column alphabet.
			// Set Auto to false if want to force column to be used as header name
			bool		ReadColumn(CStringArray &ColumnValues, CString column, bool Auto = true); 
			// Read a column from spreadsheet using column number
			bool		ReadColumn(CStringArray &ColumnValues, short column); 
			// Read a cell from Excel spreadsheet using header row or column alphabet.
			// Default is read the next cell in next row. Set Auto to false if want to force column to be used as header name
			bool		ReadCell(CString &CellValue, CString column, long row = 0, bool Auto = true); 
			// Read a cell from spreadsheet using column number. Default is read the next cell in next row.
			bool		ReadCell(CString &CellValue, short column, long row = 0); 
			// Begin transaction
			void		BeginTransaction(); 
			// Save changes to spreadsheet
			bool		Commit(); 
			// Undo changes to spreadsheet
			bool		RollBack(); 
			//
			bool		Convert(CString SheetOrSeparator);

	// Get the header row from spreadsheet
	inline	void		GetFieldNames(CStringArray &FieldNames)			{	FieldNames.RemoveAll(); FieldNames.Copy(m_aFieldNames);} 
	// Get total number of rows in  spreadsheet
	inline	long		GetTotalRows()									{	return m_dTotalRows;} 
	// Get total number of columns in  spreadsheet
	inline	short		GetTotalColumns()								{	return m_dTotalColumns;} 
	// Get the currently selected row in  spreadsheet
	inline	long		GetCurrentRow()									{	return m_dCurrentRow;} 
	// Get status of backup. True if backup is successful, False if spreadsheet is not backup
	inline	bool		GetBackupStatus()								{	return m_bBackup;} 
	// Get status of Transaction. True if Transaction is started, False if Transaction is not started or has error in starting
	inline	bool		GetTransactionStatus()							{	return m_bTransaction;}
	// Get last error message
	inline	CString		GetLastError()									{	return m_sLastError;} 

private:
			// Open a text delimited file for reading or writing
			bool		Open(); 
			// Get the name of the Excel-ODBC driver
			void		GetExcelDriver(); 
			// Convert Excel column in alphabet into column number
			short		CalculateColumnNumber(CString column, bool Auto); 

private:
	bool				m_bAppend;			// Internal flag to denote newly created spreadsheet or previously created spreadsheet
	bool				m_bBackup;			// Internal flag to denote status of Backup
	bool				m_bExcel;			// Internal flag to denote whether file is Excel spreadsheet or text delimited spreadsheet
	bool				m_bTransaction;		// Internal flag to denote status of Transaction

	long				m_dCurrentRow;		// Index of current row, starting from 1
	long				m_dTotalRows;		// Total number of rows in spreadsheet
	short				m_dTotalColumns;	// Total number of columns in Excel spreadsheet. Largest number of columns in text delimited spreadsheet

	CString 			m_sSql;				// SQL statement to open Excel spreadsheet for reading
	CString 			m_sDsn;				// DSN string to open Excel spreadsheet for reading and writing
	CString 			m_stempSql;			// Temporary string for SQL statements or for use by functions
	CString 			m_stempString;		// Temporary string for use by functions
	CString				m_sSheetName;		// Sheet name of Excel spreadsheet
	CString 			m_sExcelDriver;		// Name of Excel Driver
	CString 			m_sFile;			// Spreadsheet file name
	CString 			m_sSeparator;		// Separator in text delimited spreadsheet
	CString				m_sLastError;		// Last error message

	CStringArray		m_atempArray;		// Temporary array for use by functions
	CStringArray		m_aFieldNames;		// Header row in spreadsheet
	CStringArray		m_aRows;			// Content of all the rows in spreadsheet

	CDatabase*			m_Database;			// Database variable for Excel spreadsheet
	CRecordset*			m_rSheet;			// Recordset for Excel spreadsheet
};

#endif // !defined(AFX_EXCELCTRL_H__226A65A9_279F_42B9_911E_CB923BC77CB2__INCLUDED_)
