
// GaussSignalDlg.cpp: файл реализации
//

#include "stdafx.h"
#include "Gauss_Signal.h"
#include "GaussSignalDlg.h"
#include "afxdialogex.h"

#include <math.h>
#include <time.h>
#include <random>
#include <iostream>
#include <fstream>
#include <algorithm>

#define DOTS(x,y) (xp*((x)-xmin)),(yp*((y)-ymax)) // макрос перевода координат
#define DOTS_SH(x,y) (xp_s*((x)-xmin_s)),(yp_s*((y)-ymax_s))
#define DOTS3(x,y) (xp3*((x)-xmin3)),(yp3*((y)-ymax3))

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
using namespace std;

// Диалоговое окно CGaussSignalDlg



CGaussSignalDlg::CGaussSignalDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_GAUSS_SIGNAL_DIALOG, pParent)
	, amplitude_1(4)
	, amplitude_2(5)
	, amplitude_3(3)
	, frec_1(0.01)
	, freq_2(0.02)
	, freq_3(0.01)
	, fi_1(0)
	, fi_2(0)
	, fi_3(0)
	, length(256)
	, energ_noise(10)
	, percent_of_signal(10)
	, nevyazki(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CGaussSignalDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//  DDX_Text(pDX, IDC_A1, amplitude_1);
	DDX_Text(pDX, IDC_A1, amplitude_1);
	DDV_MinMaxDouble(pDX, amplitude_1, 0, 1000);
	DDX_Text(pDX, IDC_A2, amplitude_2);
	DDV_MinMaxDouble(pDX, amplitude_2, 0, 1000);
	DDX_Text(pDX, IDC_A3, amplitude_3);
	DDV_MinMaxDouble(pDX, amplitude_3, 0, 1000);
	DDX_Text(pDX, IDC_F1, frec_1);
	DDV_MinMaxDouble(pDX, frec_1, 0, 100);
	DDX_Text(pDX, IDC_F2, freq_2);
	DDV_MinMaxDouble(pDX, freq_2, 0, 100);
	DDX_Text(pDX, IDC_F3, freq_3);
	DDV_MinMaxDouble(pDX, freq_3, 0, 100);
	DDX_Text(pDX, IDC_FI1, fi_1);
	DDV_MinMaxDouble(pDX, fi_1, 0, 6.28318530718);
	DDX_Text(pDX, IDC_FI2, fi_2);
	DDV_MinMaxDouble(pDX, fi_2, 0, 6.28318530718);
	DDX_Text(pDX, IDC_FI3, fi_3);
	DDV_MinMaxDouble(pDX, fi_3, 0, 6.28318530718);
	DDX_Text(pDX, IDC_LEN_SIGN, length);
	DDX_Text(pDX, IDC_ENERG_NOISE, energ_noise);
	DDX_Text(pDX, IDC_PERCENT, percent_of_signal);
	DDX_Text(pDX, IDC_EDIT1, nevyazki);
}

BEGIN_MESSAGE_MAP(CGaussSignalDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CGaussSignalDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_EXIT, &CGaussSignalDlg::OnBnClickedExit)
	ON_BN_CLICKED(IDC_GEN_SHUM, &CGaussSignalDlg::OnBnClickedGenShum)
	ON_BN_CLICKED(IDC_PPF, &CGaussSignalDlg::OnBnClickedPpf)
	ON_BN_CLICKED(IDC_CLEAR, &CGaussSignalDlg::OnBnClickedClear)
END_MESSAGE_MAP()


// Обработчики сообщений CGaussSignalDlg

BOOL CGaussSignalDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Задает значок для этого диалогового окна.  Среда делает это автоматически,
	//  если главное окно приложения не является диалоговым
	SetIcon(m_hIcon, TRUE);			// Крупный значок
	SetIcon(m_hIcon, FALSE);		// Мелкий значок

	// TODO: добавьте дополнительную инициализацию
	//для  картинки
	PicWnd = GetDlgItem(IDC_GRAPH_DEFAULT);
	PicDc = PicWnd->GetDC();
	PicWnd->GetClientRect(&Pic);

	PicWnd_Sh = GetDlgItem(IDC_GRAPH_NOISE);
	PicDc_Sh = PicWnd_Sh->GetDC();
	PicWnd_Sh->GetClientRect(&Pic_Sh);

	PicWnd3 = GetDlgItem(IDC_GRAPH_SPECTR);
	PicDc3 = PicWnd3->GetDC();
	PicWnd3->GetClientRect(&Pic3);


	// перья
	setka_pen.CreatePen(		//для сетки
		PS_DOT,					//пунктирная
		1,						//толщина 1 пиксель
		RGB(0, 0, 0));			//цвет  черный

	osi_pen.CreatePen(			//координатные оси
		PS_SOLID,				//сплошная линия
		3,						//толщина 2 пикселя
		RGB(0, 0, 0));			//цвет черный

	graf_pen.CreatePen(			//график
		PS_SOLID,				//сплошная линия
		2,						//толщина 2 пикселя
		RGB(0, 0, 255));			//цвет синий

	grafshum_pen.CreatePen(			//график шума
		PS_SOLID,				//сплошная линия
		2,						//толщина 2 пикселя
		RGB(255, 0, 0));			//цвет красный

	grafspectr_pen.CreatePen(			//график шума
		PS_SOLID,				//сплошная линия
		2,						//толщина 2 пикселя
		RGB(255, 255, 0));			//цвет зеленый

	UpdateData(false);
	return TRUE;  // возврат значения TRUE, если фокус не передан элементу управления
}

// При добавлении кнопки свертывания в диалоговое окно нужно воспользоваться приведенным ниже кодом,
//  чтобы нарисовать значок.  Для приложений MFC, использующих модель документов или представлений,
//  это автоматически выполняется рабочей областью.

void CGaussSignalDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // контекст устройства для рисования

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Выравнивание значка по центру клиентского прямоугольника
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Нарисуйте значок
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();

		//*************************************************************************************************************** исходный сигнал

		PicDc->FillSolidRect(&Pic, RGB(225, 225, 225));			//закрашиваю фон 

		PicDc->SelectObject(&osi_pen);		//выбираем перо

		double *mas_s = new double[length];
		for (int i = 0; i < length; i++)
		{
			mas_s[i] = s(i);
		}

		Mashtab(mas_s, length, &mn, &mx);

		//область построения
		xmin = 0;			//минимальное значение х
		xmax = length;			//максимальное значение х
		ymin = mn * 1.5;			//минимальное значение y
		ymax = mx * 1.5;		//максимальное значение y

		xp = ((double)(Pic.Width()) / (xmax - xmin));			//Коэффициенты пересчёта координат по Х
		yp = -((double)(Pic.Height()) / (ymax - ymin));			//Коэффициенты пересчёта координат по У

		//создаём Ось Y
		PicDc->MoveTo(DOTS(0, ymax));
		PicDc->LineTo(DOTS(0, ymin));
		//создаём Ось Х
		PicDc->MoveTo(DOTS(xmin, 0));
		PicDc->LineTo(DOTS(xmax, 0));

		//подпись осей
		PicDc->TextOutW(DOTS(1, ymax - 0.2), _T("Y")); //Y
		PicDc->TextOutW(DOTS(xmax - 1, 0 - 1), _T("X")); //X


		PicDc->SelectObject(&setka_pen);

		//отрисовка сетки по y
		for (float x = xmin; x <= xmax; x += length / 4)
		{
			PicDc->MoveTo(DOTS(x, ymax));
			PicDc->LineTo(DOTS(x, ymin));
		}
		//отрисовка сетки по x
		for (float y = (int)ymin; y <= ymax; y += mx / 1.5)
		{
			PicDc->MoveTo(DOTS(xmin, y));
			PicDc->LineTo(DOTS(xmax, y));
		}


		//подпись точек на оси
		CFont font;
		font.CreateFontW(12, 0, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS || CLIP_LH_ANGLES, DEFAULT_QUALITY, DEFAULT_PITCH, _T("Times New Roman"));
		PicDc->SelectObject(font);
		//по Y с шагом 5
		for (double i = ymin; i <= ymax; i += mx / 2)
		{
			CString str;
			str.Format(_T("%.1f"), i);
			PicDc->TextOutW(DOTS(3, i), str);
		}
		//по X с шагом 0.5
		for (double j = xmin; j <= xmax; j += length / 4)
		{
			CString str;
			str.Format(_T("%.1f"), j);
			PicDc->TextOutW(DOTS(j - 0.25, -0.2), str);
		}

		//********************************************************************************************************************************************* шум
		PicDc_Sh->FillSolidRect(&Pic_Sh, RGB(225, 225, 225));			//закрашиваю фон 

		PicDc_Sh->SelectObject(&osi_pen);		//выбираем перо

		///////////////////////////////////////////
		double d = energ_noise / 100;

		double mas_energsignal = 0;
		for (int t = 0; t <= length; t++)
		{
			mas_energsignal += mas_s[t] * mas_s[t];
		}

		double *mas_psi = new double[length];
		for (int t = 0; t <= length; t++)
		{
			mas_psi[t] = Psi();
		}

		double qpsi = 0;
		for (int t = 0; t <= length; t++)
		{
			qpsi += mas_psi[t] * mas_psi[t];
		}

		double alpha = sqrt(d * mas_energsignal / qpsi);

		///////////////////////////////////////////

		double *mas_shum = new double[length];
		for (int i = 0; i <= length; i++)
		{
			mas_shum[i] = s(i) + alpha * mas_psi[i];
		}

		Mashtab(mas_shum, length, &mn_s, &mx_s);

		//область построения
		xmin_s = 0;			//минимальное значение х
		xmax_s = length;			//максимальное значение х
		ymin_s = mn_s * 1.5;		//минимальное значение y
		ymax_s = mx_s * 1.5;		//максимальное значение y


		xp_s = ((double)(Pic_Sh.Width()) / (xmax_s - xmin_s));			//Коэффициенты пересчёта координат по Х
		yp_s = -((double)(Pic_Sh.Height()) / (ymax_s - ymin_s));			//Коэффициенты пересчёта координат по У

		//создаём Ось Y
		PicDc_Sh->MoveTo(DOTS_SH(0, ymax_s));
		PicDc_Sh->LineTo(DOTS_SH(0, ymin_s));
		//создаём Ось Х
		PicDc_Sh->MoveTo(DOTS_SH(xmin_s, 0));
		PicDc_Sh->LineTo(DOTS_SH(xmax_s, 0));

		//подпись осей
		PicDc_Sh->TextOutW(DOTS_SH(1, ymax_s - 0.2), _T("Y")); //Y
		PicDc_Sh->TextOutW(DOTS_SH(xmax_s - 1, 0 - 1), _T("X")); //X


		PicDc_Sh->SelectObject(&setka_pen);

		//отрисовка сетки по y
		for (float x = xmin_s; x <= xmax_s; x += length / 4)
		{
			PicDc_Sh->MoveTo(DOTS_SH(x, ymax_s));
			PicDc_Sh->LineTo(DOTS_SH(x, ymin_s));
		}
		//отрисовка сетки по x
		for (float y = (int)ymin_s; y <= ymax_s; y += mx_s / 1.5)
		{
			PicDc_Sh->MoveTo(DOTS_SH(xmin_s, y));
			PicDc_Sh->LineTo(DOTS_SH(xmax_s, y));
		}


		//подпись точек на оси
		CFont font1;
		font1.CreateFontW(12, 0, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS || CLIP_LH_ANGLES, DEFAULT_QUALITY, DEFAULT_PITCH, _T("Times New Roman"));
		PicDc_Sh->SelectObject(font1);
		//по Y с шагом 5
		for (double i = ymin_s; i <= ymax_s; i += mx_s / 2)
		{
			CString str;
			str.Format(_T("%.1f"), i);
			PicDc_Sh->TextOutW(DOTS_SH(3, i), str);
		}
		//по X с шагом 0.5
		for (double j = xmin_s; j <= xmax_s; j += length / 4)
		{
			CString str;
			str.Format(_T("%.1f"), j);
			PicDc_Sh->TextOutW(DOTS_SH(j - 0.25, -0.2), str);
		}

		//******************************************************************************************************************************* отрисовка графика спектра

		PicDc3->FillSolidRect(&Pic3, RGB(225, 225, 225));			//закрашиваю фон 

		PicDc3->SelectObject(&osi_pen);		//выбираем перо

		int is = -1;
		cmplx *sp = new cmplx[length + 1];
		for (int i = 0; i <= length; i++)
		{
			sp[i].real = mas_shum[i];
			sp[i].image = 0;
		}

		fourea(sp, length, is);

		double *mas_mod = new double[length + 1];
		for (int i = 0; i <= length; i++)
		{
			mas_mod[i] = sqrt((sp[i].real)*(sp[i].real) + (sp[i].image)*(sp[i].image));
		}

		Mashtab(mas_mod, length, &mn3, &mx3);

		//область построения
		xmin3 = 0;			//минимальное значение х
		xmax3 = length;			//максимальное значение х
		ymin3 = mn3 - 100;		//минимальное значение y
		ymax3 = mx3 * 1.1;		//максимальное значение y


		xp3 = ((double)(Pic3.Width()) / (xmax3 - xmin3));			//Коэффициенты пересчёта координат по Х
		yp3 = -((double)(Pic3.Height()) / (ymax3 - ymin3));			//Коэффициенты пересчёта координат по У

		//создаём Ось Y
		PicDc3->MoveTo(DOTS3(0, ymax3));
		PicDc3->LineTo(DOTS3(0, ymin3));
		//создаём Ось Х
		PicDc3->MoveTo(DOTS3(xmin3, 0));
		PicDc3->LineTo(DOTS3(xmax3, 0));

		//подпись осей
		PicDc3->TextOutW(DOTS3(1, ymax3 - 0.2), _T("Y")); //Y
		PicDc3->TextOutW(DOTS3(xmax3 - 1, 0 - 1), _T("X")); //X


		PicDc3->SelectObject(&setka_pen);

		//отрисовка сетки по y
		for (float x = xmin3; x <= xmax3; x += length / 4)
		{
			PicDc3->MoveTo(DOTS3(x, ymax3));
			PicDc3->LineTo(DOTS3(x, ymin3));
		}
		//отрисовка сетки по x
		for (float y = (int)ymin3; y <= ymax3; y += mx3 / 2.5)
		{
			PicDc3->MoveTo(DOTS3(xmin3, y));
			PicDc3->LineTo(DOTS3(xmax3, y));
		}


		//подпись точек на оси
		CFont font2;
		font2.CreateFontW(12, 0, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS || CLIP_LH_ANGLES, DEFAULT_QUALITY, DEFAULT_PITCH, _T("Times New Roman"));
		PicDc3->SelectObject(font2);
		//по Y с шагом 5
		for (double i = ymin3; i <= ymax3; i += mx3 / 2)
		{
			CString str;
			str.Format(_T("%.1f"), i);
			PicDc3->TextOutW(DOTS3(3, i), str);
		}
		//по X с шагом 0.5
		for (double j = xmin3; j <= xmax3; j += length / 4)
		{
			CString str;
			str.Format(_T("%.1f"), j);
			PicDc3->TextOutW(DOTS3(j - 0.25, -0.2), str);
		}
	}
}

// Система вызывает эту функцию для получения отображения курсора при перемещении
//  свернутого окна.
HCURSOR CGaussSignalDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CGaussSignalDlg::Mashtab(double arr[], int dim, double *mmin, double *mmax)
{
	*mmin = *mmax = arr[0];

	for (int i = 0; i <= dim; i++)
	{
		if (*mmin > arr[i]) *mmin = arr[i];
		if (*mmax < arr[i]) *mmax = arr[i];
	}
}

double CGaussSignalDlg::s(int t)
{
	double A_mas[] = { amplitude_1, amplitude_2, amplitude_3 };
	double f_mas[] = { frec_1, freq_2, freq_3 };
	double fi_mas[] = { fi_1, fi_2, fi_3 };
	double result = 0;
	for (int i = 0; i < 3; i++)
	{
		result += A_mas[i] * sin(2 * Pi * f_mas[i] * t + fi_mas[i]);
	}
	return result;
}

double CGaussSignalDlg::Psi()
{
	float r = 0;
	for (int i = 1; i <= 12; i++)
	{
		r += ((rand() % 100) / (100 * 1.0) * 2) - 1;		// [-1;1]
	}
	return r / 12;
}

void CGaussSignalDlg::OnBnClickedButton1()
{
	// TODO: добавьте свой код обработчика уведомлений
	///////////////////////////////////////////////////////////////////////////////////////////// построение сигнала
	UpdateData(TRUE);
	OnPaint();
	double *mas_s = new double[length];
	for (int i = 0; i < length; i++)
	{
		mas_s[i] = s(i);
	}
	PicDc->SelectObject(&graf_pen);
	PicDc->MoveTo(DOTS(xmin, s(xmin)));
	for (int i = xmin; i <= xmax; i += 1)
	{
		PicDc->LineTo(DOTS(i, mas_s[i]));
	}
	/////////////////////////////////////////////////////////////////////////////////////////////
}

void CGaussSignalDlg::OnBnClickedGenShum()
{
	// TODO: добавьте свой код обработчика уведомлений
	///////////////////////////////////////////////////////////////////////////////////////////// перерисовка сигнала
	UpdateData(TRUE);
	OnPaint();
	PicDc->SelectObject(&graf_pen);
	PicDc->MoveTo(DOTS(xmin, s(xmin)));
	double *mas_s = new double[length + 1];
	for (int i = 0; i <= length; i++)
	{
		mas_s[i] = s(i);
	}
	for (int i = xmin; i <= xmax; i += 1)
	{
		PicDc->LineTo(DOTS(i, mas_s[i]));
	}

	///////////////////////////////////////////////////////////////////////////////////////////// расчет коэффициента альфа

	double d = energ_noise / 100;

	double mas_energsignal = 0;
	for (int t = 0; t <= length; t++)
	{
		mas_energsignal += mas_s[t] * mas_s[t];
	}

	double *mas_psi = new double[length];
	for (int t = 0; t <= length; t++)
	{
		mas_psi[t] = Psi();
	}

	double qpsi = 0;
	for (int t = 0; t <= length; t++)
	{
		qpsi += mas_psi[t] * mas_psi[t];
	}

	double alpha = sqrt(d * mas_energsignal / qpsi);

	///////////////////////////////////////////////////////////////////////////////////////////// построение самого шума (при надобности)

	/*PicDc_Sh->SelectObject(&grafshum_pen);

	double *mas_shum = new double[length+1];
	for (int i = 0; i <= length; i++)
	{
		mas_shum[i] = alp * Psi();
	}
	PicDc_Sh->MoveTo(DOTS_SH(xmin_s, mas_shum[(int)xmin_s]));

	for (int i = xmin_s; i <= xmax_s; i += 1)
	{
		PicDc_Sh->LineTo(DOTS_SH(i, mas_shum[i]));
	}*/

	///////////////////////////////////////////////////////////////////////////////////////////// построение сигнала с наложенным шумом

	PicDc_Sh->SelectObject(&grafshum_pen);

	double *mas_shum = new double[length + 1];
	for (int i = 0; i <= length; i++)
	{
		mas_shum[i] = mas_s[i] + alpha * mas_psi[i];
	}

	PicDc_Sh->MoveTo(DOTS_SH(xmin_s, mas_shum[(int)xmin_s]));

	for (int i = xmin_s; i <= xmax_s; i += 1)
	{
		PicDc_Sh->LineTo(DOTS_SH(i, mas_shum[i]));
	}

	/////////////////////////////////////////////////////////////////////////////////////////////
}

void CGaussSignalDlg::OnBnClickedPpf()
{
	// TODO: добавьте свой код обработчика уведомлений

	///////////////////////////////////////////////////////////////////////////////////////////// перерисовка сигнала
	UpdateData(TRUE);
	OnPaint();
	PicDc->SelectObject(&graf_pen);
	PicDc->MoveTo(DOTS(xmin, s(xmin)));
	double *mas_s = new double[length + 1];
	for (int i = 0; i <= length; i++)
	{
		mas_s[i] = s(i);
	}
	for (int i = xmin; i <= xmax; i += 1)
	{
		PicDc->LineTo(DOTS(i, mas_s[i]));
	}
	
	///////////////////////////////////////////////////////////////////////////////////////////// расчет альфа

	double d = energ_noise / 100;

	double mas_energsignal = 0;
	for (int t = 0; t <= length; t++)
	{
		mas_energsignal += mas_s[t] * mas_s[t];
	}

	double *mas_psi = new double[length];
	for (int t = 0; t <= length; t++)
	{
		mas_psi[t] = Psi();
	}

	double qpsi = 0;
	for (int t = 0; t <= length; t++)
	{
		qpsi += mas_psi[t] * mas_psi[t];
	}

	double alpha = sqrt(d * mas_energsignal / qpsi);

	///////////////////////////////////////////////////////////////////////////////////////////// перерисовка сигнала с шумом
	
	PicDc_Sh->SelectObject(&grafshum_pen);

	double *mas_shum = new double[length + 1];
	for (int i = 0; i <= length; i++)
	{
		mas_shum[i] = mas_s[i] + alpha * mas_psi[i];
	}

	PicDc_Sh->MoveTo(DOTS_SH(xmin_s, mas_shum[(int)xmin_s]));

	for (int i = xmin_s; i <= xmax_s; i += 1)
	{
		PicDc_Sh->LineTo(DOTS_SH(i, mas_shum[i]));
	}

	///////////////////////////////////////////////////////////////////////////////////////////// ППФ + рисуем спектр сигнала

	PicDc3->SelectObject(&graf_pen);
	
	int is = -1;	// ППФ

	cmplx *sp = new cmplx[length + 1];
	for (int i = 0; i <= length; i++)
	{
		sp[i].real = mas_shum[i];
		sp[i].image = 0;
	}

	fourea(sp, length, is);

	double *mas_mod = new double[length + 1];
	for (int i = 0; i <= length; i++)
	{
		mas_mod[i] = sqrt((sp[i].real)*(sp[i].real) + (sp[i].image)*(sp[i].image));
	}

	PicDc3->MoveTo(DOTS3(xmin3, mas_mod[(int)xmin3]));

	for (int i = xmin3; i <= xmax3; i += 1)
	{
		PicDc3->LineTo(DOTS3(i, mas_mod[i]));
	}
	/////////////////////////////////////////////////////////////////////////////////////////////
}

void CGaussSignalDlg::OnBnClickedClear()
{
	// TODO: добавьте свой код обработчика уведомлений

	///////////////////////////////////////////////////////////////////////////////////////////// перерисовка сигнала
	UpdateData(TRUE);
	OnPaint();
	PicDc->SelectObject(&graf_pen);
	PicDc->MoveTo(DOTS(xmin, s(xmin)));
	double *mas_s = new double[length + 1];
	for (int i = 0; i <= length; i++)
	{
		mas_s[i] = s(i);
	}
	for (int i = xmin; i <= xmax; i += 1)
	{
		PicDc->LineTo(DOTS(i, mas_s[i]));
	}

	///////////////////////////////////////////////////////////////////////////////////////////// расчет альфа

	double d = energ_noise / 100;

	double mas_energsignal = 0;
	for (int t = 0; t <= length; t++)
	{
		mas_energsignal += mas_s[t] * mas_s[t];
	}

	double *mas_psi = new double[length];
	for (int t = 0; t <= length; t++)
	{
		mas_psi[t] = Psi();
	}

	double qpsi = 0;
	for (int t = 0; t <= length; t++)
	{
		qpsi += mas_psi[t] * mas_psi[t];
	}

	double alpha = sqrt(d * mas_energsignal / qpsi);

	///////////////////////////////////////////////////////////////////////////////////////////// перерисовка шума

	PicDc_Sh->SelectObject(&grafshum_pen);

	double *mas_shum = new double[length + 1];
	for (int i = 0; i <= length; i++)
	{
		mas_shum[i] = mas_s[i] + alpha * mas_psi[i];
	}

	PicDc_Sh->MoveTo(DOTS_SH(xmin_s, mas_shum[(int)xmin_s]));

	for (int i = xmin_s; i <= xmax_s; i += 1)
	{
		PicDc_Sh->LineTo(DOTS_SH(i, mas_shum[i]));
	}

	///////////////////////////////////////////////////////////////////////////////////////////// перерисовка спектра сигнала

	int is = -1;

	PicDc3->SelectObject(&graf_pen);

	cmplx *sp = new cmplx[length + 1];
	for (int i = 0; i <= length; i++)
	{
		sp[i].real = mas_shum[i];
		sp[i].image = 0;
	}

	fourea(sp, length, is);

	double *mas_mod = new double[length + 1];
	for (int i = 0; i <= length; i++)
	{
		mas_mod[i] = sqrt((sp[i].real)*(sp[i].real) + (sp[i].image)*(sp[i].image));
	}

	PicDc3->MoveTo(DOTS3(xmin3, mas_mod[(int)xmin3]));

	for (int i = xmin3; i <= xmax3; i += 1)
	{
		PicDc3->LineTo(DOTS3(i, mas_mod[i]));
	}

	///////////////////////////////////////////////////////////////////////////////////////////// выделение отрезка частот спектра

	int t1 = 0;
	int t2 = 0;
	double Ei = 0;
	double Eo = 0;

	for (int i = 0; i <= length; i++)
	{
		Eo += mas_mod[i] * mas_mod[i];
	}

	for (int i = 0; i <= length; i++)
	{
		Ei += mas_mod[i] * mas_mod[i];
		if (Ei >= (percent_of_signal / 100) * Eo)
		{
			t1 = i;
			break;
		}
	}
	t2 = length - t1;

	///////////////////////////////////////////////////////////////////////////////////////////// зануляем выделенный отрезок

	cmplx *mas_null = new cmplx[length + 1];

	for (int i = 0; i <= length; i++)
	{
		if ((i > t1) && (i < t2))
		{
			mas_null[i].real = 0;
			mas_null[i].image = 0;
		}
		else
		{
			mas_null[i].real = sp[i].real;
			mas_null[i].image = sp[i].image;
		}
	}

	///////////////////////////////////////////////////////////////////////////////////////////// рисуем спектр с зануленным отрезком (шумом)

	double *mas_spectr_null = new double[length + 1];
	for (int i = 0; i <= length; i++)
	{
		mas_spectr_null[i] = sqrt((mas_null[i].real)*(mas_null[i].real)+(mas_null[i].image)*(mas_null[i].image));
	}

	PicDc3->SelectObject(&grafshum_pen);
	PicDc3->MoveTo(DOTS3(xmin3, mas_spectr_null[(int)xmin3]));

	for (int i = xmin3; i <= xmax3; i += 1)
	{
		PicDc3->LineTo(DOTS3(i, mas_spectr_null[i]));
	}

	///////////////////////////////////////////////////////////////////////////////////////////// ОФП + отрисовка очищенного сигнала

	int iss = 1;	//ОФП

	fourea(mas_null, length, iss);

	PicDc->SelectObject(&grafshum_pen);
	double *mas_modul_null = new double[length + 1];
	for (int i = 0; i <= length; i++)
	{
		mas_modul_null[i] = mas_null[i].real;	//sqrt((mas_null[i].real)*(mas_null[i].real) + (mas_null[i].image)*(mas_null[i].image));
	}

	PicDc->MoveTo(DOTS(xmin, mas_modul_null[(int)xmin]));

	for (int i = xmin; i <= xmax; i += 1)
	{
		PicDc->LineTo(DOTS(i, mas_modul_null[i]));
	}
	///////////////////////////////////////////////////////////////////////////////////////////// невязки
	
	double nev = 0;
	for (int i = 0; i <= length; i++)
	{
		nev += (mas_s[i] - mas_modul_null[i]) * (mas_s[i] - mas_modul_null[i]);
	}
	nevyazki = nev;
	UpdateData(FALSE);
}

void CGaussSignalDlg::OnBnClickedExit()
{
	// TODO: добавьте свой код обработчика уведомлений
	CDialogEx::OnCancel();
}
