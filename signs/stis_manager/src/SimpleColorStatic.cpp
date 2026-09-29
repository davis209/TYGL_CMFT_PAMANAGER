#include "StdAfx.h"
#include "SimpleColorStatic.h"

IMPLEMENT_DYNAMIC(SimpleColorStatic, CStatic)

SimpleColorStatic::SimpleColorStatic()
{
    m_brush.CreateSolidBrush(::GetSysColor(COLOR_BTNFACE));
}

BEGIN_MESSAGE_MAP(SimpleColorStatic, CStatic)
    ON_WM_CTLCOLOR_REFLECT()
END_MESSAGE_MAP()

HBRUSH SimpleColorStatic::CtlColor(CDC* pDC, UINT nCtlColor)
{
    pDC->SetBkMode(TRANSPARENT);
    pDC->SetTextColor(m_color);
    return m_brush;
}

void SimpleColorStatic::set_text_color(COLORREF color)
{
    if (m_color != color)
    {
        m_color = color;
        Invalidate();
    }
}
