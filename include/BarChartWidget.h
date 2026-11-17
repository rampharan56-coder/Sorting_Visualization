#pragma once

#include <QWidget>
#include <vector>

class BarChartWidget : public QWidget
{
public:
    explicit BarChartWidget(QWidget* parent = nullptr);

    void setFrameState(const std::vector<int>& arr,
                       int highlightA = -1,
                       int highlightB = -1,
                       int sortedBoundary = -1,
                       bool sortedIsPrefix = false);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    std::vector<int> m_array;
    int  m_highlightA     = -1;
    int  m_highlightB     = -1;
    int  m_sortedBoundary = -1;
    bool m_sortedIsPrefix = false;
};
