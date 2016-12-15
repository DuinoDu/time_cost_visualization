#ifndef WIDGET_H
#define WIDGET_H

#include <QtWidgets/QWidget>
#include <QtCharts/QChartGlobal>
#include <boost/array.hpp>
#include <QTimer>
#include <QList>
#include <vector>
#include <string>

QT_CHARTS_BEGIN_NAMESPACE
class QChart;
class QLineSeries;
class QXYSeries;
QT_CHARTS_END_NAMESPACE

QT_CHARTS_USE_NAMESPACE

class XYSeriesReader
{
public:
    XYSeriesReader(QList<QLineSeries*> series):m_series(series){}
    void update(const std::string data);
private:
    QList<QLineSeries*> m_series;
    std::string prev_data;
};

class TimeWidget : public QWidget
{
    Q_OBJECT

public:
    TimeWidget(QWidget *parent = 0);
    void setData(boost::array<char, 1000>* p_buf, size_t* p_len);

public slots:
    void update(){
        std::string data((*p_buf).data(), *p_len);
        m_reader->update(data);
    }

private:
    QChart *m_chart;
    QList<QLineSeries *> m_series;
    XYSeriesReader *m_reader;

    QTimer *timer;
    boost::array<char, 1000>* p_buf;
    size_t* p_len;

    void _addSeries(const QString& name);
};

#endif // WIDGET_H
