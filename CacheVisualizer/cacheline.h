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
    void setAddressLow(long long addlow);
    void setAddressHigh(long long addhigh);
    long long getAddressLow();
    long long getAddressHigh();
    void setTag(int tag);
    void setBlockOffset(int bf);
    void setDataStructure(std::string ds);
    int getTag();
    int getBlockOffset();
    std::string getDataStructure();
    statusController *sts;

    bool getIs_empty() const;
    void setIs_empty(bool value);

    long long getIdxLow() const;
    void setIdxLow(long long value);

    long long getIdxHigh() const;
    void setIdxHigh(long long value);

    bool getSector_one_filled() const;
    void setSector_one_filled(bool value);

    bool getSector_two_filled() const;
    void setSector_two_filled(bool value);

    bool getSector_three_filled() const;
    void setSector_three_filled(bool value);

    bool getSector_four_filled() const;
    void setSector_four_filled(bool value);

    void incAge();
    void setFilledOnEmpty();
    void displayEviction();
    void displayHit();
    void displayPartialHit();
    void displayMiss();
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

//Maybe make coords publicaly accessible?
private:
    int x;
    int y;
    long long idx_low;
    long long idx_high;
    int set_idx;
    int age;
    int tag;
    int block_offest;
    long long address_low;
    long long address_high;
    std::string data_structure;
    bool sector_one_filled;
    bool sector_two_filled;
    bool sector_three_filled;
    bool sector_four_filled;
    bool is_empty;
    QColor color;
    QVector<QPointF> stuff;

};

#endif // CACHELINE_H
