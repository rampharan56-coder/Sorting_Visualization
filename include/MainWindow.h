#pragma once

#include <QMainWindow>
#include <QComboBox>
#include <QSpinBox>
#include <QSlider>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QTableWidget>
#include <QEventLoop>
#include <vector>
#include <string>
#include <functional>

#include "SortStats.h"
#include "BarChartWidget.h"

class MainWindow : public QMainWindow
{
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

protected:
    void closeEvent(QCloseEvent* event) override;

private:
    struct AlgorithmEntry
    {
        std::string name;
        std::function<SortStats(std::vector<int>, bool)> run;
    };

    void setupUi();
    void applyTheme();
    void connectSignals();

    void generateRandomData();
    void applyManualData();
    void resetToOriginalData();

    void startSelectedSort();
    void togglePause();
    void stepOnce();
    void stopSorting();
    void runBenchmarkAll();

    void handleFrame(const std::string& title,
                     const std::string& message,
                     const std::vector<int>& arr,
                     int a, int b,
                     int sortedBoundary,
                     bool sortedIsPrefix);

    void handleComplete(const std::string& title,
                        const std::vector<int>& arr,
                        long long comparisons,
                        long long swapsOrShifts,
                        const std::string& secondMetricLabel);

    void waitForNextStep();
    void updateControlsState();

    std::vector<AlgorithmEntry> m_algorithms;
    std::vector<int>            m_originalData;
    std::vector<int>            m_currentData;

    bool        m_isSorting      = false;
    bool        m_isPaused       = false;
    bool        m_stepRequested  = false;
    bool        m_stopRequested  = false;
    long long   m_frameCounter   = 0;
    QEventLoop* m_activeWaitLoop = nullptr;

    // UI Widgets
    BarChartWidget* m_canvas         = nullptr;
    QLabel*         m_titleLabel     = nullptr;
    QLabel*         m_messageLabel   = nullptr;
    QLabel*         m_statsLabel     = nullptr;

    QComboBox*      m_algoCombo      = nullptr;
    QPushButton*    m_startBtn       = nullptr;
    QPushButton*    m_pauseBtn       = nullptr;
    QPushButton*    m_stepBtn        = nullptr;
    QPushButton*    m_stopResetBtn   = nullptr;
    QPushButton*    m_benchmarkBtn   = nullptr;

    QSlider*        m_speedSlider    = nullptr;
    QSpinBox*       m_speedSpin      = nullptr;

    QSpinBox*       m_sizeSpin       = nullptr;
    QSpinBox*       m_minSpin        = nullptr;
    QSpinBox*       m_maxSpin        = nullptr;
    QPushButton*    m_randomBtn      = nullptr;

    QLineEdit*      m_manualInput    = nullptr;
    QPushButton*    m_manualApplyBtn = nullptr;

    QTableWidget*   m_benchmarkTable = nullptr;
};
