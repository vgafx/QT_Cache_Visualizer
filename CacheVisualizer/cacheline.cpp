/*Class that implements the UI cacheline objects*/
#include <QtWidgets>
#include <QToolTip>
#include "view.h"
#include "cacheline.h"

/*Constructor*/
cacheline::cacheline(const QColor &color, int x, int y, int setID, statusController *in)
{
    this->x = x;
    this->y = y;
    this->color = color;
    this->set_idx = setID;
    this->age = 0;
    this->address_low=0;
    this->address_high=0;
    this->idx_low = 0;
    this->idx_high = 0;
    this->setDataStructure("DataAr");
    this->tag = 0;
    this->is_empty = true;
    this->block_offest = 0;
    this->sts = in;
    this->flipAllSectors(0);
    setZValue((x + y) % 2);

    setFlags(ItemIsSelectable);
    setAcceptHoverEvents(true);
}

/*Sizes of the visual representation
 * The drawing function depends on these
 */
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
            painter->fillRect(QRectF(0, 0, 340, 50), fillColor);
            return;
        }

        QBrush b = painter->brush();
        painter->setBrush(fillColor);
        painter->drawRect(13, 13, 288, 39);
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

    //Display cache line info if zoomed in.
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
}


void cacheline::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::RightButton){
        QGraphicsItem::mousePressEvent(event);
        QString tltip = QString("Set Index:  %1 \nLine Coordinates:  %2 x %3 \nAddress Range:  %4 - %5 "
                                "\nData Structure:  %6\nIndex Range: %7 - %8 \nSectors Filled:  %9 - %10 - %11 - %10\nTag:  %12 \nAge:  %13")
                .arg(this->set_idx).arg(this->x).arg(this->y).arg(this->address_low).arg(this->address_high)
                .arg(QString::fromStdString(this->data_structure)).arg(this->idx_low).arg(this->idx_high).arg(this->sector_one_filled).arg(this->sector_two_filled).arg(this->sector_three_filled).arg(this->sector_four_filled).arg(this->tag).arg(this->age);
        event->accept();
        QPoint t_pos = event->screenPos();
        QToolTip::showText(t_pos,tltip);
    } else if (event->button() == Qt::LeftButton){
        QGraphicsItem::mousePressEvent(event);
        QString sts_txt = QString("Set Index:  %1, \nAddres Range:  %2 - %3, \nData Structure:  %4, \nIndex Range: %5 - %6, \nSectors Filled:  %7 - %8 - %9 - %10, \nTag:  %11, \nAge:  %12")
                .arg(this->set_idx).arg(this->address_low).arg(this->address_high).arg(QString::fromStdString(this->data_structure)).arg(this->idx_low).arg(this->idx_high).arg(this->sector_one_filled).arg(this->sector_two_filled).arg(this->sector_three_filled).arg(this->sector_four_filled).arg(this->tag).arg(this->age);
        event->accept();
        sts->setStatusText(sts_txt);
    }
    update();
}

long long cacheline::getCycles() const
{
    return cycles;
}

void cacheline::setCycles(long long value)
{
    cycles = value;
}


/*UI Object visual updaters*/
void cacheline::displayEviction(){
    QColor evict_color(Qt::red);
    this->setColor(evict_color);
    this->update();
    QColor fresh_line_color(Qt::green);
    this->setColor(fresh_line_color);
    this->update();
}

void cacheline::displayFullHit(){
    QColor color(Qt::darkGreen);
    this->setColor(color);
    this->update();
}

void cacheline::displayPartialHit(){
    QColor part_color(Qt::yellow);
    this->setColor(part_color);
    this->update();
    QColor after_color(Qt::darkGreen);
    this->setColor(after_color);
    this->update();
}

/*Custom functions*/
void cacheline::incAge(){
    this->age++;
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
        this->sector_two_filled = 1;
        this->sector_three_filled = 1;
        this->sector_four_filled = 1;
    }
}


/*Setters & getters for the cacheline object*/
long long cacheline::getIdxHigh() const
{
    return idx_high;
}

void cacheline::setIdxHigh(long long value)
{
    idx_high = value;
}

long long cacheline::getIdxLow() const
{
    return idx_low;
}

void cacheline::setIdxLow(long long value)
{
    idx_low = value;
}

bool cacheline::getIs_empty() const
{
    return is_empty;
}

void cacheline::setIs_empty(bool value)
{
    is_empty = value;
}

int cacheline::getAge()const{
    return this->age;
}

void cacheline::setAge(int inAge){
    this->age = inAge;
}

void cacheline::setAddressLow(long long add_low){
    this->address_low = add_low;
}

void cacheline::setAddressHigh(long long add_high){
    this->address_high = add_high;
}

long long cacheline::getAddressLow()const{
    return this->address_low;
}

long long cacheline::getAddressHigh()const{
    return this->address_high;
}

void cacheline::setTag(int in_tag){
    this->tag = in_tag;
}

int cacheline::getTag()const{
    return this->tag;
}

void cacheline::setBlockOffset(int b_offset){
    this->block_offest = b_offset;
}

int cacheline::getBlockOffset()const{
    return this->block_offest;
}

void cacheline::setDataStructure(std::string d_name){
    this->data_structure = d_name;
}

std::string cacheline::getDataStructure()const{
    return this->data_structure;
}

void cacheline::setColor(const QColor &color){
    this->color = color;
}

bool cacheline::getSector_four_filled() const
{
    return sector_four_filled;
}

void cacheline::setSector_four_filled(bool value)
{
    sector_four_filled = value;
}

bool cacheline::getSector_three_filled() const
{
    return sector_three_filled;
}

void cacheline::setSector_three_filled(bool value)
{
    sector_three_filled = value;
}

bool cacheline::getSector_two_filled() const
{
    return sector_two_filled;
}

void cacheline::setSector_two_filled(bool value)
{
    sector_two_filled = value;
}

bool cacheline::getSector_one_filled() const
{
    return sector_one_filled;
}

void cacheline::setSector_one_filled(bool value)
{
    sector_one_filled = value;
}
