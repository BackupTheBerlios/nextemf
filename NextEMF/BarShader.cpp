//this file is part of eMule
//Copyright (C)2002 Merkur ( devs@emule-project.net / http://www.emule-project.net )
//
//This program is free software; you can redistribute it and/or
//modify it under the terms of the GNU General Public License
//as published by the Free Software Foundation; either
//version 2 of the License, or (at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program; if not, write to the Free Software
//Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#include "stdafx.h"
#include <math.h>
#include "emule.h"
#include "barshader.h"
#include "Preferences.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// Why does _USE_MATH_DEFINES work in debug builds, but not in release builds??
#ifndef M_PI
#define M_PI       3.14159265358979323846
#endif

#define HALF(X) (((X) + 1) / 2)

CBarShader::CBarShader(uint32 height, uint32 width) {
	m_iWidth = width;
	m_iHeight = height;
	m_uFileSize = 1;
	m_Spans.SetAt(0, 0);	// SLUGFILLER: speedBarShader
}

void CBarShader::Reset() {
	Fill(0);
}

void CBarShader::SetWidth(int width) {
	if(m_iWidth != width) {
		m_iWidth = width;
		if (m_uFileSize)
			m_dPixelsPerByte = (double)m_iWidth / m_uFileSize;
		else
			m_dPixelsPerByte = 0.0;
		if (m_iWidth)
			m_dBytesPerPixel = (double)m_uFileSize / m_iWidth;
		else
			m_dBytesPerPixel = 0.0;
	}
}

void CBarShader::SetFileSize(uint32 fileSize) {
	if(m_uFileSize != fileSize) {
		m_uFileSize = fileSize;

		if (m_uFileSize)
			m_dPixelsPerByte = (double)m_iWidth / m_uFileSize;
		else
			m_dPixelsPerByte = 0.0;

		if (m_iWidth)
			m_dBytesPerPixel = (double)m_uFileSize / m_iWidth;
		else
			m_dBytesPerPixel = 0.0;
	}
}

void CBarShader::SetHeight(int height) {
	if(m_iHeight != height) {
		m_iHeight = height;
	}
}

void CBarShader::FillRange(uint32 start, uint32 end, COLORREF color) {
	if(end > m_uFileSize)
		end = m_uFileSize;

	if(start >= end)
		return;

	// SLUGFILLER: speedBarShader
	POSITION endpos = m_Spans.FindFirstKeyAfter(end+1);

	if (endpos)
		m_Spans.GetPrev(endpos);
	else
		endpos = m_Spans.GetTailPosition();

	ASSERT(endpos != NULL);

	COLORREF endcolor = m_Spans.GetValueAt(endpos);
	endpos = m_Spans.SetAt(end, endcolor);

	for (POSITION pos = m_Spans.FindFirstKeyAfter(start+1); pos != endpos; ) {
		POSITION pos1 = pos;
		m_Spans.GetNext(pos);
		m_Spans.RemoveAt(pos1);
	}
	
	m_Spans.GetPrev(endpos);

	if (m_Spans.GetValueAt(endpos) != color)
		m_Spans.SetAt(start, color);
	// SLUGFILLER: speedBarShader
}

void CBarShader::Fill(COLORREF color) {
	// SLUGFILLER: speedBarShader
	m_Spans.RemoveAll();
	m_Spans.SetAt(0, color);
	m_Spans.SetAt(m_uFileSize, 0);
	// SLUGFILLER: speedBarShader
}

void CBarShader::Draw(CDC* dc, int iLeft, int iTop) {
	POSITION pos = m_Spans.GetHeadPosition();	// SLUGFILLER: speedBarShader
	RECT rectSpan;
	rectSpan.top = iTop;
	rectSpan.bottom = iTop + m_iHeight;
	rectSpan.right = iLeft;

	int iBytesInOnePixel = (int)(m_dBytesPerPixel + 0.5f);
	uint32 start = 0;//bsCurrent->start;
	// SLUGFILLER: speedBarShader
	COLORREF color = m_Spans.GetValueAt(pos);
	m_Spans.GetNext(pos);
	// SLUGFILLER: speedBarShader
	while(pos != NULL && rectSpan.right < (iLeft + m_iWidth)) {	// SLUGFILLER: speedBarShader
		uint32 uSpan = m_Spans.GetKeyAt(pos) - start;	// SLUGFILLER: speedBarShader
		int iPixels = (int)(uSpan * m_dPixelsPerByte + 0.5f);
		if(iPixels > 0) {
			rectSpan.left = rectSpan.right;
			rectSpan.right += iPixels;
			FillRect(dc, &rectSpan, color);	// SLUGFILLER: speedBarShader

			start += (int)(iPixels * m_dBytesPerPixel + 0.5f);
		} else {
			float fRed = 0;
			float fGreen = 0;
			float fBlue = 0;
			uint32 iEnd = start + iBytesInOnePixel;
			int iLast = start;
			// SLUGFILLER: speedBarShader
			do {
				float fWeight = (float)((min(m_Spans.GetKeyAt(pos), iEnd) - iLast) * m_dPixelsPerByte);
				fRed   += GetRValue(color) * fWeight;
				fGreen += GetGValue(color) * fWeight;
				fBlue  += GetBValue(color) * fWeight;
				if(m_Spans.GetKeyAt(pos) > iEnd)
					break;
				iLast = m_Spans.GetKeyAt(pos);
				color = m_Spans.GetValueAt(pos);
				m_Spans.GetNext(pos);
			} while(pos != NULL);
			// SLUGFILLER: speedBarShader
			rectSpan.left = rectSpan.right;
			rectSpan.right++;
			FillRect(dc, &rectSpan, fRed, fGreen, fBlue);
			start += iBytesInOnePixel;
		}
		// SLUGFILLER: speedBarShader
		while(pos != NULL && m_Spans.GetKeyAt(pos) < start) {
			color = m_Spans.GetValueAt(pos);
			m_Spans.GetNext(pos);
		}
		// SLUGFILLER: speedBarShader
	}
}

void CBarShader::FillRect(CDC *dc, LPRECT rectSpan, COLORREF color) {
	if(!color)
		dc->FillRect(rectSpan, &CBrush(color));
	else
		FillRect(dc, rectSpan, GetRValue(color), GetGValue(color), GetBValue(color));
}

void CBarShader::FillRect(CDC *dc, LPRECT rectSpan, float fRed, float fGreen,
						  float fBlue) {

		COLORREF color = RGB((int)(fRed + .5f), (int)(fGreen + .5f), (int)(fBlue + .5f));
		dc->FillRect(rectSpan, &CBrush(color));

}
