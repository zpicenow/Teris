#ifndef MYVIEW_H
#define MYVIEW_H

#include <QGraphicsView>
#include <phonon>
#include <QLabel>

class BoxGroup;

class MyView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit MyView(QWidget *parent = 0);

public slots:
    void start_game();
    void clear_full_rows();
    void move_box();
    void game_over();
    void game_help();

    void restart_game();
    void finish_game();
    void pause_game();
    void return_game();
    void about_to_finish();

protected:
    void keyPressEvent(QKeyEvent *event);

private:
    qreal game_speed;
    QList<int> rows;
    QLabel *help_label;
    BoxGroup *box_group;
    BoxGroup *next_box_group;

    QGraphicsWidget *mask_widget;
    QGraphicsWidget *start_button;
    QGraphicsWidget *finish_button;
    QGraphicsWidget *restart_button;
    QGraphicsWidget *pause_button;
    QGraphicsWidget *option_button;
    QGraphicsWidget *return_button;
    QGraphicsWidget *help_button;
    QGraphicsWidget *exit_button;
    QGraphicsWidget *show_menu_button;

    QGraphicsLineItem *top_line;
    QGraphicsLineItem *bottom_line;
    QGraphicsLineItem *left_line;
    QGraphicsLineItem *right_line;
    QGraphicsTextItem *game_score;
    QGraphicsTextItem *game_score_text;
    QGraphicsTextItem *game_level;
    QGraphicsTextItem *game_welcome_text;
    QGraphicsTextItem *game_pause_text;
    QGraphicsTextItem *game_help_text;
    QGraphicsTextItem *game_over_text;

    Phonon::MediaObject *background_music;
    Phonon::MediaObject *clearrow_sound;

    void init_view();
    void init_game();
    void update_score(const int full_row_num = 0);
    
};

#endif // MYVIEW_H
