
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtGui>
#include "image.h"

QT_BEGIN_NAMESPACE
class QAction;
class QLabel;
class QMenu;
class QScrollArea;
class QSlider;
class QPushButton;
class QGridLayout;
class QDoubleSpinBox;
class QSpinBox;
class QCheckBox;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    struct FILTER
    {
        QVector<float> rgFilter;
        int width;
        int height;
        int recursiveTime;
    };

    MainWindow();
    ~MainWindow();
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);

    FILTER getFilter(int z);
    FILTER getTiltShiftFilter(int current_z, int target_z, int distance);
    QVector<CRegion*> getRegions(){return m_rgRegions;}

private slots:
    void openFile();
    void saveFile();
    void loadFileAndData();
    void saveFileAndData();
    void onResetBtn();
    void onSharpenBtn();
    void onApplyFilterBtn();
    void onNormalizedChkbox(bool);
    void onFilterWidthChanged(int);
    void onFilterHeightChanged(int);
    void onShowRegionChkbox(bool);
    void onTiltShiftChkbox(bool);

    void onSelectedRegionLvlSpinbox(int);
    void onSelectedRegionStXSpinbox(int);
    void onSelectedRegionEndXSpinbox(int);
    void onSelectedRegionStYSpinbox(int);
    void onSelectedRegionEndYSpinbox(int);

private:
    void createActions();
    void createMenus();
    void createFilterGrid();
    void createImageArea ();
    void createButtons   ();
    void createSpinBox   ();

    void applyFilter();
    void normalizeFilter();

    void changedFilterGrid();
    void updateRegions();

    bool insideInputImage(const QPoint& point);
    bool insideOutputImage(const QPoint& point);

// Attributes

    QWidget *centralWidget;

    QScrollArea *InputImageArea;
    QScrollArea *OutputImageArea;

    QScrollArea *InputHistogramArea;
    QScrollArea *OutputHistogramArea;

    QLabel *InputImageLabel;
    QLabel *OutputImageLabel;

    QPushButton *ApplyBtn;
    QPushButton *BlurBtn;
    QPushButton *SharpenBtn;

    QCheckBox   *NormalizedChkbox;

    QSpinBox    *FilterWidth;
    QSpinBox    *FilterHeight;

    //display the selected region position
    QSpinBox    *m_selectedRegionStX;
    QSpinBox    *m_selectedRegionStY;
    QSpinBox    *m_selectedRegionEndX;
    QSpinBox    *m_selectedRegionEndY;
    QSpinBox    *m_selectedRegionLvl;


    QMenu *fileMenu;

    QAction *openFileAct;
    QAction *saveFileAct;
    QAction *loadFileAndDataAct;
    QAction *saveFileAndDataAct;
    QAction *exitAct;

    QString filename;

    CImage m_InputImage;
    CImage m_OutputImage;

    QVector<QDoubleSpinBox*> m_rgSpinBoxes;
    QVector<float> m_rgFilter;

    int m_filterWidth;
    int m_filterHeight;
    QGridLayout* m_FilterLayout;

    QGridLayout *RegionBoxLayout;

    bool m_bNormalized;
    bool m_bShowRegion;
    bool m_bTiltShift;

    //Regions
    QVector<CRegion*> m_rgRegions;
    QPoint m_firstPos;
    QPoint m_lastPos;

    CRegion* m_selectedRegion;
};

#endif
