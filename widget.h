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

class XYSeriesReader : public QObject
{
    Q_OBJECT
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
    //TimeWidget(QWidget *parent = 0);
    TimeWidget(QWidget *parent = 0, std::vector<std::string> line_names={"all", "preprocess", "proposal", "detect","tracking", "recognition"});
    void setData(boost::array<char, 1000>* p_buf, size_t* p_len);

public slots:
    void update(){
        std::string data((*p_buf).data(), *p_len);
        m_reader->update(data);
    }

private slots:
    void handleMarkerClicked();

private:
    QChart *m_chart;
    QList<QLineSeries *> m_series;
    XYSeriesReader *m_reader;

    QTimer *timer;
    boost::array<char, 1000>* p_buf;
    size_t* p_len;
    int series_num = 6;
    const QStringList color_names = {"red","yellow","green","blue","purple","gray", "pink"};
    QList<QString> legend_names = {"all", "preprocess", "proposal", "detect","tracking", "recognition"};

    void _addSeries(int num);
};

#endif // WIDGET_H
