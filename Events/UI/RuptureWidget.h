#ifndef RUPTUREWIDGET_H
#define RUPTUREWIDGET_H
/* *****************************************************************************
Copyright (c) 2016-2021, The Regents of the University of California (Regents).
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of the FreeBSD Project.

REGENTS SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
THE SOFTWARE AND ACCOMPANYING DOCUMENTATION, IF ANY, PROVIDED HEREUNDER IS
PROVIDED "AS IS". REGENTS HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT,
UPDATES, ENHANCEMENTS, OR MODIFICATIONS.

*************************************************************************** */

// Written by: Stevan Gavrilovic

#include "SimCenterAppWidget.h"

class PointSourceRuptureWidget;
class EarthquakeRuptureForecastWidget;
class OpenQuakeScenarioWidget;
class OpenQuakeClassicalWidget;
class OpenQuakeUserSpecifiedWidget;
class HazardOccurrenceWidget;

class QGroupBox;
class QComboBox;
class QStackedWidget;

class RuptureWidget : public SimCenterAppWidget
{
    Q_OBJECT
public:
    explicit RuptureWidget(QWidget *parent = nullptr);

    QJsonObject getJson(void);
    QString getWidgetType(void) const;
    QString getGMPELogicTree(void) const;
    QString getEQNum(void) const;
    bool outputToJSON(QJsonObject &jsonObject);
    bool inputFromJSON(QJsonObject &jsonObject);

public slots:
    void handleSelectionChanged(const QString& selection);

signals:
    void widgetTypeChanged(QString newWidgetType);

private:
    QGroupBox* ruptureGroupBox;
    QComboBox* ruptureSelectionCombo;
    QStackedWidget* theRootStackedWidget;
    PointSourceRuptureWidget* pointSourceWidget;
    EarthquakeRuptureForecastWidget* erfWidget;
    OpenQuakeScenarioWidget* oqsbWidget; // widget connecting OpenQuake Scenario
    OpenQuakeClassicalWidget* oqcpWidget; // widget connecting OpenQuake classical PSHA
    OpenQuakeUserSpecifiedWidget* oqcpuWidget; // widget connecting OpenQuake classical PSHA (Uesr ini)
    HazardOccurrenceWidget* hoWidget; // widget connecting OpenQuake classical PSHA (Uesr ini)
    QString widgetType = "OpenSHA ERF"; // widget type
};

#endif // RUPTUREWIDGET_H
