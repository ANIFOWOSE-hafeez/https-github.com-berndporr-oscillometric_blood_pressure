
#include <qwt/qwt_dial_needle.h>
#include <iostream>
#include "Window.h"

Window::Window(Processing *process, QWidget *parent) :
        dataLength(MAX_DATA_LENGTH),
        QMainWindow(parent) {

    for (int i = 0; i < MAX_DATA_LENGTH; i++) {
        xData[i] = (double) (MAX_DATA_LENGTH - i) / (double) SAMPLING_RATE;     // time axis in seconds
        yLPData[i] = 0;
        yHPData[i] = 0;
    }

    valHeartRate = 0.0;
    setupUi(this);
    this->process = process;

    eSwitchScreen(Screen::startScreen);
    // Generate timer event every 50ms to update the window
    (void) startTimer(50);
}

Window::~Window() {

    PLOG_VERBOSE << "Cleanup:";
    process->stopThread();
    process->join();
    PLOG_VERBOSE << "Application terminated.";
}

/**
 * This functions is to be called at the initialisation of the object.
 * It builds the whole user interface.
 * @param window  A reference to the parent object.
 */
void Window::setupUi(QMainWindow *window) {
    if (window->objectName().isEmpty())
        window->setObjectName(QString::fromUtf8("MainWindow"));

    QIcon icon(QIcon::fromTheme(QString::fromUtf8("Main")));
    window->resize(1920, 1080);
    window->setAcceptDrops(true);
    window->setWindowIcon(icon);
    window->setTabShape(QTabWidget::Triangular);

    //TODO: splitter more visible.
    //TODO: minimum width for left and right side
    splitter = new QSplitter(window);
    splitter->setObjectName(QString::fromUtf8("splitter"));
    splitter->setOrientation(Qt::Horizontal);
    splitter->setHandleWidth(5);
    splitter->setChildrenCollapsible(false);

    // The left side is a stacked widget with several pages in a vertical box
    // layout with some permanent information on the bottom.
    lWidget = new QWidget(splitter);
    vlLeft = new QVBoxLayout(lWidget);
    vlLeft->setObjectName(QString::fromUtf8("vlLeft"));
    lInstructions = new QStackedWidget(lWidget);
    lInstructions->setMinimumWidth(500);

    btnCancel = new QPushButton(lWidget);
    btnCancel->setObjectName(QString::fromUtf8("btnCancel"));

    lMeter = new QLabel(lWidget);
    lMeter->setObjectName(QString::fromUtf8("lMeter"));
    lMeter->setAlignment(Qt::AlignCenter);
    meter = new QwtDial(lWidget);
    meter->setObjectName(QString::fromUtf8("meter"));
    meter->setUpperBound(260.000000000000000);
    meter->setScaleStepSize(20.000000000000000);
    meter->setWrapping(false);
    meter->setInvertedControls(false);
    meter->setLineWidth(4);
    meter->setMode(QwtDial::RotateNeedle);
    meter->setMinScaleArc(20.000000000000000);
    meter->setMaxScaleArc(340.000000000000000);
    meter->setMinimumSize(400, 400);
    needle = new QwtDialSimpleNeedle(
            QwtDialSimpleNeedle::Arrow, true, Qt::black,
            QColor(Qt::gray).lighter(130));
    meter->setNeedle(needle);

    // Build pages and add them to the instructions panel
    lInstructions->addWidget(setupStartPage(lInstructions));
    lInstructions->addWidget(setupInflatePage(lInstructions));
    lInstructions->addWidget(setupDeflatePage(lInstructions));
    lInstructions->addWidget(setupEmptyCuffPage(lInstructions));
    lInstructions->addWidget(setupResultPage(lInstructions));

    // Add the instructions panel to the splitter
    vlLeft->addWidget(lMeter);
    vlLeft->addWidget(meter);
    vlLeft->addWidget(lInstructions);
    vlLeft->addWidget(btnCancel);
    btnCancel->hide();
    splitter->addWidget(lWidget);

    // Build and add the plot panel to the splitter
    splitter->addWidget(setupPlots(splitter));
    // Set stretch factor of left part to zero so it will not resize
    splitter->setStretchFactor(0, 0);

    // Add splitter to main window.
    window->setCentralWidget(splitter);

    // TODO: menubar working? just missing content?
    menubar = new QMenuBar(window);
    menubar->setObjectName(QString::fromUtf8("menubar"));
    menubar->setGeometry(QRect(0, 0, 2081, 39));
    window->setMenuBar(menubar);
    statusbar = new QStatusBar(window);
    statusbar->setObjectName(QString::fromUtf8("statusbar"));
    window->setStatusBar(statusbar);

    // Set all text fields in one place.
    retranslateUi(window);

    // Connect UI events (slots)
    QMetaObject::connectSlotsByName(window);

    // Set default look, specify percentage of left side:
    double leftSide = 0.3;
    QList<int> Sizes({(int) (leftSide * width()), (int) ((1.0 - leftSide) * width())});
    splitter->setSizes(Sizes);


    //TODO: only for now to be able to switch between the pages of the stacked widget
    auto *but0 = new QPushButton("0");
    auto *but1 = new QPushButton("1");
    auto *but2 = new QPushButton("2");
    auto *but3 = new QPushButton("3");
    auto *but4 = new QPushButton("4");

    statusbar->addPermanentWidget(but0);
    statusbar->addPermanentWidget(but1);
    statusbar->addPermanentWidget(but2);
    statusbar->addPermanentWidget(but3);
    statusbar->addPermanentWidget(but4);
    auto *spacerbnt = new QLabel(); // fake spacer
    statusbar->addPermanentWidget(spacerbnt, 1);

    connect(btnStart, SIGNAL (released()), this, SLOT (clkBtnStart()));
    connect(btnCancel, SIGNAL (released()), this, SLOT (clkBtnCancel()));
    connect(btnReset, SIGNAL (released()), this, SLOT (clkBtnReset()));

    connect(but0, SIGNAL (released()), this, SLOT (clkBtn1()));
    connect(but1, SIGNAL (released()), this, SLOT (clkBtn2()));
    connect(but2, SIGNAL (released()), this, SLOT (clkBtn3()));
    connect(but3, SIGNAL (released()), this, SLOT (clkBtn4()));
    connect(but4, SIGNAL (released()), this, SLOT (clkBtn5()));

}


QWidget *Window::setupPlots(QWidget *parent) {
    rWidget = new QWidget(parent);

    vlRight = new QVBoxLayout();
    vlRight->setObjectName(QString::fromUtf8("vlRight"));

    lTitlePlotRaw = new QLabel(parent);
    lTitlePlotRaw->setObjectName(QString::fromUtf8("lTitlePlotRaw"));
    lTitlePlotOsc = new QLabel(parent);
    lTitlePlotOsc->setObjectName(QString::fromUtf8("lTitlePlotOsc"));

    line = new QFrame(parent);
    line->setObjectName(QString::fromUtf8("line"));
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);

    pltPre = new Plot(xData, yLPData, dataLength, 250, 0.0, parent);
    pltPre->setObjectName(QString::fromUtf8("pltPre"));
    pltOsc = new Plot(xData, yHPData, dataLength, 0.003, -0.003, parent);
    pltOsc->setObjectName(QString::fromUtf8("pltOsc"));

    // build right side of window
    vlRight->addWidget(lTitlePlotRaw);
    vlRight->addWidget(pltPre);
    vlRight->addWidget(line);
    vlRight->addItem(vSpace5);
    vlRight->addWidget(lTitlePlotOsc);
    vlRight->addWidget(pltOsc);
    rWidget->setLayout(vlRight);

    return rWidget;

}


QWidget *Window::setupStartPage(QWidget *parent) {
    lInstrStart = new QWidget(parent);

    vlStart = new QVBoxLayout();
    vlStart->setObjectName(QString::fromUtf8("vlStart"));

    lInfoStart = new QLabel(parent);
    lInfoStart->setObjectName(QString::fromUtf8("lInfoStart"));
    lInfoStart->setWordWrap(true);
    lInfoStart->setAlignment(Qt::AlignCenter);

    btnStart = new QPushButton(parent);
    btnStart->setObjectName(QString::fromUtf8("btnStart"));
    btnStart->setDisabled(true);

    vSpace4 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    vSpace6 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

    vlStart->addItem(vSpace4);
    vlStart->addWidget(lInfoStart);
    vlStart->addItem(vSpace6);
    vlStart->addWidget(btnStart);

    lInstrStart->setLayout(vlStart);
    return lInstrStart;

}


QWidget *Window::setupInflatePage(QWidget *parent) {

    lInstrPump = new QWidget(parent);

    // Layout for this page:
    vlInflate = new QVBoxLayout();
    vlInflate->setObjectName(QString::fromUtf8("vlInflate"));

    vSpace1 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    vSpace2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    vSpace5 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

    lInfoPump = new QLabel(parent);
    lInfoPump->setObjectName(QString::fromUtf8("lInfoPump"));
    lInfoPump->setWordWrap(true);
    lInfoPump->setAlignment(Qt::AlignCenter);

    // build left side of window
    vlInflate->addItem(vSpace1);
    vlInflate->addWidget(lInfoPump);
    vlInflate->addItem(vSpace2);

    lInstrPump->setLayout(vlInflate);
    return lInstrPump;
}


QWidget *Window::setupDeflatePage(QWidget *parent) {

    lInstrRelease = new QWidget(parent);

    vlRelease = new QVBoxLayout();
    lInfoRelease = new QLabel(parent);
    lInfoRelease->setObjectName(QString::fromUtf8("lInfoRelease"));
    lInfoRelease->setWordWrap(true);
    lInfoRelease->setAlignment(Qt::AlignCenter);
    lheartRate = new QLabel(parent);
    lheartRate->setObjectName(QString::fromUtf8("lheartRate"));
    lheartRate->setAlignment(Qt::AlignCenter);

    vSpace4 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    vSpace6 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

    vlRelease->addItem(vSpace6);
    vlRelease->addWidget(lInfoRelease);
    vlRelease->addItem(vSpace4);
    vlRelease->addWidget(lheartRate);

    lInstrRelease->setLayout(vlRelease);
    return lInstrRelease;
}

QWidget *Window::setupEmptyCuffPage(QWidget *parent) {
    lInstrDeflate = new QWidget(parent);

    vlDeflate = new QVBoxLayout();
    lInfoDeflate = new QLabel(parent);
    lInfoDeflate->setObjectName(QString::fromUtf8("lInfoDeflate"));
    lInfoDeflate->setWordWrap(true);
    lInfoDeflate->setAlignment(Qt::AlignCenter);

    vlDeflate->addWidget(lInfoDeflate);
    lInstrDeflate->setLayout(vlDeflate);
    return lInstrDeflate;
}

QWidget *Window::setupResultPage(QWidget *parent) {
    lInstrResult = new QWidget(parent);

    vlResult = new QVBoxLayout();
    lInfoResult = new QLabel(parent);
    lInfoResult->setObjectName(QString::fromUtf8("lInfoDeflate"));
    lInfoResult->setWordWrap(true);
    lInfoResult->setAlignment(Qt::AlignCenter);

    btnReset = new QPushButton(parent);
    btnReset->setObjectName(QString::fromUtf8("btnReset"));

    flResults = new QFormLayout();
    flResults->setObjectName(QString::fromUtf8("flResults"));
    lMAP = new QLabel(parent);
    lMAP->setObjectName(QString::fromUtf8("lMAP"));
    lMAPval = new QLabel(parent);
    lMAPval->setObjectName(QString::fromUtf8("lMAPval"));
    lSBP = new QLabel(parent);
    lSBP->setObjectName(QString::fromUtf8("lSBP"));
    lSBPval = new QLabel(parent);
    lSBPval->setObjectName(QString::fromUtf8("lSBPval"));
    lCBP = new QLabel(parent);
    lCBP->setObjectName(QString::fromUtf8("lCBP"));
    lDBPval = new QLabel(parent);
    lDBPval->setObjectName(QString::fromUtf8("lDBPval"));
    lheartRateAV = new QLabel(parent);
    lheartRateAV->setObjectName(QString::fromUtf8("lheartRateAV"));
    lheartRateAV->setMinimumWidth(250);
    lHRvalAV = new QLabel(parent);
    lHRvalAV->setObjectName(QString::fromUtf8("lHRvalAV"));
    lHRvalAV->setMinimumWidth(150);

    flResults->setWidget(0, QFormLayout::LabelRole, lMAP);
    flResults->setWidget(0, QFormLayout::FieldRole, lMAPval);
    flResults->setWidget(1, QFormLayout::LabelRole, lSBP);
    flResults->setWidget(1, QFormLayout::FieldRole, lSBPval);
    flResults->setWidget(2, QFormLayout::LabelRole, lCBP);
    flResults->setWidget(2, QFormLayout::FieldRole, lDBPval);
    flResults->setWidget(3, QFormLayout::LabelRole, lheartRateAV);
    flResults->setWidget(3, QFormLayout::FieldRole, lHRvalAV);
//    flResults->setContentsMargins(5, 0, 5, 0);

    vlResult->addWidget(lInfoResult);
    vlResult->addLayout(flResults);
    vlResult->addWidget(btnReset);
    lInstrResult->setLayout(vlResult);

    return lInstrResult;
}


void Window::retranslateUi(QMainWindow *window) {
    window->setWindowTitle(QApplication::translate("TestWindow", "TestWindow", nullptr));

    lInfoStart->setText("<b>Prepare the measurement:</b><br><br>"
                        "1. Put the cuff on your upper arm of your nondominant hand, making sure it is tight.<br>"
                        "2. Rest your arm on a flat surface.<br>"
                        "3. Take the pump into your dominant hand.<br>"
                        "4. Make sure the valve is closed, but you can handle it easily.<br>"
                        "5. Press Start when you are ready.");
                        //"<br><br> <i>Picture missing</i><br>"
    lInfoPump->setText("<b>Pump Up to 180 mmHg</b><br><br>"
                       "Using your dominant hand, where your arm is not in the cuff, quickly pump up the cuff to 180 mmHg.<br>"
                       "Make sure the valve is fully closed.<br>"
                       "Use the dial above for reference.");
    lInfoRelease->setText("<b>Slowly release pressure at 3 mmHg/s</b><br><br>"
                          "Open the valve slightly to release pressure at about 3 mmHg per second."
                          "Wait calmly and try not to move. <br><br>");
                          //"<i>Add deflation feedback. Possibly have meter here, too.</i>"
    lInfoDeflate->setText("<b>Completely open the valve.</b><br><br>"
                          "Wait for the pressure to go down to 0 mmHg.<br><br>"
                          "You will see the results next.");
    lInfoResult->setText("<b>Results:</b><br><br>"
                         "Click Reset to start a new measurement<br>");

    lMeter->setText("<b>Pressure in mmHg:</b>");
    btnStart->setText("Start");
    btnReset->setText("Reset");
    btnCancel->setText("Cancel");
    lMAP->setText("MAP:");
    lMAPval->setText("- mmHg");
    lSBP->setText("SBP:");
    lSBPval->setText("- mmHg");
    lCBP->setText("DBP:");
    lDBPval->setText("- mmHg");
    lheartRate->setText("Current heart rate:<br><b>--</b>");
    lheartRateAV->setText("Average heart rate:");
    lHRvalAV->setText("- beats/min");

}


void Window::timerEvent(QTimerEvent *) {
    pltOsc->replot();
    pltPre->replot();
    meter->repaint();


    switch (currentScreen) {
        case Screen::startScreen:
            lInstructions->setCurrentIndex(0);
            break;
        case Screen::inflateScreen:
            lInstructions->setCurrentIndex(1);
            break;
        case Screen::deflateScreen:
            lInstructions->setCurrentIndex(2);
            break;
        case Screen::emptyCuffScreen:
            lInstructions->setCurrentIndex(3);
            break;
        case Screen::resultScreen:
            lInstructions->setCurrentIndex(4);
            break;
    }
}

void Window::eNewData(double pData, double oData) {
    pltPre->setNewData(pData);
    pltOsc->setNewData(oData);
    meter->setValue(pData); //TODO: maybe meter should be shown all the time?
}

void Window::eSwitchScreen(Screen eScreen) {

    switch (eScreen) {
        case Screen::startScreen:
            btnCancel->hide();
            break;
        case Screen::inflateScreen:
            btnCancel->show();
            break;
        case Screen::deflateScreen:
            btnCancel->show();
            if (valHeartRate != 0) {
                //TODO only needs to be done if new heart rate is available.
                lheartRate->setText("Current heart rate:<br><b>" + QString::number(valHeartRate, 'f', 0) + "</b>");
            }
            break;
        case Screen::emptyCuffScreen:
            btnCancel->show();
            break;
        case Screen::resultScreen:
            btnCancel->hide();
            if (valHeartRate != 0) {
                //TODO only needs to be done if new heart rate is available.
                lHRvalAV->setText(QString::number(valHeartRate, 'f', 0) + " beats/min");
            }
            break;
    }

    currentScreen = eScreen;
}

void Window::eResults(double map, double sbp, double dbp) {
    lMAPval->setText(QString::number(map) + " mmHg");
    lSBPval->setText(QString::number(sbp) + " mmHg");
    lDBPval->setText(QString::number(dbp) + " mmHg");
}

void Window::eHeartRate(double heartRate) {
    this->valHeartRate = heartRate;
}

void Window::eReady() {
    btnStart->setDisabled(false);
}

void Window::clkBtnStart() {
    eSwitchScreen(Screen::inflateScreen);
    process->startMeasurement();
}

void Window::clkBtnCancel() {
    eSwitchScreen(Screen::startScreen);
    process->stopMeasurement(); //TODO: make safe
}

void Window::clkBtnReset() {
    eSwitchScreen(Screen::startScreen);
}

//TODO: remove those after debugging
void Window::clkBtn1() {
    eSwitchScreen(Screen::startScreen);
}

void Window::clkBtn2() {
    eSwitchScreen(Screen::inflateScreen);
}

void Window::clkBtn3() {
    eSwitchScreen(Screen::deflateScreen);
}

void Window::clkBtn4() {
    eSwitchScreen(Screen::emptyCuffScreen);

}

void Window::clkBtn5() {
    eSwitchScreen(Screen::resultScreen);
}