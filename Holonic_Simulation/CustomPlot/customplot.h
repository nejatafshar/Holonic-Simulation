
#ifndef CUSTOMPLOT_H
#define CUSTOMPLOT_H

#include "CustomPlot_global.h"

#include "qcustomplot.h"

#include <QGridLayout>
#include <QPushButton>
#include <QDialog>
#include <QCheckBox>
#include <QShortcut>
#include <QRubberBand>
#include <QToolButton>
#include <QActionGroup>



class CUSTOMPLOTSHARED_EXPORT CustomPlot : public QCustomPlot
{
  Q_OBJECT

public:


  explicit CustomPlot(QWidget *parent = 0);
  virtual ~CustomPlot();


signals:

  void scaleChangedManually();

  void pointDoubleClicked(double x, double y);

  void appearanceChanged(int graph_index, QColor brushColor, QColor penColor, int lineStyle, int pointStyle);

private slots:

  void selectionChanged();
  void OnMousePress(QMouseEvent *event);
  void OnMouseRelease(QMouseEvent *event);
  void OnMouseMove(QMouseEvent *event);
  void mouseWheel();
  void removeSelectedGraph();
  void showPointToolTip(QMouseEvent *event);
  void onDoubleClicked(QMouseEvent *event);

  QVector<int> getnMaxIndexes(QVector<double> y, int n);
  QVector<int> getnMinIndexes(QVector<double> y, int n);

public slots:
  void addGraph(QString name, double * xdata, double * ydata, int num);
  void updateGraph(int graph_index, double * xdata, double * ydata, int num);
  void updateGraphData(int graph_index, double * xdata, double * ydata, int num);
  void updateRange(QCPRange range);
  void updateGraphInRange(int graph_index, QCPRange range);
  void clearGraph(int graph_index);
  void updateGraphAllData(int graph_index, double * xdata, double * ydata, int num);
  void addRandomGraph();
  void removeAllGraphs();
  void changeLineStyle();
  void changePoint();
  void changeBrushColor();
  void changePenColor();
  void setBrushColor(int graph_index, QColor color);
  void setPenColor(int graph_index, QColor color);
  void rescalePlot();
  void setHasBrush(bool val);
  void setHasPointTooltip(bool val);

  void setRectangleZoomMode();
  void setScrollZoomMode();

  void fullscreen();

  void setLegendVisible(bool val);

  void setHasfullScreenBut(bool val);
  void setHasChangeBrushBut(bool val);
  void setHasChangeLineStyleBut(bool val);
  void setHasChangePointStyleBut(bool val);
  void setHasRescaleBut(bool val);
  void setHasLegendCheckBox(bool val);
  void setHasZoomModeBut(bool val);

  void setButtonsTransparency(int percent);

public:

    QVector<QVector<double> > xData;
    QVector<QVector<double> > yData;
    QVector<int> dataNum;

    bool autoRescale;
    bool firstTime;

    bool holdMaxScale;
    double holdMaxScaleFactor;

    bool holdMinScale;
    double holdMinScaleFactor;

    bool rescaleYWithMargin;
    double YmarginFactor;

    bool rescaleXWithMargin;
    double XmarginFactor;

    bool hasFixedBottom;
    bool hasFixedTop;

    double fixedBottom;
    double fixedTop;



    int maxIndicateNum;
    int minIndicateNum;

    int indicateMode;


    QVector<QVector<QCPItemTracer *> > itemTracers;
    QVector<QVector<QCPItemText *> > itemTexts;
    QVector<QVector<QCPItemLine *> > itemLines;

    QPushButton * fullscreenBut;
    QPushButton * changeBrushBut;
    QPushButton * changeLineStyleBut;
    QPushButton * changePointStyleBut;
    QPushButton * rescaleBut;
    QCheckBox * legendChkBx;
    QToolButton * zoomModeToolButton;

    QDialog * fullscreenDialog;

    QShortcut * shortcut;

    QGridLayout * thePlotLayout;
    QGridLayout * layout;
    QGridLayout * fullscreenLayout;

    int plotRow;
    int plotCol;
    int plotRowSpan;
    int plotColSpan;

    bool isFullScreen;

    bool hasBrush;

    bool hasPointTooltip;

    bool showAllCoordinatesInTooltip;

    bool rectangleZoom;
    QRubberBand * mRubberBand;
    QPoint mOrigin;



};

#endif // CUSTOMPLOT_H

