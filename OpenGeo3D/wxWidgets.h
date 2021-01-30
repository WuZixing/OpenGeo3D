#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#ifndef ZERO_ERROR
#define ZERO_ERROR 1e-6
#endif

extern bool IsZero(double v);
extern bool IsEqual(double a, double b);