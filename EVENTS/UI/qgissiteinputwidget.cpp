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

// Written by: Kuanshi Zhong, adapted from QGISBuildingInputWidget by Stevan Gavrilovic

#include "QGISSiteInputWidget.h"
#include "QGISVisualizationWidget.h"
#include "ComponentTableView.h"
#include "ComponentDatabaseManager.h"
#include "AssetFilterDelegate.h"

#include <qgsfield.h>
#include <qgsfillsymbol.h>
#include <qgsvectorlayer.h>
#include <qgsmarkersymbol.h>

#include <QFileInfo>;

QGISSiteInputWidget::QGISSiteInputWidget(QWidget *parent, VisualizationWidget* visWidget, QString componentType, QString appType) : ComponentInputWidget(parent, visWidget, componentType, appType)
{
    theComponentDb = ComponentDatabaseManager::getInstance()->getSiteComponentDb();
}


int QGISSiteInputWidget::loadComponentVisualization()
{
    // initialization
    soilPropComplete = false;
    soilParamComplete = false;
    // check soil data
    this->checkSoilDataComplete();

    // Create the building attributes that are fixed
    QgsFields featFields;
    featFields.append(QgsField("ID", QVariant::Int));
    featFields.append(QgsField("AssetType", QVariant::String));
    featFields.append(QgsField("TabName", QVariant::String));

    // Set the table headers as fields in the table
    for(int i = 1; i<componentTableWidget->columnCount(); ++i)
    {
        auto fieldText = componentTableWidget->horizontalHeaderItemVariant(i);
        featFields.append(QgsField(fieldText.toString(),fieldText.type()));
    }

    QList<QgsField> attribFields;
    for(int i = 0; i<featFields.size(); ++i)
        attribFields.push_back(featFields[i]);

    auto headers = this->getTableHorizontalHeadings();

    // First check if a footprint was provided
    auto indexFootprint = headers.indexOf("Footprint");
    auto indexLatitude = headers.indexOf("Latitude");
    auto indexLongitude = headers.indexOf("Longitude");
    auto indexVs30 = headers.indexOf("Vs30");
    auto indexDepthToRock = headers.indexOf("DepthToRock");
    auto indexModelType = headers.indexOf("Model");

    if(indexLongitude == -1 || indexLatitude == -1)
    {
        this->errorMessage("Could not find latitude and longitude in the header columns");
        return -1;
    }

    // soil model is required for componentType = Soil
    if (componentType.compare("Soil")==0 && !indexModelType)
    {
        this->errorMessage("Could not find Model in the header columns");
        return -1;
    }

    // Get the number of rows
    auto nRows = componentTableWidget->rowCount();

    QString layerType;

    if(indexFootprint != -1)
        layerType = "polygon";
    else
        layerType = "point";

    // Create the buildings layer
    mainLayer = theVisualizationWidget->addVectorLayer(layerType,"All "+componentType+"s");

    if(mainLayer == nullptr)
    {
        this->errorMessage("Error adding a vector layer");
        return -1;
    }

    auto pr = mainLayer->dataProvider();

    mainLayer->startEditing();

    auto res = pr->addAttributes(attribFields);

    if(!res)
        this->errorMessage("Error adding attributes to the layer" + mainLayer->name());

    mainLayer->updateFields(); // tell the vector layer to fetch changes from the provider

    theComponentDb->setMainLayer(mainLayer);

    filterDelegateWidget  = new AssetFilterDelegate(mainLayer);

    auto numAtrb = attribFields.size();

    for(int i = 0; i<nRows; ++i)
    {
        // create the feature attributes
        QgsAttributes featureAttributes(numAtrb);

        // Create a new site
        QString siteIDStr = componentTableWidget->item(i,0).toString();

        int siteID = siteIDStr.toInt();

        // Create a unique ID for the building
//        auto uid = theVisualizationWidget->createUniqueID();

        //  "ID"
        //  "AssetType"
        //  "TabName"

        featureAttributes[0] = QVariant(siteID);
        featureAttributes[1] = QVariant("SITE");
        featureAttributes[2] = QVariant(siteID);

        // The feature attributes are the columns from the table
        for(int j = 1; j<componentTableWidget->columnCount(); ++j)
        {
            auto attrbVal = componentTableWidget->item(i,j);
            featureAttributes[2+j] = attrbVal;
        }

        auto latitude = componentTableWidget->item(i,indexLatitude).toDouble();
        auto longitude = componentTableWidget->item(i,indexLongitude).toDouble();

        // If a vs30 is given
        if (indexVs30 != -1)
        {
            auto vs30 = componentTableWidget->item(i,indexVs30).toDouble();
        }

        QgsFeature feature;
        feature.setFields(featFields);

        // If a footprint is given use that
        if(indexFootprint != -1)
        {
            QString footprint = componentTableWidget->item(i,indexFootprint).toString();

            if(footprint.compare("NA") == 0)
            {
                feature.setGeometry(QgsGeometry::fromPointXY(QgsPointXY(longitude,latitude)));
            }
            else
            {
                auto geom = theVisualizationWidget->getPolygonGeometryFromJson(footprint);
                if(geom.isEmpty())
                {
                    this->errorMessage("Error getting the building footprint geometry");
                    return -1;
                }

                feature.setGeometry(geom);
            }
        }
        else
        {
            QgsPointXY(longitude,latitude);
            auto geom = QgsGeometry::fromPointXY(QgsPointXY(longitude,latitude));
            if(geom.isEmpty())
            {
                this->errorMessage("Error getting the building geometry");
                return -1;
            }

            feature.setGeometry(geom);
        }

        feature.setAttributes(featureAttributes);

        if(!feature.isValid())
            return -1;


        auto res = pr->addFeature(feature, QgsFeatureSink::FastInsert);
        if(!res)
        {
            this->errorMessage("Error adding the feature to the layer");
            return -1;
        }

        //        auto id = feature.id();

        //        qDebug()<<id;
    }

    mainLayer->commitChanges(true);
    mainLayer->updateExtents();

    QgsSymbol* markerSymbol = nullptr;

    if(indexFootprint != -1)
        markerSymbol = new QgsFillSymbol();
    else
        markerSymbol = new QgsMarkerSymbol();

//    auto numFeat = mainLayer->featureCount();

    theVisualizationWidget->zoomToLayer(mainLayer);

    auto layerId = mainLayer->id();

    theVisualizationWidget->registerLayerForSelection(layerId,this);

    selectedFeaturesLayer = theVisualizationWidget->addVectorLayer(layerType,"Selected "+componentType+"s");

    if(selectedFeaturesLayer == nullptr)
    {
        this->errorMessage("Error adding the selected assets vector layer");
        return -1;
    }

    QgsSymbol* selectedLayerMarkerSymbol = nullptr;

    if(indexFootprint != -1)
        selectedLayerMarkerSymbol = new QgsFillSymbol();
    else
        selectedLayerMarkerSymbol = new QgsMarkerSymbol();

    selectedLayerMarkerSymbol->setColor(Qt::yellow);
    theVisualizationWidget->createSimpleRenderer(selectedLayerMarkerSymbol,selectedFeaturesLayer);

    auto pr2 = selectedFeaturesLayer->dataProvider();

    auto res2 = pr2->addAttributes(attribFields);

    if(!res2)
        this->errorMessage("Error adding attributes to the layer");

    selectedFeaturesLayer->updateFields(); // tell the vector layer to fetch changes from the provider

    theComponentDb->setSelectedLayer(selectedFeaturesLayer);

    QVector<QgsMapLayer*> mapLayers;
    mapLayers.push_back(selectedFeaturesLayer);
    mapLayers.push_back(mainLayer);

    theVisualizationWidget->createLayerGroup(mapLayers,componentType);

    // check soil properties' completeness
    if (componentType.compare("Soil")==0)
        this->checkSoilPropComplete();

    // check soil modeling parameter completeness
    if (componentType.compare("Soil")==0)
    {
        if (this->checkSoilParamComplete()==2)
        {
            this->errorMessage("Model type error: currently, BA and EI are valid.");
            return -1;
        }
    }

    return 0;
}


void QGISSiteInputWidget::reloadComponentData(QString newDataFile)
{
    QFileInfo newFilename(newDataFile);
    if (newFilename.exists() && newFilename.isFile()) {
        this->statusMessage("Loading the new site data file...");
        pathToComponentInputFile = newDataFile;
        componentFileLineEdit->setText(pathToComponentInputFile);
        this->loadComponentData();
        this->statusMessage("New site data loaded.");
        this->applyFilterString(this->getFilterString());
    }
    else
    {
        this->errorMessage("Cannot reload the site data file "+newDataFile);
    }
    // check soil data
    this->checkSoilDataComplete();
}


void QGISSiteInputWidget::checkSoilPropComplete()
{
    // get headers
    auto headers = this->getTableHorizontalHeadings();
    // vs30 index
    auto indexV = headers.indexOf("Vs30");
    // bedrock depth index
    auto indexD = headers.indexOf("DepthToRock");
    // vs30 or depth to beckrock column missing
    if (indexV == -1 || indexD == -1)
    {
        soilPropComplete = false;
        return;
    }
    // get row number
    auto nRows = componentTableWidget->rowCount();
    // loop all rows
    for(int i = 0; i<nRows; ++i)
    {
        // vs30
        if (componentTableWidget->item(i,indexV).toString().isEmpty())
        {
            this->statusMessage("Attribute Vs30 is missing at Row "+QString::number(i)+" - please fetch site data.");
            soilPropComplete = false;
            return;
        }
        // depth to rock
        if (componentTableWidget->item(i,indexD).toString().isEmpty())
        {
            this->statusMessage("Attribute DepthToRock is missing at Row "+QString::number(i)+" - please fetch site data.");
            soilPropComplete = false;
            return;
        }
    }
    // all checks passed
    this->statusMessage("Soil properties are complete.");
    soilPropComplete = true;
}


int QGISSiteInputWidget::checkSoilParamComplete()
{
    // get row number
    auto nRows = componentTableWidget->rowCount();
    // get headers
    auto headers = this->getTableHorizontalHeadings();
    // soil model index
    auto indexSoilModel = headers.indexOf("Model");
    // loop all rows
    for(int i = 0; i<nRows; ++i)
    {
        QStringList checkList;
        auto soilModelType = componentTableWidget->item(i,indexSoilModel).toString();
        if (soilModelType.compare("EI")==0)
            checkList = attrbFullSoilEI;
        else if (soilModelType.compare("BA")==0)
            checkList = attrbFullSoilBA;
        else
        {
            this->errorMessage("The soil model type "+soilModelType+" is not supported yet...");
            {
                soilParamComplete = false;
                return 2;
            }
        }
        // loop over the checkList
        for (int j = 0; j<checkList.size(); j++)
        {
            if (i == 0)
            {
                // first check if the header(s) is(are) avaliable
                if (!headers.indexOf(checkList[j]))
                {
                    this->statusMessage("Attribute "+checkList[j]+" is missing - please fetch site data.");
                    soilParamComplete = false;
                    return 1;
                }
            }
            else
            {
                // check if the data is provided for current row
                if (componentTableWidget->item(i,headers.indexOf(checkList[j])).toString().isEmpty())
                {
                    this->statusMessage("Attribute "+checkList[j]+" is missing at Row "+QString::number(j)+" - please fetch site data.");
                    soilParamComplete = false;
                    return 1;
                }
            }
        }
    }
    // all checks passed
    this->statusMessage("Soil modeling parameters are complete.");
    soilParamComplete = true;
    return 0;
}


void QGISSiteInputWidget::checkSoilDataComplete(void)
{
    if (soilParamComplete && soilPropComplete)
    {
        this->statusMessage("Soil response data is complete.");
        emit soilDataCompleteSignal(true);
    }
    else
    {
        this->statusMessage("Soil response data is incomplete.");
        emit soilDataCompleteSignal(false);
    }
}


void QGISSiteInputWidget::clear()
{
    if(selectedFeaturesLayer != nullptr)
    {
        theVisualizationWidget->removeLayer(selectedFeaturesLayer);
        theVisualizationWidget->deregisterLayerForSelection(selectedFeaturesLayer->id());
    }
    if(mainLayer != nullptr)
        theVisualizationWidget->removeLayer(mainLayer);

    ComponentInputWidget::clear();
}



