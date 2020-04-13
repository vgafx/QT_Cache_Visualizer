/*** Cacheline
 *  The Cachelines that appear on the visualizer's scene are based off of this class.
 *  Besides the GUI object, this class also keeps information regarding which data are currently
 *  stored in the cacheline (i.e. state).
 ***/
#pragma once

#include <QColor>
#include <QGraphicsItem>
//#include <QWidget>
#include <QtWidgets>
#include <QToolTip>

#include "view.h"
#include "statuscontroller.h"

class Cacheline: public QGraphicsItem
{
private:
    QColor m_color;
    int m_scene_x;
    int m_scene_y;
    int m_set_idx;
    unsigned int m_idx_low;
    unsigned int m_idx_high;
    int m_age;
    int m_tag;
    int m_block_offset;
    unsigned long long m_address_low;
    unsigned long long m_address_high;
    unsigned long long m_time_accessed;
    char m_data_structure;
    bool m_s0_filled;
    bool m_s1_filled;
    bool m_s2_filled;
    bool m_s3_filled;
    bool m_is_empty;

public:

    Cacheline(const QColor &color, int x, int y, int setID, StatusController *in);
    /*GUI functionality*/
    QRectF boundingRect() const override;
    QPainterPath shape() const override;
    StatusController *sts;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget) override;

    /*Methods for state manipulation*/
    void setColor(const QColor &color);
    void resetColor();
    void activateSector(int sector);
    void invalidateSector(int secID);
    void flipAllSectors(bool onoff);
    void incAge();
    void displayEviction();
    void displayFullHit();
    void displayPartialHit();


    /*Setters & Getters*/
    int getSceneX() const;
    void setSceneX(int scene_x);

    int getSceneY() const;
    void setSceneY(int scene_y);

    unsigned int getIdxLow() const;
    void setIdxLow(unsigned int idx_low);

    unsigned int getIdxHigh() const;
    void setIdxHigh(unsigned int idx_high);

    int getSetIdx() const;
    void setSetIdx(int set_idx);

    int getAge() const;
    void setAge(int age);

    int getTag() const;
    void setTag(int tag);

    int getBlockOffset() const;
    void setBlockOffset(int block_offset);

    unsigned long long getAddressLow() const;
    void setAddressLow(unsigned long long address_low);

    unsigned long long getAddressHigh() const;
    void setAddressHigh(unsigned long long address_high);

    unsigned long long getTimeAccessed() const;
    void setTimeAccessed(unsigned long long time_accessed);

    char getDataStructure() const;
    void setDataStructure(char data_structure);

    bool getS0Filled() const;
    void setS0Filled(bool s0_filled);

    bool getS1Filled() const;
    void setS1Filled(bool s1_filled);

    bool getS2Filled() const;
    void setS2Filled(bool s2_filled);

    bool getS3Filled() const;
    void setS3Filled(bool s3_filled);

    bool getIsEmpty() const;
    void setIsEmpty(bool is_empty);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

};
