
// ColorView.h : interface of the CColorView class
//

#pragma once


class CColorView : public CView
{
protected: // create from serialization only
	CColorView() noexcept;
	DECLARE_DYNCREATE(CColorView)

// Attributes
public:
	CColorDoc* GetDocument() const;

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
	virtual ~CColorView();

	unsigned char m_orgImg[256][256 * 3];
	unsigned char m_outImg[256][256 * 3];
	int m_viewType;
	void RGB2YUV(float r, float g, float b, float* y, float* u, float* v);
	void YUV2RGB(float* r, float* g, float* b, float y, float u, float v);
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnOpenOpencolorraw();
	afx_msg void OnColorprocessingRgbdecompose();
	afx_msg void OnColorprocessingRgbtoyuv();
	afx_msg void OnColorprocessingSharpening();
};

#ifndef _DEBUG  // debug version in ColorView.cpp
inline CColorDoc* CColorView::GetDocument() const
   { return reinterpret_cast<CColorDoc*>(m_pDocument); }
#endif

