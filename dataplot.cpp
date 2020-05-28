/***************************************************************************
 *   Copyright (C) 2003 by Matthias H. Hennig                              *
 *   hennig@cn.stir.ac.uk                                                  *
 *   Copyright (C) 2020 by Belinda Kneubühler                              *
 *   belinda.kneubuehler@outlook.com                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "dataplot.h"

DataPlot::DataPlot(double *xData, double *yData, int length,
                   double maxY, double minY, QWidget *parent) :
        QwtPlot(parent),
        xData(xData),
        yData(yData),
        max(maxY),
        min(minY),
        updateCtr(1) {
    setTitle("Raw Data");
    setAxisTitle(QwtPlot::xBottom, "Time/ms");
    setAxisTitle(QwtPlot::yLeft, "voltage / V");

    setAxisScale(QwtPlot::yLeft, min, max);
    // Insert new curve for raw data
    dataCurve = new QwtPlotCurve("Raw Data");
    dataCurve->setPen(QPen(Qt::blue, 3));
    dataCurve->setRawSamples(xData, yData, length);
    dataLength = length;
    dataCurve->attach(this);
}

void DataPlot::setDataLength(int length) {
    dataLength = length;
    dataCurve->setRawSamples(xData, yData, dataLength);
    replot();
}

void DataPlot::setNewData(double yNew) {
    static int cnt = 0;
    memmove(yData, yData + 1, (dataLength - 1) * sizeof(yData[0]));
    yData[dataLength - 1] = yNew;
}
