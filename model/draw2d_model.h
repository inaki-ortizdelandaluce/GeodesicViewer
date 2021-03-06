// --------------------------------------------------------------------------------
/*
    draw2d_model.h

  Copyright (c) 2009  Thomas Mueller, Frank Grave


   This file is part of the GeodesicViewer.

   The GeodesicViewer is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   The GeodesicViewer is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with the GeodesicViewer.  If not, see <http://www.gnu.org/licenses/>.
*/

/*!  \class  Draw2dModel
     \brief  Render engine for the 2d representation of geodesics based on Qwt.

*/
// --------------------------------------------------------------------------------

#ifndef DRAW2D_MODEL_H
#define DRAW2D_MODEL_H

#include <iostream>

#include <qwt_plot.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include <qwt_legend.h>
#include <qwt_data.h>
#include <qwt_text.h>

#include <gdefs.h>
#include <m4dGlobalDefs.h>
#include <metric/m4dMetric.h>
#include <math/TransCoordinates.h>
#include <extra/m4dObject.h>

enum enum_curve_style {
    enum_curve_lines = 0,
    enum_curve_dots
};


// ---------------------------------------------------
//    class definition:   Crosshairs_model
// ---------------------------------------------------
class Draw2dModel : public QWidget {
    Q_OBJECT

public:
    Draw2dModel();
    virtual ~Draw2dModel();

// --------- public methods -----------
public:
    void  setStyle(const enum_curve_style  style);
    void  setColor(const QColor col);

    void  setPoints(m4d::Object* data, m4d::enum_draw_type  dtype = m4d::enum_draw_pseudocart);

    void  setCoordIndices(int abscissa, int ordinate);
    void  getCoordIndices(int &abscissa, int &ordinate);

    void  doReplot() {
        plot->replot();
    }

    void  setScaling(QPointF sx, QPointF sy);
    void  getScaling(QPointF &sx, QPointF &sy);

    void  clearObjects();
    void  addObject(QList<QPointF> &points, QColor col);


// ------------ signals ---------------
signals:
    void  scalingChanged();

// -------- protected methods ---------
protected:
    virtual void keyPressEvent(QKeyEvent* event);
    virtual void keyReleaseEvent(QKeyEvent* event);

    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);
    virtual void mouseMoveEvent(QMouseEvent* event);

// -------- private attributes --------
private:
    QwtPlotCurve*      curve;
    QwtArray<double>   mX;
    QwtArray<double>   mY;

    QPoint             mLastPos;
    int                mKeyPressed;
    Qt::MouseButton    mButtonPressed;
    QColor             mCurrColor;

    QPointF            scaleX, oldScaleX;
    QPointF            scaleY, oldScaleY;
    double             xLabel,yLabel;

    int                mAbscissa;
    int                mOrdinate;

    QList<QwtPlotCurve*>  mObjects;

    QwtPlot* plot;
};

#endif



