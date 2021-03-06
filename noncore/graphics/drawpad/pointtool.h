/***************************************************************************
 *                                                                         *
 *   DrawPad - a drawing program for Opie Environment                      *
 *                                                                         *
 *   (C) 2002 by S. Prud'homme <prudhomme@laposte.net>                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef POINTTOOL_H
#define POINTTOOL_H

#include "tool.h"

#include <qpointarray.h>

class PointTool : public Tool
{
public:
    PointTool(DrawPad* drawPad, DrawPadCanvas* drawPadCanvas);
    ~PointTool();

    void mousePressEvent(QMouseEvent* e);
    void mouseReleaseEvent(QMouseEvent* e);
    void mouseMoveEvent(QMouseEvent* e);

private:
    bool m_mousePressed;
    QPointArray m_polyline;
};

#endif // POINTTOOL_H
