
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

#include <cmath>

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
	ON_COMMAND(ID_SUBSAMPLING_2, &CrawFileProcessingView::OnSubsampling2)
	ON_COMMAND(ID_SUBSAMPLING_3, &CrawFileProcessingView::OnSubsampling3)
	ON_COMMAND(ID_SUBSAMPLING_4, &CrawFileProcessingView::OnSubsampling4)
	ON_COMMAND(ID_QUANTIZATION_1BIT, &CrawFileProcessingView::OnQuantization1bit)
	ON_COMMAND(ID_QUANTIZATION_2BIT, &CrawFileProcessingView::OnQuantization2bit)
	ON_COMMAND(ID_QUANTIZATION_4BIT, &CrawFileProcessingView::OnQuantization4bit)
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

void CrawFileProcessingView::subsample(int scale) {
	int src_x, src_y;
	unsigned char tempBuf[256][256];
	for (int y = 0; y < 256 / scale; y++) {
		for (int x = 0; x < 256 / scale; x++) {
			// get corresponding coordinate on source image
			// of target image pixel (x, y)
			src_y = y * scale;
			src_x = x * scale;

			// check boundary
			if (src_x > 256 - 1) src_x = 256 - 1;
			if (src_y > 256 - 1) src_y = 256 - 1;

			//take pixel value
			tempBuf[y][x] = m_orgImg[src_y][src_x];
		}
	}

	for (int y = 0; y < 256; y++) {
		for (int x = 0; x < 256; x++)
			m_orgImg[y][x] = tempBuf[y][x];
	}
}


void CrawFileProcessingView::OnSubsampling2()
{
	subsample(2);
	Invalidate();
}


void CrawFileProcessingView::OnSubsampling3()
{
	subsample(3);
	Invalidate();
}


void CrawFileProcessingView::OnSubsampling4()
{
	subsample(4);
	Invalidate();
}


void CrawFileProcessingView::NBitQt(int bit)
{
	int nlevel = pow((float)2, bit);
	float delta = 255 / (nlevel - 1);
	for (int y = 0; y < 256; y++)
	{
		for (int x = 0; x < 256; x++)
			m_orgImg[y][x] = delta*round(m_orgImg[y][x] / delta);
	}
}


void CrawFileProcessingView::OnQuantization1bit()
{
	NBitQt(1);
	Invalidate();
}


void CrawFileProcessingView::OnQuantization2bit()
{
	NBitQt(2);
	Invalidate();
}


void CrawFileProcessingView::OnQuantization4bit()
{
	NBitQt(4);
	Invalidate();
}
