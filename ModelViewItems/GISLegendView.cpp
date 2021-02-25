#include "GISLegendView.h"

#include <QHeaderView>


GISLegendView::GISLegendView(QWidget *parent) : QTreeView(parent)
{
    currModel = nullptr;

    //    this->setSizeAdjustPolicy(SizeAdjustPolicy::AdjustToContents);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
    this->hide();
    this->setEditTriggers(EditTrigger::NoEditTriggers);
    this->setSelectionMode(SelectionMode::NoSelection);


    this->setStyleSheet("border: 1px solid transparent;"
                        "border-radius: 10px;"
                        "background:  rgba(255, 255, 255, 150);");

    this->setObjectName("legendView");
    this->header()->setObjectName("legendView");
}


QSize GISLegendView::sizeHint() const
{
    if(model() == nullptr)
        return QSize();

    if (model()->rowCount() == 0)
        return QSize(sizeHintForColumn(0)*1.2, 0);

    int nToShow = model()->rowCount();

    auto widthLegend = sizeHintForColumn(0)*1.2;
    auto heightLegend = nToShow*sizeHintForRow(0)*1.5;


    return QSize(widthLegend,heightLegend);
}


void GISLegendView::setModel(QAbstractItemModel* model)
{
    if(currModel == model)
        return;

    QTreeView::setModel(model);

}


void GISLegendView::clear(void)
{
    currModel = nullptr;

    this->hide();
}


QAbstractItemModel *GISLegendView::getModel() const
{
    return currModel;
}
