#pragma once

class CBarShader
{
public:
	CBarShader(uint32 height = 1, uint32 width = 1);
//==> removed 3D-Bar-display [shadow2004]
#ifdef BAR3D
	~CBarShader(void);
#endif
//<== removed 3D-Bar-display [shadow2004]
	//set the width of the bar
	void SetWidth(int width);

	//set the height of the bar
	void SetHeight(int height);

	//returns the width of the bar
	int GetWidth() {
		return m_iWidth;
	}

	//returns the height of the bar
	int GetHeight() {
		return m_iHeight;
	}

	//call this to blank the shaderwithout changing file size
	void Reset();

	//sets new file size and resets the shader
	void SetFileSize(uint32 fileSize);

	//fills in a range with a certain color, new ranges overwrite old
	void FillRange(uint32 start, uint32 end, COLORREF color);

	//fills in entire range with a certain color
	void Fill(COLORREF color);

	//draws the bar
//==> removed 3D-Bar-display [shadow2004]
#ifdef BAR3D
	void Draw(CDC* dc, int iLeft, int iTop, bool bFlat);
	void DrawPreview(CDC* dc, int iLeft, int iTop, uint8 previewLevel);		//Cax2 aqua bar

protected:
	void BuildModifiers();
	void FillRect(CDC *dc, LPRECT rectSpan, float fRed, float fGreen, float fBlue, bool bFlat);
	void FillRect(CDC *dc, LPRECT rectSpan, COLORREF color, bool bFlat);
#else
	void Draw(CDC* dc, int iLeft, int iTop);

protected:
	void FillRect(CDC *dc, LPRECT rectSpan, float fRed, float fGreen, float fBlue);
	void FillRect(CDC *dc, LPRECT rectSpan, COLORREF color);
#endif
//<== removed 3D-Bar-display [shadow2004]
	int    m_iWidth;
	int    m_iHeight;
	double m_dPixelsPerByte;
	double m_dBytesPerPixel;
	uint32 m_uFileSize;
//==> removed 3D-Bar-display [shadow2004]
#ifdef BAR3D
	bool	m_bIsPreview;
#endif
//<== removed 3D-Bar-display [shadow2004]

private:
	CRBMap<uint32, COLORREF> m_Spans;	// SLUGFILLER: speedBarShader
//==> removed 3D-Bar-display [shadow2004]
#ifdef BAR3D
	float *m_Modifiers;
	uint16 m_used3dlevel;
#endif
//<== removed 3D-Bar-display [shadow2004]
};
