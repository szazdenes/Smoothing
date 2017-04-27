#include "widget.h"
#include "ui_widget.h"
#include <QFile>
#include <QFileDialog>
#include "math.h"
#include <qwt/build/include/qwt_plot.h>
#include <qwt/build/include/qwt_plot_curve.h>
#include <qalgorithms.h>
#include <qwt/build/include/qwt_text.h>
#include <QDir>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    xmax=0;
}

Widget::~Widget()
{
    delete ui;
}

void Widget::loadFromFile(){

    degList.clear(); errorList.clear(); fileNames.clear(); orderedFileNames.clear(); plotList.clear();
    double deg = -99999, error = -99999;

    plotList.append(ui->qwtPlot_01);
    plotList.append(ui->qwtPlot_02);
    plotList.append(ui->qwtPlot_03);
    plotList.append(ui->qwtPlot_04);
    plotList.append(ui->qwtPlot_05);
    plotList.append(ui->qwtPlot_06);
    plotList.append(ui->qwtPlot_07);
    plotList.append(ui->qwtPlot_08);
    plotList.append(ui->qwtPlot_09);
    plotList.append(ui->qwtPlot_10);
    plotList.append(ui->qwtPlot_11);
    plotList.append(ui->qwtPlot_12);

    for(int i = 0; i < plotList.size(); i++){
        plotList.at(i)->detachItems();
    }

//    dir = (QFileDialog::getExistingDirectory(this, tr("Open Directory"), dir.absolutePath()));

    fileNames = QFileDialog::getOpenFileNames(this, 0, "/home/denes/Documents/Labor/Viking/1000Viking/felhők szilvi");

    ui->folderLabel->setText(dir.absolutePath());

//    foreach(QString current, fileNames)
//        orderedFileNames.append(QDir::absoluteFilePath(current));

    orderedFileNames = fileNames;

    int fileSize = orderedFileNames.size();

    for (int k = 0; k < fileSize; k++) {

        QFile openFile(dir.filePath(orderedFileNames.at(k)));

        if(!openFile.open(QIODevice::ReadOnly | QIODevice::Text))
            qDebug("baj");

        QTextStream read(&openFile);
        QString line;

        QVector<double> degVector, errorVector;

        for (int i=0; i<3; i++)
            read.readLine();

        while (!read.atEnd()){
            line = read.readLine();
            QTextStream stream(&line);

            stream >> deg >> error;

            degVector.append(deg);
            errorVector.append(error);
        }

        openFile.close();

        degList.append(degVector);
        errorList.append(errorVector);

        sortedErrors = errorList.at(k);
        setXmax();

        curve1 = new QwtPlotCurve();

        QString filename = QString(orderedFileNames.at(k)).split("/").last();

        setPlottingData(plotList.at(k), curve1, filename, degList.at(k), errorList.at(k), QwtText("Deg"), QwtText(""), QColor(Qt::red), -180, 180, 0, xmax);

    }


}

void Widget::setXmax(){

    qSort(sortedErrors.begin(), sortedErrors.end());

    xmax = sortedErrors.last() + sortedErrors.last() * 0.1;
}

QVector<double> Widget::gaussianSmoothing(QVector<double> &data, int half_kernel, double sigma)
{
    QVector<double> smoothedData;
    double smoothed;
    for(int i = 0; i < data.size(); i++){
        double weight = 0, sum = 0, weightSum = 0;
        for(int j = -half_kernel; j <= half_kernel; j++){
            if(i+j >= 0 && i+j < data.size()){
                weight = qExp(-(j*j)/(2*sigma*sigma));
                sum += data.at(i+j) * weight;
            }
            if(i+j < 0 || i+j >= data.size()){
                weight = 0;
                sum += weight;
            }
            weightSum += weight;
        }
        smoothed = sum / weightSum;
        smoothedData.append(smoothed);
    }

    return smoothedData;
}

void Widget::setPlottingData(QwtPlot *plot, QwtPlotCurve *curve, QString title, QVector<double> xdata, QVector<double> ydata, QwtText xtext, QwtText ytext, QColor color, double xmin, double xmax, double ymin, double ymax){

    QwtText titleText = title;
    QFont font;
    font.setBold(true);
    font.setPointSize(6);
    titleText.setFont(font);

    plot->detachItems();
    plot->setTitle(titleText);
    plot->setAxisTitle(QwtPlot::xBottom, xtext);
    plot->setAxisTitle(QwtPlot::yLeft, ytext);
    curve->setSamples(xdata, ydata);
    curve->setPen(QPen(color, 1));
    curve->attach(plot);
    plot->setAxisScale(QwtPlot::xBottom, xmin, xmax);
    plot->setAxisScale(QwtPlot::yLeft, ymin, ymax);
    plot->setCanvasBackground(Qt::white);
    plot->replot();


}

int Widget::upperInteger(double halfmax){
    int i=0;
    while (i <= halfmax) i++;
    return i;
}

int Widget::lowerInteger(double halfmax){
    int i=0;
    while (i <= halfmax) i++;
    return i-1;
}


void Widget::on_smoothWidget_currentChanged(int index)
{
    if(index == ui->smoothWidget->indexOf(ui->tab_2)) {

        ui->resultsTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        ui->resultsTableWidget->setHorizontalHeaderLabels(QStringList() << "File Name" << "North Error" << "FWHM");
        ui->resultsTableWidget->clearContents();
        ui->resultsTableWidget->setRowCount(0);

        int listSize = errorList.size();
        QList<double> maxList, minDistanceList, sortDist1, sortDist2, fwhmList;


        for (int i = 0; i < listSize; i++) {
            sortDist1.clear(); sortDist2.clear(); minDistanceList.clear();
            sortedErrors = errorList.at(i);
            qSort(sortedErrors.begin(), sortedErrors.end());
            maxList.append(degList.at(i).at(errorList.at(i).indexOf(sortedErrors.last())));

            int listLength = errorList.at(i).size();

            for (int j = 0; j < listLength; j++){
                double halfValue = sortedErrors.last() / 2;
                double dist = qAbs(errorList.at(i).at(j) - halfValue);

                minDistanceList.append(dist);
                if (j <= errorList.at(i).indexOf(sortedErrors.last()))
                    sortDist1.append(dist);
                else if (j > errorList.at(i).indexOf(sortedErrors.last()))
                    sortDist2.append(dist);

            }

            qSort(sortDist1.begin(), sortDist1.end());
            qSort(sortDist2.begin(), sortDist2.end());

            double fwhm = qAbs((degList.at(i)).at(minDistanceList.indexOf(sortDist2.first())) -  (degList.at(i)).at(minDistanceList.indexOf(sortDist1.first())));
            fwhmList.append(fwhm);

            ui->resultsTableWidget->insertRow(ui->resultsTableWidget->rowCount());
            ui->resultsTableWidget->setItem(i, 0, new QTableWidgetItem(QString(orderedFileNames.at(i)).split("/").last()));
            ui->resultsTableWidget->setItem(i, 1, new QTableWidgetItem(QString::number(maxList.at(i))));
            ui->resultsTableWidget->setItem(i, 2, new QTableWidgetItem(QString::number(fwhmList.at(i))));
        }

    }
}

void Widget::on_twentySmoothPushButton_clicked()
{
    for (int i = 0; i < 20; i++)
        ui->smoothButton->click();
}

void Widget::on_exportPushButton_clicked()
{
    QString exportFileName = QFileDialog::getSaveFileName(this, 0,"/home/denes/Documents/Labor/Viking/1000Viking/Results");
    if (!exportFileName.endsWith(".csv"))
        exportFileName.append(".csv");

    QFile exportFile(exportFileName);
    QTextStream write(&exportFile);
    if(!exportFile.exists()){
        if(!exportFile.open(QIODevice::WriteOnly | QIODevice::Text)){
            qDebug("File could not be opened.");
            return;
        }
        write << "#Filename\t" << "North error\t" << "FWHM\n";
    }

    else{
        if(!exportFile.open(QIODevice::Append | QIODevice::Text)){
            qDebug("File could not be opened.");
            return;
        }


    }

    if(ui->resultsTableWidget->rowCount() > 0){

        for(int i = 0; i < ui->resultsTableWidget->rowCount(); i++)
            write << ui->resultsTableWidget->item(i, 0)->text() + "\t" << ui->resultsTableWidget->item(i, 1)->text() + "\t" << ui->resultsTableWidget->item(i, 2)->text() + "\n";
    }

}

void Widget::on_smoothButton_clicked()
{
    QVector<double> smoothedVector;

    for (int i = 0; i < errorList.size(); i++) {

        smoothedVector = gaussianSmoothing((QVector<double>&) errorList.at(i), ui->kernelSpinBox->value(), (double) ui->sigmaSpinBox->value());

        /*
        for (int j = 0; j < errorList.at(i).size(); j++){


            if (j!=0 && j!=((errorList.at(i)).size()-1)) {
                double smooth = ((errorList.at(i)).at(j-1)+(errorList.at(i)).at(j)+(errorList.at(i)).at(j+1))/3.0;


                smoothedVector.replace(j, smooth); //Laplacian smoothing
            }
        }
        */

        errorList.replace(i,smoothedVector);

        curve1 = new QwtPlotCurve();
        sortedErrors = errorList.at(i);
        setXmax();
        QString filename = QString(orderedFileNames.at(i)).split("/").last();
        setPlottingData(plotList.at(i), curve1, filename, degList.at(i), errorList.at(i), QwtText("Deg"), QwtText(""), QColor(Qt::blue), -180, 180, 0, xmax);

    }

}

void Widget::on_loadButton_clicked()
{
    loadFromFile();
}
