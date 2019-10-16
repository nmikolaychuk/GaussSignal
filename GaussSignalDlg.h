
// GaussSignalDlg.h: файл заголовка
//

#pragma once


// Диалоговое окно CGaussSignalDlg
class CGaussSignalDlg : public CDialogEx
{
// Создание
public:
	CGaussSignalDlg(CWnd* pParent = nullptr);	// стандартный конструктор

// Данные диалогового окна
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_GAUSS_SIGNAL_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// поддержка DDX/DDV


// Реализация
protected:
	HICON m_hIcon;

	// Созданные функции схемы сообщений
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CWnd * PicWnd;
	CWnd * PicWnd_Sh;
	CWnd * PicWnd3;
	CDC * PicDc;
	CDC * PicDc_Sh;
	CDC * PicDc3;
	CRect Pic;		//для сигнала
	CRect Pic_Sh;	//для шума
	CRect Pic3;		//для спектра

	//Переменные для работы с масштабом
	double xp, xp_s, xp3, yp, yp_s, yp3,		//коэфициенты пересчета
		xmin, xmin_s, xmin3, xmax,	xmax_s, xmax3,		//максисимальное и минимальное значение х 
		ymin, ymin_s, ymin3, ymax, ymax_s, ymax3;			//максисимальное и минимальное значение y
	double Pi = 3.141592653589;
	double mn, mx;
	double mn_s, mx_s;
	double mn3, mx3;
	
	afx_msg double s(int t);
	afx_msg double s_shum(int t);
	afx_msg double Psi();
	afx_msg double alpha();
	afx_msg void Mashtab(double arr[], int dim, double *mmin, double *mmax);

	//объявление ручек
	CPen osi_pen;		// ручка для осей
	CPen setka_pen;		// для сетки
	CPen graf_pen;		// для графика функции
	CPen grafshum_pen;	// для шума
	CPen grafspectr_pen;// для спектров
	
	double amplitude_1;
	double amplitude_2;
	double amplitude_3;
	double frec_1;
	double freq_2;
	double freq_3;
	double fi_1;
	double fi_2;
	double fi_3;
	int length;
	double energ_noise;
	double percent_of_signal;
	double nevyazki;
	
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedExit();
	afx_msg void OnBnClickedGenShum();
	afx_msg void OnBnClickedPpf();
	afx_msg void OnBnClickedClear();

	typedef struct cmplx { float real; float image; } Cmplx;
	//========================================================

	void CGaussSignalDlg::fourea(struct cmplx *data, int n, int is)
	{
		int i, j, istep;
		int m, mmax;
		float r, r1, theta, w_r, w_i, temp_r, temp_i;
		float pi = 3.1415926f;

		r = pi * is;
		j = 0;
		for (i = 0; i < n; i++)
		{
			if (i < j)
			{
				temp_r = data[j].real;
				temp_i = data[j].image;
				data[j].real = data[i].real;
				data[j].image = data[i].image;
				data[i].real = temp_r;
				data[i].image = temp_i;
			}
			m = n >> 1;
			while (j >= m) { j -= m; m = (m + 1) / 2; }
			j += m;
		}
		mmax = 1;
		while (mmax < n)
		{
			istep = mmax << 1;
			r1 = r / (float)mmax;
			for (m = 0; m < mmax; m++)
			{
				theta = r1 * m;
				w_r = (float)cos((double)theta);
				w_i = (float)sin((double)theta);
				for (i = m; i < n; i += istep)
				{
					j = i + mmax;
					temp_r = w_r * data[j].real - w_i * data[j].image;
					temp_i = w_r * data[j].image + w_i * data[j].real;
					data[j].real = data[i].real - temp_r;
					data[j].image = data[i].image - temp_i;
					data[i].real += temp_r;
					data[i].image += temp_i;
				}
			}
			mmax = istep;
		}
		if (is > 0)
			for (i = 0; i < n; i++)
			{
			data[i].real /= (float)n;
			data[i].image /= (float)n;
			}
	}
};
