#include "cacheline.h"
#include <QtWidgets>
#include <QToolTip>
#include "view.h"

cacheline::cacheline(const QColor &color, int x, int y, int setID)
{
    this->x = x;
    this->y = y;
    this->color = color;
    this->set_idx = setID;
    setZValue((x + y) % 2);

    setFlags(ItemIsSelectable);
    setAcceptHoverEvents(true);
}

QRectF cacheline::boundingRect() const
{
    return QRectF(0, 0, 440, 70);
}

QPainterPath cacheline::shape() const
{
    QPainterPath path;
    path.addRect(14, 14, 328, 42);
    return path;
}


void cacheline::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget);

    QColor fillColor = (option->state & QStyle::State_Selected) ? color.darker(150) : color;
    if (option->state & QStyle::State_MouseOver)
        fillColor = fillColor.lighter(70);

    const qreal lod = option->levelOfDetailFromTransform(painter->worldTransform());
    if (lod < 0.2) {
        if (lod < 0.125) {
            painter->fillRect(QRectF(0, 0, 440, 70), fillColor);
            return;
        }

        QBrush b = painter->brush();
        painter->setBrush(fillColor);
        painter->drawRect(13, 13, 388, 57);
        painter->setBrush(b);
        return;
    }

    QPen oldPen = painter->pen();
    QPen pen = oldPen;
    int width = 0;
    if (option->state & QStyle::State_Selected)
        width += 2;

    pen.setWidth(width);
    QBrush b = painter->brush();
    painter->setBrush(QBrush(fillColor.darker(option->state & QStyle::State_Sunken ? 100 : 100)));

    painter->drawRect(QRect(14, 14, 316, 39));
    painter->setBrush(b);

    if (lod >= 0.5) {

        QFont font("Times", 76);
        font.setStyleStrategy(QFont::ForceOutline);
        painter->setFont(font);
        painter->save();
        painter->scale(0.1, 0.1);
        painter->drawText(170, 240, QString("Cache Line at %1x%2").arg(x).arg(y));
        painter->drawText(170, 340, QString("Set Index: %1").arg(set_idx));
        painter->restore();
    }

    // Display cache line data
    if (lod >= 2) {
        QFont font("Times", 76);
        font.setStyleStrategy(QFont::ForceOutline);
        painter->setFont(font);
        painter->save();
        painter->scale(0.1, 0.1);
        painter->drawText(170, 240, QString("Cache Line at %1x%2").arg(x).arg(y));
        painter->drawText(170, 340, QString("Set Index: %1").arg(set_idx));
        painter->restore();
    }


    if (stuff.size() > 1) {
        QPen p = painter->pen();
        painter->setPen(QPen(Qt::red, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        painter->setBrush(Qt::NoBrush);
        QPainterPath path;
        path.moveTo(stuff.first());
        for (int i = 1; i < stuff.size(); ++i)
            path.lineTo(stuff.at(i));
        painter->drawPath(path);
        painter->setPen(p);
    }
}


void cacheline::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::RightButton){
        QGraphicsItem::mousePressEvent(event);
        QString tltip = QString("Hello set: %1 \n This is a second line..\nThis is a third line\nTest TestTestTestTestTestTestTestTestTest\n.....................").arg(this->set_idx);
        event->accept();
        QPoint t_pos = event->screenPos();
        QToolTip::showText(t_pos,tltip);

    }
    //printf("Cache Line pressed!!!\n");
    update();
}

void cacheline::hoverEnterEvent(QGraphicsSceneHoverEvent *event){

    //printf("Set: %d hovered\n", this->set_idx);
    //View::renewLabels(this->set_idx, this->x, this->y);
    update();
}

