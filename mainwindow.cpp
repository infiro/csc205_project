
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

    //creat Menus and actions
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
    QLabel *selectedRegionLabel = new QLabel("User Selected Region: ");
    RegionBoxLayout = new QGridLayout;
    RegionBoxLayout->addWidget(regionStX, 1,2);
    RegionBoxLayout->addWidget(regionStY, 1,3);
    RegionBoxLayout->addWidget(regionEndX, 1,4);
    RegionBoxLayout->addWidget(regionEndY, 1,5);
    RegionBoxLayout->addWidget(regionLvl, 1,6);
    RegionBoxLayout->addWidget(selectedRegionLabel, 2,1);
    centralLayout->addLayout(RegionBoxLayout,8,1,2,1);

    //Region Spinbox
    createSpinBox();

    // Filtering Spinbox
    m_filterWidth  = 3;
    m_filterHeight = 3;
    createFilterGrid();
    centralLayout->addLayout(m_FilterLayout, 8,2,m_filterHeight,1);

    // Title
    centralWidget->setLayout(centralLayout);
    setWindowTitle(tr("Triet & Ding - CSC 205 Project"));
    resize(800, 600);

    //Show regions
    m_bShowRegion = true;
}

MainWindow::~MainWindow()
{
    for(int i=0; i< m_rgRegions.size(); i++)
    {
        if(m_rgRegions[i]!= NULL)
            delete m_rgRegions[i];
    }
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

    connect(BlurBtn         , SIGNAL(clicked()), this, SLOT(onBlurBtn()));
    connect(SharpenBtn      , SIGNAL(clicked()), this, SLOT(onSharpenBtn()));
    connect(ApplyBtn        , SIGNAL(clicked()), this, SLOT(onApplyFilterBtn()));
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
            spinBox->setRange(-100.0f, 100.0f);
            m_rgSpinBoxes.push_back(spinBox);
            m_FilterLayout->addWidget(spinBox,i+2,j+1);
        }
    }

    // Show region
    QCheckBox* showRegionChkBox  = new QCheckBox("Show regions");
    showRegionChkBox->setCheckState(Qt::Checked);
    connect(showRegionChkBox, SIGNAL(toggled(bool)), this, SLOT(onShowRegionChkbox(bool)));
    m_FilterLayout->addWidget(showRegionChkBox,1,4);
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

    selectedRegionStX->setRange(0, 1000);
    selectedRegionEndX->setRange(0, 1000);
    selectedRegionStY->setRange(0, 1000);
    selectedRegionEndY->setRange(0, 1000);

    //set range for the region'start position and end position
   /* selectedRegionStX->setRange(0, m_InputImage.imageWidth());
    selectedRegionEndX->setRange(0, m_InputImage.imageWidth());
    selectedRegionStY->setRange(0, m_InputImage.imageHeight());
    selectedRegionEndY->setRange(0, m_InputImage.imageHeight());*/
}
////////////////////////////////////////////
// Selected Regions' X, Y, gray&sharpen Level spinbox
void MainWindow::createSpinBox()
{
    selectedRegionStX = new QSpinBox;
    selectedRegionStY = new QSpinBox;
    selectedRegionEndX = new QSpinBox;
    selectedRegionEndY = new QSpinBox;

    selectedRegionLvl = new QSpinBox;
    connect(selectedRegionLvl, SIGNAL(valueChanged(int)), this, SLOT(onSelectedRegionLvlSpinbox()));
    selectedRegionLvl->setRange(0,10);

    RegionBoxLayout->addWidget(selectedRegionStX, 2,2);
    RegionBoxLayout->addWidget(selectedRegionStY, 2,3);
    RegionBoxLayout->addWidget(selectedRegionEndX, 2,4);
    RegionBoxLayout->addWidget(selectedRegionEndY, 2,5);
    RegionBoxLayout->addWidget(selectedRegionLvl, 2,6);
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

    // Apply filter
    applyFilter();
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
        (i == midPointIndex)?m_rgFilter.push_back(midPoint*1.0f):m_rgFilter.push_back(-1.0f);
    }

    // setup Spinbox
    for(int i=0; i< m_rgSpinBoxes.size(); i++)
    {
        (i == midPointIndex)?m_rgSpinBoxes[i]->setValue(midPoint*1.0f):m_rgSpinBoxes[i]->setValue(-1.0f);
    }

    // Apply filter
    applyFilter();
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
    // Normalize filter
    if(m_bNormalized)
        normalizeFilter();

    // override original image
    if(false)
        m_OutputImage.overrideOriginalImage();
    else
        m_OutputImage.setOriginalImage(m_InputImage.getOriginalImage());

    // Apply filter
    m_OutputImage.applyFilter(m_rgFilter,m_filterWidth,m_filterHeight, true,true,true);
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
            spinBox->setRange(-100.0f, 100.0f);
            m_rgSpinBoxes.push_back(spinBox);
            m_FilterLayout->addWidget(spinBox,i+2,j+1);
        }
    }
}

//////////////////////////////////////////
//Mouse Event

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        m_firstPos = event->pos();
        m_lastPos  = event->pos();

        //Find selected region
        m_selectedRegion = NULL;
        for(int i=0; i<m_rgRegions.size(); i++)
        {
            if(m_rgRegions[i]->containsPoint(m_firstPos))
            {
                m_selectedRegion = m_rgRegions[i];
                break;
            }
        }
    }

}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        if(m_selectedRegion!=NULL)
        {
            // move selected region
            int dx = event->pos().x() - m_lastPos.x();
            int dy = event->pos().y() - m_lastPos.y();
            m_selectedRegion->moveTo(dx,dy);
            m_selectedRegion->show(m_bShowRegion);
        }
        else // In draw mode
        {
            // Draw intermediate region
            CRegion newRegion(m_firstPos, m_lastPos, RGBColor(1.0f,1.0f,1.0f),InputImageLabel);
            newRegion.show(m_bShowRegion);
        }
    }

    m_lastPos = event->pos();
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    // Release to add new point
    if (event->button()== Qt::LeftButton)
    {
        if(m_selectedRegion == NULL)
        {
            // Create new region
            CRegion* newRegion = new CRegion(m_firstPos, m_lastPos, RGBColor(1.0f,1.0f,1.0f),InputImageLabel);
            m_rgRegions.push_back(newRegion);
            m_selectedRegion = newRegion;
            //update the current selected region's end position
            selectedRegionEndX->setValue(m_lastPos.x());
            selectedRegionEndY->setValue(m_lastPos.y());
        }
        else
        {
            // move selected region
            int dx = event->pos().x() - m_lastPos.x();
            int dy = event->pos().y() - m_lastPos.y();
            m_selectedRegion->moveTo(dx,dy);
        }

        //display the current selected region's start and end position
        selectedRegionStX->setValue(m_selectedRegion->getTopLeft().x());
        selectedRegionStY->setValue(m_selectedRegion->getTopLeft().y());
        selectedRegionEndX->setValue(m_selectedRegion->getBotRight().x());
        selectedRegionEndY->setValue(m_selectedRegion->getBotRight().y());
        selectedRegionLvl->setValue(m_selectedRegion->Z_Level());
    }
    else // Right click to Delete the previous point
    if(event->button() == Qt::RightButton)
    {
        // Delete last region
        if(m_rgRegions.size() > 0)
        {
            if(m_selectedRegion == m_rgRegions[m_rgRegions.size() -1])
                m_selectedRegion = NULL;

            delete m_rgRegions[m_rgRegions.size() -1];
            m_rgRegions.pop_back();
        }

        if(m_rgRegions.size() != 0)
        {
            //if the selected region is null
            if (m_selectedRegion == NULL)
                m_selectedRegion = m_rgRegions[m_rgRegions.size() -1];

            //display the current selected region's start and end position
            selectedRegionStX->setValue(m_selectedRegion->getTopLeft().x());
            selectedRegionStY->setValue(m_selectedRegion->getTopLeft().y());
            selectedRegionEndX->setValue(m_selectedRegion->getBotRight().x());
            selectedRegionEndY->setValue(m_selectedRegion->getBotRight().y());
            selectedRegionLvl->setValue(m_selectedRegion->Z_Level());
        } else {
                //display the start and end position to 0
                selectedRegionStX->setValue(0);
                selectedRegionStY->setValue(0);
                selectedRegionEndX->setValue(0);
                selectedRegionEndY->setValue(0);
                selectedRegionLvl->setValue(0);
        }
    }

    updateRegions();
    m_lastPos = event->pos();
}

//////////////////////////////////////////
// Region checkbox changed

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

void MainWindow::updateRegions()
{
    for(int i=0; i< m_rgRegions.size(); i++)
    {
        m_rgRegions[i]->show(m_bShowRegion);
    }
}

void MainWindow::onShowRegionChkbox(bool state)
{
    m_bShowRegion = state;
    updateRegions();
}

void MainWindow::onSelectedRegionLvlSpinbox()
{
    if (m_selectedRegion != NULL)
    {
        m_selectedRegion->Z_Level(selectedRegionLvl->value());
    }
}
