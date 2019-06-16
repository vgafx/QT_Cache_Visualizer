#include "cachevisualizer.h"
#include "ui_cachevisualizer.h"

CacheVisualizer::CacheVisualizer(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::CacheVisualizer)
{
    ui->setupUi(this);
}

CacheVisualizer::~CacheVisualizer()
{
    delete ui;
}
