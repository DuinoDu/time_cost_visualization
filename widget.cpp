#include "widget.h"
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QChart>
#include <QtCharts/QXYSeries>
#include <QtCharts/QValueAxis>
#include <QtWidgets/QVBoxLayout>
#include <QTimer>
#include <string>
#include <vector>
#include <QtCore/QtMath>


QT_CHARTS_USE_NAMESPACE

std::vector<std::string> split_string(const std::string& str, const char& c)
{
    std::string buff{""};
    std::vector<std::string> v;
    for(auto n : str){
        if(n != c) buff+=n;
        else if(n == c && buff != ""){
            v.push_back(buff);
            buff="";
        }
    }
    if(buff != "") v.push_back(buff);
    return v;
}

void XYSeriesReader::update(const std::string data)
{
    std::vector<std::string> time_costs = split_string(data, ' ');
    if(time_costs.size() > 0 && time_costs[0] != prev_data)
    {
        assert(time_costs.size() == 6);
        for(int i = 0; i < time_costs.size(); ++i){
            qint64 range = 200;
            QVector<QPointF> oldPoints = m_series[i]->pointsVector();
            QVector<QPointF> points;
            int updateTime = 1;

            if (oldPoints.count() < range) {
                points = m_series[i]->pointsVector();
            } else {
                for (int i = updateTime; i < oldPoints.count(); i++){
                    points.append(QPointF(i - updateTime, oldPoints.at(i).y()));
                }
            }
            qint64 size = points.count();
            for (int k = 0; k < updateTime; k++){
                points.append(QPointF(k+size, std::stof(time_costs[0]) ));
            }
            m_series[i]->replace(points);
            prev_data = time_costs[0];
        }
    }
}

TimeWidget::TimeWidget(QWidget *parent)
    : QWidget(parent),
      m_chart(0),
      m_reader(0)
{
    m_chart = new QChart;
    QChartView *chartView = new QChartView(m_chart);
    chartView->setMinimumSize(1200, 600);

    QValueAxis *axisX = new QValueAxis;
    axisX->setRange(0, 200);
    QValueAxis *axisY = new QValueAxis;
    axisY->setRange(0, 125);
    axisY->setTitleText("Cost/ms");

    int series_num = 6;
    const QList<QString> legend_names = {"all", "preprocess", "proposal", "detect","tracking", "recognition"};
    for(int i = 0; i < series_num; ++i){
        _addSeries(legend_names[i]);
        if(i == 0){
            m_chart->setAxisX(axisX, m_series[0]);
            m_chart->setAxisY(axisY, m_series[0]);
            //m_chart->createDefaultAxes();
        }
    }

    m_chart->setTitle("Time Cost");

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(chartView);
    setLayout(mainLayout);
    m_reader = new XYSeriesReader(m_series);

    timer = new QTimer;
    timer->setInterval(20);
    QObject::connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start();
}

void TimeWidget::setData(boost::array<char, 1000>* _p_buf, size_t* _p_len)
{
    p_buf = _p_buf;
    p_len = _p_len;
}

void TimeWidget::_addSeries(const QString& name)
{
    QLineSeries *series = new QLineSeries();
    m_series.append(series);

    // Make some sine wave for data
    QList<QPointF> data;
    int offset = m_chart->series().count();
    for (int i = 0; i < 360; i++) {
        qreal x = offset * 20 + i;
        data.append(QPointF(i, qSin(2.0 * 3.141592 * x / 360.0)));
    }
    //series->setName(QString("line " + QString::number(m_series.count())));
    series->setName(name);
    m_chart->addSeries(series);
}
