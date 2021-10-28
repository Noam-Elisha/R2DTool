#ifndef EmbeddedMapViewWidget_H
#define EmbeddedMapViewWidget_H
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

// Written by: Stevan Gavrilovic, Frank McKenna

#include "RectangleGrid.h"
#include "NodeHandle.h"

#ifdef ARC_GIS
class SimCenterMapGraphicsView;
#endif

#include <QObject>
#include <QWidget>

#ifdef Q_GIS
class QgsMapCanvas;
class SimCenterMapcanvasWidget;
#endif

class RectangleGrid;
class QGraphicsSimpleTextItem;
class QVBoxLayout;
class QGraphicsView;

class EmbeddedMapViewWidget : public QWidget
{
    Q_OBJECT
public:
#ifdef ARC_GIS
    EmbeddedMapViewWidget(QGraphicsView* parent);
#endif

#ifdef Q_GIS
    EmbeddedMapViewWidget(SimCenterMapcanvasWidget* mapCanvasWidget);
#endif

    RectangleGrid* getGrid(void);
    NodeHandle* getPoint(void);

#ifdef ARC_GIS
    virtual void setCurrentlyViewable(bool status);
#endif

public slots:

    void addGridToScene(void);
    void removeGridFromScene(void);

    void addPointToScene(void);
    void removePointFromScene(void);

protected:

    // Override widget events
    void showEvent(QShowEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

    QVBoxLayout *theViewLayout;
    QGraphicsSimpleTextItem* displayText;
    std::unique_ptr<RectangleGrid> grid;
    std::unique_ptr<NodeHandle> point;

#ifdef ARC_GIS
    SimCenterMapGraphicsView *theNewView;
#endif

#ifdef Q_GIS
    QgsMapCanvas* mapCanvas;
    SimCenterMapcanvasWidget* mapCanvasWidget;
#endif

};

#endif // EmbeddedMapViewWidget_H
