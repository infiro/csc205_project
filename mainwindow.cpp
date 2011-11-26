
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
    centralLayout->addWidget(BlurBtn, 11, 1);
    centralLayout->addWidget(SharpenBtn, 12, 1);
    centralLayout->addWidget(ApplyBtn, 11,2,1,1);

    QLabel *regionStX = new QLabel("Start X");
    QLabel *regionStY = new QLabel("Start Y");
    QLabel *regionEndX = new QLabel("End X");
    QLabel *regionEndY = new QLabel("End Y");
    QLabel *regionLvl = new QLabel("Region Level");
    RegionBoxLayout = new QGridLayout;
    RegionBoxLayout->addWidget(regionStX, 1,2);
    RegionBoxLayout->addWidget(regionStY, 1,3);
    RegionBoxLayout->addWidget(regionEndX, 1,4);
    RegionBoxLayout->addWidget(regionEndY, 1,5);
    RegionBoxLayout->addWidget(regionLvl, 1,6);
    RegionBoxLayout->addWidget(region1, 2,1);
    RegionBoxLayout->addWidget(region2, 3,1);
    RegionBoxLayout->addWidget(region3, 4,1);
    centralLayout->addLayout(RegionBoxLayout,8,1,3,1);

    // Filtering Spinbox
    m_filterWidth  = 3;
    m_filterHeight = 3;
    createFilterGrid();
    centralLayout->addLayout(m_FilterLayout, 8,2,m_filterHeight,1);

    // Title
    centralWidget->setLayout(centralLayout);
    setWindowTitle(tr("Triet & Ding - CSC 205 Project"));
    resize(800, 600);

    rubberBand_reg1 = NULL;
    rubberBand_reg2 = NULL;
    rubberBand_reg3 = NULL;
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
    BlurBtn          = new QPushButton("Blur filter");
    SharpenBtn       = new QPushButton("Sharpen filter");
    ApplyBtn         = new QPushButton("Apply filter");

    region1          = new QCheckBox  ("user defind region 1");
    region2          = new QCheckBox  ("user defind region 2");
    region3          = new QCheckBox  ("user defind region 3");

    region1         ->setCheckState(Qt::Unchecked);
    region2         ->setCheckState(Qt::Unchecked);
    region3         ->setCheckState(Qt::Unchecked);

    m_region1         = false;
    m_region2         = false;
    m_region3         = false;

    connect(BlurBtn         , SIGNAL(clicked()), this, SLOT(onBlurBtn()));
    connect(SharpenBtn      , SIGNAL(clicked()), this, SLOT(onSharpenBtn()));
    connect(ApplyBtn        , SIGNAL(clicked()), this, SLOT(onApplyFilterBtn()));
    connect(region1         , SIGNAL(toggled(bool)), this, SLOT(onRegion1Chkbox(bool)));
    connect(region2         , SIGNAL(toggled(bool)), this, SLOT(onRegion2Chkbox(bool)));
    connect(region3         , SIGNAL(toggled(bool)), this, SLOT(onRegion3Chkbox(bool)));
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
            spinBox->setRange(-10.0f, 10.0f);
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
    createSpinBox();
}
////////////////////////////////////////////
// Regions' X, Y, gray&sharpen Level spinbox
void MainWindow::createSpinBox()
{
    oneStX = new QSpinBox;
    twoStX = new QSpinBox;
    threeStX = new QSpinBox;
    oneStY = new QSpinBox;
    twoStY = new QSpinBox;
    threeStY = new QSpinBox;
    oneEndX = new QSpinBox;
    twoEndX = new QSpinBox;
    threeEndX = new QSpinBox;
    oneEndY = new QSpinBox;
    twoEndY = new QSpinBox;
    threeEndY = new QSpinBox;

    oneStX->setRange(0, m_InputImage.imageWidth());
    twoStX->setRange(0, m_InputImage.imageWidth());
    threeStX->setRange(0, m_InputImage.imageWidth());
    oneEndX->setRange(0, m_InputImage.imageWidth());
    twoEndX->setRange(0, m_InputImage.imageWidth());
    threeEndX->setRange(0, m_InputImage.imageWidth());
    oneStY->setRange(0, m_InputImage.imageHeight());
    twoStY->setRange(0, m_InputImage.imageHeight());
    threeStY->setRange(0, m_InputImage.imageHeight());
    oneEndY->setRange(0, m_InputImage.imageHeight());
    twoEndY->setRange(0, m_InputImage.imageHeight());
    threeEndY->setRange(0, m_InputImage.imageHeight());

    RegionBoxLayout->addWidget(oneStX , 2,2);
    RegionBoxLayout->addWidget(twoStX , 3,2);
    RegionBoxLayout->addWidget(threeStX , 4,2);
    RegionBoxLayout->addWidget(oneStY , 2,3);
    RegionBoxLayout->addWidget(twoStY , 3,3);
    RegionBoxLayout->addWidget(threeStY , 4,3);
    RegionBoxLayout->addWidget(oneEndX , 2,4);
    RegionBoxLayout->addWidget(twoEndX , 3,4);
    RegionBoxLayout->addWidget(threeEndX , 4,4);
    RegionBoxLayout->addWidget(oneEndY , 2,5);
    RegionBoxLayout->addWidget(twoEndY , 3,5);
    RegionBoxLayout->addWidget(threeEndY , 4,5);

    for(int i = 0; i < 3; i++)
    {
        QSpinBox *Level = new QSpinBox;
        Level->setRange(0, 100);
        RegionBoxLayout->addWidget(Level , 2+i,6);
    }
}
////////////////////////////////////////////
// Display the Blur filter
void MainWindow::onBlurBtn()
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
}

////////////////////////////////////////////
// Display the Sharpen filter
void MainWindow::onSharpenBtn()
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



///////////////////////////////////////////////////////////////////
// Apply filter to Output image
void MainWindow::applyFilter()
{

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
            spinBox->setRange(-10.0f, 10.0f);
            m_rgSpinBoxes.push_back(spinBox);
            m_FilterLayout->addWidget(spinBox,i+2,j+1);
        }
    }
}

//////////////////////////////////////////
//Mouse Event

void MainWindow::mousePressEvent(QMouseEvent *e)
{
    if(m_region1 && !rubberBand_reg1){
        rubberBand_reg1 = new QRubberBand(QRubberBand::Rectangle, InputImageLabel);
    }else if (m_region2 && !rubberBand_reg2){
        rubberBand_reg2 = new QRubberBand(QRubberBand::Rectangle, InputImageLabel);
    }else if (m_region3 && !rubberBand_reg3){
        rubberBand_reg3 = new QRubberBand(QRubberBand::Rectangle, InputImageLabel);
    }

    if(rubberBand_reg1){
        origin_reg1 = e->pos();
        rubberBand_reg1->setGeometry(QRect(origin_reg1, QSize()));
        rubberBand_reg1->show();
        oneStX->setValue(origin_reg1.x());
        oneStY->setValue(origin_reg1.y());
    }
    if(rubberBand_reg2){
        origin_reg2 = e->pos();
        rubberBand_reg2->setGeometry(QRect(origin_reg2, QSize()));
        rubberBand_reg2->show();
        twoStX->setValue(origin_reg2.x());
        twoStY->setValue(origin_reg2.y());
    }
    if(rubberBand_reg3){
        origin_reg3 = e->pos();
        rubberBand_reg3->setGeometry(QRect(origin_reg3, QSize()));
        rubberBand_reg3->show();
        threeStX->setValue(origin_reg3.x());
        threeStY->setValue(origin_reg3.y());
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *e)
{
    if(rubberBand_reg1)
    {
        rubberBand_reg1->setGeometry(QRect(origin_reg1, e->pos()).normalized());
    }
    if(rubberBand_reg2)
    {
        rubberBand_reg2->setGeometry(QRect(origin_reg2, e->pos()).normalized());
    }
    if(rubberBand_reg3)
    {
        rubberBand_reg3->setGeometry(QRect(origin_reg3, e->pos()).normalized());
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *e)
{
    if(rubberBand_reg1)
    {
        rubberBand_reg1->hide();
        rubberBand_reg1 = NULL;
        oneEndX->setValue(e->x());
        oneEndY->setValue(e->y());
    }
    if(rubberBand_reg2)
    {
        rubberBand_reg2->hide();
        rubberBand_reg2 = NULL;
        twoEndX->setValue(e->x());
        twoEndY->setValue(e->y());
    }
    if(rubberBand_reg3)
    {
        rubberBand_reg3->hide();
        rubberBand_reg3 = NULL;
        threeEndX->setValue(e->x());
        threeEndY->setValue(e->y());
    }
}

//////////////////////////////////////////
// Region checkbox changed
void MainWindow::onRegion1Chkbox(bool state)
{
    m_region1 = state;
}

void MainWindow::onRegion2Chkbox(bool state)
{
    m_region2 = state;
}

void MainWindow::onRegion3Chkbox(bool state)
{
    m_region3 = state;
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
