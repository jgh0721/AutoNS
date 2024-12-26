#ifndef __HDR_TYPE_VARIANT__
#define __HDR_TYPE_VARIANT__

#include <windows.h>

constexpr int VAR_TYPE_NULL         = 0;
constexpr int VAR_TYPE_INT32        = 1;
constexpr int VAR_TYPE_INT64        = 2;
constexpr int VAR_TYPE_DOUBLE       = 3;
constexpr int VAR_TYPE_STRING       = 4;
constexpr int VAR_TYPE_ARRAY        = 5;
constexpr int VAR_TYPE_HWND         = 6;

class Variant
{

private:

    union
    {
        int             m_nValue;
        long long       m_llValue;
        double          m_dblValue;
        Variant*        m_pValue;
        HWND            m_hWnd;
    };
};

#endif // __HDR_TYPE_VARIANT__
