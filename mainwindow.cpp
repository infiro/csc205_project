
#include <QtGui>
#include <QtOpenGL>
#include <QDebug>

#include "mainwindow.h"
#include "histogram.h"

///////////////////////////////////////////////
// Serialization
QDataStream & operator << ( QDataStream & out, const CRegion & region)
{
    out << region.getCenter()
        << region.getOrigin()
        << region.getEndPoint()
        << region.Z_Level()
        << region.getTopLeft()
        << region.getTopRight()
        << region.getBotLeft()
        << region.getBotRight();

    return (out);
}

QDataStream & operator >> ( QDataStream & in, CRegion & region )
{
    QPoint i_center;
    QPoint i_origin;
    QPoint i_endPoint;
    int z_level;
    QPoint i_topLeft;
    QPoint i_topRight;
    QPoint i_botLeft;
    QPoint i_botRight;

    in >> i_center
       >> i_origin
       >> i_endPoint
       >> z_level
       >> i_topLeft
       >> i_topRight
       >> i_botLeft
       >> i_botRight;

    region.center(i_center);
    region.origin(i_origin);
    region.endpoint(i_endPoint);
    region.Z_Level(z_level);
    region.topleft(i_topLeft);
    region.topright(i_topRight);
    region.botleft(i_botLeft);
    region.botright(i_botRight);

    return (in);
}

QDataStream & operator << ( QDataStream & out , const CImage & image)
{
    out << image.getName()
        << image.getOriginalImage();

    return (out);
}

QDataStream & operator >>( QDataStream & in , CImage & image)
{
    QString fileName;
    QImage inputImage;

    in >> fileName
       >> inputImage;

    image.setOriginalImage(inputImage);

    return (in);
}

MainWindow::MainWindow()
{
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
    centralLayout->addWidget(ResetBtn, 11, 1);
    centralLayout->addWidget(ApplyBtn, 11,2,1,1);

    //Region Spinbox
    createSpinBox();
    createFilterGrid();
    centralLayout->addLayout(RegionBoxLayout,8,1,2,1);
    centralLayout->addLayout(m_FilterLayout, 8,2,1,1);

    // Title
    centralWidget->setLayout(centralLayout);
    setWindowTitle(tr("Triet & Ding - CSC 205 Project"));
    resize(800, 600);
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
    ResetBtn         = new QPushButton("Reset");
    ApplyBtn         = new QPushButton("Apply");
    connect(ResetBtn , SIGNAL(clicked()), this, SLOT(onResetBtn()));
    connect(ApplyBtn, SIGNAL(clicked()), this, SLOT(onApplyFilterBtn()));
}

////////////////////////////////////////////////////////
// Selected Regions' X, Y, gray&sharpen Level spinbox
void MainWindow::createSpinBox()
{
    RegionBoxLayout = new QGridLayout;

    // Label
    QLabel *regionStX = new QLabel("Left");
    QLabel *regionStY = new QLabel("Top");
    QLabel *regionEndX = new QLabel("Right");
    QLabel *regionEndY = new QLabel("Bot");
    QLabel *regionLvl = new QLabel("Depth(-10 to 10)");
    QLabel *selectedRegionLabel = new QLabel("Selected Region: ");

    RegionBoxLayout->addWidget(regionStX, 1,2);
    RegionBoxLayout->addWidget(regionStY, 1,3);
    RegionBoxLayout->addWidget(regionEndX, 1,4);
    RegionBoxLayout->addWidget(regionEndY, 1,5);
    RegionBoxLayout->addWidget(regionLvl, 1,6);
    RegionBoxLayout->addWidget(selectedRegionLabel, 2,1);

    // Spinboxed
    m_selectedRegionStX  = new QSpinBox;
    m_selectedRegionStY  = new QSpinBox;
    m_selectedRegionEndX = new QSpinBox;
    m_selectedRegionEndY = new QSpinBox;
    m_selectedRegionLvl  = new QSpinBox;

    m_selectedRegionStX ->setRange(0, 1000);
    m_selectedRegionEndX->setRange(0, 1000);
    m_selectedRegionStY ->setRange(0, 1000);
    m_selectedRegionEndY->setRange(0, 1000);
    m_selectedRegionLvl ->setRange(-10,10);

    RegionBoxLayout->addWidget(m_selectedRegionStX, 2,2);
    RegionBoxLayout->addWidget(m_selectedRegionStY, 2,3);
    RegionBoxLayout->addWidget(m_selectedRegionEndX, 2,4);
    RegionBoxLayout->addWidget(m_selectedRegionEndY, 2,5);
    RegionBoxLayout->addWidget(m_selectedRegionLvl, 2,6);

    // Show region
    m_showRegionChkbox  = new QCheckBox("Show regions");
    m_showRegionChkbox->setCheckState(Qt::Checked);
    connect(m_showRegionChkbox, SIGNAL(toggled(bool)), this, SLOT(onShowRegionChkbox(bool)));
    RegionBoxLayout->addWidget(m_showRegionChkbox,1,1);
    m_bShowRegion = true;

    // Connect action
    connect(m_selectedRegionLvl , SIGNAL(valueChanged(int)), this, SLOT(onSelectedRegionLvlSpinbox(int)));
    connect(m_selectedRegionStX , SIGNAL(valueChanged(int)), this, SLOT(onSelectedRegionStXSpinbox(int)));
    connect(m_selectedRegionEndX, SIGNAL(valueChanged(int)), this, SLOT(onSelectedRegionEndXSpinbox(int)));
    connect(m_selectedRegionStY , SIGNAL(valueChanged(int)), this, SLOT(onSelectedRegionStYSpinbox(int)));
    connect(m_selectedRegionEndY, SIGNAL(valueChanged(int)), this, SLOT(onSelectedRegionEndYSpinbox(int)));
}

//////////////////////////////////////////
// create filterlayout
void MainWindow::createFilterGrid()
{
    // Add new spinbox control
    m_FilterLayout = new QGridLayout;

    // Radio buttons
    QRadioButton* editModeRbtn = new QRadioButton("Region Edit mode");
    editModeRbtn->setChecked(true);
    connect(editModeRbtn, SIGNAL(toggled(bool)), this, SLOT(onEditModeRadio(bool)));
    m_FilterLayout->addWidget(editModeRbtn,1,1,Qt::AlignCenter);

    QRadioButton* tiltShiftRbtn = new QRadioButton("Interactive mode");
    tiltShiftRbtn->setChecked(false);
    connect(tiltShiftRbtn, SIGNAL(toggled(bool)), this, SLOT(onInteractiveRadio(bool)));
    m_FilterLayout->addWidget(tiltShiftRbtn,2,1, Qt::AlignCenter);

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

    // save result image file
    saveFileAct = new QAction(tr("&Save Output Image File"), this);
    saveFileAct->setShortcut(tr("Ctrl+I"));
    connect(saveFileAct, SIGNAL(triggered()), this, SLOT(saveFile()));

    //load input file with data
    loadFileAndDataAct = new QAction(tr("&Load Imgae File With Data"), this);
    loadFileAndDataAct->setShortcut(tr("Ctrl+l"));
    connect(loadFileAndDataAct, SIGNAL(triggered()), this, SLOT(loadFileAndData()));

    //save input file with data
    saveFileAndDataAct = new QAction(tr("&Save Image File With Data"), this);
    saveFileAndDataAct->setShortcut(tr("Ctrl+m"));
    connect(saveFileAndDataAct, SIGNAL(triggered()), this, SLOT(saveFileAndData()));

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
    fileMenu->addAction(saveFileAct);
    fileMenu->addAction(loadFileAndDataAct);
    fileMenu->addAction(saveFileAndDataAct);

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

///////////////////////////////////////////////////////////////
// On file save
// Save output image
void MainWindow::saveFile()
{
    QString savefilename = QFileDialog::getSaveFileName(this, tr("save file"), "/home",tr("Images (*.png *.tif *.bmp *.jpg)")  );

    if(!savefilename.isEmpty())
    {
        if(!m_OutputImage.save(savefilename))
        {
            QMessageBox::information(this, tr("Warning!"), tr("Cannot save %1.").arg(savefilename));
            return;
        }
    }
}

////////////////////////////////////////////////////////
// load the input image and its data
void MainWindow::loadFileAndData()
{

}

////////////////////////////////////////////////////////
// save the input image and its data
void MainWindow::saveFileAndData()
{

}


////////////////////////////////////////////
// Reset
void MainWindow::onResetBtn()
{
    if(m_OutputImage.isNull())
        return;

    // Clean OutputImage
    OutputImageLabel->setPixmap(m_OutputImage.getOriginalPixmap());
    OutputImageLabel->adjustSize();
    m_OutputImage.updateHistogram();

    // Clean Regions
    for(int i=0; i< m_rgRegions.size(); i++)
    {
        if(m_rgRegions[i]!= NULL)
            delete m_rgRegions[i];
    }
    m_rgRegions.clear();
    m_selectedRegion = NULL;
}

////////////////////////////////////////////
// Apply filter
void MainWindow::onApplyFilterBtn()
{
    if(m_OutputImage.isNull())
        return;

    this->setCursor(Qt::WaitCursor);
    QApplication::processEvents();

    // Apply filter
    applyFilter();

    this->setCursor(Qt::ArrowCursor);
}

///////////////////////////////////////////////////////////////////
// Apply filter to Output image
void MainWindow::applyFilter()
{
    // Apply filter
    m_OutputImage.applyFilter(this);
    OutputImageLabel->setPixmap(m_OutputImage.getGrayPixmap());
    OutputImageLabel->adjustSize();

    // update histogram
    m_OutputImage.updateHistogram();
}

/////////////////////////////////////////////////////////////////////
// Normalized filter
void MainWindow::normalizeFilter(QVector<float> &rgFilter)
{
    // get weight
    float weight = 0.0f;
    for(int i =0; i< rgFilter.size(); i++)
    {
         weight += rgFilter[i];
    }

    if(weight == 0.0f)
        weight = 1.0f;

    // normalize filter
    for(int i =0; i< rgFilter.size(); i++)
    {
         rgFilter[i] /= weight;
    }
}

//////////////////////////////////////////
//Mouse Event
void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (!m_bTiltShift) {
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

}

//////////////////////////////////////////////////////////
// Mouse move
void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (!m_bTiltShift) {
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
                CRegion newRegion(m_firstPos, m_lastPos,InputImageLabel);
                newRegion.show(m_bShowRegion);
            }
        }

        m_lastPos = event->pos();
    }
}

///////////////////////////////////////////////////////
// Release
void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    if (!m_bTiltShift)
    {
        // Release to add new point
        if (event->button()== Qt::LeftButton)
        {
            if(m_selectedRegion == NULL)
            {
                if(!insideInputImage(m_lastPos))
                    return;

                // Create new region
                CRegion* newRegion = new CRegion(m_firstPos, m_lastPos,InputImageLabel);
                m_rgRegions.push_back(newRegion);
                m_selectedRegion = newRegion;
            }
            else
            {
                // move selected region
                int dx = event->pos().x() - m_lastPos.x();
                int dy = event->pos().y() - m_lastPos.y();
                m_selectedRegion->moveTo(dx,dy);
            }

            //display the current selected region's start and end position
            m_selectedRegionStX->setValue (m_selectedRegion->getTopLeft().x());
            m_selectedRegionStY->setValue (m_selectedRegion->getTopLeft().y());
            m_selectedRegionEndX->setValue(m_selectedRegion->getBotRight().x());
            m_selectedRegionEndY->setValue(m_selectedRegion->getBotRight().y());
            m_selectedRegionLvl->setValue (m_selectedRegion->Z_Level());
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
                m_selectedRegionStX->setValue(m_selectedRegion->getTopLeft().x());
                m_selectedRegionStY->setValue(m_selectedRegion->getTopLeft().y());
                m_selectedRegionEndX->setValue(m_selectedRegion->getBotRight().x());
                m_selectedRegionEndY->setValue(m_selectedRegion->getBotRight().y());
                m_selectedRegionLvl->setValue(m_selectedRegion->Z_Level());
            }
            else
            {
                //display the start and end position to 0
                m_selectedRegionStX->setValue(0);
                m_selectedRegionStY->setValue(0);
                m_selectedRegionEndX->setValue(0);
                m_selectedRegionEndY->setValue(0);
                m_selectedRegionLvl->setValue(0);
            }
        }

        updateRegions();
        m_lastPos = event->pos();
    }
    else
    {
        int target = 0;
        QPoint selectedPoint;
        selectedPoint = event->pos();

        // Only apply if it is inside Image label 1
        if(!insideInputImage(m_lastPos))
            return;

        this->setCursor(Qt::WaitCursor);
        QApplication::processEvents();

        if(m_rgRegions.size() != 0)
        {
            for(int k=0; k< m_rgRegions.size(); k++)
            {
                if(m_rgRegions[k]->containsPoint(selectedPoint))
                {
                    target = m_rgRegions[k]->Z_Level();
                    m_selectedRegion = m_rgRegions[k];
                    break;
                }
            }

            //upload the data information for the selected region
            m_selectedRegionStX->setValue(m_selectedRegion->getTopLeft().x());
            m_selectedRegionStY->setValue(m_selectedRegion->getTopLeft().y());
            m_selectedRegionEndX->setValue(m_selectedRegion->getBotRight().x());
            m_selectedRegionEndY->setValue(m_selectedRegion->getBotRight().y());
            m_selectedRegionLvl->setValue(m_selectedRegion->Z_Level());

            // Apply tilt-shift filter
            m_OutputImage.tiltShift(this, target, selectedPoint);
            OutputImageLabel->setPixmap(m_OutputImage.getGrayPixmap());
            OutputImageLabel->adjustSize();

            // update histogram
            m_OutputImage.updateHistogram();
         }
        this->setCursor(Qt::ArrowCursor);
     }
}


//////////////////////////////////////////////
// Return the approviate filter depend on z value
MainWindow::FILTER MainWindow::getFilter(int z)
{
    FILTER myFilter;

    if(z == 0)
    {
        //do nothing
        myFilter.width         = 0;
        myFilter.height        = 0;
        myFilter.recursiveTime = 0;
    }
    else
    if(z<0) // Apply bluring
    {
        myFilter.width         = 0-z;
        myFilter.height        = 0-z;
        myFilter.recursiveTime = z;

        float filterSize = myFilter.width*myFilter.height*1.0f;
        for(int i =0; i< filterSize; i++)
        {
            myFilter.rgFilter.push_back(1.0f);
        }
    }
    else
    if(z>0) // Apply sharpening
    {
        myFilter.width         = z;
        myFilter.height        = z;
        myFilter.recursiveTime = z;

        // set up filter
        float filterSize = myFilter.width*myFilter.height*1.0f;
        float midPoint = filterSize + filterSize - 1;
        int midPointIndex;

        //If Height is even, set the mid pixel to be the previous one
        if(myFilter.height%2 == 0)
            midPointIndex = filterSize/2 -1;
        else
            midPointIndex = filterSize/2;

        for(int i =0; i< filterSize; i++)
        {
            (i == midPointIndex)?myFilter.rgFilter.push_back(midPoint*1.0f):myFilter.rgFilter.push_back(-1.0f);
        }
    }

    // Normalize filter
    normalizeFilter(myFilter.rgFilter);
    return myFilter;
}

//////////////////////////////////////////
//return the interactive filter
MainWindow::FILTER MainWindow::getTiltShiftFilter(int current_z, int target_z, int distance)
{
    FILTER myFilter;
    if (current_z == target_z) {
        if (current_z < 0)
            current_z = (-1) * current_z;

        if ( distance <= int((0.2)*m_InputImage.imageWidth())) {
            current_z = 0;
        }else {
            current_z = 2;
        }
    } else {
        if (current_z > 0 && target_z > 0){
            if (current_z > target_z) {
                current_z = current_z - target_z;
            } else {
                current_z = target_z - current_z;
            }
        } else if (current_z > 0 && target_z < 0) {
            current_z = current_z - target_z;
        } else if (current_z < 0 && target_z > 0) {
            current_z = target_z - current_z;
        } else {
            if (current_z > target_z) {
                current_z = current_z - target_z;
            }else{
                current_z = target_z - current_z;
            }
        }

    }


    myFilter.width         = current_z;
    myFilter.height        = current_z;
    myFilter.recursiveTime = current_z;

    float filterSize = myFilter.width*myFilter.height*1.0f;
    for(int i =0; i< filterSize; i++)
    {
        myFilter.rgFilter.push_back(1.0f);
    }

    // Normalize filter
    normalizeFilter(myFilter.rgFilter);

    return myFilter;
   /* FILTER myFilter;

    if (current_z == target_z) {
        if (current_z < 0)
            current_z = (-1) * current_z;

        if (distance > int((0.2)*m_InputImage.imageWidth())
                  && distance <= int((0.3)*m_InputImage.imageWidth())) {
            current_z --;
        }else if (distance > int((0.3)*m_InputImage.imageWidth())
                  && distance <= int((0.4)*m_InputImage.imageWidth())) {
            current_z = current_z - 2;
        }else {
            current_z = current_z - 3;
        }

        myFilter.width         = current_z;
        myFilter.height        = current_z;
        myFilter.recursiveTime = current_z;

        // set up filter
        float filterSize = myFilter.width*myFilter.height*1.0f;
        float midPoint = filterSize + filterSize - 1;
        int midPointIndex;

        //If Height is even, set the mid pixel to be the previous one
        if(myFilter.height%2 == 0)
            midPointIndex = filterSize/2 -1;
        else
            midPointIndex = filterSize/2;

        for(int i =0; i< filterSize; i++)
        {
            (i == midPointIndex)?myFilter.rgFilter.push_back(midPoint*1.0f):myFilter.rgFilter.push_back(-1.0f);
        }
    } else {
        if (current_z > 0) {
            if (current_z > target_z){
                myFilter.width         = current_z - target_z;
                myFilter.height        = current_z - target_z;
            } else {
                myFilter.width         = target_z - current_z;
                myFilter.height        = target_z - current_z;
            }
            myFilter.recursiveTime = (-1) * current_z;
        } else {
            if (current_z > target_z){
                myFilter.width         = current_z - target_z;
                myFilter.height        = current_z - target_z;
            } else {
                myFilter.width         = target_z - current_z;
                myFilter.height        = target_z - current_z;
            }
            myFilter.recursiveTime = current_z;
        }

        float filterSize = myFilter.width*myFilter.height*1.0f;
        for(int i =0; i< filterSize; i++)
        {
            myFilter.rgFilter.push_back(1.0f);
        }
    }

    // Normalize filter
    float weight = 0.0f;
    for(int i =0; i< myFilter.rgFilter.size(); i++)
    {
         weight += myFilter.rgFilter[i];
    }

    if(weight == 0.0f)
        weight = 1.0f;

    for(int i =0; i< myFilter.rgFilter.size(); i++)
    {
         myFilter.rgFilter[i] /= weight;
    }

    return myFilter;*/
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

//////////////////////////////////////////////////////
// control chkbox for showing the interactive effect
void MainWindow::onInteractiveRadio(bool state)
{
    m_bTiltShift = state;
    // Hide region
    m_showRegionChkbox->setChecked(false);
    m_bShowRegion = false;
    updateRegions();
}

void MainWindow::onEditModeRadio(bool state)
{
    m_bTiltShift = !state;
    // show region
    m_showRegionChkbox->setChecked(true);
    m_bShowRegion = true;
    updateRegions();
}

void MainWindow::onSelectedRegionLvlSpinbox(int value)
{
    if (m_selectedRegion != NULL)
    {
        m_selectedRegion->Z_Level(value);
    }
}

void MainWindow::onSelectedRegionStXSpinbox(int value)
{
    if (m_selectedRegion != NULL)
    {
        QPoint topLeft = m_selectedRegion->getTopLeft();
        topLeft.setX(value);
        m_selectedRegion->setTopLeft(topLeft);
    }
}
void MainWindow::onSelectedRegionStYSpinbox(int value)
{
    if (m_selectedRegion != NULL)
    {
        QPoint topLeft = m_selectedRegion->getTopLeft();
        topLeft.setY(value);
        m_selectedRegion->setTopLeft(topLeft);
    }
}
void MainWindow::onSelectedRegionEndXSpinbox(int value)
{
    if (m_selectedRegion != NULL)
    {
        QPoint botRight = m_selectedRegion->getBotRight();
        botRight.setX(value);
        m_selectedRegion->setBotRight(botRight);
    }
}
void MainWindow::onSelectedRegionEndYSpinbox(int value)
{
    if (m_selectedRegion != NULL)
    {
        QPoint botRight = m_selectedRegion->getBotRight();
        botRight.setY(value);
        m_selectedRegion->setBotRight(botRight);
    }
}

bool MainWindow::insideInputImage(const QPoint &point)
{
    QRect rect = InputImageLabel->rect();

    // Check point
    if( point.x() < rect.topRight().x()+ 20 &&
        point.x() > rect.topLeft().x() &&
        point.y() < rect.bottomLeft().y() + 40 &&
        point.y() > rect.topLeft().y())
        return true;

    return false;
}

///////////////////////////////////////////////////////
// todo: need to test
bool MainWindow::insideOutputImage(const QPoint &point)
{
    QRect rect = OutputImageLabel->rect();

    // Check point
    if( point.x() < rect.topRight().x()+ 20 &&
        point.x() > rect.topLeft().x() &&
        point.y() < rect.bottomLeft().y() + 40 &&
        point.y() > rect.topLeft().y())
        return true;

    return false;
}
