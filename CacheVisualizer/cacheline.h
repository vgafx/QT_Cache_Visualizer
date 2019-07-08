#ifndef CACHELINE_H
#define CACHELINE_H

#include <QColor>
#include <QGraphicsItem>
#include <QWidget>
#include "statuscontroller.h"

class cacheline: public QGraphicsItem
{
public:
    cacheline(const QColor &color, int x, int y, int setID, statusController *in);
    QRectF boundingRect() const override;
    QPainterPath shape() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget) override;
    void setColor(const QColor &color);
    int getAge();
    void setAge(int age);
    void activateSector(int sector);
    void invalidateSector(int secID);
    void flipAllSectors(int onoff);
    void setAddressLow(unsigned long long addlow);
    void setAddressHigh(unsigned long long addhigh);
    unsigned long long getAddressLow();
    unsigned long long getAddressHigh();
    void setTag(int tag);
    void setBlockOffset(int bf);
    void setDataStructure(std::string ds);
    int getTag();
    int getBlockOffset();
    std::string getDataStructure();
    statusController *sts;

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

//Maybe make coords publicaly accessible?
private:
    int x;
    int y;
    int set_idx;
    int age;
    int tag;
    int block_offest;
    unsigned long long address_low;
    unsigned long long address_high;
    std::string data_structure;
    bool sector_one_filled;
    bool sector_two_filled;
    bool sector_three_filled;
    bool sector_four_filled;
    QColor color;
    QVector<QPointF> stuff;

};

#endif // CACHELINE_H
