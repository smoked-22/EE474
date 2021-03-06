
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
//	ON_COMMAND(ID_PIXELPOINTPROCESSING_CONTRAST, &CrawFileProcessingView::OnPixelpointprocessingContrast)
ON_COMMAND(ID_PIXELPOINTPROCESSING_CONTRAST, &CrawFileProcessingView::OnPixelpointprocessingContrast)
ON_COMMAND(ID_SMOOTHING_SMOOTHING, &CrawFileProcessingView::OnSmoothingSmoothing)
ON_COMMAND(ID_SHARPENING_SHARPENING, &CrawFileProcessingView::OnSharpeningSharpening)
ON_COMMAND(ID_INTERPOLATION_BILINEAR, &CrawFileProcessingView::OnInterpolationBilinear)
ON_COMMAND(ID_INTERPOLATION_NEARESTNEIGHBOR, &CrawFileProcessingView::OnInterpolationNearestneighbor)
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


//void CrawFileProcessingView::OnPixelpointprocessingContrast()
//{
//	int r1 = 70, s1 = 50;
//	int r2 = 150, s2 = 180;
//	int L = 256;
//	float leftFactor = (r1 != 0) ? (float)s1 / r1 : (float)s1 / (r1 + 1);
//	float rightFactor = (L - 1 - r2 != 0) ?
//		(float)(L - 1 - s2) / (L - 1 - r2) : (float)(L - 1 - s2) / (L - r2);
//	float middleFactor = (r2 - r1 != 0) ?
//		(float)(s2 - s1) / (r2 - r1) : (float)(s2 - s1) / (r2 - r1 + 1);
//	for (int y = 0; y < 256; y++) {
//		if (m_orgImg[y][x] < r1) {
//			m_orgImg[y][x] - (unsigned char)(leftFactor * m_orgImg[y][x] + .5);
//		}
//		else if (r1 <= m_orgImg[y][x] && m_orgImg[y][x] <= r2) {
//			m_orgImg[y][x] = (unsigned char)(middleFactor * (m_orgImg[y][x] - r1) + s1 + .5);
//		}
//		else {
//			m_orgImg[y][x] = (unsigned char)(rightFactor * (m_orgImg[y][x] - r2) + s2 + .5);
//		}
//	}
//	Invalidate();
//}


void CrawFileProcessingView::OnPixelpointprocessingContrast()
{
	int r1 = 70, s1 = 50;
	int r2 = 150, s2 = 180;
	int L = 256;
	float leftFactor = (r1 != 0) ? (float)s1 / r1 : (float)s1 / (r1 + 1);
	float rightFactor = (L - 1 - r2 != 0) ?
		(float)(L - 1 - s2) / (L - 1 - r2) : (float)(L - 1 - s2) / (L - r2);
	float middleFactor = (r2 - r1 != 0) ?
		(float)(s2 - s1) / (r2 - r1) : (float)(s2 - s1) / (r2 - r1 + 1);
	for (int y = 0; y < L; y++) {
		for (int x = 0; x < L; x++) {
			if (m_orgImg[y][x] < r1) {
				m_orgImg[y][x] - (unsigned char)(leftFactor * m_orgImg[y][x] + .5);
			}
			else if (r1 <= m_orgImg[y][x] && m_orgImg[y][x] <= r2) {
				m_orgImg[y][x] = (unsigned char)(middleFactor * (m_orgImg[y][x] - r1) + s1 + .5);
			}
			else {
				m_orgImg[y][x] = (unsigned char)(rightFactor * (m_orgImg[y][x] - r2) + s2 + .5);
			}
		}
	}
	Invalidate();
}


void CrawFileProcessingView::OnSmoothingSmoothing()
{
	int maskValues[3][3];
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			maskValues[i][j] = 1;
		}
	}

	int sumValue;

	unsigned char tempBuf[256][256];
	for (int y = 1; y < 256 - 1; y++) {
		for (int x = 1; x < 256 - 1; x++) {
			sumValue = 0;

			for (int ir = -1; ir <= 1; ir++) {
				for (int ic = -1; ic <= 1; ic++) {
					sumValue += m_orgImg[y + ir][x + ic] * maskValues[ir + 1][ic + 1];
				}
			}

			tempBuf[y][x] = sumValue / 9;
		}
	}

	for (int y = 1; y < 256 - 1; y++) {
		for (int x = 1; x < 256 - 1; x++) {
			m_orgImg[y][x] = tempBuf[y][x];
		}
	}

	Invalidate();
}


void CrawFileProcessingView::OnSharpeningSharpening()
{
	//filter init
	int maskValues[3][3];
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			maskValues[i][j] = -1;
		}
	}
	maskValues[1][1] = 9;

	//convolution
	int sumValue;
	unsigned char tempBuf[256][256];
	for (int y = 1; y < 256 - 1; y++) {
		for (int x = 1; x < 256 - 1; x++) {
			sumValue = 0;

			for (int ir = -1; ir <= 1; ir++) {
				for (int ic = -1; ic <= 1; ic++) {
					sumValue += m_orgImg[y + ir][x + ic] * maskValues[ir + 1][ic + 1];
				}
			}

			//clipping values out of range
			if (sumValue > 255)
				sumValue = 255;
			if (sumValue < 0)
				sumValue = 0;
			tempBuf[y][x] = sumValue;
		}
	}

	//replace original image with sharpened
	for (int y = 1; y < 256 - 1; y++) {
		for (int x = 1; x < 256 - 1; x++) {
			m_orgImg[y][x] = tempBuf[y][x];
		}
	}

	Invalidate();
}


unsigned char lint(unsigned char l1, unsigned char r1, float w_x) {
	return (unsigned char)(l1 * w_x + r1 * (1 - w_x));
}


unsigned char blint(unsigned char l1, unsigned char r1, unsigned char l2, unsigned char r2, float w_x, float w_y) {
	return (unsigned char)lint(lint(l1, r1, w_x), lint(l2, r2, w_x), w_y);
}


void CrawFileProcessingView::OnInterpolationBilinear()
{
	/// <summary>
	/// bilinear interpolation calculates pixel values based on quarter length
	/// </summary>
	
	// temporary buffer initialiation
	unsigned char tempBuf[256][256];

	// TODO
	//create a padded m_orgImg
	unsigned char m_orgImg_pad[130][130];
	m_orgImg_pad[0][0] = m_orgImg[0][0];
	m_orgImg_pad[0][129] = m_orgImg[0][127];
	m_orgImg_pad[129][0] = m_orgImg[127][0];
	m_orgImg_pad[129][129] = m_orgImg[127][127];

	for (int y = 0; y < 128; y++) {
		for (int x = 0; x < 128; x++) {
			if (y == 0){
				// copy horizontal pad
				m_orgImg_pad[y][x + 1] = m_orgImg[y][x];
			}
			if (y == 127) {
				m_orgImg_pad[y + 2][x + 2] = m_orgImg[y][x];
			}
			// copy non-padded values
			m_orgImg_pad[y + 1][x + 1] = m_orgImg[y][x];
		}
		// copy vertical pad
		m_orgImg_pad[y + 1][0] = m_orgImg[y][0];
		m_orgImg_pad[y + 1][129] = m_orgImg[y][127];
	}


	// iteration for interpolation
	unsigned char left1, left2, right1, right2;
	for (int y = 0; y < 256; y ++) {
		for (int x = 0; x < 256; x++) {
			float w_x = (x % 2 == 0) ? .25 : .75;
			float w_y = (y % 2 == 0) ? .25 : .75;
			int row = (y + 1) / 2;
			int col = (x + 1) / 2;
			left1 = m_orgImg_pad[row][col];
			left2 = m_orgImg_pad[row + 1][col];
			right1 = m_orgImg_pad[row][col + 1];
			right2 = m_orgImg_pad[row + 1][col + 1];

			tempBuf[y][x] = blint(left1, right1, left2, right2, w_x, w_y);
		}
	}
	

	//replace original image with interpolated
	for (int y = 0; y < 256; y++) {
		for (int x = 0; x < 256; x++) {
			m_orgImg[y][x] = tempBuf[y][x];
		}
	}

	Invalidate();
}


void CrawFileProcessingView::OnInterpolationNearestneighbor()
{
	/// <summary>
	/// interpolation process is done with expanding pixels in odd numbered indices
	/// </summary>
	int L = 256;
	unsigned char tempBuf[256][256];
	for (int ir = 0; ir < L / 2; ++ir) {
		for (int ic = 0; ic < L / 2; ++ic) {
			int y = ic * 2;
			int x = ir * 2;
			for (int i = 0; i < 2; i++) {
				for (int j = 0; j < 2; j++)
					tempBuf[y + i][x + j] = m_orgImg[ic][ir];
			}
		}
	}

	//replace original image with interpolated
	for (int y = 0; y < 256; y++) {
		for (int x = 0; x < 256; x++) {
			m_orgImg[y][x] = tempBuf[y][x];
		}
	}
	Invalidate();
}
