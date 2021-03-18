
// rawFileProcessingView.h : interface of the CrawFileProcessingView class
//

#pragma once


class CrawFileProcessingView : public CView
{
protected: // create from serialization only
	CrawFileProcessingView() noexcept;
	DECLARE_DYNCREATE(CrawFileProcessingView)

// Attributes
public:
	CrawFileProcessingDoc* GetDocument() const;

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
	unsigned char m_orgImg[256][256];

	virtual ~CrawFileProcessingView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnOpenrawfileOpen();
};

#ifndef _DEBUG  // debug version in rawFileProcessingView.cpp
inline CrawFileProcessingDoc* CrawFileProcessingView::GetDocument() const
   { return reinterpret_cast<CrawFileProcessingDoc*>(m_pDocument); }
#endif

