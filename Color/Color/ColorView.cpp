
// ColorView.cpp : implementation of the CColorView class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "Color.h"
#endif

#include "ColorDoc.h"
#include "ColorView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CColorView

IMPLEMENT_DYNCREATE(CColorView, CView)

BEGIN_MESSAGE_MAP(CColorView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_COMMAND(ID_OPEN_OPENCOLORRAW, &CColorView::OnOpenOpencolorraw)
	ON_COMMAND(ID_COLORPROCESSING_RGBDECOMPOSE, &CColorView::OnColorprocessingRgbdecompose)
	ON_COMMAND(ID_COLORPROCESSING_RGBTOYUV, &CColorView::OnColorprocessingRgbtoyuv)
	ON_COMMAND(ID_COLORPROCESSING_SHARPENING, &CColorView::OnColorprocessingSharpening)
END_MESSAGE_MAP()

// CColorView construction/destruction

CColorView::CColorView() noexcept
{
	// TODO: add construction code here

}

CColorView::~CColorView()
{
}

BOOL CColorView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CColorView drawing

void CColorView::OnDraw(CDC* pDC)
{
	CColorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
	for (int y = 0; y < 256; y++) {
		for (int x = 0; x < 256; x++) {
			if (m_viewType == 0)
				pDC->SetPixel(x, y, RGB(m_outImg[y][x * 3 + 0],
					m_outImg[y][x * 3 + 1],
					m_outImg[y][x * 3 + 2]));

			if (m_viewType == 1) {
				pDC->SetPixel(x, y, RGB(m_outImg[y][x * 3 + 0], 0, 0));
				pDC->SetPixel(x + 258, y, RGB(0, m_outImg[y][x * 3 + 1], 0));
				pDC->SetPixel(x + 516, y, RGB(0, 0, m_outImg[y][x * 3 + 2]));
			}

			if (m_viewType == 2) {
				pDC->SetPixel(x, y,
					RGB(m_outImg[y][x * 3],
						m_outImg[y][x * 3],
						m_outImg[y][x * 3]));
				pDC->SetPixel(x + 258, y,
					RGB(m_outImg[y][x * 3 + 1],
						m_outImg[y][x * 3 + 1],
						m_outImg[y][x * 3 + 1]));
				pDC->SetPixel(x + 516, y,
					RGB(m_outImg[y][x * 3 + 2],
						m_outImg[y][x * 3 + 2],
						m_outImg[y][x * 3 + 2]));
			}
		}
	}
}


// CColorView printing

BOOL CColorView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CColorView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CColorView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


// CColorView diagnostics

#ifdef _DEBUG
void CColorView::AssertValid() const
{
	CView::AssertValid();
}

void CColorView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CColorDoc* CColorView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CColorDoc)));
	return (CColorDoc*)m_pDocument;
}
#endif //_DEBUG


// CColorView message handlers


void CColorView::OnOpenOpencolorraw()
{
	// TODO: Add your command handler code here
	CFileDialog dlg(TRUE, _T("raw"), _T("*.raw"), OFN_OVERWRITEPROMPT, + _T("RAW (*.raw)|(*.raw)}"));

	if (dlg.DoModal() == IDOK) {
		CString pathname;
		pathname = dlg.GetPathName();

		FILE* fp;
		_tfopen_s(&fp, pathname, _T("rb"));
		fread(m_orgImg, sizeof(char), 256 * 256 * 3, fp);
		fclose(fp);
		memcpy(m_outImg, m_orgImg, 256 * 256 * 3 * sizeof(unsigned char));
		m_viewType = 0;

		Invalidate();
	}
}


void CColorView::OnColorprocessingRgbdecompose()
{
	// TODO: Add your command handler code here
	memcpy(m_outImg, m_orgImg, 256 * 256 * 3 * sizeof(unsigned char));
	m_viewType = 1;

	Invalidate();
}


void CColorView::RGB2YUV(float r, float g, float b, float* y, float* u, float* v) {
	*y = .288f * r + .587f * g + .114f * b;
	*u = -.147f * r - .289f * g + .437f * b;
	*v = .615f * r - 0.515f * g - .100f * b;
}


void CColorView::YUV2RGB(float* r, float* g, float* b, float y, float u, float v) {
	*r = 1 * y + 0 * u + 1.13983 * v;
	*g = 1 * y - .39465 * u - .58060 * v;
	*b = 1 * y + 2.03211 * u;
}


void CColorView::OnColorprocessingRgbtoyuv()
{
	// TODO: Add your command handler code here
	float r, g, b, y, u, v;
	
	for (int yIdx = 0; yIdx < 256; yIdx++) {
		for (int x = 0; x < 256; x++) {
			r = (float)m_orgImg[yIdx][x * 3 + 0] / 255.0f;
			g = (float)m_orgImg[yIdx][x * 3 + 1] / 255.0f;
			b = (float)m_orgImg[yIdx][x * 3 + 2] / 255.0f;

			RGB2YUV(r, g, b, &y, &u, &v);

			m_outImg[yIdx][x * 3 + 0] = (unsigned char)(y * 255.0);
			m_outImg[yIdx][x * 3 + 1] = (unsigned char)(u * 255.0);
			m_outImg[yIdx][x * 3 + 2] = (unsigned char)(v * 255.0);
		}
	}

	m_viewType = 2;
	
	Invalidate();
}


void CColorView::OnColorprocessingSharpening()
{
	// TODO: Add your command handler code here
	float r, g, b, y, u, v;

	float tempBuf[256][256 * 3];

	for (int yIdx = 0; yIdx < 256; yIdx++) {
		for (int x = 0; x < 256; x++) {
			r = (float)m_orgImg[yIdx][x * 3 + 0] / 255.0f;
			g = (float)m_orgImg[yIdx][x * 3 + 1] / 255.0f;
			b = (float)m_orgImg[yIdx][x * 3 + 2] / 255.0f;

			RGB2YUV(r, g, b, &y, &u, &v);

			tempBuf[yIdx][x * 3 + 0] = (y * 255.0);
			tempBuf[yIdx][x * 3 + 1] = (u * 255.0);
			tempBuf[yIdx][x * 3 + 2] = (v * 255.0);
		}
	}
;
	//filter init
	int maskValues[3][3] = { {0, -1, 0}, {-1, 5, -1}, {0, -1, 0} };

	//convolution
	int sumValue;
	float subTempBuf[256][256];
	for (int y = 1; y < 256 - 1; y++) {
		for (int x = 1; x < 256 - 1; x++) {
			sumValue = 0;

			for (int ir = -1; ir <= 1; ir++) {
				for (int ic = -1; ic <= 1; ic++) {
					sumValue += tempBuf[y + ir][(x + ic) * 3] * maskValues[ir + 1][ic + 1];
				}
			}

			//clipping values out of range
			if (sumValue > 255)
				sumValue = 255;
			if (sumValue < 0)
				sumValue = 0;
			subTempBuf[y][x] = sumValue;
		}
	}
	
	for (int yIdx = 0; yIdx < 256; yIdx++) {
		for (int x = 0; x < 256; x++) {
			y = subTempBuf[yIdx][x] / 255.0f;
			u = tempBuf[yIdx][x * 3 + 1] / 255.0f;
			v = tempBuf[yIdx][x * 3 + 2] / 255.0f;

			YUV2RGB(&r, &g, &b, y, u, v);

			if (r > 1) r = 1;
			if (r < 0) r = 0;
			if (g > 1) g = 1;
			if (g < 0) g = 0;
			if (b > 1) b = 1;
			if (b < 0) b = 0;

			m_outImg[yIdx][x * 3 + 0] = (unsigned char)(r * 255.0);
			m_outImg[yIdx][x * 3 + 1] = (unsigned char)(g * 255.0);
			m_outImg[yIdx][x * 3 + 2] = (unsigned char)(b * 255.0);
		}
	}

	m_viewType = 0;

	Invalidate();
}
