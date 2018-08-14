#include "myview.h"
#include "box.h"
#include <QIcon>
#include <QPropertyAnimation>
#include <QGraphicsBlurEffect>
#include <QTimer>
#include <QPushButton>
#include <QGraphicsProxyWidget>
#include <QApplication>
#include <QLabel>
#include <QFileInfo>

static const qreal INITSSPEED = 500;
static const QString SOUNDPATH = "./sounds/";

MyView::MyView(QWidget *parent) :
    QGraphicsView(parent)
{
    init_view();
}

void MyView::init_view() {

    setRenderHint(QPainter::Antialiasing);
    setCacheMode(CacheBackground);
    setWindowTitle(tr("俄罗斯方块"));
    setWindowIcon(QIcon(":/images/icon.png"));
    setMinimumSize(810, 510);
    setMaximumSize(810, 510);
    QGraphicsScene *scene = new QGraphicsScene;
    scene->setSceneRect(5, 5, 800, 500);
    scene->setBackgroundBrush(QPixmap(":/images/background.png"));
    setScene(scene);

    top_line = scene->addLine(197, 47, 403, 47);
    bottom_line = scene->addLine(197, 453, 403, 453);
    left_line = scene->addLine(197, 47, 197, 453);
    right_line = scene->addLine(403, 47, 403, 453);

    box_group = new BoxGroup;
    connect(box_group, SIGNAL(need_new_box()), this, SLOT(clear_full_rows()));
    connect(box_group, SIGNAL(game_finished()), this, SLOT(game_over()));
    scene->addItem(box_group);

    next_box_group = new BoxGroup;
    scene->addItem(next_box_group);

    game_score = new QGraphicsTextItem(0, scene);
    game_score->setFont(QFont("Times", 50, QFont::Bold));
    game_score->setPos(600, 350);
    game_score_text = new QGraphicsTextItem(0, scene);
    game_score_text->setFont(QFont("Times", 50, QFont::Bold));
    game_score_text->setPos(450, 350);

    game_level = new QGraphicsTextItem(0, scene);
    game_level->setFont(QFont("Times", 50, QFont::Bold));
    game_level->setPos(20, 150);

    top_line->hide();
    bottom_line->hide();
    left_line->hide();
    right_line->hide();
    game_score->hide();
    game_level->hide();

    QWidget *mask = new QWidget;
    mask->setAutoFillBackground(true);
    mask->setPalette(QPalette(QColor(0, 0, 0, 50)));
    mask->resize(900, 600);

    mask_widget = scene->addWidget(mask);
    mask_widget->setPos(-50, -50);
    mask_widget->setZValue(1);

    QWidget *option = new QWidget;

    QPushButton *option_close_button = new QPushButton(tr("返 回"), option);
    option_close_button->setShortcut(QKeySequence::InsertParagraphSeparator);

    QPalette palette;
    palette.setColor(QPalette::ButtonText, Qt::black);
    option_close_button->setPalette(palette);

    option_close_button->move(370, 320);
    option_close_button->setStyleSheet("background-color:#ffffff");

    connect(option_close_button, SIGNAL(clicked()), option, SLOT(hide()));

    option->setAutoFillBackground(true);
    option->setPalette(QPalette(QColor(0, 0, 0, 180)));
    option->resize(850, 550);
    QGraphicsWidget *option_widget = scene->addWidget(option);
    option_widget->setPos(0, 0);
    option_widget->setZValue(3);
    option_widget->hide();

    QLabel *maker_label = new QLabel(tr("制作者：赵  鹏\n              崔书涵"),this);
    maker_label->setParent(this);
    maker_label->resize(150,80);
    maker_label->show();
    maker_label->setFont(QFont("Times",15,QFont::Light));

    maker_label->setPalette(palette);
    maker_label->move(600,420);

    QWidget *help = new QWidget;
    QLabel *help_title = new QLabel(tr("帮 助"),this);
    help_label = new QLabel(tr(" \n\n左右键左右移动\n\n  向上键旋转\n\n  向下键加速\n\n  空格键坠落"),this);
    help_label->setParent(help);
    help_title->setParent(help);
    help_title->show();
    help_title->setStyleSheet("color:green");
    help_title->setPalette(palette);
    help_title->move(360,10);
    help_title->setFont(QFont("Times",30));
    help_title->resize(150,150);
    help_label->show();
    help_label->resize(200,230);
    help_label->setFont(QFont("Times", 15, QFont::DemiBold));

    help_label->setStyleSheet("color:white");
    help_label->setPalette(palette);
    help_label->move(343,100);

    QPushButton *help_close_button = new QPushButton(tr("返 回"), help);
    help_close_button->setShortcut(QKeySequence::InsertParagraphSeparator);
    help_close_button->setStyleSheet("background-color:#ffffff");
    help_close_button->setPalette(palette);
    help_close_button->move(370, 350);
    connect(help_close_button, SIGNAL(clicked()), help, SLOT(hide()));

    help->setAutoFillBackground(true);
    help->setPalette(QPalette(QColor(0, 0, 0, 180)));
    help->resize(850, 550);
    QGraphicsWidget *help_widget = scene->addWidget(help);
    help_widget->setPos(0, 0);
    help_widget->setZValue(3);
    help_widget->hide();

    game_welcome_text = new QGraphicsTextItem(0, scene);
    game_welcome_text->setHtml(tr("<font face=times color = blue>俄罗斯方块</font>"));
    game_welcome_text->setFont(QFont("Times", 40, QFont::Bold));
    game_welcome_text->setPos(270, 100);
    game_welcome_text->setZValue(2);

    game_pause_text = new QGraphicsTextItem(0, scene);
    game_pause_text->setHtml(tr("<font color=red>游戏暂停中！</font>"));
    game_pause_text->setFont(QFont("Times", 40, QFont::Bold));
    game_pause_text->setPos(260, 100);
    game_pause_text->setZValue(2);
    game_pause_text->hide();

    game_help_text = new QGraphicsTextItem(0,scene);
    game_help_text->setHtml(tr("<font color=green>左右键控制方向\n向下键坠落\n空格旋转</font>"));
    game_help_text->setFont(QFont("Times", 20, QFont::Bold));
    game_help_text->setPos(300, 500);
    game_help_text->setZValue(2);

    game_over_text = new QGraphicsTextItem(0, scene);
    game_over_text->setHtml(tr("<font color=RED>GAME　OVER！</font>"));
    game_over_text->setFont(QFont("Times", 40, QFont::Bold));
    game_over_text->setPos(230, 100);
    game_over_text->setZValue(2);
    game_over_text->hide();

    QPushButton *button1 = new QPushButton(tr("开   始(S)"));
    button1->setShortcut(Qt::Key_S);
    QPushButton *button2 = new QPushButton(tr("选   项(O)"));
    button2->setShortcut(Qt::Key_O);
    QPushButton *button3 = new QPushButton(tr("帮   助(H)"));
    button3->setShortcut(Qt::Key_H);
    QPushButton *button4 = new QPushButton(tr("退   出(Q)"));
    button4->setShortcut(Qt::Key_Q);
    QPushButton *button5 = new QPushButton(tr("重新开始(R)"));
    button5->setShortcut(Qt::Key_R);
    QPushButton *button6 = new QPushButton(tr("暂   停(P)"));
    button6->setShortcut(Qt::Key_P);
    QPushButton *button7 = new QPushButton(tr("主 菜 单(M)"));
    button7->setShortcut(Qt::Key_M);
    QPushButton *button8 = new QPushButton(tr("返回游戏┛"));
    button8->setShortcut(QKeySequence::InsertParagraphSeparator);
    QPushButton *button9 = new QPushButton(tr("结束游戏(F)"));
    button9->setShortcut(Qt::Key_F);

    connect(button1, SIGNAL(clicked()), this, SLOT(start_game()));
    connect(button2, SIGNAL(clicked()), option, SLOT(show()));
    connect(button3, SIGNAL(clicked()), help, SLOT(show()));
    connect(button4, SIGNAL(clicked()), qApp, SLOT(quit()));
    connect(button5, SIGNAL(clicked()), this, SLOT(restart_game()));
    connect(button6, SIGNAL(clicked()), this, SLOT(pause_game()));
    connect(button7, SIGNAL(clicked()), this, SLOT(finish_game()));
    connect(button8, SIGNAL(clicked()), this, SLOT(return_game()));
    connect(button9, SIGNAL(clicked()), this, SLOT(finish_game()));

    start_button = scene->addWidget(button1);
    option_button = scene->addWidget(button2);
    help_button = scene->addWidget(button3);
    exit_button = scene->addWidget(button4);
    restart_button = scene->addWidget(button5);
    pause_button = scene->addWidget(button6);
    show_menu_button = scene->addWidget(button7);
    return_button = scene->addWidget(button8);
    finish_button = scene->addWidget(button9);

    start_button->setPos(370, 210);
    option_button->setPos(370, 270);
    help_button->setPos(370, 330);
    exit_button->setPos(370, 390);
    restart_button->setPos(600, 150);
    pause_button->setPos(600, 200);
    show_menu_button->setPos(600, 250);
    return_button->setPos(370, 200);
    finish_button->setPos(370, 250);


    start_button->setZValue(2);
    option_button->setZValue(2);
    help_button->setZValue(2);
    exit_button->setZValue(2);
    restart_button->setZValue(2);
    return_button->setZValue(2);
    finish_button->setZValue(2);

    restart_button->hide();
    finish_button->hide();
    pause_button->hide();
    show_menu_button->hide();
    return_button->hide();

    background_music = new Phonon::MediaObject(this);
    clearrow_sound = new Phonon::MediaObject(this);

    Phonon::AudioOutput *audio1 = new Phonon::AudioOutput(Phonon::MusicCategory, this);
    Phonon::AudioOutput *audio2 = new Phonon::AudioOutput(Phonon::MusicCategory, this);
    Phonon::createPath(background_music, audio1);//
    Phonon::createPath(clearrow_sound, audio2);

    Phonon::VolumeSlider *volume1 = new Phonon::VolumeSlider(audio1, option);
    volume1->setStyleSheet("color:#eeeeee");
    Phonon::VolumeSlider *volume2 = new Phonon::VolumeSlider(audio2, option);
    volume2->setStyleSheet("color:#eeeeee");
    QLabel *volume_label1 = new QLabel(tr("音乐声："), option);
    QLabel *volume_label2 = new QLabel(tr("音效声："), option);
    volume1->move(365, 140);
    volume2->move(365, 220);
    volume_label1->move(330, 145);
    volume_label2->move(330, 225);

    connect(background_music, SIGNAL(aboutToFinish()), this, SLOT(about_to_finish()));
    connect(clearrow_sound, SIGNAL(finished()), clearrow_sound, SLOT(stop()));
    background_music->setCurrentSource(Phonon::MediaSource(SOUNDPATH + "background.mp3"));
    clearrow_sound->setCurrentSource(Phonon::MediaSource(SOUNDPATH + "clearrow.mp3"));
    background_music->play();
}



void MyView::start_game() {
    game_welcome_text->hide();
    start_button->hide();
    option_button->hide();
    help_button->hide();
    exit_button->hide();
    mask_widget->hide();
    init_game();
}



void MyView::init_game() {
    box_group->create_box(QPointF(300, 70));
    box_group->setFocus();
    box_group->startTimer(INITSSPEED);
    game_speed = INITSSPEED;
    next_box_group->create_box(QPoint(500, 70));

    scene()->setBackgroundBrush(QPixmap(":/images/background01.png"));
    game_score_text->setHtml(tr("<font face=times size=1 color = white><b>scores:<b></font>"));
    game_score->setHtml(tr("<font face=times size=1 color = white>0</font>"));
    game_level->setHtml(tr("<font color = white size=1>第<br>一<br>关</font>"));

    restart_button->show();
    pause_button->show();
    show_menu_button->show();
    game_score->show();
    game_level->show();
    top_line->show();
    bottom_line->show();
    left_line->show();
    right_line->show();

    help_label->show();
    box_group->show();

    background_music->setCurrentSource(Phonon::MediaSource(SOUNDPATH + "background01.mp3"));
    background_music->play();

}

void MyView::clear_full_rows() {

    for(int y = 429; y > 50; y -= 20) {
        QList<QGraphicsItem *> list = scene()->items(199, y, 202, 22, Qt::ContainsItemShape);
        if(list.count() == 10) {
            foreach(QGraphicsItem *item, list) {
                OneBox *box = (OneBox *) item;
                QGraphicsBlurEffect *blur_effect = new QGraphicsBlurEffect;
                box->setGraphicsEffect(blur_effect);
                QPropertyAnimation *animation = new QPropertyAnimation(box, "scale");
                animation->setEasingCurve(QEasingCurve::OutBounce);
                animation->setDuration(250);
                animation->setStartValue(4);
                animation->setEndValue(0.25);
                animation->start(QAbstractAnimation::DeleteWhenStopped);
                connect(animation, SIGNAL(finished()), box, SLOT(deleteLater()));
            }
            rows << y;
        }
    }

    if(rows.count()>0) {
        clearrow_sound->play();
        QTimer::singleShot(400, this, SLOT(move_box()));
    //    clearrow_sound->play();
    }
    else {

        box_group->create_box(QPointF(300, 70), next_box_group->getCurrentShape());
        next_box_group->clear_box_group(true);
        next_box_group->create_box(QPointF(500, 70));//
    }
}

void MyView::move_box() {

    for(int i = rows.count(); i > 0; --i) {
        int row = rows.at(i-1);
        foreach(QGraphicsItem *item, scene()->items(199, 49, 202, row-47, Qt::ContainsItemShape)) {
            item->moveBy(0, 20);
        }
    }
    update_score(rows.count());
    rows.clear();
    box_group->create_box(QPointF(300, 70), next_box_group->getCurrentShape());
    next_box_group->clear_box_group(true);
    next_box_group->create_box(QPoint(500, 70));
}

void MyView::update_score(const int full_row_num) {

    long score = full_row_num*10;
    int current_score = game_score->toPlainText().toInt();
    current_score += score;
    game_score->setHtml(tr("<font face=times size=1 color = white>%1</font>").arg(current_score));
    if(current_score >= 10) {
        game_level->setHtml(tr("<font color = white size=1>第<br>二<br>关</font>"));
        scene()->setBackgroundBrush(QPixmap(":/images/background02.png"));
        game_speed = 300;
        box_group->stop_timer();
        box_group->startTimer(game_speed);
        if (QFileInfo(background_music->currentSource().fileName()).baseName() != "background02") {
             background_music->setCurrentSource(Phonon::MediaSource(SOUNDPATH + "background02.mp3"));
             background_music->play();
        }
    }
}

void MyView::game_over() {

    pause_button->hide();
    show_menu_button->hide();
    mask_widget->show();
    game_over_text->show();
    restart_button->setPos(370, 200);
    finish_button->show();
}

void MyView::restart_game()
{
    mask_widget->hide();
    game_over_text->hide();
    finish_button->hide();
    restart_button->setPos(600, 150);
    next_box_group->clear_box_group(true);
    box_group->clear_box_group();
    box_group->hide();
    foreach(QGraphicsItem *item, scene()->items(199, 49, 202, 402,Qt::ContainsItemBoundingRect)) {
        scene()->removeItem(item);
        OneBox *box = (OneBox*)item;
        box->deleteLater();
    }
    init_game();
}

void MyView::finish_game()
{
    game_over_text->hide();
    finish_button->hide();
    restart_button->setPos(600, 150);
    restart_button->hide();
    pause_button->hide();
    show_menu_button->hide();
    game_score->hide();
    game_level->hide();
    top_line->hide();
    bottom_line->hide();
    left_line->hide();
    right_line->hide();

    next_box_group->clear_box_group(true);
    box_group->clear_box_group();
    box_group->hide();
    foreach(QGraphicsItem *item, scene()->items(199, 49, 202, 402,Qt::ContainsItemBoundingRect)) {
        scene()->removeItem(item);
        OneBox *box = (OneBox*)item;
        box->deleteLater();
    }
    mask_widget->show();
    game_welcome_text->show();
    start_button->show();
    option_button->show();
    help_button->show();
    exit_button->show();
    scene()->setBackgroundBrush(QPixmap(":/images/background.png"));

    background_music->setCurrentSource(Phonon::MediaSource(SOUNDPATH + "background.mp3"));
    background_music->play();
}

void MyView::pause_game()
{
    box_group->stop_timer();
    restart_button->hide();
    pause_button->hide();
    show_menu_button->hide();
    mask_widget->show();
    game_pause_text->show();
    return_button->show();
}
void MyView::game_help()
{
    pause_button->hide();
    show_menu_button->hide();
    mask_widget->show();
    game_over_text->show();
    restart_button->setPos(370, 200);
    finish_button->show();
}

void MyView::return_game()
{
    return_button->hide();
    game_pause_text->hide();
    mask_widget->hide();
    restart_button->show();
    pause_button->show();
    show_menu_button->show();
    box_group->startTimer(game_speed);
}

void MyView::about_to_finish()
{
    background_music->enqueue(background_music->currentSource());
}

void MyView::keyPressEvent(QKeyEvent *event) {
    if(pause_button->isVisible())
        box_group->setFocus();
    else
        box_group->clearFocus();
    QGraphicsView::keyPressEvent(event);
}
