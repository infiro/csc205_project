
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
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);

private slots:
    void openFile();
    void edgeDetection();
    void blur();
    void sharpen();
    void onApplyFilterBtn();
    void onRecursiveChkbox(bool);
    void onRedChannelChkbox(bool);
    void onGreenChannelChkbox(bool);
    void onBlueChannelChkbox(bool);
    void onNormalizedChkbox(bool);
    void onRegion1Chkbox(bool);
    void onRegion2Chkbox(bool);
    void onRegion3Chkbox(bool);

    void onFilterWidthChanged(int);
    void onFilterHeightChanged(int);

private:
    void createActions();
    void createMenus();
    void createFilterGrid();
    void createImageArea ();
    void createButtons   ();

    void applyFilter();
    void normalizeFilter();

    void changedFilterGrid();

// Attributes

    QWidget *centralWidget;

    QScrollArea *InputImageArea;
    QScrollArea *OutputImageArea;

    QScrollArea *InputHistogramArea;
    QScrollArea *OutputHistogramArea;

    QLabel *InputImageLabel;
    QLabel *OutputImageLabel;

    QPushButton *EdgeDetectionBtn;
    QPushButton *BlurBtn;
    QPushButton *SharpenBtn;
    QPushButton *ApplyBtn;

    QCheckBox   *RecursiveChkbox;
    QCheckBox   *RedChkbox;
    QCheckBox   *GreenChkbox;
    QCheckBox   *BlueChkbox;
    QCheckBox   *NormalizedChkbox;

    QCheckBox   *region1;
    QCheckBox   *region2;
    QCheckBox   *region3;

    QSpinBox    *FilterWidth;
    QSpinBox    *FilterHeight;

    QMenu *fileMenu;

    QAction *openFileAct;
    QAction *exitAct;

    QString filename;

    CImage m_InputImage;
    CImage m_OutputImage;

    QRubberBand *rubberBand_reg1;
    QRubberBand *rubberBand_reg2;
    QRubberBand *rubberBand_reg3;

    QPoint origin_reg1;
    QPoint origin_reg2;
    QPoint origin_reg3;


    QVector<QDoubleSpinBox*> m_rgSpinBoxes;
    QVector<float> m_rgFilter;

    int m_filterWidth;
    int m_filterHeight;
    QGridLayout* m_FilterLayout;

    bool m_bRecursive;
    bool m_bRedChannel;
    bool m_bGreenChannel;
    bool m_bBlueChannel;
    bool m_region1;
    bool m_region2;
    bool m_region3;

    bool m_bNormalized;
};

#endif
