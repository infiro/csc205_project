
#include <QtGui>
#include <QtOpenGL>
#include <QDebug>

#include "mainwindow.h"
#include "histogram.h"

MainWindow::MainWindow()
{
    ///////////////////////////////////////////
    // Central widget
    centralWidget = new QWidget;
    setCentralWidget(centralWidget);
    QGridLayout *centralLayout = new QGridLayout;

    // Create Menus and actions
    createActions();
    createMenus();

    // Set Layout for Image
    createImageArea();
    centralLayout->addWidget(InputImageArea , 1,1);
    centralLayout->addWidget(OutputImageArea, 1,2);

    centralLayout->addWidget(InputHistogramArea , 2,1,5,1);
    centralLayout->addWidget(OutputHistogramArea, 2,2,5,1);

    // Buttons
    createButtons();
    QLabel *inputImage  = new QLabel("Input Image");
    QLabel *outputImage = new QLabel("Output Image");
    centralLayout->addWidget(inputImage , 7, 1);
    centralLayout->addWidget(outputImage, 7, 2);
    centralLayout->addWidget(EdgeDetectionBtn, 8,1,1,1);
    centralLayout->addWidget(BlurBtn, 9,1,1,1);
    centralLayout->addWidget(SharpenBtn, 10,1,1,1);
    centralLayout->addWidget(ApplyBtn, 11,2,1,1);

    QGridLayout *CheckBoxLayout = new QGridLayout;
    CheckBoxLayout->addWidget(RecursiveChkbox, 1,1);
    CheckBoxLayout->addWidget(RedChkbox      , 1,2);
    CheckBoxLayout->addWidget(GreenChkbox    , 1,3);
    CheckBoxLayout->addWidget(BlueChkbox     , 1,4);

    centralLayout->addLayout(CheckBoxLayout,11,1,4,1);

    // Filtering Spinbox
    m_filterWidth  = 3;
    m_filterHeight = 3;
    createFilterGrid();
    centralLayout->addLayout(m_FilterLayout, 8,2,m_filterHeight,1);

    // Title
    centralWidget->setLayout(centralLayout);
    setWindowTitle(tr("Triet Huynh - CSC 205 A3 Filtering"));
    resize(800, 600);
}

//////////////////////////////////////////
// create Image Area
void MainWindow::createImageArea()
{
    /////////////////////////////////////////////
    // Original image
    InputImageLabel = new QLabel;
    InputImageLabel->setBackgroundRole(QPalette::Base);
    InputImageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    InputImageLabel->setScaledContents(true);

    OutputImageLabel = new QLabel;
    OutputImageLabel->setBackgroundRole(QPalette::Base);
    OutputImageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    OutputImageLabel->setScaledContents(true);

    // add histogram
    CHistogramWidget *InputHistogram  = m_InputImage .getHistogramWidget();
    CHistogramWidget *OutputHistogram = m_OutputImage.getHistogramWidget();

    // Scrollable in image area
    InputImageArea = new QScrollArea;
    InputImageArea->setBackgroundRole(QPalette::Dark);
    InputImageArea->setWidget(InputImageLabel);
    InputImageArea->setWidgetResizable(true);

    OutputImageArea = new QScrollArea;
    OutputImageArea->setBackgroundRole(QPalette::Dark);
    OutputImageArea->setWidget(OutputImageLabel);
    OutputImageArea->setWidgetResizable(true);

    InputHistogramArea = new QScrollArea;
    InputHistogramArea->setBackgroundRole(QPalette::Dark);
    InputHistogramArea->setWidget(InputHistogram);
    InputHistogramArea->setWidgetResizable(true);

    OutputHistogramArea = new QScrollArea;
    OutputHistogramArea->setBackgroundRole(QPalette::Dark);
    OutputHistogramArea->setWidget(OutputHistogram);
    OutputHistogramArea->setWidgetResizable(true);
}

//////////////////////////////////////////
// create Buttons
void MainWindow::createButtons()
{
    EdgeDetectionBtn = new QPushButton("Edge detection");
    BlurBtn          = new QPushButton("Blur");
    SharpenBtn       = new QPushButton("Sharpen");
    ApplyBtn         = new QPushButton("Apply filter");

    RecursiveChkbox  = new QCheckBox  ("Use output image as source");
    RedChkbox        = new QCheckBox  ("Red channel");
    GreenChkbox      = new QCheckBox  ("Green channel");
    BlueChkbox       = new QCheckBox  ("Blue channel");

    RecursiveChkbox->setCheckState(Qt::Unchecked);
    RedChkbox       ->setCheckState(Qt::Checked);
    GreenChkbox     ->setCheckState(Qt::Checked);
    BlueChkbox      ->setCheckState(Qt::Checked);

    m_bRecursive    = false;
    m_bRedChannel   = true;
    m_bGreenChannel = true;
    m_bBlueChannel  = true;

    connect(EdgeDetectionBtn, SIGNAL(clicked()), this, SLOT(edgeDetection()));
    connect(BlurBtn         , SIGNAL(clicked()), this, SLOT(blur()));
    connect(SharpenBtn      , SIGNAL(clicked()), this, SLOT(sharpen()));
    connect(ApplyBtn        , SIGNAL(clicked()), this, SLOT(onApplyFilterBtn()));
    connect(RecursiveChkbox , SIGNAL(toggled(bool)), this, SLOT(onRecursiveChkbox(bool)));
    connect(RedChkbox       , SIGNAL(toggled(bool)), this, SLOT(onRedChannelChkbox(bool)));
    connect(GreenChkbox     , SIGNAL(toggled(bool)), this, SLOT(onGreenChannelChkbox(bool)));
    connect(BlueChkbox      , SIGNAL(toggled(bool)), this, SLOT(onBlueChannelChkbox(bool)));
}

//////////////////////////////////////////
// create filterlayout
void MainWindow::createFilterGrid()
{
    // Clear Spinbox controls
    for(int i=0; i< m_rgSpinBoxes.size(); i++)
    {
        if(m_rgSpinBoxes[i] != NULL)
            delete m_rgSpinBoxes[i];
    }
    m_rgSpinBoxes.clear();

    // Add new spinbox control
    m_FilterLayout = new QGridLayout;

    // Add filter size
    QSpinBox *FilterWidth  = new QSpinBox;
    QSpinBox *FilterHeight = new QSpinBox;

    FilterWidth->setRange(1,10);
    FilterHeight->setRange(1,10);

    FilterWidth->setValue(3);
    FilterHeight->setValue(3);
    connect(FilterWidth       , SIGNAL(valueChanged(int)), this, SLOT(onFilterWidthChanged(int)));
    connect(FilterHeight      , SIGNAL(valueChanged(int)), this, SLOT(onFilterHeightChanged(int)));

    // Normalized checkbox
    NormalizedChkbox = new QCheckBox("Normalize filter");
    NormalizedChkbox->setCheckState(Qt::Checked);
    m_bNormalized   = true;
    connect(NormalizedChkbox, SIGNAL(toggled(bool)), this, SLOT(onNormalizedChkbox(bool)));

    m_FilterLayout->addWidget(FilterWidth     ,1,1);
    m_FilterLayout->addWidget(FilterHeight    ,1,2);
    m_FilterLayout->addWidget(NormalizedChkbox,1,3);

    // Add filter grid
    for(int i =0; i< m_filterWidth; i++)
    {
        for(int j =0; j<m_filterHeight; j++)
        {
            QDoubleSpinBox *spinBox = new QDoubleSpinBox;
            spinBox->setRange(-200.0f, 200.0f);
            m_rgSpinBoxes.push_back(spinBox);
            m_FilterLayout->addWidget(spinBox,i+2,j+1);
        }
    }
}

/////////////////////////////////////////////////////////////////
// Hook up action to method
// Action == menu
void MainWindow::createActions()
{
    // Open file
    openFileAct = new QAction(tr("&Open Input Image File"), this);
    openFileAct->setShortcut(tr("Ctrl+O"));
    connect(openFileAct, SIGNAL(triggered()), this, SLOT(openFile()));

    // Exit menu
    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcuts(QKeySequence::Close);
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));
}

//////////////////////////////////////////////////////////////
// Hook up menu to actions
void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(openFileAct);

    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);
}

///////////////////////////////////////////////////////////////
// On file open
// Load image and display on screen
void MainWindow::openFile()
{
    filename = QFileDialog::getOpenFileName(this, tr("open file"), "/home",tr("Images (*.png *.tif *.bmp *.jpg)")  );

    if(!filename.isEmpty())
    {
        if(!m_InputImage.load(filename))
        {
            QMessageBox::information(this, tr("Warning!"), tr("Cannot load %1.").arg(filename));
            return;
        }
        else
        {
            InputImageLabel->setPixmap(m_InputImage.getOriginalPixmap());
            InputImageLabel->adjustSize();

            // Copy InputImage to Output Image
            m_OutputImage.load(filename);

            OutputImageLabel->setPixmap(m_InputImage.getOriginalPixmap());
            OutputImageLabel->adjustSize();
        }
    }    
}

////////////////////////////////////////////
// Apply filter
void MainWindow::onApplyFilterBtn()
{
    if(m_OutputImage.isNull())
        return;

    // Get Filter from Spinboxes
    m_rgFilter.clear();
    for(int i=0; i< m_rgSpinBoxes.size(); i++)
    {
        m_rgFilter.push_back(m_rgSpinBoxes[i]->value());
    }

    // Apply filter
    applyFilter();
}

////////////////////////////////////////////
// edge Detection
void MainWindow::edgeDetection()
{
    if(m_OutputImage.isNull())
        return;

    // set up filter
    float filterSize = m_filterWidth*m_filterHeight*1.0f;
    float midPointWeight = -1.0f*(filterSize - 1);
    int midPointIndex;

    //If Height is even, set the mid pixel to be the previous one
    if(m_filterHeight%2 == 0)
        midPointIndex = filterSize/2 -1;
    else
        midPointIndex = filterSize/2;

    m_rgFilter.clear();
    for(int i =0; i< filterSize; i++)
    {
         (i == midPointIndex)?m_rgFilter.push_back(midPointWeight):m_rgFilter.push_back(1.0f);
    }

    // setup Spinbox
    for(int i=0; i< m_rgSpinBoxes.size(); i++)
    {
        (i == midPointIndex)?m_rgSpinBoxes[i]->setValue(midPointWeight):m_rgSpinBoxes[i]->setValue(1.0f);
    }

    // Apply filter
    applyFilter();
}

////////////////////////////////////////////
// blur
// display blur matrix and apply it
void MainWindow::blur()
{
    if(m_OutputImage.isNull())
        return;

    // calculate Blur matrix based on Filtersize
    float filterSize = m_filterWidth*m_filterHeight*1.0f;
    m_rgFilter.clear();
    for(int i =0; i< filterSize; i++)
    {
         m_rgFilter.push_back(1.0f);
    }

    // setup Spinbox
    for(int i=0; i< m_rgSpinBoxes.size(); i++)
    {
        m_rgSpinBoxes[i]->setValue(1.0f);
    }

    // Apply filter
    applyFilter();
}

////////////////////////////////////////////
// Sharpen
void MainWindow::sharpen()
{
    if(m_OutputImage.isNull())
        return;

    // set up filter
    float filterSize = m_filterWidth*m_filterHeight*1.0f;
    float midPoint = filterSize + filterSize - 1;
    int midPointIndex;

    //If Height is even, set the mid pixel to be the previous one
    if(m_filterHeight%2 == 0)
        midPointIndex = filterSize/2 -1;
    else
        midPointIndex = filterSize/2;

    m_rgFilter.clear();
    for(int i =0; i< filterSize; i++)
    {
         //(i == midPointIndex)?m_rgFilter.push_back(midPoint*1.0f/filterSize):m_rgFilter.push_back(-1.0f/filterSize);
        (i == midPointIndex)?m_rgFilter.push_back(midPoint*1.0f):m_rgFilter.push_back(-1.0f);
    }

    // setup Spinbox
    for(int i=0; i< m_rgSpinBoxes.size(); i++)
    {
         //(i == midPointIndex)?m_rgSpinBoxes[i]->setValue(midPoint*1.0f/filterSize):m_rgSpinBoxes[i]->setValue(-1.0f/filterSize);
        (i == midPointIndex)?m_rgSpinBoxes[i]->setValue(midPoint*1.0f):m_rgSpinBoxes[i]->setValue(-1.0f);
    }

    // Apply filter
    applyFilter();
}

///////////////////////////////////////////////////////////////////
// Apply filter to Output image
void MainWindow::applyFilter()
{
    // Normalize filter
    if(m_bNormalized)
        normalizeFilter();

    // override original image
    if(m_bRecursive)
        m_OutputImage.overrideOriginalImage();
    else
        m_OutputImage.setOriginalImage(m_InputImage.getOriginalImage());

    // Apply filter
    m_OutputImage.applyFilter(m_rgFilter,m_filterWidth,m_filterHeight, m_bRedChannel,m_bGreenChannel,m_bBlueChannel);
    OutputImageLabel->setPixmap(m_OutputImage.getGrayPixmap());
    OutputImageLabel->adjustSize();

    // update histogram
    m_OutputImage.updateHistogram();
}

/////////////////////////////////////////////////////////////////////
// Normalized filter
void MainWindow::normalizeFilter()
{
    // get weight
    float weight = 0.0f;
    for(int i =0; i< m_rgFilter.size(); i++)
    {
         weight += m_rgFilter[i];
    }

    if(weight == 0.0f)
        weight = 1.0f;

    // normalize filter
    for(int i =0; i< m_rgFilter.size(); i++)
    {
         m_rgFilter[i] /= weight;
    }
}

///////////////////////////////////////////////////////////////
// GridSize changed
void MainWindow::changedFilterGrid()
{
    // Clear Spinbox controls
    for(int i=0; i< m_rgSpinBoxes.size(); i++)
    {
        if(m_rgSpinBoxes[i] != NULL)
        {
            m_FilterLayout->removeWidget(m_rgSpinBoxes[i]);
            delete m_rgSpinBoxes[i];
        }
    }
    m_rgSpinBoxes.clear();

    // Add filter grid
    for(int i =0; i< m_filterWidth; i++)
    {
        for(int j =0; j<m_filterHeight; j++)
        {
            QDoubleSpinBox *spinBox = new QDoubleSpinBox;
            spinBox->setRange(-200.0f, 200.0f);
            m_rgSpinBoxes.push_back(spinBox);
            m_FilterLayout->addWidget(spinBox,i+2,j+1);
        }
    }
}

//////////////////////////////////////////
// Recursive checkbox changed
void MainWindow::onRecursiveChkbox(bool state)
{
    m_bRecursive = state;
}

void MainWindow::onRedChannelChkbox(bool state)
{
    m_bRedChannel = state;
}

void MainWindow::onGreenChannelChkbox(bool state)
{
    m_bGreenChannel = state;
}

void MainWindow::onBlueChannelChkbox(bool state)
{
    m_bBlueChannel = state;
}

void MainWindow::onNormalizedChkbox(bool state)
{
    m_bNormalized = state;
}
void MainWindow::onFilterWidthChanged(int size)
{
    m_filterWidth = size;
    changedFilterGrid();
}
void MainWindow::onFilterHeightChanged(int size)
{
    m_filterHeight = size;
    changedFilterGrid();
}
