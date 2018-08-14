#include "box.h"
#include <QPainter>
#include <QKeyEvent>
#include <QTimer>

OneBox::OneBox(const QColor &color) : brushColor(color) {

}

QRectF OneBox::boundingRect() const {

    qreal pen_width = 1;

    return QRectF(-10-pen_width/2, -10-pen_width/2, 20+pen_width, 20+pen_width);

}

void OneBox::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){
    painter->drawPixmap(-10, -10, 20, 20, QPixmap(":/images/box.gif"));
    painter->setBrush(brushColor);
    painter->drawRect(-10, -10, 20, 20);
}

QPainterPath OneBox::shape() const{
    QPainterPath path;
    path.addRect(-9.5, -9.5, 19, 19);
    return path;
}

BoxGroup::BoxGroup() {
    setFlags(QGraphicsItem::ItemIsFocusable);
    old_transform = transform();
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(move_one_step()));
    current_shape = RandomShape;
}


QRectF BoxGroup::boundingRect() const {
    qreal pen_width = 1;
    return QRectF(-40-pen_width/2, -40-pen_width/2, 80+pen_width, 80+pen_width);
}


void BoxGroup::keyPressEvent(QKeyEvent *event) {

    switch(event->key())
    {
        case Qt::Key_Space:
            moveBy(0, 20);
            while(!isColliding()) {
                moveBy(0, 20);
            }
            moveBy(0, -20);
            clear_box_group();
            emit need_new_box();
            break;
        case Qt::Key_A:
        case Qt::Key_Left:
            moveBy(-20, 0);
            if(isColliding()) {
                moveBy(20, 0);
            }
            break;
        case Qt::Key_D:
        case Qt::Key_Right:
            moveBy(20, 0);
            if(isColliding()) {
                moveBy(-20, 0);
            }
            break;
         case Qt::Key_W:
         case Qt::Key_Up:
            rotate(90);
            if(isColliding())
                rotate(-90);
            break;
         case Qt::Key_S:
         case Qt::Key_Down:
            moveBy(0,20);
            if(isColliding()){
               moveBy(0,-20);
               clear_box_group();
               emit need_new_box();
            }
            break;
    }
}

bool BoxGroup::isColliding() {

    QList<QGraphicsItem *> item_list = childItems();
    QGraphicsItem *item;
    foreach(item, item_list) {
        if(item->collidingItems().count()>1)
            return true;
    }
    return false;
}

void BoxGroup::clear_box_group(bool destroy_box) {

    QList<QGraphicsItem *> item_list = childItems();
    QGraphicsItem *item;
    foreach(item, item_list) {
        removeFromGroup(item);
        if(destroy_box) {
            OneBox *box = (OneBox*)item;
            box->deleteLater();
        }
    }
}

void BoxGroup::create_box(const QPointF &point, BoxShape shape) {

    static const QColor color_table[7] = {
        QColor(0,255,255,100), QColor(226,236,0,100), QColor(255,155,115,100),
        QColor(0,255,13,100), QColor(255,115,126,100), QColor(251,185,199,100),
        QColor(251,185,199,100)
    };
    int shape_id = shape;
    if(shape == RandomShape) {
        shape_id = qrand()%7;
    }
    QColor color = color_table[shape_id];
    QList<OneBox *> list;
    setTransform(old_transform);
    for(int i = 0; i < 4; ++i) {
        OneBox *temp  = new OneBox(color);
        list << temp;
        addToGroup(temp);
    }
    switch(shape_id) {
        case IShape:
        current_shape = IShape;
        list.at(0)->setPos(-30, -10);
        list.at(1)->setPos(-10, -10);
        list.at(2)->setPos(10, -10);
        list.at(3)->setPos(30, -10);
        break;
        case JShape:
        current_shape = JShape;
        list.at(0)->setPos(10, -10);
        list.at(1)->setPos(10, 10);
        list.at(2)->setPos(10, 30);
        list.at(3)->setPos(-10, 30);
        break;
        case LShape:
        current_shape = LShape;
        list.at(0)->setPos(-10, -10);
        list.at(1)->setPos(-10, 10);
        list.at(2)->setPos(-10, 30);
        list.at(3)->setPos(10, 30);
        break;
        case OShape:
        current_shape = OShape;
        list.at(0)->setPos(-10, -10);
        list.at(1)->setPos(10, -10);
        list.at(2)->setPos(-10, 10);
        list.at(3)->setPos(10, 10);
        break;
        case SShape:
        current_shape = SShape;
        list.at(0)->setPos(10, -10);
        list.at(1)->setPos(30, -10);
        list.at(2)->setPos(-10, 10);
        list.at(3)->setPos(10, 10);
        break;
        case TShape:
        current_shape = TShape;
        list.at(0)->setPos(-10, -10);
        list.at(1)->setPos(10, -10);
        list.at(2)->setPos(30, -10);
        list.at(3)->setPos(10, 10);
        break;
        case ZShape:
        current_shape = ZShape;
        list.at(0)->setPos(-10, -10);
        list.at(1)->setPos(10, -10);
        list.at(2)->setPos(10, 10);
        list.at(3)->setPos(30, 10);
        break;
        default: break;
    }
    setPos(point);
    if(isColliding()) {

        stop_timer();
        emit game_finished();
    }
}

void BoxGroup::startTimer(int interval) {
    timer->start(interval);
}

void BoxGroup::move_one_step() {
    moveBy(0, 20);
    if(isColliding()) {
        moveBy(0, -20);
        clear_box_group();
        emit need_new_box();
    }
}

void BoxGroup::stop_timer() {
    timer->stop();
}


