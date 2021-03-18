
// rawFileProcessingView.cpp : implementation of the CrawFileProcessingView class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "rawFileProcessing.h"
#endif

#include "rawFileProcessingDoc.h"
#include "rawFileProcessingView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CrawFileProcessingView

IMPLEMENT_DYNCREATE(CrawFileProcessingView, CView)

BEGIN_MESSAGE_MAP(CrawFileProcessingView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_COMMAND(ID_OPENRAWFILE_OPEN, &CrawFileProcessingView::OnOpenrawfileOpen)
END_MESSAGE_MAP()

// CrawFileProcessingView construction/destruction

CrawFileProcessingView::CrawFileProcessingView() noexcept
{
	// TODO: add construction code here

}

CrawFileProcessingView::~CrawFileProcessingView()
{
}

BOOL CrawFileProcessingView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CrawFileProcessingView drawing

void CrawFileProcessingView::OnDraw(CDC* pDC)
{
	CrawFileProcessingDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
	for (int i = 0; i < 256; ++i)
	{
		for (int j = 0; j < 256; ++j)
		{
			pDC->SetPixel(j, i, RGB(m_orgImg[i][j], m_orgImg[i][j], m_orgImg[i][j]));
		}
	}
}


// CrawFileProcessingView printing

BOOL CrawFileProcessingView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CrawFileProcessingView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CrawFileProcessingView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


// CrawFileProcessingView diagnostics

#ifdef _DEBUG
void CrawFileProcessingView::AssertValid() const
{
	CView::AssertValid();
}

void CrawFileProcessingView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CrawFileProcessingDoc* CrawFileProcessingView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CrawFileProcessingDoc)));
	return (CrawFileProcessingDoc*)m_pDocument;
}
#endif //_DEBUG


// CrawFileProcessingView message handlers


void CrawFileProcessingView::OnOpenrawfileOpen()
{
	// TODO: Add your command handler code here
	CFileDialog dlg(TRUE, _T("raw"), _T("*.raw"), OFN_OVERWRITEPROMPT, _T("RAW (*.raw)|(*.raw)}"));

	if (dlg.DoModal() == IDOK)
	{
		CString pathname;
		pathname = dlg.GetPathName();

		FILE* fp;
		_tfopen_s(&fp, pathname, _T("rb"));
		fread(m_orgImg, sizeof(char), 256 * 256, fp);
		fclose(fp);

		Invalidate();
	}
}
