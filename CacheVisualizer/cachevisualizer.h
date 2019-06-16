#ifndef CACHEVISUALIZER_H
#define CACHEVISUALIZER_H

#include <QMainWindow>

namespace Ui {
class CacheVisualizer;
}

class CacheVisualizer : public QMainWindow
{
    Q_OBJECT

public:
    explicit CacheVisualizer(QWidget *parent = nullptr);
    ~CacheVisualizer();

private:
    Ui::CacheVisualizer *ui;
};

#endif // CACHEVISUALIZER_H
