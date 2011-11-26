
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
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

    QSpinBox    *FilterWidth;
    QSpinBox    *FilterHeight;

    QMenu *fileMenu;

    QAction *openFileAct;
    QAction *exitAct;

    QString filename;

    CImage m_InputImage;
    CImage m_OutputImage;

    QVector<QDoubleSpinBox*> m_rgSpinBoxes;
    QVector<float> m_rgFilter;

    int m_filterWidth;
    int m_filterHeight;
    QGridLayout* m_FilterLayout;

    bool m_bRecursive;
    bool m_bRedChannel;
    bool m_bGreenChannel;
    bool m_bBlueChannel;

    bool m_bNormalized;
};

#endif
