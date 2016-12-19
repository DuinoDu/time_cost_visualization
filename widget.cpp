#include "widget.h"
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QChart>
#include <QtCharts/QXYSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QLegendMarker>
#include <QtCharts/QXYLegendMarker>
#include <QtWidgets/QVBoxLayout>
#include <QTimer>
#include <string>
#include <vector>
#include <QtCore/QtMath>
#include <QDebug>


QT_CHARTS_USE_NAMESPACE

/*
 * XYSeriesReader
 */

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
    std::vector<std::string> time_costs_data = split_string(data, ' ');

    if(time_costs_data.size() > 0 && time_costs_data[0] != prev_data)
    {
        assert(time_costs_data.size() == 6);
        assert(m_series.size() == 6);
        for(int i = 0; i < time_costs_data.size(); ++i){
            qint64 range = 200;
            QVector<QPointF> oldPoints = m_series[i]->pointsVector();
            QVector<QPointF> points;

            if (oldPoints.count() < range){
                points = oldPoints;
            }else{
                for (int j = 1; j < oldPoints.count(); ++j){
                    points.append(QPointF(j-1, oldPoints.at(j).y()));
                }
            }
            points.append(QPointF(points.count()+1, std::stof(time_costs_data[i])));
            m_series[i]->replace(points);
            //qDebug() << "num:" << i;
            //qDebug() << points;
        }
        prev_data = time_costs_data[0];
    }
}


/*
 * TimeWidget
 */

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
    axisY->setRange(-2, 150);
    axisY->setTitleText("Cost(ms)");

    for(int i = 0; i < series_num; ++i){
        _addSeries(i);
        m_chart->setAxisX(axisX, m_series[i]);
        m_chart->setAxisY(axisY, m_series[i]);
    }

    m_chart->setTitle("Time Cost");

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(chartView);
    setLayout(mainLayout);

    foreach (QLegendMarker* marker, m_chart->legend()->markers()) {
        // Disconnect possible existing connection to avoid multiple connections
        QObject::disconnect(marker, SIGNAL(clicked()), this, SLOT(handleMarkerClicked()));
        QObject::connect(marker, SIGNAL(clicked()), this, SLOT(handleMarkerClicked()));
    }

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

void TimeWidget::handleMarkerClicked()
{
    QLegendMarker* marker = qobject_cast<QLegendMarker*> (sender());
    Q_ASSERT(marker);

    switch (marker->type())
    {
        case QLegendMarker::LegendMarkerTypeXY:
        {
        // Toggle visibility of series
        marker->series()->setVisible(!marker->series()->isVisible());

        // Turn legend marker back to visible, since hiding series also hides the marker
        // and we don't want it to happen now.
        marker->setVisible(true);

        // Dim the marker, if series is not visible
        qreal alpha = 1.0;

        if (!marker->series()->isVisible()) {
            alpha = 0.5;
        }

        QColor color;
        QBrush brush = marker->labelBrush();
        color = brush.color();
        color.setAlphaF(alpha);
        brush.setColor(color);
        marker->setLabelBrush(brush);

        brush = marker->brush();
        color = brush.color();
        color.setAlphaF(alpha);
        brush.setColor(color);
        marker->setBrush(brush);

        QPen pen = marker->pen();
        color = pen.color();
        color.setAlphaF(alpha);
        pen.setColor(color);
        marker->setPen(pen);

        break;
        }
    default:
        {
        qDebug() << "Unknown marker type";
        break;
        }
    }
}

void TimeWidget::_addSeries(int num)
{
    QLineSeries *series = new QLineSeries();
    series->setName(legend_names[num]);
    QPen pen;
    pen.setColor(QColor(color_names[num]));
    pen.setWidth(2);
    series->setPen(pen);
    m_series.append(series);

    m_chart->addSeries(series);
}
