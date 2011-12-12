
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtGui>
#include <QStringList>
#include <QDir>
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
class QRadioButton;
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
    QVector<CRegion*>getRegions(){return m_rgRegions;}


private slots:
    void openFile();
    void saveFile();
    void loadData();
    void saveData();
    void onResetBtn();
    void onApplyFilterBtn();
    void onShowRegionChkbox(bool);
    void onInteractiveRadio(bool);
    void onEditModeRadio(bool);

    void showImage();
    void turnToNext();
    void turnToPrev();
    void dirChanged(QString);

    void onSelectedRegionLvlSpinbox(int);
    void onSelectedRegionStXSpinbox(int);
    void onSelectedRegionEndXSpinbox(int);
    void onSelectedRegionStYSpinbox(int);
    void onSelectedRegionEndYSpinbox(int);

signals:
    void get2end(bool);
    void get2beg(bool);

private:
    void createActions();
    void createMenus();
    void createFilterGrid();
    void createImageArea ();
    void createButtons   ();
    void createSpinBox   ();

    //init path of image gallery
    void init_path(QDir, QString);

    void applyFilter();
    void normalizeFilter(QVector<float> &rgFilter);

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
    QPushButton *ResetBtn;

    //display the selected region position
    QSpinBox    *m_selectedRegionStX;
    QSpinBox    *m_selectedRegionStY;
    QSpinBox    *m_selectedRegionEndX;
    QSpinBox    *m_selectedRegionEndY;
    QSpinBox    *m_selectedRegionLvl;
    QCheckBox   *m_showRegionChkbox;

    QMenu *fileMenu;

    QAction *openFileAct;
    QAction *saveFileAct;
    QAction *loadDataAct;
    QAction *saveDataAct;
    QAction *pervImage;
    QAction *nextImage;
    QAction *indexPath;
    QAction *exitAct;

    QString filename;

    CImage m_InputImage;
    CImage m_OutputImage;

    QGridLayout* m_FilterLayout;

    QGridLayout *RegionBoxLayout;

    bool m_bShowRegion;
    bool m_bTiltShift;

    //image gallery
    QStringList::const_iterator iterator;
    QStringList list;
    QFileDialog *fileDialog;
    QDir filepath;

    //Regions
    QVector<CRegion*> m_rgRegions;
    QPoint m_firstPos;
    QPoint m_lastPos;

    CRegion* m_selectedRegion;
};

#endif
