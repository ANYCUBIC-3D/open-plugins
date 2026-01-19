// Copyright (c) [Year] [name of copyright holder]
// Open-Plugin is licensed under Mulan PSL v2.
// You can use this software according to the terms and conditions of the Mulan
// PSL v2.
// You may obtain a copy of Mulan PSL v2 at:
//          http://license.coscl.org.cn/MulanPSL2
// THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY
// KIND, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// NON-INFRINGEMENT, MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
// See the Mulan PSL v2 for more details.

#pragma once
#include <wx/wx.h>
#include "program_color.hxx"
#include <wx/graphics.h>

struct GradientStop
{
    double   position;
    wxColour color;
};

static double calculateColorDifference_RGB(const wxColour& color1, const wxColour& color2)
{
    int deltaR = color2.Red() - color1.Red();
    int deltaG = color2.Green() - color1.Green();
    int deltaB = color2.Blue() - color1.Blue();
    int deltaA = color2.Alpha() - color1.Alpha();

    return std::sqrt(deltaR * deltaR + deltaG * deltaG + deltaB * deltaB + deltaA * deltaA);
}

static wxColour getTextFitColor(const wxColour& color1)
{
    wxColour whiteColor = COLOR_Neutral_01;
    wxColour blackColor = COLOR_Neutral_10;

    double luminance = 0.299 * color1.Red() + 0.587 * color1.Green() + 0.114 * color1.Blue();
    return (luminance > 140) ? blackColor : whiteColor;
}

static wxColour InterpolateColor(const wxColour& c1, const wxColour& c2, double t)
{
    int r = c1.Red() + (c2.Red() - c1.Red()) * t;
    int g = c1.Green() + (c2.Green() - c1.Green()) * t;
    int b = c1.Blue() + (c2.Blue() - c1.Blue()) * t;
    int a = c1.Alpha() + (c2.Alpha() - c1.Alpha()) * t;
    return wxColour(r, g, b, a);
}

static wxColour GetGradientColor(const wxVector<GradientStop>& stops, double t)
{
    if (stops.empty())
        return *wxBLACK;
    if (t <= stops.front().position)
        return stops.front().color;
    if (t >= stops.back().position)
        return stops.back().color;

    for (size_t i = 0; i < stops.size() - 1; ++i) {
        if (t >= stops[i].position && t <= stops[i + 1].position) {
            double localT = (t - stops[i].position) / (stops[i + 1].position - stops[i].position);
            return InterpolateColor(stops[i].color, stops[i + 1].color, localT);
        }
    }

    return stops.back().color;
}


static wxVector<GradientStop> GenerateGradientStops(const wxVector<wxColour>& colors)
{
    wxVector<GradientStop> stops;
    size_t                 count = colors.size();

    if (count == 0)
        return stops;

    if (count == 2) {
        stops.push_back({0.2, colors[0]});
        stops.push_back({0.8, colors[1]});

    } else if (count == 3) {
        stops.push_back({0.1, colors[0]});
        stops.push_back({0.5, colors[1]});
        stops.push_back({0.9, colors[2]});
    } else if (count == 4) {
        stops.push_back({0.0, colors[0]});
        stops.push_back({0.33, colors[1]});
        stops.push_back({0.66, colors[2]});
        stops.push_back({1.0, colors[3]});
    } else {
        for (size_t i = 0; i < count; ++i) {
            double pos = (count == 1) ? 0.0 : static_cast<double>(i) / (count - 1);
            stops.push_back({pos, colors[i]});
        }
    }

    return stops;
}

static void DrawGradientColorEvent(wxDC& dc, wxPoint circularPoint, wxVector<wxColour> colorList, int dipRadius, bool horizontal)
{
    int                    centerX  = circularPoint.x;
    int                    centerY  = circularPoint.y;
    int                    radius   = dipRadius;
    int                    diameter = radius * 2;
    wxVector<GradientStop> stops    = GenerateGradientStops(colorList);

    for (int i = 0; i < diameter; ++i) {
        int pos = centerX - radius + i;

        double   t     = static_cast<double>(i) / diameter;
        wxColour color = GetGradientColor(stops, t);
        dc.SetPen(wxPen(color));

        for (int j = 0; j < diameter; ++j) {
            int x = centerX - radius + (horizontal ? i : j);
            int y = centerY - radius + (horizontal ? j : i);

            int dx = x - centerX;
            int dy = y - centerY;

            if (dx * dx + dy * dy <= radius * radius) {
                dc.DrawPoint(x, y);
            }
        }
    }
}

static void DrawRoundedRectangleColorEvent(wxDC& dc, wxRect rect, wxVector<wxColour> colorList, int radius, bool roundTopCorners)
{
    wxVector<GradientStop> stops = GenerateGradientStops(colorList);

    int x = rect.x, y = rect.y, w = rect.width, h = rect.height;
    int steps = w;

    for (int i = 0; i < steps; ++i) {
        int px = x + i;
        int pw = 1;

        double pos = static_cast<double>(i) / (steps - 1);

        size_t idx = 0;
        for (; idx < stops.size() - 1; ++idx) {
            if (pos <= stops[idx + 1].position)
                break;
        }
        if (idx >= stops.size() - 1)
            idx = stops.size() - 2;

        const auto& stop1     = stops[idx];
        const auto& stop2     = stops[idx + 1];
        double      local_pos = (pos - stop1.position) / (stop2.position - stop1.position);
        local_pos             = std::clamp(local_pos, 0.0, 1.0);

        wxColour color = InterpolateColor(stop1.color, stop2.color, local_pos);
        dc.SetPen(wxPen(color));
        dc.SetBrush(wxBrush(color));

        int py = y;
        int ph = h;

        if (px < x + radius) {
            double dx = x + radius - px;
            if (dx <= radius) {
                double dy        = std::sqrt(radius * radius - dx * dx);
                int    bottomCut = radius - static_cast<int>(dy);
                ph -= bottomCut;
            }
        } else if (px >= x + w - radius) {
            double dx = px - (x + w - radius - 1);
            if (dx <= radius) {
                double dy        = std::sqrt(radius * radius - dx * dx);
                int    bottomCut = radius - static_cast<int>(dy);
                ph -= bottomCut;
            }
        }

        if (roundTopCorners) {
            if (px < x + radius) {
                double dx = x + radius - px;
                if (dx <= radius) {
                    double dy     = std::sqrt(radius * radius - dx * dx);
                    int    topCut = radius - static_cast<int>(dy);
                    py += topCut;
                    ph -= topCut;
                }
            } else if (px >= x + w - radius) {
                double dx = px - (x + w - radius - 1);
                if (dx <= radius) {
                    double dy     = std::sqrt(radius * radius - dx * dx);
                    int    topCut = radius - static_cast<int>(dy);
                    py += topCut;
                    ph -= topCut;
                }
            }
        }

        if (ph > 0)
            dc.DrawRectangle(px, py, pw, ph);
    }
}

static void DrawRoundedRectangleColorVerticalEvent(wxDC& dc, wxRect rect, wxVector<wxColour> colorList, int radius, bool roundTopCorners)
{
    wxVector<GradientStop> stops = GenerateGradientStops(colorList);

    bool horizontal = false;

    int segments = 16;
    int x = rect.x, y = rect.y, w = rect.width, h = rect.height;

    int steps = horizontal ? w : h;

    for (int i = 0; i < steps; ++i) {
        int px = horizontal ? x + i : x;
        int py = horizontal ? y : y + i;
        int pw = horizontal ? 1 : w;
        int ph = horizontal ? h : 1;

        double pos = static_cast<double>(i) / (steps - 1);

        size_t idx = 0;
        for (; idx < stops.size() - 1; ++idx) {
            if (pos <= stops[idx + 1].position)
                break;
        }
        if (idx >= stops.size() - 1)
            idx = stops.size() - 2;

        const auto& stop1     = stops[idx];
        const auto& stop2     = stops[idx + 1];
        double      local_pos = (pos - stop1.position) / (stop2.position - stop1.position);
        local_pos             = std::clamp(local_pos, 0.0, 1.0);

        wxColour color = InterpolateColor(stop1.color, stop2.color, local_pos);
        dc.SetPen(wxPen(color));
        dc.SetBrush(wxBrush(color));

        std::vector<wxPoint> line;

        bool inRoundedArea = false;
        if (!horizontal && (py + ph) > (y + h - radius))
            inRoundedArea = true;
        if (horizontal && (px) > (x + w - radius))
            inRoundedArea = true;

        if (!inRoundedArea) {
            dc.DrawRectangle(px, py, pw, ph);
        } else {
            int inner_left  = x + radius;
            int inner_right = x + w - radius;
            int bottom      = y + h;

            if (horizontal) {
                if (px < inner_left || px > inner_right) {
                    double cx = (px < inner_left) ? inner_left : inner_right;
                    double dx = std::abs(px - cx);
                    if (dx > radius)
                        continue;
                    double dy   = std::sqrt(radius * radius - dx * dx);
                    int    maxY = static_cast<int>(bottom - dy);
                    dc.DrawRectangle(px, y, 1, maxY - y);
                } else {
                    dc.DrawRectangle(px, y, 1, h);
                }
            } else {
                if (py < y + h - radius) {
                    dc.DrawRectangle(x, py, w, 1);
                } else {
                    double dy = py - (y + h - radius);
                    if (dy > radius)
                        continue;
                    double dx    = std::sqrt(radius * radius - dy * dy);
                    int    left  = static_cast<int>(x + radius - dx);
                    int    right = static_cast<int>(x + w - radius + dx);
                    dc.DrawRectangle(left, py, right - left, 1);
                }
            }
        }
    }
}

static void DrawRoundedRectangleColorEvent(wxGraphicsContext* gc, wxRect rect, wxVector<wxColour> colorList, int radius, bool verticalGradient, bool roundTopCorners)
{
    wxVector<GradientStop> stops = GenerateGradientStops(colorList);

    int x = rect.x, y = rect.y, w = rect.width, h = rect.height;
    int steps = verticalGradient ? h : w;

    for (int i = 0; i < steps; ++i) {
        int posCoord = verticalGradient ? y + i : x + i;
        int length   = 1;

        double pos = static_cast<double>(i) / (steps - 1);

        size_t idx = 0;
        for (; idx < stops.size() - 1; ++idx) {
            if (pos <= stops[idx + 1].position)
                break;
        }
        if (idx >= stops.size() - 1)
            idx = stops.size() - 2;

        const auto& stop1     = stops[idx];
        const auto& stop2     = stops[idx + 1];
        double      local_pos = (pos - stop1.position) / (stop2.position - stop1.position);
        local_pos             = std::clamp(local_pos, 0.0, 1.0);

        wxColour color = InterpolateColor(stop1.color, stop2.color, local_pos);
        gc->SetPen(wxPen(color));
        gc->SetBrush(wxBrush(color));

        int px = x;
        int py = y;
        int pw = w;
        int ph = h;

        if (verticalGradient) {
            py = posCoord;
            ph = length;
        } else {
            px = posCoord;
            pw = length;
        }

        if (!verticalGradient) {
            if (px < x + radius) {
                double dx = x + radius - px;
                if (dx <= radius) {
                    double dy        = std::sqrt(radius * radius - dx * dx);
                    int    bottomCut = radius - static_cast<int>(dy);
                    ph -= bottomCut;
                }
            } else if (px >= x + w - radius) {
                double dx = px - (x + w - radius - 1);
                if (dx <= radius) {
                    double dy        = std::sqrt(radius * radius - dx * dx);
                    int    bottomCut = radius - static_cast<int>(dy);
                    ph -= bottomCut;
                }
            }

            if (roundTopCorners) {
                if (px < x + radius) {
                    double dx = x + radius - px;
                    if (dx <= radius) {
                        double dy     = std::sqrt(radius * radius - dx * dx);
                        int    topCut = radius - static_cast<int>(dy);
                        py += topCut;
                        ph -= topCut;
                    }
                } else if (px >= x + w - radius) {
                    double dx = px - (x + w - radius - 1);
                    if (dx <= radius) {
                        double dy     = std::sqrt(radius * radius - dx * dx);
                        int    topCut = radius - static_cast<int>(dy);
                        py += topCut;
                        ph -= topCut;
                    }
                }
            }
        } else {
            if (py < y + radius) {
                double dy = y + radius - py;
                if (dy <= radius) {
                    double dx       = std::sqrt(radius * radius - dy * dy);
                    int    rightCut = radius - static_cast<int>(dx);
                    pw -= rightCut;
                }
            } else if (py >= y + h - radius) {
                double dy = py - (y + h - radius - 1);
                if (dy <= radius) {
                    double dx       = std::sqrt(radius * radius - dy * dy);
                    int    rightCut = radius - static_cast<int>(dx);
                    pw -= rightCut;
                }
            }

            if (roundTopCorners) {
                if (py < y + radius) {
                    double dy = y + radius - py;
                    if (dy <= radius) {
                        double dx      = std::sqrt(radius * radius - dy * dy);
                        int    leftCut = radius - static_cast<int>(dx);
                        px += leftCut;
                        pw -= leftCut;
                    }
                } else if (py >= y + h - radius) {
                    double dy = py - (y + h - radius - 1);
                    if (dy <= radius) {
                        double dx      = std::sqrt(radius * radius - dy * dy);
                        int    leftCut = radius - static_cast<int>(dx);
                        px += leftCut;
                        pw -= leftCut;
                    }
                }
            }
        }
        if ((pw > 0 && ph > 0))
            gc->DrawRectangle(px, py, pw, ph);
    }
}
