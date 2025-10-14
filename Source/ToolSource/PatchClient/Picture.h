////////////////////////////////////////////////////////////////
// MSDN Magazine -- October 2001
// If this code works, it was written by Paul DiLascia.
// If not, I don't know who wrote it.
// Compiles with Visual C++ 6.0 for Windows 98 and probably Windows 2000 too.
// Set tabsize = 3 in your editor.
//
#pragma once
#include <atlbase.h>

//////////////////
// Picture object--encapsulates IPicture
//
class CPicture {
public:
	CPicture();
	~CPicture();

	// Load frm various sosurces
	BOOL Load(UINT nIDRes);
	BOOL Load(LPCTSTR pszPathName);
	BOOL Load(CFile& file);
	BOOL Load(CArchive& ar);
	BOOL Load(IStream* pstm);

	// get the bitmap handle
	HBITMAP GetBitmap()
	{        
        HBITMAP hBitmap = NULL;
		if( m_spIPicture )
		{
			if (S_OK != m_spIPicture->get_Handle((OLE_HANDLE FAR *) &hBitmap))
			{
				FLTRACE_LOG( PROGRAM_NAME, _T( "Couldn't get bitmap handle from picture" ) );
				FLASSERT(FALSE);
			}
		}
		return hBitmap;
	}

	// render to device context
	void Render(CDC* pDC, CRect rc=CRect(0,0,0,0), LPCRECT prcMFBounds=NULL) const;

	CSize GetImageSize(CDC* pDC=NULL) const;

	operator IPicture*() {
		return m_spIPicture;
	}

	void GetHIMETRICSize(OLE_XSIZE_HIMETRIC& cx, OLE_YSIZE_HIMETRIC& cy) const {
		cx = cy = 0;
		const_cast<CPicture*>(this)->m_hr = m_spIPicture->get_Width(&cx);
		FLASSERT(SUCCEEDED(m_hr));
		const_cast<CPicture*>(this)->m_hr = m_spIPicture->get_Height(&cy);
		FLASSERT(SUCCEEDED(m_hr));
	}

	void Free() {
		if (m_spIPicture) {
			m_spIPicture.Release();
		}
	}

protected:
	CComQIPtr<IPicture>m_spIPicture;		 // ATL smart pointer to IPicture
	HRESULT m_hr;								 // last error code
};
