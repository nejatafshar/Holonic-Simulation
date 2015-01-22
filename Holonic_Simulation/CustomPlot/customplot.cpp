
#include "customplot.h"


CustomPlot::CustomPlot(QWidget *parent) :
  QCustomPlot(parent)
{


   this->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes |
                                   QCP::iSelectLegend | QCP::iSelectPlottables | QCP::iSelectOther);
   this->axisRect()->setRangeDrag(Qt::Horizontal|Qt::Vertical);
   this->axisRect()->setRangeZoom(Qt::Horizontal|Qt::Vertical);

   this->xAxis->setRange(-8, 8);
   this->yAxis->setRange(-5, 5);
   this->axisRect()->setupFullAxesBox();
//   this->plotLayout()->insertRow(0);
//   this->plotLayout()->addElement(0,0,new QCPPlotTitle(this,""));
   this->xAxis->setLabel("x Axis");
   this->yAxis->setLabel("y Axis");
   this->legend->setVisible(true);
   QFont legendFont = font();
   legendFont.setPointSize(10);
   this->legend->setFont(legendFont);
   this->legend->setSelectedFont(legendFont);
   this->legend->setSelectableParts(QCPLegend::spItems); // legend box shall not be selectable, only legend items
   //setNoAntialiasingOnDrag(true);


   // connect slot that ties some axis selections together (especially opposite axes):
   connect(this, SIGNAL(selectionChangedByUser()), this, SLOT(selectionChanged()));

   // connect slots that takes care that when an axis is selected, only that direction can be dragged and zoomed:
   connect(this, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(OnMousePress(QMouseEvent*)));
   connect(this, SIGNAL(mouseWheel(QWheelEvent*)), this, SLOT(mouseWheel()));
   connect(this, SIGNAL(mouseMove(QMouseEvent*)), this,SLOT(OnMouseMove(QMouseEvent*)));
   connect(this, SIGNAL(mouseDoubleClick(QMouseEvent*)), this,SLOT(onDoubleClicked(QMouseEvent*)));
   connect(this, SIGNAL(mouseRelease(QMouseEvent*)), this, SLOT(OnMouseRelease(QMouseEvent*)));

   // make bottom and left axes transfer their ranges to top and right axes:
   connect(this->xAxis, SIGNAL(rangeChanged(QCPRange)), this->xAxis2, SLOT(setRange(QCPRange)));
   connect(this->yAxis, SIGNAL(rangeChanged(QCPRange)), this->yAxis2, SLOT(setRange(QCPRange)));

   thePlotLayout = (QGridLayout *)this->parentWidget()->layout();

   layout = new QGridLayout(this);
   layout->setAlignment(Qt::AlignRight | Qt::AlignTop);

   fullscreenBut = new QPushButton(this);
   fullscreenBut->setFixedWidth(25);
   fullscreenBut->setFixedHeight(25);
   fullscreenBut->setToolTip(tr("Show Full Screen"));
   fullscreenBut->setIcon(QIcon(":/plotZoom"));
   fullscreenBut->setIconSize(QSize(20,20));

   changeBrushBut = new QPushButton(this);
   changeBrushBut->setFixedWidth(25);
   changeBrushBut->setFixedHeight(25);
   changeBrushBut->setToolTip(tr("Change Brush"));
   changeBrushBut->setIcon(QIcon(":/plotBrush"));
   changeBrushBut->setIconSize(QSize(20,20));

   changeLineStyleBut = new QPushButton(this);
   changeLineStyleBut->setFixedWidth(25);
   changeLineStyleBut->setFixedHeight(25);
   changeLineStyleBut->setToolTip(tr("Change Line Style"));
   changeLineStyleBut->setIcon(QIcon(":/plotLineStyle"));
   changeLineStyleBut->setIconSize(QSize(20,20));

   changePointStyleBut = new QPushButton(this);
   changePointStyleBut->setFixedWidth(25);
   changePointStyleBut->setFixedHeight(25);
   changePointStyleBut->setToolTip(tr("Change Point Style"));
   changePointStyleBut->setIcon(QIcon(":/plotPointStyle"));
   changePointStyleBut->setIconSize(QSize(20,20));


   rescaleBut = new QPushButton(this);
   rescaleBut->setFixedWidth(25);
   rescaleBut->setFixedHeight(25);
   rescaleBut->setToolTip(tr("Rescale"));
   rescaleBut->setIcon(QIcon(":/plotRescale"));
   rescaleBut->setIconSize(QSize(20,20));

   legendChkBx = new QCheckBox(this);
   legendChkBx->setText("Legend");
   legendChkBx->setChecked(true);
   legendChkBx->setFixedWidth(70);
   legendChkBx->setFixedHeight(25);


   zoomModeToolButton = new QToolButton(this);
   zoomModeToolButton->setPopupMode(QToolButton::InstantPopup);
   zoomModeToolButton->setFixedWidth(30);
   zoomModeToolButton->setFixedHeight(30);
   zoomModeToolButton->setToolTip(tr("Change zoom mode"));
   zoomModeToolButton->setIcon(QIcon(":/scrollZoom"));
   zoomModeToolButton->setIconSize(QSize(20,20));

   QActionGroup * actionGroup = new QActionGroup(this);
   actionGroup->setExclusive(true);

   QAction * scrollZoomAction = new QAction(QIcon(":/scrollZoom"),"Scroll Zoom", actionGroup);
   scrollZoomAction->setCheckable(true);
   QAction * rectangleZoomAction = new QAction(QIcon(":/rectangleZoom"),"Rectangle Zoom", actionGroup);
   rectangleZoomAction->setCheckable(true);

   zoomModeToolButton->addAction(scrollZoomAction);
   zoomModeToolButton->addAction(rectangleZoomAction);

   connect(scrollZoomAction,SIGNAL(triggered()),this,SLOT(setScrollZoomMode()));
   connect(rectangleZoomAction,SIGNAL(triggered()),this,SLOT(setRectangleZoomMode()));


   layout->addWidget(fullscreenBut,0,1);
   layout->addWidget(changeBrushBut,1,1);
   layout->addWidget(changeLineStyleBut,2,1);
   layout->addWidget(changePointStyleBut,3,1);
   layout->addWidget(rescaleBut,4,1);
   layout->addWidget(legendChkBx,0,0);
   layout->addWidget(legendChkBx,0,0);

   connect(fullscreenBut,SIGNAL(clicked()),this,SLOT(fullscreen()));
   connect(changeBrushBut,SIGNAL(clicked()),this,SLOT(changeBrushColor()));
   connect(changeBrushBut,SIGNAL(clicked()),this,SLOT(changePenColor()));
   connect(changeLineStyleBut,SIGNAL(clicked()),this,SLOT(changeLineStyle()));
   connect(changePointStyleBut,SIGNAL(clicked()),this,SLOT(changePoint()));
   connect(rescaleBut,SIGNAL(clicked()),this,SLOT(rescalePlot()));
   connect(legendChkBx,SIGNAL(toggled(bool)),this,SLOT(setLegendVisible(bool)));


   setHasLegendCheckBox(false);


   fullscreenDialog = new QDialog(this->parentWidget());
   fullscreenLayout = new QGridLayout(fullscreenDialog);

   isFullScreen = false;

   hasBrush = true;

   hasPointTooltip = true;

   firstTime=true;
   autoRescale=true;
   holdMaxScale=false;
   holdMaxScaleFactor=0.1;
   holdMinScale=false;
   holdMinScaleFactor=0.1;
   rescaleYWithMargin=false;
   YmarginFactor=0.2;
   rescaleXWithMargin=false;
   XmarginFactor=0.1;
   minIndicateNum=0;
   maxIndicateNum=0;
   indicateMode=0;
   dataNum.clear();
   hasFixedBottom=false;
   hasFixedTop=false;
   fixedBottom=0;
   fixedTop=0;

   showAllCoordinatesInTooltip =false;

   rectangleZoom = false;
   mRubberBand = new QRubberBand(QRubberBand::Rectangle, this);

   connect(this->xAxis, SIGNAL(rangeChanged(QCPRange)),this,SLOT(updateRange(QCPRange)));


   srand(QDateTime::currentDateTime().toTime_t());


}

CustomPlot::~CustomPlot()
{
}

void CustomPlot::selectionChanged()
{
    /*
     normally, axis base line, axis tick labels and axis labels are selectable separately, but we want
     the user only to be able to select the axis as a whole, so we tie the selected states of the tick labels
     and the axis base line together. However, the axis label shall be selectable individually.

     The selection state of the left and right axes shall be synchronized as well as the state of the
     bottom and top axes.

     Further, we want to synchronize the selection of the graphs with the selection state of the respective
     legend item belonging to that graph. So the user can select a graph by either clicking on the graph itself
     or on its legend item.
    */

    // make top and bottom axes be selected synchronously, and handle axis and tick labels as one selectable object:
    if (this->xAxis->selectedParts().testFlag(QCPAxis::spAxis) || this->xAxis->selectedParts().testFlag(QCPAxis::spTickLabels) ||
        this->xAxis2->selectedParts().testFlag(QCPAxis::spAxis) || this->xAxis2->selectedParts().testFlag(QCPAxis::spTickLabels))
    {
      this->xAxis2->setSelectableParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
      this->xAxis->setSelectableParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
    }
    // make left and right axes be selected synchronously, and handle axis and tick labels as one selectable object:
    if (this->yAxis->selectedParts().testFlag(QCPAxis::spAxis) || this->yAxis->selectedParts().testFlag(QCPAxis::spTickLabels) ||
        this->yAxis2->selectedParts().testFlag(QCPAxis::spAxis) || this->yAxis2->selectedParts().testFlag(QCPAxis::spTickLabels))
    {
      this->yAxis2->setSelectableParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
      this->yAxis->setSelectableParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
    }

    // synchronize selection of graphs with selection of corresponding legend items:
    for (int i=0; i<this->graphCount(); ++i)
    {
      QCPGraph *graph = this->graph(i);
      QCPPlottableLegendItem *item = this->legend->itemWithPlottable(graph);
      if (item->selected() || graph->selected())
      {
        item->setSelected(true);
        graph->setSelected(true);
      }
    }

}

void CustomPlot::OnMousePress(QMouseEvent* event)
{
    // if an axis is selected, only allow the direction of that axis to be dragged
    // if no axis is selected, both directions may be dragged

    if(rectangleZoom)
    {
        if (event->button() == Qt::LeftButton)
        {
            mOrigin = event->pos();
            mRubberBand->setGeometry(QRect(mOrigin, QSize()));
            mRubberBand->show();
        }
    }
    else
    {
        if (this->xAxis->selectedParts().testFlag(QCPAxis::spAxis) || this->xAxis->selectedParts().testFlag(QCPAxis::spTickLabels))
            this->axisRect()->setRangeDrag(this->xAxis->orientation());
        else if (this->yAxis->selectedParts().testFlag(QCPAxis::spAxis) || this->yAxis->selectedParts().testFlag(QCPAxis::spTickLabels))
            this->axisRect()->setRangeDrag(this->yAxis->orientation());
        else if (this->xAxis2->selectedParts().testFlag(QCPAxis::spAxis) || this->xAxis2->selectedParts().testFlag(QCPAxis::spTickLabels))
            this->axisRect()->setRangeDrag(this->xAxis->orientation());
        else if (this->yAxis2->selectedParts().testFlag(QCPAxis::spAxis) || this->yAxis2->selectedParts().testFlag(QCPAxis::spTickLabels))
            this->axisRect()->setRangeDrag(this->yAxis->orientation());
        else
            this->axisRect()->setRangeDrag(Qt::Horizontal|Qt::Vertical);

        emit scaleChangedManually();
    }
}
void CustomPlot::OnMouseRelease(QMouseEvent*)
{
    if (mRubberBand->isVisible())
    {
        const QRect & zoomRect = mRubberBand->geometry();
        int xp1, yp1, xp2, yp2;
        zoomRect.getCoords(&xp1, &yp1, &xp2, &yp2);
        double x1 = xAxis->pixelToCoord(xp1);
        double x2 = xAxis->pixelToCoord(xp2);
        double y1 = yAxis->pixelToCoord(yp1);
        double y2 = yAxis->pixelToCoord(yp2);

        xAxis->setRange(x1, x2);
        yAxis->setRange(y1, y2);

        mRubberBand->hide();
        replot();

        emit scaleChangedManually();
    }
}

void CustomPlot::OnMouseMove(QMouseEvent *event)
{
    if(mRubberBand->isVisible())
    {
        mRubberBand->setGeometry(QRect(mOrigin, event->pos()).normalized());
    }

    showPointToolTip(event);
}

void CustomPlot::mouseWheel()
{
    // if an axis is selected, only allow the direction of that axis to be zoomed
    // if no axis is selected, both directions may be zoomed

    if(!rectangleZoom &&  this->axisRect()->rangeZoom()!=0)
    {
        if (this->xAxis->selectedParts().testFlag(QCPAxis::spAxis) || this->xAxis->selectedParts().testFlag(QCPAxis::spTickLabels) || this->xAxis->selectedParts().testFlag(QCPAxis::spAxisLabel))
            this->axisRect()->setRangeZoom(this->xAxis->orientation());
        else if (this->yAxis->selectedParts().testFlag(QCPAxis::spAxis) || this->yAxis->selectedParts().testFlag(QCPAxis::spTickLabels) || this->yAxis->selectedParts().testFlag(QCPAxis::spAxisLabel))
            this->axisRect()->setRangeZoom(this->yAxis->orientation());
        else if (this->xAxis2->selectedParts().testFlag(QCPAxis::spAxis) || this->xAxis2->selectedParts().testFlag(QCPAxis::spTickLabels) || this->xAxis2->selectedParts().testFlag(QCPAxis::spAxisLabel))
            this->axisRect()->setRangeZoom(this->xAxis->orientation());
        else if (this->yAxis2->selectedParts().testFlag(QCPAxis::spAxis) || this->yAxis2->selectedParts().testFlag(QCPAxis::spTickLabels) || this->yAxis2->selectedParts().testFlag(QCPAxis::spAxisLabel))
            this->axisRect()->setRangeZoom(this->yAxis->orientation());
        else
            this->axisRect()->setRangeZoom(Qt::Horizontal|Qt::Vertical);

        emit scaleChangedManually();
    }
}

void CustomPlot::removeSelectedGraph()
{
    if (this->selectedGraphs().size() > 0)
    {
      this->removeGraph(this->selectedGraphs().first());
      this->replot();
    }
}

void CustomPlot::showPointToolTip(QMouseEvent *event)
{

    if(!showAllCoordinatesInTooltip)
    {

        QCPItemTracer * item=(QCPItemTracer *) itemAt(QPointF(event->pos().x(),event->pos().y()));


        if(!qobject_cast<QCPItemTracer *>(item))
        {
            setToolTip("");
            return;
        }

        if (item)
        {
            setToolTip(QString("%1 , %2").arg(item->graphKey()).arg(item->graph()->data()->value(item->graphKey()).value));
        }
        else
            setToolTip("");

    }
    else
    {

        int x = this->xAxis->pixelToCoord(event->pos().x());
        int y = this->yAxis->pixelToCoord(event->pos().y());

        setToolTip(QString("%1 , %2").arg(x).arg(y));
    }
}

void CustomPlot::onDoubleClicked(QMouseEvent *event)
{

    QCPItemTracer * item=(QCPItemTracer *) itemAt(QPointF(event->pos().x(),event->pos().y()));

    if(!qobject_cast<QCPItemTracer *>(item))
    {
        return;
    }

    if (item)
    {
        emit pointDoubleClicked(item->graphKey(), item->graph()->data()->value(item->graphKey()).value);
    }
    else
        ;
}


class sort_data
{
public:
  int val;
  int index;
};

bool MaxSortPredicate(const sort_data& d1, const sort_data& d2)
{
    return d1.val > d2.val;
}
bool MinSortPredicate(const sort_data& d1, const sort_data& d2)
{
    return d1.val < d2.val;
}

QVector<int> CustomPlot::getnMaxIndexes(QVector<double> y, int n)
{
    std::vector<sort_data> sortedData;

    for(int i=0;i<y.count();i++)
    {
        sort_data sdata;
        sdata.val=y[i];
        sdata.index=i;
        sortedData.push_back(sdata);
    }
    std::sort(sortedData.begin(),sortedData.end(),MaxSortPredicate);

    QVector<int> indexes;

    for(int i=0;i<n;i++)
        indexes.append(sortedData[i].index);

    return indexes;
}

QVector<int> CustomPlot::getnMinIndexes(QVector<double> y, int n)
{
    std::vector<sort_data> sortedData;

    for(int i=0;i<y.count();i++)
    {
        sort_data sdata;
        sdata.val=y[i];
        sdata.index=i;
        sortedData.push_back(sdata);
    }
    std::sort(sortedData.begin(),sortedData.end(),MinSortPredicate);

    QVector<int> indexes;

    for(int i=0;i<n;i++)
        indexes.append(sortedData[i].index);

    return indexes;
}

void CustomPlot::addGraph(QString name, double *xdata, double *ydata, int num)
{
    xData.append(QVector<double>());
    yData.append(QVector<double>());
    dataNum.append(num);

    itemTracers.append(QVector<QCPItemTracer *>());
    itemTexts.append(QVector<QCPItemText *>());
    itemLines.append(QVector<QCPItemLine *>());


    for(int i=0;i<num;i++)
    {
        this->xData[dataNum.count()-1].push_back(xdata[i]);
        this->yData[dataNum.count()-1].push_back(ydata[i]);
    }



    QCustomPlot::addGraph();
    this->graph(dataNum.count()-1)->setName(name);
    this->graph(dataNum.count()-1)->setLineStyle((QCPGraph::LineStyle)(1));

    QPen graphPen;
    graphPen.setColor(QColor(rand()%245+10, rand()%245+10, rand()%245+10));
    graphPen.setWidthF(rand()/(double)RAND_MAX*2+1);
    this->graph(dataNum.count()-1)->setPen(graphPen);



    updateRange(this->xAxis->range());
}

void CustomPlot::updateGraph(int graph_index, double *xdata, double *ydata, int num)
{
    if(autoRescale)
        firstTime=true;

    xData[graph_index].clear();
    yData[graph_index].clear();
    for(int i=0;i<num;i++)
    {
        this->xData[graph_index].push_back(xdata[i]);
        this->yData[graph_index].push_back(ydata[i]);
    }
    this->dataNum[graph_index]=num;


    //updateRange(this->xAxis->range());

    //this->graph(graph_index)->setData(xData[graph_index], yData[graph_index]);
    updateGraphInRange(graph_index, this->xAxis->range());

    this->replot();
}

void CustomPlot::updateGraphData(int graph_index, double *xdata, double *ydata, int num)
{
    if(autoRescale)
        firstTime=true;

    xData[graph_index].clear();
    yData[graph_index].clear();
    for(int i=0;i<num;i++)
    {
        this->xData[graph_index].push_back(xdata[i]);
        this->yData[graph_index].push_back(ydata[i]);
    }
    this->dataNum[graph_index]=num;

    //this->graph(graph_index)->setData(xData[graph_index], yData[graph_index]);
    updateGraphInRange(graph_index, this->xAxis->range());

}

void CustomPlot::updateRange(QCPRange range)
{
    for(int i=0;i<dataNum.count();i++)
        updateGraphInRange(i, range);

    this->replot();

}

void CustomPlot::updateGraphInRange(int graph_index, QCPRange range)
{
    if (dataNum[graph_index]==0)
        return;


// code to reduce large amount of data
    double lower;
    double upper;

    if(!firstTime)
    {
        lower= std::max(range.lower, xData[graph_index][0]);
        upper= std::min(range.upper, xData[graph_index][dataNum[graph_index]-1]);
    }
    else
    {
        lower= xData[graph_index][0];
        upper= xData[graph_index][dataNum[graph_index]-1];
    }

    if(lower>=upper)
        return;

    int indLow=0;
    while(xData[graph_index][indLow]<lower)
        indLow++;

    int indHigh=indLow;
    while(xData[graph_index][indHigh]<upper)
        indHigh++;

    int rangeNum=indHigh-indLow+1;



    if(rangeNum<1)
        return;


    double inc=((double)rangeNum)/(double)1000;

    int curNum=2000;
    if(inc<2)
    {
        inc=2;
        curNum=rangeNum;
    }


    int intervalPoints[1001];

    double ind=indLow;
    for(int i=0;i<1001;i++)
    {
        intervalPoints[i]=ind;

        ind+= inc;
    }


    QVector<double> x, y;


    double index=indLow;

    if(inc==2)
    {
        for(int i=0;i<curNum;i++)
        {
            x.append(xData[graph_index][index]);
            y.append(yData[graph_index][index]);

            index++;
        }
    }
    else
    {
        for(int i=0;i<1000;i++)
        {
            double maxVal=INT_MIN;
            int maxIndex=intervalPoints[i];
            double minVal=INT_MAX;
            int minIndex=intervalPoints[i];

            for(int j=intervalPoints[i];j<=intervalPoints[i+1];j++)
            {
                if(j>=dataNum[graph_index])
                    break;

                if(yData[graph_index][j]>  maxVal)
                {
                    maxIndex=j;
                    maxVal=yData[graph_index][j];
                }
                else if(yData[graph_index][j]<minVal)
                {
                    minIndex=j;
                    minVal=yData[graph_index][j];
                }
            }

            if(minIndex>maxIndex)
            {
                x.append(xData[graph_index][maxIndex]);
                y.append(yData[graph_index][maxIndex]);

                x.append(xData[graph_index][minIndex]);
                y.append(yData[graph_index][minIndex]);
            }
            else
            {
                x.append(xData[graph_index][minIndex]);
                y.append(yData[graph_index][minIndex]);

                x.append(xData[graph_index][maxIndex]);
                y.append(yData[graph_index][maxIndex]);
            }
        }

    }
    this->graph(graph_index)->setData(x, y);

    if(hasPointTooltip)
    {

        for (int i=0; i<qMin(itemTracers[graph_index].count(),curNum); i++)
        {
            itemTracers[graph_index][i]->setGraphKey(x[i]);
        }


        for (int i=qMin(itemTracers[graph_index].count(),curNum); i<curNum; i++)
        {
            QCPItemTracer *itemTracer = new QCPItemTracer(this);
            itemTracers[graph_index].push_back(itemTracer);
            this->addItem(itemTracer);
            itemTracer->setGraph(this->graph(graph_index));
            itemTracer->setGraphKey(x[i]);
            itemTracer->setInterpolating(true);
            itemTracer->setStyle(QCPItemTracer::tsCircle);
            itemTracer->setPen(QPen(Qt::transparent));
        }
    }

    if(firstTime)
    {
        if(rescaleYWithMargin || maxIndicateNum>0 || minIndicateNum>0 || holdMaxScale || holdMinScale)
        {
            double maxVal=INT_MIN;
            int maxIndex=0;
            double minVal=INT_MAX;
            int minIndex=0;
            for(int i=0;i<x.count();i++)
            {
                if(y[i]>  maxVal)
                {
                    maxIndex=i;
                    maxVal=y[i];
                }
                else if(y[i]<minVal)
                {
                    minIndex=i;
                    minVal=y[i];
                }
            }


            double margin= ( maxVal!=minVal ? ((maxVal-minVal)*YmarginFactor) : abs(maxVal*YmarginFactor) ) ;

            if(margin==0)
                margin=1;


            double newUpper =0;
            bool hasNewUpper=false;
            int newLower = 0;
            bool hasNewLower=false;

            if(holdMaxScale)
            {
                if( (maxVal+margin)>this->yAxis->range().upper || (maxVal+margin)<(this->yAxis->range().lower+holdMaxScaleFactor*(this->yAxis->range().upper-this->yAxis->range().lower)) )
                {
                    newUpper = maxVal+margin;
                    hasNewUpper = true;
                }

            }
            else
            {
                newUpper = maxVal+margin;
                hasNewUpper = true;
            }
            if(holdMinScale)
            {

                if( (minVal-margin)<this->yAxis->range().lower || (minVal-margin)>(this->yAxis->range().upper-holdMinScaleFactor*(this->yAxis->range().upper-this->yAxis->range().lower)) )
                {
                    newLower = minVal-margin;
                    hasNewLower = true;
                }

            }
            else
            {
                newLower = minVal-margin;
                hasNewLower = true;
            }


            if(newUpper==newLower)
                newUpper+=1;

            if(hasFixedTop)
                this->yAxis->setRangeUpper(fixedTop);
            else if(hasNewUpper)
                this->yAxis->setRangeUpper(newUpper);

            if(hasFixedBottom)
                this->yAxis->setRangeLower(fixedBottom);
            else if(hasNewLower)
                this->yAxis->setRangeLower(newLower);



            for(int i=0;i<itemTexts[graph_index].count();i++)
            {
                this->removeItem(itemTexts[graph_index][i]);
            }
            itemTexts[graph_index].clear();

            for(int i=0;i<itemLines[graph_index].count();i++)
            {
                this->removeItem(itemLines[graph_index][i]);
            }
            itemLines[graph_index].clear();

            if(maxIndicateNum>0)
            {
                QVector<int> indexes = getnMaxIndexes(y, maxIndicateNum);

                for(int i=0;i<indexes.count();i++)
                {
                    QCPItemText *textLabel = new QCPItemText(this);
                    itemTexts[graph_index].push_back(textLabel);
                    textLabel->setColor(this->xAxis->tickLabelColor());
                    this->addItem(textLabel);
                    textLabel->setPositionAlignment(Qt::AlignTop|Qt::AlignCenter);
                    //textLabel->position->setType(QCPItemPosition::ptAxisRectRatio);
                    textLabel->position->setCoords(x[indexes[i]],  y[indexes[i]]+((maxVal-minVal)*YmarginFactor*0.8)); // place position at center/top of axis rect

                    if(indicateMode == 0)
                        textLabel->setText(QString("%1 , %2").arg(x[indexes[i]]).arg(y[indexes[i]]));
                    else if(indicateMode == 1)
                        textLabel->setText(QString("%1").arg(x[indexes[i]]));
                    else if(indicateMode == 2)
                        textLabel->setText(QString("%1").arg(y[indexes[i]]));

                    //textLabel->setFont(QFont(font().family(), 16)); // make font a bit larger

                    // add the arrow:
                    QCPItemLine *arrow = new QCPItemLine(this);
                    itemLines[graph_index].push_back(arrow);
                    QPen  pen(this->xAxis->tickLabelColor());
                    arrow->setPen(pen);
                    this->addItem(arrow);
                    arrow->start->setParentAnchor(textLabel->bottom);
                    arrow->end->setCoords(x[indexes[i]], y[indexes[i]]); // point to (4, 1.6) in x-y-plot coordinates
                    arrow->setHead(QCPLineEnding::esSpikeArrow);
                }
            }

            if(minIndicateNum>0)
            {
                QVector<int> indexes = getnMinIndexes(y, minIndicateNum);

                for(int i=0;i<indexes.count();i++)
                {
                    QCPItemText *textLabel = new QCPItemText(this);
                    itemTexts[graph_index].push_back(textLabel);
                    textLabel->setColor(this->xAxis->tickLabelColor());
                    this->addItem(textLabel);
                    textLabel->setPositionAlignment(Qt::AlignTop|Qt::AlignCenter);
                    //textLabel->position->setType(QCPItemPosition::ptAxisRectRatio);
                    textLabel->position->setCoords(x[indexes[i]],  y[indexes[i]]-((maxVal-minVal)*YmarginFactor*0.8)); // place position at center/top of axis rect
                    textLabel->setText(QString("%1 , %2").arg(x[indexes[i]]).arg(y[indexes[i]]));
                    //textLabel->setFont(QFont(font().family(), 16)); // make font a bit larger

                    // add the arrow:
                    QCPItemLine *arrow = new QCPItemLine(this);
                    itemLines[graph_index].push_back(arrow);
                    QPen  pen(this->xAxis->tickLabelColor());
                    arrow->setPen(pen);
                    this->addItem(arrow);
                    arrow->start->setParentAnchor(textLabel->top);
                    arrow->end->setCoords(x[indexes[i]], y[indexes[i]]); // point to (4, 1.6) in x-y-plot coordinates
                    arrow->setHead(QCPLineEnding::esSpikeArrow);
                }
            }



        }
        else
            this->graph(graph_index)->rescaleValueAxis();

        if(rescaleXWithMargin)
        {
            double margin=((xData[graph_index][xData[graph_index].count()-1]-xData[graph_index][0])*XmarginFactor);
            this->xAxis->setRange(xData[graph_index][0]-margin, xData[graph_index][xData[graph_index].count()-1]+margin);
        }
        else
        {
             this->xAxis->setRange(xData[graph_index][0], xData[graph_index][xData[graph_index].count()-1]);
        }



        firstTime=false;
    }
}

void CustomPlot::clearGraph(int graph_index)
{
    xData[graph_index].clear();
    yData[graph_index].clear();
    this->dataNum[graph_index]=0;
    //this->graph(graph_index)->setData(QVector<double>(), QVector<double>());
    this->graph(graph_index)->clearData();
    this->clearItems();
    itemTracers[graph_index].clear();

    this->replot();
}

void CustomPlot::updateGraphAllData(int graph_index, double *xdata, double *ydata, int num)
{
    xData[graph_index].clear();
    yData[graph_index].clear();
    for(int i=0;i<num;i++)
    {
        this->xData[graph_index].push_back(xdata[i]);
        this->yData[graph_index].push_back(ydata[i]);
    }
    this->dataNum[graph_index]=num;

    if(firstTime && num>0)
        this->xAxis->setRange(xData[graph_index][0],xData[graph_index][xData[graph_index].count()-1]);

    this->graph(graph_index)->setData(xData[graph_index], yData[graph_index]);


    for(int i=0;i<itemTracers[graph_index].count();i++)
    {
        this->removeItem(itemTracers[graph_index][i]);
    }
    itemTracers[graph_index].clear();

    for (int i=0; i<num; i++)
    {
        QCPItemTracer *itemTracer = new QCPItemTracer(this);
        itemTracers[graph_index].push_back(itemTracer);
        this->addItem(itemTracer);
        itemTracer->setGraph(this->graph(graph_index));
        itemTracer->setGraphKey(xData[graph_index][i]);
        itemTracer->setInterpolating(true);
        itemTracer->setStyle(QCPItemTracer::tsCircle);
        itemTracer->setPen(QPen(Qt::transparent));

    }

    if(firstTime)
    {
        this->graph(graph_index)->rescaleValueAxis();
        firstTime=false;
    }

    this->replot();

}

void CustomPlot::addRandomGraph()
{
    int n = 100; // number of points in graph
    double xScale = (rand()/(double)RAND_MAX + 0.5)*2;
    double yScale = (rand()/(double)RAND_MAX + 0.5)*2;
    double xOffset = (rand()/(double)RAND_MAX - 0.5)*4;
    double yOffset = (rand()/(double)RAND_MAX - 0.5)*5;
    double r1 = (rand()/(double)RAND_MAX - 0.5)*2;
    double r2 = (rand()/(double)RAND_MAX - 0.5)*2;
    double r3 = (rand()/(double)RAND_MAX - 0.5)*2;
    double r4 = (rand()/(double)RAND_MAX - 0.5)*2;
    QVector<double> x(n), y(n);
    for (int i=0; i<n; i++)
    {
      x[i] = (i/(double)n-0.5)*10.0*xScale + xOffset;
      y[i] = (sin(x[i]*r1*5)*sin(cos(x[i]*r2)*r4*3)+r3*cos(sin(x[i])*r4*2))*yScale + yOffset;
    }

    QCustomPlot::addGraph();
    this->graph()->setName(QString("New graph %1").arg(this->graphCount()-1));
    this->graph()->setData(x, y);
    this->graph()->setLineStyle((QCPGraph::LineStyle)(rand()%5+1));
    if (rand()%100 > 75)
      this->graph()->setScatterStyle(QCPScatterStyle((QCPScatterStyle::ScatterShape)(rand()%9+1)));
    QPen graphPen;
    graphPen.setColor(QColor(rand()%245+10, rand()%245+10, rand()%245+10));
    graphPen.setWidthF(rand()/(double)RAND_MAX*2+1);
    this->graph()->setPen(graphPen);
    this->replot();
}

void CustomPlot::removeAllGraphs()
{
    this->clearGraphs();
    this->replot();
}

void CustomPlot::changeLineStyle()
{
    for(int graph_index=0;graph_index<dataNum.count();graph_index++)
    {
        int lineStyle;

        if(this->graph(graph_index)->lineStyle()==QCPGraph::lsLine)
            lineStyle=QCPGraph::lsStepLeft;
        else if(this->graph(graph_index)->lineStyle()==QCPGraph::lsStepLeft)
            lineStyle=QCPGraph::lsStepRight;
        else if(this->graph(graph_index)->lineStyle()==QCPGraph::lsStepRight)
            lineStyle=QCPGraph::lsStepCenter;
        else if(this->graph(graph_index)->lineStyle()==QCPGraph::lsStepCenter)
            lineStyle=QCPGraph::lsNone;
        else if(this->graph(graph_index)->lineStyle()==QCPGraph::lsNone)
            lineStyle=QCPGraph::lsImpulse;
        else if(this->graph(graph_index)->lineStyle()==QCPGraph::lsImpulse)
            lineStyle=QCPGraph::lsLine;


        this->graph(graph_index)->setLineStyle((QCPGraph::LineStyle)lineStyle);

        emit appearanceChanged(graph_index, this->graph(graph_index)->brush().color(),this->graph(graph_index)->pen().color(),this->graph(graph_index)->lineStyle(), this->graph(graph_index)->scatterStyle().shape());

    }

    this->replot();

}

void CustomPlot::changePoint()
{
    for(int graph_index=0;graph_index<dataNum.count();graph_index++)
    {
        int pointStyle=this->graph(graph_index)->scatterStyle().shape();

        pointStyle++;
        if(pointStyle>QCPScatterStyle::ssPeace)
            pointStyle=QCPScatterStyle::ssNone;


        this->graph(graph_index)->setScatterStyle(QCPScatterStyle((QCPScatterStyle::ScatterShape) pointStyle));

        emit appearanceChanged(graph_index, this->graph(graph_index)->brush().color(),this->graph(graph_index)->pen().color(),this->graph(graph_index)->lineStyle(), this->graph(graph_index)->scatterStyle().shape());
    }

    this->replot();
}

void CustomPlot::changeBrushColor()
{
    if(!hasBrush)
        return;

    for(int graph_index=0;graph_index<dataNum.count();graph_index++)
    {
        this->graph(graph_index)->setBrush(QBrush(QColor(rand()%245+10,rand()%245+10,rand()%245+10,150)));

        emit appearanceChanged(graph_index, this->graph(graph_index)->brush().color(),this->graph(graph_index)->pen().color(),this->graph(graph_index)->lineStyle(), this->graph(graph_index)->scatterStyle().shape());
    }

    this->replot();
}

void CustomPlot::changePenColor()
{
    for(int graph_index=0;graph_index<dataNum.count();graph_index++)
    {
        QPen graphPen = this->graph(graph_index)->pen();
        graphPen.setColor(QColor(rand()%245+10, rand()%245+10, rand()%245+10));
        this->graph(graph_index)->setPen(graphPen);

        emit appearanceChanged(graph_index, this->graph(graph_index)->brush().color(),this->graph(graph_index)->pen().color(),this->graph(graph_index)->lineStyle(), this->graph(graph_index)->scatterStyle().shape());
    }
    this->replot();
}

void CustomPlot::setBrushColor(int graph_index, QColor color)
{
    QColor c = color;
    c.setAlpha(150);
    this->graph(graph_index)->setBrush(QBrush(c));

    this->replot();
}

void CustomPlot::setPenColor(int graph_index, QColor color)
{
    QPen graphPen = this->graph(graph_index)->pen();
    graphPen.setColor(color);
    this->graph(graph_index)->setPen(graphPen);

    this->replot();
}

void CustomPlot::rescalePlot()
{
    for(int i=0;i<dataNum.count();i++)
    {
        firstTime = true;
        updateGraphInRange(i, this->xAxis->range());
    }

    this->replot();
}

void CustomPlot::setHasBrush(bool val)
{
    hasBrush = val;
}

void CustomPlot::setHasPointTooltip(bool val)
{
    hasPointTooltip = val;
}

void CustomPlot::setRectangleZoomMode()
{
    rectangleZoom = true;
    this->axisRect()->setRangeDrag(0);
    this->axisRect()->setRangeZoom(0);
    zoomModeToolButton->setIcon(QIcon(":/rectangleZoom"));
}

void CustomPlot::setScrollZoomMode()
{
    rectangleZoom = false;
    this->axisRect()->setRangeDrag(Qt::Horizontal|Qt::Vertical);
    this->axisRect()->setRangeZoom(Qt::Horizontal|Qt::Vertical);
    zoomModeToolButton->setIcon(QIcon(":/scrollZoom"));
}

void CustomPlot::fullscreen()
{
    isFullScreen = ! isFullScreen;

    if(isFullScreen)
    {

        thePlotLayout->getItemPosition(thePlotLayout->indexOf(this),&plotRow, &plotCol, &plotRowSpan, &plotColSpan);

        fullscreenLayout->addWidget(this,0,0,0,0);
        fullscreenBut->setToolTip(tr("Show Normal"));
        fullscreenDialog->showFullScreen();

        //creating shortcut
        shortcut = new QShortcut(QKeySequence(Qt::Key_Escape),this,SLOT(fullscreen()));
        shortcut->setAutoRepeat(false);
    }
    else
    {
        thePlotLayout->addWidget(this, plotRow, plotCol, plotRowSpan, plotColSpan);
        fullscreenDialog->close();
        fullscreenBut->setToolTip(tr("Show Full Screen"));

        delete shortcut;
    }
}

void CustomPlot::setLegendVisible(bool val)
{
    this->legend->setVisible(val);
    this->replot();
}

void CustomPlot::setHasfullScreenBut(bool val)
{
    fullscreenBut->setVisible(val);
}

void CustomPlot::setHasChangeBrushBut(bool val)
{
    changeBrushBut->setVisible(val);
}

void CustomPlot::setHasChangeLineStyleBut(bool val)
{
    changeLineStyleBut->setVisible(val);
}

void CustomPlot::setHasChangePointStyleBut(bool val)
{
    changePointStyleBut->setVisible(val);
}

void CustomPlot::setHasRescaleBut(bool val)
{
    rescaleBut->setVisible(val);
}

void CustomPlot::setHasLegendCheckBox(bool val)
{
    legendChkBx->setVisible(val);
}

void CustomPlot::setHasZoomModeBut(bool val)
{
    zoomModeToolButton->setVisible(val);
}

void CustomPlot::setButtonsTransparency(int percent)
{
    QPalette palette = fullscreenBut->palette();
    QColor color = palette.button().color();
    color.setAlpha(((double)(100.0-percent)/100.0)*255.0);
    palette.setBrush(QPalette::Button, color);
    fullscreenBut->setPalette(palette);
    changeBrushBut->setPalette(palette);
    changeLineStyleBut->setPalette(palette);
    changePointStyleBut->setPalette(palette);
    rescaleBut->setPalette(palette);
    zoomModeToolButton->setPalette(palette);
}
