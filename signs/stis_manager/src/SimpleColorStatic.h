#pragma once

class SimpleColorStatic : public CStatic
{
    DECLARE_DYNAMIC(SimpleColorStatic)

public:

    SimpleColorStatic();

    void set_text_color(COLORREF color);

    afx_msg HBRUSH CtlColor(CDC*, UINT);
    DECLARE_MESSAGE_MAP()

    CBrush m_brush;
    COLORREF m_color = ::GetSysColor(COLOR_BTNFACE);
};
