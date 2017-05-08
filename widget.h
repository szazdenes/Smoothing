#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <qwt/build/include/qwt_plot_curve.h>
#include <QDir>
#include <QStringList>


namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT
    
public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

    int upperInteger (double halfmax);
    int lowerInteger (double halfmax);
    
private:
    Ui::Widget *ui;

    QList< QVector<double> > degList, errorList;
    QVector<double> sortedErrors;
    QList<QwtPlot*> plotList;
    QwtPlotCurve* curve1;
    QDir dir;
    QStringList fileNames, orderedFileNames;
    double xmax;

    void setPlottingData(QwtPlot *plot, QwtPlotCurve *curve, QString title, QVector<double> xdata, QVector<double> ydata, QwtText xtext, QwtText ytext, QColor color, double xmin, double xmax, double ymin, double ymax);
    void setXmax();
    QVector<double> gaussianSmoothing(QVector<double> &data, int half_kernel, double sigma);

private slots:

    void loadFromFile();

    void on_smoothWidget_currentChanged(int index);
    void on_exportPushButton_clicked();
    void on_smoothButton_clicked();
    void on_loadButton_clicked();
};


#endif // WIDGET_H
