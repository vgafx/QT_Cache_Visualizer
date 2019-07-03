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
    this->age = 0;
    this->address_low=0;
    this->address_high=127;
    this->setDataStructure("DataAr");
    this->tag = 0;
    this->block_offest = 0;
    this->flipAllSectors(0);
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
        QString tltip = QString("Set Index:  %1 \nLine Coordinates:  %2 x %3 \nAddres Range:  %4 - %5 "
                                "\nData Structure:  %6\nSectors Filled:  %7 - %8 - %9 - %10\nTag:  %11 \nAge:  %12")
                .arg(this->set_idx).arg(this->x).arg(this->y).arg(this->address_low).arg(this->address_high)
                .arg(QString::fromStdString(this->data_structure)).arg(this->sector_one_filled).arg(this->sector_two_filled).arg(this->sector_three_filled).arg(this->sector_four_filled).arg(this->tag).arg(this->age);
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

int cacheline::getAge(){
    return this->age;
}

void cacheline::setAge(int inAge){
    this->age = inAge;
}

void cacheline:: activateSector(int secID){
    switch (secID){
    case 1:
        this->sector_one_filled = true;
        break;
    case 2:
        this->sector_two_filled = true;
        break;
    case 3:
        this->sector_three_filled = true;
        break;
    case 4:
        this->sector_four_filled = true;
        break;
    }
}


void cacheline:: invalidateSector(int secID){
    switch (secID){
    case 1:
        this->sector_one_filled = false;
        break;
    case 2:
        this->sector_two_filled = false;
        break;
    case 3:
        this->sector_three_filled = false;
        break;
    case 4:
        this->sector_four_filled = false;
        break;
    }
}

void cacheline::flipAllSectors(int on_off){
    if(on_off == 0){
        this->sector_one_filled = 0;
        this->sector_two_filled = 0;
        this->sector_three_filled = 0;
        this->sector_four_filled = 0;
    } else if(on_off == 1){
        this->sector_one_filled = 1;
        this->sector_two_filled = 2;
        this->sector_three_filled = 3;
        this->sector_four_filled = 4;
    }
}

void cacheline::setAddressLow(unsigned long long add_low){
    this->address_low = add_low;
}

void cacheline::setAddressHigh(unsigned long long add_high){
    this->address_high = add_high;
}

unsigned long long cacheline::getAddressLow(){
    return this->address_low;
}

unsigned long long cacheline::getAddressHigh(){
    return this->address_high;
}

void cacheline::setTag(int in_tag){
    this->tag = in_tag;
}

int cacheline::getTag(){
    return this->tag;
}

void cacheline::setBlockOffset(int b_offset){
    this->block_offest = b_offset;
}

int cacheline::getBlockOffset(){
    return this->block_offest;
}

void cacheline::setDataStructure(std::string d_name){
    this->data_structure = d_name;
}

std::string cacheline::getDataStructure(){
    return this->data_structure;
}
