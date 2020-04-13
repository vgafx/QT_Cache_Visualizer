/*Class that implements the UI cacheline objects*/

#include "cacheline.h"

/*Constructor*/
Cacheline::Cacheline(const QColor &col, int x, int y, int setID, StatusController *in)
    : m_color(col), m_scene_x(x), m_scene_y(y), m_set_idx(setID), sts(in)
{
    m_age = 0;
    m_address_low=0;
    m_address_high=0;
    m_idx_low = 0;
    m_idx_high = 0;
    m_data_structure = 'D';
    m_tag = 0;
    m_is_empty = true;
    m_block_offset = 0;

    this->flipAllSectors(false);
    setZValue((x + y) % 2);

    setFlags(ItemIsSelectable);
    setAcceptHoverEvents(true);
}

/*Sizes of the visual representation
 * The drawing function depends on these
 */
QRectF Cacheline::boundingRect() const
{
    return QRectF(0, 0, 440, 70);
}

QPainterPath Cacheline::shape() const
{
    QPainterPath path;
    path.addRect(14, 14, 328, 42);
    return path;
}


void Cacheline::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget);

    QColor fillColor = (option->state & QStyle::State_Selected) ? m_color.darker(150) : m_color;
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
        painter->drawText(170, 240, QString("Cache Line at %1x%2").arg(m_scene_x).arg(m_scene_y));
        painter->drawText(170, 340, QString("Set Index: %1").arg(m_set_idx));
        painter->restore();
    }

    // Display cache line data
    if (lod >= 2) {
        QFont font("Times", 76);
        font.setStyleStrategy(QFont::ForceOutline);
        painter->setFont(font);
        painter->save();
        painter->scale(0.1, 0.1);
        painter->drawText(170, 240, QString("Cache Line at %1x%2").arg(m_scene_x).arg(m_scene_y));
        painter->drawText(170, 340, QString("Set Index: %1").arg(m_set_idx));
        painter->restore();
    }
}

void Cacheline::setColor(const QColor &color)
{
    m_color = color;
}


void Cacheline::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::RightButton){
        QGraphicsItem::mousePressEvent(event);
        QString tltip = QString("Set Index:  %1 \nLine Coordinates:  %2 x %3 \nAddress Range:  %4 - %5 "
                                "\nData Structure:  %6\nIndex Range: %7 - %8 \nSectors Filled:  %9 - %10 - %11 - %10\nTag:  %12 \nAge:  %13")
                .arg(m_set_idx).arg(m_scene_x).arg(m_scene_y).arg(m_address_low).arg(m_address_high)
                .arg(m_data_structure).arg(m_idx_low).arg(m_idx_high).arg(m_s0_filled).arg(m_s1_filled).arg(m_s2_filled).arg(m_s3_filled).arg(m_tag).arg(m_age);
        event->accept();
        QPoint t_pos = event->screenPos();
        QToolTip::showText(t_pos,tltip);
    } else if (event->button() == Qt::LeftButton){
        QGraphicsItem::mousePressEvent(event);
        QString sts_txt = QString("Set Index:  %1, \nAddres Range:  %2 - %3, \nData Structure:  %4, \nIndex Range: %5 - %6, \nSectors Filled:  %7 - %8 - %9 - %10, \nTag:  %11, \nAge:  %12")
                .arg(m_set_idx).arg(m_address_low).arg(m_address_high).arg(m_data_structure).arg(m_idx_low).arg(m_idx_high).arg(m_s0_filled).arg(m_s1_filled).arg(m_s2_filled).arg(m_s3_filled).arg(m_tag).arg(m_age);
        event->accept();
        sts->setStatusText(sts_txt);
    }
    update();
}


/*UI Object visual updaters*/
void Cacheline::displayEviction()
{
    QColor evict_color(Qt::red);
    this->setColor(evict_color);
    this->update();
    QColor fresh_line_color(Qt::green);
    this->setColor(fresh_line_color);
    this->update();
}

void Cacheline::displayFullHit()
{
    QColor color(Qt::darkGreen);
    this->setColor(color);
    this->update();
}

void Cacheline::displayPartialHit()
{
    QColor part_color(Qt::yellow);
    this->setColor(part_color);
    this->update();
    QColor after_color(Qt::darkGreen);
    this->setColor(after_color);
    this->update();
}


void Cacheline::resetColor()
{
    QColor reset_color(Qt::gray);
    this->setColor(reset_color);
    this->update();
}


int Cacheline::getSceneX() const
{
    return m_scene_x;
}

void Cacheline::setSceneX(int scene_x)
{
    m_scene_x = scene_x;
}

int Cacheline::getSceneY() const
{
    return m_scene_y;
}

void Cacheline::setSceneY(int scene_y)
{
    m_scene_y = scene_y;
}

unsigned int Cacheline::getIdxLow() const
{
    return m_idx_low;
}

void Cacheline::setIdxLow(unsigned int idx_low)
{
    m_idx_low = idx_low;
}

unsigned int Cacheline::getIdxHigh() const
{
    return m_idx_high;
}

void Cacheline::setIdxHigh(unsigned int idx_high)
{
    m_idx_high = idx_high;
}

int Cacheline::getSetIdx() const
{
    return m_set_idx;
}

void Cacheline::setSetIdx(int set_idx)
{
    m_set_idx = set_idx;
}

int Cacheline::getAge() const
{
    return m_age;
}

void Cacheline::setAge(int age)
{
    m_age = age;
}

int Cacheline::getTag() const
{
    return m_tag;
}

void Cacheline::setTag(int in_tag)
{
    m_tag = in_tag;
}

int Cacheline::getBlockOffset() const
{
    return m_block_offset;
}

void Cacheline::setBlockOffset(int b_offset)
{
    m_block_offset = b_offset;
}

unsigned long long Cacheline::getAddressLow() const
{
    return m_address_low;
}

void Cacheline::setAddressLow(unsigned long long address_low)
{
    m_address_low = address_low;
}

unsigned long long Cacheline::getAddressHigh() const
{
    return m_address_high;
}

void Cacheline::setAddressHigh(unsigned long long address_high)
{
    m_address_high = address_high;
}

unsigned long long Cacheline::getTimeAccessed() const
{
    return  m_time_accessed;
}

void Cacheline::setTimeAccessed(unsigned long long time_accessed)
{
    m_time_accessed = time_accessed;
}

char Cacheline::getDataStructure() const
{
    return m_data_structure;
}

void Cacheline::setDataStructure(char data_structure)
{
    m_data_structure = data_structure;
}

bool Cacheline::getS0Filled() const
{
    return m_s0_filled;
}

void Cacheline::setS0Filled(bool s0_filled)
{
    m_s0_filled = s0_filled;
}

bool Cacheline::getS1Filled() const
{
    return m_s1_filled;
}

void Cacheline::setS1Filled(bool s1_filled)
{
    m_s1_filled = s1_filled;
}

bool Cacheline::getS2Filled() const
{
    return m_s2_filled;
}

void Cacheline::setS2Filled(bool s2_filled)
{
    m_s2_filled = s2_filled;
}

bool Cacheline::getS3Filled() const
{
    return m_s3_filled;
}

void Cacheline::setS3Filled(bool s3_filled)
{
    m_s3_filled = s3_filled;
}

bool Cacheline::getIsEmpty() const
{
    return m_is_empty;
}

void Cacheline::setIsEmpty(bool is_empty)
{
    m_is_empty = is_empty;
}


/*Custom functions*/
void Cacheline::incAge(){
    m_age++;
}

void Cacheline:: activateSector(int secID){
    switch (secID){
    case 0:
        m_s0_filled = true;
        break;
    case 1:
        m_s1_filled = true;
        break;
    case 2:
        m_s2_filled = true;
        break;
    case 3:
        m_s3_filled = true;
        break;
    }
}

void Cacheline:: invalidateSector(int secID){
    switch (secID){
    case 1:
        m_s0_filled = false;
        break;
    case 2:
        m_s1_filled = false;
        break;
    case 3:
        m_s2_filled = false;
        break;
    case 4:
        m_s3_filled = false;
        break;
    }
}

void Cacheline::flipAllSectors(bool on_off){
    if(on_off){
        m_s0_filled = true;
        m_s1_filled = true;
        m_s2_filled = true;
        m_s3_filled = true;
    } else {
        m_s0_filled = false;
        m_s1_filled = false;
        m_s2_filled = false;
        m_s3_filled = false;
    }
}
