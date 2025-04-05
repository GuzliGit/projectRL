#include "createproj_dialog.h"
#include "ui_createproj_dialog.h"
#include <QMessageBox>
#include <QSizePolicy>

CreateProj_Dialog::CreateProj_Dialog(int block_limit, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::CreateProj_Dialog)
{
    ui->setupUi(this);

    width = -1;
    height = -1;
    limit = block_limit;

    set_parameters();
}

CreateProj_Dialog::~CreateProj_Dialog()
{
    delete ui;
}

void CreateProj_Dialog::set_parameters()
{
    connect(ui->width_edit, &QLineEdit::textChanged, this, &CreateProj_Dialog::on_textChanged);
    connect(ui->height_edit, &QLineEdit::textChanged, this, &CreateProj_Dialog::on_textChanged);
}

int CreateProj_Dialog::get_width()
{
    return ui->width_edit->text().toInt();
}

int CreateProj_Dialog::get_height()
{
    return ui->height_edit->text().toInt();
}

bool CreateProj_Dialog::validate_accept()
{
    bool isint1, isint2;
    int val1 = ui->width_edit->text().toInt(&isint1);
    int val2 = ui->height_edit->text().toInt(&isint2);

    if (!isint1 || !isint2)
    {
        QMessageBox::warning(this, "Ошибка", "Числа должны быть целыми и неотрицательными!");
        return false;
    }

    if (val1 > limit || val2 > limit)
    {
        QMessageBox::warning(this, "Ошибка", "Числа не должны превышать " + QString::number(limit) + "!");
        return false;
    }

    return true;
}

void CreateProj_Dialog::on_decline_button_clicked()
{
    this->reject();
}


void CreateProj_Dialog::on_accept_button_clicked()
{
    if (validate_accept())
        this->accept();
}

void CreateProj_Dialog::on_textChanged()
{
    int temp = limit;
    int len_limit = 0;
    while (temp > 0) {
        len_limit++;
        temp /= 10;
    }

    QLineEdit *edit = qobject_cast<QLineEdit*>(sender());
    if (edit->text().length() > len_limit) {
        QString text = edit->text();
        text.chop(text.length() - len_limit);
        edit->setText(text);

        edit->setCursorPosition(text.length());
    }
}
