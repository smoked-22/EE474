
// GraphicsView.h : interface of the CGraphicsView class
//

#pragma once


class CGraphicsView : public CView
{
protected: // create from serialization only
	CGraphicsView() noexcept;
	DECLARE_DYNCREATE(CGraphicsView)

// Attributes
public:
	CGraphicsDoc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~CGraphicsView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in GraphicsView.cpp
inline CGraphicsDoc* CGraphicsView::GetDocument() const
   { return reinterpret_cast<CGraphicsDoc*>(m_pDocument); }
#endif

