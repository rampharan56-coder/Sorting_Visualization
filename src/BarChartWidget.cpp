#include "BarChartWidget.h"

#include <QPainter>
#include <QPaintEvent>
#include <QFontMetrics>
#include <algorithm>

BarChartWidget::BarChartWidget(QWidget* parent)
    : QWidget(parent)
{
    setMinimumSize(640, 360);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void BarChartWidget::setFrameState(const std::vector<int>& arr,
                                   int highlightA,
                                   int highlightB,
                                   int sortedBoundary,
                                   bool sortedIsPrefix)
{
    m_array          = arr;
    m_highlightA     = highlightA;
    m_highlightB     = highlightB;
    m_sortedBoundary = sortedBoundary;
    m_sortedIsPrefix = sortedIsPrefix;
    update();
}

void BarChartWidget::paintEvent(QPaintEvent* /*event*/)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    const int w = width();
    const int h = height();

    // Warm dark canvas background
    painter.fillRect(rect(), QColor(24, 24, 28));

    // Subtle border
    painter.setPen(QPen(QColor(55, 58, 68), 1));
    painter.drawRoundedRect(rect().adjusted(1, 1, -1, -1), 8, 8);

    if (m_array.empty())
    {
        painter.setPen(QColor(160, 165, 175));
        QFont emptyFont = font();
        emptyFont.setPointSize(12);
        painter.setFont(emptyFont);
        painter.drawText(rect(), Qt::AlignCenter, "No array loaded. Generate or enter an array to begin.");
        return;
    }

    const int n = static_cast<int>(m_array.size());
    const int maxVal = std::max(1, *std::max_element(m_array.begin(), m_array.end()));

    const int leftMargin   = 48;
    const int rightMargin  = 24;
    const int topMargin    = 36;
    const int bottomMargin = 40;

    const int plotW = std::max(10, w - leftMargin - rightMargin);
    const int plotH = std::max(10, h - topMargin - bottomMargin);

    // Draw horizontal reference grid lines & Y-axis labels
    QFont axisFont = font();
    axisFont.setPointSize(8);
    painter.setFont(axisFont);

    const int gridSteps = 4;
    for (int step = 0; step <= gridSteps; ++step)
    {
        const double ratio = static_cast<double>(step) / gridSteps;
        const int y = topMargin + plotH - static_cast<int>(ratio * plotH);
        const int val = static_cast<int>(ratio * maxVal);

        painter.setPen(QPen(QColor(45, 48, 58), 1, Qt::DashLine));
        painter.drawLine(leftMargin, y, leftMargin + plotW, y);

        painter.setPen(QColor(145, 150, 165));
        painter.drawText(QRect(4, y - 10, leftMargin - 10, 20),
                         Qt::AlignRight | Qt::AlignVCenter,
                         QString::number(val));
    }

    // Baseline
    painter.setPen(QPen(QColor(90, 95, 110), 2));
    painter.drawLine(leftMargin, topMargin + plotH, leftMargin + plotW, topMargin + plotH);

    // Bar geometry
    const double slotWidth = static_cast<double>(plotW) / n;
    const double gap       = std::clamp(slotWidth * 0.18, 1.0, 8.0);
    const double barWidth  = std::max(2.0, slotWidth - gap);

    // Colors matching Config::Color semantics:
    //   Cyan   -> Unsorted default
    //   Yellow -> Active comparison / swap (a, b)
    //   Green  -> Sorted region
    const QColor kCyanColor(34, 211, 238);
    const QColor kYellowColor(250, 204, 21);
    const QColor kGreenColor(74, 222, 128);

    QFont valFont = font();
    valFont.setPointSize(n <= 25 ? 9 : 7);
    valFont.setBold(true);
    painter.setFont(valFont);

    for (int i = 0; i < n; ++i)
    {
        const bool isSorted = (m_sortedBoundary >= 0) &&
            (m_sortedIsPrefix ? (i < m_sortedBoundary) : (i >= m_sortedBoundary));
        const bool isHighlighted = (i == m_highlightA || i == m_highlightB);

        QColor barColor = kCyanColor;
        if (isSorted)
            barColor = kGreenColor;
        if (isHighlighted)
            barColor = kYellowColor;

        const double normalized = static_cast<double>(std::max(0, m_array[i])) / maxVal;
        const double barH       = std::max(4.0, normalized * plotH);
        const double x          = leftMargin + i * slotWidth + gap * 0.5;
        const double y          = topMargin + plotH - barH;

        QRectF barRect(x, y, barWidth, barH);

        painter.setPen(Qt::NoPen);
        painter.setBrush(barColor);
        const double radius = std::min(4.0, barWidth * 0.3);
        painter.drawRoundedRect(barRect, radius, radius);

        // Draw value label above bar and index label below baseline when array is reasonably small
        if (n <= 40)
        {
            painter.setPen(isHighlighted ? kYellowColor : QColor(225, 230, 240));
            QRectF labelRect(x - 6, y - 20, barWidth + 12, 18);
            painter.drawText(labelRect, Qt::AlignCenter, QString::number(m_array[i]));

            painter.setPen(QColor(120, 125, 140));
            QRectF idxRect(x - 4, topMargin + plotH + 4, barWidth + 8, 18);
            painter.drawText(idxRect, Qt::AlignCenter, QString::number(i));
        }
    }
}
