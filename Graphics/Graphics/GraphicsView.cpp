
// GraphicsView.cpp : implementation of the CGraphicsView class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "Graphics.h"
#endif

#include "GraphicsDoc.h"
#include "GraphicsView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CGraphicsView

IMPLEMENT_DYNCREATE(CGraphicsView, CView)

BEGIN_MESSAGE_MAP(CGraphicsView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
END_MESSAGE_MAP()

// CGraphicsView construction/destruction

CGraphicsView::CGraphicsView() noexcept
{
	// TODO: add construction code here

}

CGraphicsView::~CGraphicsView()
{
}

BOOL CGraphicsView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CGraphicsView drawing

void CGraphicsView::OnDraw(CDC* pDC)
{
	CGraphicsDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	
	CPen pen1(PS_SOLID, 5, RGB(255, 0, 0));
	CPen pen2(PS_SOLID, 10, RGB(0, 255, 0));
	CPen pen3(PS_SOLID, 15, RGB(0, 0, 255));
	CPen* oldPen;

	oldPen = pDC->SelectObject(&pen1);
	pDC->MoveTo(50, 50);
	pDC->LineTo(400, 50);
	
	pDC->SelectObject(&pen2);
	pDC->MoveTo(50, 100);
	pDC->LineTo(400, 100);

	pDC->SelectObject(&pen3);
	CPoint pt[4] = { CPoint(50, 150), CPoint(88, 246),
		CPoint(364, 192), CPoint(250, 48) };
	pDC->PolyBezier(pt, 4);

	pDC->SelectObject(oldPen);
}


// CGraphicsView printing

BOOL CGraphicsView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CGraphicsView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CGraphicsView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


// CGraphicsView diagnostics

#ifdef _DEBUG
void CGraphicsView::AssertValid() const
{
	CView::AssertValid();
}

void CGraphicsView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CGraphicsDoc* CGraphicsView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CGraphicsDoc)));
	return (CGraphicsDoc*)m_pDocument;
}
#endif //_DEBUG


// CGraphicsView message handlers
