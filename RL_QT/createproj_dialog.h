#ifndef CREATEPROJ_DIALOG_H
#define CREATEPROJ_DIALOG_H

#include <QDialog>

namespace Ui {
class CreateProj_Dialog;
}

class CreateProj_Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit CreateProj_Dialog(int block_limit, QWidget *parent = nullptr);
    ~CreateProj_Dialog();
    int get_width();
    int get_height();

private slots:
    void on_decline_button_clicked();

    void on_accept_button_clicked();

    void on_text_changed();

private:
    int width;
    int height;
    int limit;

    Ui::CreateProj_Dialog *ui;
    void set_parameters();
    bool validate_accept();
};

#endif // CREATEPROJ_DIALOG_H
