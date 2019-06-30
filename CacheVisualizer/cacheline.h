#ifndef CACHELINE_H
#define CACHELINE_H

#include <QColor>
#include <QGraphicsItem>
#include <QWidget>

class cacheline: public QGraphicsItem
{
public:
    cacheline(const QColor &color, int x, int y, int setID);
    QRectF boundingRect() const override;
    QPainterPath shape() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget) override;
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

//Maybe make coords publicaly accessible?
private:
    int x;
    int y;
    int set_idx;
    QColor color;
    QVector<QPointF> stuff;
};

#endif // CACHELINE_H
