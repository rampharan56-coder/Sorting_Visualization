#include "MainWindow.h"
#include "Config.h"
#include "InputUtils.h"
#include "Renderer.h"
#include "Sorting.h"

#include <QApplication>
#include <QCloseEvent>
#include <QFrame>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QSplitter>
#include <QTimer>
#include <QVBoxLayout>
#include <sstream>
#include <limits>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    m_algorithms = {
        {"Bubble Sort",    Sorting::bubbleSort},
        {"Insertion Sort", Sorting::insertionSort},
        {"Selection Sort", Sorting::selectionSort},
        {"Merge Sort",     Sorting::mergeSort},
        {"Quick Sort",     Sorting::quickSort},
        {"Heap Sort",      Sorting::heapSort},
        {"Shell Sort",     Sorting::shellSort},
    };

    Config::delayMs = 120;

    setupUi();
    applyTheme();
    connectSignals();

    Renderer::setGuiCallbacks(
        [this](const std::string& title, const std::string& message,
               const std::vector<int>& arr, int a, int b,
               int sortedBoundary, bool sortedIsPrefix)
        {
            handleFrame(title, message, arr, a, b, sortedBoundary, sortedIsPrefix);
        },
        [this](const std::string& title, const std::vector<int>& arr,
               long long comparisons, long long swapsOrShifts,
               const std::string& secondMetricLabel)
        {
            handleComplete(title, arr, comparisons, swapsOrShifts, secondMetricLabel);
        }
    );

    generateRandomData();
}

MainWindow::~MainWindow()
{
    Renderer::clearGuiCallbacks();
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    m_stopRequested = true;
    m_isPaused      = false;
    if (m_activeWaitLoop && m_activeWaitLoop->isRunning())
        m_activeWaitLoop->quit();
    QMainWindow::closeEvent(event);
}

void MainWindow::setupUi()
{
    setWindowTitle("Sorting Visualizer — Qt C++17 GUI");
    resize(1180, 740);

    QWidget* central = new QWidget(this);
    setCentralWidget(central);

    QHBoxLayout* rootLayout = new QHBoxLayout(central);
    rootLayout->setContentsMargins(14, 14, 14, 14);
    rootLayout->setSpacing(14);

    // ================= LEFT CONTROL PANEL =================
    QWidget* leftPanel = new QWidget(this);
    leftPanel->setFixedWidth(360);
    QVBoxLayout* leftLayout = new QVBoxLayout(leftPanel);
    leftLayout->setContentsMargins(0, 0, 0, 0);
    leftLayout->setSpacing(12);

    // 1. Array Setup Group
    QGroupBox* arrayGroup = new QGroupBox("1. Array Configuration", leftPanel);
    QVBoxLayout* arrayLayout = new QVBoxLayout(arrayGroup);

    QHBoxLayout* sizeRow = new QHBoxLayout();
    sizeRow->addWidget(new QLabel("Size:"));
    m_sizeSpin = new QSpinBox();
    m_sizeSpin->setRange(2, 150);
    m_sizeSpin->setValue(20);
    sizeRow->addWidget(m_sizeSpin);

    sizeRow->addWidget(new QLabel("Min:"));
    m_minSpin = new QSpinBox();
    m_minSpin->setRange(1, 999);
    m_minSpin->setValue(Config::kDefaultRandomMin);
    sizeRow->addWidget(m_minSpin);

    sizeRow->addWidget(new QLabel("Max:"));
    m_maxSpin = new QSpinBox();
    m_maxSpin->setRange(1, 999);
    m_maxSpin->setValue(Config::kDefaultRandomMax);
    sizeRow->addWidget(m_maxSpin);
    arrayLayout->addLayout(sizeRow);

    m_randomBtn = new QPushButton("Generate Random Array");
    arrayLayout->addWidget(m_randomBtn);

    QLabel* manualHint = new QLabel("Or enter custom numbers (comma/space separated):");
    manualHint->setWordWrap(true);
    arrayLayout->addWidget(manualHint);

    QHBoxLayout* manualRow = new QHBoxLayout();
    m_manualInput = new QLineEdit();
    m_manualInput->setPlaceholderText("e.g. 45, 12, 88, 7, 33, 64, 19");
    m_manualApplyBtn = new QPushButton("Apply");
    manualRow->addWidget(m_manualInput, 1);
    manualRow->addWidget(m_manualApplyBtn);
    arrayLayout->addLayout(manualRow);

    leftLayout->addWidget(arrayGroup);

    // 2. Algorithm & Playback Group
    QGroupBox* algoGroup = new QGroupBox("2. Sorting Algorithm & Controls", leftPanel);
    QVBoxLayout* algoLayout = new QVBoxLayout(algoGroup);

    QHBoxLayout* comboRow = new QHBoxLayout();
    comboRow->addWidget(new QLabel("Algorithm:"));
    m_algoCombo = new QComboBox();
    for (const auto& entry : m_algorithms)
        m_algoCombo->addItem(QString::fromStdString(entry.name));
    comboRow->addWidget(m_algoCombo, 1);
    algoLayout->addLayout(comboRow);

    QHBoxLayout* playRow1 = new QHBoxLayout();
    m_startBtn = new QPushButton("Start Sort");
    m_startBtn->setObjectName("primaryBtn");
    m_pauseBtn = new QPushButton("Pause");
    playRow1->addWidget(m_startBtn);
    playRow1->addWidget(m_pauseBtn);
    algoLayout->addLayout(playRow1);

    QHBoxLayout* playRow2 = new QHBoxLayout();
    m_stepBtn      = new QPushButton("Step (+1 Frame)");
    m_stopResetBtn = new QPushButton("Stop / Reset Array");
    playRow2->addWidget(m_stepBtn);
    playRow2->addWidget(m_stopResetBtn);
    algoLayout->addLayout(playRow2);

    // Speed Slider
    QHBoxLayout* speedRow = new QHBoxLayout();
    speedRow->addWidget(new QLabel("Delay (ms/step):"));
    m_speedSlider = new QSlider(Qt::Horizontal);
    m_speedSlider->setRange(1, 1000);
    m_speedSlider->setValue(Config::delayMs);
    m_speedSpin = new QSpinBox();
    m_speedSpin->setRange(1, 1000);
    m_speedSpin->setSuffix(" ms");
    m_speedSpin->setValue(Config::delayMs);
    speedRow->addWidget(m_speedSlider, 1);
    speedRow->addWidget(m_speedSpin);
    algoLayout->addLayout(speedRow);

    leftLayout->addWidget(algoGroup);

    // 3. Benchmark All Group
    QGroupBox* benchGroup = new QGroupBox("3. Benchmark All Algorithms", leftPanel);
    QVBoxLayout* benchLayout = new QVBoxLayout(benchGroup);

    m_benchmarkBtn = new QPushButton("Benchmark ALL (No Animation)");
    benchLayout->addWidget(m_benchmarkBtn);

    m_benchmarkTable = new QTableWidget(0, 4, benchGroup);
    m_benchmarkTable->setHorizontalHeaderLabels({"Algorithm", "Comparisons", "Swaps/Shifts", "Time (ms)"});
    m_benchmarkTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_benchmarkTable->verticalHeader()->setVisible(false);
    m_benchmarkTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_benchmarkTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_benchmarkTable->setAlternatingRowColors(true);
    benchLayout->addWidget(m_benchmarkTable, 1);

    leftLayout->addWidget(benchGroup, 1);

    rootLayout->addWidget(leftPanel);

    // ================= RIGHT VISUALIZER PANEL =================
    QWidget* rightPanel = new QWidget(this);
    QVBoxLayout* rightLayout = new QVBoxLayout(rightPanel);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(10);

    // Top Status Banner
    QFrame* banner = new QFrame(rightPanel);
    banner->setObjectName("statusBanner");
    QVBoxLayout* bannerLayout = new QVBoxLayout(banner);
    bannerLayout->setContentsMargins(16, 12, 16, 12);

    QHBoxLayout* topBannerRow = new QHBoxLayout();
    m_titleLabel = new QLabel("SORTING VISUALIZER");
    m_titleLabel->setObjectName("bannerTitle");
    m_statsLabel = new QLabel("Ready");
    m_statsLabel->setObjectName("bannerStats");
    topBannerRow->addWidget(m_titleLabel);
    topBannerRow->addStretch();
    topBannerRow->addWidget(m_statsLabel);
    bannerLayout->addLayout(topBannerRow);

    m_messageLabel = new QLabel("Select an algorithm and click 'Start Sort' to animate.");
    m_messageLabel->setObjectName("bannerMessage");
    bannerLayout->addWidget(m_messageLabel);

    rightLayout->addWidget(banner);

    // Bar Chart Canvas
    m_canvas = new BarChartWidget(rightPanel);
    rightLayout->addWidget(m_canvas, 1);

    // Legend Row
    QHBoxLayout* legendLayout = new QHBoxLayout();
    legendLayout->setContentsMargins(8, 2, 8, 2);
    auto makeLegendItem = [](const QString& colorHex, const QString& text) -> QWidget*
    {
        QWidget* item = new QWidget();
        QHBoxLayout* l = new QHBoxLayout(item);
        l->setContentsMargins(0, 0, 16, 0);
        l->setSpacing(6);
        QLabel* swatch = new QLabel();
        swatch->setFixedSize(14, 14);
        swatch->setStyleSheet(QString("background-color: %1; border-radius: 3px;").arg(colorHex));
        QLabel* lbl = new QLabel(text);
        l->addWidget(swatch);
        l->addWidget(lbl);
        return item;
    };

    legendLayout->addWidget(makeLegendItem("#22d3ee", "Unsorted (Cyan)"));
    legendLayout->addWidget(makeLegendItem("#facc15", "Active Comparison / Swap (Yellow)"));
    legendLayout->addWidget(makeLegendItem("#4ade80", "Sorted Region (Green)"));
    legendLayout->addStretch();
    rightLayout->addLayout(legendLayout);

    rootLayout->addWidget(rightPanel, 1);

    updateControlsState();
}

void MainWindow::applyTheme()
{
    setStyleSheet(R"(
        QMainWindow, QWidget {
            background-color: #1e1e24;
            color: #e6e1d6;
            font-family: "Consolas", "Segoe UI", monospace;
            font-size: 10pt;
        }
        QGroupBox {
            border: 1px solid #383844;
            border-radius: 8px;
            margin-top: 10px;
            padding-top: 10px;
            font-weight: bold;
            color: #f3c677;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 12px;
            padding: 0 6px;
        }
        QPushButton {
            background-color: #2d2f3b;
            border: 1px solid #45485a;
            border-radius: 6px;
            padding: 7px 12px;
            font-weight: bold;
            color: #e6e1d6;
        }
        QPushButton:hover {
            background-color: #3a3d4d;
            border-color: #f3c677;
        }
        QPushButton:disabled {
            background-color: #22232b;
            color: #6c6f7e;
            border-color: #2f313d;
        }
        QPushButton#primaryBtn {
            background-color: #d97706;
            border-color: #f59e0b;
            color: #ffffff;
        }
        QPushButton#primaryBtn:hover {
            background-color: #f59e0b;
        }
        QLineEdit, QSpinBox, QComboBox {
            background-color: #15161b;
            border: 1px solid #3d4050;
            border-radius: 5px;
            padding: 5px 8px;
            color: #f0ece1;
        }
        QTableWidget {
            background-color: #15161b;
            alternate-background-color: #1b1c24;
            gridline-color: #2e303e;
            border: 1px solid #383844;
            border-radius: 6px;
        }
        QHeaderView::section {
            background-color: #282a36;
            color: #f3c677;
            padding: 5px;
            border: 1px solid #383844;
            font-weight: bold;
        }
        QFrame#statusBanner {
            background-color: #262730;
            border: 1px solid #3d4050;
            border-radius: 8px;
        }
        QLabel#bannerTitle {
            font-size: 14pt;
            font-weight: bold;
            color: #f59e0b;
        }
        QLabel#bannerStats {
            font-size: 10pt;
            font-weight: bold;
            color: #4ade80;
        }
        QLabel#bannerMessage {
            font-size: 10.5pt;
            color: #d1d5db;
        }
    )");
}

void MainWindow::connectSignals()
{
    connect(m_randomBtn, &QPushButton::clicked, this, [this]() { generateRandomData(); });
    connect(m_manualApplyBtn, &QPushButton::clicked, this, [this]() { applyManualData(); });
    connect(m_manualInput, &QLineEdit::returnPressed, this, [this]() { applyManualData(); });

    connect(m_startBtn, &QPushButton::clicked, this, [this]() { startSelectedSort(); });
    connect(m_pauseBtn, &QPushButton::clicked, this, [this]() { togglePause(); });
    connect(m_stepBtn, &QPushButton::clicked, this, [this]() { stepOnce(); });
    connect(m_stopResetBtn, &QPushButton::clicked, this, [this]()
    {
        if (m_isSorting)
            stopSorting();
        else
            resetToOriginalData();
    });

    connect(m_benchmarkBtn, &QPushButton::clicked, this, [this]() { runBenchmarkAll(); });

    connect(m_speedSlider, &QSlider::valueChanged, this, [this](int val)
    {
        Config::delayMs = val;
        if (m_speedSpin->value() != val)
            m_speedSpin->setValue(val);
    });

    connect(m_speedSpin, QOverload<int>::of(&QSpinBox::valueChanged), this, [this](int val)
    {
        Config::delayMs = val;
        if (m_speedSlider->value() != val)
            m_speedSlider->setValue(val);
    });
}

void MainWindow::generateRandomData()
{
    if (m_isSorting)
        stopSorting();

    int minVal = m_minSpin->value();
    int maxVal = m_maxSpin->value();
    if (minVal > maxVal)
        std::swap(minVal, maxVal);

    const int size = m_sizeSpin->value();
    m_originalData = InputUtils::generateRandomArray(size, minVal, maxVal);
    m_currentData  = m_originalData;

    m_titleLabel->setText("SORTING VISUALIZER");
    m_messageLabel->setText(QString("Generated %1 random elements in range [%2..%3].")
                                .arg(size).arg(minVal).arg(maxVal));
    m_statsLabel->setText(QString("Array Size: %1").arg(size));
    m_canvas->setFrameState(m_currentData);
}

void MainWindow::applyManualData()
{
    if (m_isSorting)
        stopSorting();

    QString raw = m_manualInput->text();
    raw.replace(',', ' ');
    std::stringstream ss(raw.toStdString());
    std::vector<int> parsed;
    int val = 0;
    while (ss >> val)
        parsed.push_back(val);

    if (parsed.empty())
    {
        QMessageBox::warning(this, "Invalid Input",
                             "Please enter at least one valid integer (separated by spaces or commas).");
        return;
    }

    m_originalData = parsed;
    m_currentData  = m_originalData;
    m_sizeSpin->setValue(static_cast<int>(parsed.size()));

    m_titleLabel->setText("SORTING VISUALIZER");
    m_messageLabel->setText(QString("Loaded %1 custom elements.").arg(parsed.size()));
    m_statsLabel->setText(QString("Array Size: %1").arg(parsed.size()));
    m_canvas->setFrameState(m_currentData);
}

void MainWindow::resetToOriginalData()
{
    m_currentData = m_originalData;
    m_titleLabel->setText("SORTING VISUALIZER");
    m_messageLabel->setText("Array reset to unsorted state.");
    m_statsLabel->setText(QString("Array Size: %1").arg(m_currentData.size()));
    m_canvas->setFrameState(m_currentData);
}

void MainWindow::startSelectedSort()
{
    if (m_isSorting || m_originalData.empty())
        return;

    const int idx = m_algoCombo->currentIndex();
    if (idx < 0 || idx >= static_cast<int>(m_algorithms.size()))
        return;

    // Always start from the unsorted snapshot so repeated runs are easy to compare
    m_currentData   = m_originalData;
    m_isSorting     = true;
    m_isPaused      = false;
    m_stepRequested = false;
    m_stopRequested = false;
    m_frameCounter  = 0;
    updateControlsState();

    // Run the selected sorting algorithm from Sorting.cpp
    SortStats stats = m_algorithms[idx].run(m_currentData, /*visualize=*/true);

    const bool wasStopped = m_stopRequested;
    m_isSorting     = false;
    m_isPaused      = false;
    m_stepRequested = false;
    m_stopRequested = false;
    updateControlsState();

    if (wasStopped)
    {
        resetToOriginalData();
        m_messageLabel->setText("Sorting stopped and array reset to original unsorted order.");
    }
    else
    {
        m_currentData = stats.result;
    }
}

void MainWindow::togglePause()
{
    if (!m_isSorting)
        return;

    m_isPaused = !m_isPaused;
    m_pauseBtn->setText(m_isPaused ? "Resume" : "Pause");
    m_stepBtn->setEnabled(m_isPaused);

    if (!m_isPaused && m_activeWaitLoop && m_activeWaitLoop->isRunning())
        m_activeWaitLoop->quit();
}

void MainWindow::stepOnce()
{
    if (!m_isSorting || !m_isPaused)
        return;

    m_stepRequested = true;
    if (m_activeWaitLoop && m_activeWaitLoop->isRunning())
        m_activeWaitLoop->quit();
}

void MainWindow::stopSorting()
{
    if (!m_isSorting)
        return;

    m_stopRequested = true;
    m_isPaused      = false;
    if (m_activeWaitLoop && m_activeWaitLoop->isRunning())
        m_activeWaitLoop->quit();
}

void MainWindow::handleFrame(const std::string& title,
                             const std::string& message,
                             const std::vector<int>& arr,
                             int a, int b,
                             int sortedBoundary,
                             bool sortedIsPrefix)
{
    if (m_stopRequested)
        return;

    ++m_frameCounter;
    m_currentData = arr;

    m_titleLabel->setText(QString::fromStdString(title));
    m_messageLabel->setText(QString::fromStdString(message.empty() ? "Animating..." : message));
    m_statsLabel->setText(QString("Step Frame #%1  |  Speed: %2 ms/step")
                              .arg(m_frameCounter)
                              .arg(Config::delayMs));

    m_canvas->setFrameState(arr, a, b, sortedBoundary, sortedIsPrefix);
    QApplication::processEvents();

    waitForNextStep();
}

void MainWindow::handleComplete(const std::string& title,
                                const std::vector<int>& arr,
                                long long comparisons,
                                long long swapsOrShifts,
                                const std::string& secondMetricLabel)
{
    if (m_stopRequested)
        return;

    m_currentData = arr;
    m_titleLabel->setText(QString::fromStdString(title) + " — COMPLETE");
    m_messageLabel->setText("Sorting complete! All elements are in ascending order.");
    m_statsLabel->setText(QString("Comparisons: %1   |   %2: %3")
                              .arg(comparisons)
                              .arg(QString::fromStdString(secondMetricLabel))
                              .arg(swapsOrShifts));

    m_canvas->setFrameState(arr, -1, -1, static_cast<int>(arr.size()), true);
    QApplication::processEvents();
}

void MainWindow::waitForNextStep()
{
    if (m_stopRequested)
        return;

    QEventLoop loop;
    m_activeWaitLoop = &loop;

    if (m_isPaused)
    {
        m_messageLabel->setText(m_messageLabel->text() + "  [PAUSED]");
        while (m_isPaused && !m_stepRequested && !m_stopRequested)
        {
            loop.exec();
        }
        m_stepRequested = false;
    }
    else
    {
        const int delay = std::max(1, Config::delayMs);
        QTimer::singleShot(delay, &loop, &QEventLoop::quit);
        loop.exec();
    }

    m_activeWaitLoop = nullptr;
}

void MainWindow::runBenchmarkAll()
{
    if (m_isSorting || m_originalData.empty())
        return;

    m_benchmarkTable->setRowCount(static_cast<int>(m_algorithms.size()));

    std::vector<SortStats> results;
    results.reserve(m_algorithms.size());

    double fastestMs = std::numeric_limits<double>::max();
    for (const auto& algo : m_algorithms)
    {
        SortStats st = algo.run(m_originalData, /*visualize=*/false);
        if (st.elapsedMs < fastestMs)
            fastestMs = st.elapsedMs;
        results.push_back(st);
    }

    for (int row = 0; row < static_cast<int>(results.size()); ++row)
    {
        const SortStats& st = results[row];
        const bool isFastest = (st.elapsedMs <= fastestMs * 1.05);

        auto makeItem = [&](const QString& text) -> QTableWidgetItem*
        {
            QTableWidgetItem* item = new QTableWidgetItem(text);
            item->setTextAlignment(Qt::AlignCenter);
            if (isFastest)
                item->setForeground(QColor(74, 222, 128));
            return item;
        };

        m_benchmarkTable->setItem(row, 0, makeItem(QString::fromStdString(st.algorithmName)));
        m_benchmarkTable->setItem(row, 1, makeItem(QString::number(st.comparisons)));
        m_benchmarkTable->setItem(row, 2, makeItem(QString::number(st.swapsOrShifts)));
        m_benchmarkTable->setItem(row, 3, makeItem(QString::number(st.elapsedMs, 'f', 3)));
    }

    m_titleLabel->setText("BENCHMARK: ALL 7 ALGORITHMS");
    m_messageLabel->setText(QString("Benchmarked all 7 algorithms silently on %1 elements (green = fastest).")
                                .arg(m_originalData.size()));
}

void MainWindow::updateControlsState()
{
    m_startBtn->setEnabled(!m_isSorting);
    m_algoCombo->setEnabled(!m_isSorting);
    m_randomBtn->setEnabled(!m_isSorting);
    m_manualApplyBtn->setEnabled(!m_isSorting);
    m_benchmarkBtn->setEnabled(!m_isSorting);
    m_sizeSpin->setEnabled(!m_isSorting);
    m_minSpin->setEnabled(!m_isSorting);
    m_maxSpin->setEnabled(!m_isSorting);

    m_pauseBtn->setEnabled(m_isSorting);
    m_pauseBtn->setText(m_isPaused ? "Resume" : "Pause");
    m_stepBtn->setEnabled(m_isSorting && m_isPaused);
    m_stopResetBtn->setText(m_isSorting ? "Stop Sort" : "Reset Array");
}
