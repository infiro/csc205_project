
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
    MainWindow();
    ~MainWindow();
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);

private slots:
    void openFile();
    void onBlurBtn();
    void onSharpenBtn();
    void onApplyFilterBtn();
    void onNormalizedChkbox(bool);
    void onRegion1Chkbox(bool);
    void onRegion2Chkbox(bool);
    void onRegion3Chkbox(bool);
    void onFilterWidthChanged(int);
    void onFilterHeightChanged(int);
    void onShowRegionChkbox(bool);

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

    QCheckBox   *region1;
    QCheckBox   *region2;
    QCheckBox   *region3;

    QSpinBox    *FilterWidth;
    QSpinBox    *FilterHeight;

    QSpinBox    *oneStX;
    QSpinBox    *twoStX;
    QSpinBox    *threeStX;
    QSpinBox    *oneEndX;
    QSpinBox    *twoEndX;
    QSpinBox    *threeEndX;
    QSpinBox    *oneStY;
    QSpinBox    *twoStY;
    QSpinBox    *threeStY;
    QSpinBox    *oneEndY;
    QSpinBox    *twoEndY;
    QSpinBox    *threeEndY;

    QMenu *fileMenu;

    QAction *openFileAct;
    QAction *exitAct;

    QString filename;

    CImage m_InputImage;
    CImage m_OutputImage;

    // Rubberband display selected region
    QRubberBand *rubberBand_reg1;
    QRubberBand *rubberBand_reg2;
    QRubberBand *rubberBand_reg3;

    // Origin for each region
    QPoint origin_reg1;
    QPoint origin_reg2;
    QPoint origin_reg3;

    QVector<QDoubleSpinBox*> m_rgSpinBoxes;
    QVector<float> m_rgFilter;

    int m_filterWidth;
    int m_filterHeight;
    QGridLayout* m_FilterLayout;

    QGridLayout *RegionBoxLayout;

    bool m_region1;
    bool m_region2;
    bool m_region3;
    bool m_bNormalized;
    bool m_bShowRegion;

    //Regions
    QVector<CRegion*> m_rgRegions;
    QPoint m_firstPos;
    QPoint m_lastPos;

    CRegion* m_selectedRegion;
};

#endif
